#include "TLSFAllocator.h"
#include "../MathBit/MathBit.hpp"
#include "AllocateInterface.h"

#include <new>
#include <cstdlib>

#include <cassert>
#include <cstring>

#define SLIB_TLSF_CONST_BIT_ONE			 0x00000001u //0b00000000000000000000000000000001
#define SLIB_TLSF_CONST_BIT_FIVE		 0x0000001Fu //0b00000000000000000000000000011111
#define SLIB_TLSF_CONST_BIT_32_ONBIT	 0xFFFFFFFFu //0b11111111111111111111111111111111
#define SLIB_TLSF_CONST_BIT_64_DW32ONBIT 0x00000000FFFFFFFFULL //0b000000000000000000000000000000011111111111111111111111111111111ULL
#define SLIB_TLSF_CONST_BIT_64_UP32ONBIT 0xFFFFFFFF00000000ULL //0b111111111111111111111111111111110000000000000000000000000000000ULL
#define SLIB_TLSF_CONST_BIT_64_ALLBITON  0xFFFFFFFFFFFFFFFFULL //0b1111111111111111111111111111111111111111111111111111111111111111ULL

namespace SonikLib
{
	//各種アクセス、設定用クラス(ポインタ解釈としてしか使わない。
	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	static constexpr uint32_t SLIB_TLSF_CATEGORY_BIT_CNT = 32;
	static constexpr uint32_t SLIB_TLSF_FREELIST_ADDRESS_BLOCK_CNT = (SLIB_TLSF_CATEGORY_BIT_CNT * SLIB_TLSF_CATEGORY_BIT_CNT); //FirstCategory分 * SecondCategory分

	// 静的チェック（サイズが極端に大きくなっていないか確認）
	static_assert(SLIB_TLSF_CATEGORY_BIT_CNT == 32u, "Category bit count must be 32");
	static_assert(SLIB_TLSF_FREELIST_ADDRESS_BLOCK_CNT == 1024u, "Free list address block count must be 1024");

	class SLIB_TLSF_MEMBLOCK
	{
	public:
		SLIB_TLSF_MEMBLOCK* front; //前方
		SLIB_TLSF_MEMBLOCK* back;
		uint32_t BlockSize;
		uint8_t IsUse;

	};
	static constexpr uint32_t SLIB_TLSF_LOWLIMIT_ALLOCATESIZE = sizeof(SLIB_TLSF_MEMBLOCK) + 32; //最低アロケートサイズ

	class SLIB_TLSF_FREELISTMEMBLOCK : public SLIB_TLSF_MEMBLOCK
	{
	public:
		SLIB_TLSF_FREELISTMEMBLOCK* free_next;

	};

	class SLIB_TLSF_BITCOLUMBLOCK
	{
	public:
		uint32_t  FirstCategoryBit;											  //ファーストカテゴリのビット列
		uint32_t  SecondCategoryBit[SLIB_TLSF_CATEGORY_BIT_CNT];			  //セカンドカテゴリのビット列
		uint32_t  dummy_is_pading;											  //パディング領域
		uintptr_t FreeAreaAddressBlock[SLIB_TLSF_FREELIST_ADDRESS_BLOCK_CNT]; //ビット列に対応した、「フリーエリアへのアドレスを管理するアドレスブロック」。最大1024アドレスを同時に管理できる。
	};

	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝


	//__________________________________________________________
	//															|
	//		重複する処理をinlineグローバル関数でまとめる		|
	//__________________________________________________________|
	namespace SLIB_TLSF_GLOBAL_FUNC
	{
		//マージ処理時に同じ処理が入るためグローバル関数に一つにまとめる。
		DEF_FORCE_INLINE void AdjustMemBlock(SonikLib::SLIB_TLSF_FREELISTMEMBLOCK* _target_, SLIB_TLSF_BITCOLUMBLOCK* _free_bit_) noexcept
		{
			uint32_t l_delsize_fcat = 0; //サイズに対する第１カテゴリを取得
			uint32_t l_fcat = 0;		 //実使用フリーリスト第１カテゴリ
			uint32_t l_scat = 0;		 //実使用フリーリスト第２カテゴリ
			uint32_t FreeBlockAddresIndex = 0;		//フリーリストインデックス
			uint32_t freelist_bit_update_mask = 0;	//マスクに使用


			//大元の第１カテゴリの取得
			l_delsize_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_target_->BlockSize));
			//フリーリスト対応の第１カテゴリを取得
			//mask = 0xFFFFFFFF << l_fcat;
			//mask &= lp_freelist_bit->FirstCategoryBit;
			//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
			//↑の1行書き。
			l_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(_free_bit_->FirstCategoryBit & (0xFFFFFFFFu << l_delsize_fcat)));

			//第２カテゴリを取得(ここで0が来ることはない。)
			//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
			l_scat = (l_delsize_fcat == l_fcat)
					 ? (_target_->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE						 //2^5 ということで MSB - 5 オフセット
					 : static_cast<uint32_t>(SonikMathBit::GetLSB(_free_bit_->SecondCategoryBit[l_fcat])); //上位のファーストカテゴリならどの場所でも入るので最小位置を取得

			 //位置特定できたのでフリーリスト調整処理
			FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_fcat) + l_scat;

			//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
			_free_bit_->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(_target_->free_next);

			//ビット演算でFreeListが0(nullptr)になっていないかチェック。
			//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
			freelist_bit_update_mask = (1u << l_scat) & -(_free_bit_->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0);
			_free_bit_->SecondCategoryBit[l_fcat] &= (~freelist_bit_update_mask);

			//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
			freelist_bit_update_mask = (1u << l_fcat) & -(_free_bit_->SecondCategoryBit[l_fcat] == 0);
			_free_bit_->FirstCategoryBit &= (~freelist_bit_update_mask);
		
			return;
		};
#if false
		//対象のfree_prev, free_nextのポインタをチェックし、以下の値を返却します。
		//0：prev, next 共にnullptr
		//1：prevが存在、nextがnullptr
		//2：prevがnullptr、nextが存在
		//3：prev, next 共に存在
		DEF_FORCE_INLINE int32_t Prev_Next_Pointer_Check(SonikLib::SLIB_TLSF_FREELISTMEMBLOCK* _target_) noexcept
		{
			//!!_target_->free_prev(or next) -> !でブール値(0 or 1)に変換。!のみだとnullptr = 1, それ以外 = 0 になるのでこの結果を更に !で反転させるために !! を使う。
			int32_t prev = (-(!!_target_->free_prev)) & S_CONST_PLUS_PREV; //反転した結果に - をつけることで　-1 * 反転結果になり、結果 0のとき -1 * 0 = 0, 結果 1のとき -1 * 1 = -1 になる。
			int32_t next = (-(!!_target_->free_prev)) & S_CONST_PLUS_NEXT;

			return (prev | next);
		};

		//フリーリストから対象を外します。
		DEF_FORCE_INLINE bool RemoveFreeList(SonikLib::SLIB_TLSF_FREELISTMEMBLOCK* _target_, SLIB_TLSF_BITCOLUMBLOCK* _freelist_bit_) noexcept
		{
			uint32_t l_switch_value = 0;

			//対象のfree_prev, free_nextの有無を確認。
			l_switch_value = Prev_Next_Pointer_Check(_target_);

			//後方オブジェクトのカテゴリ番号を取得
			//第１カテゴリ番号の取得
			int32_t l_fcat = SonikMathBit::GetMSB(_target_->BlockSize);
			//第２カテゴリ番号の取得
			int32_t l_scat = (_target_->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//switchで処理分岐
			switch (l_switch_value)
			{
			case 0: //free_prev, free_next 共にnullptr
				//フリーリスト最後なので外す。
				_freelist_bit_->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_fcat) + l_scat] = 0;
				//現地点の最後のフリーブロックだったのでフラグを下ろす。
				_freelist_bit_->SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//下ろした結果該当のセカンドビット値が0(すべてOFF)になった場合は対応するファーストカテゴリのビットも下ろす。
				if (_freelist_bit_->SecondCategoryBit[l_fcat] == 0)
				{
					_freelist_bit_->FirstCategoryBit &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				break;

			case 1: //free_prev のみ存在
				//フリーリスト最終ではないので外れるだけ
				_target_->free_prev->free_next = nullptr; //自分を指しているprevのnextポインタをnullptrへ。

				//該当ビット位置の最終要素ではないためビット操作はせず終了。
				break;

			case 2: //free_next のみ存在
				//そのビット位置のフリーリストの先頭なので、nextに先頭を譲り脱退。
				_freelist_bit_->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_fcat) + l_scat] = reinterpret_cast<uintptr_t>(_target_->free_next);
				_target_->free_next->free_prev = nullptr; //自分を指しているnextのprevポインタをnullptrへ。

				//該当ビット位置の最終要素ではないためビット操作はせず終了。
				break;

			case 3: //free_prev, free_next どちらも存在
				//間から抜けるためにポインタを付け替えるだけ。
				_target_->free_prev->free_next = _target_->free_next;
				_target_->free_next->free_prev = _target_->free_prev;

				//該当ビット位置の最終要素ではないためビット操作はせず終了。
				break;

			default:
				//エラー。ソースコード変えない限り0~3の範囲を超えることがないので不要かとは思うが...。
				return false;
			};

			return true;
		};
#endif
	};//end namespace SLIB_TLSF_GLOBAL_FUNC



	//クラス実装================================================================================
	TLSFAllocator::TLSFAllocator(void) noexcept
		:mp_main_memblock(nullptr)
		, mp_freelistblock(nullptr)
		, m_lock(nullptr)
		, mem_block_size(0)
	{
		//no process
	};

	TLSFAllocator::TLSFAllocator(const TLSFAllocator& _copy_) noexcept
		:mp_main_memblock(_copy_.mp_main_memblock)
		, mp_freelistblock(_copy_.mp_freelistblock)
		, m_lock(_copy_.m_lock)
		, mem_block_size(_copy_.mem_block_size)
	{
		this->m_enabled_state = _copy_.m_enabled_state;

	};

	//すべてリテラル/ポインタ型で、オブジェクト型はないためstd::move不要。
	//してもいいけど、std::move分のオーバーヘッドコストが発生するのでしなくていいならしないほうがいい。
	TLSFAllocator::TLSFAllocator(TLSFAllocator&& _move_) noexcept
		:mp_main_memblock(_move_.mp_main_memblock)
		, mp_freelistblock(_move_.mp_freelistblock)
		, m_lock(_move_.m_lock)
		, mem_block_size(_move_.mem_block_size)
	{
		this->m_enabled_state = _move_.m_enabled_state;

		_move_.mp_main_memblock = nullptr;
		_move_.mp_freelistblock = nullptr;
		_move_.m_enabled_state = SLibAllocEnums::EnableRet::ENABLE_DEFAULT;
		_move_.m_lock = nullptr;
		_move_.mem_block_size = 0;
	};

	TLSFAllocator& TLSFAllocator::operator =(const TLSFAllocator& _copy_) noexcept
	{
		//自己代入を解決する実装。
		if (mp_main_memblock != _copy_.mp_main_memblock)
		{
			TLSFAllocator lsp;
			lsp.mp_main_memblock = mp_main_memblock;
			lsp.mp_freelistblock = mp_freelistblock;
			lsp.m_enabled_state = m_enabled_state;
			lsp.mem_block_size = mem_block_size;

			mp_main_memblock = _copy_.mp_main_memblock;
			mp_freelistblock = _copy_.mp_freelistblock;
			m_enabled_state = _copy_.m_enabled_state;
			m_lock = _copy_.m_lock;
			mem_block_size = _copy_.mem_block_size;

		};

		return (*this);
	};

	TLSFAllocator& TLSFAllocator::operator =(TLSFAllocator&& _move_) noexcept
	{
		//自己代入を解決する実装。
		if (mp_main_memblock != _move_.mp_main_memblock)
		{
			TLSFAllocator lsp;
			lsp.mp_main_memblock = mp_main_memblock;
			lsp.mp_freelistblock = mp_freelistblock;
			lsp.m_enabled_state = m_enabled_state;
			lsp.mem_block_size = mem_block_size;

			//すべてリテラル/ポインタ型で、オブジェクト型はないためstd::move不要。
			//してもいいけど、std::move分のオーバーヘッドコストが発生するのでしなくていいならしないほうがいい。
			mp_main_memblock = _move_.mp_main_memblock;
			mp_freelistblock = _move_.mp_freelistblock;
			m_enabled_state = _move_.m_enabled_state;
			m_lock = _move_.m_lock;
			mem_block_size = _move_.mem_block_size;

			_move_.mp_main_memblock = nullptr;
			_move_.mp_freelistblock = nullptr;
			_move_.m_enabled_state = SLibAllocEnums::EnableRet::ENABLE_DEFAULT;
			_move_.m_lock = nullptr;
			_move_.mem_block_size = 0;

		};

		return (*this);
	};

	TLSFAllocator::~TLSFAllocator(void)
	{

		if (mp_main_memblock != nullptr)
		{
#if defined(_WIN64)
			//windows
			_aligned_free(mp_main_memblock);

#elif defined(__linux__)
			//linux
			free(mp_main_memblock);
#else
#if __cplusplus >= 201703L
			//C++17以降
			std::free(mp_main_memblock)
#else
			//C++17以前
			delete[] mp_main_memblock;
#endif

#endif

		};

		if (mp_freelistblock != nullptr)
		{
#if defined(_WIN64)
			//windows
			_aligned_free(mp_freelistblock);

#elif defined(__linux__)
			//linux
			free(mp_freelistblock);
#else
#if __cplusplus >= 201703L
			//C++17以降
			std::free(mp_freelistblock)

#else
			//C++17以前
			delete[] mp_freelistblock;
#endif

#endif

#ifdef __SLIB_TLSF_DEBUG_REPORT_USED__ 
			gl_write_text = "============================== CloseLog ==============================\n";
			gl_tlsf_fcon->Write_UTF8(gl_write_text);
			gl_tlsf_fcon->CloseFile();
#endif

		};

	};

	//アロケータの有効化設定を行います。
	//失敗の場合はサイズ調整の上で再度実施してみてください。
	SLibAllocEnums::EnableRet TLSFAllocator::CreateAllocator(SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface>& _out_, uint32_t _CreateMemorySize_)
	{
		//※内部でローカルオブジェクトの対してまずは初期化を実施し、コピーする形なので
		//どの失敗ルートを通っても必ずデストラクタを通る。
		//デストラクタでは参照カウントによってオブジェクトの破棄操作が行われるため、
		//失敗時は別途自前でdelete等をする必要はない。してもいいけどnullptr入れ忘れるとクラッシュする可能性もあるため
		//何もしないほうがいい。

		//作成依頼されているメモリサイズが32Byte以下なら 固定で32Byteのフリーブロック+管理ブロック区域を作成する。
		if (_CreateMemorySize_ <= 32)
		{
			//最低32Byte + sizeof(SLIB_TLSF_MEMBLOCK)分を確保する。
			_CreateMemorySize_ = 32 + sizeof(SLIB_TLSF_MEMBLOCK);
		};

		//メモリサイズに対してセンチネル領域をプラスして作成する。
		_CreateMemorySize_ += sizeof(SLIB_TLSF_MEMBLOCK) * 2;

		//ローカルオブジェクト(本体)を生成
		TLSFAllocator* local_object = new(std::nothrow) TLSFAllocator;
		if (local_object == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MTBLOCK;
		};

		local_object->m_lock = new(std::nothrow) SonikLib::S_CAS::SonikAtomicLock;
		if (local_object->m_lock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MTBLOCK;
		};

		//実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		uint64_t l_aling_malloc_size = sizeof(SLIB_TLSF_BITCOLUMBLOCK);

#if defined(_WIN64)
		//windows
		local_object->mp_main_memblock = reinterpret_cast<uint8_t*>(_aligned_malloc(static_cast<size_t>(_CreateMemorySize_), 32));
		if (local_object->mp_main_memblock == nullptr)
		{
			delete local_object->m_lock;
			delete local_object;
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};

		memset(local_object->mp_main_memblock, 0, _CreateMemorySize_);

		local_object->mp_freelistblock = reinterpret_cast<uint8_t*>(_aligned_malloc(l_aling_malloc_size, 32));
		if (local_object->mp_freelistblock == nullptr)
		{
			delete local_object->m_lock;
			delete local_object;
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

		memset(local_object->mp_freelistblock, 0, l_aling_malloc_size);

#elif defined(__linux__)
		//linux
		if (posix_memalign(&local_object->mp_main_memblock, 32, _CreateMemorySize_) != 0)
		{
			delete local_object->m_lock;
			delete local_object;

			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};

		memset(local_object->mp_main_memblock, 0, _CreateMemorySize_);

		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		if (posix_memalign(&local_object->mp_freelistblock, 32, l_aling_malloc_size) != 0)
		{
			delete local_object->m_lock;
			delete local_object;

			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

		memset(local_object->mp_freelistblock, 0, l_aling_malloc_size);

#else

#if __cplusplus >= 201703L
		//C++17以降 ※Visucal C++ 2019 Update 3 ではstd::aligned_allocは未実装
		local_object->mp_main_memblock = reinterpret_cast<uint8_t*>(std::aligned_alloc(32, static_cast<size_t>(_CreateMemorySize_)));
		if (local_object->mp_main_memblock == nullptr)
		{
			delete local_object->m_lock;
			delete local_object;

			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};

		memset(local_object->mp_main_memblock, 0, _CreateMemorySize_);

		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		local_object->mp_freelistblock = reinterpret_cast<uint8_t*>(std::aligned_alloc(32, l_aling_malloc_size));
		if (local_object->mp_freelistblock == nullptr)
		{
			delete local_object->m_lock;
			delete local_object;

			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

		memset(local_object->mp_freelistblock, 0, l_aling_malloc_size);

#else
		//C++17以前
		local_object->mp_main_memblock = new(std::nothrow) uint8_t[_CreateMemorySize_]{};
		if (local_object->mp_main_memblock == nullptr)
		{
			delete local_object->m_lock;
			delete local_object;

			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};


		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		local_object->mp_freelistblock = new(std::nothrow) uint8_t[l_aling_malloc_size]{};
		if (local_object->mp_freelistblock == nullptr)
		{
			delete local_object->m_lock;
			delete local_object;

			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

#endif

#endif

		//ここまで成功したら未設定変数のセット

		//センチネルの設定
		uint8_t* l_controlpointer = reinterpret_cast<uint8_t*>(local_object->mp_main_memblock);
		SLIB_TLSF_MEMBLOCK* l_sentinel_top = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_controlpointer);
		SLIB_TLSF_MEMBLOCK* l_sentinel_end = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>( ((l_controlpointer + _CreateMemorySize_) - sizeof(SLIB_TLSF_MEMBLOCK)) );

		l_sentinel_top->IsUse = true;
		l_sentinel_top->back = l_sentinel_end;

		l_sentinel_end->IsUse = true;
		l_sentinel_end->front = l_sentinel_top;

		l_controlpointer += sizeof(SLIB_TLSF_MEMBLOCK);

		//領域への簡易アクセス用
		SLIB_TLSF_BITCOLUMBLOCK* l_controlbit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(local_object->mp_freelistblock);

		//使用サイズの値を管理用ヘッダ分引いて、実際使えるサイズにしておく。
		uint32_t usevalues = _CreateMemorySize_;
		usevalues -= sizeof(SLIB_TLSF_MEMBLOCK) * 3 ; //通常の管理ブロック + センチネル領域(メモリ空間トップとエンドに配置)

		//第１カテゴリを取得
		uint32_t l_fcat = SonikMathBit::GetMSB(usevalues);
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (usevalues >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//それぞれのカテゴリ番号を使ってビットを立てる。
		l_controlbit->FirstCategoryBit			|= (SLIB_TLSF_CONST_BIT_ONE << l_fcat);
		l_controlbit->SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

		//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
		//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。 
		l_controlbit->FreeAreaAddressBlock[ (SLIB_TLSF_CATEGORY_BIT_CNT * l_fcat) + l_scat ] = reinterpret_cast<uintptr_t>(l_controlpointer);

		//初期値セットのため、初回の貸出用メモリブロックのアドレスを取得し、値を書き込む
		SLIB_TLSF_FREELISTMEMBLOCK* setpoint = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_controlpointer);
		
		setpoint->front = l_sentinel_top;
		setpoint->back = l_sentinel_end;
		setpoint->BlockSize = usevalues;
		setpoint->free_next = nullptr;

		l_sentinel_top->back = setpoint;
		l_sentinel_end->front = setpoint;

		local_object->mem_block_size = usevalues;
		local_object->m_enabled_state = SLibAllocEnums::EnableRet::ENABLED_OK; //状態を成功状態へ。

		if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(local_object, _out_))
		{
			delete local_object->m_lock;
			delete local_object;
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_OBJECTSMTPTR;
		};

		return SLibAllocEnums::EnableRet::ENABLED_OK;
	};

	void* TLSFAllocator::memal(size_t _size_) noexcept
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if (lp_freelist_bit->FirstCategoryBit == 0)
		{
			m_lock->unlock();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		//if (_size_ < 32)
		//{
		//	_size_ = 32;
		//};
		//↑のifのビット演算一発解答バージョン
		_size_ = (((static_cast<uint64_t>(_size_) + 31ULL) & ~31ULL) | (static_cast<uint64_t>(_size_ == 0) * 32ULL));

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		uint32_t l_request_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_size_)); //0が来ることがないのでGetMSBのエラー値である -1が返却されることはない。のでキャストしてしまう。

		uint32_t fcat_mask = lp_freelist_bit->FirstCategoryBit & (0xFFFFFFFFu << l_request_fcat);
		if (fcat_mask == 0u)
		{
			//ファーストカテゴリで使えるのがない。
			m_lock->unlock();
			return nullptr;
		};

		//第１カテゴリとフリーリストの状態をビット演算で見て、フリーリスト側で使用可能な第１カテゴリ値を算出する。
		//mask = 0xFFFFFFFF << l_fcat;
		//mask &= lp_freelist_bit->FirstCategoryBit;
		//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
		//↑の1行書き。
		uint32_t l_use_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(fcat_mask));

		// 同一ファーストカテゴリ内で start_scat 以上のセカンドがあるか先に確認
		uint32_t start_scat = (l_use_fcat > 5)
							  ? (_size_ >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE
							  : 0u;
		uint32_t secBits	 = lp_freelist_bit->SecondCategoryBit[l_use_fcat] & (0xFFFFFFFFu << start_scat);

		//第２カテゴリを取得(ここで0が来ることはない。)
		//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
		uint32_t l_scat;
		if (secBits != 0u)
		{
			int tmpS = SonikMathBit::GetLSB(secBits);
			if (tmpS < 0)
			{
				//LSB見つからず
				m_lock->unlock();
				return nullptr;
			};

			l_scat = static_cast<uint32_t>(tmpS);

		}
		else
		{
			//同一ファーストカテゴリに適合無し -> 次のファーストカテゴリを取得
			uint32_t remainingFirst = fcat_mask & ~(1u << l_use_fcat);
			if (remainingFirst == 0u)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				return nullptr;
			};

			int tmpNextF = SonikMathBit::GetLSB(remainingFirst);
			if (tmpNextF < 0)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				return nullptr;
			};

			l_use_fcat = static_cast<uint32_t>(tmpNextF);
			uint32_t secBits2 = lp_freelist_bit->SecondCategoryBit[l_use_fcat];
			if (secBits2 == 0u)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				return nullptr;
			};
			int tmpS = SonikMathBit::GetLSB(secBits2);
			if (tmpS < 0)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				return nullptr;
			};
			l_scat = static_cast<uint32_t>(tmpS);
		};


		//位置特定できたのでフリーリスト調整処理
		uint32_t FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat; //結構コールしたので変数用意

		//フリーブロックを取得
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex]);
		if (l_block == nullptr)
		{
			m_lock->unlock();
			return nullptr;
		};
		
		//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
		lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(l_block->free_next);
		
		//ビット演算でFreeListが0(nullptr)になっていないかチェック。
		//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
		uint32_t freelist_bit_update_mask = (1u << l_scat);
		if(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0)
		{
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] &= ~freelist_bit_update_mask;
		};
		
		//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
		freelist_bit_update_mask = (1u << l_use_fcat); 
		if (lp_freelist_bit->SecondCategoryBit[l_use_fcat] == 0)
		{
			lp_freelist_bit->FirstCategoryBit &= (~freelist_bit_update_mask);

		};

		//要求サイズと管理ブロックの合計
		uint64_t request_block_size = (_size_ + sizeof(SLIB_TLSF_MEMBLOCK));
		uint64_t free_block_size = l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

		//分割後の最低限必要なサイズを上回るなら分割(最低限必要なサイズ = 前方管理ブロックサイズ + 32Byte)
		//分割不可の場合はそのまま貸出なので後続処理へ
		if ((free_block_size - request_block_size) >= SLIB_TLSF_LOWLIMIT_ALLOCATESIZE)
		{
			//フリーブロックの分割位置
			uint8_t* l_split_point = reinterpret_cast<uint8_t*>(l_block) + (l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK)); //一旦最終地点へ。
			l_split_point -= request_block_size; //最終地点から戻って後方分割のポイント設置完了

			//分割先(返却ポインタ)
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_split_point);

			//設定
			//分割して減った分のサイズで再設定
			l_block->BlockSize -= static_cast<uint32_t>(request_block_size);

			//分割したオブジェクトの設定
			l_block_split->front	 = l_block;		  //分割したオブジェクトの前方を分割前オブジェクトへ接続
			l_block_split->back		 = l_block->back; //分割したオブジェクトの後方を分割前オブジェクトが指している後方と同じ方向へ接続
			l_block_split->BlockSize = static_cast<uint32_t>(_size_);		  //ブロックサイズの設定(request_block_sizeは前方タグを含むサイズになっていて、ここにセットするのは前方タグ抜きの純粋なブロックサイズ)
			l_block->back->front = l_block_split; //分割前オブジェクトが指している後方の前方オブジェクト先を自分から分割後オブジェクトへ接続
			l_block->back 		 = l_block_split; //分割前オブジェクトの後方を分割後オブジェクトへ接続

			//フリーリストへの再登録
			l_block->free_next = nullptr;

			//ブロックサイズから入るべきフリーリスト位置を取得
			//第１カテゴリを取得
			l_use_fcat = SonikMathBit::GetMSB(l_block->BlockSize);
			//第２カテゴリを取得(ここで0が来ることはない。)
			l_scat = (l_block->BlockSize >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//それぞれのカテゴリ番号を使ってビットを立てる。
			lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_use_fcat);
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

			//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
			//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
			//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
			uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat;
			SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
			l_block->free_next = l_control_block;
			lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_block);
			
			//最後に主操作用のポインタ兼返却用であるl_blockを分割後の前方タグへ更新して終了
			l_block = l_block_split;
		};
		
		//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
		//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
		l_block->IsUse = 1;
		m_lock->unlock(); //ロック解除


		//事後作業
		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		uint8_t* ret_point = reinterpret_cast<uint8_t*>(l_block);
		ret_point += sizeof(SLIB_TLSF_MEMBLOCK);

		std::memset(ret_point, 0, _size_);

		//最後に返したる。
		return ret_point;
	};

	//エラーコード出力バージョン
	void* TLSFAllocator::memal(size_t _size_, SLibAllocEnums::EnableRet& _errcode_) noexcept
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			_errcode_ = SLibAllocEnums::EnableRet::ENABLE_FAILED;
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if (lp_freelist_bit->FirstCategoryBit == 0)
		{
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
			m_lock->unlock();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		//if (_size_ < 32)
		//{
		//	_size_ = 32;
		//};
		//↑のifのビット演算一発解答バージョン
		_size_ = (((static_cast<uint64_t>(_size_) + 31ULL) & ~31ULL) | (static_cast<uint64_t>(_size_ == 0) * 32ULL));

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		uint32_t l_request_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_size_)); //0が来ることがないのでGetMSBのエラー値である -1が返却されることはない。のでキャストしてしまう。

		uint32_t fcat_mask = lp_freelist_bit->FirstCategoryBit & (0xFFFFFFFFu << l_request_fcat);
		if (fcat_mask == 0u)
		{
			//ファーストカテゴリで使えるのがない。
			_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_FIRSTCATEGORY;
			m_lock->unlock();
			return nullptr;
		};

		//第１カテゴリとフリーリストの状態をビット演算で見て、フリーリスト側で使用可能な第１カテゴリ値を算出する。
		//mask = 0xFFFFFFFF << l_fcat;
		//mask &= lp_freelist_bit->FirstCategoryBit;
		//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
		//↑の1行書き。
		uint32_t l_use_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(fcat_mask));

		// 同一ファーストカテゴリ内で start_scat 以上のセカンドがあるか先に確認
		uint32_t start_scat = (l_use_fcat > 5)
			? (_size_ >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE
			: 0u;
		uint32_t secBits = lp_freelist_bit->SecondCategoryBit[l_use_fcat] & (0xFFFFFFFFu << start_scat);

		//第２カテゴリを取得(ここで0が来ることはない。)
		//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
		uint32_t l_scat;
		if (secBits != 0u)
		{
			int tmpS = SonikMathBit::GetLSB(secBits);
			if (tmpS < 0)
			{
				//LSB見つからず
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_SECONDCATEGORY;
				m_lock->unlock();
				return nullptr;
			};

			l_scat = static_cast<uint32_t>(tmpS);

		}
		else
		{
			//同一ファーストカテゴリに適合無し -> 次のファーストカテゴリを取得
			uint32_t remainingFirst = fcat_mask & ~(1u << l_use_fcat);
			if (remainingFirst == 0u)
			{
				//ファーストカテゴリ見つからず？
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_FIRSTCATEGORY;
				m_lock->unlock();
				return nullptr;
			};

			int tmpNextF = SonikMathBit::GetLSB(remainingFirst);
			if (tmpNextF < 0)
			{
				//ファーストカテゴリ見つからず？
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_FIRSTCATEGORY;
				m_lock->unlock();
				return nullptr;
			};

			l_use_fcat = static_cast<uint32_t>(tmpNextF);
			uint32_t secBits2 = lp_freelist_bit->SecondCategoryBit[l_use_fcat];
			if (secBits2 == 0u)
			{
				//セカンドカテゴリ見つからず
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_SECONDCATEGORY;
				m_lock->unlock();
				return nullptr;
			};
			int tmpS = SonikMathBit::GetLSB(secBits2);
			if (tmpS < 0)
			{
				//セカンドカテゴリ見つからず
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_SECONDCATEGORY;
				m_lock->unlock();
				return nullptr;
			};
			l_scat = static_cast<uint32_t>(tmpS);
		};


		//位置特定できたのでフリーリスト調整処理
		uint32_t FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat; //結構コールしたので変数用意

		//フリーブロックを取得
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex]);
		if (l_block == nullptr)
		{
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_UNEXCEPTED;
			m_lock->unlock();
			return nullptr;
		};

		//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
		lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(l_block->free_next);

		//ビット演算でFreeListが0(nullptr)になっていないかチェック。
		//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
		uint32_t freelist_bit_update_mask = (1u << l_scat);
		if (lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0)
		{
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] &= ~freelist_bit_update_mask;
		};

		//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
		freelist_bit_update_mask = (1u << l_use_fcat);
		if (lp_freelist_bit->SecondCategoryBit[l_use_fcat] == 0)
		{
			lp_freelist_bit->FirstCategoryBit &= (~freelist_bit_update_mask);

		};

		//要求サイズと管理ブロックの合計
		uint64_t request_block_size = (_size_ + sizeof(SLIB_TLSF_MEMBLOCK));
		uint64_t free_block_size = l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

		//分割後の最低限必要なサイズを上回るなら分割(最低限必要なサイズ = 前方管理ブロックサイズ + 32Byte)
		//分割不可の場合はそのまま貸出なので後続処理へ
		if ((free_block_size - request_block_size) >= SLIB_TLSF_LOWLIMIT_ALLOCATESIZE)
		{
			//フリーブロックの分割位置
			uint8_t* l_split_point = reinterpret_cast<uint8_t*>(l_block) + (l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK)); //一旦最終地点へ。
			l_split_point -= request_block_size; //最終地点から戻って後方分割のポイント設置完了

			//分割先(返却ポインタ)
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_split_point);

			//設定
			//分割して減った分のサイズで再設定
			l_block->BlockSize -= static_cast<uint32_t>(request_block_size);

			//分割したオブジェクトの設定
			l_block_split->front = l_block;		  //分割したオブジェクトの前方を分割前オブジェクトへ接続
			l_block_split->back = l_block->back; //分割したオブジェクトの後方を分割前オブジェクトが指している後方と同じ方向へ接続
			l_block_split->BlockSize = static_cast<uint32_t>(_size_);		  //ブロックサイズの設定(request_block_sizeは前方タグを含むサイズになっていて、ここにセットするのは前方タグ抜きの純粋なブロックサイズ)
			l_block->back->front = l_block_split; //分割前オブジェクトが指している後方の前方オブジェクト先を自分から分割後オブジェクトへ接続
			l_block->back = l_block_split; //分割前オブジェクトの後方を分割後オブジェクトへ接続

			//フリーリストへの再登録
			l_block->free_next = nullptr;

			//ブロックサイズから入るべきフリーリスト位置を取得
			//第１カテゴリを取得
			l_use_fcat = SonikMathBit::GetMSB(l_block->BlockSize);
			//第２カテゴリを取得(ここで0が来ることはない。)
			l_scat = (l_block->BlockSize >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//それぞれのカテゴリ番号を使ってビットを立てる。
			lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_use_fcat);
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

			//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
			//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
			//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
			uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat;
			SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
			l_block->free_next = l_control_block;
			lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_block);

			//最後に主操作用のポインタ兼返却用であるl_blockを分割後の前方タグへ更新して終了
			l_block = l_block_split;
		};

		//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
		//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
		l_block->IsUse = 1;
		m_lock->unlock(); //ロック解除


		//事後作業
		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		uint8_t* ret_point = reinterpret_cast<uint8_t*>(l_block);
		ret_point += sizeof(SLIB_TLSF_MEMBLOCK);

		std::memset(ret_point, 0, _size_);

		//最後に返したる。
		_errcode_ = SLibAllocEnums::EnableRet::ENABLED_OK;
		return ret_point;

	};

	//例外送出版
	//nullptrで返却する部分をthrow std::bad_allocにしてます。
	//処理自体はTLSFNew通常版と同一
	void* TLSFAllocator::memal_Exception(size_t _size_)
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			throw std::bad_alloc();
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if (lp_freelist_bit->FirstCategoryBit == 0)
		{
			m_lock->unlock();
			throw std::bad_alloc();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		//if (_size_ < 32)
		//{
		//	_size_ = 32;
		//};
		//↑のifのビット演算一発解答バージョン
		_size_ = (((static_cast<uint64_t>(_size_) + 31ULL) & ~31ULL) | (static_cast<uint64_t>(_size_ == 0) * 32ULL));

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		uint32_t l_request_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_size_)); //0が来ることがないのでGetMSBのエラー値である -1が返却されることはない。のでキャストしてしまう。

		uint32_t fcat_mask = lp_freelist_bit->FirstCategoryBit & (0xFFFFFFFFu << l_request_fcat);
		if (fcat_mask == 0u)
		{
			//ファーストカテゴリで使えるのがない。
			m_lock->unlock();
			throw std::bad_alloc();
			return nullptr;
		};

		//第１カテゴリとフリーリストの状態をビット演算で見て、フリーリスト側で使用可能な第１カテゴリ値を算出する。
		//mask = 0xFFFFFFFF << l_fcat;
		//mask &= lp_freelist_bit->FirstCategoryBit;
		//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
		//↑の1行書き。
		uint32_t l_use_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(fcat_mask));

		// 同一ファーストカテゴリ内で start_scat 以上のセカンドがあるか先に確認
		uint32_t start_scat = (l_use_fcat > 5)
			? (_size_ >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE
			: 0u;
		uint32_t secBits = lp_freelist_bit->SecondCategoryBit[l_use_fcat] & (0xFFFFFFFFu << start_scat);

		//第２カテゴリを取得(ここで0が来ることはない。)
		//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
		uint32_t l_scat;
		if (secBits != 0u)
		{
			int tmpS = SonikMathBit::GetLSB(secBits);
			if (tmpS < 0)
			{
				//LSB見つからず
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};

			l_scat = static_cast<uint32_t>(tmpS);

		}
		else
		{
			//同一ファーストカテゴリに適合無し -> 次のファーストカテゴリを取得
			uint32_t remainingFirst = fcat_mask & ~(1u << l_use_fcat);
			if (remainingFirst == 0u)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};

			int tmpNextF = SonikMathBit::GetLSB(remainingFirst);
			if (tmpNextF < 0)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};

			l_use_fcat = static_cast<uint32_t>(tmpNextF);
			uint32_t secBits2 = lp_freelist_bit->SecondCategoryBit[l_use_fcat];
			if (secBits2 == 0u)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};
			int tmpS = SonikMathBit::GetLSB(secBits2);
			if (tmpS < 0)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};
			l_scat = static_cast<uint32_t>(tmpS);
		};


		//位置特定できたのでフリーリスト調整処理
		uint32_t FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat; //結構コールしたので変数用意

		//フリーブロックを取得
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex]);
		if (l_block == nullptr)
		{
			m_lock->unlock();
			throw std::bad_alloc();
			return nullptr;
		};

		//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
		lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(l_block->free_next);

		//ビット演算でFreeListが0(nullptr)になっていないかチェック。
		//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
		uint32_t freelist_bit_update_mask = (1u << l_scat);
		if (lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0)
		{
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] &= ~freelist_bit_update_mask;
		};

		//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
		freelist_bit_update_mask = (1u << l_use_fcat);
		if (lp_freelist_bit->SecondCategoryBit[l_use_fcat] == 0)
		{
			lp_freelist_bit->FirstCategoryBit &= (~freelist_bit_update_mask);

		};

		//要求サイズと管理ブロックの合計
		uint64_t request_block_size = (_size_ + sizeof(SLIB_TLSF_MEMBLOCK));
		uint64_t free_block_size = l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

		//分割後の最低限必要なサイズを上回るなら分割(最低限必要なサイズ = 前方管理ブロックサイズ + 32Byte)
		//分割不可の場合はそのまま貸出なので後続処理へ
		if ((free_block_size - request_block_size) >= SLIB_TLSF_LOWLIMIT_ALLOCATESIZE)
		{
			//フリーブロックの分割位置
			uint8_t* l_split_point = reinterpret_cast<uint8_t*>(l_block) + (l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK)); //一旦最終地点へ。
			l_split_point -= request_block_size; //最終地点から戻って後方分割のポイント設置完了

			//分割先(返却ポインタ)
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_split_point);

			//設定
			//分割して減った分のサイズで再設定
			l_block->BlockSize -= static_cast<uint32_t>(request_block_size);

			//分割したオブジェクトの設定
			l_block_split->front = l_block;		  //分割したオブジェクトの前方を分割前オブジェクトへ接続
			l_block_split->back = l_block->back; //分割したオブジェクトの後方を分割前オブジェクトが指している後方と同じ方向へ接続
			l_block_split->BlockSize = static_cast<uint32_t>(_size_);		  //ブロックサイズの設定(request_block_sizeは前方タグを含むサイズになっていて、ここにセットするのは前方タグ抜きの純粋なブロックサイズ)
			l_block->back->front = l_block_split; //分割前オブジェクトが指している後方の前方オブジェクト先を自分から分割後オブジェクトへ接続
			l_block->back = l_block_split; //分割前オブジェクトの後方を分割後オブジェクトへ接続

			//フリーリストへの再登録
			l_block->free_next = nullptr;

			//ブロックサイズから入るべきフリーリスト位置を取得
			//第１カテゴリを取得
			l_use_fcat = SonikMathBit::GetMSB(l_block->BlockSize);
			//第２カテゴリを取得(ここで0が来ることはない。)
			l_scat = (l_block->BlockSize >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//それぞれのカテゴリ番号を使ってビットを立てる。
			lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_use_fcat);
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

			//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
			//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
			//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
			uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat;
			SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
			l_block->free_next = l_control_block;
			lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_block);

			//最後に主操作用のポインタ兼返却用であるl_blockを分割後の前方タグへ更新して終了
			l_block = l_block_split;
		};

		//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
		//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
		l_block->IsUse = 1;
		m_lock->unlock(); //ロック解除


		//事後作業
		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		uint8_t* ret_point = reinterpret_cast<uint8_t*>(l_block);
		ret_point += sizeof(SLIB_TLSF_MEMBLOCK);

		std::memset(ret_point, 0, _size_);

		//最後に返したる。
		return ret_point;
	};


	//配列用の領域獲得
	//基本処理はmemalと一緒。
	void* TLSFAllocator::memalArray(size_t _sizeof_, size_t _elemcnt_) noexcept
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			return nullptr;
		};

		uint64_t _size_ = static_cast<uint64_t>((static_cast<uint64_t>(_sizeof_) * _elemcnt_));

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if (lp_freelist_bit->FirstCategoryBit == 0)
		{
			m_lock->unlock();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		//if (_size_ < 32)
		//{
		//	_size_ = 32;
		//};
		//↑のifのビット演算一発解答バージョン
		_size_ = (((static_cast<uint64_t>(_size_) + 31ULL) & ~31ULL) | (static_cast<uint64_t>(_size_ == 0) * 32ULL));

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		uint32_t l_request_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_size_)); //0が来ることがないのでGetMSBのエラー値である -1が返却されることはない。のでキャストしてしまう。

		uint32_t fcat_mask = lp_freelist_bit->FirstCategoryBit & (0xFFFFFFFFu << l_request_fcat);
		if (fcat_mask == 0u)
		{
			//ファーストカテゴリで使えるのがない。
			m_lock->unlock();
			return nullptr;
		};

		//第１カテゴリとフリーリストの状態をビット演算で見て、フリーリスト側で使用可能な第１カテゴリ値を算出する。
		//mask = 0xFFFFFFFF << l_fcat;
		//mask &= lp_freelist_bit->FirstCategoryBit;
		//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
		//↑の1行書き。
		uint32_t l_use_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(fcat_mask));

		// 同一ファーストカテゴリ内で start_scat 以上のセカンドがあるか先に確認
		uint32_t start_scat = (l_use_fcat > 5)
			? (_size_ >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE
			: 0u;
		uint32_t secBits = lp_freelist_bit->SecondCategoryBit[l_use_fcat] & (0xFFFFFFFFu << start_scat);

		//第２カテゴリを取得(ここで0が来ることはない。)
		//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
		uint32_t l_scat;
		if (secBits != 0u)
		{
			int tmpS = SonikMathBit::GetLSB(secBits);
			if (tmpS < 0)
			{
				//LSB見つからず
				m_lock->unlock();
				return nullptr;
			};

			l_scat = static_cast<uint32_t>(tmpS);

		}
		else
		{
			//同一ファーストカテゴリに適合無し -> 次のファーストカテゴリを取得
			uint32_t remainingFirst = fcat_mask & ~(1u << l_use_fcat);
			if (remainingFirst == 0u)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				return nullptr;
			};

			int tmpNextF = SonikMathBit::GetLSB(remainingFirst);
			if (tmpNextF < 0)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				return nullptr;
			};

			l_use_fcat = static_cast<uint32_t>(tmpNextF);
			uint32_t secBits2 = lp_freelist_bit->SecondCategoryBit[l_use_fcat];
			if (secBits2 == 0u)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				return nullptr;
			};
			int tmpS = SonikMathBit::GetLSB(secBits2);
			if (tmpS < 0)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				return nullptr;
			};
			l_scat = static_cast<uint32_t>(tmpS);
		};


		//位置特定できたのでフリーリスト調整処理
		uint32_t FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat; //結構コールしたので変数用意

		//フリーブロックを取得
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex]);
		if (l_block == nullptr)
		{
			m_lock->unlock();
			return nullptr;
		};

		//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
		lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(l_block->free_next);

		//ビット演算でFreeListが0(nullptr)になっていないかチェック。
		//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
		uint32_t freelist_bit_update_mask = (1u << l_scat);
		if (lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0)
		{
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] &= ~freelist_bit_update_mask;
		};

		//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
		freelist_bit_update_mask = (1u << l_use_fcat);
		if (lp_freelist_bit->SecondCategoryBit[l_use_fcat] == 0)
		{
			lp_freelist_bit->FirstCategoryBit &= (~freelist_bit_update_mask);

		};

		//要求サイズと管理ブロックの合計
		uint64_t request_block_size = (_size_ + sizeof(SLIB_TLSF_MEMBLOCK));
		uint64_t free_block_size = l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

		//分割後の最低限必要なサイズを上回るなら分割(最低限必要なサイズ = 前方管理ブロックサイズ + 32Byte)
		//分割不可の場合はそのまま貸出なので後続処理へ
		if ((free_block_size - request_block_size) >= SLIB_TLSF_LOWLIMIT_ALLOCATESIZE)
		{
			//フリーブロックの分割位置
			uint8_t* l_split_point = reinterpret_cast<uint8_t*>(l_block) + (l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK)); //一旦最終地点へ。
			l_split_point -= request_block_size; //最終地点から戻って後方分割のポイント設置完了

			//分割先(返却ポインタ)
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_split_point);

			//設定
			//分割して減った分のサイズで再設定
			l_block->BlockSize -= static_cast<uint32_t>(request_block_size);

			//分割したオブジェクトの設定
			l_block_split->front = l_block;		  //分割したオブジェクトの前方を分割前オブジェクトへ接続
			l_block_split->back = l_block->back; //分割したオブジェクトの後方を分割前オブジェクトが指している後方と同じ方向へ接続
			l_block_split->BlockSize = static_cast<uint32_t>(_size_);		  //ブロックサイズの設定(request_block_sizeは前方タグを含むサイズになっていて、ここにセットするのは前方タグ抜きの純粋なブロックサイズ)
			l_block->back->front = l_block_split; //分割前オブジェクトが指している後方の前方オブジェクト先を自分から分割後オブジェクトへ接続
			l_block->back = l_block_split; //分割前オブジェクトの後方を分割後オブジェクトへ接続

			//フリーリストへの再登録
			l_block->free_next = nullptr;

			//ブロックサイズから入るべきフリーリスト位置を取得
			//第１カテゴリを取得
			l_use_fcat = SonikMathBit::GetMSB(l_block->BlockSize);
			//第２カテゴリを取得(ここで0が来ることはない。)
			l_scat = (l_block->BlockSize >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//それぞれのカテゴリ番号を使ってビットを立てる。
			lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_use_fcat);
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

			//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
			//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
			//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
			uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat;
			SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
			l_block->free_next = l_control_block;
			lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_block);

			//最後に主操作用のポインタ兼返却用であるl_blockを分割後の前方タグへ更新して終了
			l_block = l_block_split;
		};

		//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
		//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
		l_block->IsUse = 1;
		m_lock->unlock(); //ロック解除


		//事後作業
		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		uint8_t* ret_point = reinterpret_cast<uint8_t*>(l_block);
		ret_point += sizeof(SLIB_TLSF_MEMBLOCK);

		std::memset(ret_point, 0, _size_);

		//最後に返したる。
		return ret_point;
	};

	//エラーコード出力バージョン
	void* TLSFAllocator::memalArray(size_t _sizeof_, size_t _elemcnt_, SLibAllocEnums::EnableRet& _errcode_) noexcept
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			_errcode_ = SLibAllocEnums::EnableRet::ENABLE_FAILED;
			return nullptr;
		};

		uint64_t _size_ = static_cast<uint64_t>((static_cast<uint64_t>(_sizeof_) * _elemcnt_));

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if (lp_freelist_bit->FirstCategoryBit == 0)
		{
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
			m_lock->unlock();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		//if (_size_ < 32)
		//{
		//	_size_ = 32;
		//};
		//↑のifのビット演算一発解答バージョン
		_size_ = (((static_cast<uint64_t>(_size_) + 31ULL) & ~31ULL) | (static_cast<uint64_t>(_size_ == 0) * 32ULL));

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		uint32_t l_request_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_size_)); //0が来ることがないのでGetMSBのエラー値である -1が返却されることはない。のでキャストしてしまう。

		uint32_t fcat_mask = lp_freelist_bit->FirstCategoryBit & (0xFFFFFFFFu << l_request_fcat);
		if (fcat_mask == 0u)
		{
			//ファーストカテゴリで使えるのがない。
			_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_FIRSTCATEGORY;
			m_lock->unlock();
			return nullptr;
		};

		//第１カテゴリとフリーリストの状態をビット演算で見て、フリーリスト側で使用可能な第１カテゴリ値を算出する。
		//mask = 0xFFFFFFFF << l_fcat;
		//mask &= lp_freelist_bit->FirstCategoryBit;
		//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
		//↑の1行書き。
		uint32_t l_use_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(fcat_mask));

		// 同一ファーストカテゴリ内で start_scat 以上のセカンドがあるか先に確認
		uint32_t start_scat = (l_use_fcat > 5)
			? (_size_ >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE
			: 0u;
		uint32_t secBits = lp_freelist_bit->SecondCategoryBit[l_use_fcat] & (0xFFFFFFFFu << start_scat);

		//第２カテゴリを取得(ここで0が来ることはない。)
		//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
		uint32_t l_scat;
		if (secBits != 0u)
		{
			int tmpS = SonikMathBit::GetLSB(secBits);
			if (tmpS < 0)
			{
				//LSB見つからず
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_SECONDCATEGORY;
				m_lock->unlock();
				return nullptr;
			};

			l_scat = static_cast<uint32_t>(tmpS);

		}
		else
		{
			//同一ファーストカテゴリに適合無し -> 次のファーストカテゴリを取得
			uint32_t remainingFirst = fcat_mask & ~(1u << l_use_fcat);
			if (remainingFirst == 0u)
			{
				//ファーストカテゴリ見つからず？
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_FIRSTCATEGORY;
				m_lock->unlock();
				return nullptr;
			};

			int tmpNextF = SonikMathBit::GetLSB(remainingFirst);
			if (tmpNextF < 0)
			{
				//ファーストカテゴリ見つからず？
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_FIRSTCATEGORY;
				m_lock->unlock();
				return nullptr;
			};

			l_use_fcat = static_cast<uint32_t>(tmpNextF);
			uint32_t secBits2 = lp_freelist_bit->SecondCategoryBit[l_use_fcat];
			if (secBits2 == 0u)
			{
				//セカンドカテゴリ見つからず
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_SECONDCATEGORY;
				m_lock->unlock();
				return nullptr;
			};
			int tmpS = SonikMathBit::GetLSB(secBits2);
			if (tmpS < 0)
			{
				//セカンドカテゴリ見つからず
				_errcode_ = SLibAllocEnums::EnableRet::MEM_CAT_ERR_NOMATCH_SECONDCATEGORY;
				m_lock->unlock();
				return nullptr;
			};
			l_scat = static_cast<uint32_t>(tmpS);
		};


		//位置特定できたのでフリーリスト調整処理
		uint32_t FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat; //結構コールしたので変数用意

		//フリーブロックを取得
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex]);
		if (l_block == nullptr)
		{
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_UNEXCEPTED;
			m_lock->unlock();
			return nullptr;
		};

		//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
		lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(l_block->free_next);

		//ビット演算でFreeListが0(nullptr)になっていないかチェック。
		//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
		uint32_t freelist_bit_update_mask = (1u << l_scat);
		if (lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0)
		{
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] &= ~freelist_bit_update_mask;
		};

		//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
		freelist_bit_update_mask = (1u << l_use_fcat);
		if (lp_freelist_bit->SecondCategoryBit[l_use_fcat] == 0)
		{
			lp_freelist_bit->FirstCategoryBit &= (~freelist_bit_update_mask);

		};

		//要求サイズと管理ブロックの合計
		uint64_t request_block_size = (_size_ + sizeof(SLIB_TLSF_MEMBLOCK));
		uint64_t free_block_size = l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

		//分割後の最低限必要なサイズを上回るなら分割(最低限必要なサイズ = 前方管理ブロックサイズ + 32Byte)
		//分割不可の場合はそのまま貸出なので後続処理へ
		if ((free_block_size - request_block_size) >= SLIB_TLSF_LOWLIMIT_ALLOCATESIZE)
		{
			//フリーブロックの分割位置
			uint8_t* l_split_point = reinterpret_cast<uint8_t*>(l_block) + (l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK)); //一旦最終地点へ。
			l_split_point -= request_block_size; //最終地点から戻って後方分割のポイント設置完了

			//分割先(返却ポインタ)
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_split_point);

			//設定
			//分割して減った分のサイズで再設定
			l_block->BlockSize -= static_cast<uint32_t>(request_block_size);

			//分割したオブジェクトの設定
			l_block_split->front = l_block;		  //分割したオブジェクトの前方を分割前オブジェクトへ接続
			l_block_split->back = l_block->back; //分割したオブジェクトの後方を分割前オブジェクトが指している後方と同じ方向へ接続
			l_block_split->BlockSize = static_cast<uint32_t>(_size_);		  //ブロックサイズの設定(request_block_sizeは前方タグを含むサイズになっていて、ここにセットするのは前方タグ抜きの純粋なブロックサイズ)
			l_block->back->front = l_block_split; //分割前オブジェクトが指している後方の前方オブジェクト先を自分から分割後オブジェクトへ接続
			l_block->back = l_block_split; //分割前オブジェクトの後方を分割後オブジェクトへ接続

			//フリーリストへの再登録
			l_block->free_next = nullptr;

			//ブロックサイズから入るべきフリーリスト位置を取得
			//第１カテゴリを取得
			l_use_fcat = SonikMathBit::GetMSB(l_block->BlockSize);
			//第２カテゴリを取得(ここで0が来ることはない。)
			l_scat = (l_block->BlockSize >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//それぞれのカテゴリ番号を使ってビットを立てる。
			lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_use_fcat);
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

			//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
			//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
			//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
			uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat;
			SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
			l_block->free_next = l_control_block;
			lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_block);

			//最後に主操作用のポインタ兼返却用であるl_blockを分割後の前方タグへ更新して終了
			l_block = l_block_split;
		};

		//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
		//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
		l_block->IsUse = 1;
		m_lock->unlock(); //ロック解除


		//事後作業
		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		uint8_t* ret_point = reinterpret_cast<uint8_t*>(l_block);
		ret_point += sizeof(SLIB_TLSF_MEMBLOCK);

		std::memset(ret_point, 0, _size_);

		//最後に返したる。
		_errcode_ = SLibAllocEnums::EnableRet::ENABLED_OK;
		return ret_point;
	};

	//例外送出版
	void* TLSFAllocator::memalArray_Exception(size_t _sizeof_, size_t _elemcnt_)
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			throw std::bad_alloc();
			return nullptr;
		};

		uint64_t _size_ = static_cast<uint64_t>((static_cast<uint64_t>(_sizeof_) * _elemcnt_));

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if (lp_freelist_bit->FirstCategoryBit == 0)
		{
			m_lock->unlock();
			throw std::bad_alloc();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		//if (_size_ < 32)
		//{
		//	_size_ = 32;
		//};
		//↑のifのビット演算一発解答バージョン
		_size_ = (((static_cast<uint64_t>(_size_) + 31ULL) & ~31ULL) | (static_cast<uint64_t>(_size_ == 0) * 32ULL));

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		uint32_t l_request_fcat = static_cast<uint32_t>(SonikMathBit::GetMSB(_size_)); //0が来ることがないのでGetMSBのエラー値である -1が返却されることはない。のでキャストしてしまう。

		uint32_t fcat_mask = lp_freelist_bit->FirstCategoryBit & (0xFFFFFFFFu << l_request_fcat);
		if (fcat_mask == 0u)
		{
			//ファーストカテゴリで使えるのがない。
			m_lock->unlock();
			throw std::bad_alloc();
			return nullptr;
		};

		//第１カテゴリとフリーリストの状態をビット演算で見て、フリーリスト側で使用可能な第１カテゴリ値を算出する。
		//mask = 0xFFFFFFFF << l_fcat;
		//mask &= lp_freelist_bit->FirstCategoryBit;
		//l_fcat = static_cast<uint32_t>( SonikMathBit::GetMSB(mask) );
		//↑の1行書き。
		uint32_t l_use_fcat = static_cast<uint32_t>(SonikMathBit::GetLSB(fcat_mask));

		// 同一ファーストカテゴリ内で start_scat 以上のセカンドがあるか先に確認
		uint32_t start_scat = (l_use_fcat > 5)
			? (_size_ >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE
			: 0u;
		uint32_t secBits = lp_freelist_bit->SecondCategoryBit[l_use_fcat] & (0xFFFFFFFFu << start_scat);

		//第２カテゴリを取得(ここで0が来ることはない。)
		//リクエストサイズから直接算出したファーストカテゴリー番号と、フリーリストと照らし合わせて算出した使用可能なファーストカテゴリー番号が同じ値ならリクエストサイズから計算
		uint32_t l_scat;
		if (secBits != 0u)
		{
			int tmpS = SonikMathBit::GetLSB(secBits);
			if (tmpS < 0)
			{
				//LSB見つからず
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};

			l_scat = static_cast<uint32_t>(tmpS);

		}
		else
		{
			//同一ファーストカテゴリに適合無し -> 次のファーストカテゴリを取得
			uint32_t remainingFirst = fcat_mask & ~(1u << l_use_fcat);
			if (remainingFirst == 0u)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};

			int tmpNextF = SonikMathBit::GetLSB(remainingFirst);
			if (tmpNextF < 0)
			{
				//ファーストカテゴリ見つからず？
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};

			l_use_fcat = static_cast<uint32_t>(tmpNextF);
			uint32_t secBits2 = lp_freelist_bit->SecondCategoryBit[l_use_fcat];
			if (secBits2 == 0u)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};
			int tmpS = SonikMathBit::GetLSB(secBits2);
			if (tmpS < 0)
			{
				//セカンドカテゴリ見つからず
				m_lock->unlock();
				throw std::bad_alloc();
				return nullptr;
			};
			l_scat = static_cast<uint32_t>(tmpS);
		};


		//位置特定できたのでフリーリスト調整処理
		uint32_t FreeBlockAddresIndex = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat; //結構コールしたので変数用意

		//フリーブロックを取得
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex]);
		if (l_block == nullptr)
		{
			m_lock->unlock();
			throw std::bad_alloc();
			return nullptr;
		};

		//フリーブロックの位置から一旦抜けるため、先頭ブロックをnextへ変更する。
		lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] = reinterpret_cast<uintptr_t>(l_block->free_next);

		//ビット演算でFreeListが0(nullptr)になっていないかチェック。
		//リストがnullptr(0)になっていれば該当箇所のセカンドカテゴリビットを下ろす。
		uint32_t freelist_bit_update_mask = (1u << l_scat);
		if (lp_freelist_bit->FreeAreaAddressBlock[FreeBlockAddresIndex] == 0)
		{
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] &= ~freelist_bit_update_mask;
		};

		//セカンドカテゴリビットの変化でファーストカテゴリビット列も更新が必要なので全く同じように計算し、フラグを更新する。
		freelist_bit_update_mask = (1u << l_use_fcat);
		if (lp_freelist_bit->SecondCategoryBit[l_use_fcat] == 0)
		{
			lp_freelist_bit->FirstCategoryBit &= (~freelist_bit_update_mask);

		};

		//要求サイズと管理ブロックの合計
		uint64_t request_block_size = (_size_ + sizeof(SLIB_TLSF_MEMBLOCK));
		uint64_t free_block_size = l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

		//分割後の最低限必要なサイズを上回るなら分割(最低限必要なサイズ = 前方管理ブロックサイズ + 32Byte)
		//分割不可の場合はそのまま貸出なので後続処理へ
		if ((free_block_size - request_block_size) >= SLIB_TLSF_LOWLIMIT_ALLOCATESIZE)
		{
			//フリーブロックの分割位置
			uint8_t* l_split_point = reinterpret_cast<uint8_t*>(l_block) + (l_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK)); //一旦最終地点へ。
			l_split_point -= request_block_size; //最終地点から戻って後方分割のポイント設置完了

			//分割先(返却ポインタ)
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_split_point);

			//設定
			//分割して減った分のサイズで再設定
			l_block->BlockSize -= static_cast<uint32_t>(request_block_size);

			//分割したオブジェクトの設定
			l_block_split->front = l_block;		  //分割したオブジェクトの前方を分割前オブジェクトへ接続
			l_block_split->back = l_block->back; //分割したオブジェクトの後方を分割前オブジェクトが指している後方と同じ方向へ接続
			l_block_split->BlockSize = static_cast<uint32_t>(_size_);		  //ブロックサイズの設定(request_block_sizeは前方タグを含むサイズになっていて、ここにセットするのは前方タグ抜きの純粋なブロックサイズ)
			l_block->back->front = l_block_split; //分割前オブジェクトが指している後方の前方オブジェクト先を自分から分割後オブジェクトへ接続
			l_block->back = l_block_split; //分割前オブジェクトの後方を分割後オブジェクトへ接続

			//フリーリストへの再登録
			l_block->free_next = nullptr;

			//ブロックサイズから入るべきフリーリスト位置を取得
			//第１カテゴリを取得
			l_use_fcat = SonikMathBit::GetMSB(l_block->BlockSize);
			//第２カテゴリを取得(ここで0が来ることはない。)
			l_scat = (l_block->BlockSize >> (l_use_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

			//それぞれのカテゴリ番号を使ってビットを立てる。
			lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_use_fcat);
			lp_freelist_bit->SecondCategoryBit[l_use_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

			//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
			//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
			//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
			uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat;
			SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
			l_block->free_next = l_control_block;
			lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_use_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_block);

			//最後に主操作用のポインタ兼返却用であるl_blockを分割後の前方タグへ更新して終了
			l_block = l_block_split;
		};

		//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
		//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
		l_block->IsUse = 1;
		m_lock->unlock(); //ロック解除


		//事後作業
		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		uint8_t* ret_point = reinterpret_cast<uint8_t*>(l_block);
		ret_point += sizeof(SLIB_TLSF_MEMBLOCK);

		std::memset(ret_point, 0, _size_);

		//最後に返したる。
		return ret_point;
	};

	//フリーの実装
	//マージ処理は後方 > 前方の順にしていく。
	void TLSFAllocator::__vfunc_memdel__(void* _pfree_) noexcept
	{
		//nullcheckはコール元で実施済みのためスキップ

		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			return;
		};

		uintptr_t IS_ADDR_begin = reinterpret_cast<uintptr_t>(mp_main_memblock);
		uintptr_t IS_ADDR_end = reinterpret_cast<uintptr_t>((mp_main_memblock + mem_block_size));

		if ((reinterpret_cast<uintptr_t>(_pfree_) < IS_ADDR_begin) || (reinterpret_cast<uintptr_t>(_pfree_) >= IS_ADDR_end))
		{
			//範囲外なら強制終了
			return;
		};

		//前方タグを指すポインタ
		SLIB_TLSF_FREELISTMEMBLOCK* l_freepoint = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(reinterpret_cast<uint8_t*>(_pfree_) - sizeof(SLIB_TLSF_MEMBLOCK));
		//フリーリストビット列アクセス用。
		SLIB_TLSF_BITCOLUMBLOCK* lp_freelist_bit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);

		//すでにfree済みなら処理しない。
		if (!l_freepoint->IsUse)
		{
			return;
		};

		m_lock->lock();//マルチスレッドロック（最後までフリーリストの更新とかあるので全体をブロックすることになる..。

		//使用フラグを下ろしておく。
		//ロックあとに降ろさないとマルチスレッドのときに、下ろした瞬間に誰かロックをとって処理された場合にマージされるおそれもあるので。
		l_freepoint->IsUse = 0;

		//編集操作のためのポインタ
		SLIB_TLSF_FREELISTMEMBLOCK* l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_freepoint->back);
		//マージサイズ
		uint32_t marge_size = 0;

		//後方マージチェック及び実施
		if(!l_control_block->IsUse)
		{
			//マージ可能なので自身をフリーリストから除去

			SLIB_TLSF_GLOBAL_FUNC::AdjustMemBlock(l_control_block, lp_freelist_bit);

			//サイズ取得
			marge_size += l_control_block->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);
			//ポインタ調整
			l_freepoint->back = l_control_block->back;
			l_control_block->back->front = l_freepoint;
		};

		//前方マージチェック及び実施
		l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_freepoint->front);
		if (!l_control_block->IsUse)
		{
			//マージ可能なので自身をフリーリストから除去

			SLIB_TLSF_GLOBAL_FUNC::AdjustMemBlock(l_control_block, lp_freelist_bit);

			//サイズ取得(前方なので、l_freepointのサイズを渡す)
			marge_size += l_freepoint->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);
			//ポインタ調整
			l_control_block->back = l_freepoint->back;
			l_freepoint->back->front = l_control_block;
			//freepointを前方に移す
			l_freepoint = l_control_block;
		};

		//ブロックサイズを結合
		l_freepoint->BlockSize = l_freepoint->BlockSize + marge_size;

		//ブロックサイズから入るべきフリーリスト位置を取得
		//第１カテゴリを取得
		uint32_t l_fcat = SonikMathBit::GetMSB(l_freepoint->BlockSize);
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (l_freepoint->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//それぞれのカテゴリ番号を使ってビットを立てる。
		lp_freelist_bit->FirstCategoryBit |= (SLIB_TLSF_CONST_BIT_ONE << l_fcat);
		lp_freelist_bit->SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

		//該当のフリーリストアドレス保存場所へ、空き領域へのアドレスを登録する。
		//移動量は1行32列(2^5) で32行(2^5)まであるため、 SLIB_TLSF_CATEGORY_BIT_CNT(=32) * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。
		//フリーリストの先頭を今回登録するオブジェクトのnextへ設定して先頭に今回のオブジェクトを登録
		uint32_t idx = (SLIB_TLSF_CATEGORY_BIT_CNT * l_fcat) + l_scat;
		l_control_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(lp_freelist_bit->FreeAreaAddressBlock[idx]);
		l_freepoint->free_next = l_control_block;
		lp_freelist_bit->FreeAreaAddressBlock[(SLIB_TLSF_CATEGORY_BIT_CNT * l_fcat) + l_scat] = reinterpret_cast<uintptr_t>(l_freepoint);

		//終了
		m_lock->unlock();
		return;
	};

	//処理内容自体は変わらないのでmemdelをコールしているだけ。
	//外側から呼ぶ際に関数コールを別にしたかったので。
	void TLSFAllocator::__vfung_memdelarray__(void* _pfree_) noexcept
	{
		memdel(_pfree_);
	};


};//end namespace 


#undef SLIB_TLSF_CONST_BIT_ONE
#undef SLIB_TLSF_CONST_BIT_FIVE
#undef SLIB_TLSF_CONST_BIT_32_ONBIT
#undef SLIB_TLSF_CONST_BIT_64_DW32ONBIT
#undef SLIB_TLSF_CONST_BIT_64_UP32ONBIT
#undef SLIB_TLSF_CONST_BIT_64_ALLBITON