#include "ext.h"
#include "Game.h"
// parts
// CGame::xxx > Funcsions > Main

void CGame::MainStart()
{
	m_eGameState = eGameLoadRelease;
	for (std::vector<ptype_session>::iterator it = m_vecCharacters.begin();
		it != m_vecCharacters.end();
		it++)
	{
		ptype_session sess = (ptype_session)(*it);
		m_pStageCharacters[sess->sess_index]->Update();
		if (m_SessionArray[sess->sess_index].obj_state == OBJ_STATE_LOADING)
		{
			m_SessionArray[sess->sess_index].obj_state = OBJ_STATE_MAIN_GALLERY;
			m_SessionArray[sess->sess_index].entity = 0;
		}
	}
	/*
	for (int i=0;i<MAXLOGINNUM;i++)
	{
		m_pStageCharacters[i]->Update();
		if (m_SessionArray[i].obj_state == OBJ_STATE_LOADING)
			m_SessionArray[i].obj_state = OBJ_STATE_MAIN_GALLERY;
	}
*/
	UpdateControlPanelHPInfo();
	// 描画範囲を更新しておく
	m_vecStageViewTrans = D3DXVECTOR3(0,0,0);
	UpdateStageDrawRect();
	UpdateSCExp();

	// キャラロードイベントを呼ぶループ
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (!m_SessionArray[i].entity) 
		{
			m_SessionArray[i].obj_no = MAXUSERNUM;
			continue;
		}

		ptype_session sess = &m_SessionArray[i];
		if (sess->obj_state == OBJ_STATE_MAIN_GALLERY)
			continue;
		LuaFuncParam luaParams,luaResults;
		// ユーザ番号と位置
		luaParams.Number(sess->scrinfo->scr_index).Number(sess->obj_no).Number(sess->ax).Number(sess->ay);
		if (!common::scr::CallLuaFunc(g_pLuah, "onLoad_Chara", &luaResults, 0, &luaParams, g_pCriticalSection))
			return;

		// 認証していないなら死亡
		if (m_SessionArray[i].connect_state != CONN_STATE_AUTHED) 
		{
			if (m_SessionArray[i].entity)
				g_pGame->SetCharacterDead(i, PK_USER_MAININFO_CHARA_DEAD_CLOSE);
			continue;
		}
	}
	if (m_pSelectedStageScrInfo)
	{
		LuaFuncParam luaParams,luaResults;
		// ユーザ番号と位置
		luaParams.Number(m_pSelectedStageScrInfo->scr_index);
		if (!common::scr::CallLuaFunc(g_pLuah, "onLoad_Stage", &luaResults, 0, &luaParams, g_pCriticalSection))
			return;
	}

}

// 最新プロパティをステージキャラに反映（表示更新）
void CGame::UpdateStageCharaDisplay(int nCharaIndex)
{
	if (nCharaIndex >= GetMaxLoginNum() || nCharaIndex < 0)	return;
	m_pStageCharacters[nCharaIndex]->Update();
	// 更新キャラが自キャラ
	if (m_nUserIndex == nCharaIndex)
	{
		m_pMeterMovable->SetMaxValue(m_SessionArray[nCharaIndex].MV_m);
		m_pMeterMovable->SetValue(m_SessionArray[nCharaIndex].MV_c);
		m_pShotAngle->SetBaseLineAngle(m_SessionArray[nCharaIndex].angle);
		UpdateSCExp();
	}
}

void CGame::ActivateCharacter(int nObjNo, WORD wTurnCount)
{
	ptype_session sess = NULL;
	// 前キャラのアクティブ状態をfalseに設定
	if (sess = GetSessionObj(m_nActiveObjectNo))
	{
		if (sess->obj_state & OBJ_STATE_ACT_FLG )
		{
			sess->obj_state = OBJ_STATE_MAIN_WAIT;
			m_pStageCharacters[sess->sess_index]->SetMyTurn(false);
			// ターンエンドイベント
//			NotifyTurnEnd(sess);	// 20120819
		}
		sess->frame_count = 0;
	}
	
	m_nActiveObjectNo = nObjNo;
	if (sess = GetSessionObj(nObjNo))
	{
		sess->turn_count = wTurnCount;
		if (!(sess->obj_state & OBJ_STATE_MAIN_NOLIVE_FLG))
			sess->obj_state = OBJ_STATE_MAIN_ACTIVE;
		m_pStageCharacters[sess->sess_index]->SetMyTurn(true);
		sess->frame_count = 0;
		// ターンスタートイベント
		NotifyTurnStart(sess);	// 20120819
		// 同チームか観戦、別チームでステルスじゃなければフォーカス
		int nMyTeamNo = GetMySessionInfo()->team_no;
		if (m_bActChrFocus &&
			(
			(sess->team_no == nMyTeamNo || nMyTeamNo == GALLERY_TEAM_NO)
		|| ((sess->team_no != nMyTeamNo)
			&& !sess->chara_state[CHARA_STATE_STEALTH_INDEX]) 
			)
			)
		{
			m_nActiveCharaFocusCounter = CHARA_FOCUS_TIME;
			m_vecActiveCharaFocusStartPos = m_vecStageViewTrans;
		}

		// 自分がアクティブ
		bool bActive = (sess->sess_index == m_nUserIndex);
		if (bActive)
		{
			UpdateMyItems();	// !!!!!!
			PlaySysSoundSE(SE_sgi_MyTurn);
	//		// ショットパワーメータ初期化
	//		m_pShotMeter->SetValue(0);
			// カウント
			m_pASpriteTimer->SetVisible(true);
			m_pASpriteTimer->InitAnimationTime();
			m_pASpriteTimer->SetAnimationIndex(GAME_TURN_ACT_COUNT_MAX-m_nActTimeLimit);
			m_pASpriteTimerBG->SetVisible(true);
			m_pASpriteTimerBG->InitAnimationTime();
			m_nActiveTimeCounter = 0;
			m_eMainPhase = GAME_MAIN_PHASE_ACT;
			// 自ターンパス有効表示テクスチャ設定
			SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_ENABLE_RECT);
			p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)TRUE);
			m_nNotifyMyTurn = NOTIFY_MY_TURN_TIME;
			WCHAR wsNotifyTurn[16];
			SafePrintf(wsNotifyTurn, 16, L"TURN %02d", sess->live_count);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN)->SetText(wsNotifyTurn);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN)->SetVisible(true);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN_TEXT)->SetVisible(true);
		}
		else
		{
			m_pASpriteTimer->SetVisible(false);
			m_pASpriteTimerBG->SetVisible(false);
			m_eMainPhase = GAME_MAIN_PHASE_NONE;
			// 自ターンパス有効表示テクスチャ設定
			SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_DISABLE_RECT);
			p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)FALSE);
		}
	}
}

void CGame::UpdateActiveState()
{
	bool bActive = (GetMySessionInfo()->obj_state == OBJ_STATE_MAIN_ACTIVE);
	// カウントダウン有効/無効化
	m_pASpriteTimer->SetEnabled(bActive);
	m_pASpriteTimerBG->SetEnabled(bActive);
///	SetMyIteamsEnabled(bActive);	// 説明みたいから無効化はなし
}

// 使用アイテムの有効/無効化
void CGame::SetMyIteamsEnabled(bool bEnabled)
{
	// 選択アイテム位置設定
	m_pMyItemBtnList->SetEnable(bEnabled);

	// 有効化する場合は個別に確認
	if (bEnabled)
	{
		for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
		{
			if (p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->GetUserData())
				p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetEnabled(true);
			else
				p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetEnabled(false);
		}
	}
}

// 弾選択の有効/無効化
void CGame::SetMyBulletsEnabled(bool bEnabled)
{
	ptype_session sess = GetMySessionInfo();
	int nButtons = ((TCHARA_SCR_INFO*)sess->scrinfo)->blt_type_count;

	for (int i=0;i<nButtons;i++)
		p_pUI->GetControl(i+IDC_MAIN_BTN_SEL_BULLET_BASE)->SetEnabled(bEnabled);
	p_pUI->GetControl(IDC_MAIN_RB_SEL_SPELL)->SetEnabled(bEnabled);

	p_pUI->GetRadioButton(IDC_MAIN_BTN_SEL_BULLET_BASE+m_nSelectBulletNo)->SetChecked(true);
}

void CGame::UpdateCharaState(int nObjNo, int nStateIndex, int nState)
{
	ptype_session pSess = GetAuthedSessionObj(nObjNo);
	if (!pSess) return;
	WCHAR wsMessage[MAX_MSG_BUFFER];
	
	pSess->chara_state[nStateIndex] = nState;
	if (nState)
	{
		switch (nStateIndex)
		{
		case CHARA_STATE_STEALTH_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo))
					SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s", wsName, GAME_STATE_INFO_STEALTH);
				else
					SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s(残り%dターン)", wsName, GAME_STATE_INFO_STEALTH, pSess->chara_state[CHARA_STATE_STEALTH_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
				PlaySysSoundSE(SE_sgi_Stealth);
			}
			break;
		case CHARA_STATE_DOUBLE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s", wsName, GAME_STATE_INFO_DOUBLE);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
				PlaySysSoundSE(SE_sgi_Double);
			}
			break;
		case CHARA_STATE_POWER_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// パワーアップ状態になったときだけ鳴る
				if (nState == CHARA_STATE_POWERUP_ON)
				{
					WCHAR wsName[MAX_USER_NAME];
					common::session::GetSessionName(pSess, wsName);
					SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s", wsName, GAME_STATE_INFO_POWERUP);
					AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
					PlaySysSoundSE(SE_sgi_Double);
				}
			}
			break;
		case CHARA_STATE_BLIND_INDEX:		// 暗転状態
			// 自分がなった場合、現在のアクティブキャラが分からなくなるため▼マークを■にしておく
			if (pSess->sess_index == m_nUserIndex)
			{
				if (pSess->obj_no != m_nActiveObjectNo)
					m_pStageCharacters[pSess->sess_index]->SetMyTurn(false);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, GAME_STATE_INFO_BLIND_SELF, pSess->chara_state[CHARA_STATE_BLIND_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s(残り%dターン)", wsName, GAME_STATE_INFO_BLIND, pSess->chara_state[CHARA_STATE_BLIND_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sgi_Blind);
			break;
		case CHARA_STATE_REVERSE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s(残り%dターン)", wsName, GAME_STATE_INFO_REVERSE, pSess->chara_state[CHARA_STATE_REVERSE_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				SafePrintf(wsMessage, MAX_MSG_BUFFER, GAME_STATE_INFO_REVERSE_SELF, pSess->chara_state[CHARA_STATE_REVERSE_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sgi_Blind);
			break;
		case CHARA_STATE_NOMOVE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s(残り%dターン)", wsName, GAME_STATE_INFO_NOMOVE, pSess->chara_state[CHARA_STATE_NOMOVE_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				SafePrintf(wsMessage, MAX_MSG_BUFFER, GAME_STATE_INFO_NOMOVE_SELF, pSess->chara_state[CHARA_STATE_NOMOVE_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sgi_NoMove);
			break;
		case CHARA_STATE_NOANGLE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s(残り%dターン)", wsName, GAME_STATE_INFO_NOANGLE, pSess->chara_state[CHARA_STATE_NOANGLE_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				SafePrintf(wsMessage, MAX_MSG_BUFFER, GAME_STATE_INFO_NOANGLE_SELF, pSess->chara_state[CHARA_STATE_NOANGLE_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sgi_NoMove);
			break;
		case CHARA_STATE_PAIN_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s(残り%dターン)", wsName, GAME_STATE_INFO_PAIN, pSess->chara_state[CHARA_STATE_PAIN_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				SafePrintf(wsMessage, MAX_MSG_BUFFER, GAME_STATE_INFO_PAIN_SELF, pSess->chara_state[CHARA_STATE_PAIN_INDEX]);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sgi_Pain);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (nStateIndex)
		{
		case CHARA_STATE_STEALTH_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんの%s", wsName, GAME_STATE_INFO_STEALTH_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
				AddChatMessage(GAME_STATE_INFO_STEALTH_OFF, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_DOUBLE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんの%s", wsName, GAME_STATE_INFO_DOUBLE_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
				AddChatMessage(GAME_STATE_INFO_DOUBLE_OFF, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_POWER_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// パワーアップ状態になったときだけ鳴る
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s", wsName, GAME_STATE_INFO_POWERUP_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
				AddChatMessage(GAME_STATE_INFO_POWERUP_OFF, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_BLIND_INDEX:		// 暗転状態
			// 自分がなった場合、現在のアクティブキャラが分からなくなるため▼マークを■にしておく
			if (pSess->sess_index == m_nUserIndex)
			{
				if (pSess->obj_no != m_nActiveObjectNo)
					m_pStageCharacters[m_nUserIndex]->SetMyTurn(false);
				AddChatMessage(GAME_STATE_INFO_BLIND_OFF, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんの%s", wsName, GAME_STATE_INFO_BLIND_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_REVERSE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんの%s", wsName, GAME_STATE_INFO_REVERSE_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
			{
				AddChatMessage(GAME_STATE_INFO_REVERSE_OFF, PK_USER_CHAT_SERVER_INFO);
			}
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_NOMOVE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s", wsName, GAME_STATE_INFO_NOMOVE_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
				AddChatMessage(GAME_STATE_INFO_NOMOVE_OFF, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_NOANGLE_INDEX:
			// 他キャラの場合、情報出力
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんが%s", wsName, GAME_STATE_INFO_NOANGLE_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
				AddChatMessage(GAME_STATE_INFO_NOANGLE_OFF, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_ITEM_STEAL_INDEX:
			// 自キャラの場合、情報出力
			if (pSess->sess_index == m_nUserIndex)
				AddChatMessage(GAME_STATE_INFO_STEAL_OFF, PK_USER_CHAT_SERVER_INFO);
			break;
		case CHARA_STATE_PAIN_INDEX:
			// 他キャラの場合
			if (pSess->sess_index != m_nUserIndex)
			{
				// 自キャラが暗転中はメッセージを出さない
				if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
				WCHAR wsName[MAX_USER_NAME];
				common::session::GetSessionName(pSess, wsName);
				SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんの%s", wsName, GAME_STATE_INFO_PAIN_OFF);
				AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
			}
			else
				AddChatMessage(GAME_STATE_INFO_PAIN_OFF, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sai_Chat);
			break;
		case CHARA_STATE_SHIELD_INDEX:
			{
				// 他キャラの場合
				if (pSess->sess_index != m_nUserIndex)
				{
					// 自キャラが暗転中はメッセージを出さない
					if (GetMySessionInfo()->chara_state[CHARA_STATE_BLIND_INDEX])	break;
				// 観戦以外且つ味方以外のステルスキャラのメッセージを出さない
				int nMyTeamNo = GetMySessionInfo()->team_no;
				if ((nMyTeamNo != GALLERY_TEAM_NO && pSess->team_no != nMyTeamNo) && pSess->chara_state[CHARA_STATE_STEALTH_INDEX])	break;
					WCHAR wsName[MAX_USER_NAME];
					common::session::GetSessionName(pSess, wsName);
					SafePrintf(wsMessage, MAX_MSG_BUFFER, L"%sさんの%s", wsName, GAME_STATE_INFO_SHIELD_OFF);
					AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
				}
				else
					AddChatMessage(GAME_STATE_INFO_SHIELD_OFF, PK_USER_CHAT_SERVER_INFO);

				PlaySysSoundSE(SE_sgi_Shield);
				int nFxID = C_AddEffect(DEF_SYSTEM_ID, MAIN_FX_SHIELD_IMG_RECT, pSess->ax, pSess->ay, (int)(FRAMES/2));
				if (nFxID != -1)
				{
					C_SetEffectFade(nFxID, -15);
					C_SetEffectScalling(nFxID, 0.3, 0.3);
				}
				break;
			}
		}
	}
}

void CGame::UpdatePrevWindValue()
{
	if (GetSessionIndex(m_nActiveObjectNo) != m_nUserIndex)	return;

	// 風向きを記憶
	CDXUTSprite* pSprite = p_pUI->GetSprite(IDC_MAIN_SPRITE_PREV_WIND_INDICATOR);
	if (!pSprite)	return;
	if (!m_nWindValue)
	{
		pSprite->SetVisible(false);
	}
	else if(m_nWindValue>0)
	{
		pSprite->SetVisible(true);
		pSprite->SetLocation(MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_PLUS_ORIGN_X+((int)((float)m_nWindValue*(MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_OFFSET+0.05f))),
										MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_PLUS_ORIGN_Y);
	}
	else
	{
		pSprite->SetVisible(true);
		pSprite->SetLocation(MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_MINUS_ORIGN_X+((int)((float)m_nWindValue*MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_OFFSET)),
										MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_MINUS_ORIGN_Y);
	}
}

// 風向き更新
void CGame::UpdateWindValue(int value)
{
	m_nWindValue = min(max(value,MIN_WIND_VALUE),MAX_WIND_VALUE);
	if (value >= 0)
	{
		m_pMeterWindL->SetValue(0);
		m_pMeterWindR->SetValue(value);
	}
	else
	{
		m_pMeterWindR->SetValue(0);
		m_pMeterWindL->SetValue(value);
	}
}

// アイテム追加
BOOL CGame::AddCharaItem(int nObjNo, int nSlot, DWORD dwItemFlg, BOOL bSteal)
{
	ptype_session sess = GetSessionObj(nObjNo);
	// 引数チェック
	if (!sess) return FALSE;
	if (nSlot < 0 && nSlot >= GAME_ITEM_STOCK_MAX_COUNT) return FALSE;
	if (!(dwItemFlg & GAME_ITEM_ENABLE_FLG))	return FALSE;
	
	sess->items[nSlot] = dwItemFlg;		// 設定
	
	// 自キャラの場合、表示を更新
	if (nObjNo == m_nUserIndex)
	{
		UpdateMyItems();
		if (bSteal)
		{
			AddChatMessage(GAME_STATE_MSG_STOLE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
		}
	}
	
	return TRUE;
}

// 弾発射
void CGame::ShotBullet(int nAngle, int nPower, int nIndicatorAngle, int nIndicatorPower)
{
	WORD	packetSize = 0;
	BYTE		packetData[MAX_PACKET_SIZE];

	// アイテム弾の使用
	int nProcType = BLT_PROC_TYPE_SCR_CHARA;
	// スペルカード選択
	if (m_nSelectBulletNo == MAX_CHARA_BULLET_TYPE)
		nProcType = BLT_PROC_TYPE_SCR_SPELL;

	int nBltType = GetStockItemBullet();	// 装填済みのアイテム弾取得
	if (nBltType != -1)
		nProcType = BLT_PROC_TYPE_ITEM;
	else
		nBltType = m_nSelectBulletNo;
	
	// パケット作成
	packetSize = PacketMaker::MakePacketData_MainInfoShot( nAngle, nPower, nBltType, nProcType, nIndicatorAngle, nIndicatorPower, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}

int	CGame::GetStockItemBullet()
{
	int nBltType = -1;
	if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_REVERSE_INDEX])
		nBltType = CHARA_STATE_ITEM_REVERSE_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_BLIND_INDEX])
		nBltType = CHARA_STATE_ITEM_BLIND_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_REPAIRBLT_INDEX])
		nBltType = CHARA_STATE_ITEM_REPAIRBLT_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_TELEPORT_INDEX])
		nBltType = CHARA_STATE_ITEM_TELEPORT_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_DRAIN_INDEX])
		nBltType = CHARA_STATE_ITEM_DRAIN_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_FETCH_INDEX])
		nBltType = CHARA_STATE_ITEM_FETCH_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_EXCHANGE_INDEX])
		nBltType = CHARA_STATE_ITEM_EXCHANGE_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_NOANGLE_INDEX])
		nBltType = CHARA_STATE_ITEM_NOANGLE_INDEX;
	else if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_NOMOVE_INDEX])
		nBltType = CHARA_STATE_ITEM_NOMOVE_INDEX;

	return nBltType;
}
/*
void CGame::SetShotBullet(int nCharaIndex, int nType)
{
	if (m_nActiveObjectNo != nCharaIndex)	return;
	if (m_eMainPhase == GAME_MAIN_PHASE_SHOT_REQ)
	{
		m_SessionArray[nCharaIndex].frame_count = 0;
		m_SessionArray[nCharaIndex].obj_state = OBJ_STATE_MAIN_SHOT;
	}
	m_pStageCharacters[nCharaIndex]->Update();
	m_eMainPhase = GAME_MAIN_PHASE_SHOT;

}
*/
void CGame::SetTrigger(int nCharaIndex, int nProcType, int nBltType, int nShotAngle, int nShotPower, int nShotIndicatorAngle, int nShotIndicatorPower)
{
	if (m_nActiveObjectNo != m_SessionArray[nCharaIndex].obj_no)	return;
	m_SessionArray[nCharaIndex].obj_state = OBJ_STATE_MAIN_TRIGGER_BULLET;
	if (nProcType == BLT_PROC_TYPE_SCR_SPELL)
		m_SessionArray[nCharaIndex].obj_state = OBJ_STATE_MAIN_TRIGGER_SPELL;

	m_SessionArray[nCharaIndex].frame_count = 0;
	m_nShootingCharaObjNo = m_SessionArray[nCharaIndex].obj_no;
	m_nShootingProcType = nProcType;
	m_nShootingBltType = nBltType;
	m_nShootingAngle = nShotAngle;
	m_nShootingPower = nShotPower;
	m_nShootingIndicatorAngle = nShotIndicatorAngle;
	m_nShootingIndicatorPower = nShotIndicatorPower;
	m_pStageCharacters[nCharaIndex]->Update();
	m_eMainPhase = GAME_MAIN_PHASE_TRIGGER;
	m_nTriggerType = nBltType;

	m_bTriggerEnd = FALSE;

	m_bPauseBulletFocus = FALSE;
	m_bScrFocus = FALSE;
	if (g_bDebug)
	{
		WCHAR logf[128];
		SafePrintf(logf, 128, L"TRIGGER(C%d,#%d,$%d,X%d,Y%d,B%d,A%d,P%d)",m_SessionArray[nCharaIndex].chara_type, nCharaIndex, nProcType, m_SessionArray[nCharaIndex].ax, m_SessionArray[nCharaIndex].ay, nBltType, nShotAngle, nShotPower); 
		AddMessageLog(logf);
	}
}

void CGame::ReqTriggerEnd(int nCharaIndex)
{
// 20160101
//	if (
//		(m_eMainPhase == GAME_MAIN_PHASE_TRIGGER || m_eMainPhase == GAME_MAIN_PHASE_SHOT_REQ)
//		&& m_bTriggerEnd
//		)
//	{
//		BYTE		packetData[MAX_PACKET_SIZE];
//		WORD packetSize = PacketMaker::MakePacketData_MainInfoTriggerEnd(GetMySessionInfo(), m_nShootingProcType, m_nShootingBltType, m_nShootingAngle, m_nShootingPower, packetData);
//		AddPacket(packetData, packetSize);
//	}
// 20160101>>
	if (!m_bTriggerEnd)
	{
		m_bTriggerEnd = true;
		type_session* sess = GetMySessionInfo();
		sess->obj_state = OBJ_STATE_MAIN_SHOOTING;
		m_eMainPhase = GAME_MAIN_PHASE_SHOOTING;
		sess->frame_count = 0xFFFF;
	}
	BYTE		packetData[MAX_PACKET_SIZE];
	WORD packetSize = PacketMaker::MakePacketData_MainInfoTriggerEnd(GetMySessionInfo(), m_nShootingProcType, m_nShootingBltType, m_nShootingAngle, m_nShootingPower, m_nShootingIndicatorAngle, m_nShootingIndicatorPower, packetData);
	AddPacket(packetData, packetSize);
// <<20160101
}

void CGame::ReqBulletShot(int nObjNo)
{
//	m_eMainPhase = GAME_MAIN_PHASE_SHOT_REQ;
	m_pASpriteTimer->SetEnabled(false);				// カウントストップ
	m_pASpriteTimerBG->SetEnabled(false);

	int powerIndicatorValue = m_pShotPowerIndicator->GetCXForView();
	int angleIndicatorValue = m_pShotAngleIndicator->GetCurrentAngle();
	ShotBullet(m_pShotAngle->GetShotAngle(), m_pShotMeter->GetValue(), angleIndicatorValue, powerIndicatorValue);
	// 発射ならコンフィグ、アイテム使用、自ターンパスを無効化
	if (GetSessionIndex(nObjNo) == m_nUserIndex)
	{
		p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetEnabled(true);
		// 弾選択を有効化
		SetMyBulletsEnabled(true);		// 20101122
///		SetMyIteamsEnabled(false);
//		// 自ターンパス有効表示テクスチャ設定
//		SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_DISABLE_RECT);
//		p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)FALSE);
	}
}

void CGame::RejBulletShot(int nObjNo)
{
	m_eMainPhase = GAME_MAIN_PHASE_ACT;
	m_pShotMeter->SetValue(0);
	
	// 発射拒否ならコンフィグ、アイテム使用、自ターンパス有効化
	if (GetSessionIndex(nObjNo) == m_nUserIndex)
	{
		p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetEnabled(true);
		// 弾選択を有効化
		SetMyBulletsEnabled(true);		// 20101122
///		SetMyIteamsEnabled(true);
		// 自ターンパス有効表示テクスチャ設定
		SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_ENABLE_RECT);
		p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)TRUE);
	}
}

void CGame::RejTurnPass(int nObjNo)
{
	m_eMainPhase = GAME_MAIN_PHASE_ACT;
	// 発射拒否ならコンフィグ、アイテム使用、自ターンパス有効化
	if (GetSessionIndex(nObjNo) == m_nUserIndex)
	{
		p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetEnabled(true);
///		SetMyIteamsEnabled(true);
		// 自ターンパス有効表示テクスチャ設定
		SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_ENABLE_RECT);
		p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)TRUE);
	}	
}

BOOL CGame::AddObject(type_obj* obj)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'2');
	if (g_bDebug)
	{
		WCHAR s[32];
		SafePrintf(s, 30, L"AddObject obj_no:%d", obj->obj_no);
		AddMessageLog(s);
	}
	if (obj->obj_type & OBJ_TYPE_BLT)
	{
		if (!AddBullet(((type_blt*)obj)))
		{
			g_pCriticalSection->LeaveCriticalSection_Object();
			return FALSE;
		}
#if (E0401==0)
		if (!(obj->obj_type == OBJ_TYPE_BLT_TACTIC) &&
			!m_pFocusObject &&
			!m_bPauseBulletFocus &&
			!m_bScrFocus &&
			m_vecObjectNo.size() <= BLT_FOCUS_OBJ_RM_TO_ACTIVE_COUNT_MAX)
		{
			ptype_session self = GetMySessionInfo();
			ptype_session blt_sess = &m_SessionArray[((type_blt*)obj)->chr_obj_no];
			if (self->chara_state[CHARA_STATE_BLIND_INDEX]	// 暗転中か
				|| (self->team_no == blt_sess->team_no || self->team_no == GALLERY_TEAM_NO)	// 同チーム観戦
				|| ((self->team_no != blt_sess->team_no)				// 敵チーム且つステルス中
				&& !blt_sess->chara_state[CHARA_STATE_STEALTH_INDEX]))
			{
//				AddMessageLog(L"NewFocus");
				m_pFocusObject = obj;
				m_bPauseBulletFocus = FALSE;
			}
		}
#else
		if (!m_pFocusObject)
		{
//			AddMessageLog(L"NewFocus");
			m_pFocusObject = obj;
			m_bPauseBulletFocus = FALSE;
		}

#endif
	}

	m_mapObjects.insert(std::map<int, type_obj* >::value_type( (int)obj->obj_no, obj ));
	UpdateObjectNo();
/*
	if (obj->obj_type & OBJ_TYPE_BLT)
	{
		switch ( ((type_blt*)obj)->proc_type)
		{
		case BLT_PROC_TYPE_SCR_CHARA:
			OnCreateScrCharaBullet((type_blt*)obj);
			break;
		case BLT_PROC_TYPE_SCR_SPELL:
			OnCreateScrCharaSpell((type_blt*)obj);
			break;
		case BLT_PROC_TYPE_SCR_STAGE:
//			OnCreateItemStageBullet((type_blt*)obj);
		case BLT_PROC_TYPE_ITEM:
//			OnCreateItemBullet((type_blt*)obj);
			break;
		}
	}
*/
	g_pCriticalSection->LeaveCriticalSection_Object();
	return TRUE;
}

BOOL CGame::AddBullet(type_blt* blt)
{
	switch (blt->proc_type)
	{
	case BLT_PROC_TYPE_SCR_CHARA:
	{
		TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(blt->chara_type, &m_mapCharaScrInfo);
		if (!pCharaScrInfo)	return FALSE;
		// スクリプト情報とオブジェクト構造体の関連付け
		blt->scrinfo = pCharaScrInfo;
		blt->hit_range = pCharaScrInfo->blt_info[blt->bullet_type].hit_range;
		break;
	}
	case BLT_PROC_TYPE_SCR_SPELL:
	{
		TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(blt->chara_type, &m_mapCharaScrInfo);
		if (!pCharaScrInfo)	return FALSE;
		// スクリプト情報とオブジェクト構造体の関連付け
		blt->scrinfo = pCharaScrInfo;
		blt->hit_range = pCharaScrInfo->sc_info.hit_range;
		break;
	}
	case BLT_PROC_TYPE_SCR_STAGE:
		blt->scrinfo = m_pSelectedStageScrInfo;
		blt->hit_range = m_pSelectedStageScrInfo->blt_info[blt->bullet_type].hit_range;
		break;
	case BLT_PROC_TYPE_ITEM:
	default:	// アイテム
		PlaySysSoundSE(SE_sgi_Shot);
		blt->hit_range = BLT_DEFAULT_HITRANGE;
		break;
	}		
	blt->bx = blt->ax*BLT_POS_FACT_N;
	blt->by = blt->ay*BLT_POS_FACT_N;
	return TRUE;
}

void CGame::UpdateObjectNo()
{
	m_vecObjectNo.clear();
	// 各オブジェクトの移動
	for (std::map<int, type_obj* >::iterator it = m_mapObjects.begin();
		it != m_mapObjects.end();
		it++)
		m_vecObjectNo.push_back( (int)(*it).first );
}
/// 未使用
void CGame::OnCreateScrCharaBullet(type_blt* blt)
{
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;

	TCHARA_SCR_INFO* pCharaScrInfo = (TCHARA_SCR_INFO*)blt->scrinfo;
	if (!pCharaScrInfo)	return;

	m_pScrProcObject = blt;

	// 生成イベント
	luaParams.Number(pCharaScrInfo->scr_index).Number(blt->chr_obj_no).Number(blt->bullet_type).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->angle);
	common::scr::CallLuaFunc(g_pLuah, "shot_CharaBullet", &luaResults, 1, &luaParams, g_pCriticalSection);
	m_pScrProcObject = NULL;
}
/// 未使用
void CGame::OnCreateScrCharaSpell(type_blt* blt)
{
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;

	TCHARA_SCR_INFO* pCharaScrInfo = (TCHARA_SCR_INFO*)blt->scrinfo;
	if (!pCharaScrInfo)	return;

	m_pScrProcObject = blt;
	// 生成イベント
	luaParams.Number(pCharaScrInfo->scr_index).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->angle);
	common::scr::CallLuaFunc(g_pLuah, "shot_CharaSpell", &luaResults, 1, &luaParams, g_pCriticalSection);
	m_pScrProcObject = NULL;
}

void CGame::OnCreateScrStageBullet(type_blt* blt)
{
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	if (!m_pSelectedStageScrInfo)	return;
	m_pScrProcObject = blt;
	
	// 弾描画前イベント
	luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt->chr_obj_no).Number(blt->bullet_type).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy);
	common::scr::CallLuaFunc(g_pLuah, "onCreate_StageBullet", &luaResults, 1, &luaParams, g_pCriticalSection);
	m_pScrProcObject = NULL;
}

void CGame::FrameObjects()
{
	g_pCriticalSection->EnterCriticalSection_Object(L'3');
	int objno = 0;
	std::vector< int > vecFrame;
	vecFrame.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
	for ( std::vector<int>::iterator itno = vecFrame.begin();
		itno != vecFrame.end();
		itno++)
	{
		std::map<int ,type_obj*>::iterator it = m_mapObjects.find( (*itno));
		if (it == m_mapObjects.end())	continue;

		// 削除フラグ立っているか
		if ((*it).second->proc_flg & (PROC_FLG_OBJ_WAIT_SERVER_MSG))		continue;

		type_blt* blt = (type_blt*)(*it).second;

		if (blt->obj_state & OBJ_STATE_MAIN_PAUSE_FLG)
		{
//			AddMessageLog(L"FRAME_PAUSE");
			continue;
		}
		// TACTICはフレーム処理しない
		if (blt->obj_type & OBJ_TYPE_TACTIC)	continue;

		switch (blt->obj_state)
		{
		case OBJ_STATE_MAIN_ACTIVE:
			// 移動処理
			switch (common::blt::MoveActBullet(m_pMainStage, blt, &m_vecCharacters, g_pLuah, GetWindValue(), &m_mapCharaScrInfo, &m_mapObjects, g_pCriticalSection, FALSE))
			{
			case common::blt::MOVE_ACT_BULLET_RESULT_DROP:			// 画面外
			case common::blt::MOVE_ACT_BULLET_RESULT_REMOVE:		// スクリプトによって削除された
			case common::blt::MOVE_ACT_BULLET_RESULT_VEC_CHANGE:	// 移動値変更
				PauseSetObjectWaitForServerEvent(blt, TRUE);
				break;
			case common::blt::MOVE_ACT_BULLET_RESULT_MOVED:	// 通常移動
			case common::blt::MOVE_ACT_BULLET_RESULT_NONE:		// 変更なし
				break;
			}
			break;
		case OBJ_STATE_MAIN_WAIT:
			switch (common::blt::MoveWaitBullet(m_pMainStage, blt, &m_vecCharacters, g_pLuah, GetWindValue(), &m_mapCharaScrInfo, &m_mapObjects, g_pCriticalSection, FALSE))
			{
			case common::blt::MOVE_WAIT_BULLET_RESULT_DROPOUT:			// 画面外
			case common::blt::MOVE_WAIT_BULLET_RESULT_REMOVED:		// スクリプトによって削除された
			case common::blt::MOVE_WAIT_BULLET_RESULT_VEC_CHANGE:	// 移動値変更
				PauseSetObjectWaitForServerEvent(blt, TRUE);
				break;
//			case common::blt::MOVE_ACT_BULLET_RESULT_MOVED:	// 通常移動
//			case common::blt::MOVE_ACT_BULLET_RESULT_NONE:		// 変更なし
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
}

void CGame::PauseSetObjectWaitForServerEvent(int obj_no, BOOL bPause, WORD wFrame)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'4');
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())
	{
		g_pCriticalSection->LeaveCriticalSection_Object();
		WCHAR log[64];
		SafePrintf(log, 64,L"PauseSetObjectWaitForServerEvent not found:%d", obj_no);
		return;
	}
	type_obj* obj = (type_blt*)(*itfind).second;
	PauseSetObjectWaitForServerEvent(obj, bPause, wFrame, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Object();
}

void CGame::PauseSetObjectWaitForServerEvent(type_obj* obj, BOOL bPause, WORD wFrame, BOOL bCriticalSectionEntered)
{
	if (!bCriticalSectionEntered)
		g_pCriticalSection->EnterCriticalSection_Object(L'5');
	DWORD flg = 0;
	if (bPause)
		flg = (DWORD)obj->obj_state | OBJ_STATE_MAIN_PAUSE_FLG;
	else
	{
		flg = (DWORD)obj->obj_state & (OBJ_STATE_MAIN_PAUSE_FLG^0xFFFFFFFF);
		obj->frame_count = wFrame;
	}
	WCHAR log[64];
	SafePrintf(log, 64, L"PauseSet:[%d]-[%x]>[%x][frame:%d][1:%d][2:%d]", obj->obj_no, obj->obj_state, flg, obj->frame_count, obj->extdata1, obj->extdata2);
	AddMessageLog(log);
	obj->obj_state = (E_TYPE_OBJ_STATE)flg;
	if (!bCriticalSectionEntered)
		g_pCriticalSection->LeaveCriticalSection_Object();
}

bool CGame::UpdateBulletAngle(int blt_no, int angle)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'6');
	std::map< int, type_obj* >::iterator itfind = m_mapObjects.find(blt_no);

	if (itfind == m_mapObjects.end())
	{
		MessageBoxA(NULL, "UpdateBulletAngle", "map object find error", MB_OK);
		g_pCriticalSection->LeaveCriticalSection_Object();
		return false;
	}
	// 削除フラグ立っているか
	if ( !((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
	{
		type_blt* blt = (type_blt*)(*itfind).second;
		blt->angle = angle;
		blt->proc_flg |= PROC_FLG_OBJ_UPDATE_ANGLE;
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
	return true;
}

bool CGame::SetBulletTextureIndex(int blt_no, int index)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'7');
	std::map< int, type_obj* >::iterator itfind = m_mapObjects.find(blt_no);
	if (itfind == m_mapObjects.end())
	{
		MessageBoxA(NULL, "SetBulletTextureIndex", "map object find error", MB_OK);
		g_pCriticalSection->LeaveCriticalSection_Object();
		return false;
	}
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
	{
		type_blt* blt = (type_blt*)(*itfind).second;
		blt->tex_index = (BYTE)index;
		blt->proc_flg |= PROC_FLG_OBJ_SET_TEX_INDEX;
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
	return true;
}
// Luaからの処理フラグを立てる
bool CGame::SetObjectLuaFlg(int obj_no, DWORD flg, BOOL on)
{
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())	return false;
	common::obj::SetLuaFlg((*itfind).second, flg, on);
	return true;
}

void CGame::UpdateObjectVector(int nObjNo, short ax, short ay, short vx, short vy)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'8');
	std::map < int, type_obj* >::iterator itfind = m_mapObjects.find(nObjNo); 
	if (itfind == m_mapObjects.end())
	{
		UpdateObjectNo();
		g_pCriticalSection->LeaveCriticalSection_Object();
		AddMessageLog(L"map find error\UpdateObjectVector()");
		return;
	}
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
	{
		(*itfind).second->ax = ax;
		(*itfind).second->ay = ay;
		(*itfind).second->vx = vx;
		(*itfind).second->vy = vy;
		if ((*itfind).second->obj_type & OBJ_TYPE_BLT)
		{
			((ptype_blt)(*itfind).second)->bx = ax*BLT_POS_FACT_N;
			((ptype_blt)(*itfind).second)->by = ay*BLT_POS_FACT_N;
		}
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
}

void CGame::UpdateBulletVector(int nObjNo, short ax, short ay, short vx, short vy, short adx, short ady)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'9');
	std::map < int, type_obj* >::iterator itfind = m_mapObjects.find(nObjNo); 
	if (itfind == m_mapObjects.end())
	{
		UpdateObjectNo();
		g_pCriticalSection->LeaveCriticalSection_Object();
		AddMessageLog(L"map find error\UpdateBulletVector()");
		return;
	}
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
	{
		type_blt* blt = (type_blt*)((*itfind).second);
		blt->ax = ax;
		blt->ay = ay;
		blt->vx = vx;
		blt->vy = vy;
		blt->adx = (char)adx;
		blt->ady = (char)ady;
		blt->bx = ax*BLT_VEC_FACT_N;
		blt->by = ay*BLT_VEC_FACT_N;
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
}

void CGame::RenderObject(ptype_obj obj, D3DXMATRIX* matStage)
{
	if (!obj)	return;
	
	if (obj->obj_type & OBJ_TYPE_BLT)
	{
		ptype_blt blt = (ptype_blt)obj;
		switch (blt->proc_type)
		{
		case BLT_PROC_TYPE_SCR_CHARA:
		case BLT_PROC_TYPE_SCR_STAGE:
		case BLT_PROC_TYPE_SCR_SPELL:
			OnRenderScrBullet(blt, matStage);
			break;
		case BLT_PROC_TYPE_ITEM:
			OnRenderItemBullet(blt, matStage);
			break;
		}
	}
}

void CGame::OnRenderScrBullet(type_blt* blt, D3DXMATRIX* matStage)
{
	// TACTICは描画しない
	if (blt->obj_type & OBJ_TYPE_TACTIC)	return;

	// 弾描画前にスクリプトに通知
	NotifyRenderScrBullet(blt);

	if (blt->scrinfo)
	{
		D3DXMATRIX mat, matR, matT, matT2, matDraw;
		// 傾きD3DXMATRIX matR, matT, matT2;
		// 変換行列初期化
		::D3DXMatrixIdentity(&mat);
		// 1. 一度スプライトの中心点をウィンドウの原点(0,0)にあわせる
		::D3DXMatrixTranslation(&matT, (float)(-blt->ax), (float)(-blt->ay), 0.0f);
		// 2. スプライトを回転させる
		::D3DXMatrixRotationZ(&matR, D3DXToRadian(blt->angle));
		// 3. 1でずらした分を元に戻す
		::D3DXMatrixTranslation(&matT2,blt->ax, blt->ay, 0.0f);
		// 行列の合成
		matDraw = matT * matR * matT2 * (*matStage);

		RECT rcTexture;
		RECT rcTexInfo;
		switch (blt->proc_type)
		{
		case  BLT_PROC_TYPE_SCR_CHARA:
			CopyRect(&rcTexInfo, &((TCHARA_SCR_INFO*)blt->scrinfo)->blt_info[blt->bullet_type].rec_blt_tex);
			break;
		case  BLT_PROC_TYPE_SCR_SPELL:
			CopyRect(&rcTexInfo, &((TCHARA_SCR_INFO*)blt->scrinfo)->sc_info.rec_blt_tex);
			break;
		case BLT_PROC_TYPE_SCR_STAGE:
			CopyRect(&rcTexInfo, &((TSTAGE_SCR_INFO*)blt->scrinfo)->blt_info[blt->bullet_type].rec_blt_tex);
			break;
		}
			// 使用テクスチャ上の描画範囲
		SetRect(&rcTexture,
			rcTexInfo.left+(blt->tex_index*rcTexInfo.right),
			rcTexInfo.top,//+(blt->tex_index*rcTexInfo.bottom),
			rcTexInfo.left+((blt->tex_index+1)*rcTexInfo.right),
			rcTexInfo.top+/*((blt->tex_index+1)**/rcTexInfo.bottom//)
			);
		
		g_pSprite->SetTransform(&matDraw);
		g_pSprite->Draw(
			blt->scrinfo->pTexture,
			&rcTexture,
			&D3DXVECTOR3((float)rcTexInfo.right,(float)rcTexInfo.bottom,0),
			&D3DXVECTOR3(blt->ax+(float)(rcTexInfo.right/2), blt->ay+(float)(rcTexInfo.bottom/2), 0),
			0xFFFFFFFF);
		g_pSprite->SetTransform(&mat);
	}
#if _DEBUG
	else
	{
		OutputDebugStr(L"RenderObject::FindCharaScrInfo not found");
	}
#endif
}

// 弾の描画
void CGame::NotifyRenderScrBullet(type_blt* blt)
{
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	int vec_angle = GetAngle((double)blt->vx, (double)blt->vy);	// 進んでいる方向（角度）を算出
	TCHARA_SCR_INFO* pScrInfo = (TCHARA_SCR_INFO*)blt->scrinfo;

	// 弾描画前イベント
	switch (blt->proc_type)
	{
	case BLT_PROC_TYPE_SCR_CHARA:
		luaParams.Number(pScrInfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number((DWORD)blt->obj_state&OBJ_STATE_MAIN_MASK).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(vec_angle).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
		common::scr::CallLuaFunc(g_pLuah, "onDraw_CharaBullet", &luaResults, 0, &luaParams, g_pCriticalSection);
		break;
	case BLT_PROC_TYPE_SCR_SPELL:
		luaParams.Number(pScrInfo->scr_index).Number(blt->obj_no).Number((DWORD)blt->obj_state&OBJ_STATE_MAIN_MASK ).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(vec_angle).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
		common::scr::CallLuaFunc(g_pLuah, "onDraw_CharaSpell", &luaResults, 0, &luaParams, g_pCriticalSection);
		break;
	case BLT_PROC_TYPE_SCR_STAGE:
		luaParams.Number(pScrInfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number((DWORD)blt->obj_state&OBJ_STATE_MAIN_MASK).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(vec_angle).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
		common::scr::CallLuaFunc(g_pLuah, "onDraw_StageBullet", &luaResults, 0, &luaParams, g_pCriticalSection);
		break;
	}
}

void CGame::OnRenderItemBullet(type_blt* blt, D3DXMATRIX* matStage)
{
	int blt_type = blt->bullet_type;
	int chr_type = blt->chara_type;
	
	DWORD dwColor = 0xFFFFFFFF;
	RECT rcTexture;
	switch (blt->bullet_type)
	{
	case CHARA_STATE_ITEM_REVERSE_INDEX:
		SetRect(&rcTexture,TEXTURE_BLT_REVERSE_RECT);
		blt->angle = (blt->ax>0)?blt->frame_count*30:blt->frame_count*-30;
		if (!(blt->frame_count % 3))
		{
			int nEffectNo = AddEffect(DEF_SYSTEM_ID, &rcTexture, &D3DXVECTOR3((float)blt->ax, (float)blt->ay,0), FRAMES/2, TRUE);
			SetEffectFade(nEffectNo, -20, TRUE);
		}
		break;
	case CHARA_STATE_ITEM_BLIND_INDEX:
		dwColor = 0x3FFFFFFF;
		SetRect(&rcTexture,TEXTURE_BLT_BLIND_RECT);
		break;
	case CHARA_STATE_ITEM_REPAIRBLT_INDEX:
//		dwColor = 0x7FFFFFFF;
		SetRect(&rcTexture,TEXTURE_BLT_REPAIR_RECT);
		blt->angle = (blt->ax>0)?blt->frame_count*20:blt->frame_count*-20;
		break;
	case CHARA_STATE_ITEM_TELEPORT_INDEX:
		blt->angle = (short)GetAngle((double)blt->vx, (double)blt->vy);
		SetRect(&rcTexture,TEXTURE_BLT_TELEPORT_RECT);
		break;
	case CHARA_STATE_ITEM_DRAIN_INDEX:
		blt->angle = (short)GetAngle((double)blt->vx, (double)blt->vy);
		SetRect(&rcTexture,TEXTURE_BLT_DRAIN_RECT);
		break;
	case CHARA_STATE_ITEM_FETCH_INDEX:
		blt->angle = (short)GetAngle((double)blt->vx, (double)blt->vy);
		SetRect(&rcTexture,TEXTURE_BLT_FETCH_RECT);
		break;
	case CHARA_STATE_ITEM_EXCHANGE_INDEX:
		blt->angle = (short)GetAngle((double)blt->vx, (double)blt->vy);
		SetRect(&rcTexture,TEXTURE_BLT_EXCHANGE_RECT);
		break;
	case CHARA_STATE_ITEM_NOANGLE_INDEX:
		blt->angle = (blt->ax>0)?blt->frame_count*10:blt->frame_count*-10;
		SetRect(&rcTexture,TEXTURE_BLT_NOANGLE_RECT);
		break;
	case CHARA_STATE_ITEM_NOMOVE_INDEX:
		blt->angle = (blt->ax>0)?blt->frame_count*10:blt->frame_count*-10;
		SetRect(&rcTexture,TEXTURE_BLT_NOMOVE_RECT);
		break;
	}

	D3DXMATRIX mat, matR, matT, matT2, matDraw;
	// 傾きD3DXMATRIX matR, matT, matT2;
	// 変換行列初期化
	::D3DXMatrixIdentity(&mat);
	// 1. 一度スプライトの中心点をウィンドウの原点(0,0)にあわせる
	::D3DXMatrixTranslation(&matT, (float)(-blt->ax), (float)(-blt->ay), 0.0f);
	// 2. スプライトを回転させる
	::D3DXMatrixRotationZ(&matR, D3DXToRadian(blt->angle));
	// 3. 1でずらした分を元に戻す
	::D3DXMatrixTranslation(&matT2,blt->ax, blt->ay, 0.0f);
	// 行列の合成
	matDraw = matT * matR * matT2 * (*matStage);

	g_pSprite->SetTransform(&matDraw);
	g_pSprite->Draw(
		m_pDefaultGUIResourceTexture,
		&rcTexture,
		&D3DXVECTOR3((float)BLT_DEFAULT_W,(float)BLT_DEFAULT_H,0),
		&D3DXVECTOR3(blt->ax+(float)(BLT_DEFAULT_W/2), blt->ay+(float)(BLT_DEFAULT_H/2), 0),
		dwColor);
	g_pSprite->SetTransform(&mat);
}

bool CGame::SetBulletOptionData(int obj_no, int index, DWORD data)
{
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())	return false;
	
	if (index >= OBJ_OPTION_COUNT)
	{
		MessageBox(NULL, L"SetBulletOptionDataのindex値を超えています", L"lua error", MB_OK); 
		return false;
	}
//	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
	((ptype_blt)(*itfind).second)->option[index] = data;
	return true;
}

bool CGame::SetCharaOptionData(int obj_no, int index, DWORD data)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return false;
	if (index >= OBJ_OPTION_COUNT)
	{
		MessageBox(NULL, L"SetBulletOptionDataのindex値を超えています", L"lua error", MB_OK); 
		return false;
	}
	sess->option[index] = data;
	return true;
}

unsigned int CGame::GetCharaOptionData(int obj_no, int index)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return 0;
	if (index >= OBJ_OPTION_COUNT)
	{
		MessageBox(NULL, L"SetBulletOptionDataのindex値を超えています", L"lua error", MB_OK); 
		return false;
	}
	return sess->option[index];
}


bool CGame::SetBulletExtData1(int obj_no, DWORD extdata1)
{
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())	return false;
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
		((ptype_blt)(*itfind).second)->extdata1 = extdata1;
	return true;
}

bool CGame::SetBulletExtData2(int obj_no, DWORD extdata2)
{
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())	return false;
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
		((ptype_blt)(*itfind).second)->extdata2 = extdata2;
	return true;
}

unsigned int CGame::GetCharaExtData1(int obj_no)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return 0;
	return sess->extdata1;
}

unsigned int CGame::GetCharaExtData2(int obj_no)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return 0;
	return sess->extdata2;
}

bool CGame::SetCharaExtData1(int obj_no, DWORD extdata1)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return false;
	sess->extdata1 = extdata1;
	return true;
}

bool CGame::SetCharaExtData2(int obj_no, DWORD extdata2)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return false;
	sess->extdata2 = extdata2;
	return true;
}

int CGame::GetCharaState(int obj_no, int chr_stt)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return -1;
	if (chr_stt < 0 || chr_stt >= CHARA_STATE_COUNT) return -1;
	return sess->chara_state[chr_stt];
}

unsigned int CGame::GetCharaItem(int obj_no, int item_index)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return 0;
	if (item_index < 0 || item_index >= GAME_ITEM_STOCK_MAX_COUNT) return 0;
	return (unsigned long)sess->items[item_index];
}

bool CGame::RemoveObject(int nObjNo)
{
	bool ret = true;
	g_pCriticalSection->EnterCriticalSection_Object(L'0');
	std::map < int, type_obj* >::iterator itfind = m_mapObjects.find(nObjNo);
	if (itfind == m_mapObjects.end())
	{
		UpdateObjectNo();
		g_pCriticalSection->LeaveCriticalSection_Object();
		return false;
	}

	type_obj* obj = (*itfind).second;
	if (obj->proc_flg & PROC_FLG_OBJ_REMOVE)
		ret = false;
	else
		obj->proc_flg |= PROC_FLG_OBJ_REMOVE;
	if (m_pFocusObject == obj)
	{
		m_bScrFocus = FALSE;
		m_pFocusObject = NULL;
		AddMessageLog(L"RemoveFocus");
#if (E0401==0)

		if (m_bBulletFocus
			&& !m_bPauseBulletFocus
			&& m_vecObjectNo.size() <= BLT_FOCUS_OBJ_RM_TO_ACTIVE_COUNT_MAX)
		{
			// 弾ループ
			std::vector<int> vecList;
			vecList.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
			
			for (std::vector<int>::iterator itno = vecList.begin();
				itno != vecList.end();
				itno++
				)
			{
				std::map<int, type_obj*>::iterator it = m_mapObjects.find((*itno));
				if (it == m_mapObjects.end())	continue;
				ptype_obj obj = (ptype_obj)(*it).second;
				if (obj->proc_flg & PROC_FLG_OBJ_REMOVE)
					continue;
				if (!(obj->obj_type & OBJ_TYPE_BLT_TACTIC) && obj->obj_state & OBJ_STATE_MAIN_ACTIVE_FLG)
				{
					AddMessageLog(L"ChangeFocus");
					m_pFocusObject = obj;
					break;
				}			
			}
		}
#else
		// 弾ループ
		std::vector<int> vecList;
		vecList.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
		for (std::vector<int>::iterator itno = vecList.begin();
			itno != vecList.end();
			itno++
			)
		{
			std::map<int, type_obj*>::iterator it = m_mapObjects.find((*itno));
			if (it == m_mapObjects.end())	continue;
			ptype_obj obj = (ptype_obj)(*it).second;
			if (obj->proc_flg & PROC_FLG_OBJ_REMOVE)
				continue;
			if (obj->obj_state & OBJ_STATE_MAIN_ACTIVE_FLG)
			{
				AddMessageLog(L"ChangeFocus");
				m_pFocusObject = obj;
				break;
			}			
		}
#endif
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
	return ret;
}

// 爆発で地面削除
BOOL CGame::BombObject(int scr_id, int blt_type, int blt_chr_no, int blt_no, int pos_x,int pos_y, BOOL erase, BYTE* data)
{
//	HRESULT hr;
	
	int range = 0;
	TCHARA_SCR_INFO* pCharaScrInfo = NULL;
	type_blt* blt = NULL;
	// 弾を検索
//	g_pCriticalSection->EnterCriticalSection_Lua();
	g_pCriticalSection->EnterCriticalSection_Object(L'-');
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(blt_no);
	if (itfind != m_mapObjects.end())
	{
		if (!((*itfind).second->obj_type & OBJ_TYPE_BLT))
		{
			g_pCriticalSection->LeaveCriticalSection_Object();
//			g_pCriticalSection->LeaveCriticalSection_Lua();
			AddMessageLog(L"bomb obj is no bullet");
			return FALSE;
		}
		blt = (type_blt*)((*itfind).second);
	}
	// 半径をとる
	if (blt_chr_no == STAGE_OBJ_NO)
		range = m_pSelectedStageScrInfo->blt_info[blt_type].bomb_range;
	else
	{
		pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(scr_id, &m_mapCharaScrInfo);
		if (!pCharaScrInfo)
		{
			g_pCriticalSection->LeaveCriticalSection_Object();
//			g_pCriticalSection->LeaveCriticalSection_Lua();
			AddMessageLog(L"BombObject() キャラスクリプト検索エラー");
			return FALSE;
		}
		if (blt_type != DEF_BLT_TYPE_SPELL)
			range = pCharaScrInfo->blt_info[blt_type].bomb_range;
		else
			range = pCharaScrInfo->sc_info.bomb_range;
	}

	// 爆発半径ありか
	int nRetStageErasePixels = 0;
	if (range && erase)
	{
		while (DXUTIsDeviceLost())
		{
			AddMessageLog(L"BombObject:DeviceLost,Wait DeviceReset");
			Sleep(50);
		}

		int yc = (int)floor(pos_y+0.5);
		if (m_pMainStage->GetStageHeight() <= yc-range || 0 >= yc+range)
		{
			g_pCriticalSection->LeaveCriticalSection_Object();
//			g_pCriticalSection->LeaveCriticalSection_Lua();
			return TRUE;
		}
		int ys	=	max(0, -range+yc);
		int ye	=	min(m_pMainStage->GetStageHeight(), range+yc);
		int xc = (int)floor(pos_x+0.5);
		if (m_pMainStage->GetStageWidth() <= xc-range || 0 >= xc+range)
		{
			g_pCriticalSection->LeaveCriticalSection_Object();
//			g_pCriticalSection->LeaveCriticalSection_Lua();
			return TRUE;
		}
		int xs	=	max(0, -range+xc);
		int xe	=	min(m_pMainStage->GetStageWidth(), range+xc);

		if (g_bDebug)
		{
			WCHAR log[80];
			SafePrintf(log, 80, L"StageErase(x%d,y%d,r%d)", xc,yc,range);
			AddMessageLog(log);
		}

		nRetStageErasePixels = m_pMainStage->EraseStage(&D3DXVECTOR2((float)xc,(float)yc), range);
		if (nRetStageErasePixels)
		{
			// 削れたピクセル数をログ追加
			if (g_bDebug)
			{
				WCHAR log[80];
				SafePrintf(log, 80, L"StageErased(%d) pxs & CpyTex[%d,%d,%d,%d]", nRetStageErasePixels,xs,ys,xe,ye);
				AddMessageLog(log);
			}
			RECT rcCopyRect = {xs,ys,xe,ye};
			g_pCriticalSection->EnterCriticalSection_StageTexture(L'\"');
			BOOL bRetCopyToTexture = PngLoader::CopyToTexture(m_pDev, &rcCopyRect, xs, ys, m_pMainStage->GetImage(), m_pStageTexture);
			g_pCriticalSection->LeaveCriticalSection_StageTexture();

			// コピー失敗ログ
			if (!bRetCopyToTexture)
				AddMessageLog(L"FailedErasedTextureCopyProc");

			// ステージスクリプトへステージ削除イベント
			LuaFuncParam luaParams;
			LuaFuncParam luaResults;
			
			// キャラが発射した弾の場合、ステージ削除イベントを起こす
//> 20110420 ステージが作成した弾の場合、onErase_Stageが呼び出されなかったのを修正
//			if (blt_chr_no != STAGE_OBJ_NO)
//< 20110420 ステージが作成した弾の場合、onErase_Stageが呼び出されなかったのを修正
			{
				// 弾を作ったキャラのスクリプト番号,弾を作ったキャラのObjNo,当たった弾のObjNo,弾のタイプ,当たった弾の位置x,y/爆発位置x,y/範囲の近さによる威力値/extdata/削除ピクセル数
				luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt_type).Number(blt_chr_no).Number(blt_no).Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
				if (!common::scr::CallLuaFunc(g_pLuah, "onErase_Stage", &luaResults, 0, &luaParams, g_pCriticalSection))
				{
					g_pCriticalSection->LeaveCriticalSection_Object();
//					g_pCriticalSection->LeaveCriticalSection_Lua();
					return FALSE;
				}
			}
		}
		else if (g_bDebug)
		{
			AddMessageLog(L"StageNoErased");
		}
	}

	// 爆発範囲を得る、爆発イベントを通知する
	// ステージが作成した弾か
	if (blt_chr_no == STAGE_OBJ_NO)
	{
		LuaFuncParam luaParams, luaResults;
		// 存在しているなら情報取得
		// script,弾タイプ,弾ObjNo,弾位置x,y/移動x,y/extdata
		if (blt)
			luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
		else
			luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt_type).Number(-1).Number(pos_x).Number(pos_y).Nil().Nil().Nil().Nil().Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
		if (!common::scr::CallLuaFunc(g_pLuah, "onBomb_StageBullet", &luaResults, 0, &luaParams, g_pCriticalSection))
		{
			g_pCriticalSection->LeaveCriticalSection_Object();
//			g_pCriticalSection->LeaveCriticalSection_Lua();
			return FALSE;
		}
	}
	else
	{
		LuaFuncParam luaParams;
		LuaFuncParam luaResults;
		// script,弾タイプ,弾ObjNo,弾を作ったキャラのObjNo,弾位置x,y/移動x,y/extdata
		if (blt_type != DEF_BLT_TYPE_SPELL)
		{
			if (blt)
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"onBomb_CharaBullet1(id/blt_no/blt_type):%d/%d/%d", scr_id,blt_no,blt->bullet_type);
				AddMessageLog(log);
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
			}
			else
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"onBomb_CharaBullet2(id/blt_no/blt_type):%d/%d/%d", scr_id,blt_no,blt_type);
				AddMessageLog(log);
				luaParams.Number(pCharaScrInfo->scr_index).Number(blt_type).Number(-1).Number(blt_chr_no).Number(pos_x).Number(pos_y).Nil().Nil().Nil().Nil().Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
			}
			if (!common::scr::CallLuaFunc(g_pLuah, "onBomb_CharaBullet", &luaResults, 0, &luaParams, g_pCriticalSection))
			{
				g_pCriticalSection->LeaveCriticalSection_Object();
//				g_pCriticalSection->LeaveCriticalSection_Lua();
				return FALSE;
			}
		}
		else
		{
			if (blt)
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"onBomb_CharaSpell1(id/blt_no/blt_type):%d/%d/%d", scr_id,blt->obj_no,blt_type);
				AddMessageLog(log);
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->obj_no).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
			}
			else
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"onBomb_CharaSpell2(id/blt_no/blt_type):%d/%d/-1", scr_id,blt_no,blt_type);
				AddMessageLog(log);
				luaParams.Number(pCharaScrInfo->scr_index).Number(-1).Number(blt_chr_no).Number(pos_x).Number(pos_y).Nil().Nil().Nil().Nil().Number(pos_x).Number(pos_y).Number(nRetStageErasePixels);
			}
			if (!common::scr::CallLuaFunc(g_pLuah, "onBomb_CharaSpell", &luaResults, 0, &luaParams, g_pCriticalSection))
			{
				g_pCriticalSection->LeaveCriticalSection_Object();
//				g_pCriticalSection->LeaveCriticalSection_Lua();
				return FALSE;
			}
		}
	}

	// キャラに当たった
	if (data[0]>0)
	{
		g_pCriticalSection->EnterCriticalSection_Session(L'8');

		// Luaに爆発範囲内のキャラを知らせる
		LuaFuncParam luaParams,luaResults;
		int nIndex = 1;
		int i=0;
		for (i=0; i < data[0]; i++)
		{
			WCHAR log[64];
//			nIndex = (i*3);
			short hit_chr_no;
			memcpy(&hit_chr_no, &data[nIndex], sizeof(short));
			nIndex += sizeof(short);
			float fPower = (float)data[nIndex]*0.01f;
			nIndex += sizeof(BYTE);
			SafePrintf(log, 64, L"%d:onHitChara_CharaBulletBomb(b%d,h%d,pw%f)", i,blt_chr_no, hit_chr_no, fPower);
			AddMessageLog(log);
			luaParams.Clear();
			luaResults.Clear();

			if (blt_chr_no != STAGE_OBJ_NO)	// ステージ生成の弾ではない
			{
				if (blt_type != DEF_BLT_TYPE_SPELL)
				{
					// script,弾タイプ,当たったキャラのObjNo,弾を作ったキャラのObjNo,当たった弾のObjNo,当たった弾の位置x,y/爆発位置x,y/範囲の近さによる威力値
					luaParams.Number(pCharaScrInfo->scr_index).Number(blt_type).Number(hit_chr_no).Number(blt_chr_no).Number(blt_no).Number(pos_x).Number(pos_y).Number(fPower);
					if (!common::scr::CallLuaFunc(g_pLuah, "onHitChara_CharaBulletBomb", &luaResults, 0, &luaParams, g_pCriticalSection))
					{
						g_pCriticalSection->LeaveCriticalSection_Session();
						g_pCriticalSection->LeaveCriticalSection_Object();
//						g_pCriticalSection->LeaveCriticalSection_Lua();
						return FALSE;
					}
				}
				else
				{
					// script,当たったキャラのObjNo,弾を作ったキャラのObjNo,当たった弾のObjNo,当たった弾の位置x,y/爆発位置x,y/範囲の近さによる威力値
					luaParams.Number(pCharaScrInfo->scr_index).Number(hit_chr_no).Number(blt_chr_no).Number(blt_no).Number(pos_x).Number(pos_y).Number(fPower);
					if (!common::scr::CallLuaFunc(g_pLuah, "onHitChara_CharaSpellBomb", &luaResults, 0, &luaParams, g_pCriticalSection))
					{
						g_pCriticalSection->LeaveCriticalSection_Session();
						g_pCriticalSection->LeaveCriticalSection_Object();
//						g_pCriticalSection->LeaveCriticalSection_Lua();
						return FALSE;
					}
				}
			}
			else
			{
				// script,弾タイプ,当たったキャラのObjNo,当たった弾の位置x,y/爆発位置x,y/範囲の近さによる威力値
				luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt_type).Number(hit_chr_no).Number(blt_no).Number(pos_x).Number(pos_y).Number(fPower);
				if (!common::scr::CallLuaFunc(g_pLuah,"onHitChara_StageBulletBomb", &luaResults, 0, &luaParams, g_pCriticalSection))
				{
					g_pCriticalSection->LeaveCriticalSection_Session();
					g_pCriticalSection->LeaveCriticalSection_Object();
//					g_pCriticalSection->LeaveCriticalSection_Lua();
					return FALSE;
				}
			}
		}
		g_pCriticalSection->LeaveCriticalSection_Session();
	}

	g_pCriticalSection->LeaveCriticalSection_Object();
//	g_pCriticalSection->LeaveCriticalSection_Lua();

	return TRUE;
}

void CGame::UpdateObjectState(int nObjNo, DWORD dwObjState)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'^');
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(nObjNo);
	if (itfind == m_mapObjects.end())
	{
		g_pCriticalSection->LeaveCriticalSection_Object();
		AddMessageLog(L"UpdateObjectState, map object find error");
		return;
	}
	// 削除済みでなかったら設定
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
	{
		type_obj* obj = (type_blt*)(*itfind).second;
		obj->obj_state = (E_TYPE_OBJ_STATE) (obj->obj_state&(OBJ_STATE_MAIN_MASK^0xFFFFFFFF)|(OBJ_STATE_MAIN_MASK&dwObjState));
	}

	// 弾の場合、状態が設定されたイベントをスクリプトに伝える
	if ((*itfind).second->obj_type & OBJ_TYPE_BLT)
	{
		ptype_blt blt = (type_blt*)((*itfind).second);
		// ステージが作成した弾か
		if (nObjNo == STAGE_OBJ_NO)
		{
			LuaFuncParam luaParams, luaResults;
			// 存在しているなら情報取得
			// script,弾タイプ,弾ObjNo,弾位置x,y/移動x,y/extdata/obj_state
			luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number((DWORD)blt->obj_state&OBJ_STATE_MAIN_MASK);
			if (!common::scr::CallLuaFunc(g_pLuah, "onUpdateState_StageBullet", &luaResults, 0, &luaParams, g_pCriticalSection))
			{
				g_pCriticalSection->LeaveCriticalSection_Object();
				return;
			}
		}
		else
		{
			LuaFuncParam luaParams;
			LuaFuncParam luaResults;
			// script,弾タイプ,弾ObjNo,弾を作ったキャラのObjNo,弾位置x,y/移動x,y/extdata
			if (blt->bullet_type != DEF_BLT_TYPE_SPELL)
			{
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number((DWORD)blt->obj_state&OBJ_STATE_MAIN_MASK);
				if (!common::scr::CallLuaFunc(g_pLuah, "onUpdateState_CharaBullet", &luaResults, 0, &luaParams, g_pCriticalSection))
				{
					g_pCriticalSection->LeaveCriticalSection_Object();
					return;
				}
			}
			else
			{
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->obj_no).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number((DWORD)blt->obj_state&OBJ_STATE_MAIN_MASK);
				if (!common::scr::CallLuaFunc(g_pLuah, "onUpdateState_CharaSpell", &luaResults, 0, &luaParams, g_pCriticalSection))
				{
					g_pCriticalSection->LeaveCriticalSection_Object();
					return;
				}
			}
		}
#if (E0401 == 0)
		// フォーカスしている弾がWAITに設定されていたら、新しいフォーカスする弾を探す
		if (m_pFocusObject == blt && (blt->obj_state & OBJ_STATE_MAIN_WAIT_FLG))
		{
			m_pFocusObject = NULL;
			m_bScrFocus = FALSE;
			if (m_bBulletFocus
			&& !m_bPauseBulletFocus
			&& m_vecObjectNo.size() <= BLT_FOCUS_OBJ_RM_TO_ACTIVE_COUNT_MAX
			)
			{
				// 弾ループ
				std::vector<int> vecList;
				vecList.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
				for (std::vector<int>::iterator itno = vecList.begin();
					itno != vecList.end();
					itno++
					)
				{
					std::map<int, type_obj*>::iterator it = m_mapObjects.find((*itno));
					if (it == m_mapObjects.end())	continue;
					ptype_obj obj = (ptype_obj)(*it).second;
					if (obj->proc_flg & PROC_FLG_OBJ_REMOVE)
						continue;
					if (!(obj->obj_type & OBJ_TYPE_BLT_TACTIC) && obj->obj_state & OBJ_STATE_MAIN_ACTIVE_FLG)
					{
						AddMessageLog(L"ChangeFocus");
						m_pFocusObject = obj;
						break;
					}			
				}
			}
		}
#else
		// フォーカスしている弾がWAITに設定されていたら、新しいフォーカスする弾を探す
		if (m_bBulletFocus && !m_bPauseBulletFocus
			&& m_pFocusObject == blt && (blt->obj_state & OBJ_STATE_MAIN_WAIT_FLG))
		{
			m_pFocusObject = NULL;
			// 弾ループ
			std::vector<int> vecList;
			vecList.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
			for (std::vector<int>::iterator itno = vecList.begin();
				itno != vecList.end();
				itno++
				)
			{
				std::map<int, type_obj*>::iterator it = m_mapObjects.find((*itno));
				if (it == m_mapObjects.end())	continue;
				ptype_obj obj = (ptype_obj)(*it).second;
				if (obj->proc_flg & PROC_FLG_OBJ_REMOVE)
					continue;
				if (obj->obj_state & OBJ_STATE_MAIN_ACTIVE_FLG)
				{
					AddMessageLog(L"ChangeFocus");
					m_pFocusObject = obj;
					break;
				}			
			}
		}
#endif
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
}

// ターン終了(nCharaIndex=現状自キャラしかこない)
void CGame::OnTurnEnd(int nCharaIndex)
{
	WCHAR wsText[12];
	m_pStageCharacters[nCharaIndex]->SetMyTurn(false);

	// TURN
	if (m_nLimitTurn>0)		// 制限ターン数がある場合
		SafePrintf(wsText, 12, L"%d/%d", ++m_nTurnCounter, m_nLimitTurn);
	else
		SafePrintf(wsText, 12, L"%d", ++m_nTurnCounter);
	p_pUI->GetStatic(IDC_MAIN_STATIC_TURN)->SetText(wsText);
	
	NotifyTurnEnd(GetSessionInfo( m_nActiveObjectNo));

	// 自ターン終了
	if (nCharaIndex == m_nUserIndex)
	{
		ptype_session sess = GetMySessionInfo();
		// DELAY
		SafePrintf(wsText, 12, L"%d", sess->delay);
		p_pUI->GetStatic(IDC_MAIN_STATIC_DELAY)->SetText(wsText);		
//>	20110212 移動値をクリア
		if (m_SessionArray[m_nUserIndex].obj_state != OBJ_STATE_MAIN_GALLERY)
		{
			m_SessionArray[m_nUserIndex].MV_m = ((TCHARA_SCR_INFO*)(m_SessionArray[m_nUserIndex].scrinfo))->move;
			m_pMeterMovable->SetMaxValue(m_SessionArray[m_nUserIndex].MV_m);
			m_pMeterMovable->SetValue(m_SessionArray[m_nUserIndex].MV_m);
		}
//<	20110212 移動値をクリア

		g_pCriticalSection->EnterCriticalSection_Object(L'!');
		BOOL bRemoved = FALSE;
		// 弾ループ
		std::vector<int> vecList;
		vecList.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
		for (std::vector<int>::iterator itno = vecList.begin();
			itno != vecList.end();
			itno++
			)
		{
			std::map<int, type_obj*>::iterator it = m_mapObjects.find((*itno));
			if (it == m_mapObjects.end())	continue;
			ptype_obj obj = (ptype_obj)(*it).second;
			if (obj->proc_flg & PROC_FLG_OBJ_REMOVE)
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"RemoveBullet:%d", (*it).second->obj_no);
				AddMessageLog(log);
				
				if (obj->obj_type & OBJ_TYPE_BLT)
				{
					ptype_blt blt = (ptype_blt)obj;
					WCHAR bltlog[64];
					SafePrintf(bltlog, 64, L"blt(%d)_type[#%d(%d)],chr(%d),turn:%d/rm%d", blt->obj_no, blt->chara_type, blt->bullet_type, blt->chr_obj_no, blt->turn_count,(blt->proc_flg & PROC_FLG_OBJ_REMOVE)?1:0);
					AddMessageLog(bltlog);
				}

				it = m_mapObjects.erase(it);
				if (obj->obj_type & OBJ_TYPE_BLT)
				{
					type_blt* blt = (type_blt*)obj;
					DEBUG_DELETE(blt, L"RemoveObject");
				}
				else
				{
					DEBUG_DELETE(obj, L"RemoveObject");
				}
				bRemoved = TRUE;
			}
			else
			{
				PauseSetObjectWaitForServerEvent(obj->obj_no, FALSE, obj->frame_count);
				SetObjectLuaFlg(obj->obj_no, PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS, FALSE);
			}
		}
		if (bRemoved)
			UpdateObjectNo();
		m_pFocusObject = NULL;
		g_pCriticalSection->LeaveCriticalSection_Object();
		
		// ターン終了後のコントロール表示
		m_pASpriteTimer->SetVisible(false);
		SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_DISABLE_RECT);
		p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)FALSE);

		m_eMainPhase = GAME_MAIN_PHASE_NONE;
		// ステージを暗転から戻す
		ShowStage();
	}
#if TEST_CAPTION
	WCHAR caption[65];
	SafePrintf(caption, 64, L"%d / %d / %d", m_nWindValue, m_pShotAngle->GetShotAngle(), m_pShotPowerIndicator->GetCX());
	SetWindowText(g_hWnd, caption);
#endif

}

void CGame::OnGameEnd()
{
	m_eGameState = eGameMainRelease;
}

void CGame::ReqUseItem(int nControlID)
{
	BYTE pkt[MAX_PACKET_SIZE];
	WORD			packetSize = 0;
	if (!(m_bytRuleFlg & GAME_RULE_ITEM_ENABLE))
		return;
	
	// アクティブ時以外は無効
	if (m_nUserIndex != GetSessionIndex(m_nActiveObjectNo))	return;
	// 弾の発射パワー動かす前以外は使用不可
	if (m_eMainPhase != GAME_MAIN_PHASE_ACT) return;
	if (m_SessionArray[m_nUserIndex].obj_state != OBJ_STATE_MAIN_ACTIVE)	return;

	CDXUTButton *pBtn = p_pUI->GetButton(nControlID);
	if (!pBtn->GetVisible())	return;	

	int nIndex = nControlID- IDC_MAIN_BTN_MY_ITEM_BASE;
	if (m_SessionArray[m_nUserIndex].items[nIndex] & GAME_ITEM_BULLET_TYPE_FLG)
	{
		// アイテム弾装填済みならアイテム使用不可
		int nBltType = GetStockItemBullet();
		if (nBltType != -1)
		{
			AddChatMessage(L"既にアイテム弾が装填済みです", PK_USER_CHAT_SERVER_WARNING);
			PlaySysSoundSE(SE_sai_SrvInfo);
			return;
		}
	}
	else if ((m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_SHIELD_INDEX])
		&& m_SessionArray[m_nUserIndex].items[nIndex] & GAME_ITEM_SHIELD)
	{
		AddChatMessage(L"シールドは既に使用済みです", PK_USER_CHAT_SERVER_WARNING);
		PlaySysSoundSE(SE_sai_SrvInfo);
		return;
	}
	else if ((m_SessionArray[m_nUserIndex].items[nIndex] & GAME_ITEM_POWER)
		&& m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_POWER_INDEX])
	{
		AddChatMessage(L"既にパワーアップ状態です", PK_USER_CHAT_SERVER_WARNING);
		PlaySysSoundSE(SE_sai_SrvInfo);
		return;
	}
	else if ((m_SessionArray[m_nUserIndex].items[nIndex] & GAME_ITEM_DOUBLE_SHOT)
		&& m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_DOUBLE_INDEX])
	{
		AddChatMessage(L"既に連射状態です", PK_USER_CHAT_SERVER_WARNING);
		PlaySysSoundSE(SE_sai_SrvInfo);
		return;
	}
	else if ((m_SessionArray[m_nUserIndex].items[nIndex] & GAME_ITEM_STEAL)
		&& (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_ITEM_STEAL_INDEX]&GAME_ITEM_STEAL_SET))
	{
		AddChatMessage(L"既にスティール状態です", PK_USER_CHAT_SERVER_WARNING);
		PlaySysSoundSE(SE_sai_SrvInfo);
		return;
	}

	pBtn->SetEnabled(false);

	packetSize = PacketMaker::MakePacketData_MainInfoItemOrder(nIndex, pkt);
	if (packetSize)
		AddPacket(pkt, packetSize);
}

void CGame::SetCharacterDead(int nSessIndex, E_TYPE_PACKET_MAININFO_HEADER dead_type)
{
	WCHAR dlog[32];
	SafePrintf(dlog, 32, L"DeadSess:%d/Type:%d", nSessIndex, (int)dead_type);
	AddMessageLog(dlog);

	if (ptype_session sess = GetSessionObj(nSessIndex))
	{
		sess->HP_c = 0;
		DWORD prev_state = (DWORD)sess->obj_state;
		switch (dead_type)
		{
		case PK_USER_MAININFO_CHARA_DEAD_CLOSE:
			sess->obj_state = OBJ_STATE_MAIN_DEAD;
			sess->frame_count = 0;
			NotifyDeadScrChara(sess, CHARA_DEAD_CLOSE, prev_state);
			break;
		case PK_USER_MAININFO_CHARA_DEAD_KILL:
			sess->obj_state = OBJ_STATE_MAIN_DEAD;
			sess->frame_count = 0;
			NotifyDeadScrChara(sess, CHARA_DEAD_KILL, prev_state);
			break;
		case PK_USER_MAININFO_CHARA_DEAD_DROP:
			sess->obj_state = OBJ_STATE_MAIN_DROP;
			sess->frame_count = 0;
			NotifyDeadScrChara(sess, CHARA_DEAD_DROP, prev_state);
			break;
		}

		// キャラ状態をクリア
		ZeroMemory(sess->chara_state, sizeof(char)*CHARA_STATE_COUNT);
		// ステージキャラを死んだ状態に変更
		if (m_pStageCharacters[sess->sess_index] && m_pStageCharacters[sess->sess_index]->IsCreated())
		{
			m_pStageCharacters[sess->sess_index]->Update();
			m_pStageCharacters[sess->sess_index]->SetDead();
		}
		if (sess->sess_index == m_nUserIndex)
			UpdateControlPanelHPInfo();
	}
}

void CGame::UpdateObjectType(int obj_no, BYTE type)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'#');
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())
	{
		g_pCriticalSection->LeaveCriticalSection_Object();
		AddMessageLog(L"UpdateObjectType, map object find error");
		return;
	}
	if (!((*itfind).second->proc_flg & PROC_FLG_OBJ_REMOVE))
		(*itfind).second->obj_type = type;

	// 弾の場合、状態が設定されたイベントをスクリプトに伝える
	if ((*itfind).second->obj_type & OBJ_TYPE_BLT)
	{
		ptype_blt blt = (type_blt*)((*itfind).second);
		// ステージが作成した弾か
		if (blt->obj_no == (short)STAGE_OBJ_NO)
		{
			LuaFuncParam luaParams, luaResults;
			// 存在しているなら情報取得
			// script,弾タイプ,弾ObjNo,弾位置x,y/移動x,y/extdata/obj_state
			luaParams.Number(m_pSelectedStageScrInfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number((BYTE)(OBJ_TYPE_MASK&type));
			if (!common::scr::CallLuaFunc(g_pLuah, "onUpdateType_StageBullet", &luaResults, 0, &luaParams, g_pCriticalSection))
			{
				g_pCriticalSection->LeaveCriticalSection_Object();
				return;
			}
		}
		else
		{
			LuaFuncParam luaParams;
			LuaFuncParam luaResults;
			// script,弾タイプ,弾ObjNo,弾を作ったキャラのObjNo,弾位置x,y/移動x,y/extdata
			if (blt->bullet_type != DEF_BLT_TYPE_SPELL)
			{
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number((BYTE)(OBJ_TYPE_MASK&type));
				if (!common::scr::CallLuaFunc(g_pLuah, "onUpdateType_CharaBullet", &luaResults, 0, &luaParams, g_pCriticalSection))
				{
					g_pCriticalSection->LeaveCriticalSection_Object();
					return;
				}
			}
			else
			{
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->obj_no).Number(blt->chr_obj_no).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2).Number((BYTE)(OBJ_TYPE_MASK&type));
				if (!common::scr::CallLuaFunc(g_pLuah, "onUpdateType_CharaSpell", &luaResults, 0, &luaParams, g_pCriticalSection))
				{
					g_pCriticalSection->LeaveCriticalSection_Object();
					return;
				}
			}
		}
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
}

type_session* CGame::GetCharacterAtVector(int nCharaIndex)
{
	if ((int)m_vecCharacters.size() <= nCharaIndex || nCharaIndex < 0)
		return NULL;
	return (type_session*)m_vecCharacters[nCharaIndex];
}

int CGame::GetEntityCharacters()
{
	return m_vecCharacters.size();
}

int CGame::GetLivingCharacters()
{
	int nCount=0;
	for (std::vector< type_session* >::iterator it = m_vecCharacters.begin();
		it != m_vecCharacters.end();
		it++)
	{
		if ( (*it)->entity
			&& !((*it)->obj_state & (OBJ_STATE_MAIN_NOLIVE_FLG|OBJ_STATE_EMPTY)))
			nCount++;
	}
	return nCount;
}

// ステージにテクスチャ貼り付け
void CGame::PasteTextureOnStage(int scr_id,int sx,int sy, int tx,int ty,int tw,int th)
{
	int stage_top = max(sy,0);
	int stage_bottom = min(m_pMainStage->GetHeight(), sy+th);
	if (stage_top >= stage_bottom)
		return;
	int stage_left = max(sx,0);
	int stage_right = min(m_pMainStage->GetWidth(), sx+tw);
	if (stage_left >= stage_right)
		return;
	RECT rcLock;
	SetRect(&rcLock, stage_left, stage_top, stage_right, stage_bottom);
	RECT rcPasteImage = {tx, ty, tw, th};
	LPDIRECT3DTEXTURE9 pPasteTexture = NULL;

	switch (scr_id)
	{
	case DEF_STAGE_ID:
		pPasteTexture = m_pSelectedStageScrInfo->pTexture;
		break;
	case DEF_SYSTEM_ID:
		pPasteTexture = g_DialogResourceManager.GetTextureNode(0)->pTexture;
		break;
	default:
		{
			// キャラのスクリプト取得
			TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(scr_id, &m_mapCharaScrInfo);
			pPasteTexture = pCharaScrInfo->pTexture;
		}
		break;
	}
	WCHAR log[64];
	SafePrintf(log, 64, L"Paste(src{%d,%d,%d,%d},dst{%d,%d}", tx,ty,tw,th, sx,sy);
	AddMessageLog(log);

	g_pCriticalSection->EnterCriticalSection_StageTexture(L'#');
	
	// 画像貼り付け
//	if (!m_pMainStage->PasteImage(g_pFiler, pCharaScrInfo->tex_path, &rcPasteImage, sx,sy))
	if (!m_pMainStage->PasteImage(m_pDev, &rcPasteImage, sx,sy, pPasteTexture))
	{
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		AddMessageLog(L"ステージ画像の操作に失敗しました");
		return;
	}
	SetRect(&rcPasteImage, sx,sy,sx+rcPasteImage.right-rcPasteImage.left, sy+rcPasteImage.bottom-rcPasteImage.top);
	PngLoader::CopyToTexture(m_pDev, &rcPasteImage, sx,sy, m_pMainStage->GetImage(), m_pStageTexture);
/*
	// サーフェースロック
	D3DLOCKED_RECT LockedRect;
	HRESULT hr = m_pStageTexture->LockRect(0, &LockedRect, &rcLock,0);
	if (hr != D3D_OK)
	{
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		if (hr == D3DERR_WASSTILLDRAWING)
			return;
		AddMessageLog(L"ステージ画像の操作に失敗しました");
		return;
	}
	DWORD* p = (DWORD*)LockedRect.pBits;
	const TPngImage* pStagePngImage = m_pMainStage->GetImage();

	size_t sizePasteLine = (size_t)((stage_right-stage_left)*sizeof(DWORD));
	int stage_height = stage_bottom-stage_top;
	for (int y=0;y<stage_height;y++)
	{
		// 転送先
		DWORD* p1 = (DWORD*)((char*)LockedRect.pBits
							+ ((LockedRect.Pitch) * (y)));
		// 転送元
		DWORD* src_line = pStagePngImage->lines[y+stage_top];
		src_line += stage_left;
		memcpy(p1, src_line, sizePasteLine);
	}
	hr = m_pStageTexture->UnlockRect(0);
*/
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
}

// 弾の表示
void CGame::SetVisibleBulletSelectButtons(bool value)
{
	TCHARA_SCR_INFO* scrinfo = (TCHARA_SCR_INFO*)m_SessionArray[m_nUserIndex].scrinfo;	
	
	POINT pnt;
	pnt.x = MAIN_RB_SEL_BULLET_CNT_X;
	pnt.y = MAIN_RB_SEL_BULLET_CNT_Y;


	if (value)
	{
		m_pSelBulletBtnList->Clear();
		m_pSelBulletBtnList->Create(
			p_pUI->GetButton(IDC_MAIN_RB_SEL_BULLET_L),
			p_pUI->GetButton(IDC_MAIN_RB_SEL_BULLET_R),
			&pnt,
			MAIN_RB_SEL_BULLET_CNT_W,
			0, 3);
	}

	// 弾選択ボタン
	for (int i=0;i<MAX_CHARA_BULLET_TYPE;i++)
	{
/*
		m_pBulletSelectButtons[i]->SetVisible(value);

		if (scrinfo->blt_type_count<=i)
		{
			if (m_pBulletSelectButtons[i]->GetVisible())
				m_pBulletSelectButtons[i]->SetVisible(false);
			continue;
		}
		else if (!value)
			continue;
*/
		m_pBulletSelectButtons[i]->SetVisible(false);
		if (scrinfo->blt_sel_count<=i)
			continue;
		
		RECT rcBullet;
		SetRect(&rcBullet,
			scrinfo->blt_info[i].rec_blt_icon.left,
			scrinfo->blt_info[i].rec_blt_icon.top,
			scrinfo->blt_info[i].rec_blt_icon.left+scrinfo->blt_info[i].rec_blt_icon.right,
			scrinfo->blt_info[i].rec_blt_icon.top+scrinfo->blt_info[i].rec_blt_icon.bottom
			);

		CDXUTRadioButton* pBtn = p_pUI->GetRadioButton(IDC_MAIN_BTN_SEL_BULLET_BASE+i);
		CDXUTElement* pElement = pBtn->GetElement(0);
		pElement->SetTexture(scrinfo->res_index, &rcBullet);
		m_pSelBulletBtnList->AddButton((CDXUTButton*)m_pBulletSelectButtons[i]);
//		m_pBulletSelectButtons[i]->SetVisible(true);
	}
	m_pBulletSelectButtons[0]->SetChecked(true);
	m_nSelectBulletNo = 0;

	m_pSelBulletBtnList->SetLRBtnAlwaysVisible(true);
	m_pSelBulletBtnList->SetVisible(value);

}

void CGame::EraseDisconnectCharacter(int nCharaIndex)
{
	for (std::vector< type_session* >::iterator it = m_vecCharacters.begin();
		it != m_vecCharacters.end();
		it++)
	{
		if ((*it)->sess_index != nCharaIndex)
			continue;
		m_vecCharacters.erase(it);
		break;
	}
}

CStageEffect* CGame::FindEffectFromEffectNo(int nEffectNo, BOOL bStageEffect)
{
	std::map<int, CStageEffect* >* pMapEffects = &m_mapStageEffects;
	if (!bStageEffect)
		pMapEffects = &m_mapBGEffects;

	std::map<int, CStageEffect*>::iterator itfind = pMapEffects->find(nEffectNo);
	if (itfind == pMapEffects->end())
	{
		DXTRACE_MSG(L"NotFound:FindEffectFromEffectNo()");
		return NULL;
	}
	return (*itfind).second;
}

#ifdef HOGE
int CGame::AddEffect(int nCharaID, RECT* rcTexture, D3DXVECTOR3* pos, int nAge, BOOL bStageEffect, BOOL force)
#else
int CGame::AddEffect(int nCharaID, RECT* rcTexture, D3DXVECTOR3* pos, int nAge, BOOL bStageEffect)
#endif
{
	CStageEffect* pStageEffect = NULL;
	DEBUG_NEW(pStageEffect, CStageEffect(), L"AddEffect");

	LPDIRECT3DTEXTURE9 pTexture = NULL;
	switch (nCharaID)
	{
	case DEF_STAGE_ID:
		{
			if (m_pSelectedStageScrInfo)
			{
				pTexture = m_pStageTexture;
//				pTexture = m_pSelectedStageScrInfo->pTexture;
			}
			break;
		}
	case DEF_SYSTEM_ID:
		pTexture = m_pDefaultGUIResourceTexture;
		break;
	default:
		{
			TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(nCharaID, &m_mapCharaScrInfo);
			if (pCharaScrInfo)
				pTexture = pCharaScrInfo->pTexture;
			break;
		}
	}

	if (!pTexture)	return -1;

	int* pEffectNoCounter = &m_nStageEffectNoCounter;
	std::map<int, CStageEffect* >* pMapEffects = &m_mapStageEffects;
	if (!bStageEffect)
	{
		pEffectNoCounter = &m_nBGEffectNoCounter;
		pMapEffects = &m_mapBGEffects;
	}

	if (pMapEffects->empty())
		*pEffectNoCounter = 0;

	int nEffectNo = (int)(*pEffectNoCounter);
#ifdef HOGE
	pStageEffect->Create(nEffectNo, pTexture, rcTexture, pos, nAge, force);
#else
	pStageEffect->Create(nEffectNo, pTexture, rcTexture, pos, nAge);
#endif
	pMapEffects->insert(std::map<int, CStageEffect* >::value_type( nEffectNo, pStageEffect ));

	(*pEffectNoCounter)++;
	return nEffectNo;
}

bool CGame::RemoveEffect(int nEffectNo, BOOL bStageEffect)
{
	std::map<int, CStageEffect* >* pMapEffects = &m_mapStageEffects;
	if (!bStageEffect)
		pMapEffects = &m_mapBGEffects;

	std::map<int, CStageEffect*>::iterator itfind = pMapEffects->find(nEffectNo);
	if (itfind == pMapEffects->end())
	{
		DXTRACE_MSG(L"NotFound:FindEffectFromEffectNo()");
		return false;
	}
	CStageEffect* pStageEffect = (*itfind).second;
	DEBUG_DELETE(pStageEffect, L"RemoveEffect");
	pMapEffects->erase(itfind);
	return true;
}

void CGame::ClearStageEffects()
{
	OutputDebugStr(L"ClearStageEffects\n");
	if (m_mapStageEffects.empty())	return;

	for (std::map <int, CStageEffect* >::iterator it = m_mapStageEffects.begin();
		it != m_mapStageEffects.end();
		/*it++*/)
	{
		CStageEffect* pStageEffect = (*it).second;
		DEBUG_DELETE(pStageEffect, L"ClearStageEffects");
		it = m_mapStageEffects.erase(it);
	}
	m_mapStageEffects.clear();
}

void CGame::ClearBGEffects()
{
	OutputDebugStr(L"ClearBGEffects\n");
	if (m_mapBGEffects.empty())	return;

	for (std::map <int, CStageEffect* >::iterator it = m_mapBGEffects.begin();
		it != m_mapBGEffects.end();
		/*it++*/)
	{
		CStageEffect* pStageEffect = (*it).second;
		DEBUG_DELETE(pStageEffect, L"ClearBGEffects");
		it = m_mapBGEffects.erase(it);
	}
	m_mapBGEffects.clear();
}

bool CGame::SetEffectVector(int nEffectNo, D3DXVECTOR3* vec, D3DXVECTOR3* add, BOOL bStageEffect, int nEffectTime)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectVector(vec, add,nEffectTime);
	return true;
}

bool CGame::SetEffectAlpha(int nEffectNo, BYTE bytAlpha, BOOL bStageEffect)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectAlpha(bytAlpha);
	return true;
}

bool CGame::SetEffectFade(int nEffectNo, int nFade, BOOL bStageEffect, int nEffectTime)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectFade(nFade, nEffectTime);
	return true;
}

bool CGame::SetEffectFadeInOut(int nEffectNo, int nFadeInOut, BOOL bStageEffect)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectFadeInOut(nFadeInOut);
	return true;
}

bool CGame::SetEffectRotate(int nEffectNo, float fRot, BOOL bStageEffect)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectRotate(fRot);
	return true;
}

bool CGame::SetEffectRotation(int nEffectNo, float fRot, BOOL bStageEffect, int nEffectTime)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectRotation(fRot, nEffectTime);
	return true;
}

bool CGame::SetEffectScale(int nEffectNo, float fScaleX, float fScaleY, BOOL bStageEffect)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectScale(fScaleX, fScaleY);
	return true;
}

bool CGame::SetEffectScalling(int nEffectNo, float fScaleX, float fScaleY, BOOL bStageEffect, int nEffectTime)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo, bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectScalling(fScaleX, fScaleY, nEffectTime);
	return true;
}

bool CGame::SetEffectAnimation(int nEffectNo, int nATime, int nACount, BOOL bLoop, BOOL bStageEffect, int nEffectTime)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo,bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectAnimation(nATime, nACount, bLoop, nEffectTime);
	return true;
}

bool CGame::SetEffectTexture(int nEffectNo, RECT* rcTexture, BOOL bStageEffect)
{
	CStageEffect* pStageEffect = FindEffectFromEffectNo(nEffectNo,bStageEffect);
	if (!pStageEffect) return false;
	pStageEffect->SetEffectTexture(rcTexture);
	return true;
}


void CGame::SetVisibleStageCharacterInfo(bool bVisible)
{
	// 各キャラの情報表示設定
	if (m_eGameState & GAME_STATE_MAIN)
	{
		for (int i=0;i<GetMaxLoginNum();i++)
		{
			if (m_pStageCharacters[i]->IsCreated())
				m_pStageCharacters[i]->SetVisibleInfo(bVisible);
		}
	}
}

void CGame::RegistSoundSE(char* rc_name)
{
	if (!rc_name)	return;
	WCHAR pRcName[_MAX_PATH*2+1];
	ZeroMemory(pRcName,sizeof(pRcName));
	if (!CStringToWString(pRcName, rc_name,_MAX_PATH*2))
		return;

	g_pCriticalSection->EnterCriticalSection_Sound(L'6');
	int ret = m_pScrSoundLibs->AddFromFile(pRcName);
	if (ret != -1)
	{
		m_mapScrSoundIDHash.insert(std::map<std::string, int>::value_type(rc_name, ret));
		g_pCriticalSection->LeaveCriticalSection_Sound();
	}
	else
	{
		g_pCriticalSection->LeaveCriticalSection_Sound();
		MessageBox(g_hWnd, pRcName, L"読込み失敗：RegistSoundSE()", MB_OK);
	}
}

void CGame::RegistSoundBGM(char* rc_name)
{
	if (!rc_name)	return;
	WCHAR pRcName[_MAX_PATH*2+1];
	ZeroMemory(pRcName,sizeof(pRcName));
	CStringToWString(pRcName, rc_name,_MAX_PATH*2);

	g_pCriticalSection->EnterCriticalSection_Sound(L'7');
	int ret = m_pScrSoundLibs->AddFromFile(pRcName);
	if (ret == -1)
	{
		g_pCriticalSection->LeaveCriticalSection_Sound();
		MessageBox(g_hWnd, pRcName, L"読込み失敗：BGM", MB_OK);
	}
	else
	{
		m_nBGMSoundID = ret;
		g_pCriticalSection->LeaveCriticalSection_Sound();
	}
}

void CGame::PlayScrSoundSE(char* rc_name, int loop, int fade)
{
	if (m_mapScrSoundIDHash.empty())
	{
		AddMessageLog(L"Empty,ScrSoundIDHash");
		return;
	}
	if (!m_bytSEVolume)	return;

	g_pCriticalSection->EnterCriticalSection_Sound(L'8');
	// リソース名からID検索
	std::map<std::string, int>::iterator itfind = m_mapScrSoundIDHash.find(rc_name);
	if (itfind == m_mapScrSoundIDHash.end())
	{
		g_pCriticalSection->LeaveCriticalSection_Sound();
//		MessageBox(g_hWnd, pRcName, L"未登録のサウンド:PlayScrSoundSE1()", MB_OK);
		WCHAR pRcName[_MAX_PATH*2+1];
		ZeroMemory(pRcName,sizeof(pRcName));
		CStringToWString(pRcName, rc_name,_MAX_PATH*2);
		WCHAR log[128];
		SafePrintf(log, 128, L"NotFound,ScrSoundIDHash:%s", pRcName);
		AddMessageLog(log);
		return;
	}
	CSoundBuffer* pSoundBuffer = m_pScrSoundLibs->GetDuplicatedFromID((*itfind).second);
	if (!pSoundBuffer)
	{
		g_pCriticalSection->LeaveCriticalSection_Sound();
//		MessageBox(g_hWnd, pRcName, L"未登録のサウンド:PlayScrSoundSE2()", MB_OK);
		WCHAR pRcName[_MAX_PATH*2+1];
		ZeroMemory(pRcName,sizeof(pRcName));
		CStringToWString(pRcName, rc_name,_MAX_PATH*2);
		WCHAR log[128];
		SafePrintf(log, 128, L"PlayScrSoundSE duplicate error:%s", pRcName);
		AddMessageLog(log);
	}
//	WCHAR sndlog[64];
//	SafePrintf(sndlog,64, L"PlayScrSoundSE:%x", (DWORD)pSoundBuffer->GetDirectSoundBuffer8());
//	AddMessageLog(sndlog);
	// 無限ループ確認
	if (loop == -1)
	{
		if (pSoundBuffer)
			m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytSEVolume, SOUNDLIB_ENDLESSLOOP, fade);
	}
	else
	{
		if (pSoundBuffer)
			m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytSEVolume, loop, fade);
	}
	g_pCriticalSection->LeaveCriticalSection_Sound();
}

void CGame::PlayScrSoundBGM(int fade)
{
	g_pCriticalSection->EnterCriticalSection_Sound(L'9');
	CSoundBuffer* pSoundBuffer = m_pScrSoundLibs->GetDuplicatedFromID(m_nBGMSoundID);
	if (pSoundBuffer)
		m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytBGMVolume, SOUNDLIB_ENDLESSLOOP, 0);
	g_pCriticalSection->LeaveCriticalSection_Sound();
}

void CGame::StopScrSoundBGM(int fade)
{
	g_pCriticalSection->EnterCriticalSection_Sound(L'0');
	CSoundBuffer* pSoundBuffer = m_pScrSoundLibs->GetDuplicatedFromID(m_nBGMSoundID);
	if (pSoundBuffer)
		m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytBGMVolume, SOUNDLIB_ENDLESSLOOP, 0);
	g_pCriticalSection->LeaveCriticalSection_Sound();
}

type_blt* CGame::GetBulletInfo(int obj_no)
{
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(obj_no);
	if (itfind == m_mapObjects.end())
		return NULL;

	type_blt* blt = NULL;

	if ((*itfind).second->obj_type & OBJ_TYPE_BLT)
		blt = (type_blt*)(*itfind).second;
	return blt;
}

void CGame::SetCameraFocusToBullet(int blt_no)
{
	std::map<int, type_obj*>::iterator itfind = m_mapObjects.find(blt_no);
	if (itfind == m_mapObjects.end())
		return;

	type_blt* blt = NULL;

	if ((*itfind).second->obj_type & OBJ_TYPE_BLT)
		blt = (type_blt*)(*itfind).second;
	m_pFocusObject = blt;
	m_bScrFocus = blt != NULL;
}

void CGame::SetCameraFocusToChara(int obj_no)
{
	ptype_session sess = GetSessionObj(obj_no);
	if (!sess) return;
	m_pFocusObject = sess;
	m_bScrFocus = TRUE;
}

int CGame::GetBulletAtkValue(int obj_no)
{
	int ret = 0;
	g_pCriticalSection->EnterCriticalSection_Object(L'%');
	type_blt* blt = GetBulletInfo(obj_no);
	if (!blt)
	{
		g_pCriticalSection->LeaveCriticalSection_Object();
		return 0;
	}
	switch (blt->proc_type)
	{
	case BLT_PROC_TYPE_SCR_CHARA:
		ret = ((TCHARA_SCR_INFO*)blt->scrinfo)->blt_info[blt->bullet_type].blt_atk;
		break;
	case BLT_PROC_TYPE_SCR_SPELL:
		ret = ((TCHARA_SCR_INFO*)blt->scrinfo)->sc_info.blt_atk;
		break;
	case BLT_PROC_TYPE_SCR_STAGE:
		ret = ((TSTAGE_SCR_INFO*)blt->scrinfo)->blt_info[blt->bullet_type].blt_atk;
		break;
	default:
		break;
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
	return ret;

}

void CGame::NotifyTurnStart(ptype_session sess)
{
	if (sess) return;
	LuaFuncParam luaResults;
	LuaFuncParam luaParams;
	luaParams.Number( sess->scrinfo->scr_index).Number(sess->obj_no).Number(sess->turn_count+1).Number(sess->extdata1).Number(sess->extdata2);
	common::scr::CallLuaFunc(g_pLuah, "onTurnStart_Chara", &luaResults, 0, &luaParams, g_pCriticalSection);
	
	std::vector< int > vecFrame;
	vecFrame.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
	for ( std::vector<int>::iterator itno = vecFrame.begin();
		itno != vecFrame.end();
		itno++)
	{
		std::map<int ,type_obj*>::iterator it = m_mapObjects.find( (*itno));
		if (it == m_mapObjects.end())	continue;
		if ((*it).second->proc_flg & PROC_FLG_OBJ_REMOVE) continue;
		if (!((*it).second->obj_type & OBJ_TYPE_BLT))	continue;

		type_blt* blt = (type_blt*)(*it).second;
		if (blt->chr_obj_no == sess->obj_no)
		{
			blt->frame_count = 0;
			blt->turn_count++;

			luaResults.Clear();
			luaParams.Clear();
			switch ( blt->proc_type )
			{
			case BLT_PROC_TYPE_SCR_CHARA:
				luaParams.Number( blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(blt->obj_no).Number(blt->turn_count).Number(blt->extdata1).Number(blt->extdata2).Number(sess->obj_no);
				common::scr::CallLuaFunc(g_pLuah, "onTurnStart_CharaBullet", &luaResults, 0, &luaParams, g_pCriticalSection);
				break;
			case BLT_PROC_TYPE_SCR_SPELL:
				luaParams.Number( blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(blt->obj_no).Number(blt->turn_count).Number(blt->extdata1).Number(blt->extdata2).Number(sess->obj_no);
				common::scr::CallLuaFunc(g_pLuah, "onTurnStart_CharaSpell", &luaResults, 0, &luaParams, g_pCriticalSection);
				break;
			case BLT_PROC_TYPE_SCR_STAGE:
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->turn_count).Number(blt->extdata1).Number(blt->extdata2).Number(sess->obj_no);
				common::scr::CallLuaFunc(g_pLuah, "onTurnStart_StageBullet", &luaResults, 0, &luaParams, g_pCriticalSection);
				break;
			}
		}
	}
}

void CGame::NotifyTurnEnd(ptype_session sess)
{
	if (!sess)	return;

	LuaFuncParam luaResults;
	LuaFuncParam luaParams;
	if (sess->obj_state & OBJ_STATE_ACT_FLG)
	{
		luaParams.Number( sess->scrinfo->scr_index).Number(sess->obj_no).Number(sess->turn_count).Number(sess->extdata1).Number(sess->extdata2);
		common::scr::CallLuaFunc(g_pLuah, "onTurnEnd_Chara", &luaResults, 0, &luaParams, g_pCriticalSection);
	}

	BOOL bRemoved = FALSE;
	std::vector< int > vecFrame;
	vecFrame.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
	for ( std::vector<int>::iterator itno = vecFrame.begin();
		itno != vecFrame.end();
		itno++)
	{
		std::map<int ,type_obj*>::iterator it = m_mapObjects.find( (*itno));
		if (it == m_mapObjects.end())	continue;
		if (!((*it).second->obj_type & OBJ_TYPE_BLT))	continue;

		type_blt* blt = (type_blt*)(*it).second;
		if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)
			continue;
		if (blt->chr_obj_no == sess->obj_no)
		{
			blt->frame_count = 0;
			blt->turn_count++;
			luaParams.Clear();
			luaResults.Clear();
			switch ( blt->proc_type )
			{
			case BLT_PROC_TYPE_SCR_CHARA:
				luaParams.Number( blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(blt->obj_no).Number(blt->turn_count).Number(blt->extdata1).Number(blt->extdata2).Number(sess->obj_no);
				common::scr::CallLuaFunc(g_pLuah, "onTurnEnd_CharaBullet", &luaResults, 0, &luaParams, g_pCriticalSection);
				break;
			case BLT_PROC_TYPE_SCR_SPELL:
				luaParams.Number( blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(blt->obj_no).Number(blt->turn_count).Number(blt->extdata1).Number(blt->extdata2).Number(sess->obj_no);
				common::scr::CallLuaFunc(g_pLuah, "onTurnEnd_CharaSpell", &luaResults, 0, &luaParams, g_pCriticalSection);
				break;
			case BLT_PROC_TYPE_SCR_STAGE:
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->turn_count).Number(blt->extdata1).Number(blt->extdata2).Number(sess->obj_no);
				common::scr::CallLuaFunc(g_pLuah, "onTurnEnd_StageBullet", &luaResults, 0, &luaParams, g_pCriticalSection);
				break;
			}
		}
	}
}

void CGame::UseItem(int nItemIndex, DWORD dwItemFlg, BOOL bSteal)
{
	ptype_session sess = g_pGame->GetMySessionInfo();
	if (bSteal)
	{
		AddChatMessage(GAME_STATE_MSG_STOLEN, PK_USER_CHAT_SERVER_INFO);
		PlaySysSoundSE(SE_sgi_Blind);
	}
	else
	{
		switch (dwItemFlg)
		{
		case GAME_ITEM_POWER:
			AddChatMessage(GAME_STATE_MSG_POWERUP, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_Double);
			break;
		case GAME_ITEM_DOUBLE_SHOT:
			AddChatMessage(GAME_STATE_MSG_DOUBLE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_Double);
			break;
		case GAME_ITEM_STEALTH:
			AddChatMessage(L"ステルス状態になりました(残り3ターン)", PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_Stealth);
			break;
		case GAME_ITEM_SHIELD:
			AddChatMessage(GAME_STATE_MSG_SHIELD, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_Shield);
			break;
		case GAME_ITEM_MOVE_UP:
			AddChatMessage(GAME_STATE_MSG_MOVE_UP, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_Double);
			break;
		case GAME_ITEM_REPAIR:
			AddChatMessage(GAME_STATE_MSG_REPAIR, PK_USER_CHAT_SERVER_INFO);
			break;
		case GAME_ITEM_REPAIR_TEAM:
			AddChatMessage(GAME_STATE_MSG_REPAIR_TEAM, PK_USER_CHAT_SERVER_INFO);
			break;
		case GAME_ITEM_REPAIR_BIG:
			AddChatMessage(GAME_STATE_MSG_REPAIR_BIG, PK_USER_CHAT_SERVER_INFO);
	//		PlaySysSoundSE(SE_sgi_Repair);
			break;
		case GAME_ITEM_TELEPORT:
			AddChatMessage(GAME_STATE_MSG_TELEPORT, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_NOMOVE:
			AddChatMessage(GAME_STATE_MSG_NOMOVE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_NOANGLE:
			AddChatMessage(GAME_STATE_MSG_NOANGLE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_REVERSE:
			AddChatMessage(GAME_STATE_MSG_REVERSE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_BLIND:
			AddChatMessage(GAME_STATE_MSG_BLIND, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_REPAIR_BULLET:
			AddChatMessage(GAME_STATE_MSG_REPAIR_BULLET, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_DRAIN:
			AddChatMessage(GAME_STATE_MSG_DRAIN, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_FETCH:
			AddChatMessage(GAME_STATE_MSG_FETCH, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_EXCHANGE:
			AddChatMessage(GAME_STATE_MSG_EXCHANGE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_WIND_CHANGE:
	//		AddChatMessage(GAME_STATE_MSG_WIND_CHANGE, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		case GAME_ITEM_STEAL:
			AddChatMessage(GAME_STATE_MSG_STEAL, PK_USER_CHAT_SERVER_INFO);
			PlaySysSoundSE(SE_sgi_ItemUse);
			break;
		default:
			break;
		}
	}
	sess->items[nItemIndex] = 0x0;
	g_pGame->UpdateMyItems();
}

void CGame::NotifyDeadScrChara(ptype_session sess, E_TYPE_CHARA_DEAD type, DWORD prev_state)
{
	LuaFuncParam luaParams, luaResults;
	luaParams.Number(sess->scrinfo->scr_index).Number(sess->obj_no).Number((double)type).Number((OBJ_STATE_MAIN_MASK&prev_state));
	common::scr::CallLuaFunc(g_pLuah, "onDead_Chara", &luaResults, 0, &luaParams, g_pCriticalSection);
}

void CGame::OnSelectBulletType(int nControlID)
{
	PlaySysSoundSE(SE_sgr_BltSel);
	m_nSelectBulletNo = nControlID - IDC_MAIN_BTN_SEL_BULLET_BASE;
}

void CGame::UpdateControlPanelHPInfo()
{
	ptype_session sess = GetMySessionInfo();
	if (!sess) return;
	// HPメータ
	m_pMeterHP->SetMaxValue( sess->HP_m );
	m_pMeterHP->SetValue( sess->HP_c );

	// 顔を変える
	if (sess->scrinfo)
	{
		int nCharaFaceType = CHARA_FACE_HURT;
		if (sess->obj_state & (OBJ_STATE_MAIN_GALLERY_FLG|OBJ_STATE_LOAD))
			nCharaFaceType = CHARA_FACE_TYPE_FINE;
		else if (sess->HP_c)
		{
			double value = (double)sess->HP_c / (double)sess->HP_m;
			if (value > CHARA_FACE_FINE_HP)
				nCharaFaceType = CHARA_FACE_TYPE_FINE;
			else if (value > CHARA_FACE_NORMAL_HP)
				nCharaFaceType = CHARA_FACE_TYPE_NORMAL;
		}

		RECT rcTexture;
		LuaFuncParam luaParams, luaResults;
		luaParams.Number(sess->scrinfo->scr_index).Number(nCharaFaceType);
		common::scr::CallLuaFunc(g_pLuah, "getChara_Face", &luaResults, 4, &luaParams, g_pCriticalSection);
		rcTexture.left = (int)luaResults.GetNumber(0);
		rcTexture.top = (int)luaResults.GetNumber(1);
		rcTexture.right = rcTexture.left + (int)luaResults.GetNumber(2);
		rcTexture.bottom = rcTexture.top + (int)luaResults.GetNumber(3);

		CDXUTElement* pElement = p_pUI->GetControl(IDC_MAIN_SPRITE_FACE)->GetElement(0);
		pElement->iTexture = sess->scrinfo->res_index;
		CopyRect(&(pElement->rcTexture), &rcTexture);
	}
}

void CGame::ChangeCharaHP(int nObjNo, int value)
{
	if (value > 0)
		PlaySysSoundSE(SE_sgi_Repair);
	int nIndex = GetSessionIndex(nObjNo);
	if (nIndex != -1)
	{
		m_pStageCharacters[nIndex]->ChangeHP(value);	
		if (nIndex == m_nUserIndex)
			UpdateControlPanelHPInfo();
	}
}

void CGame::UpdateStageDrawRect(BOOL bMain)
{
	if (m_pSelectedStageScrInfo)
	{
		int nWinStageHieght = WIN_HEIGHT - MAIN_CONTROLPANEL_CNT_HIDE;
		if (!bMain)
			nWinStageHieght = WIN_HEIGHT;
		else
			m_rcDrawStage.bottom -= MAIN_CONTROLPANEL_CNT_HIDE;

		SetRect(&m_rcDrawStage, (int)m_vecStageViewTrans.x, (int)m_vecStageViewTrans.y, (int)m_vecStageViewTrans.x+WIN_WIDTH, (int)m_vecStageViewTrans.y+nWinStageHieght);
		float fBGWidthRate = 0.0f;
		if (m_pSelectedStageScrInfo->stage.size.cx-WIN_WIDTH)
			fBGWidthRate = (float)(m_pSelectedStageScrInfo->bg.size.cx-WIN_WIDTH)/(float)(m_pSelectedStageScrInfo->stage.size.cx-WIN_WIDTH);
		float fBGHeightRate= 0.0f;
		if (m_pSelectedStageScrInfo->stage.size.cy-nWinStageHieght)
			fBGHeightRate = (float)(m_pSelectedStageScrInfo->bg.size.cy-nWinStageHieght)/(float)(m_pSelectedStageScrInfo->stage.size.cy-nWinStageHieght);
		SetRect(&m_rcDrawBGStage,
			(int)(m_vecStageViewTrans.x*fBGWidthRate),
			(int)(m_vecStageViewTrans.y*fBGHeightRate),
			(int)((m_vecStageViewTrans.x*fBGWidthRate)+WIN_WIDTH),
			(int)((m_vecStageViewTrans.y*fBGHeightRate)+nWinStageHieght)
		);
	}
}

void CGame::OnTurnPassButtonDown(int nControlID)
{
	// 無効なら処理しない
	if (!(BOOL)p_pUI->GetControl(nControlID)->GetUserData() )	return;

	// パス音声再生
	PlaySysSoundSE(SE_sgr_Pass);

	g_pCriticalSection->EnterCriticalSection_Session(L'9');
	ptype_session sess = GetMySessionInfo();
	if (!sess
	|| !(sess->connect_state == CONN_STATE_AUTHED)
	|| !(sess->obj_state & OBJ_STATE_MAIN_ACTIVE)
	)
	{
		g_pCriticalSection->LeaveCriticalSection_Session();
		return;
	}

	// 自ターンパス無効表示テクスチャ設定
	SetRect( &p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_DISABLE_RECT);
	p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)FALSE);

	// パケット送信
	BYTE pkt[MAX_PACKET_SIZE];
	WORD packetSize = PacketMaker::MakePacketData_MainInfoTurnPass(sess->obj_no, pkt);
	AddPacket(pkt, packetSize);

	g_pCriticalSection->LeaveCriticalSection_Session();
}

void CGame::OnCharaDisconnect(int nCharaIndex)
{
	ptype_session sess = GetSessionInfo(nCharaIndex);
	if (!sess) return;
	// 表示を戻すため異常状態をクリアしておく
	ZeroMemory(sess->chara_state, sizeof(char)*CHARA_STATE_COUNT);
	m_pStageCharacters[nCharaIndex]->SetDead();	// 切断したキャラは死んでおく

}

void CGame::UpdateNotifyMyTurn()
{
	if (m_nNotifyMyTurn)
	{
		m_nNotifyMyTurn--;
		if ( m_nNotifyMyTurn == 0)
		{
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN)->SetVisible(false);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN_TEXT)->SetVisible(false);
		}
		else if (m_nNotifyMyTurn <= MAIN_STATIC_NOTIFY_TURN_FADE)
		{
			BYTE bytAlpha = (BYTE)min(( ((double)m_nNotifyMyTurn/(double)MAIN_STATIC_NOTIFY_TURN_FADE)*255.0),0xFF);
			DWORD clr = (DWORD)(bytAlpha<<24|bytAlpha<<16|bytAlpha<<8|bytAlpha);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN)->SetTextColor(clr);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN_TEXT)->SetTextColor(clr);
		}
		else if (m_nNotifyMyTurn >= NOTIFY_MY_TURN_TIME-MAIN_STATIC_NOTIFY_TURN_FADE)
		{
			int nRest = NOTIFY_MY_TURN_TIME - m_nNotifyMyTurn+1;
			BYTE bytAlpha = (BYTE) min( ((double)nRest / (double)MAIN_STATIC_NOTIFY_TURN_FADE) * 255.0, 0xFF);
			DWORD clr = (DWORD)(bytAlpha<<24|bytAlpha<<16|bytAlpha<<8|bytAlpha);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN)->SetTextColor(clr);
			p_pUI->GetStatic(IDC_MAIN_STATIC_NOTIFY_TURN_TEXT)->SetTextColor(clr);
		}
	}
}

void CGame::FrameStageCharacter()
{
	// ゲーム状態がメイン以外は処理しない
	if (m_eGameState != eGameMain)
		return;
	g_pCriticalSection->EnterCriticalSection_Session(L'0');
	switch (m_eMainPhase)
	{
	case GAME_MAIN_PHASE_TRIGGER:
		{
			ptype_session sess = GetAuthedSessionObj(m_nActiveObjectNo);
			if (!sess)	break;
//			sess->frame_count++;
			LuaFuncParam luaParams, luaResults;
			int nBltType = m_nShootingBltType;
			if (m_nShootingProcType == BLT_PROC_TYPE_ITEM)
			{
				switch (m_nShootingBltType)
				{
				case CHARA_STATE_ITEM_REVERSE_INDEX:		nBltType = DEF_BLT_TRG_TYPE_REVERSE;		break;
				case CHARA_STATE_ITEM_BLIND_INDEX:			nBltType = DEF_BLT_TRG_TYPE_BLIND;			break;
				case CHARA_STATE_ITEM_REPAIRBLT_INDEX:	nBltType = DEF_BLT_TRG_TYPE_REPAIR;		break;
				case CHARA_STATE_ITEM_TELEPORT_INDEX:		nBltType = DEF_BLT_TRG_TYPE_TELEPORT;	break;
				case CHARA_STATE_ITEM_DRAIN_INDEX:			nBltType = DEF_BLT_TRG_TYPE_DRAIN;		break;
				case CHARA_STATE_ITEM_FETCH_INDEX:			nBltType = DEF_BLT_TRG_TYPE_FETCH;		break;
				case CHARA_STATE_ITEM_EXCHANGE_INDEX:	nBltType = DEF_BLT_TRG_TYPE_EXCHANGE;	break;
				case CHARA_STATE_ITEM_NOANGLE_INDEX:		nBltType = DEF_BLT_TRG_TYPE_NOANGLE;	break;
				case CHARA_STATE_ITEM_NOMOVE_INDEX:		nBltType = DEF_BLT_TRG_TYPE_NOMOVE;		break;
				}
			}
			luaParams.Number(sess->scrinfo->scr_index).Number(nBltType).Number(sess->ax).Number(sess->ay).Number(sess->angle).Number(sess->frame_count);
			if (!common::scr::CallLuaFunc(g_pLuah, "onTriggerFrame", &luaResults, 1, &luaParams, g_pCriticalSection))
				break;
			if ( luaResults.GetBool(0) == true)
			{
				AddMessageLog(L"TriggerFrameEnd");

				BYTE		packetData[MAX_PACKET_SIZE];
				WORD packetSize = PacketMaker::MakePacketData_MainInfoTriggerEnd(GetMySessionInfo(), m_nShootingProcType, m_nShootingBltType, m_nShootingAngle, m_nShootingPower, m_nShootingIndicatorAngle, m_nShootingIndicatorPower, packetData);
				AddPacket(packetData, packetSize);

				m_bTriggerEnd = TRUE;
//				m_eMainPhase = GAME_MAIN_PHASE_SHOT_REQ;
				sess->obj_state = OBJ_STATE_MAIN_SHOOTING;
				m_eMainPhase = GAME_MAIN_PHASE_SHOOTING;
				sess->frame_count = 0xFFFF;
			}
			break;
		}
	case GAME_MAIN_PHASE_SHOT_REQ:
		{
			break;
		}
	case GAME_MAIN_PHASE_SHOOTING:
		{
			ptype_session sess = GetAuthedSessionObj(m_nShootingCharaObjNo);
			if (!sess)	break;
			if (common::scr::CallShootingFunc(g_pLuah, sess, m_nShootingProcType, m_nShootingBltType, m_nShootingAngle, m_nShootingPower, (TCHARA_SCR_INFO*)sess->scrinfo, m_nShootingCharaObjNo, sess->frame_count, m_nShootingIndicatorAngle, m_nShootingIndicatorPower, g_pCriticalSection))
			{
				sess->obj_state = OBJ_STATE_MAIN_SHOT;
				m_eMainPhase = GAME_MAIN_PHASE_SHOT;
			}
			break;
		}
	default:
		break;
	}
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (!m_pStageCharacters[i]->IsCreated())	continue;
//		if (m_SessionArray[i].connect_state != CONN_STATE_AUTHED) continue;
//		if (!(m_SessionArray[i].obj_state & OBJ_STATE_GAME)) continue;
		if (!(m_SessionArray[i].entity)) continue;
		if (m_SessionArray[i].obj_state & OBJ_STATE_MAIN_DROP_FLG)	continue;

		m_SessionArray[i].frame_count++;
//		switch (common::chr::MoveStage(m_pMainStage, &m_SessionArray[i], &m_mapCharaScrInfo))
		switch (common::chr::MoveOnStage(m_pMainStage, &m_SessionArray[i], &m_mapCharaScrInfo))
		{
		case common::chr::MOVE_STAGE_RESULT_MOVED:
			UpdateStageCharaDisplay(i);
			break;
		case common::chr::MOVE_STAGE_RESULT_NONE:
			break;
		case common::chr::MOVE_STAGE_RESULT_DROP:
//> 20101218 パケットのみで設定される
//			m_SessionArray[i].obj_state = OBJ_STATE_MAIN_DROP;
//			m_SessionArray[i].HP_c = 0;
//< 20101218
			break;
		}
		m_pStageCharacters[i]->Frame();
	}
	g_pCriticalSection->LeaveCriticalSection_Session();

}

bool CGame::SetCharaTexture(int chr_no, RECT *prcTexture)
{
	if (!prcTexture) return false;
	ptype_session sess = GetSessionObj(chr_no);
	if (!sess) return false;

	if (m_pStageCharacters[sess->sess_index]->IsCreated())
		m_pStageCharacters[sess->sess_index]->SetTexture(prcTexture);
	return true;
}


void CGame::HideStage()
{
	if (m_bStageVisible)
	{
		m_bStageVisible = FALSE;
		m_nStageFadeCounter = GAME_STAGE_FADE_TIME-m_nStageFadeCounter;
	}
}

void CGame::ShowStage()
{
	if (!m_bStageVisible)
	{
		m_bStageVisible = TRUE;
		m_nStageFadeCounter = GAME_STAGE_FADE_TIME-m_nStageFadeCounter;
	}
}

DWORD CGame::GetStageColor()
{
	DWORD ret = m_bStageVisible?0xFFFFFFFF:0x00000000;
	
	if (m_nStageFadeCounter)
	{
		int nCount = m_nStageFadeCounter;
		if (m_bStageVisible)
			nCount = GAME_STAGE_FADE_TIME-m_nStageFadeCounter;
		BYTE bytClr = (BYTE)(((float)nCount/(float)GAME_STAGE_FADE_TIME)*0xFF);
		ret = 0xFF000000|(bytClr<<16)|(bytClr<<8)|(bytClr);
		m_nStageFadeCounter--;
	}
	return ret;
}

void CGame::UpdateSCExp()
{
	ptype_session sess = GetMySessionInfo();
	if (!sess) return;
	if (sess->obj_state == OBJ_STATE_MAIN_GALLERY)
		return;
	m_pMeterSCExp->SetValue(sess->EXP_c);
	// EXPが溜まっている
	if (sess->EXP_c >= ((TCHARA_SCR_INFO*)sess->scrinfo)->sc_info.max_exp)
	{
		if (m_pMeterSCExp->GetVisible())
			m_pMeterSCExp->SetVisible(false);
		if (!m_pRbSC->GetVisible())
			m_pRbSC->SetVisible(true);
	}
	else
	{
		if (m_pRbSC->GetVisible())
			m_pRbSC->SetVisible(false);
		if (!m_pMeterSCExp->GetVisible())
		{
			m_pMeterSCExp->SetVisible(true);
			// チェックされている場合、別弾にチェックを移す
			if (m_pRbSC->GetChecked())
			{
				m_pBulletSelectButtons[0]->SetChecked(true);
				m_nTriggerType = 0;
				m_nSelectBulletNo = 0;
			}
		}
	}	
}

void CGame::SetSelectBulletInfo()
{
	m_nTriggerType = m_nSelectBulletNo;
	m_nShootingProcType = BLT_PROC_TYPE_SCR_CHARA;
	// スペルカード選択
	if (m_nSelectBulletNo == MAX_CHARA_BULLET_TYPE)
		m_nShootingProcType = BLT_PROC_TYPE_SCR_SPELL;
	// 装填済みのアイテム弾取得
	m_nShootingBltType = GetStockItemBullet();	
	if (m_nShootingBltType != -1)
		m_nShootingProcType = BLT_PROC_TYPE_ITEM;
	else
		m_nShootingBltType = m_nSelectBulletNo;
}

bool CGame::ShotCommand(int chr_obj_no, int proc_type, int chr_id, int blt_type, int vec_angle, int power, int frame, int indicator_angle, int indicator_power)
{
	g_pCriticalSection->EnterCriticalSection_Session(L'-');
	ptype_session sess = GetSessionObj(chr_obj_no);
	if (!sess)
	{
		AddMessageLog(L"!C_Shot chr_obj_no out range");
		g_pCriticalSection->LeaveCriticalSection_Session();
		return true;	// 発射終了
	}

	std::map < int, TCHARA_SCR_INFO >::iterator itfind = m_mapCharaScrInfo.find(chr_id);
	if (itfind == m_mapCharaScrInfo.end())
	{
		AddMessageLog(L"!Shooting ScrID error");
		g_pCriticalSection->LeaveCriticalSection_Session();
		return true;	// 発射終了
	}

	bool ret = common::scr::CallShootingFunc(g_pLuah, sess, proc_type, blt_type, vec_angle, power, &(*itfind).second, chr_obj_no, frame, indicator_angle, indicator_power, g_pCriticalSection)?true:false;
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}
