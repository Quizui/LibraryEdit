/*
 * SonikRBTree.h
 *
 *  Created on: 2016/01/27
 *      Author: SONIK
 */

#ifndef SONIKBALANCEDTREE_RB_SONIKRBTREE_H_
#define SONIKBALANCEDTREE_RB_SONIKRBTREE_H_

#if defined(_DEBUG) || defined(DEBUG)
#include <SonikString/SonikString.h>
#endif

#include <utility>
#include <cstdint>
#include <new>
#include <iostream>


#include <SonikCAS/SonikAtomicLock.h>
#include <SmartPointer/SonikSmartPointer.hpp>
#include <CompilersPreProcesser.h>

 //赤黒木
namespace SonikLib
{
	//本体
	template <class Key_T, class Val_T>
	class SonikRBTree
	{
	private:
		//インナー定数
		enum class NODECOLOR : uint8_t
		{
			ND_NIL = 0, //NILNODE
			ND_BLACK,   //NODE_BLACK
			ND_RED,     //NODE_RED
		};

		//インナークラス
		class RBNode
		{
		public:
			Key_T	  m_key;  //検索キー
			Val_T	  m_val;  //検索キーに紐づける値
			RBNode*	  m_LNode;		//自分から左の子へのポインタ
			RBNode*   m_RNode;		//自分から右の子へのポインタ
			RBNode*   m_ParentNode; //自分の親へのポインタ
			NODECOLOR m_color;		//自分の色(NIL, 黒, 赤)

#if defined(_DEBUG) || defined(DEBUG)
			//デバッグ時のみ有効にする。
			uint16_t black_height_cnt;
#endif

		public:
			DEF_FORCE_INLINE RBNode(Key_T _key_, Val_T _val_, RBNode* _left_ = nullptr, RBNode* _right_ = nullptr, RBNode* _parent_ = nullptr, NODECOLOR _nd_color_ = NODECOLOR::ND_RED)
				:m_key(_key_)
				, m_val(_val_)
				, m_LNode(_left_)
				, m_RNode(_right_)
				, m_ParentNode(_parent_)
				, m_color(_nd_color_)
#if defined(_DEBUG) || defined(DEBUG)
				, black_height_cnt(0)
#endif
			{

			};
			DEF_FORCE_INLINE RBNode(RBNode* _left_ = nullptr, RBNode* _right_ = nullptr, RBNode* _parent_ = nullptr, NODECOLOR _nd_color_ = NODECOLOR::ND_RED)
				:m_key()
				, m_val()
				, m_LNode(_left_)
				, m_RNode(_right_)
				, m_ParentNode(_parent_)
				, m_color(_nd_color_)
#if defined(_DEBUG) || defined(DEBUG)
				, black_height_cnt(0)
#endif
			{

			};

			DEF_FORCE_INLINE ~RBNode()
			{
				//各デストラクタコール
				m_key.~Key_T();
				m_val.~Val_T();
			};

			//値を初期化します。(主にNILノードで使います)
			DEF_FORCE_INLINE void ValueInit(void) noexcept
			{
				//m_keyとm_valは型がわからないのでスルー。
				m_LNode = this;
				m_RNode = this;
				m_ParentNode = this;
#if defined(_DEBUG) || defined(DEBUG)
				black_height_cnt = 0;
#endif
			};

			//値を初期化します。
			//すべての方向を指定のノードで初期化します。
			DEF_FORCE_INLINE const void ValueInit(RBNode* _target_) noexcept
			{
				//m_keyとm_valは型がわからないのでスルー。
				m_LNode = _target_;
				m_RNode = _target_;
				m_ParentNode = _target_;
#if defined(_DEBUG) || defined(DEBUG)
				black_height_cnt = 0;
#endif
			};


		}; //end Inner Class RBNode;

	private:
		//フリーリスト
		RBNode* mp_FreeArea;
		//根ノードのポインタ(先頭ノードポインタ)
		RBNode* mp_RootNode;
		//NILノード
		RBNode* mp_NILNode;

		//排他処理ビジーロック
		SonikLib::S_CAS::SonikAtomicLock atmlock_;

		//アロケータ
		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

	private:
	#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
    	//コピーと代入の禁止
		SonikRBTree(const SonikRBTree<Key_T, Val_T>& _copy_) = delete;
		SonikRBTree(SonikRBTree<Key_T, Val_T>&& _move_) = delete;
		SonikRBTree& operator =(const SonikRBTree<Key_T, Val_T>& _copy_) = delete;
		SonikRBTree& operator =(SonikRBTree<Key_T, Val_T>&& _move_) = delete;

   	#else //C++ 11 以下
        //コピーと代入の禁止
		SonikRBTree(const SonikRBTree& _copy_);
		SonikRBTree& operator =(const SonikRBTree& _copy_);

        #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
           	//MSVC2010ならmove可能なので定義だけしておく。
           	SonikRBTree(SonikRBTree&& _copy_);
           	SonikRBTree& operator =(SonikRBTree&& _copy_);

        #endif
    #endif

		//指定されたノードの色値がND_BLACK or ND_NILなら黒としてtrueを返却します。
		DEF_PRE_NO_DISCARD DEF_FORCE_INLINE bool __INNER_IsBLACK__(RBNode* _checknode_) const DEF_POST_NO_DISCARD
		{
			return (_checknode_->m_color == NODECOLOR::ND_BLACK || _checknode_->m_color == NODECOLOR::ND_NIL);
		};

		//指定されたノードの色値がND_REDなら赤としてtrueを返却します。
		DEF_PRE_NO_DISCARD DEF_FORCE_INLINE bool __INNER_IsRED__(RBNode* _checknode_) const DEF_POST_NO_DISCARD
		{
			return (_checknode_->m_color == NODECOLOR::ND_RED);
		};

		// ノード置換を行います。
		DEF_FORCE_INLINE void __INNER_ReplaceSubTree__(RBNode* prevNode, RBNode* nextNode)
		{
			if(prevNode->m_ParentNode == mp_NILNode)
			{
				mp_RootNode = nextNode;
			
			}else if(prevNode == prevNode->m_ParentNode->m_LNode)
			{
				prevNode->m_ParentNode->m_LNode = nextNode;
			
			}else
			{
				prevNode->m_ParentNode->m_RNode = nextNode;
			};

			nextNode->m_ParentNode = prevNode->m_ParentNode;
		}

		//指定位置を基準に右回転します。
		DEF_FORCE_INLINE void __INNER_RIGHT_ROTATE__(RBNode* _rotate_point_)
		{
			RBNode* after_rotate_item = _rotate_point_->m_LNode;

			//rotate point の左の子がもっていた右の子はrotate pointの左の子へつけかえる
			_rotate_point_->m_LNode = after_rotate_item->m_RNode;
			//付け替えた子の親をrotate pointへ付け替える
			if(_rotate_point_->m_LNode != mp_NILNode)
			{
				//NILノード以外なら実行し、NILノードなら何もしない。
				_rotate_point_->m_LNode->m_ParentNode = _rotate_point_;
			};

			//new item の右の子にrotate pointを配置
			after_rotate_item->m_RNode = _rotate_point_;

			//双方の親ポインタを置き換え
			after_rotate_item->m_ParentNode = _rotate_point_->m_ParentNode;
			_rotate_point_->m_ParentNode = after_rotate_item;

			//after_rotate_item の親がNILならルート情報を更新して終了
			if(after_rotate_item->m_ParentNode == mp_NILNode)
			{
				//ここに入った場合rotate pointがRoot(根)だったということなので根情報も置き換える
				mp_RootNode = after_rotate_item;
				
				//終了
				return;
			};

			//NILでなければどちらの子だったかを判別してポインタを付け替える
			if(after_rotate_item->m_ParentNode->m_LNode == _rotate_point_)
			{
				//祖父の左の子だった
				after_rotate_item->m_ParentNode->m_LNode = after_rotate_item;

			}else
			{
				//祖父の右の子だった
				after_rotate_item->m_ParentNode->m_RNode = after_rotate_item;
			}

			//終了
			return;
		};

		//指定位置を基準に左回転します。
		DEF_FORCE_INLINE void __INNER_LEFT_ROTATE__(RBNode* _rotate_point_)
		{
			RBNode* after_rotate_item = _rotate_point_->m_RNode;

			//rotate point の右の子がもっていた左の子はrotate pointの右の子へつけかえる
			_rotate_point_->m_RNode = after_rotate_item->m_LNode;
			//付け替えた子の親をrotate pointへ付け替える
			if (_rotate_point_->m_RNode != mp_NILNode)
			{
				//NILノード以外なら実行し、NILノードなら何もしない。
				_rotate_point_->m_RNode->m_ParentNode = _rotate_point_;
			};

			//new item の左の子にrotate pointを配置
			after_rotate_item->m_LNode = _rotate_point_;

			//双方の親ポインタを置き換え
			after_rotate_item->m_ParentNode = _rotate_point_->m_ParentNode;
			_rotate_point_->m_ParentNode = after_rotate_item;

			//after_rotate_item の親がNILならルート情報を更新して終了
			if (after_rotate_item->m_ParentNode == mp_NILNode)
			{
				//ここに入った場合rotate pointがRoot(根)だったということなので根情報も置き換える
				mp_RootNode = after_rotate_item;

				//終了
				return;
			};

			//NILでなければどちらの子だったかを判別してポインタを付け替える
			if (after_rotate_item->m_ParentNode->m_LNode == _rotate_point_)
			{
				//祖父の左の子だった
				after_rotate_item->m_ParentNode->m_LNode = after_rotate_item;

			}
			else
			{
				//祖父の右の子だった
				after_rotate_item->m_ParentNode->m_RNode = after_rotate_item;
			}

			//終了
			return;
		};

		//削除後の黒高さ補正を実施します。
		DEF_FORCE_INLINE void __INNER_REMOVE_BLACK_HEIGHT_FIXUP__(RBNode* _fixupnode_)
		{
			while(_fixupnode_ != mp_RootNode && __INNER_IsBLACK__(_fixupnode_))
			{
				RBNode* parentNode = _fixupnode_->m_ParentNode;

				// ① 左のサブツリー側か右のサブツリー側かで分岐
				if(_fixupnode_ == parentNode->m_LNode)
				{
					// 左側から呼ばれた → 兄弟は右側
					RBNode* siblingNode = parentNode->m_RNode;

					// CASE1: 兄弟が赤
					if(__INNER_IsRED__(siblingNode))
					{
						siblingNode->m_color = NODECOLOR::ND_BLACK;
						parentNode->m_color = NODECOLOR::ND_RED;
					
						__INNER_LEFT_ROTATE__(parentNode);
						siblingNode = parentNode->m_RNode;
					};

					// CASE2: 兄弟とその両子が黒
					if(    __INNER_IsBLACK__(siblingNode)
						&& __INNER_IsBLACK__(siblingNode->m_LNode)
						&& __INNER_IsBLACK__(siblingNode->m_RNode)
					  )
					{
						siblingNode->m_color = NODECOLOR::ND_RED;
						_fixupnode_ = parentNode;
					
					}else
					{
						// CASE3: 兄弟の右子が黒
						if(__INNER_IsBLACK__(siblingNode->m_RNode))
						{
							siblingNode->m_LNode->m_color = NODECOLOR::ND_BLACK;
							siblingNode->m_color = NODECOLOR::ND_RED;
					
							__INNER_RIGHT_ROTATE__(siblingNode);
							siblingNode = parentNode->m_RNode;
						};

						// CASE4: 兄弟の右子が赤
						siblingNode->m_color = parentNode->m_color;
						parentNode->m_color = NODECOLOR::ND_BLACK;
						siblingNode->m_RNode->m_color = NODECOLOR::ND_BLACK;
						
						__INNER_LEFT_ROTATE__(parentNode);
						_fixupnode_ = mp_RootNode;
					};

				}else
				{
					// 右側から呼ばれた → 兄弟は左側
					RBNode* siblingNode = parentNode->m_LNode;

					if(__INNER_IsRED__(siblingNode))
					{
						siblingNode->m_color = NODECOLOR::ND_BLACK;
						parentNode->m_color = NODECOLOR::ND_RED;

						__INNER_RIGHT_ROTATE__(parentNode);
						siblingNode = parentNode->m_LNode;
					};

					if(    __INNER_IsBLACK__(siblingNode)
						&& __INNER_IsBLACK__(siblingNode->m_LNode)
						&& __INNER_IsBLACK__(siblingNode->m_RNode)
					  )
					{
						siblingNode->m_color = NODECOLOR::ND_RED;
						_fixupnode_ = parentNode;

					}else
					{
						if (__INNER_IsBLACK__(siblingNode->m_LNode)) 
						{
							siblingNode->m_RNode->m_color = NODECOLOR::ND_BLACK;
							siblingNode->m_color = NODECOLOR::ND_RED;

							__INNER_LEFT_ROTATE__(siblingNode);
							siblingNode = parentNode->m_LNode;
						};

						siblingNode->m_color = parentNode->m_color;
						parentNode->m_color = NODECOLOR::ND_BLACK;
						siblingNode->m_LNode->m_color = NODECOLOR::ND_BLACK;

						__INNER_RIGHT_ROTATE__(parentNode);
						_fixupnode_ = mp_RootNode;
					};

				}; //end if-else (_fixupnode_ == parentNode->m_LNode)

			};//end while

			// sentinel 以外のノードは必ず黒に
			if (_fixupnode_ != mp_NILNode)
			{
				_fixupnode_->m_color = NODECOLOR::ND_BLACK;
			};

			//終了
			return;
		};

		//平衡バランスを考慮したインサートを行います。
		DEF_FORCE_INLINE void __INNER_BALANCED_INSERT__(RBNode* _current_)
		{
			RBNode* Current     = _current_;             //挿入或いは、チェック開始位置
			RBNode* Parent      = Current->m_ParentNode; //Currentに対する親
			RBNode* GrandParent = Parent->m_ParentNode;  //Parentに対する親(Currentに対する祖父
			RBNode* Uncle		= (GrandParent->m_LNode == Parent) //GrandParentに対するParent方向とは逆の子(Currentに対するおじ)
								  ? GrandParent->m_RNode
								  : GrandParent->m_LNode;

			//チェックポイントであるCurrentの親が赤である限り違反しているためループ
			while(Parent->m_color == NODECOLOR::ND_RED)
			{
				//ここのループ内の初回処理は常にParentが赤であることが保証される。
				 
				GrandParent = Parent->m_ParentNode;      //Parentに対する親(Currentに対する祖父
				Uncle = (GrandParent->m_LNode == Parent) //GrandParentに対するParent方向とは逆の子(Currentに対するおじ)
						? GrandParent->m_RNode
					    : GrandParent->m_LNode;
				 	 
				//_______________________________________________
				//										         |
				//  1. 以下の場合の処理	       		             |
				//     ・Parent     ：赤                         |
				//	   ・GrandParent：黒				         |
				//     ・Uncle		：赤                         |
				//  ※Uncleが非NILで赤の場合GrandParentは必ず黒  |
				//_______________________________________________|
				if(Uncle->m_color == NODECOLOR::ND_RED) //すべてのポインタ先はNILがいて、NILは実質黒だが色値としてはNIL色が指定されるようになっている。->NODECOLOR::ND_NILがある。
				{
					//Parent、Uncleを黒にする
					Parent->m_color = NODECOLOR::ND_BLACK;
					Uncle->m_color  = NODECOLOR::ND_BLACK;
					//GrandParentを赤にする
					GrandParent->m_color = NODECOLOR::ND_RED;

					//チェックポイント(Current)をGrandParentに昇格し、一つ上の断層のチェックを実施する。
					Current = GrandParent;

					//カレントから初期チェック区間と同じ区間の構成を作成(再設定)する。
					Parent      = Current->m_ParentNode;           //Currentに対する親

					//区間を一つ上に昇格したのでcontinueで再チェック
					continue;
				};

				//此処から先はUncleの色は黒orNILが保証される。(NILも黒として扱うため実質黒確定)

				//_______________________________________________
				//										         |
				//  2. 以下の場合の処理	       		             |
				//     ・Parent     ：赤                         |
				//	   ・GrandParent：黒				         |
				//     ・Uncle		：黒かつGrandParentの右の子  |
				//_______________________________________________|
				if(GrandParent->m_RNode == Uncle)
				{
					//チェックポイント(Current)がParentの内側の子(右側の子)にいる場合先に左回転する。(ここに入ったらParentは祖父の左の子になる)
					if(Parent->m_RNode == Current)
					{
						//ケース2-1:Currentが左サブツリ-内の内側の子として所属している場合
						//左回転
						Current = Parent;
						__INNER_LEFT_ROTATE__(Current);

						Parent      = Current->m_ParentNode;
						GrandParent = Parent->m_ParentNode;
					};

					//ケース2-2:外側にいるので右回転させる。
					//先に色を変更しておく。
					Parent->m_color = NODECOLOR::ND_BLACK;    //Parentの色を黒に設定
					GrandParent->m_color = NODECOLOR::ND_RED; //GrandParentの色を赤に設定
					//右回転
					__INNER_RIGHT_ROTATE__(GrandParent);

					//ループ抜けて終了処理へ。
					break;
				};

				//此処から先はさらにUncoleがGrandParentに対して左の子が保証される。

				//_______________________________________________
				//										         |
				//  3. 以下の場合の処理	       		             |
				//     ・Parent     ：赤                         |
				//	   ・GrandParent：黒				         |
				//     ・Uncle		：黒かつGrandParentの左の子  |
				//_______________________________________________|

				//チェックポイント(Current)がParentの内側の子(左側の子)にいる場合先に右回転する。(ここに入ったらParentは祖父の右の子になる)
				if (Parent->m_LNode == Current)
				{
					//ケース3-1:Currentが右サブツリ-内の内側の子として所属している場合
					//右回転
					Current = Parent;
					__INNER_RIGHT_ROTATE__(Current);

					Parent = Current->m_ParentNode;
					GrandParent = Parent->m_ParentNode;
				};

				//ケース3-2:外側にいるので左回転させる。
				//先に色を変更しておく。
				Parent->m_color      = NODECOLOR::ND_BLACK;    //Parentの色を黒に設定
				GrandParent->m_color = NODECOLOR::ND_RED; //GrandParentの色を赤に設定
				//左回転
				__INNER_LEFT_ROTATE__(GrandParent);

				//ループ抜けて終了処理へ。
				break;

			}; //end while (Parent->m_color == NODECOLOR::ND_RED)

			//ループが終了したらルートの色を黒にする
			mp_RootNode->m_color = NODECOLOR::ND_BLACK;

			//終了
			return;
		};

		//平衡バランスを考慮したリムーヴを行います。
		DEF_FORCE_INLINE void __INNER_BALANCED_REMOVE__(RBNode* _remove_node_)
		{
			NODECOLOR originalColor = _remove_node_->m_color;
			RBNode* fixupNode = mp_NILNode;

			// A: 左子が NIL
			if(_remove_node_->m_LNode == mp_NILNode)
			{
				fixupNode = _remove_node_->m_RNode;
				__INNER_ReplaceSubTree__(_remove_node_, _remove_node_->m_RNode);
			
			}else if(_remove_node_->m_RNode == mp_NILNode)
			{
				// B: 右子が NIL

				fixupNode = _remove_node_->m_LNode;
				__INNER_ReplaceSubTree__(_remove_node_, _remove_node_->m_LNode);
			
			}else
			{
				// C: 両子あり → in-order 後続ノード(replacementNode) を探す

				RBNode* replacementNode = _remove_node_->m_RNode;
				
				while(replacementNode->m_LNode != mp_NILNode)
				{
					replacementNode = replacementNode->m_LNode;
				};

				originalColor = replacementNode->m_color;
				fixupNode = replacementNode->m_RNode;

				if(replacementNode->m_ParentNode != _remove_node_)
				{
					__INNER_ReplaceSubTree__(replacementNode, replacementNode->m_RNode);
					replacementNode->m_RNode = _remove_node_->m_RNode;
					replacementNode->m_RNode->m_ParentNode = replacementNode;

				}else
				{
					//y が z の直接右子だった場合、
					//削除後の x (= y->m_RNode) の親を y に直しておく
					fixupNode->m_ParentNode = replacementNode;
				};

				__INNER_ReplaceSubTree__(_remove_node_, replacementNode);
				replacementNode->m_LNode = _remove_node_->m_LNode;
				replacementNode->m_LNode->m_ParentNode = replacementNode;
				replacementNode->m_color = _remove_node_->m_color;
			};

			// _remove_node_ をフリーリストへ戻す
			_remove_node_->ValueInit(mp_NILNode);
			_remove_node_->m_RNode = mp_FreeArea;
			mp_FreeArea->m_LNode = _remove_node_;
			mp_FreeArea = _remove_node_;

			// 黒ノードを削除したときだけ fixup
			if (originalColor == NODECOLOR::ND_BLACK)
			{
				__INNER_REMOVE_BLACK_HEIGHT_FIXUP__(fixupNode);
			};

			//終了
			return;

		};


#if defined(_DEBUG) || defined(DEBUG)
		// ─────────────────────────────────────────────────
		// debugPrintTree: 木構造を自動インデント付きで出力
		// ─────────────────────────────────────────────────
		DEF_FORCE_INLINE void __INNER_DebugPrintTree__(RBNode* node, int depth = 0) const
		{
			if(node == mp_NILNode)
			{
				return;
			};

			// 右部分木を先に（上に描かれるイメージ）
			__INNER_DebugPrintTree__(node->m_RNode, depth + 1);

			// 自分の行：depth 個の“\t”でインデント → key(color)
			for (int i = 0; i < depth; ++i)
			{
				std::cout << "\t";
			};
			
			char c = (node->m_color == NODECOLOR::ND_RED)
					  ? 'R'
					  : (node->m_color == NODECOLOR::ND_BLACK)
					    ? 'B'
					    : 'N';
			
			std::cout << node->m_key << "(" << c << ")" << "\n";

			// 左部分木
			__INNER_DebugPrintTree__(node->m_LNode, depth + 1);

			//終了
			return;
		};

		//ツリーの不変状態チェック
		DEF_FORCE_INLINE void __BALANCED_TREE_CHECK(void)
		{
			//ルートノードから固定ですべてを見ていく。
			if (mp_RootNode == nullptr || mp_RootNode == mp_NILNode)
			{
				//ルート(根)がnullptr or NILならチェック不要
				return;
			};

			SonikString debug_str = "";

			//ルートが黒以外なら違反とみなす。
			if (mp_RootNode->m_color != NODECOLOR::ND_BLACK)
			{
				debug_str = "違反を検知\n";
				debug_str += "Root Color Not Black ：ルートの色が黒以外の色で違反しています。\n";

				//MessageBoxW(nullptr, debug_str.str_wchar(), L"検知", MB_OK);

				return;
			};

			//Curr/Last法で走査
			RBNode* Current = mp_RootNode; //次にたどるノード
			RBNode* Last	= mp_NILNode;  //直前に処理したノード

			while (Current != mp_NILNode)
			{
				RBNode* Parent = Current->m_ParentNode;

				// ▼ 降りがけのチェック（pre-order 相当）
				if (Last == Parent) //直前に処理したノードと親ノードが一緒であれば処理
				{
					// 初回訪問なので一度だけクリア
					Current->black_height_cnt = 0;

					if (   Current->m_LNode != mp_NILNode && Current->m_LNode->m_ParentNode != Current
						|| Current->m_RNode != mp_NILNode && Current->m_RNode->m_ParentNode != Current
						)
					{
						//親子のポインタに齟齬があった場合IN
						debug_str = "違反を検知\n";
						debug_str += "Parent ⇔ Child UnMatched Link Pointer ：親から子、子から親へのポインタに相違があります。\n";

						//MessageBoxW(nullptr, debug_str.str_wchar(), L"検知", MB_OK);

						return;
					};

					if (Current->m_color == NODECOLOR::ND_RED)
					{
						if (   Current->m_LNode != mp_NILNode && Current->m_LNode->m_color == NODECOLOR::ND_RED
							|| Current->m_RNode != mp_NILNode && Current->m_RNode->m_color == NODECOLOR::ND_RED
							)
						{
							//親子が赤赤違反している場合IN
							debug_str = "違反を検知\n";
							debug_str += "Parent→Child Red→Red ：赤赤連続違反をしています。\n";

							//MessageBoxW(nullptr, debug_str.str_wchar(), L"検知", MB_OK);
							return;
						};
					};

					//現在見ているノードの左の子がNILでないならそちらへ進み、現在位置をLastに保持させてcontinue
					if (Current->m_LNode != mp_NILNode)
					{
						Last = Current;
						Current = Current->m_LNode;
						continue;
					};

					//ここまで来たら現在見ているノードの左の子はNIL

					//現在見ているノードの右の子がNILでないならそちらへ進み、現在位置をLastに保持させてcontinue
					if (Current->m_RNode != mp_NILNode)
					{
						Last = Current;
						Current = Current->m_RNode;
						continue;
					}

					// 両子なし → ポスト処理へ
				
				}else if (Last == Current->m_LNode) //直前に処理したノードとCurrentの親が一緒では無く、直前に処理したノードと次にたどるノードの左の子が一緒の時
				{
					//右の子がNIL以外ならIN
					if (Current->m_RNode != mp_NILNode)
					{
						Last = Current;					//直前に処理したノードを今処理中のノードに更新
						Current = Current->m_RNode;     //次にたどる(処理する)ノードは右の子に更新
						continue;
					}

					// 右もない → ポスト処理へ
				};


				// ▼ ポストオーダー処理
				uint16_t lb = (Current->m_LNode == mp_NILNode) ? 1 : Current->m_LNode->black_height_cnt;
				uint16_t rb = (Current->m_RNode == mp_NILNode) ? 1 : Current->m_RNode->black_height_cnt;
				if (lb != rb)
				{
					//黒高さ違反
					debug_str = "違反を検知\n";
					debug_str += "Black_Height_Vioration ：黒高さ違反をしています。\n";
					debug_str += "回転とかミスってる可能性があります。\n";

					//MessageBoxW(nullptr, debug_str.str_wchar(), L"検知", MB_OK);

					printf("黒高さ違反 at key=%d: 左=%u, 右=%u\n",
						Current->m_key, lb, rb);

					return;
				};

				//確認対象が黒ならカウントアップ
				Current->black_height_cnt = lb + ((Current->m_color == NODECOLOR::ND_BLACK) ? 1 : 0);

				// 親へ戻る
				Last = Current;
				Current = Parent;

			}; //end while

			//正常終了
			return;

		};

#endif

	public:
		//コンストラクタ
		SonikRBTree(uint32_t _initElemCnt_ = 100) //初期100個のノードまでのバッファを取る。
		:mp_FreeArea(nullptr)
		,mp_RootNode(nullptr)
		,mp_NILNode(nullptr)
		{

			SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
			
			try
			{
				//デフォルトアロケータ生成
				l_defalloc = new SonikLib::SLibAllocateInterface;
				//スマポへ設定
				if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
				{
					delete l_defalloc;
					l_defalloc = nullptr;
					throw std::bad_alloc();
				};

				//NILNodeの生成
				void* l_allocbuffer = l_defalloc->memal(sizeof(RBNode));
				if (l_allocbuffer == nullptr)
				{
					throw std::bad_alloc();
				};
				mp_NILNode = new(l_allocbuffer) RBNode();
				mp_NILNode->ValueInit();
				mp_NILNode->m_color = NODECOLOR::ND_NIL;
				mp_FreeArea = mp_NILNode;
				mp_RootNode = mp_NILNode;

				//指定された要素バッファを指定個数分作成
				for(uint32_t i = 0; i < _initElemCnt_; ++i)
				{
					//フリーリストは新規をL側に置き、R側に旧をつなげていく。
					l_allocbuffer = l_defalloc->memal(sizeof(RBNode));
					if (l_allocbuffer == nullptr)
					{
						throw std::bad_alloc();
					};

					mp_FreeArea = new(l_allocbuffer) RBNode(mp_NILNode, mp_FreeArea);

				};


			} catch (std::bad_alloc&)
			{
				if (l_defalloc != nullptr)
				{
					//フリーリストの開放
					RBNode* ControlN;
					while (mp_FreeArea != mp_NILNode)
					{
						ControlN = mp_FreeArea;
						mp_FreeArea = mp_FreeArea->m_RNode;

						ControlN->~RBNode();
						l_defalloc->memdel(ControlN);
					};

					//最後にNILノード開放
					mp_NILNode->~RBNode();
					l_defalloc->memdel(mp_NILNode);

					//l_defallocはnullptrになっていなければスマートポインタに入っているのでここでは明示的にdeleteしない。
				};

				throw;
			};

			//初期化完了
		};
		SonikRBTree(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, uint32_t _initElemCnt_ = 100) //初期100個のノードまでのバッファを取る。
		:mp_FreeArea(nullptr)
		,mp_RootNode(nullptr)
		,mp_NILNode(nullptr)
		,m_allocator(_allocator_)
		{
			try
			{
				//NILNodeの生成
				void* l_allocbuffer = _allocator_->memal(sizeof(RBNode));
				if (l_allocbuffer == nullptr)
				{
					throw std::bad_alloc();
				};
				mp_NILNode = new(l_allocbuffer) RBNode();
				mp_NILNode->ValueInit();
				mp_NILNode->m_color = NODECOLOR::ND_NIL;
				mp_FreeArea = mp_NILNode;
				mp_RootNode = mp_NILNode;

				//指定された要素バッファを指定個数分作成
				for (uint32_t i = 0; i < _initElemCnt_; ++i)
				{
					//フリーリストは新規をL側に置き、R側に旧をつなげていく。
					l_allocbuffer = _allocator_->memal(sizeof(RBNode));
					if (l_allocbuffer == nullptr)
					{
						throw std::bad_alloc();
					};

					mp_FreeArea = new(l_allocbuffer) RBNode(mp_NILNode, mp_FreeArea);

				};


			}catch (std::bad_alloc&)
			{
				//フリーリストの開放
				RBNode* ControlN;
				while (mp_FreeArea != mp_NILNode)
				{
					ControlN = mp_FreeArea;
					mp_FreeArea = mp_FreeArea->m_RNode;

					ControlN->~RBNode();
					_allocator_->memdel(ControlN);
				};

				//最後にNILノード開放
				mp_NILNode->~RBNode();
				_allocator_->memdel(mp_NILNode);

				throw;
			};

			//初期化完了
		};

		//デストラクタ
		~SonikRBTree(void) noexcept
		{
			RBNode* ControlCurrent = nullptr;
			RBNode* ControlLast = nullptr;

			//使用ツリーの開放処理
			if (mp_RootNode != nullptr)
			{
				ControlCurrent = mp_RootNode; //次にたどるノード
				ControlLast = mp_NILNode;     //直前に処理したノード

				while(ControlCurrent != mp_NILNode)
				{
					//親から降りてきた直後
					if (ControlLast == ControlCurrent->m_ParentNode)
					{
						if(ControlCurrent->m_LNode != mp_NILNode)
						{
							ControlLast = ControlCurrent;
							ControlCurrent = ControlCurrent->m_LNode;
							
							continue;
						};

						if(ControlCurrent->m_RNode != mp_NILNode)
						{
							ControlLast = ControlCurrent;
							ControlCurrent = ControlCurrent->m_RNode;
							
							continue;
						};

						//左右両方の子が存在しないなら削除へ
					
					//左を戻ってきた直後
					}else if (ControlLast == ControlCurrent->m_LNode)
					{
						if(ControlCurrent->m_RNode != mp_NILNode)
						{
							ControlLast = ControlCurrent;
							ControlCurrent = ControlCurrent->m_RNode;
							
							continue;
						};

						//右の子も存在しないなら削除へ
					};
					
					//右を戻ってきた直後
					//ここまできたら左右とも済んでいるので削除フェーズ

					RBNode* parent = ControlCurrent->m_ParentNode;
					//親とのリンク切り離し
					if(parent != mp_NILNode)
					{
						if (parent->m_LNode == ControlCurrent)
						{
							parent->m_LNode = mp_NILNode;

						}
						else
						{
							parent->m_RNode = mp_NILNode;

						};
					};
					
					m_allocator->memdel(ControlCurrent);
					ControlLast = ControlCurrent;
					ControlCurrent = parent; //親へ戻る。

				}//end while (ControlCurrent != mp_NILNode)

			};//end if (mp_RootNode != nullptr)


			//フリーリストの開放処理
			while (mp_FreeArea != mp_NILNode)
			{
				ControlCurrent = mp_FreeArea;
				mp_FreeArea = mp_FreeArea->m_RNode;

				ControlCurrent->~RBNode();
				m_allocator->memdel(ControlCurrent);
			};

			//NILノードの開放処理
			if (mp_NILNode != nullptr)
			{
				mp_NILNode->~RBNode();
				m_allocator->memdel(mp_NILNode);
			};


			//明示的にデストラクタが呼べてしまうため、その場合を考慮してnullptrで再初期化しておく。
			mp_RootNode = nullptr;
			mp_FreeArea = nullptr;
			mp_NILNode  = nullptr;

		};

		//インサート
		bool Insert(Key_T _key_, Val_T _val_)
		{
			//ロック
			atmlock_.lock();

			//_______________________________________________________________
			//
			//挿入位置の検索
			//_______________________________________________________________
			//ルートに要素がない場合はルートに根として追加
			if (mp_RootNode == mp_NILNode)
			{
				//NILNodeであれば無いので、初回インサート
				//フリーリストから取得
				mp_RootNode = mp_FreeArea;
				mp_FreeArea = mp_FreeArea->m_RNode;
				
				//フリーリスト先頭を使用済みとして取得したため、次のフリーエリアノードの左をNILに置き換える。(このままだとRootNodeにアクセスできてしまうため)
				mp_FreeArea->m_LNode = mp_NILNode;

				//フリーリストオブジェクトの値初期化
				mp_RootNode->~RBNode();
				std::memset(mp_RootNode, 0, sizeof(RBNode));
				//領域を再利用してオブジェクトを新規生成
				new(reinterpret_cast<void*>(mp_RootNode)) RBNode(_key_, _val_, mp_NILNode, mp_NILNode, mp_NILNode, NODECOLOR::ND_BLACK);

				//終わり
				atmlock_.unlock();
				return true;
			};

			//ルートに要素がある場合、挿入場所を検索
			//ルートから辿っていく
			RBNode* l_InsertPoint = mp_RootNode;
			RBNode* l_parent = nullptr;
			uint8_t l_insert_dir = 0; //挿入方向 0:左 1:右
			//NILNode見つかるまで再帰的に実行
			while (l_InsertPoint != mp_NILNode)
			{
				//Keyに同値がある場合は挿入不可とする。
				if (l_InsertPoint->m_key == _key_)
				{
					atmlock_.unlock();
					return false;
				};

				l_parent = l_InsertPoint;
				l_InsertPoint = (_key_ < l_InsertPoint->m_key)
								? (l_insert_dir = 0, l_InsertPoint->m_LNode)  //真の場合は左ルート
								: (l_insert_dir = 1, l_InsertPoint->m_RNode); //偽の場合は右ルート

			};

			//ループから抜けたら挿入箇所のNILにいるので一つ上の親にもどる。
			l_InsertPoint = l_parent;
			

			//_______________________________________________________________
			//
			//新規オブジェクトの作成と挿入位置へのオブジェクトの挿入
			//_______________________________________________________________
			//フリーエリアから新しいオブジェクトを取得
			RBNode* N_Obj = nullptr;
			if (mp_FreeArea == mp_NILNode)
			{
				//フリーエリアにバッファが不足している場合はアロケータから追加取得
				void* l_allocbuffer = m_allocator->memal(sizeof(RBNode));
				if (l_allocbuffer == nullptr)
				{
					//アロケータから新しい領域取得ができなかった場合は追加できないので終了
					atmlock_.unlock();
					return false;
				};

				N_Obj = reinterpret_cast<RBNode*>(l_allocbuffer);

			}else
			{
				//フリーエリアにバッファがある場合はフリーエリアから拝借
				N_Obj = mp_FreeArea;
				mp_FreeArea = mp_FreeArea->m_RNode;
				mp_FreeArea->m_LNode = mp_NILNode;

				//オブジェクト領域初期化
				N_Obj->~RBNode();
				std::memset(N_Obj, 0, sizeof(RBNode));

			};

			//フラグを見てそっちの方向にノードを追加
			RBNode*& InsertDir = (l_insert_dir == 0)
								 ? l_InsertPoint->m_LNode  //左右フラグが0なら左に挿入
								 : l_InsertPoint->m_RNode; //左右フラグが0以外なら右に挿入
			
			//該当の方向に該当のフラグビット列でオブジェクトを追加
			InsertDir = new(N_Obj) RBNode(_key_, _val_, mp_NILNode, mp_NILNode, l_InsertPoint, NODECOLOR::ND_RED);

			//_______________________________________________________________
			//
			//色チェックを行い、バランス処理の実施を行う。
			//_______________________________________________________________

			//バランスチェックはインナー関数に内容を記載
			__INNER_BALANCED_INSERT__(N_Obj);

#if defined(_DEBUG) || defined(DEBUG)

			//デバッグ時インサート構造チェック
			//挿入後の違反をしていないか？
			__BALANCED_TREE_CHECK();
#endif

			atmlock_.unlock();
			return true;
		};

		//リム－ヴ
		void Remove(Key_T _key_)
		{
			//ロック
			atmlock_.lock();

			RBNode* l_RemoveItem = nullptr;
			RBNode* l_tmp = mp_RootNode;
			
			//削除対象検索(Find)
			while(l_tmp != mp_NILNode)
			{
				if (l_tmp->m_key == _key_)
				{
					l_RemoveItem = l_tmp;
					break;
				};

				l_tmp = (_key_ < l_tmp->m_key) ? l_tmp->m_LNode : l_tmp->m_RNode;
			};

			if (l_RemoveItem == nullptr)
			{
				atmlock_.unlock();
				return;
			};

			__INNER_BALANCED_REMOVE__(l_RemoveItem);

#if defined(_DEBUG) || defined(DEBUG)

			//デバッグ時Remove後構造チェック
			//削除後の違反をしていないか？
			__BALANCED_TREE_CHECK();
#endif

			atmlock_.unlock();
			return;
		};

		//要素の検索(Find)
		//_key_で検索した結果の値を、_val_に入れます。
		//_key_がない場合はfalse, ある場合はtrueを返却します。
		//またFind中構造が変化するとクラッシュの可能性があるためInsert, Remove同様にlockがかかります。
		bool Find(const Key_T& _key_, Val_T& _val_)
		{
			//lock
			atmlock_.lock();

			//find
			RBNode* l_tmp = mp_RootNode;

			while (l_tmp != mp_NILNode)
			{
				if (l_tmp->m_key == _key_)
				{
					_val_ = l_tmp->m_val;

					//unlock
					atmlock_.unlock();
					return true;
				};

				l_tmp = (_key_ < l_tmp->m_key) ? l_tmp->m_LNode : l_tmp->m_RNode;
			};


			//unlock
			atmlock_.unlock();

			//値がなかった。
			return false;

		};


#if defined(_DEBUG) || defined(DEBUG)
		//ツリー状態を横向きに出力します。
		void DebugPrintTree(void)
		{
			__INNER_DebugPrintTree__(mp_RootNode);
		};
#endif


	};//end class;



};//end namespace SonikLib



#endif /* SONIKBALANCEDTREE_RB_SONIKRBTREE_H_ */