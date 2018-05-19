#include "ext.h"
#include "main.h"

BOOL PacketProc_Main_PK_USER_MAININFO_START(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_TURNEND(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_ACTIVE(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATE(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_MV(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATUS(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATE(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_ITEM_USE(BYTE* data);

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_KILL(BYTE* data);		// キャラ死亡パケット情報
BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_DROP(BYTE* data);		// キャラ死亡パケット情報
BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_CLOSE(BYTE* data);	// キャラ死亡パケット情報

BOOL PacketProc_Main_PK_REQ_TRIGGER_END(BYTE* data);

BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_TRIGGER(BYTE* data);
BOOL PacketProc_Main_PK_REQ_MAININFO_BULLET_SHOT(BYTE* data);
BOOL PacketProc_Main_PK_REJ_MAININFO_BULLET_SHOT(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_SHOT(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_MV(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_VEC(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_OBJECT_RM(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_OBJECT_BOMB(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_PASTE_IMAGE(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_OBJECT_UPDATE_STATE(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_UPDATE_TYPE(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_GAMEEND(BYTE* data);
BOOL PacketProc_Main_PK_REJ_MAININFO_TURN_PASS(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_WIND(BYTE* data);
BOOL PacketProc_Main_PK_USER_MAININFO_ADD_ITEM(BYTE* data);

BOOL PacketProc_Main(BYTE* data)
{
	BOOL ret = FALSE;
	switch (data[3])
	{
	case PK_USER_MAININFO_START:
		ret = PacketProc_Main_PK_USER_MAININFO_START(data);
		break;
	case PK_USER_MAININFO_TURNEND:
		ret = PacketProc_Main_PK_USER_MAININFO_TURNEND(data);
		break;
	case PK_USER_MAININFO_ACTIVE:			// キャラアクティブ情報
		ret = PacketProc_Main_PK_USER_MAININFO_ACTIVE(data);
		break;
	case PK_USER_MAININFO_CHARA_UPDATE_STATE:
		ret = PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATE(data);
		break;
	case PK_USER_MAININFO_CHARA_MV:		// キャラ移動パケット
		ret = PacketProc_Main_PK_USER_MAININFO_CHARA_MV(data);
		break;
	case PK_USER_MAININFO_CHARA_DEAD_KILL:
		ret = PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_KILL(data);
		break;
	case PK_USER_MAININFO_CHARA_DEAD_DROP:
		ret = PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_DROP(data);
		break;
	case PK_USER_MAININFO_CHARA_DEAD_CLOSE:
		ret = PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_CLOSE(data);
		break;
	case PK_USER_MAININFO_BULLET_TRIGGER:		// トリガーパケット
		ret = PacketProc_Main_PK_USER_MAININFO_BULLET_TRIGGER(data);
		break;
	case PK_REQ_MAININFO_BULLET_SHOT:		// 弾発射要求パケット
		ret = PacketProc_Main_PK_REQ_MAININFO_BULLET_SHOT(data);
		break;
	case PK_REJ_MAININFO_BULLET_SHOT:		// 弾発射拒否パケット
		ret = PacketProc_Main_PK_REJ_MAININFO_BULLET_SHOT(data);
		break;
	case PK_USER_MAININFO_BULLET_SHOT:
		ret = PacketProc_Main_PK_USER_MAININFO_BULLET_SHOT(data);
		break;
	case PK_USER_MAININFO_BULLET_MV:
		ret = PacketProc_Main_PK_USER_MAININFO_BULLET_MV(data);
		break;
	case PK_USER_MAININFO_BULLET_VEC:
		ret = PacketProc_Main_PK_USER_MAININFO_BULLET_VEC(data);
		break;
	case PK_USER_MAININFO_OBJECT_RM:
		ret = PacketProc_Main_PK_USER_MAININFO_OBJECT_RM(data);
		break;
	case PK_USER_MAININFO_OBJECT_BOMB:
		ret = PacketProc_Main_PK_USER_MAININFO_OBJECT_BOMB(data);
		break;
	case PK_USER_MAININFO_PASTE_IMAGE:
		ret = PacketProc_Main_PK_USER_MAININFO_PASTE_IMAGE(data);
		break;
	case PK_USER_MAININFO_CHARA_UPDATE_STATUS:
		ret = PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATUS(data);
		break;
	case PK_USER_MAININFO_OBJECT_UPDATE_STATE:
		ret = PacketProc_Main_PK_USER_MAININFO_OBJECT_UPDATE_STATE(data);
		break;
	case PK_USER_MAININFO_ITEM_USE:
		ret = PacketProc_Main_PK_USER_MAININFO_ITEM_USE(data);
		break;
	case PK_USER_MAININFO_GAMEEND:
		ret = PacketProc_Main_PK_USER_MAININFO_GAMEEND(data);
		break;
	case PK_USER_MAININFO_BULLET_UPDATE_TYPE:
		ret = PacketProc_Main_PK_USER_MAININFO_BULLET_UPDATE_TYPE(data);
		break;
	case PK_REJ_MAININFO_TURN_PASS:
		ret = PacketProc_Main_PK_REJ_MAININFO_TURN_PASS(data);
		break;
	case PK_USER_MAININFO_WIND:
		ret = PacketProc_Main_PK_USER_MAININFO_WIND(data);
		break;
	case PK_USER_MAININFO_ADD_ITEM:
		ret = PacketProc_Main_PK_USER_MAININFO_ADD_ITEM(data);
		break;
	case PK_REQ_TRIGGER_END:
		ret = PacketProc_Main_PK_REQ_TRIGGER_END(data);
		break;
	default:
		break;
	}
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_START(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 5;
	g_pCriticalSection->EnterCriticalSection_Session(L'|');

	for (int i=0;i<data[4];i++)
	{
		int nCharaIndex = data[nIndex];
		nIndex++;
		ptype_session sess = NULL;
		sess = g_pGame->GetSessionInfo(nCharaIndex);
		if (!sess)	continue;
		memcpy(&sess->obj_no, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);
		// x
		memcpy(&sess->ax, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);
		// y
		memcpy(&sess->ay, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);
		// dir
		sess->dir = (E_TYPE_USER_DIRECTION)data[nIndex];
		nIndex++;
		// angle
		memcpy(&sess->angle, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// HP
		memcpy(&sess->HP_c, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);
		sess->HP_m = sess->HP_c;
		// MV
		memcpy(&sess->MV_c, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);
		sess->MV_m = sess->MV_c;
		// delay
		memcpy(&sess->delay, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);
		// exp_c
		memcpy(&sess->EXP_c, &data[nIndex], sizeof(short));
		nIndex+= sizeof(short);

		if (sess->connect_state == CONN_STATE_AUTHED)
		{
			if (sess->team_no != GALLERY_TEAM_NO)
				sess->obj_state = OBJ_STATE_MAIN_WAIT;
			else
				sess->obj_state = OBJ_STATE_MAIN_GALLERY;
		}
		else
		{
			WCHAR log[64];
			SafePrintf(log, 64, L"(mainstart)Loading disc user:%d", nCharaIndex);
			AddMessageLog(log);
//			sess->obj_state = OBJ_STATE_MAIN_DEAD;
			sess->live_count = 0;
			sess->turn_count = 0;
			g_pGame->SetCharacterDead(nCharaIndex, PK_USER_MAININFO_CHARA_DEAD_CLOSE);
		}
	}
	AddMessageLog(L"PK_MAIN_START\n");
	g_pGame->MainStart();
	g_pCriticalSection->LeaveCriticalSection_Session();
	g_pGame->UpdateStageCharaDisplay(g_pGame->GetUserIndex());
	ret = TRUE;
	return ret;
}
BOOL PacketProc_Main_PK_USER_MAININFO_TURNEND(BYTE* data)
{
	BOOL ret = FALSE;
	OutputDebugStr(L"PK_USER_MAININFO_TURNEND\n");

	g_pCriticalSection->EnterCriticalSection_Session(L'q');
	int nCharaIndex = g_pGame->GetUserIndex();
	type_session* pSess = g_pGame->GetSessionInfo(nCharaIndex);

//	if (!pSess)	return ret;
	memcpy(&pSess->delay, (BYTE*)&data[4], sizeof(short));
	DWORD state = OBJ_STATE_MAIN_WAIT;
	memcpy(&state, (BYTE*)&data[6], sizeof(DWORD));
	if ((DWORD)pSess->obj_state != state)
	{
		pSess->obj_state = (E_TYPE_OBJ_STATE)state;
		pSess->frame_count = 0;
	}
	pSess->MV_c = pSess->MV_m;
	char wind;
	memcpy(&wind, (BYTE*)&data[12], sizeof(char));
	if (pSess->obj_state != OBJ_STATE_MAIN_GALLERY)
	{
		memcpy(&pSess->MV_m, (BYTE*)&data[10], sizeof(short));
		memcpy(&pSess->turn_count, (BYTE*)&data[13], sizeof(WORD));
		memcpy(&pSess->live_count, (BYTE*)&data[15], sizeof(WORD));

		// スペル溜まってないか
		BOOL bExp = ((pSess->sess_index == g_pGame->GetUserIndex()) && (pSess->EXP_c < ((TCHARA_SCR_INFO*)pSess->scrinfo)->sc_info.max_exp));
		memcpy(&pSess->EXP_c, (BYTE*)&data[17], sizeof(short));
		if (bExp && pSess->EXP_c >= ((TCHARA_SCR_INFO*)pSess->scrinfo)->sc_info.max_exp)
		{
			g_pGame->AddChatMessage(L"スペルカードが使用可能になりました。", PK_USER_CHAT_SERVER_INFO);
			g_pGame->PlaySysSoundSE(SE_sai_Chat);
		}
	}
	// ターンエンド
	g_pGame->UpdatePrevWindValue();
	g_pGame->UpdateWindValue((int)wind);
	g_pGame->UpdateActiveState();
	g_pGame->OnTurnEnd(nCharaIndex);
	g_pGame->UpdateStageCharaDisplay(nCharaIndex);
	g_pCriticalSection->LeaveCriticalSection_Session();
	if (g_bDebug)
	{
		WCHAR tendlog[32];
		SafePrintf(tendlog, 32, L"TurnEnd:%d(count:%d)", nCharaIndex, g_pGame->GetTurnCount());
		AddMessageLog(tendlog);
	}

	return ret;
}
BOOL PacketProc_Main_PK_USER_MAININFO_ACTIVE(BYTE* data)
{
	BOOL ret = FALSE;
	short nActObjNo;
	int nIndex = 4;
	memcpy(&nActObjNo, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	WORD wTurnCount = 0;
	memcpy(&wTurnCount, (BYTE*)&data[nIndex], sizeof(WORD));
	g_pCriticalSection->EnterCriticalSection_Session(L'w');
	g_pCriticalSection->EnterCriticalSection_Object(L'i');
	g_pGame->ActivateCharacter(nActObjNo, wTurnCount);
	g_pGame->UpdateActiveState();
	g_pCriticalSection->LeaveCriticalSection_Object();
	g_pCriticalSection->LeaveCriticalSection_Session();
	if (g_bDebug)
	{
		WCHAR log[64];
		SafePrintf(log, 64, L"PK_ACTIVE : %d\n", nActObjNo);
		OutputDebugStr(log);
	}
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATE(BYTE* data)
{
	OutputDebugStr( L"CHARA_UPDATE_STATE\n");
	BOOL ret = FALSE;
	int nIndex = 4;
	short nObjNo;
	memcpy(&nObjNo, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
 	int nStateIndex = (BYTE)data[nIndex];
	nIndex++;
	int nState = (BYTE)data[nIndex];
	nIndex++;
	short sAngle = 0;
	memcpy(&sAngle, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	short sPosX = 0;
	memcpy(&sPosX, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	short sPosY = 0;
	memcpy(&sPosY, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	E_TYPE_USER_DIRECTION dir = USER_DIRECTION_LEFT;
	memcpy(&dir, (BYTE*)&data[nIndex], sizeof(E_TYPE_USER_DIRECTION));
	nIndex += sizeof(E_TYPE_USER_DIRECTION);
	short sVecX = 0;
	memcpy(&sVecX, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	short sVecY = 0;
	memcpy(&sVecY, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);


	g_pCriticalSection->EnterCriticalSection_Session(L'e');
	g_pGame->UpdateCharaState(nObjNo, nStateIndex, nState);
	type_session* pSess = NULL;
	pSess = g_pGame->GetSessionObj(nObjNo);
	if (!pSess)
	{
		pSess->angle = sAngle;
		pSess->ax = sPosX;
		pSess->ay = sPosY;
		pSess->dir = dir;
		pSess->vx = sVecX;
		pSess->vy = sVecY;
		// キャラ表示更新
		g_pGame->UpdateStageCharaDisplay(pSess->sess_index);
	}
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_MV(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;

	short nObjNo;		// ユーザNo
	memcpy(&nObjNo, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);

	g_pCriticalSection->EnterCriticalSection_Session(L'r');
	type_session* pSess = NULL;
	pSess = g_pGame->GetSessionObj(nObjNo);
	if (!pSess)
	{
		g_pCriticalSection->LeaveCriticalSection_Session();
		return ret;
	}
	memcpy(&pSess->ax, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->ay, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->vx, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->vy, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	pSess->dir = (E_TYPE_USER_DIRECTION)data[nIndex];	// 方向
	nIndex += sizeof(E_TYPE_USER_DIRECTION);
	memcpy(&pSess->angle, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->MV_c, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	if (g_bDebug)
	{
		if (!pSess->vx)
		{
			WCHAR msglog[80];
			SafePrintf(msglog, 80, L"ChrMove#%d(px:%d,py%d,vx%d,vy%d)",pSess->obj_no, pSess->ax, pSess->ay, pSess->vx, pSess->vy);
			AddMessageLog(msglog);
		}
	}
	// キャラ表示更新
	g_pGame->UpdateStageCharaDisplay(pSess->sess_index);
	g_pCriticalSection->LeaveCriticalSection_Session();

	return TRUE;
}

BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_TRIGGER(BYTE* data)
{
	int nIndex = 4;
	// キャラ番号
	short nObjNo;
	memcpy(&nObjNo, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	// スクリプト/アイテムのタイプ
	int nProcType = data[nIndex];
	nIndex++;
	// 演出タイプ
	int nBltType = data[nIndex];
	nIndex++;
	// 角度
	short sShotAngle = 0;
	memcpy(&sShotAngle, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	// パワー
	short sShotPower = 0;
	memcpy(&sShotPower, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	// indicator角度
	short sIndicatorShotAngle = 0;
	memcpy(&sIndicatorShotAngle, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	// Indicatorパワー
	short sIndicatorShotPower = 0;
	memcpy(&sIndicatorShotPower, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);

	g_pCriticalSection->EnterCriticalSection_Session(L't');
	ptype_session sess = g_pGame->GetSessionObj((int)nObjNo);
	g_pGame->SetTrigger(sess->sess_index, nProcType, nBltType, (int)sShotAngle, (int)sShotPower, (int)sIndicatorShotAngle, (int)sIndicatorShotPower);
	g_pCriticalSection->LeaveCriticalSection_Session();
	AddMessageLog(L"PK_TRIGGER");
	return TRUE;
}

BOOL PacketProc_Main_PK_REQ_TRIGGER_END(BYTE* data)
{
	// キャラ番号
	short nObjNo = 0;
	memcpy(&nObjNo, &data[4], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L't');
	g_pGame->ReqTriggerEnd(nObjNo);
	g_pCriticalSection->LeaveCriticalSection_Session();
	AddMessageLog(L"PK_REQ_TRIGGER_END");
	return TRUE;
}

BOOL PacketProc_Main_PK_REQ_MAININFO_BULLET_SHOT(BYTE* data)
{
	// キャラ番号
	short nObjNo = 0;
	memcpy(&nObjNo, &data[4], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L'y');
	g_pGame->ReqBulletShot(nObjNo);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return TRUE;
}

BOOL PacketProc_Main_PK_REJ_MAININFO_BULLET_SHOT(BYTE* data)
{
	// キャラ番号
	short nObjNo = 0;
	memcpy(&nObjNo, &data[4], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L'u');
	g_pGame->RejBulletShot(nObjNo);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return TRUE;
}

BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_SHOT(BYTE* data)
{
	int nIndex=4;
	type_blt* newblt = NULL;
	DEBUG_NEW(newblt, type_blt(), L"PK_BULLET_SHOT");
	ZeroMemory(newblt, sizeof(type_blt));
	// キャラ
	memcpy(&newblt->chr_obj_no, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// キャラ
	newblt->chara_type = (BYTE)data[nIndex];
	nIndex++;
	// 弾の種類
	newblt->bullet_type = (BYTE)data[nIndex];
	nIndex++;
	// 弾番号
	memcpy(&newblt->obj_no, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 処理タイプ
	newblt->proc_type = (BYTE)data[nIndex];
	nIndex++;
	// 弾タイプ
	newblt->obj_type = (BYTE)data[nIndex];
	nIndex++;
	// 状態
	memcpy(&newblt->obj_state, &data[nIndex], sizeof(DWORD));
	nIndex+=sizeof(DWORD);
	// 座標X
	memcpy(&newblt->ax, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 座標Y
	memcpy(&newblt->ay, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 移動値X
	memcpy(&newblt->vx, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 移動値Y
	memcpy(&newblt->vy, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 加速値X
	memcpy(&newblt->adx, &data[nIndex], sizeof(char));
	nIndex+=sizeof(char);
	// 加速Y
	memcpy(&newblt->ady, &data[nIndex], sizeof(char));
	nIndex+=sizeof(char);
	// extdata1
	memcpy(&newblt->extdata1, &data[nIndex], sizeof(DWORD));
	nIndex+=sizeof(DWORD);
	// extdata2
	memcpy(&newblt->extdata2, &data[nIndex], sizeof(DWORD));
	nIndex+=sizeof(DWORD);
	
	newblt->angle = (short)GetAngle((double)newblt->vx, (double)newblt->vy);	// 進んでいる方向（角度）を算出

//	g_pGame->SetShotBullet(newblt->chr_obj_no, 0);
	if (!g_pGame->AddObject((type_obj*)newblt))
	{
		DEBUG_DELETE(newblt, L"error PK_BULLET_SHOT");
	}
	return TRUE;
}

BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_MV(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex=5;
	int nBulletCount = data[4];
	for (int i=0;i<nBulletCount;i++)
	{
		// 弾番号
		short nObjNo;
		memcpy(&nObjNo, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 座標X
		short ax = 0;
		memcpy(&ax, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 座標Y
		short ay = 0;
		memcpy(&ay, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 移動値X
		short vx = 0;
		memcpy(&vx, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 移動値Y
		short vy = 0;
		memcpy(&vy, &data[nIndex], sizeof(short));
		nIndex+=sizeof(WORD);
		// 加速値X
		char adx;
		memcpy(&adx, &data[nIndex], sizeof(char));
		nIndex+=sizeof(char);
		// 移動値Y
		char ady;
		memcpy(&ady, &data[nIndex], sizeof(char));
		nIndex+=sizeof(char);
		// frame
		WORD frame = 0;
		memcpy(&frame, &data[nIndex], sizeof(WORD));
		nIndex+=sizeof(WORD);
		WCHAR log[64];
		SafePrintf(log, 64, L"PK_BLT_MV:[%d][frame:%d][%d,%d / %d,%d / %d,%d]", nObjNo, frame, ax,ay,vx,vy, adx,ady);
		AddMessageLog(log);
		g_pCriticalSection->EnterCriticalSection_Object(L'i');
		g_pGame->PauseSetObjectWaitForServerEvent(nObjNo, FALSE, frame);
		g_pGame->SetObjectLuaFlg(nObjNo, PROC_FLG_OBJ_UPDATE_POS, FALSE);
		g_pGame->UpdateBulletVector(nObjNo, ax, ay, vx, vy, adx, ady);
		g_pCriticalSection->LeaveCriticalSection_Object();
		ret = TRUE;
	}
	return ret;
}


BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_VEC(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex=5;
	int nBulletCount = data[4];
	for (int i=0;i<nBulletCount;i++)
	{
		// 弾番号
		short nObjNo;
		memcpy(&nObjNo, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 座標X
		short ax = 0;
		memcpy(&ax, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 座標Y
		short ay = 0;
		memcpy(&ay, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 移動値X
		short vx = 0;
		memcpy(&vx, &data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 移動値Y
		short vy = 0;
		memcpy(&vy, &data[nIndex], sizeof(short));
		nIndex+=sizeof(WORD);
		// 加速値X
		char adx;
		memcpy(&adx, &data[nIndex], sizeof(char));
		nIndex+=sizeof(char);
		// 移動値Y
		char ady;
		memcpy(&ady, &data[nIndex], sizeof(char));
		nIndex+=sizeof(char);
		// frame
		WORD frame = 0;
		memcpy(&frame, &data[nIndex], sizeof(WORD));
		nIndex+=sizeof(WORD);
		WCHAR log[64];
		SafePrintf(log, 64, L"PK_BLT_VEC:[%d][frame:%d][%d,%d / %d,%d / %d,%d]", nObjNo, frame, ax,ay,vx,vy, adx,ady);
		AddMessageLog(log);
		g_pCriticalSection->EnterCriticalSection_Object(L'o');
		g_pGame->PauseSetObjectWaitForServerEvent(nObjNo, FALSE, frame);
		g_pGame->SetObjectLuaFlg(nObjNo, PROC_FLG_OBJ_UPDATE_VEC, FALSE);
		g_pGame->UpdateBulletVector(nObjNo, ax, ay, vx, vy, adx, ady);
		g_pCriticalSection->LeaveCriticalSection_Object();
		ret = TRUE;
	}
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_OBJECT_RM(BYTE* data)
{
	BOOL ret = FALSE;
	// 弾番号
	int nIndex = 4;
	short nObjNo;
	memcpy(&nObjNo, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 消える種類
	E_OBJ_RM_TYPE rm_type = (E_OBJ_RM_TYPE)data[nIndex];
	nIndex++;
	// 座標X
	short ax = 0;
	memcpy(&ax, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 座標Y
	short ay = 0;
	memcpy(&ay, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 移動値X
	short vx = 0;
	memcpy(&vx, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// 移動値Y
	short vy = 0;
	memcpy(&vy, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// frame
	WORD frame = 0;
	memcpy(&frame, &data[nIndex], sizeof(WORD));
	nIndex+=sizeof(WORD);

	g_pCriticalSection->EnterCriticalSection_Object(L'p');
	WCHAR rmlog[32];
	SafePrintf(rmlog, 32, L"PK_OBJECT_RM:%d", nObjNo);
	AddMessageLog(rmlog);
	g_pGame->UpdateObjectVector(nObjNo, ax, ay, vx, vy);
	g_pGame->RemoveObject(nObjNo);
	g_pGame->PauseSetObjectWaitForServerEvent(nObjNo, FALSE, frame);

	g_pCriticalSection->LeaveCriticalSection_Object();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_OBJECT_BOMB(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;
	// scr
	int scr_id = data[nIndex];
	nIndex++;
	// type
	int blt_type = data[nIndex];
	nIndex++;
	// blt_chr_no
	short blt_chr_no;
	memcpy(&blt_chr_no, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// obj_no
	short obj_no = 0;
	memcpy(&obj_no, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// X
	short bx = 0;
	memcpy(&bx, (void*)&data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// Y
	short by = 0;
	memcpy(&by, (void*)&data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	
	BYTE erase = data[nIndex];
	nIndex++;

//	WCHAR bmlog[96];
//	SafePrintf(bmlog, 96, L">PK_OBJ_BOMB(scr_id:%d/blt_type:%d/blt_chr_no:%d/obj_no:%d/%d,%d", scr_id,blt_type,blt_chr_no, obj_no,bx,by);
//	AddMessageLog(bmlog);
	ret = g_pGame->BombObject(scr_id, blt_type, blt_chr_no, obj_no,bx, by, erase, &data[nIndex]);
	AddMessageLog(L"<PK_OBJ_BOMB");
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_PASTE_IMAGE(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;
	// scr_id
	int scr_id = data[nIndex];
	nIndex+=sizeof(char);
	// StageX
	short sx = 0;
	memcpy(&sx, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// StageY
	short sy = 0;
	memcpy(&sy, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// ImageX
	short ix = 0;
	memcpy(&ix, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// ImageY
	short iy = 0;
	memcpy(&iy, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// ImageW
	short iw = 0;
	memcpy(&iw, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// ImageH
	short ih = 0;
	memcpy(&ih, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	
	g_pGame->PasteTextureOnStage(scr_id, sx,sy, ix,iy,iw,ih);
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_UPDATE_STATUS(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;
	short nObjNo;		// ユーザNo
	memcpy(&nObjNo, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);

	g_pCriticalSection->EnterCriticalSection_Session(L'i');
	type_session* pSess = NULL;
	pSess = g_pGame->GetSessionObj(nObjNo);
	if (!pSess)
	{
		g_pCriticalSection->LeaveCriticalSection_Session();
		return ret;
	}
	int nPrevHP = pSess->HP_c;
	memcpy(&pSess->HP_c, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->MV_m, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->MV_c, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	memcpy(&pSess->delay, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);

	BOOL bExp = ((pSess->sess_index == g_pGame->GetUserIndex()) && (pSess->EXP_c < ((TCHARA_SCR_INFO*)pSess->scrinfo)->sc_info.max_exp));
	memcpy(&pSess->EXP_c, (BYTE*)&data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	if (bExp && pSess->EXP_c >= ((TCHARA_SCR_INFO*)pSess->scrinfo)->sc_info.max_exp)
	{
		g_pGame->AddChatMessage(L"スペルカードが使用可能になりました。", PK_USER_CHAT_SERVER_INFO);
		g_pGame->PlaySysSoundSE(SE_sai_Chat);
	}
	if (nPrevHP != pSess->HP_c)
		g_pGame->ChangeCharaHP(nObjNo, pSess->HP_c-nPrevHP);
	// キャラ表示更新
	g_pGame->UpdateStageCharaDisplay(pSess->sess_index);
	g_pCriticalSection->LeaveCriticalSection_Session();
	ret = TRUE;
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_OBJECT_UPDATE_STATE(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;
	short nObjNo;
	memcpy(&nObjNo, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	DWORD dwObjState;
	memcpy(&dwObjState, (BYTE*)&data[nIndex], sizeof(DWORD));
	nIndex += sizeof(DWORD);
	// frame
	WORD frame = 0;
	memcpy(&frame, &data[nIndex], sizeof(WORD));
	nIndex += sizeof(WORD);

	WCHAR log[64];
	SafePrintf(log, 64, L"PK_OBJ_UPD_STT:%d_%d_%x\n", nObjNo,frame,dwObjState);
	OutputDebugStr(log);

	g_pCriticalSection->EnterCriticalSection_Object(L'@');
	g_pGame->PauseSetObjectWaitForServerEvent(nObjNo, FALSE,frame);
	g_pGame->SetObjectLuaFlg(nObjNo, PROC_FLG_OBJ_UPDATE_STATE, FALSE);
	g_pGame->UpdateObjectState(nObjNo, dwObjState);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_GAMEEND(BYTE* data)
{
	AddMessageLog(L"PK_GAMEEND");
	BOOL ret = FALSE;
	int nIndex=5;
	int nUserCount = data[4];
	g_pCriticalSection->EnterCriticalSection_Session(L'o');
	
	//> 一端順位をMAXにしておく 20101227
	for (int i=0;i<g_pGame->GetMaxLoginNum();i++)
	{
		ptype_session pSess = g_pGame->GetSessionInfo(i);
		if (pSess->entity)
			pSess->frame_count = MAXUSERNUM;
	}
	//< 一端順位をMAXにしておく 20101227

	for (int i=0;i<nUserCount;i++)
	{
		// User番号
		short nObjNo;
		memcpy(&nObjNo, (BYTE*)&data[nIndex], sizeof(short));
		nIndex+=sizeof(short);
		// 順位
		WORD wNo = 0;
		memcpy(&wNo, (BYTE*)&data[nIndex], sizeof(WORD));
		nIndex+=sizeof(WORD);
		WORD wLiveCount = 0;
		memcpy(&wLiveCount, (BYTE*)&data[nIndex], sizeof(WORD));
		nIndex+=sizeof(WORD);
		// 順位を設定
		ptype_session pSess = g_pGame->GetSessionObj(nObjNo);
		if (pSess)
		{
			pSess->frame_count = wNo;
			pSess->live_count = wLiveCount;
		}
	}
	g_pGame->OnGameEnd();
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_ITEM_USE(BYTE* data)
{
	BOOL ret = FALSE;

	int nItemIndex = data[4];
	DWORD dwItemFlg;
	memcpy(&dwItemFlg, (void*)&data[5], sizeof(DWORD));	// アイテムフラグ
	BOOL bSteal = (BOOL)data[9];

	g_pCriticalSection->EnterCriticalSection_Session(L'p');
	g_pGame->UseItem(nItemIndex, dwItemFlg, bSteal);
	g_pCriticalSection->LeaveCriticalSection_Session();

	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_KILL(BYTE* data)		// キャラ死亡パケット情報
{
	BOOL ret = FALSE;
	short nObjNo;		// ユーザNo
	memcpy(&nObjNo, (void*)&data[4], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L'@');
	g_pGame->SetCharacterDead(nObjNo, PK_USER_MAININFO_CHARA_DEAD_KILL);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_DROP(BYTE* data)		// キャラ死亡パケット情報
{
	BOOL ret = FALSE;
	short nObjNo;		// ユーザNo
	memcpy(&nObjNo, (void*)&data[4], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L'[');
	g_pGame->SetCharacterDead(nObjNo, PK_USER_MAININFO_CHARA_DEAD_DROP);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_CHARA_DEAD_CLOSE(BYTE* data)		// キャラ死亡パケット情報
{
	BOOL ret = FALSE;
	short nObjNo;		// ユーザNo
	memcpy(&nObjNo, (void*)&data[4], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L'Q');
	g_pGame->SetCharacterDead(nObjNo, PK_USER_MAININFO_CHARA_DEAD_CLOSE);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

BOOL PacketProc_Main_PK_USER_MAININFO_BULLET_UPDATE_TYPE(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;
	// obj_no
	short obj_no;
	memcpy(&obj_no, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// type
	int type =  data[nIndex];
	nIndex+=sizeof(char);
	// ax
	short ax = 0;
	memcpy(&ax, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// ay
	short ay = 0;
	memcpy(&ay, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// vx
	short vx = 0;
	memcpy(&vx, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// vy
	short vy = 0;
	memcpy(&vy, &data[nIndex], sizeof(short));
	nIndex+=sizeof(short);
	// adx
	char adx;
	memcpy(&adx, &data[nIndex], sizeof(char));
	nIndex+=sizeof(char);
	// ady
	char ady;
	memcpy(&ady, &data[nIndex], sizeof(char));
	nIndex+=sizeof(char);
	// frame
	WORD frame = 0;
	memcpy(&frame, &data[nIndex], sizeof(WORD));
	nIndex+=sizeof(WORD);

	AddMessageLog(L"PK_BLT_UPD_TYPE");
	g_pCriticalSection->EnterCriticalSection_Object(L'[');

	g_pGame->UpdateBulletVector(obj_no, ax, ay, vx, vy, adx, ady);
	g_pGame->UpdateObjectType(obj_no, type);
	g_pGame->SetObjectLuaFlg(obj_no, PROC_FLG_OBJ_UPDATE_TYPE, FALSE);

	g_pGame->PauseSetObjectWaitForServerEvent(obj_no, FALSE,frame);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return ret;
}

BOOL PacketProc_Main_PK_REJ_MAININFO_TURN_PASS(BYTE* data)
{
	BOOL ret = FALSE;
	int nIndex = 4;
	// obj_no
	short obj_no;
	memcpy(&obj_no, &data[nIndex], sizeof(short));
	g_pCriticalSection->EnterCriticalSection_Session(L'W');
	g_pGame->RejTurnPass(obj_no);
	g_pCriticalSection->LeaveCriticalSection_Session();
	ret  = TRUE;
	return ret;	
}

BOOL PacketProc_Main_PK_USER_MAININFO_WIND(BYTE* data)
{
	BOOL ret = FALSE;
	// 風向き
	char cWindValue;
	memcpy(&cWindValue, &data[4], sizeof(char));
	g_pGame->UpdateWindValue((int)cWindValue);
	g_pGame->AddChatMessage(GAME_STATE_MSG_WIND_CHANGE, PK_USER_CHAT_SERVER_INFO);
	ret  = TRUE;
	return ret;	
}

BOOL PacketProc_Main_PK_USER_MAININFO_ADD_ITEM(BYTE* data)
{
	BOOL ret = FALSE;
	// スロット番号
	int nIndex = 4;
	// obj_no
	short obj_no;
	memcpy(&obj_no, &data[nIndex], sizeof(short));
	nIndex += sizeof(short);
	int nSlot = (int)data[nIndex];
	nIndex++;
	DWORD dwItemFlg = 0;
	memcpy(&dwItemFlg, &data[nIndex], sizeof(DWORD));
	nIndex += sizeof(DWORD);
	BOOL bSteal = data[nIndex];
	return g_pGame->AddCharaItem(obj_no, nSlot, dwItemFlg, bSteal);
}
