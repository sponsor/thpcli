#include "ext.h"
#include "main.h"

BOOL PacketProc_Room(BYTE *data)
{
	BOOL ret = FALSE;
	switch (data[3])
	{
	case PK_USER_ROOMINFO_IN:
		{
			int nNumUsers = min(g_pGame->GetMaxLoginNum(), data[4]);
			int nIndex = 5;
			g_pCriticalSection->EnterCriticalSection_Session(L'E');
			for (int i=0;i<nNumUsers;i++)
			{
				type_session sess;
				ZeroMemory(&sess, sizeof(type_session));
				// キャラNo
				sess.sess_index = (BYTE)data[nIndex];
				sess.obj_no = sess.sess_index;
				nIndex++;
				// オブジェクトタイプ
				sess.obj_type = OBJ_TYPE_CHARA;
				// キャラタイプ
				sess.chara_type = data[nIndex];
				nIndex++;
				// ユーザ名長
				sess.name_len = data[nIndex];
				// ユーザ名範囲オーバー
				if (sess.name_len > MAX_USER_NAME*sizeof(WCHAR) || data[nIndex] == NULL)
				{
					g_pCriticalSection->LeaveCriticalSection_Session();
					DXTRACE_MSG(L"PacketProc_RoomInfo:ユーザ名範囲オーバー");
					return FALSE;
				}
				nIndex++;
				// ユーザ名
				SafeMemCopy(sess.name, &data[nIndex], sess.name_len, MAX_USER_NAME*sizeof(WCHAR));
				nIndex += sess.name_len;
				// 状態
				memcpy(&sess.obj_state, (BYTE*)&data[nIndex], sizeof(E_TYPE_OBJ_STATE));
				nIndex+= sizeof(E_TYPE_OBJ_STATE);
				// マスター状態
				sess.master = data[nIndex];
				nIndex++;
				// チームNo
				sess.team_no = data[nIndex];
				nIndex++;
				// 準備OK
				sess.game_ready = data[nIndex];
				nIndex++;
				// 座標値X
				memcpy(&sess.lx, (BYTE*)&data[nIndex], sizeof(short));
				nIndex+= sizeof(short);
				// 座標値Y
				memcpy(&sess.ly, (BYTE*)&data[nIndex], sizeof(short));
				nIndex+= sizeof(short);
				// 移動値X
				memcpy(&sess.vx, (BYTE*)&data[nIndex], sizeof(short));
				nIndex+= sizeof(short);
				// 移動値Y
				memcpy(&sess.vy, (BYTE*)&data[nIndex], sizeof(short));
				nIndex+= sizeof(short);
				// 方向
				sess.dir = (E_TYPE_USER_DIRECTION)data[nIndex];
				nIndex++;
				sess.entity = 1;
				memcpy(&sess.cost, (BYTE*)&data[nIndex], sizeof(short));
				nIndex+= sizeof(short);

				sess.connect_state = CONN_STATE_AUTHED;
				// キャラ情報設定
				g_pGame->SetNewUser(&sess, FALSE);
				// 準備状態を設定
				g_pGame->SetGameReady(sess.sess_index, sess.game_ready);
			}
			// 部屋情報の有効状態更新
			g_pGame->UpdateGameMasterPropertyControlsEnabled();
			g_pGame->UpdateReadyButtonState();
			g_pGame->UpdatePropertyControlsEnabled();
//			ZeroMemory(g_pGame->GetMySessionInfo()->items, sizeof(DWORD)*GAME_ITEM_STOCK_MAX_COUNT);
			g_pGame->UpdateMyItems();
			g_pCriticalSection->LeaveCriticalSection_Session();
		}
		break;
	case PK_USER_ROOMINFO_NEW:
		{
			int nIndex = 4;
			g_pCriticalSection->EnterCriticalSection_Session(L'R');
			type_session sess;
			ZeroMemory(&sess, sizeof(type_session));
			// キャラNo
			sess.sess_index = (BYTE)data[nIndex];
			sess.obj_no = sess.sess_index;
			nIndex++;
			// オブジェクトタイプ
			sess.obj_type = OBJ_TYPE_CHARA;
			// キャラタイプ
			sess.chara_type = data[nIndex];
			nIndex++;
			// ユーザ名長
			sess.name_len = data[nIndex];
			// ユーザ名範囲オーバー
			if (sess.name_len > MAX_USER_NAME*sizeof(WCHAR) || data[nIndex] == NULL)
			{
				g_pCriticalSection->LeaveCriticalSection_Session();
				DXTRACE_MSG(L"PacketProc_RoomInfo:ユーザ名範囲オーバー");
				return FALSE;
			}
			nIndex++;
			// ユーザ名
			SafeMemCopy(sess.name, &data[nIndex], sess.name_len, MAX_USER_NAME*sizeof(WCHAR));
			nIndex += sess.name_len;
			// 状態
			memcpy(&sess.obj_state, (BYTE*)&data[nIndex], sizeof(E_TYPE_OBJ_STATE));
			nIndex+= sizeof(E_TYPE_OBJ_STATE);
			// マスター状態
			sess.master = data[nIndex];
			nIndex++;
			// チームNo
			sess.team_no = data[nIndex];
			nIndex++;
			// 準備OK
			sess.game_ready = data[nIndex];
			nIndex++;
			// 座標値X
			memcpy(&sess.lx, (BYTE*)&data[nIndex], sizeof(short));
			nIndex+= sizeof(short);
			// 座標値Y
			memcpy(&sess.ly, (BYTE*)&data[nIndex], sizeof(short));
			nIndex+= sizeof(short);
			// 移動値X
			memcpy(&sess.vx, (BYTE*)&data[nIndex], sizeof(short));
			nIndex+= sizeof(short);
			// 移動値Y
			memcpy(&sess.vy, (BYTE*)&data[nIndex], sizeof(short));
			nIndex+= sizeof(short);
			// 方向
			sess.dir = (E_TYPE_USER_DIRECTION)data[nIndex];
			nIndex++;
			sess.connect_state = CONN_STATE_AUTHED;
			sess.entity = 1;
			memcpy(&sess.cost, (BYTE*)&data[nIndex], sizeof(short));
			g_nMaxCost = sess.cost;
			g_nMaxItemStockCount = (int)(sess.cost/GAME_ITEM_01_COST);
			nIndex+= sizeof(short);
			// キャラ情報設定
			g_pGame->SetNewUser(&sess, TRUE);
			// 準備状態を設定
			g_pGame->SetGameReady(sess.sess_index, sess.game_ready);
			// 部屋情報の有効状態更新
			if (sess.master)
				g_pGame->SetGameMaster(sess.sess_index, TRUE);
			g_pGame->UpdateGameMasterPropertyControlsEnabled();
			g_pGame->UpdateReadyButtonState();
			g_pGame->UpdatePropertyControlsEnabled();
			g_pCriticalSection->LeaveCriticalSection_Session();
			OutputDebugStr(L"PK_USER_ROOMINFO_NEW\n");
		}
		break;
	case PK_USER_ROOMINFO_MASTER:
		{
			int nCharaIndex = data[4];		// ユーザNo
			BOOL bMaster = (BOOL)data[5];	// マスター設定
			// 状態設定	
			g_pCriticalSection->EnterCriticalSection_Session(L'T');
			g_pGame->SetGameMaster(nCharaIndex, bMaster);
			g_pGame->SetGameReady(nCharaIndex, FALSE);
			g_pCriticalSection->LeaveCriticalSection_Session();
			g_pGame->UpdateReadyButtonState();
		}
		break;
	case PK_USER_ROOMINFO_MOVE:	// 部屋移動パケット
		{
			int nCharaIndex = data[4];		// ユーザNo
			short nPosX = 0;	// 位置X
			memcpy(&nPosX, (BYTE*)&data[5], sizeof(short));
			short nPosY =0;	// 位置Y
			memcpy(&nPosY, (BYTE*)&data[7], sizeof(short));
			short nVecX = 0;		// 移動X
			memcpy(&nVecX, (BYTE*)&data[9], sizeof(short));
			short nVecY = 0;	// 移動Y
			memcpy(&nVecY, (BYTE*)&data[11], sizeof(short));
			g_pCriticalSection->EnterCriticalSection_Session(L'Y');
			type_session* pSess = g_pGame->GetAuthedSessionInfo(nCharaIndex);
			if (!pSess)
			{
				g_pCriticalSection->LeaveCriticalSection_Session();
				break;
			}
			pSess->lx = nPosX;
			pSess->ly = nPosY;
			pSess->vx = nVecX;
			pSess->vy = nVecY;
			if (nVecX != 0)
				pSess->dir = (nVecX>0)?USER_DIRECTION_RIGHT:USER_DIRECTION_LEFT;
#ifdef _DEBUG
			WCHAR log[64];
			SafePrintf(log, 64, L"移動:%d,%d / %d,%d\n", nPosX, nPosY, nVecX, nVecY);
			DXUTTRACE(log);
#endif
			g_pCriticalSection->LeaveCriticalSection_Session();
		}
		break;
	case PK_USER_ROOMINFO_CHARA_SEL:		// キャラ選択
		{
			int nCharaIndex = data[4];		// ユーザNo
			int nCharaType = data[5];		// キャラタイプ
			// 情報更新
			g_pCriticalSection->EnterCriticalSection_Session(L'U');
			type_session* pSess = g_pGame->GetAuthedSessionInfo(nCharaIndex);
			if (!pSess)
			{
				g_pCriticalSection->LeaveCriticalSection_Session();
				break;
			}
			pSess->chara_type = nCharaType;
			g_pCriticalSection->LeaveCriticalSection_Session();
			// 自分が変わったのなら、キャラ選択画面変更
			if (nCharaIndex == g_pGame->GetUserIndex())
			{
#if RTRIAL
				g_bCloseSocket = TRUE;	// 終了
#else
				g_pGame->SetChacterSelectInfo(nCharaType);
#endif
			}

			g_pGame->UpdateRoomCharacter(nCharaIndex);
			g_pGame->UpdateUserList(nCharaIndex);
			OutputDebugStr(L"PK_USER_ROOMINFO_CHARA_SEL");
		}
		break;
	case PK_USER_ROOMINFO_STAGE_SEL:	// ステージ選択
		g_pGame->SetStageIndex((int)data[4]);
		break;
	case PK_USER_ROOMINFO_RULE:
		g_pGame->SetRule(data[4]);
		break;
	case PK_USER_ROOMINFO_RULE_TURN_LIMIT:
		{
			short turn = 0;
			memcpy(&turn, (void*)&data[4], sizeof(short));
			g_pGame->SetTurnLimit(turn);
		}
		break;
	case PK_USER_ROOMINFO_RULE_ACT_TIME_LIMIT:
		g_pGame->SetActTimeLimit(data[4]);
		break;
	case PK_USER_ROOMINFO_ITEM_SEL:	// アイテム選択
		{
			int nItemIndex = data[4];		// アイテム欄No
			DWORD dwItemFlg;
			memcpy(&dwItemFlg, (void*)&data[5], sizeof(DWORD));	// アイテムフラグ
			WORD wCost = 0;
			memcpy(&wCost, &data[9], sizeof(short));

			g_pCriticalSection->EnterCriticalSection_Session(L'I');
			type_session* pSess = g_pGame->GetMySessionInfo();
			if (pSess)
			{
				pSess->items[nItemIndex] = dwItemFlg;
				pSess->cost = wCost;
				g_pGame->UpdateMyItems();
			}
			g_pCriticalSection->LeaveCriticalSection_Session();
		}
		break;
	case PK_USER_ROOMINFO_TEAM_COUNT:
		{
			int nTeamCount = data[4];
			g_pGame->SetTeamCount(nTeamCount);
//			g_pGame->ResetGameReady();
		}
		break;
	case PK_USER_ROOMINFO_READY:
		{
			int nCharaIndex = data[4];		// ユーザNo
			BOOL bReady = (BOOL)data[5];	// キャラNo
			// 状態設定	
			g_pCriticalSection->EnterCriticalSection_Session(L'O');
			g_pGame->SetGameReady(nCharaIndex, bReady);
			g_pGame->UpdateReadyButtonState();
			g_pCriticalSection->LeaveCriticalSection_Session();
			OutputDebugStr(L"PK_USER_ROOMINFO_READY");
		}
		break;
	case PK_USER_ROOMINFO_RE_ENTER:
		{
			int nIndex = 4;
			int nCharaIndex = data[nIndex];	// user No
			nIndex++;
			g_pCriticalSection->EnterCriticalSection_Session(L'P');
			// 再入場時、未接続ユーザーのクリア
			for (int i=0;i<g_pGame->GetMaxLoginNum();i++)
			{
				ptype_session inisess = g_pGame->GetSessionInfo(i);
				if (inisess->connect_state != CONN_STATE_AUTHED)
//					inisess->entity = 0;
					ZeroMemory(inisess, sizeof(type_session));
			}

			ptype_session sess = g_pGame->GetAuthedSessionInfo(nCharaIndex);
			if (!sess)
			{
				g_pCriticalSection->LeaveCriticalSection_Session();
				break;
			}
			sess->entity = 1;
			memcpy(&sess->lx, &data[nIndex], sizeof(short));
			nIndex+=sizeof(short);
			memcpy(&sess->ly, &data[nIndex], sizeof(short));
			nIndex+=sizeof(short);
			memcpy(&sess->vx, &data[nIndex], sizeof(short));
			nIndex+=sizeof(short);
			memcpy(&sess->vy, &data[nIndex], sizeof(short));
			// 方向
			sess->dir = (E_TYPE_USER_DIRECTION)data[nIndex];
			nIndex++;
			nIndex+=sizeof(short);
			sess->master = data[nIndex];
			nIndex++;
			memcpy(&sess->obj_state, &data[nIndex], sizeof(E_TYPE_OBJ_STATE));
			g_pGame->ReEnterCharacter(nCharaIndex);
			g_pCriticalSection->LeaveCriticalSection_Session();
			break;
		}
	case PK_USER_ROOMINFO_TEAM_RAND:
		{
			int nIndex = 4;
			int nTeams = data[nIndex];	// Team num
			if (nTeams <= 0)
			{
				g_pGame->AddChatMessage(L"正しいチーム数を入力して下さい", PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				g_pGame->AddChatMessage(L"---------チーム分け---------", PK_USER_CHAT_SERVER_INFO);
				++nIndex;
				for (int i=0;i<nTeams;++i)
				{
					size_t size = data[nIndex];
					++nIndex;
					WCHAR* msg = new WCHAR[size/sizeof(WCHAR)+1];
					memset((void*)msg, 0, size+sizeof(WCHAR));
					if (msg)
					{
						memcpy((void*)msg, &data[nIndex], size);
						g_pGame->AddChatMessage(msg, PK_USER_CHAT_SERVER_INFO);
						delete [] msg;
					}
					nIndex += size;
				}
			}
		}
		break;
	}
	return ret;
}

