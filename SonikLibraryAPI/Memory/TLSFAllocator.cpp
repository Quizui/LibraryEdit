#include "TLSFAllocator.h"
#include "../MathBit/MathBit.h"

#include <new>
#include <utility>
#include <cstdlib>
#include <atomic>

#define SLIB_TLSF_CONST_BIT_ONE 0b00000000000000000000000000000001	//0x01
#define SLIB_TLSF_CONST_BIT_FIVE 0b00000000000000000000000000011111 //0x1F
#define SLIB_TLSF_CONST_BIT_32_ONBIT 0b11111111111111111111111111111111 //0xFFFFFFFF
#define SLIB_TLSF_CONST_BIT_64_DW32ONBIT 0b000000000000000000000000000000011111111111111111111111111111111ULL //0x00000000FFFFFFFF
#define SLIB_TLSF_CONST_BIT_64_UP32ONBIT 0b111111111111111111111111111111110000000000000000000000000000000ULL //0xFFFFFFFF00000000
#define SLIB_TLSF_CONST_BIT_64_ALLBITON 0b1111111111111111111111111111111111111111111111111111111111111111ULL //0xFFFFFFFFFFFFFFFF

namespace SonikLib
{
	//各種アクセス、設定用クラス(ポインタ解釈としてしか使わない。
	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	class SLIB_TLSF_MEMBLOCK
	{
	public:
		SLIB_TLSF_MEMBLOCK* prev;
		SLIB_TLSF_MEMBLOCK* next;
		uint32_t BlockSize;
		uint8_t IsUse;

	};

	class SLIB_TLSF_FREELISTMEMBLOCK
	{
	public:
		SLIB_TLSF_MEMBLOCK* phys_prev;
		SLIB_TLSF_MEMBLOCK* phys_next;
		uint32_t BlockSize;
		uint8_t IsUse;
		SLIB_TLSF_FREELISTMEMBLOCK* free_prev;
		SLIB_TLSF_FREELISTMEMBLOCK* free_next;
		uint8_t IsLast;

	};

	class SLIB_TLSF_BITCOLUMBLOCK
	{
	public:
		uint32_t* mp_FirstCategoryBit;
		uint32_t* mp_SecondCategoryBit;
	};

	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝


	//クラス実装================================================================================
	TLSFAllocator::TLSFAllocator(void) noexcept
		:mp_main_memblock(nullptr)
		, mp_freelistblock(nullptr)
		, mp_FreelistAreaTop(nullptr)
		, m_lock(nullptr)
		, mem_block_size(0)
		//		, m_RefCnt(nullptr)
	{
		//no process
	};

	TLSFAllocator::TLSFAllocator(const TLSFAllocator& _copy_) noexcept
		:mp_main_memblock(_copy_.mp_main_memblock)
		, mp_freelistblock(_copy_.mp_freelistblock)
		, mp_FreelistAreaTop(_copy_.mp_FreelistAreaTop)
		, m_lock(_copy_.m_lock)
		, mem_block_size(_copy_.mem_block_size)
		//		, m_RefCnt(_copy_.m_RefCnt)
	{
		this->m_enabled_state = _copy_.m_enabled_state;

		//		__RefCntAddRef__();
	};

	//すべてリテラル/ポインタ型で、オブジェクト型はないためstd::move不要。
	//してもいいけど、std::move分のオーバーヘッドコストが発生するのでしなくていいならしないほうがいい。
	TLSFAllocator::TLSFAllocator(TLSFAllocator&& _move_) noexcept
		:mp_main_memblock(_move_.mp_main_memblock)
		, mp_freelistblock(_move_.mp_freelistblock)
		, mp_FreelistAreaTop(_move_.mp_FreelistAreaTop)
		, m_lock(_move_.m_lock)
		, mem_block_size(_move_.mem_block_size)
		//		, m_RefCnt(_move_.m_RefCnt)
	{
		this->m_enabled_state = _move_.m_enabled_state;

		_move_.mp_main_memblock = nullptr;
		_move_.mp_freelistblock = nullptr;
		_move_.mp_FreelistAreaTop = nullptr;
		_move_.m_enabled_state = SLibAllocEnums::EnableRet::ENABLE_DEFAULT;
		_move_.m_lock = nullptr;
		_move_.mem_block_size = 0;
		//		_move_.m_RefCnt = nullptr;
	};

	TLSFAllocator& TLSFAllocator::operator =(const TLSFAllocator& _copy_) noexcept
	{
		//自己代入を解決する実装。
		if (mp_main_memblock != _copy_.mp_main_memblock)
		{
			TLSFAllocator lsp;
			lsp.mp_main_memblock = mp_main_memblock;
			lsp.mp_freelistblock = mp_freelistblock;
			lsp.mp_FreelistAreaTop = mp_FreelistAreaTop;
			lsp.m_enabled_state = m_enabled_state;
			lsp.mem_block_size = mem_block_size;
			//			lsp.m_RefCnt = m_RefCnt;

			mp_main_memblock = _copy_.mp_main_memblock;
			mp_freelistblock = _copy_.mp_freelistblock;
			mp_FreelistAreaTop = _copy_.mp_FreelistAreaTop;
			m_enabled_state = _copy_.m_enabled_state;
			m_lock = _copy_.m_lock;
			mem_block_size = _copy_.mem_block_size;
			//			m_RefCnt = _copy_.m_RefCnt;

			//			__RefCntAddRef__();
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
			lsp.mp_FreelistAreaTop = mp_FreelistAreaTop;
			lsp.m_enabled_state = m_enabled_state;
			lsp.mem_block_size = mem_block_size;
			//			lsp.m_RefCnt = m_RefCnt;

						//すべてリテラル/ポインタ型で、オブジェクト型はないためstd::move不要。
						//してもいいけど、std::move分のオーバーヘッドコストが発生するのでしなくていいならしないほうがいい。
			mp_main_memblock = _move_.mp_main_memblock;
			mp_freelistblock = _move_.mp_freelistblock;
			mp_FreelistAreaTop = _move_.mp_FreelistAreaTop;
			m_enabled_state = _move_.m_enabled_state;
			m_lock = _move_.m_lock;
			mem_block_size = _move_.mem_block_size;
			//			m_RefCnt = _move_.m_RefCnt;

			_move_.mp_main_memblock = nullptr;
			_move_.mp_freelistblock = nullptr;
			_move_.mp_FreelistAreaTop = nullptr;
			_move_.m_enabled_state = SLibAllocEnums::EnableRet::ENABLE_DEFAULT;
			_move_.m_lock = nullptr;
			_move_.mem_block_size = 0;
			//			_move_.m_RefCnt = nullptr;

						//moveなのでAddref()をコールする必要はない。
		};

		return (*this);
	};

	TLSFAllocator::~TLSFAllocator(void)
	{
		/*
		if (!__RefCntRelease__())
		{
			return;
		};
		*/

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
		};

	};

	/*
		//参照カウンタの加算
		void TLSFAllocator::__RefCntAddRef__(void) noexcept
		{
			if (m_RefCnt == nullptr)
			{
				return;
			};

			uint64_t TmpCnt = m_RefCnt->load(std::memory_order_acquire);
			while (!m_RefCnt->compare_exchange_strong(TmpCnt, TmpCnt + 1, std::memory_order_acq_rel))
			{
				//no process
			};
		};

		//参照カウンタの減算
		bool TLSFAllocator::__RefCntRelease__(void) noexcept
		{
			//Release処理
			if (m_RefCnt == nullptr)
			{
				return false;
			};

			uint64_t TmpCnt = m_RefCnt->load(std::memory_order_acquire);
			if (TmpCnt == 1)
			{
				delete m_RefCnt;
				m_RefCnt = nullptr;

				return true;
			};

			while (!m_RefCnt->compare_exchange_strong(TmpCnt, TmpCnt - 1, std::memory_order_acq_rel))
			{
				//no process
			};

			if (TmpCnt == 1)
			{
				delete m_RefCnt;
				m_RefCnt = nullptr;

				return true;
			};

			return false;

		};
	*/

	//アロケータの有効化設定を行います。
	//失敗の場合はサイズ調整の上で再度実施してみてください。
	SLibAllocEnums::EnableRet TLSFAllocator::CreateAllocator(SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface>& _out_, uint32_t _CreateMemorySize_)
	{
		//※内部でローカルオブジェクトの対してまずは初期化を実施し、コピーする形なので
		//どの失敗ルートを通っても必ずデストラクタを通る。
		//デストラクタでは参照カウントによってオブジェクトの破棄操作が行われるため、
		//失敗時は別途自前でdelete等をする必要はない。してもいいけどnullptr入れ忘れるとクラッシュする可能性もあるため
		//何もしないほうがいい。

		if (_CreateMemorySize_ <= 32)
		{
			//最低32Byte + sizeof(SLIB_TLSF_MEMBLOCK)分を確保する。
			_CreateMemorySize_ = 32 + sizeof(SLIB_TLSF_MEMBLOCK);
		};

		TLSFAllocator* local_object = new(std::nothrow) TLSFAllocator;

		/*
				local_object->m_RefCnt = new(std::nothrow) std::atomic<uint64_t>(1);
				if (local_object->m_RefCnt == nullptr)
				{
					return SLibAllocEnums::EnableRet::HEEPALLOCERR_REFCNT;
				};
		*/
		local_object->m_lock = new(std::nothrow) SonikLib::S_CAS::SonikAtomicLock;
		if (local_object->m_lock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MTBLOCK;
		};

#if defined(_WIN64)
		//windows
		local_object->mp_main_memblock = reinterpret_cast<uint8_t*>(_aligned_malloc(static_cast<size_t>(_CreateMemorySize_), 32));
		if (local_object->mp_main_memblock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};

		memset(local_object->mp_main_memblock, 0, _CreateMemorySize_);

		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		local_object->mp_freelistblock = reinterpret_cast<uint8_t*>(_aligned_malloc(static_cast<size_t>((sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024)), 32));
		if (local_object->mp_freelistblock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

		memset(local_object->mp_freelistblock, 0, (sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024));

#elif defined(__linux__)
		//linux
		if (posix_memalign(&local_object->mp_main_memblock, 32, _CreateMemorySize_) != 0)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};

		memset(local_object->mp_main_memblock, 0, _CreateMemorySize_);

		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		if (posix_memalign(&local_object->mp_freelistblock, 32, (sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024)) != 0)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

		memset(local_object->mp_freelistblock, 0, (sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024));

#else

#if __cplusplus >= 201703L
		//C++17以降 ※Visucal C++ 2019 Update 3 ではstd::aligned_allocは未実装
		local_object->mp_main_memblock = reinterpret_cast<uint8_t*>(std::aligned_alloc(32, static_cast<size_t>(_CreateMemorySize_)));
		if (local_object->mp_main_memblock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};

		memset(local_object->mp_main_memblock, 0, _CreateMemorySize_);

		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		local_object->mp_freelistblock = reinterpret_cast<uint8_t*>(std::aligned_alloc(32, static_cast<size_t>((sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024))));
		if (local_object->mp_freelistblock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

		memset(local_object->mp_freelistblock, 0, (sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024));

#else
		//C++17以前
		local_object->mp_main_memblock = new(std::nothrow) uint8_t[_CreateMemorySize_]{};
		if (local_object->mp_main_memblock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_MAINBLOCK;
		};


		//フリーリストビット列へのポインタ保持領域(縦分1個*横分1個) + 実フリーリストビット列領域 + 実フリーリストへのポインタ保持領域
		local_object->mp_freelistblock = new(std::nothrow) uint8_t[(sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33) + (sizeof(uintptr_t) * 1024)]{};
		if (local_object->mp_freelistblock == nullptr)
		{
			return SLibAllocEnums::EnableRet::HEEPALLOCERR_FREEBLOCK;
		};

#endif

#endif

		//ここまで成功したら未設定変数のセット
		local_object->mp_FreelistAreaTop = local_object->mp_freelistblock + ((sizeof(uintptr_t) * 2) + (sizeof(uint32_t) * 33));

		//初回用フリーリストとビット列を設定
		uintptr_t* l_setaddr = reinterpret_cast<uintptr_t*>(local_object->mp_freelistblock);
		(*l_setaddr) = reinterpret_cast<uintptr_t>(local_object->mp_freelistblock + (sizeof(uintptr_t) * 2));
		++l_setaddr;
		(*l_setaddr) = reinterpret_cast<uintptr_t>(local_object->mp_freelistblock + (sizeof(uintptr_t) * 2) + sizeof(uint32_t));

		//領域への簡易アクセス用
		SLIB_TLSF_BITCOLUMBLOCK* l_controlbit = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(local_object->mp_freelistblock);

		//使用サイズの値を管理用ヘッダ分引いて、実際使えるサイズにしておく。
		_CreateMemorySize_ -= sizeof(SLIB_TLSF_MEMBLOCK);

		//第１カテゴリを取得
		uint32_t l_fcat = SonikMathBit::GetMSB(_CreateMemorySize_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_CreateMemorySize_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//それぞれのカテゴリ番号を使ってビットを立てる。
		(*l_controlbit->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_fcat);
		l_controlbit->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_scat);

		//フリーリストのアドレスを登録する領域を算出
		//(uint32_t * 33)Byte先がフリーリストのスタート地点(mp_FreelistAreaTopとして保存)
		//uintptr_t* として再解釈し、uintptr_tのサイズでポインタを移動させる。
		//移動量は1行32列(2^5) で32行(2^5)まであるため、 32 * ファーストカテゴリMSB値で 行をずらす。これに + セカンドカテゴリMSB値することでずらした先の列を特定する。 
		uintptr_t* l_point = reinterpret_cast<uintptr_t*>(local_object->mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);
		(*l_point) = reinterpret_cast<uintptr_t>(local_object->mp_main_memblock); //uintptr_tの領域にポインタ先アドレスを書き込み

		SLIB_TLSF_FREELISTMEMBLOCK* setpoint = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_point)); //アドレス値を翻訳
		setpoint->phys_prev = nullptr;
		setpoint->phys_next = nullptr;
		setpoint->BlockSize = _CreateMemorySize_;
		setpoint->free_prev = nullptr;
		setpoint->free_next = nullptr;
		setpoint->IsLast = true;

		local_object->mem_block_size = _CreateMemorySize_;
		local_object->m_enabled_state = SLibAllocEnums::EnableRet::ENABLED_OK; //状態を成功状態へ。

		if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(local_object, _out_))
		{
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

		//0はだめ。
		if (_size_ == 0)
		{
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if ((*lp_flg->mp_FirstCategoryBit) == 0)
		{
			m_lock->unlock();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		if (_size_ < 32)
		{
			_size_ = 32;
		};

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		int32_t l_fcat = SonikMathBit::GetMSB(_size_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_size_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//要求サイズのカテゴリビットを64bit列に纏める
		//uint64_t nowsize_categorybit = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat)) << 32);
		//nowsize_categorybit |= static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_scat));
		//↑を1行にまとめたもの。
		uint64_t nowsize_categorybit = ((static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_fcat) << 32) | (static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_scat);

		//現在のフリーリストの状態(該当行、列)を64bit列に纏める。
		//uint64_t nowfreelist_categorybit = static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32;
		//nowfreelist_categorybit |= lp_flg->mp_SecondCategoryBit[l_fcat];
		//↑を1行にまとめたもの。
		uint64_t nowfreelist_categorybit = (static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32) | lp_flg->mp_SecondCategoryBit[l_fcat];

		//現在と要求のビット列をAND > Count してswitch要素を獲得
		uint16_t l_switch_bitcnt = SonikMathBit::OnBitCount((nowfreelist_categorybit & nowsize_categorybit));

		//0 or 1なら各位置調整処理、2ならそのまま後続処理へ。
		//case 2 はbreakのみだが、無いとdefault分類されてしまう。
		switch (l_switch_bitcnt)
		{
		case 0:
			//ファースト、セカンドカテゴリともにヒットせず。
			//上位のファーストカテゴリを探し、あれば一番近いファーストのセカンドカテゴリ位置を使用する。

			//下位32bitのセカンドカテゴリ区域 + 現在のファーストカテゴリの位置までのビットをすべてOFFに。
			//uint64_t l_tmp = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat) << 32) -1; //対象のファーストカテゴリの位置以下のビットをすべてONに。
			//l_tmp = l_tmp << 1;																	 //1bit左にずらして、1bit目を除いた以降のファーストカテゴリ位置までのビットをONの状態に。
			//l_tmp = l_tmp | 0x01;																	 //1bit目のみ0なのでONに。
			//l_tmp = ~l_tmp;																		 //反転して、要求サイズファーストカテゴリを含む下位ビットをすべてOFFにするマスクを作成
			//l_tmp = (nowfreelist_categorybit & l_tmp) >> 32;										 //マスクを適用し、もともとの32bit列の整数へ。
			//l_fcat = SonikMathBit::GetLSB(l_tmp);													 //最後にLSBを取ると、要求サイズより上位で、かつ一番近いビット位置が取得できる。
			//↑を1行にまとめたもの。
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 1:
			//仕様上ファーストカテゴリのみ立っている状態でHITする可能性がある。
			//同じファーストカテゴリ内の上位に使えるリストのビットが立っていればそこを、なければcase 0 のときと同様の位置を使う。

			//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
			l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
			if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
			{
				break;
			};

			//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
			//以下case0 処理をコピーレフト
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 2:
			//マッチヒット。その位置を使う。
			break;

		default:
			//エラーです。
			m_lock->unlock();
			return nullptr;

		};


		//位置特定できたのでフリーリスト調整処理
		uintptr_t* l_matchaddress = 0;
		//位置ポインタを取得
		l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//該当場所のポインタから分割処理等を実施する。
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_matchaddress));

		//先頭ポインタ位置からフリーリストを手繰って要求サイズの広さを持ったオブジェクトを探す。
		while (l_block->BlockSize < _size_) //絶対マッチした場所のポインタを拾ってくるのでnullptr参照になることはない。(警告はでるけどね。)
		{
			//ブロックサイズが足りなければ次の要素へ
			l_block = l_block->free_next;
			if (l_block == nullptr)
			{
				//なかったので上位から取る。
				//switch のcase 1処理をコピーレフト
				//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
				l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
				if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
				{
					break;
				};

				//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
				//以下case0 処理をコピーレフト
				l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
				if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
				{
					m_lock->unlock();
					return nullptr;
				};

				//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
				l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

				//位置ポインタを取得
				l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

				break;
			};

		};


		//違う場合、分割後のサイズによって更に分岐。
		//分割後のサイズが32Byte以上になるなら分割実施(分割要求サイズはヘッダサイズもプラスされる。)
		if ((l_block->BlockSize - (_size_ + sizeof(SLIB_TLSF_MEMBLOCK))) >= 32)
		{
			uint8_t* l_offsetcontrol = reinterpret_cast<uint8_t*>((*l_matchaddress));
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = nullptr;

			//分割先を保存
			l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_offsetcontrol + (sizeof(SLIB_TLSF_MEMBLOCK) + _size_));

			//設定
			//一旦情報全てコピー
			std::memcpy(l_block_split, l_block, sizeof(SLIB_TLSF_FREELISTMEMBLOCK)); //ここもnullptrになることはない。
			//分割居残り側のサイズ設定
			l_block_split->BlockSize -= _size_ + sizeof(SLIB_TLSF_MEMBLOCK);

			//各物理メモリ配列の双方向リストの構築
			l_block_split->phys_prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block); //分割された側はnewのときは左(prev)方向しか操作する必要がない。はず....
			l_block->phys_next = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block_split); //分割した側はnewのときは右(next)方向しか操作する必要がない。はず....


			//分割居残り組のフリーリスト候補先を算出================
			//第１カテゴリを取得
			int32_t l_split_fcat = SonikMathBit::GetMSB(l_block_split->BlockSize) - 1;
			//第２カテゴリを取得
			int32_t l_split_scat = (l_block_split->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット	
			//アクセス先
			uintptr_t* l_split_access_addr = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_split_fcat) + l_split_scat);

			//現在の自分の位置と一緒でなければ処理
			if ((l_fcat != l_split_fcat) || (l_scat != l_split_scat))
			{
				//先に古巣からおさらばする
				if ((*l_matchaddress) == reinterpret_cast<uintptr_t>(l_block))
				{
					//分割前のアドレスがマッチアドレスと一緒なら先頭ポインタなのでそのままnextを代入
					(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split->free_next);
					//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
					if ((*l_matchaddress) == 0)
					{
						lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					};

				}
				else
				{
					//一緒じゃない場合中間からの離脱なので双方向ポインタを操作していい具合にする。
					if (l_block_split->free_prev != nullptr)
					{
						l_block_split->free_prev->free_next = l_block_split->free_next;
					};
					if (l_block_split->free_next != nullptr)
					{
						l_block_split->free_next->free_prev = l_block_split->free_prev;
					};

					//中間なのでフリーリストはまだあるのでビット列は操作しない。
				};

				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				//登録処理
				if ((*l_split_access_addr) == 0)
				{
					//新しい先がnullptr なら登録だけ。
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);

				}
				else
				{
					//nullptr以外ならとりあえず先頭に挿入
					l_block_split->free_prev = nullptr;
					l_block_split->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_split_access_addr));
					l_block_split->free_next->free_prev = l_block_split;
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);
				};

				//新しい登録先のビットを立てる。(立っていても立てる)
				(*lp_flg->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_split_fcat);
				lp_flg->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_split_scat);

			}
			else
			{
				//一緒ならポインタだけつけかえてそのまま。
				if (l_block_split->free_prev != nullptr)
				{
					l_block_split->free_prev->free_next = l_block_split;
				};
				if (l_block_split->free_next != nullptr)
				{
					l_block_split->free_next->free_prev = l_block_split;
				};

				//アドレス値の更新
				(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split);

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();

		}
		else
		{
			//サイズ一致の場合はそのまま貸し出す。
			//nextポインタをアドレス値として使う。最後なら結局nullptrになるので。
			//アドレス値の更新
			(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block->free_next);
			//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
			if ((*l_matchaddress) == 0)
			{
				lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();
		};

		//事後作業
		//分割後サイズ設定
		l_block->BlockSize = _size_;
		l_block->free_prev = nullptr;
		l_block->free_next = nullptr;

		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		std::memset(reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK), 0, _size_);

		//最後に返したる。
		return (reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK));

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

		//0はだめ。
		if (_size_ == 0)
		{
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_TARGETSIZE_ZERO;
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if ((*lp_flg->mp_FirstCategoryBit) == 0)
		{
			m_lock->unlock();
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		if (_size_ < 32)
		{
			_size_ = 32;
		};

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		int32_t l_fcat = SonikMathBit::GetMSB(_size_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_size_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//要求サイズのカテゴリビットを64bit列に纏める
		//uint64_t nowsize_categorybit = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat)) << 32);
		//nowsize_categorybit |= static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_scat));
		//↑を1行にまとめたもの。
		uint64_t nowsize_categorybit = ((static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_fcat) << 32) | (static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_scat);

		//現在のフリーリストの状態(該当行、列)を64bit列に纏める。
		//uint64_t nowfreelist_categorybit = static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32;
		//nowfreelist_categorybit |= lp_flg->mp_SecondCategoryBit[l_fcat];
		//↑を1行にまとめたもの。
		uint64_t nowfreelist_categorybit = (static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32) | lp_flg->mp_SecondCategoryBit[l_fcat];

		//現在と要求のビット列をAND > Count してswitch要素を獲得
		uint16_t l_switch_bitcnt = SonikMathBit::OnBitCount((nowfreelist_categorybit & nowsize_categorybit));

		//0 or 1なら各位置調整処理、2ならそのまま後続処理へ。
		//case 2 はbreakのみだが、無いとdefault分類されてしまう。
		switch (l_switch_bitcnt)
		{
		case 0:
			//ファースト、セカンドカテゴリともにヒットせず。
			//上位のファーストカテゴリを探し、あれば一番近いファーストのセカンドカテゴリ位置を使用する。

			//下位32bitのセカンドカテゴリ区域 + 現在のファーストカテゴリの位置までのビットをすべてOFFに。
			//uint64_t l_tmp = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat) << 32) -1; //対象のファーストカテゴリの位置以下のビットをすべてONに。
			//l_tmp = l_tmp << 1;																	 //1bit左にずらして、1bit目を除いた以降のファーストカテゴリ位置までのビットをONの状態に。
			//l_tmp = l_tmp | 0x01;																	 //1bit目のみ0なのでONに。
			//l_tmp = ~l_tmp;																		 //反転して、要求サイズファーストカテゴリを含む下位ビットをすべてOFFにするマスクを作成
			//l_tmp = (nowfreelist_categorybit & l_tmp) >> 32;										 //マスクを適用し、もともとの32bit列の整数へ。
			//l_fcat = SonikMathBit::GetLSB(l_tmp);													 //最後にLSBを取ると、要求サイズより上位で、かつ一番近いビット位置が取得できる。
			//↑を1行にまとめたもの。
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 1:
			//仕様上ファーストカテゴリのみ立っている状態でHITする可能性がある。
			//同じファーストカテゴリ内の上位に使えるリストのビットが立っていればそこを、なければcase 0 のときと同様の位置を使う。

			//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
			l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
			if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
			{
				break;
			};

			//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
			//以下case0 処理をコピーレフト
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 2:
			//マッチヒット。その位置を使う。
			break;

		default:
			//エラーです。
			m_lock->unlock();
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_UNEXCEPTED;
			return nullptr;

		};


		//位置特定できたのでフリーリスト調整処理
		uintptr_t* l_matchaddress = 0;
		//位置ポインタを取得
		l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//該当場所のポインタから分割処理等を実施する。
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_matchaddress));

		//先頭ポインタ位置からフリーリストを手繰って要求サイズの広さを持ったオブジェクトを探す。
		while (l_block->BlockSize < _size_) //絶対マッチした場所のポインタを拾ってくるのでnullptr参照になることはない。(警告はでるけどね。)
		{
			//ブロックサイズが足りなければ次の要素へ
			l_block = l_block->free_next;
			if (l_block == nullptr)
			{
				//なかったので上位から取る。
				//switch のcase 1処理をコピーレフト
				//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
				l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
				if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
				{
					break;
				};

				//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
				//以下case0 処理をコピーレフト
				l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
				if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
				{
					m_lock->unlock();
					_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
					return nullptr;
				};

				//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
				l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

				//位置ポインタを取得
				l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

				break;
			};

		};


		//違う場合、分割後のサイズによって更に分岐。
		//分割後のサイズが32Byte以上になるなら分割実施(分割要求サイズはヘッダサイズもプラスされる。)
		if ((l_block->BlockSize - (_size_ + sizeof(SLIB_TLSF_MEMBLOCK))) >= 32)
		{
			uint8_t* l_offsetcontrol = reinterpret_cast<uint8_t*>((*l_matchaddress));
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = nullptr;

			//分割先を保存
			l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_offsetcontrol + (sizeof(SLIB_TLSF_MEMBLOCK) + _size_));

			//設定
			//一旦情報全てコピー
			std::memcpy(l_block_split, l_block, sizeof(SLIB_TLSF_FREELISTMEMBLOCK)); //ここもnullptrになることはない。
			//分割居残り側のサイズ設定
			l_block_split->BlockSize -= _size_ + sizeof(SLIB_TLSF_MEMBLOCK);

			//各物理メモリ配列の双方向リストの構築
			l_block_split->phys_prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block); //分割された側はnewのときは左(prev)方向しか操作する必要がない。はず....
			l_block->phys_next = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block_split); //分割した側はnewのときは右(next)方向しか操作する必要がない。はず....


			//分割居残り組のフリーリスト候補先を算出================
			//第１カテゴリを取得
			int32_t l_split_fcat = SonikMathBit::GetMSB(l_block_split->BlockSize) - 1;
			//第２カテゴリを取得
			int32_t l_split_scat = (l_block_split->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット	
			//アクセス先
			uintptr_t* l_split_access_addr = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_split_fcat) + l_split_scat);

			//現在の自分の位置と一緒でなければ処理
			if ((l_fcat != l_split_fcat) || (l_scat != l_split_scat))
			{
				//先に古巣からおさらばする
				if ((*l_matchaddress) == reinterpret_cast<uintptr_t>(l_block))
				{
					//分割前のアドレスがマッチアドレスと一緒なら先頭ポインタなのでそのままnextを代入
					(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split->free_next);
					//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
					if ((*l_matchaddress) == 0)
					{
						lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					};

				}
				else
				{
					//一緒じゃない場合中間からの離脱なので双方向ポインタを操作していい具合にする。
					if (l_block_split->free_prev != nullptr)
					{
						l_block_split->free_prev->free_next = l_block_split->free_next;
					};
					if (l_block_split->free_next != nullptr)
					{
						l_block_split->free_next->free_prev = l_block_split->free_prev;
					};

					//中間なのでフリーリストはまだあるのでビット列は操作しない。
				};

				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				//登録処理
				if ((*l_split_access_addr) == 0)
				{
					//新しい先がnullptr なら登録だけ。
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);

				}
				else
				{
					//nullptr以外ならとりあえず先頭に挿入
					l_block_split->free_prev = nullptr;
					l_block_split->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_split_access_addr));
					l_block_split->free_next->free_prev = l_block_split;
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);
				};

				//新しい登録先のビットを立てる。(立っていても立てる)
				(*lp_flg->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_split_fcat);
				lp_flg->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_split_scat);

			}
			else
			{
				//一緒ならポインタだけつけかえてそのまま。
				if (l_block_split->free_prev != nullptr)
				{
					l_block_split->free_prev->free_next = l_block_split;
				};
				if (l_block_split->free_next != nullptr)
				{
					l_block_split->free_next->free_prev = l_block_split;
				};

				//アドレス値の更新
				(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split);

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();

		}
		else
		{
			//サイズ一致の場合はそのまま貸し出す。
			//nextポインタをアドレス値として使う。最後なら結局nullptrになるので。
			//アドレス値の更新
			(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block->free_next);
			//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
			if ((*l_matchaddress) == 0)
			{
				lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();
		};

		//事後作業
		//分割後サイズ設定
		l_block->BlockSize = _size_;
		l_block->free_prev = nullptr;
		l_block->free_next = nullptr;

		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		std::memset(reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK), 0, _size_);

		//最後に返したる。
		_errcode_ = SLibAllocEnums::EnableRet::ENABLED_OK;
		return (reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK));

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
		};

		//0はだめ。
		if (_size_ == 0)
		{
			throw std::bad_alloc();
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if ((*lp_flg->mp_FirstCategoryBit) == 0)
		{
			m_lock->unlock();
			throw std::bad_alloc();
		};

		//0以外で32よりしたなら32に矯正
		if (_size_ < 32)
		{
			_size_ = 32;
		};

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		int32_t l_fcat = SonikMathBit::GetMSB(_size_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_size_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//要求サイズのカテゴリビットを64bit列に纏める
		//uint64_t nowsize_categorybit = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat)) << 32);
		//nowsize_categorybit |= static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_scat));
		//↑を1行にまとめたもの。
		uint64_t nowsize_categorybit = ((static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_fcat) << 32) | (static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_scat);

		//現在のフリーリストの状態(該当行、列)を64bit列に纏める。
		//uint64_t nowfreelist_categorybit = static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32;
		//nowfreelist_categorybit |= lp_flg->mp_SecondCategoryBit[l_fcat];
		//↑を1行にまとめたもの。
		uint64_t nowfreelist_categorybit = (static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32) | lp_flg->mp_SecondCategoryBit[l_fcat];

		//現在と要求のビット列をAND > Count してswitch要素を獲得
		uint16_t l_switch_bitcnt = SonikMathBit::OnBitCount((nowfreelist_categorybit & nowsize_categorybit));

		//0 or 1なら各位置調整処理、2ならそのまま後続処理へ。
		//case 2 はbreakのみだが、無いとdefault分類されてしまう。
		switch (l_switch_bitcnt)
		{
		case 0:
			//ファースト、セカンドカテゴリともにヒットせず。
			//上位のファーストカテゴリを探し、あれば一番近いファーストのセカンドカテゴリ位置を使用する。

			//下位32bitのセカンドカテゴリ区域 + 現在のファーストカテゴリの位置までのビットをすべてOFFに。
			//uint64_t l_tmp = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat) << 32) -1; //対象のファーストカテゴリの位置以下のビットをすべてONに。
			//l_tmp = l_tmp << 1;																	 //1bit左にずらして、1bit目を除いた以降のファーストカテゴリ位置までのビットをONの状態に。
			//l_tmp = l_tmp | 0x01;																	 //1bit目のみ0なのでONに。
			//l_tmp = ~l_tmp;																		 //反転して、要求サイズファーストカテゴリを含む下位ビットをすべてOFFにするマスクを作成
			//l_tmp = (nowfreelist_categorybit & l_tmp) >> 32;										 //マスクを適用し、もともとの32bit列の整数へ。
			//l_fcat = SonikMathBit::GetLSB(l_tmp);													 //最後にLSBを取ると、要求サイズより上位で、かつ一番近いビット位置が取得できる。
			//↑を1行にまとめたもの。
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				throw std::bad_alloc();
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 1:
			//仕様上ファーストカテゴリのみ立っている状態でHITする可能性がある。
			//同じファーストカテゴリ内の上位に使えるリストのビットが立っていればそこを、なければcase 0 のときと同様の位置を使う。

			//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
			l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
			if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
			{
				break;
			};

			//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
			//以下case0 処理をコピーレフト
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				throw std::bad_alloc();
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 2:
			//マッチヒット。その位置を使う。
			break;

		default:
			//エラーです。
			m_lock->unlock();
			throw std::bad_alloc();

		};


		//位置特定できたのでフリーリスト調整処理
		uintptr_t* l_matchaddress = 0;
		//位置ポインタを取得
		l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//該当場所のポインタから分割処理等を実施する。
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_matchaddress));

		//先頭ポインタ位置からフリーリストを手繰って要求サイズの広さを持ったオブジェクトを探す。
		while (l_block->BlockSize < _size_) //絶対マッチした場所のポインタを拾ってくるのでnullptr参照になることはない。(警告はでるけどね。)
		{
			//ブロックサイズが足りなければ次の要素へ
			l_block = l_block->free_next;
			if (l_block == nullptr)
			{
				//なかったので上位から取る。
				//switch のcase 1処理をコピーレフト
				//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
				l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
				if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
				{
					break;
				};

				//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
				//以下case0 処理をコピーレフト
				l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
				if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
				{
					m_lock->unlock();
					throw std::bad_alloc();
				};

				//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
				l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

				//位置ポインタを取得
				l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

				break;
			};

		};


		//違う場合、分割後のサイズによって更に分岐。
		//分割後のサイズが32Byte以上になるなら分割実施(分割要求サイズはヘッダサイズもプラスされる。)
		if ((l_block->BlockSize - (_size_ + sizeof(SLIB_TLSF_MEMBLOCK))) >= 32)
		{
			uint8_t* l_offsetcontrol = reinterpret_cast<uint8_t*>((*l_matchaddress));
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = nullptr;

			//分割先を保存
			l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_offsetcontrol + (sizeof(SLIB_TLSF_MEMBLOCK) + _size_));

			//設定
			//一旦情報全てコピー
			std::memcpy(l_block_split, l_block, sizeof(SLIB_TLSF_FREELISTMEMBLOCK)); //ここもnullptrになることはない。
			//分割居残り側のサイズ設定
			l_block_split->BlockSize -= _size_ + sizeof(SLIB_TLSF_MEMBLOCK);

			//各物理メモリ配列の双方向リストの構築
			l_block_split->phys_prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block); //分割された側はnewのときは左(prev)方向しか操作する必要がない。はず....
			l_block->phys_next = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block_split); //分割した側はnewのときは右(next)方向しか操作する必要がない。はず....


			//分割居残り組のフリーリスト候補先を算出================
			//第１カテゴリを取得
			int32_t l_split_fcat = SonikMathBit::GetMSB(l_block_split->BlockSize) - 1;
			//第２カテゴリを取得
			int32_t l_split_scat = (l_block_split->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット	
			//アクセス先
			uintptr_t* l_split_access_addr = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_split_fcat) + l_split_scat);

			//現在の自分の位置と一緒でなければ処理
			if ((l_fcat != l_split_fcat) || (l_scat != l_split_scat))
			{
				//先に古巣からおさらばする
				if ((*l_matchaddress) == reinterpret_cast<uintptr_t>(l_block))
				{
					//分割前のアドレスがマッチアドレスと一緒なら先頭ポインタなのでそのままnextを代入
					(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split->free_next);
					//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
					if ((*l_matchaddress) == 0)
					{
						lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					};

				}
				else
				{
					//一緒じゃない場合中間からの離脱なので双方向ポインタを操作していい具合にする。
					if (l_block_split->free_prev != nullptr)
					{
						l_block_split->free_prev->free_next = l_block_split->free_next;
					};
					if (l_block_split->free_next != nullptr)
					{
						l_block_split->free_next->free_prev = l_block_split->free_prev;
					};

					//中間なのでフリーリストはまだあるのでビット列は操作しない。
				};

				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				//登録処理
				if ((*l_split_access_addr) == 0)
				{
					//新しい先がnullptr なら登録だけ。
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);

				}
				else
				{
					//nullptr以外ならとりあえず先頭に挿入
					l_block_split->free_prev = nullptr;
					l_block_split->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_split_access_addr));
					l_block_split->free_next->free_prev = l_block_split;
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);
				};

				//新しい登録先のビットを立てる。(立っていても立てる)
				(*lp_flg->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_split_fcat);
				lp_flg->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_split_scat);

			}
			else
			{
				//一緒ならポインタだけつけかえてそのまま。
				if (l_block_split->free_prev != nullptr)
				{
					l_block_split->free_prev->free_next = l_block_split;
				};
				if (l_block_split->free_next != nullptr)
				{
					l_block_split->free_next->free_prev = l_block_split;
				};

				//アドレス値の更新
				(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split);

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();

		}
		else
		{
			//サイズ一致の場合はそのまま貸し出す。
			//nextポインタをアドレス値として使う。最後なら結局nullptrになるので。
			//アドレス値の更新
			(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block->free_next);
			//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
			if ((*l_matchaddress) == 0)
			{
				lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();
		};

		//事後作業
		//分割後サイズ設定
		l_block->BlockSize = _size_;
		l_block->free_prev = nullptr;
		l_block->free_next = nullptr;

		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		std::memset(reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK), 0, _size_);

		//最後に返したる。
		return (reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK));
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

		uint32_t _size_ = static_cast<uint32_t>((static_cast<uint32_t>(_sizeof_) * _elemcnt_));
		//0はだめ。
		if (_size_ == 0)
		{
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if ((*lp_flg->mp_FirstCategoryBit) == 0)
		{
			m_lock->unlock();
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		if (_size_ < 32)
		{
			_size_ = 32;
		};

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		int32_t l_fcat = SonikMathBit::GetMSB(_size_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_size_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//要求サイズのカテゴリビットを64bit列に纏める
		//uint64_t nowsize_categorybit = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat)) << 32);
		//nowsize_categorybit |= static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_scat));
		//↑を1行にまとめたもの。
		uint64_t nowsize_categorybit = ((static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_fcat) << 32) | (static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_scat);

		//現在のフリーリストの状態(該当行、列)を64bit列に纏める。
		//uint64_t nowfreelist_categorybit = static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32;
		//nowfreelist_categorybit |= lp_flg->mp_SecondCategoryBit[l_fcat];
		//↑を1行にまとめたもの。
		uint64_t nowfreelist_categorybit = (static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32) | lp_flg->mp_SecondCategoryBit[l_fcat];

		//現在と要求のビット列をAND > Count してswitch要素を獲得
		uint16_t l_switch_bitcnt = SonikMathBit::OnBitCount((nowfreelist_categorybit & nowsize_categorybit));

		//0 or 1なら各位置調整処理、2ならそのまま後続処理へ。
		//case 2 はbreakのみだが、無いとdefault分類されてしまう。
		switch (l_switch_bitcnt)
		{
		case 0:
			//ファースト、セカンドカテゴリともにヒットせず。
			//上位のファーストカテゴリを探し、あれば一番近いファーストのセカンドカテゴリ位置を使用する。

			//下位32bitのセカンドカテゴリ区域 + 現在のファーストカテゴリの位置までのビットをすべてOFFに。
			//uint64_t l_tmp = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat) << 32) -1; //対象のファーストカテゴリの位置以下のビットをすべてONに。
			//l_tmp = l_tmp << 1;																	 //1bit左にずらして、1bit目を除いた以降のファーストカテゴリ位置までのビットをONの状態に。
			//l_tmp = l_tmp | 0x01;																	 //1bit目のみ0なのでONに。
			//l_tmp = ~l_tmp;																		 //反転して、要求サイズファーストカテゴリを含む下位ビットをすべてOFFにするマスクを作成
			//l_tmp = (nowfreelist_categorybit & l_tmp) >> 32;										 //マスクを適用し、もともとの32bit列の整数へ。
			//l_fcat = SonikMathBit::GetLSB(l_tmp);													 //最後にLSBを取ると、要求サイズより上位で、かつ一番近いビット位置が取得できる。
			//↑を1行にまとめたもの。
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 1:
			//仕様上ファーストカテゴリのみ立っている状態でHITする可能性がある。
			//同じファーストカテゴリ内の上位に使えるリストのビットが立っていればそこを、なければcase 0 のときと同様の位置を使う。

			//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
			l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
			if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
			{
				break;
			};

			//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
			//以下case0 処理をコピーレフト
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 2:
			//マッチヒット。その位置を使う。
			break;

		default:
			//エラーです。
			m_lock->unlock();
			return nullptr;

		};


		//位置特定できたのでフリーリスト調整処理
		uintptr_t* l_matchaddress = 0;
		//位置ポインタを取得
		l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//該当場所のポインタから分割処理等を実施する。
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_matchaddress));

		//先頭ポインタ位置からフリーリストを手繰って要求サイズの広さを持ったオブジェクトを探す。
		while (l_block->BlockSize < _size_) //絶対マッチした場所のポインタを拾ってくるのでnullptr参照になることはない。(警告はでるけどね。)
		{
			//ブロックサイズが足りなければ次の要素へ
			l_block = l_block->free_next;
			if (l_block == nullptr)
			{
				//なかったので上位から取る。
				//switch のcase 1処理をコピーレフト
				//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
				l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
				if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
				{
					break;
				};

				//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
				//以下case0 処理をコピーレフト
				l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
				if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
				{
					m_lock->unlock();
					return nullptr;
				};

				//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
				l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

				//位置ポインタを取得
				l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

				break;
			};

		};


		//違う場合、分割後のサイズによって更に分岐。
		//分割後のサイズが32Byte以上になるなら分割実施(分割要求サイズはヘッダサイズもプラスされる。)
		if ((l_block->BlockSize - (_size_ + sizeof(SLIB_TLSF_MEMBLOCK))) >= 32)
		{
			uint8_t* l_offsetcontrol = reinterpret_cast<uint8_t*>((*l_matchaddress));
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = nullptr;

			//分割先を保存
			l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_offsetcontrol + (sizeof(SLIB_TLSF_MEMBLOCK) + _size_));

			//設定
			//一旦情報全てコピー
			std::memcpy(l_block_split, l_block, sizeof(SLIB_TLSF_FREELISTMEMBLOCK)); //ここもnullptrになることはない。
			//分割居残り側のサイズ設定
			l_block_split->BlockSize -= _size_ + sizeof(SLIB_TLSF_MEMBLOCK);

			//各物理メモリ配列の双方向リストの構築
			l_block_split->phys_prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block); //分割された側はnewのときは左(prev)方向しか操作する必要がない。はず....
			l_block->phys_next = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block_split); //分割した側はnewのときは右(next)方向しか操作する必要がない。はず....


			//分割居残り組のフリーリスト候補先を算出================
			//第１カテゴリを取得
			int32_t l_split_fcat = SonikMathBit::GetMSB(l_block_split->BlockSize) - 1;
			//第２カテゴリを取得
			int32_t l_split_scat = (l_block_split->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット	
			//アクセス先
			uintptr_t* l_split_access_addr = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_split_fcat) + l_split_scat);

			//現在の自分の位置と一緒でなければ処理
			if ((l_fcat != l_split_fcat) || (l_scat != l_split_scat))
			{
				//先に古巣からおさらばする
				if ((*l_matchaddress) == reinterpret_cast<uintptr_t>(l_block))
				{
					//分割前のアドレスがマッチアドレスと一緒なら先頭ポインタなのでそのままnextを代入
					(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split->free_next);
					//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
					if ((*l_matchaddress) == 0)
					{
						lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					};

				}
				else
				{
					//一緒じゃない場合中間からの離脱なので双方向ポインタを操作していい具合にする。
					if (l_block_split->free_prev != nullptr)
					{
						l_block_split->free_prev->free_next = l_block_split->free_next;
					};
					if (l_block_split->free_next != nullptr)
					{
						l_block_split->free_next->free_prev = l_block_split->free_prev;
					};

					//中間なのでフリーリストはまだあるのでビット列は操作しない。
				};

				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				//登録処理
				if ((*l_split_access_addr) == 0)
				{
					//新しい先がnullptr なら登録だけ。
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);

				}
				else
				{
					//nullptr以外ならとりあえず先頭に挿入
					l_block_split->free_prev = nullptr;
					l_block_split->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_split_access_addr));
					l_block_split->free_next->free_prev = l_block_split;
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);
				};

				//新しい登録先のビットを立てる。(立っていても立てる)
				(*lp_flg->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_split_fcat);
				lp_flg->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_split_scat);

			}
			else
			{
				//一緒ならポインタだけつけかえてそのまま。
				if (l_block_split->free_prev != nullptr)
				{
					l_block_split->free_prev->free_next = l_block_split;
				};
				if (l_block_split->free_next != nullptr)
				{
					l_block_split->free_next->free_prev = l_block_split;
				};

				//アドレス値の更新
				(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split);

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();

		}
		else
		{
			//サイズ一致の場合はそのまま貸し出す。
			//nextポインタをアドレス値として使う。最後なら結局nullptrになるので。
			//アドレス値の更新
			(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block->free_next);
			//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
			if ((*l_matchaddress) == 0)
			{
				lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();
		};

		//事後作業
		//分割後サイズ設定
		l_block->BlockSize = _size_;
		l_block->free_prev = nullptr;
		l_block->free_next = nullptr;

		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		std::memset(reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK), 0, _size_);

		//最後に返したる。
		return (reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK));

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

		uint32_t _size_ = static_cast<uint32_t>((static_cast<uint32_t>(_sizeof_) * _elemcnt_));
		//0はだめ。
		if (_size_ == 0)
		{
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_TARGETSIZE_ZERO;
			return nullptr;
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if ((*lp_flg->mp_FirstCategoryBit) == 0)
		{
			m_lock->unlock();
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
			return nullptr;
		};

		//0以外で32よりしたなら32に矯正
		if (_size_ < 32)
		{
			_size_ = 32;
		};

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		int32_t l_fcat = SonikMathBit::GetMSB(_size_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_size_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//要求サイズのカテゴリビットを64bit列に纏める
		//uint64_t nowsize_categorybit = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat)) << 32);
		//nowsize_categorybit |= static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_scat));
		//↑を1行にまとめたもの。
		uint64_t nowsize_categorybit = ((static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_fcat) << 32) | (static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_scat);

		//現在のフリーリストの状態(該当行、列)を64bit列に纏める。
		//uint64_t nowfreelist_categorybit = static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32;
		//nowfreelist_categorybit |= lp_flg->mp_SecondCategoryBit[l_fcat];
		//↑を1行にまとめたもの。
		uint64_t nowfreelist_categorybit = (static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32) | lp_flg->mp_SecondCategoryBit[l_fcat];

		//現在と要求のビット列をAND > Count してswitch要素を獲得
		uint16_t l_switch_bitcnt = SonikMathBit::OnBitCount((nowfreelist_categorybit & nowsize_categorybit));

		//0 or 1なら各位置調整処理、2ならそのまま後続処理へ。
		//case 2 はbreakのみだが、無いとdefault分類されてしまう。
		switch (l_switch_bitcnt)
		{
		case 0:
			//ファースト、セカンドカテゴリともにヒットせず。
			//上位のファーストカテゴリを探し、あれば一番近いファーストのセカンドカテゴリ位置を使用する。

			//下位32bitのセカンドカテゴリ区域 + 現在のファーストカテゴリの位置までのビットをすべてOFFに。
			//uint64_t l_tmp = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat) << 32) -1; //対象のファーストカテゴリの位置以下のビットをすべてONに。
			//l_tmp = l_tmp << 1;																	 //1bit左にずらして、1bit目を除いた以降のファーストカテゴリ位置までのビットをONの状態に。
			//l_tmp = l_tmp | 0x01;																	 //1bit目のみ0なのでONに。
			//l_tmp = ~l_tmp;																		 //反転して、要求サイズファーストカテゴリを含む下位ビットをすべてOFFにするマスクを作成
			//l_tmp = (nowfreelist_categorybit & l_tmp) >> 32;										 //マスクを適用し、もともとの32bit列の整数へ。
			//l_fcat = SonikMathBit::GetLSB(l_tmp);													 //最後にLSBを取ると、要求サイズより上位で、かつ一番近いビット位置が取得できる。
			//↑を1行にまとめたもの。
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 1:
			//仕様上ファーストカテゴリのみ立っている状態でHITする可能性がある。
			//同じファーストカテゴリ内の上位に使えるリストのビットが立っていればそこを、なければcase 0 のときと同様の位置を使う。

			//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
			l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
			if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
			{
				break;
			};

			//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
			//以下case0 処理をコピーレフト
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
				return nullptr;
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 2:
			//マッチヒット。その位置を使う。
			break;

		default:
			//エラーです。
			m_lock->unlock();
			_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_UNEXCEPTED;
			return nullptr;

		};


		//位置特定できたのでフリーリスト調整処理
		uintptr_t* l_matchaddress = 0;
		//位置ポインタを取得
		l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//該当場所のポインタから分割処理等を実施する。
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_matchaddress));

		//先頭ポインタ位置からフリーリストを手繰って要求サイズの広さを持ったオブジェクトを探す。
		while (l_block->BlockSize < _size_) //絶対マッチした場所のポインタを拾ってくるのでnullptr参照になることはない。(警告はでるけどね。)
		{
			//ブロックサイズが足りなければ次の要素へ
			l_block = l_block->free_next;
			if (l_block == nullptr)
			{
				//なかったので上位から取る。
				//switch のcase 1処理をコピーレフト
				//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
				l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
				if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
				{
					break;
				};

				//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
				//以下case0 処理をコピーレフト
				l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
				if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
				{
					m_lock->unlock();
					_errcode_ = SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE;
					return nullptr;
				};

				//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
				l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

				//位置ポインタを取得
				l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

				break;
			};

		};


		//違う場合、分割後のサイズによって更に分岐。
		//分割後のサイズが32Byte以上になるなら分割実施(分割要求サイズはヘッダサイズもプラスされる。)
		if ((l_block->BlockSize - (_size_ + sizeof(SLIB_TLSF_MEMBLOCK))) >= 32)
		{
			uint8_t* l_offsetcontrol = reinterpret_cast<uint8_t*>((*l_matchaddress));
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = nullptr;

			//分割先を保存
			l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_offsetcontrol + (sizeof(SLIB_TLSF_MEMBLOCK) + _size_));

			//設定
			//一旦情報全てコピー
			std::memcpy(l_block_split, l_block, sizeof(SLIB_TLSF_FREELISTMEMBLOCK)); //ここもnullptrになることはない。
			//分割居残り側のサイズ設定
			l_block_split->BlockSize -= _size_ + sizeof(SLIB_TLSF_MEMBLOCK);

			//各物理メモリ配列の双方向リストの構築
			l_block_split->phys_prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block); //分割された側はnewのときは左(prev)方向しか操作する必要がない。はず....
			l_block->phys_next = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block_split); //分割した側はnewのときは右(next)方向しか操作する必要がない。はず....


			//分割居残り組のフリーリスト候補先を算出================
			//第１カテゴリを取得
			int32_t l_split_fcat = SonikMathBit::GetMSB(l_block_split->BlockSize) - 1;
			//第２カテゴリを取得
			int32_t l_split_scat = (l_block_split->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット	
			//アクセス先
			uintptr_t* l_split_access_addr = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_split_fcat) + l_split_scat);

			//現在の自分の位置と一緒でなければ処理
			if ((l_fcat != l_split_fcat) || (l_scat != l_split_scat))
			{
				//先に古巣からおさらばする
				if ((*l_matchaddress) == reinterpret_cast<uintptr_t>(l_block))
				{
					//分割前のアドレスがマッチアドレスと一緒なら先頭ポインタなのでそのままnextを代入
					(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split->free_next);
					//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
					if ((*l_matchaddress) == 0)
					{
						lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					};

				}
				else
				{
					//一緒じゃない場合中間からの離脱なので双方向ポインタを操作していい具合にする。
					if (l_block_split->free_prev != nullptr)
					{
						l_block_split->free_prev->free_next = l_block_split->free_next;
					};
					if (l_block_split->free_next != nullptr)
					{
						l_block_split->free_next->free_prev = l_block_split->free_prev;
					};

					//中間なのでフリーリストはまだあるのでビット列は操作しない。
				};

				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				//登録処理
				if ((*l_split_access_addr) == 0)
				{
					//新しい先がnullptr なら登録だけ。
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);

				}
				else
				{
					//nullptr以外ならとりあえず先頭に挿入
					l_block_split->free_prev = nullptr;
					l_block_split->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_split_access_addr));
					l_block_split->free_next->free_prev = l_block_split;
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);
				};

				//新しい登録先のビットを立てる。(立っていても立てる)
				(*lp_flg->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_split_fcat);
				lp_flg->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_split_scat);

			}
			else
			{
				//一緒ならポインタだけつけかえてそのまま。
				if (l_block_split->free_prev != nullptr)
				{
					l_block_split->free_prev->free_next = l_block_split;
				};
				if (l_block_split->free_next != nullptr)
				{
					l_block_split->free_next->free_prev = l_block_split;
				};

				//アドレス値の更新
				(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split);

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();

		}
		else
		{
			//サイズ一致の場合はそのまま貸し出す。
			//nextポインタをアドレス値として使う。最後なら結局nullptrになるので。
			//アドレス値の更新
			(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block->free_next);
			//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
			if ((*l_matchaddress) == 0)
			{
				lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();
		};

		//事後作業
		//分割後サイズ設定
		l_block->BlockSize = _size_;
		l_block->free_prev = nullptr;
		l_block->free_next = nullptr;

		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		std::memset(reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK), 0, _size_);

		//最後に返したる。
		_errcode_ = SLibAllocEnums::EnableRet::ENABLED_OK;
		return (reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK));

	};
	
	//例外送出版
	//nullptrで返却する部分をthrow std::bad_allocにしてます。
	//処理自体はmemalArray通常版と同一
	void* TLSFAllocator::memalArray_Exception(size_t _sizeof_, size_t _elemcnt_)
	{
		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			throw std::bad_alloc();
		};

		uint32_t _size_ = static_cast<uint32_t>((static_cast<uint32_t>(_sizeof_) * _elemcnt_));
		//0はだめ。
		if (_size_ == 0)
		{
			throw std::bad_alloc();
		};

		m_lock->lock(); //ロック開始。↑は引数しかみてないのでロック掛ける必要がない。

		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//そもそもファーストのフラグがすべて0ならフリーリストが無い
		if ((*lp_flg->mp_FirstCategoryBit) == 0)
		{
			m_lock->unlock();
			throw std::bad_alloc();
		};

		//0以外で32よりしたなら32に矯正
		if (_size_ < 32)
		{
			_size_ = 32;
		};

		//指定されたサイズのファースト、セカンドカテゴリの抽出
		//第１カテゴリを取得
		int32_t l_fcat = SonikMathBit::GetMSB(_size_) - 1;
		//第２カテゴリを取得(ここで0が来ることはない。)
		uint32_t l_scat = (_size_ >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット

		//要求サイズのカテゴリビットを64bit列に纏める
		//uint64_t nowsize_categorybit = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat)) << 32);
		//nowsize_categorybit |= static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_scat));
		//↑を1行にまとめたもの。
		uint64_t nowsize_categorybit = ((static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_fcat) << 32) | (static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_ONE) << l_scat);

		//現在のフリーリストの状態(該当行、列)を64bit列に纏める。
		//uint64_t nowfreelist_categorybit = static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32;
		//nowfreelist_categorybit |= lp_flg->mp_SecondCategoryBit[l_fcat];
		//↑を1行にまとめたもの。
		uint64_t nowfreelist_categorybit = (static_cast<uint64_t>((*lp_flg->mp_FirstCategoryBit)) << 32) | lp_flg->mp_SecondCategoryBit[l_fcat];

		//現在と要求のビット列をAND > Count してswitch要素を獲得
		uint16_t l_switch_bitcnt = SonikMathBit::OnBitCount((nowfreelist_categorybit & nowsize_categorybit));

		//0 or 1なら各位置調整処理、2ならそのまま後続処理へ。
		//case 2 はbreakのみだが、無いとdefault分類されてしまう。
		switch (l_switch_bitcnt)
		{
		case 0:
			//ファースト、セカンドカテゴリともにヒットせず。
			//上位のファーストカテゴリを探し、あれば一番近いファーストのセカンドカテゴリ位置を使用する。

			//下位32bitのセカンドカテゴリ区域 + 現在のファーストカテゴリの位置までのビットをすべてOFFに。
			//uint64_t l_tmp = (static_cast<uint64_t>((SLIB_TLSF_CONST_BIT_ONE << l_fcat) << 32) -1; //対象のファーストカテゴリの位置以下のビットをすべてONに。
			//l_tmp = l_tmp << 1;																	 //1bit左にずらして、1bit目を除いた以降のファーストカテゴリ位置までのビットをONの状態に。
			//l_tmp = l_tmp | 0x01;																	 //1bit目のみ0なのでONに。
			//l_tmp = ~l_tmp;																		 //反転して、要求サイズファーストカテゴリを含む下位ビットをすべてOFFにするマスクを作成
			//l_tmp = (nowfreelist_categorybit & l_tmp) >> 32;										 //マスクを適用し、もともとの32bit列の整数へ。
			//l_fcat = SonikMathBit::GetLSB(l_tmp);													 //最後にLSBを取ると、要求サイズより上位で、かつ一番近いビット位置が取得できる。
			//↑を1行にまとめたもの。
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				throw std::bad_alloc();
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 1:
			//仕様上ファーストカテゴリのみ立っている状態でHITする可能性がある。
			//同じファーストカテゴリ内の上位に使えるリストのビットが立っていればそこを、なければcase 0 のときと同様の位置を使う。

			//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
			l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
			if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
			{
				break;
			};

			//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
			//以下case0 処理をコピーレフト
			l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
			if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
			{
				m_lock->unlock();
				throw std::bad_alloc();
			};

			//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
			l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

			break;

		case 2:
			//マッチヒット。その位置を使う。
			break;

		default:
			//エラーです。
			m_lock->unlock();
			throw std::bad_alloc();

		};


		//位置特定できたのでフリーリスト調整処理
		uintptr_t* l_matchaddress = 0;
		//位置ポインタを取得
		l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//該当場所のポインタから分割処理等を実施する。
		SLIB_TLSF_FREELISTMEMBLOCK* l_block = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_matchaddress));

		//先頭ポインタ位置からフリーリストを手繰って要求サイズの広さを持ったオブジェクトを探す。
		while (l_block->BlockSize < _size_) //絶対マッチした場所のポインタを拾ってくるのでnullptr参照になることはない。(警告はでるけどね。)
		{
			//ブロックサイズが足りなければ次の要素へ
			l_block = l_block->free_next;
			if (l_block == nullptr)
			{
				//なかったので上位から取る。
				//switch のcase 1処理をコピーレフト
				//上位32bitのファーストカテゴリ区間 + セカンドカテゴリの自分の位置のビットを含めた下のビットをすべてOFFに。
				l_scat = SonikMathBit::GetLSB(nowfreelist_categorybit & static_cast<uint64_t>(SLIB_TLSF_CONST_BIT_32_ONBIT << l_scat)) - 1; //オーバーフロー警告がでるが、わざとなので警告無視。
				if (l_scat > 0) //0より大きい場合はとれてるのでそこを採用。break。
				{
					break;
				};

				//取れてなければcase 0 と同様にファーストカテゴリから上位をさらに探す。
				//以下case0 処理をコピーレフト
				l_fcat = SonikMathBit::GetLSB((nowfreelist_categorybit >> 32) & (~(SLIB_TLSF_CONST_BIT_64_DW32ONBIT >> (31 - l_fcat)))) - 1;
				if (l_fcat <= -1) //-1以下(つまり、0より下ということ。)ならビットなしなので取得可能なフリー領域がない。
				{
					m_lock->unlock();
					throw std::bad_alloc();
				};

				//1以外ならとれてるのでファーストカテゴリを採用、セカンドカテゴリを再設定
				l_scat = SonikMathBit::GetLSB(lp_flg->mp_SecondCategoryBit[l_fcat]) - 1;

				//位置ポインタを取得
				l_matchaddress = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

				break;
			};

		};


		//違う場合、分割後のサイズによって更に分岐。
		//分割後のサイズが32Byte以上になるなら分割実施(分割要求サイズはヘッダサイズもプラスされる。)
		if ((l_block->BlockSize - (_size_ + sizeof(SLIB_TLSF_MEMBLOCK))) >= 32)
		{
			uint8_t* l_offsetcontrol = reinterpret_cast<uint8_t*>((*l_matchaddress));
			SLIB_TLSF_FREELISTMEMBLOCK* l_block_split = nullptr;

			//分割先を保存
			l_block_split = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_offsetcontrol + (sizeof(SLIB_TLSF_MEMBLOCK) + _size_));

			//設定
			//一旦情報全てコピー
			std::memcpy(l_block_split, l_block, sizeof(SLIB_TLSF_FREELISTMEMBLOCK)); //ここもnullptrになることはない。
			//分割居残り側のサイズ設定
			l_block_split->BlockSize -= _size_ + sizeof(SLIB_TLSF_MEMBLOCK);

			//各物理メモリ配列の双方向リストの構築
			l_block_split->phys_prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block); //分割された側はnewのときは左(prev)方向しか操作する必要がない。はず....
			l_block->phys_next = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_block_split); //分割した側はnewのときは右(next)方向しか操作する必要がない。はず....


			//分割居残り組のフリーリスト候補先を算出================
			//第１カテゴリを取得
			int32_t l_split_fcat = SonikMathBit::GetMSB(l_block_split->BlockSize) - 1;
			//第２カテゴリを取得
			int32_t l_split_scat = (l_block_split->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット	
			//アクセス先
			uintptr_t* l_split_access_addr = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_split_fcat) + l_split_scat);

			//現在の自分の位置と一緒でなければ処理
			if ((l_fcat != l_split_fcat) || (l_scat != l_split_scat))
			{
				//先に古巣からおさらばする
				if ((*l_matchaddress) == reinterpret_cast<uintptr_t>(l_block))
				{
					//分割前のアドレスがマッチアドレスと一緒なら先頭ポインタなのでそのままnextを代入
					(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split->free_next);
					//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
					if ((*l_matchaddress) == 0)
					{
						lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					};

				}
				else
				{
					//一緒じゃない場合中間からの離脱なので双方向ポインタを操作していい具合にする。
					if (l_block_split->free_prev != nullptr)
					{
						l_block_split->free_prev->free_next = l_block_split->free_next;
					};
					if (l_block_split->free_next != nullptr)
					{
						l_block_split->free_next->free_prev = l_block_split->free_prev;
					};

					//中間なのでフリーリストはまだあるのでビット列は操作しない。
				};

				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

				//登録処理
				if ((*l_split_access_addr) == 0)
				{
					//新しい先がnullptr なら登録だけ。
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);

				}
				else
				{
					//nullptr以外ならとりあえず先頭に挿入
					l_block_split->free_prev = nullptr;
					l_block_split->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*l_split_access_addr));
					l_block_split->free_next->free_prev = l_block_split;
					(*l_split_access_addr) = reinterpret_cast<uintptr_t>(l_block_split);
				};

				//新しい登録先のビットを立てる。(立っていても立てる)
				(*lp_flg->mp_FirstCategoryBit) |= (SLIB_TLSF_CONST_BIT_ONE << l_split_fcat);
				lp_flg->mp_SecondCategoryBit[l_fcat] |= (SLIB_TLSF_CONST_BIT_ONE << l_split_scat);

			}
			else
			{
				//一緒ならポインタだけつけかえてそのまま。
				if (l_block_split->free_prev != nullptr)
				{
					l_block_split->free_prev->free_next = l_block_split;
				};
				if (l_block_split->free_next != nullptr)
				{
					l_block_split->free_next->free_prev = l_block_split;
				};

				//アドレス値の更新
				(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block_split);

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();

		}
		else
		{
			//サイズ一致の場合はそのまま貸し出す。
			//nextポインタをアドレス値として使う。最後なら結局nullptrになるので。
			//アドレス値の更新
			(*l_matchaddress) = reinterpret_cast<uintptr_t>(l_block->free_next);
			//結果的にnullptrとなった場合はもうリストがないのでビット列のフラグを下ろす。
			if ((*l_matchaddress) == 0)
			{
				lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
				//処理後のセカンドカテゴリビット列が0ならファーストカテゴリのビット列の該当位置も下ろす。
				if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
				{
					(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
				};

			};

			//ここでフリーリストのすべての操作が完了しているため、ロックを解除する。
			//ロック解除前に借用ブロックを使用中に変更しておく。(マルチスレッド対策)
			l_block->IsUse = 1;
			m_lock->unlock();
		};

		//事後作業
		//分割後サイズ設定
		l_block->BlockSize = _size_;
		l_block->free_prev = nullptr;
		l_block->free_next = nullptr;

		//使用可能領域として設定している範囲をサイズ分メモリ初期化
		std::memset(reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK), 0, _size_);

		//最後に返したる。
		return (reinterpret_cast<uint8_t*>(l_block) + sizeof(SLIB_TLSF_MEMBLOCK));

	};

	//フリーの実装
	//マージ処理は後方 > 前方の順にしていく。
	void TLSFAllocator::memdel(void* _pfree_) noexcept
	{
		if (_pfree_ == nullptr)
		{
			return;
		};

		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			return;
		};

		uintptr_t IS_ADDR_begin = reinterpret_cast<uintptr_t>(mp_main_memblock);
		uintptr_t IS_ADDR_end = reinterpret_cast<uintptr_t>((mp_main_memblock + mem_block_size));

		if ((reinterpret_cast<uintptr_t>(_pfree_) < IS_ADDR_begin) && (IS_ADDR_end > reinterpret_cast<uintptr_t>(_pfree_)))
		{
			//範囲外なら強制終了
			return;
		};

		//nullptrチェックは.h側でデストラクタコール前に実施済み。
		SLIB_TLSF_MEMBLOCK* l_now = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(reinterpret_cast<uint8_t*>(_pfree_) - sizeof(SLIB_TLSF_MEMBLOCK));
		//l_freelist_control_target
		SLIB_TLSF_FREELISTMEMBLOCK* l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now);
		//第１カテゴリ
		int32_t l_fcat = 0;
		//第２カテゴリ
		int32_t l_scat = 0;
		//フリーリストのアクセスポイント
		uintptr_t* accesspoint = nullptr;
		//フラグビット列アクセス用。
		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//ゼロクリア範囲サイズ
		uint64_t BlcokZeroClearSize = l_now->BlockSize; //とりえあず自身の範囲はクリア対象なので。

		m_lock->lock();//マルチスレッドロック（最後までフリーリストの更新とかるので全体をブロックすることになる..。

		//使用フラグを下ろしておく。
		//ロックあとに降ろさないとマルチスレッドのときに、下ろした瞬間に誰かロックをとって処理された場合にマージされるおそれもあるので。
		l_now->IsUse = 0;

		//後方マージ
		if (l_now->next != nullptr && !(l_now->next->IsUse)) //後方がいて、未使用状態であれば処理
		{
			//操作用ポインタへ後方ブロックを入れる
			l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now->next);

			//自分のフリーリストと、フリーリストに紐づくビット列を操作する。(フリーリストの状態によってはビット列は操作しない。)
			//まずは所属するフリーリストのアクセスポイントを獲得する。
			//第１カテゴリを取得
			l_fcat = SonikMathBit::GetMSB(l_free_ctt->BlockSize) - 1;
			//第２カテゴリを取得
			l_scat = (l_free_ctt->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット
			//フリーリストのアクセスポイントを取得
			accesspoint = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

			//アクセスポイントと等しいか確認
			if ((*accesspoint) == reinterpret_cast<uintptr_t>(l_free_ctt)) //等しければ先頭ポインタ処理を実施
			{
				//next(先頭より後方)に誰もいないなら、0代入とセカンドカテゴリのビットを下ろす。
				if (l_free_ctt->free_next == nullptr)
				{
					(*accesspoint) = 0;
					lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					//フラグを下げた結果該当セカンドカテゴリ列のビットがALLOFFとなった場合はファーストカテゴリのビットも下ろす。
					if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
					{
						(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
					};


				}
				else //先頭でないのであれば中間なので左右をくっつけて終わり。
				{
					if (l_free_ctt->free_next != nullptr)
					{
						l_free_ctt->free_next->free_prev = l_free_ctt->free_prev;
					};
					if (l_free_ctt->free_prev != nullptr)
					{
						l_free_ctt->free_prev->free_next = l_free_ctt->free_next;
					};
				};

				//物理メモリ配列ポインタを付け替えて、サイズを加算して終了
				l_now->next = l_free_ctt->phys_next;
				if (l_free_ctt->phys_next != nullptr)
				{
					l_free_ctt->phys_next->prev = l_now;
				};
				//マージで空くサイズは前方タグサイズ＋貸出サイズ
				l_now->BlockSize += l_free_ctt->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

				//最終操作用ポインタ更新
				l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now);
				//後方マージしたら、後方の前置タグ分クリアサイズをプラス
				BlcokZeroClearSize += sizeof(SLIB_TLSF_FREELISTMEMBLOCK);

				//後方マージ終わり
			};
		};

		//前方マージ
		if (l_now->prev != nullptr && !(l_now->prev->IsUse)) //前方がいて、未使用状態であれば処理
		{
			//操作用ポインタへ前方ブロックを入れる
			l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now->prev);

			//自分のフリーリストと、フリーリストに紐づくビット列を操作する。(フリーリストの状態によってはビット列は操作しない。)
			//まずは所属するフリーリストのアクセスポイントを獲得する。
			//第１カテゴリを取得
			l_fcat = SonikMathBit::GetMSB(l_free_ctt->BlockSize) - 1;
			//第２カテゴリを取得
			l_scat = (l_free_ctt->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット
			//フリーリストのアクセスポイントを取得
			accesspoint = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

			//アクセスポイントと等しいか確認
			if ((*accesspoint) == reinterpret_cast<uintptr_t>(l_free_ctt)) //等しければ先頭ポインタ処理を実施
			{
				//next(先頭より後方)に誰もいないなら、0代入とセカンドカテゴリのビットを下ろす。
				if (l_free_ctt->free_next == nullptr)
				{
					(*accesspoint) = 0;
					lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					//フラグを下げた結果該当セカンドカテゴリ列のビットがALLOFFとなった場合はファーストカテゴリのビットも下ろす。
					if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
					{
						(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
					};

				}
				else //先頭でないのであれば中間なので左右をくっつけて終わり。
				{
					if (l_free_ctt->free_next != nullptr)
					{
						l_free_ctt->free_next->free_prev = l_free_ctt->free_prev;
					};
					if (l_free_ctt->free_prev != nullptr)
					{
						l_free_ctt->free_prev->free_next = l_free_ctt->free_next;
					};
				};

				//物理メモリ配列ポインタを付け替えて、サイズを加算して終了
				l_free_ctt->phys_next = l_now->next;
				if (l_now->next != nullptr)
				{
					l_now->next->prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_free_ctt);
				};

				//マージで計算サイズは前方タグサイズ+貸出サイズ
				l_free_ctt->BlockSize += l_now->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

				//前方マージしたら、今回デリート分(now)の前置タグの領域もプラス
				BlcokZeroClearSize += sizeof(SLIB_TLSF_MEMBLOCK);

				//変数再利用。前方マージ後はnowのブロックサイズ - SLIB_TLSF_MEMBLOCKの位置から削除。前方ブロックはクリア済みなのでクリアしなくていい。
				_pfree_ = l_now;

				//前方マージ終わり
			};
		};

		//先にメモリの初期化(フリーリストポインタのサイズ分クリアサイズを調整するよこっちに移動のほうが楽だった。
		std::memset(_pfree_, 0, BlcokZeroClearSize);


		//マージしたブロックをフリーリストに再分類し、登録する。
		//第１カテゴリを取得
		l_fcat = SonikMathBit::GetMSB(l_free_ctt->BlockSize) - 1;
		//第２カテゴリを取得
		l_scat = (l_free_ctt->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット
		//分類先のポインタ取得
		accesspoint = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//カテゴリ先に既にフリーリストがあるか？
		if ((*accesspoint) != 0) //すでにリストには先客がいるようだ。
		{
			//先頭に割り込み
			reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*accesspoint))->free_prev = l_free_ctt;
			l_free_ctt->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*accesspoint));
		};

		//先客いなければそのまま登録だし先客いても先頭に登録なのでここは共通処理
		(*accesspoint) = reinterpret_cast<uintptr_t>(l_free_ctt);

		//delete終了
		m_lock->unlock();
	};

	//実施memdelをコールしているだけ。
	//関数コールオーバーヘッド削減のため、処理内容はコピーレフト。
	void TLSFAllocator::memdelArray(void* _pfree_) noexcept
	{
		if (_pfree_ == nullptr)
		{
			return;
		};

		//有効化されていないため強制失敗
		if (m_enabled_state != SLibAllocEnums::EnableRet::ENABLED_OK)
		{
			return;
		};

		uintptr_t IS_ADDR_begin = reinterpret_cast<uintptr_t>(mp_main_memblock);
		uintptr_t IS_ADDR_end = reinterpret_cast<uintptr_t>((mp_main_memblock + mem_block_size));

		if ((reinterpret_cast<uintptr_t>(_pfree_) < IS_ADDR_begin) && (IS_ADDR_end > reinterpret_cast<uintptr_t>(_pfree_)))
		{
			//範囲外なら強制終了
			return;
		};
		//nullptrチェックは.h側でデストラクタコール前に実施済み。
		SLIB_TLSF_MEMBLOCK* l_now = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(reinterpret_cast<uint8_t*>(_pfree_) - sizeof(SLIB_TLSF_MEMBLOCK));
		//l_freelist_control_target
		SLIB_TLSF_FREELISTMEMBLOCK* l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now);
		//第１カテゴリ
		int32_t l_fcat = 0;
		//第２カテゴリ
		int32_t l_scat = 0;
		//フリーリストのアクセスポイント
		uintptr_t* accesspoint = nullptr;
		//フラグビット列アクセス用。
		SLIB_TLSF_BITCOLUMBLOCK* lp_flg = reinterpret_cast<SLIB_TLSF_BITCOLUMBLOCK*>(mp_freelistblock);
		//ゼロクリア範囲サイズ
		uint64_t BlcokZeroClearSize = l_now->BlockSize; //とりえあず自身の範囲はクリア対象なので。

		m_lock->lock();//マルチスレッドロック（最後までフリーリストの更新とかるので全体をブロックすることになる..。

		//使用フラグを下ろしておく。
		//ロックあとに降ろさないとマルチスレッドのときに、下ろした瞬間に誰かロックをとって処理された場合にマージされるおそれもあるので。
		l_now->IsUse = 0;

		//後方マージ
		if (l_now->next != nullptr && !(l_now->next->IsUse)) //後方がいて、未使用状態であれば処理
		{
			//操作用ポインタへ後方ブロックを入れる
			l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now->next);

			//自分のフリーリストと、フリーリストに紐づくビット列を操作する。(フリーリストの状態によってはビット列は操作しない。)
			//まずは所属するフリーリストのアクセスポイントを獲得する。
			//第１カテゴリを取得
			l_fcat = SonikMathBit::GetMSB(l_free_ctt->BlockSize) - 1;
			//第２カテゴリを取得
			l_scat = (l_free_ctt->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット
			//フリーリストのアクセスポイントを取得
			accesspoint = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

			//アクセスポイントと等しいか確認
			if ((*accesspoint) == reinterpret_cast<uintptr_t>(l_free_ctt)) //等しければ先頭ポインタ処理を実施
			{
				//next(先頭より後方)に誰もいないなら、0代入とセカンドカテゴリのビットを下ろす。
				if (l_free_ctt->free_next == nullptr)
				{
					(*accesspoint) = 0;
					lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					//フラグを下げた結果該当セカンドカテゴリ列のビットがALLOFFとなった場合はファーストカテゴリのビットも下ろす。
					if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
					{
						(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
					};


				}
				else //先頭でないのであれば中間なので左右をくっつけて終わり。
				{
					if (l_free_ctt->free_next != nullptr)
					{
						l_free_ctt->free_next->free_prev = l_free_ctt->free_prev;
					};
					if (l_free_ctt->free_prev != nullptr)
					{
						l_free_ctt->free_prev->free_next = l_free_ctt->free_next;
					};
				};

				//物理メモリ配列ポインタを付け替えて、サイズを加算して終了
				l_now->next = l_free_ctt->phys_next;
				if (l_free_ctt->phys_next != nullptr)
				{
					l_free_ctt->phys_next->prev = l_now;
				};
				//マージで空くサイズは前方タグサイズ＋貸出サイズ
				l_now->BlockSize += l_free_ctt->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

				//最終操作用ポインタ更新
				l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now);
				//後方マージしたら、後方の前置タグ分クリアサイズをプラス
				BlcokZeroClearSize += sizeof(SLIB_TLSF_FREELISTMEMBLOCK);

				//後方マージ終わり
			};
		};

		//前方マージ
		if (l_now->prev != nullptr && !(l_now->prev->IsUse)) //前方がいて、未使用状態であれば処理
		{
			//操作用ポインタへ前方ブロックを入れる
			l_free_ctt = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>(l_now->prev);

			//自分のフリーリストと、フリーリストに紐づくビット列を操作する。(フリーリストの状態によってはビット列は操作しない。)
			//まずは所属するフリーリストのアクセスポイントを獲得する。
			//第１カテゴリを取得
			l_fcat = SonikMathBit::GetMSB(l_free_ctt->BlockSize) - 1;
			//第２カテゴリを取得
			l_scat = (l_free_ctt->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット
			//フリーリストのアクセスポイントを取得
			accesspoint = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

			//アクセスポイントと等しいか確認
			if ((*accesspoint) == reinterpret_cast<uintptr_t>(l_free_ctt)) //等しければ先頭ポインタ処理を実施
			{
				//next(先頭より後方)に誰もいないなら、0代入とセカンドカテゴリのビットを下ろす。
				if (l_free_ctt->free_next == nullptr)
				{
					(*accesspoint) = 0;
					lp_flg->mp_SecondCategoryBit[l_fcat] &= (~(SLIB_TLSF_CONST_BIT_ONE << l_scat));
					//フラグを下げた結果該当セカンドカテゴリ列のビットがALLOFFとなった場合はファーストカテゴリのビットも下ろす。
					if (lp_flg->mp_SecondCategoryBit[l_fcat] == 0)
					{
						(*lp_flg->mp_FirstCategoryBit) &= (~(SLIB_TLSF_CONST_BIT_ONE << l_fcat));
					};

				}
				else //先頭でないのであれば中間なので左右をくっつけて終わり。
				{
					if (l_free_ctt->free_next != nullptr)
					{
						l_free_ctt->free_next->free_prev = l_free_ctt->free_prev;
					};
					if (l_free_ctt->free_prev != nullptr)
					{
						l_free_ctt->free_prev->free_next = l_free_ctt->free_next;
					};
				};

				//物理メモリ配列ポインタを付け替えて、サイズを加算して終了
				l_free_ctt->phys_next = l_now->next;
				if (l_now->next != nullptr)
				{
					l_now->next->prev = reinterpret_cast<SLIB_TLSF_MEMBLOCK*>(l_free_ctt);
				};

				//マージで計算サイズは前方タグサイズ+貸出サイズ
				l_free_ctt->BlockSize += l_now->BlockSize + sizeof(SLIB_TLSF_MEMBLOCK);

				//前方マージしたら、今回デリート分(now)の前置タグの領域もプラス
				BlcokZeroClearSize += sizeof(SLIB_TLSF_MEMBLOCK);

				//変数再利用。前方マージ後はnowのブロックサイズ - SLIB_TLSF_MEMBLOCKの位置から削除。前方ブロックはクリア済みなのでクリアしなくていい。
				_pfree_ = l_now;

				//前方マージ終わり
			};
		};

		//先にメモリの初期化(フリーリストポインタのサイズ分クリアサイズを調整するよこっちに移動のほうが楽だった。
		std::memset(_pfree_, 0, BlcokZeroClearSize);


		//マージしたブロックをフリーリストに再分類し、登録する。
		//第１カテゴリを取得
		l_fcat = SonikMathBit::GetMSB(l_free_ctt->BlockSize) - 1;
		//第２カテゴリを取得
		l_scat = (l_free_ctt->BlockSize >> (l_fcat - 5)) & SLIB_TLSF_CONST_BIT_FIVE; //2^5 ということで MSB - 5 オフセット
		//分類先のポインタ取得
		accesspoint = reinterpret_cast<uintptr_t*>(mp_FreelistAreaTop) + ((static_cast<uint64_t>(32) * l_fcat) + l_scat);

		//カテゴリ先に既にフリーリストがあるか？
		if ((*accesspoint) != 0) //すでにリストには先客がいるようだ。
		{
			//先頭に割り込み
			reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*accesspoint))->free_prev = l_free_ctt;
			l_free_ctt->free_next = reinterpret_cast<SLIB_TLSF_FREELISTMEMBLOCK*>((*accesspoint));
		};

		//先客いなければそのまま登録だし先客いても先頭に登録なのでここは共通処理
		(*accesspoint) = reinterpret_cast<uintptr_t>(l_free_ctt);

		//delete終了
		m_lock->unlock();
	};


};//end namespace 


#undef SLIB_TLSF_CONST_BIT_ONE
#undef SLIB_TLSF_CONST_BIT_FIVE
#undef SLIB_TLSF_CONST_BIT_32_ONBIT
#undef SLIB_TLSF_CONST_BIT_64_DW32ONBIT
#undef SLIB_TLSF_CONST_BIT_64_UP32ONBIT
#undef SLIB_TLSF_CONST_BIT_64_ALLBITON