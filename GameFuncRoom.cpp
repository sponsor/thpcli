#include "ext.h"
#include "Game.h"
// parts
// CGame::xxx > Funcsions > Room

// 新規ユーザー情報の設定
void CGame::SetNewUser(type_session* sess, BOOL bAddLog)
{
	SetSessionInfo(sess);
	m_nAuthedCount = CalcAuthedUserCount();
	if (!SetRoomCharacter(sess->sess_index))					// アバター
	{
		DXTRACE_ERR(L"SetNewUser::SetRoomCharacter", E_FAIL);
		return;
	}
	UpdateUserList(sess->sess_index);								// ユーザリストコントロール
	UpdateWisperList(sess->sess_index);							// 個人チャットリスト
	if (sess->sess_index == m_nUserIndex)
		SetChacterSelectInfo(sess->chara_type);
	m_pTeamRulePropertyManager->Update((GetMySessionInfo()->master!=0), m_nTeamCount);
	// 入室情報をログに追加
	if (bAddLog)
		AddLogRoomIn(sess->sess_index);
	if (g_bDebug)
	{
		WCHAR log[96];
		WCHAR cname[MAX_CHARACTER_NAME+1];
		common::session::GetSessionName(sess, cname);
		SafePrintf(log, 96, L"NewUser(%d):%s", sess->sess_index, cname);
		AddMessageLog(log);
	}
}

void CGame::AddLogRoomIn(int nCharaIndex)
{
	WCHAR addlog[MAX_MSG_BUFFER];
	WCHAR name[MAX_USER_NAME+1];
	common::session::GetSessionName(&m_SessionArray[nCharaIndex], name);
	SafePrintf(addlog, MAX_MSG_BUFFER, L"%s さんが入室しました。", name);
	AddChatMessage(addlog, PK_USER_CHAT_SERVER_INFO);
}

// キャラセレ情報の設定
void CGame::SetChacterSelectInfo(int nObjType)
{
	// ランダム
	if (nObjType == ROOM_CHARA_RANDOM_ID)
	{
		SetChacterSelectInfoRandom();
		return;
	}

	// スクリプト情報を取得
	TCHARA_SCR_INFO* scrinfo = common::scr::FindCharaScrInfoFromCharaType(nObjType, &m_mapCharaScrInfo); 

	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_CHARA_NAME);
	pStatic->SetText(scrinfo->name);

	WCHAR	wsCharaInfo[64];
	pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_CHARA_INFO);
	CDXUTColorListBox* pCLBBulletInfo = p_pUI->GetColorListBox(IDC_ROOM_LIST_CHARA_BLT_INFO);

	SafePrintf(wsCharaInfo, 64, L"HP:%4d / MOVE:%4d / RANGE:%3d～%3d", scrinfo->max_hp, scrinfo->move, scrinfo->angle_range_min-180, scrinfo->angle_range_max-180);
	pStatic->SetText(wsCharaInfo);

	RECT rcBullet;
	WCHAR wsBulletInfo[32];
	// 弾リストをクリア
	pCLBBulletInfo->RemoveAllItems();
	CDXUTElement* pDefElement = pCLBBulletInfo->GetElement(2);
	pDefElement->SetFont(IDC_ROOM_BTN_CHARA_SEL_FONT, 0xFFFFFFFF, DT_LEFT);
	for (int i=0;i<scrinfo->blt_sel_count;i++)
	{
		WCHAR* wsInfo[] = {L"小",L"中",L"大"};
		int nInfoAtkType = 0;
		int nInfoDelayType = 0;

		SetRect(&rcBullet,
			scrinfo->blt_info[i].rec_blt_icon.left,
			scrinfo->blt_info[i].rec_blt_icon.top,
			scrinfo->blt_info[i].rec_blt_icon.left+scrinfo->blt_info[i].rec_blt_icon.right,
			scrinfo->blt_info[i].rec_blt_icon.top+scrinfo->blt_info[i].rec_blt_icon.bottom
			);
		// 2=開始番号
		CDXUTElement* pElement = new CDXUTElement(*pDefElement);
		pElement->FontColor.Init(0xFFFFFFFF);
		pElement->TextureColor.Init(0xFFFFFFFF);
		pElement->SetTexture(scrinfo->res_index , &rcBullet, 0xFFFFFFFF);
		pCLBBulletInfo->SetElement(scrinfo->res_index+i , pElement);

		nInfoAtkType = 0;
		if (scrinfo->blt_info[i].blt_atk >= ROOM_CHARA_INFO_ATK_MID)
			nInfoAtkType = 1;
		if (scrinfo->blt_info[i].blt_atk >= ROOM_CHARA_INFO_ATK_HIGH)
			nInfoAtkType = 2;
		nInfoDelayType=0;
		if (scrinfo->blt_info[i].blt_delay >= ROOM_CHARA_INFO_DELAY_MID)
			nInfoDelayType = 1;
		if (scrinfo->blt_info[i].blt_delay >= ROOM_CHARA_INFO_DELAY_HIGH)
			nInfoDelayType = 2;

		SafePrintf(wsBulletInfo, 32, L"ATK: %s / DELAY: %s", wsInfo[nInfoAtkType], wsInfo[nInfoDelayType]);
		pCLBBulletInfo->AddItem(wsBulletInfo, NULL, scrinfo->res_index+i );
		SafeDelete(pElement);
	}
	pCLBBulletInfo->GetScrollBar()->SetPageSize(ROOM_LIST_CHARA_BLT_INFO_PAGE_SIZE);
	pCLBBulletInfo->SetEnabled(true);

	CDXUTSprite* pSprite = p_pUI->GetSprite(IDC_ROOM_SPRITE_SC);
	SetRect(&rcBullet,
			scrinfo->sc_info.rec_blt_icon.left,
			scrinfo->sc_info.rec_blt_icon.top,
			scrinfo->sc_info.rec_blt_icon.left+scrinfo->sc_info.rec_blt_icon.right,
			scrinfo->sc_info.rec_blt_icon.top+scrinfo->sc_info.rec_blt_icon.bottom);
	pSprite->GetElement(0)->SetTexture(scrinfo->res_index, &rcBullet);
	m_pRbSC->GetElement(0)->SetTexture(scrinfo->res_index, &rcBullet);
	pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_SC_NAME);
	pStatic->SetText(scrinfo->sc_info.name);

//	int nCharaScrCount = m_mapCharaScrInfo.size();
	for (std::map < int, TCHARA_SCR_INFO >::iterator it = m_mapCharaScrInfo.begin();
		it != m_mapCharaScrInfo.end();
		it++)
	{
		CDXUTRadioButton* pRadioButton = p_pUI->GetRadioButton((*it).first+IDC_ROOM_BTN_CHARA_SEL_LIST_BASE);
		bool bChecked = (nObjType == (int)pRadioButton->GetUserData());
		if (pRadioButton->GetChecked() != bChecked)
			pRadioButton->SetChecked(bChecked);
	}
	m_pCharacterBtnList->SetEnable(true);
}

void CGame::SetChacterSelectInfoRandom()
{
	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_CHARA_NAME);
	pStatic->SetText(L"？？？");

	pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_CHARA_INFO);
	CDXUTColorListBox* pCLBBulletInfo = p_pUI->GetColorListBox(IDC_ROOM_LIST_CHARA_BLT_INFO);
	pStatic->SetText(L"HP:？ / MOVE:？ / RANGE:？～？");
	// 弾リストをクリア
	pCLBBulletInfo->RemoveAllItems();

	CDXUTSprite* pSprite = p_pUI->GetSprite(IDC_ROOM_SPRITE_SC);
	RECT rcBullet = {0,0,0,0};
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcBullet, 0);
	m_pRbSC->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcBullet, 0);
	pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_SC_NAME);
	pStatic->SetText(L"");

	m_pCharacterBtnList->SetEnable(true);
}

// アイテムアイコンボタン押下
void CGame::OnItemIconButtonDown(CDXUTRadioButton* pBtn)
{
	int nIndex = (int)pBtn->GetUserData();
	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_ITEM_INFO);
	{
		WCHAR wsDetail[MAX_GAME_ITEM_TEXT+9];
		SafePrintf(wsDetail, MAX_GAME_ITEM_TEXT+8, L"コスト:%02d\n%s", c_tItemResouceInfoTable[nIndex].cost, c_tItemResouceInfoTable[nIndex].text);
		pStatic->SetText(wsDetail);
	}
	// 選択したアイテムのアイコン切り替え
	CDXUTButton* pIconBtn = p_pUI->GetButton(IDC_ROOM_BTN_ITEM_SEL_ICON);
	if (nIndex != (int)pIconBtn->GetUserData())
	{
		RECT rcTexture;
		SetRect(&rcTexture,
			c_tItemResouceInfoTable[nIndex].left,
			c_tItemResouceInfoTable[nIndex].top,
			c_tItemResouceInfoTable[nIndex].right,
			c_tItemResouceInfoTable[nIndex].bottom
			);
		CDXUTElement* pElement = pIconBtn->GetElement(1);
		pElement->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
		pIconBtn->SetUserData((void*)nIndex);
	}
}

// アイテムラジオボタンの用意
void CGame::SetItemIconButtonList()
{
	RECT rcTexture;
	CDXUTButton *pCharaSelBtnL;
	p_pUI->AddButton( IDC_ROOM_RB_ITEM_L,L"",
		IDC_ROOM_BTN_ITEM_SEL_LIST_X,
		IDC_ROOM_BTN_ITEM_SEL_LIST_Y,
		IDC_ROOM_BTN_ITEM_SEL_LIST_W,
		IDC_ROOM_BTN_ITEM_SEL_LIST_H,
		0, false, &pCharaSelBtnL );
	SetRect( &rcTexture,	IDC_ROOM_BTN_ITEM_SEL_LIST_L_IMG_RECT);
	pCharaSelBtnL->GetElement(0)->TextureColor.Init(0,0,0);
	pCharaSelBtnL->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pCharaSelBtnL->SetStateMouserOverOffset(0,0);
	pCharaSelBtnL->SetStatePressedOverOffset(-1,0);
	pCharaSelBtnL->SetEnabledRightClickEvent(true);

	CDXUTButton *pCharaSelBtnR;
	p_pUI->AddButton( IDC_ROOM_RB_ITEM_R,	L"",
		IDC_ROOM_BTN_ITEM_SEL_LIST_X,
		IDC_ROOM_BTN_ITEM_SEL_LIST_Y,
		IDC_ROOM_BTN_ITEM_SEL_LIST_W,
		IDC_ROOM_BTN_ITEM_SEL_LIST_H,
		0, false, &pCharaSelBtnR );
	SetRect( &rcTexture,	IDC_ROOM_BTN_ITEM_SEL_LIST_R_IMG_RECT);
	pCharaSelBtnR->GetElement(0)->TextureColor.Init(0,0,0);
	pCharaSelBtnR->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pCharaSelBtnR->SetStateMouserOverOffset(0,0);
	pCharaSelBtnR->SetStatePressedOverOffset(1,0);
	pCharaSelBtnR->SetEnabledRightClickEvent(true);
	POINT pnt;
	pnt.x = IDC_ROOM_BTN_CHARA_SEL_LIST_X;
	pnt.y = IDC_ROOM_BTN_CHARA_SEL_LIST_Y;
	m_pItemBtnList->Create(pCharaSelBtnL, pCharaSelBtnR, &pnt, 256);

	int i=0;
	while (c_tItemResouceInfoTable[i].flg)
	{
		RECT rcTexture;
		CDXUTRadioButton *pRadioButton;

		p_pUI->AddRadioButton( IDC_ROOM_RB_ITEM_BASE_INDEX+i,
			IDC_ROOM_BTN_ITEM_SEL_BTN_GROUP,	L"",
			IDC_ROOM_BTN_CHARA_SEL_LIST_X,
			IDC_ROOM_BTN_CHARA_SEL_LIST_Y,
			GAME_ITEM_W,
			GAME_ITEM_H,
			0, 0, false, &pRadioButton );

		pRadioButton->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
		pRadioButton->GetElement(0)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
		pRadioButton->GetElement(0)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);

		// 描画範囲
		SetRect( &rcTexture,
			c_tItemResouceInfoTable[i].left,
			c_tItemResouceInfoTable[i].top,
			c_tItemResouceInfoTable[i].right,
			c_tItemResouceInfoTable[i].bottom);

		// 追加した管理テクスチャ番号を指定
		pRadioButton->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );

		// ボタンとスクリプトID情報を関連付ける
		SetRect( &rcTexture,
			IDC_ROOM_BTN_CHARA_SEL_CUR_X,
			IDC_ROOM_BTN_CHARA_SEL_CUR_Y,
			(IDC_ROOM_BTN_CHARA_SEL_CUR_X+IDC_ROOM_BTN_CHARA_SEL_CUR_W),
			(IDC_ROOM_BTN_CHARA_SEL_CUR_Y+IDC_ROOM_BTN_CHARA_SEL_CUR_H)
		);

		pRadioButton->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);
		pRadioButton->SetUserData( (void*)i );
		m_pItemBtnList->AddButton((CDXUTButton*)pRadioButton);
		i++;
	}
	m_pItemBtnList->SetVisible(false);
}

void CGame::UpdateUserList(int nIndex)
{
	if (nIndex >= GetMaxLoginNum() || nIndex < 0)	return;

	BOOL bAdd = TRUE;
	CDXUTColorListBox* pColorListBox = (CDXUTColorListBox*)p_pUI->GetControl(IDC_ROOM_LB_USERLIST);

	if (!pColorListBox)
	{
		DXUTTRACE(L"p_pUI->GetControl(IDC_ROOM_LB_USERLIST) == NULL");
		return;
	}
	// 更新
	for (int i=0;i<pColorListBox->GetSize();i++)
	{
		DXUTColorListBoxItem* pItem = (DXUTColorListBoxItem*)pColorListBox->GetItem(i);
		
		DWORD dwObjNo = (DWORD)pItem->pData;
		if (dwObjNo == (DWORD)nIndex)
		{
			// 部屋に居ないならリストから消す
			if (m_SessionArray[dwObjNo].connect_state != CONN_STATE_AUTHED)
			{
				pColorListBox->RemoveItem(i);
				bAdd = FALSE;
				break;
			}
			bAdd = FALSE;
			common::session::GetSessionName(&m_SessionArray[dwObjNo], pItem->strText);

			pItem->pData = (void*)dwObjNo;
			if (m_SessionArray[dwObjNo].scrinfo)
			{
				int nResIndex = m_SessionArray[dwObjNo].scrinfo->res_index;
				pItem->nResourceIndex = nResIndex;
//				pItem->bSelected = (0x80&pItem->bSelected)|(nResIndex&0x7F);
			}
//#ifdef _DEBUG
			else
			{
				pItem->bSelected = false;
				pItem->nResourceIndex = 2;
//				pItem->bSelected = (0x80&pItem->bSelected)|(2);
			}
//#endif
			break;
		}
	}
	// 追加
	if (bAdd)
	{
		WCHAR name[MAX_USER_NAME+1];
		common::session::GetSessionName(&m_SessionArray[nIndex], name);
		if (m_SessionArray[nIndex].scrinfo)
		{
			pColorListBox->AddItem(name,
				(void*)((DWORD)nIndex),
				m_SessionArray[nIndex].scrinfo->res_index);
		}
//#ifdef _DEBUG
		else
		{
			pColorListBox->AddItem(name,(void*)((DWORD)nIndex),2);
		}
//#endif
	}
	
	if (!g_bOneClient)
	{
		WCHAR caption[64];
		SafePrintf(caption, 64, L"%s %s [接続:%d人]", NAME_CAPTION_CLI, CLI_VER, pColorListBox->GetSize());
		SetWindowTextW(g_hWnd, caption);
	}
}

void CGame::SetSessionInfo(type_session *sess)
{
	TCHARA_SCR_INFO* pScrInfo = NULL;
	int nIndex = sess->sess_index;
	if (sess->chara_type == ROOM_CHARA_RANDOM_ID)
	{
		// ランダム
		m_SessionArray[nIndex].scrinfo = NULL;
	}
	else
	{
		// スクリプトと関連付け
		pScrInfo = common::scr::FindCharaScrInfoFromCharaType(sess->chara_type, &m_mapCharaScrInfo);
		if (!pScrInfo)
		{
			MessageBox(g_hWnd, L"スクリプトに未登録のキャラクタがログインしようとしました。", L"error", MB_OK);
			return;
		}
	}

	ZeroMemory(sess->chara_state, sizeof(char)*CHARA_STATE_COUNT);
	
	memcpy(&m_SessionArray[nIndex], sess, sizeof(type_session));
	m_SessionArray[nIndex].scrinfo = pScrInfo;
	m_pTeamRulePropertyManager->Update((GetMySessionInfo()->master!=0), m_nTeamCount);
}

void CGame::UpdateWisperList(int nIndex)
{
	if (nIndex >= GetMaxLoginNum() || nIndex < 0)	return;
	// 自分自身は更新しない(追加しない)
	if (nIndex == m_nUserIndex)		return;

	int nCharaType = 0;
	WCHAR* pUserName = NULL;
	BOOL bAdd = TRUE;
	CDXUTComboBox* pComboBox = (CDXUTComboBox*)p_pUI->GetControl(IDC_ROOM_CMB_CHAT);

	if (!pComboBox)
	{
		DXUTTRACE(L"p_pUI->GetControl(IDC_ROOM_CMB_CHAT) == NULL");
		return;
	}

	// 更新	(0=ALL、1=TEAM、2～=個人)
	for (int i=2;i < (int)pComboBox->GetNumItems();i++)
	{
		DXUTComboBoxItem* pItem = pComboBox->GetItem(i);
		DWORD dwObjNo = (DWORD)pItem->pData;
		if (dwObjNo == (DWORD)nIndex)
		{
			// 部屋に居ないならリストから消す
			if (m_SessionArray[dwObjNo].connect_state != CONN_STATE_AUTHED)
				{
				WCHAR log[64];
				SafePrintf(log, 64, L"DeleteWhisperList:%s", pItem->strText);
				AddMessageLog(log);
				pComboBox->RemoveItem(i);
				return;
			}
			bAdd = FALSE;
			common::session::GetSessionName(&m_SessionArray[nIndex], pItem->strText);
			pItem->pData = (void*)dwObjNo;
			break;
		}
	}

	// 追加
	if (bAdd)
	{
		WCHAR name[MAX_USER_NAME+1];
		common::session::GetSessionName(&m_SessionArray[nIndex], name);
		pComboBox->AddItem(name, (void*)((DWORD)nIndex));
	}
}

// セッション情報からキャラクタを部屋にセット
BOOL CGame::SetRoomCharacter(int nIndex)
{
	HRESULT hr;
	TCHARA_SCR_INFO* pScrInfo = (TCHARA_SCR_INFO*)m_SessionArray[nIndex].scrinfo;
	if (!pScrInfo)
	{
		if (m_SessionArray[nIndex].chara_type == ROOM_CHARA_RANDOM_ID)
		{
			V(m_pRoomCharacters[nIndex]->Create(m_pDefaultGUIResourceTexture, m_nDefaultGUIResourceIndex, p_pUI, &m_SessionArray[nIndex], ROOM_BTN_CHARA_SEL_RANDOM_CHARA_SIZE_W, ROOM_BTN_CHARA_SEL_RANDOM_CHARA_SIZE_H, ROOM_BTN_CHARA_SEL_RANDOM_IMG_X, ROOM_BTN_CHARA_SEL_RANDOM_IMG_Y, ROOM_BTN_CHARA_SEL_RANDOM_CHARA_SIZE_W, ROOM_BTN_CHARA_SEL_RANDOM_CHARA_SIZE_H));
		}
		else
		{
			AddMessageLog(L"SetRoomCharacter no scrinfo!!!");
			return FALSE;
		}
	}
	else
		V(m_pRoomCharacters[nIndex]->Create(pScrInfo->pTexture, m_nDefaultGUIResourceIndex, p_pUI, &m_SessionArray[nIndex], pScrInfo->rec_tex_chr.right, pScrInfo->rec_tex_chr.bottom, pScrInfo->tex_chr_num, pScrInfo->draw_w, pScrInfo->draw_h));

	// 自キャラなら自キャラ情報窓を更新
	if (nIndex == m_nUserIndex)
		UpdateMyCharaInfo();

	return TRUE;
}

// セッション情報からキャラクタを更新
BOOL CGame::UpdateRoomCharacter(int nIndex)
{
	int nCharaType = m_SessionArray[nIndex].chara_type;

	// キャラランダム
	if (nCharaType == ROOM_CHARA_RANDOM_ID)
	{
		m_SessionArray[nIndex].scrinfo = NULL;
		m_pRoomCharacters[nIndex]->UpdateTexture(m_pDefaultGUIResourceTexture, ROOM_BTN_CHARA_SEL_RANDOM_CHARA_SIZE_W,ROOM_BTN_CHARA_SEL_RANDOM_CHARA_SIZE_H, ROOM_BTN_CHARA_SEL_RANDOM_IMG_X, ROOM_BTN_CHARA_SEL_RANDOM_IMG_Y, ROOM_BTN_CHARA_SEL_RANDOM_IMG_W, ROOM_BTN_CHARA_SEL_RANDOM_IMG_H);
		// 自キャラなら自キャラ情報窓を更新
		if (nIndex == m_nUserIndex)
			UpdateMyCharaInfo();
		return TRUE;
	}
	// キャラタイプIDからスクリプト情報を検索
	TCHARA_SCR_INFO* pScrInfo = common::scr::FindCharaScrInfoFromCharaType(nCharaType, &m_mapCharaScrInfo);
	// スクリプトとの関連付けの更新が必要なら更新
	if (m_SessionArray[nIndex].scrinfo != pScrInfo)
	{
		m_SessionArray[nIndex].scrinfo = pScrInfo;
		m_pRoomCharacters[nIndex]->UpdateTexture(pScrInfo->pTexture, pScrInfo->rec_tex_chr.right, pScrInfo->rec_tex_chr.bottom, pScrInfo->tex_chr_num, pScrInfo->draw_w, pScrInfo->draw_h);
		// 自キャラなら自キャラ情報窓を更新
		if (nIndex == m_nUserIndex)
			UpdateMyCharaInfo();
	}

	return TRUE;
}

// 自キャラ情報更新
void CGame::UpdateMyCharaInfo()
{
	// キャラ名が未設定なら
	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_MY_NAME);
	if (! pStatic->GetUserData() )
	{
		WCHAR wsName[MAX_USER_NAME+1];
		common::session::GetSessionName(&m_SessionArray[m_nUserIndex], wsName);
		pStatic->SetText(wsName);
		// 設定済みにする
		pStatic->SetUserData((void*)1L);
	}
}

// キャラ、アイテム、ルール表示ボタン
void CGame::OnClickViewButton(int nControl)
{
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_1)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_2)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_3)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_4)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_ADD_ITEM)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_ITEM_SEL_ICON)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_ITEM_INFO)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_CHARA_INFO)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_LIST_CHARA_BLT_INFO)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_CHARA_NAME)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_SC_NAME)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_SPRITE_SC)->SetVisible(false);

	p_pUI->GetControl(IDC_STATIC_SPIN_LIMIT_TURN_TEXT)->SetVisible(false);
	m_pSpinTurnLimit001->SetVisible(false);
	m_pSpinTurnLimit010->SetVisible(false);
	m_pSpinTurnLimit100->SetVisible(false);

	p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_TEXT)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_VALUE)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_L)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_R)->SetVisible(false);
	
	m_pItemBtnList->SetVisible(false);
	m_pCharacterBtnList->SetVisible(false);
	SetRect(&p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->GetElement(1)->rcTexture, IDC_CHECK_BTN_OFF);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->GetElement(1)->FontColor.States[DXUT_STATE_NORMAL]= D3DXCOLOR(0xFFFFFFFF);

	SetRect(&p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->GetElement(1)->rcTexture, IDC_CHECK_BTN_OFF);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->GetElement(1)->FontColor.States[DXUT_STATE_NORMAL] = D3DXCOLOR(0xFFFFFFFF);

	SetRect(&p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->GetElement(1)->rcTexture, IDC_CHECK_BTN_OFF);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->GetElement(1)->FontColor.States[DXUT_STATE_NORMAL] = D3DXCOLOR(0xFFFFFFFF);

	switch (nControl)
	{
	case IDC_ROOM_BTN_VIEW_CHARA:
		p_pUI->GetControl(IDC_ROOM_STATIC_CHARA_NAME)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_STATIC_CHARA_INFO)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_LIST_CHARA_BLT_INFO)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_STATIC_SC_NAME)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_SPRITE_SC)->SetVisible(true);
		m_pCharacterBtnList->SetVisible(true);
		SetRect(&p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->GetElement(1)->rcTexture, IDC_ROOM_BTN_VIEW_CHARA_RECT);
		p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->GetElement(1)->FontColor.States[DXUT_STATE_NORMAL] = D3DXCOLOR(0xFF0000FF);
		break;
	case IDC_ROOM_BTN_VIEW_ITEM:
		p_pUI->GetControl(IDC_ROOM_BTN_ADD_ITEM)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_BTN_ITEM_SEL_ICON)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_STATIC_ITEM_INFO)->SetVisible(true);
		m_pItemBtnList->SetVisible(true);
		SetRect(&p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->GetElement(1)->rcTexture, IDC_ROOM_BTN_VIEW_ITEM_RECT);
		p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->GetElement(1)->FontColor.States[DXUT_STATE_NORMAL] = D3DXCOLOR(0xFF0000FF);
		break;
	case IDC_ROOM_BTN_VIEW_RULE:
		p_pUI->GetControl(IDC_ROOM_CHK_RULE_1)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_CHK_RULE_2)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_CHK_RULE_3)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_CHK_RULE_4)->SetVisible(true);
		SetRect(&p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->GetElement(1)->rcTexture, IDC_ROOM_BTN_VIEW_RULE_RECT);
		p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->GetElement(1)->FontColor.States[DXUT_STATE_NORMAL] = D3DXCOLOR(0xFF0000FF);

		p_pUI->GetControl(IDC_STATIC_SPIN_LIMIT_TURN_TEXT)->SetVisible(true);
		m_pSpinTurnLimit001->SetVisible(true);
		m_pSpinTurnLimit010->SetVisible(true);
		m_pSpinTurnLimit100->SetVisible(true);

		p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_TEXT)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_VALUE)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_L)->SetVisible(true);
		p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_R)->SetVisible(true);
		break;
	}
}

// キャラ発言
// nIndex	:	キャラNo
// message	:	発言内容
void CGame::CharacterSay(int nIndex, WCHAR *message)
{
	// ユーザNo範囲外
	if (nIndex >= GetMaxLoginNum() || nIndex < 0)	return;

	if ( m_eGameState == eGameRoom)
	{
		if (m_pRoomCharacters[nIndex] && m_pRoomCharacters[nIndex]->IsCreated())
			m_pRoomCharacters[nIndex]->Say(message);
	}
}

// アイテムアイコンボタン押下
void CGame::OnSelectItemBtnList(CDXUTRadioButton* pBtn)
{
	int nIndex = (int)pBtn->GetUserData();
	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_ITEM_INFO);
	WCHAR wsDetail[MAX_GAME_ITEM_TEXT+9];
	SafePrintf(wsDetail, MAX_GAME_ITEM_TEXT+8, L"コスト:%02d\n%s", c_tItemResouceInfoTable[nIndex].cost, c_tItemResouceInfoTable[nIndex].text);
	pStatic->SetText(wsDetail);
//	pStatic->SetText(c_tItemResouceInfoTable[nIndex].text);

	// 選択したアイテムのアイコン切り替え
	CDXUTButton* pIconBtn = p_pUI->GetButton(IDC_ROOM_BTN_ITEM_SEL_ICON);
	if (nIndex != (int)pIconBtn->GetUserData())
	{
		RECT rcTexture;
		SetRect(&rcTexture,
			c_tItemResouceInfoTable[nIndex].left,
			c_tItemResouceInfoTable[nIndex].top,
			c_tItemResouceInfoTable[nIndex].right,
			c_tItemResouceInfoTable[nIndex].bottom
			);
		CDXUTElement* pElement = pIconBtn->GetElement(1);
		pElement->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
		pIconBtn->SetUserData((void*)nIndex);
	}
	p_pUI->GetControl(IDC_ROOM_BTN_ADD_ITEM)->SetEnabled(GetMySessionInfo()->cost>= c_tItemResouceInfoTable[nIndex].cost);
}

// キャラ選択ラジオボタンにチェックを入れる
void CGame::CheckCharacterSelectRadioBtnFromID(int nObjType)
{
	CDXUTRadioButton* pRadioButton = NULL;
	// ボタンに設定されたユーザデータと照合
	for (int i=0;pRadioButton = p_pUI->GetRadioButton(IDC_ROOM_BTN_CHARA_SEL_LIST_BASE+i);i++)
	{
		if (nObjType == (int)pRadioButton->GetUserData())
		{
			pRadioButton->SetChecked(true);
			break;	
		}
	}
}

#if RTRIAL

#else
// キャラ選択ラジオボタン押下イベント
void CGame::OnCharacterSelectRadioBtnDown(CDXUTRadioButton* pBtn)
{
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];

	int nCharaType = (int)pBtn->GetUserData();
	// 選択済みならしない
	if (m_SessionArray[m_nUserIndex].chara_type == nCharaType)	return;
	m_pCharacterBtnList->SetEnable(false);
	// キャラ選択パケット送信
	packetSize = PacketMaker::MakePacketData_RoomCharacterSelect((BYTE)nCharaType, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}
#endif

// 選択ステージの変更
void CGame::SetStageIndex(int nStageIndex)
{
//	if (m_nStageIndex == nStageIndex)	return;
	TSTAGE_SCR_INFO* pScrInfo = common::scr::FindStageScrInfoFromStageIndex(nStageIndex, &m_mapStageScrInfo);
	if (!pScrInfo) return;
	m_nStageIndex = nStageIndex;
	m_pSelectedStageScrInfo = pScrInfo;

	CDXUTButton* pBtnStageMain = p_pUI->GetButton(IDC_ROOM_BTN_STAGE_BASE);
	CDXUTStatic* pStaticStageTitle = p_pUI->GetStatic(IDC_ROOM_STATIC_STAGE_TITLE);
	// ステージ画像更新
	CDXUTElement* pElement = pBtnStageMain->GetElement(0);
	if (m_nStageIndex != (int)pBtnStageMain->GetUserData())
	{
		RECT rcTexture;
		SetRect(&rcTexture,0,0,	pScrInfo->thumnail.size.cx,pScrInfo->thumnail.size.cy);
		pElement->SetTexture(pScrInfo->res_index, &rcTexture);
		if (!pBtnStageMain->GetVisible())
			pBtnStageMain->SetVisible(true);
		// ステージタイトル更新
		pStaticStageTitle->SetText(pScrInfo->name);
		// 設定したステージインデックスを入れておく
		pBtnStageMain->SetUserData((void*)m_nStageIndex);
	}
	UpdateStageSelectButton();
}

void CGame::UpdateStageSelectButton()
{
	CDXUTButton* pBtnStageL = p_pUI->GetButton(IDC_ROOM_BTN_STAGE_L);
	CDXUTButton* pBtnStageR = p_pUI->GetButton(IDC_ROOM_BTN_STAGE_R);	// 選択ボタン設定
	CDXUTButton* pBtnStageRnd = p_pUI->GetButton(IDC_ROOM_BTN_STAGE_RND);	
	if (m_SessionArray[m_nUserIndex].master)
	{
		pBtnStageL->SetEnabled( m_nStageIndex > 0 );
		pBtnStageR->SetEnabled( (m_nStageIndex+1) < (int)m_mapStageScrInfo.size() );
		pBtnStageRnd->SetEnabled(true);
	}
	else
	{
		if (pBtnStageL->GetEnabled())	pBtnStageL->SetEnabled(false);
		if (pBtnStageR->GetEnabled())	pBtnStageR->SetEnabled(false);
		if (pBtnStageRnd->GetEnabled())	pBtnStageRnd->SetEnabled(false);
	}
}

// ステージ選択LRボタン押下
void CGame::OnStageSelectButtonDown(int nNum)
{
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];
	int nReqStageIndex = m_nStageIndex+nNum;
	// インデックスチェック
	if (nReqStageIndex < 0)
	{
		p_pUI->GetButton(IDC_ROOM_BTN_STAGE_L)->SetEnabled(false);
		return;
	}
	else if (nReqStageIndex >= (int)m_mapStageScrInfo.size())
	{
		p_pUI->GetButton(IDC_ROOM_BTN_STAGE_R)->SetEnabled(false);
		return;
	}

	packetSize = PacketMaker::MakePacketData_RoomSelectStage(nReqStageIndex, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}

void CGame::OnStageRandomButtonDown()
{
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];
	int nReqStageIndex = genrand_int32() % (int)m_mapStageScrInfo.size();
	packetSize = PacketMaker::MakePacketData_RoomSelectStage(nReqStageIndex, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}

// ルール用チェックボタン押下
void CGame::OnRuleCheckButtonDown(BYTE flg)
{
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];

	if (!m_SessionArray[m_nUserIndex].master)	return;
	// 変更
	m_bytRuleFlg ^= flg;
		
	packetSize = PacketMaker::MakePacketData_RoomRule(m_bytRuleFlg, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}
// ルール変更
void CGame::SetRule(BYTE rule_flg)
{
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_1)->SetChecked((rule_flg & GAME_RULE_01)?true:false);
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_2)->SetChecked((rule_flg & GAME_RULE_02)?true:false);
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_3)->SetChecked((rule_flg & GAME_RULE_03)?true:false);
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_4)->SetChecked((rule_flg & GAME_RULE_04)?true:false);
	m_bytRuleFlg = rule_flg;
}

// 制限ターン数設定
void CGame::SetTurnLimit(int turn)
{
	m_nLimitTurn = turn;
	int nTurn = turn;
	if (nTurn)
	{
		int nTemp = nTurn/100;
		m_pSpinTurnLimit100->SetValue(nTemp);
		nTurn -= nTemp*100;
		nTemp = nTurn/10;
		m_pSpinTurnLimit010->SetValue(nTemp);
		nTurn -= nTemp*10;
		m_pSpinTurnLimit001->SetValue(nTurn);
	}
	else
	{
		m_pSpinTurnLimit001->SetValue(0);
		m_pSpinTurnLimit010->SetValue(0);
		m_pSpinTurnLimit100->SetValue(0);
	}
}

// 制限時間設定更新
void CGame::SetActTimeLimit(int time)
{
	m_nActTimeLimit = time;
	WCHAR wsTime[4];
	SafePrintf(wsTime, 4, L"%02d", time);
	p_pUI->GetStatic(IDC_ROOM_STATIC_ACT_TIME_LIMIT_VALUE)->SetText(wsTime);
}

// 使用アイテム追加ボタン押下
void CGame::OnAddItemButtonDown()
{
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];

	// インデックス確認
	if (m_nUserIndex >= GetMaxLoginNum() || m_nUserIndex < 0)	return;
	int nItemIndex=0;
	// 空いているアイテム欄を探す
	for (;nItemIndex<GAME_ITEM_STOCK_MAX_COUNT;nItemIndex++)
	{
		if (!m_SessionArray[m_nUserIndex].items[nItemIndex])
			break;
	}
	// 見つからなかったら、追加ボタンを無効化
	if (nItemIndex >= GAME_ITEM_STOCK_MAX_COUNT)
	{
		p_pUI->GetControl(IDC_ROOM_BTN_ADD_ITEM)->SetEnabled(false);
		return;
	}
	// 選択中のアイテムアイコンのユーザーデータにアイテムのインデックスが保持されている
	CDXUTButton* pIconBtn = p_pUI->GetButton(IDC_ROOM_BTN_ITEM_SEL_ICON);
	int nItemTableIndex = (int)pIconBtn->GetUserData();
	// インデックス確認
	if (nItemTableIndex < 0 && nItemTableIndex >= GAME_ITEM_COUNT)	return;
	DWORD dwItemFlg = c_tItemResouceInfoTable[nItemTableIndex].flg;

	// アイテム選択要求の結果パケット受信まで追加ボタン無効化
	p_pUI->GetControl(IDC_ROOM_BTN_ADD_ITEM)->SetEnabled(false);		
	packetSize = PacketMaker::MakePacketData_RoomSelectItem(nItemIndex, dwItemFlg, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}

// 自キャラの使用アイテムの更新
void CGame::UpdateMyItems()
{
	// アイテムの表示更新
	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
	{
		CDXUTButton* pBtn = p_pUI->GetButton(IDC_ROOM_BTN_MY_ITEM_BASE+i);
		if (!(m_bytRuleFlg & GAME_RULE_ITEM_ENABLE))
		{
			pBtn->SetEnabled(false);
			continue;
		}
		// フラグがNULLなら未使用欄で非表示
		if (!GetMySessionInfo()->items[i])
		{
			pBtn->SetUserData((void*)0);
			pBtn->GetElement(1)->TextureColor.Init(0,0);
			pBtn->SetEnabled(false);
		}
		else
		{
			// アイテムを探す
			int j=0;
			int nItemFlg = 0;
			for (;j<GAME_ITEM_COUNT;j++)
			{
				if (m_SessionArray[m_nUserIndex].items[i] == c_tItemResouceInfoTable[j].flg)
				{
					nItemFlg = c_tItemResouceInfoTable[j].flg;
					break;
				}
			}
			// アイテムテーブルになかった
			if (j== GAME_ITEM_COUNT)
			{
				// 未使用に設定要求
				WORD			packetSize = 0;
				BYTE				packetData[MAX_PACKET_SIZE];
				packetSize = PacketMaker::MakePacketData_RoomSelectItem(i, 0x0, packetData);
				if (packetSize)
					AddPacket(packetData, packetSize);
				continue;
			}
			// テクスチャセット、設定済みなら設定しない
			if ((int)pBtn->GetUserData() != nItemFlg)
			{
				pBtn->SetUserData((void*)c_tItemResouceInfoTable[j].flg);

				// テクスチャセット
				RECT rcTexture;
				SetRect(&rcTexture,
					c_tItemResouceInfoTable[j].left,
					c_tItemResouceInfoTable[j].top,
					c_tItemResouceInfoTable[j].right,
					c_tItemResouceInfoTable[j].bottom);
				CDXUTElement* pElement = pBtn->GetElement(1);
				pElement->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);
				pElement->TextureColor.Init(0xFFFFFFFF,0xFFFFFFFF);
			}
			// 表示
			if (!pBtn->GetEnabled())
				pBtn->SetEnabled(true);

			if (m_eGameState == GAME_STATE_MAIN) 
			{
				if (!pBtn->GetVisible())
					pBtn->SetVisible(true);
			}
		}
	}
	
	WCHAR wsCost[8];
	SafePrintf(wsCost, 8, L"%d", (int)m_SessionArray[m_nUserIndex].cost);
	CDXUTStatic* pStatic =  p_pUI->GetStatic(IDC_ROOM_STATIC_ITEM_REST_COST);
	pStatic->SetText(wsCost);
	// アイテム追加ボタンの有効状態更新
	CheckEnableItemAddButton();
}

// アイテム追加ボタンの有効状態確認
void CGame::CheckEnableItemAddButton()
{
	bool bEnable = false;
	CDXUTButton* pAddBtn = p_pUI->GetButton(IDC_ROOM_BTN_ADD_ITEM);
	// 準備OK状態なら無効
	if (!m_SessionArray[m_nUserIndex].game_ready)
	{
		// 追加可能なアイテム欄があるか確認
		for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
		{
			// フラグがNULLなら未使用欄
			if (!m_SessionArray[m_nUserIndex].items[i])
			{
				bEnable = true;
				break;
			}
		}
		if (bEnable)
		{
			// 選択中のアイテムのコストと残りコストを確認
			int nItemTableIndex = (int)p_pUI->GetButton(IDC_ROOM_BTN_ITEM_SEL_ICON)->GetUserData();
			if (nItemTableIndex >= 0 && nItemTableIndex < GAME_ITEM_COUNT)
				bEnable = (m_SessionArray[m_nUserIndex].cost >= c_tItemResouceInfoTable[nItemTableIndex].cost);
		}
	}
	// 状態設定
	if (pAddBtn->GetEnabled() != bEnable)
		pAddBtn->SetEnabled(bEnable);
}

// 使用アイテム一覧クリック(使用アイテムを消す)
BOOL CGame::OnMyItemButtonDown(int nControlID)
{
	int nItemIndex = nControlID - IDC_ROOM_BTN_MY_ITEM_BASE;
	if (nItemIndex < 0 || nItemIndex >= GAME_ITEM_STOCK_MAX_COUNT)	return FALSE;
	if (!GetMySessionInfo()->items[nItemIndex])
		return FALSE;

	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];
	// 0x00で使用アイテムを消す
	packetSize = PacketMaker::MakePacketData_RoomSelectItem(nItemIndex, 0x00, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
	return TRUE;
}

// チーム設定変更ボタン押下
void CGame::OnTeamRuleButtonDown(int nNum)
{
	// マスター以外は変更できない FAILSAFE
	if (!m_SessionArray[m_nUserIndex].master)
	{
		p_pUI->GetControl(IDC_ROOM_BTN_RULE_TEAM_L)->SetEnabled(false);
		p_pUI->GetControl(IDC_ROOM_BTN_RULE_TEAM_R)->SetEnabled(false);
		return;
	}
	// 変更要求値の確認
	int nReqTeamCount = m_nTeamCount+nNum;
//> 20101105 端数でもチーム設定できるようにする
	int nMaxTeamCount = min(MAXUSERNUM-1,m_nAuthedCount-1);
//	int nMaxTeamCount = (int)( (float)m_nAuthedCount/ 2.0f);	

//	BOOL bOver = TRUE;
	// 割り切れる数を探す
//	while (nReqTeamCount >= 1 && nReqTeamCount <= nMaxTeamCount)
//	{
//		if (!(m_nAuthedCount % nReqTeamCount))
//		{
//			bOver = FALSE;
//			break;
//		}
//		nReqTeamCount += nNum;
//	}
	// 変更値が最大・最小値を越えていた
//	if (bOver)
//	{
		if (nReqTeamCount < 1)
		{
			p_pUI->GetControl(IDC_ROOM_BTN_RULE_TEAM_L)->SetEnabled(false);
			return;
		}
		// 最大チーム数を越えているか確認
		if (nReqTeamCount > nMaxTeamCount)
		{
			p_pUI->GetControl(IDC_ROOM_BTN_RULE_TEAM_R)->SetEnabled(false);
			return;
		}
//	}
//> 20101105 端数でもチーム設定できるようにする

	// パケット送信
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];
	packetSize = PacketMaker::MakePacketData_RoomTeamCount(nReqTeamCount, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}

// チーム数設定変更
void CGame::SetTeamCount(int nTeamCount)
{
	m_nTeamCount = nTeamCount;
	m_pTeamRulePropertyManager->Update((GetMySessionInfo()->master!=0), m_nTeamCount);
	
	m_pTeamSeparater->Update(m_nTeamCount);

	if (g_bDebug)
	{
		WCHAR log[32];
		SafePrintf(log, 32, L"SetTeamCount:%d", nTeamCount);
		AddMessageLog(log);
	}
	//> チャットログに切断メッセージ追加
	if (m_eGameState == eGameRoom)
	{
		PlaySysSoundSE(SE_sai_SrvInfo);
		if (nTeamCount==1)
			AddChatMessage(L"チーム設定[個人バトル]に設定されました", PK_USER_CHAT_SERVER_INFO);
		else
		{
			WCHAR wsMessage[MAX_CHAT_MSG+1];
			SafePrintf(wsMessage, MAX_CHAT_MSG, L"チーム設定[%dチームバトル]に設定されました", nTeamCount);
			CDXUTListBox* pListBox = (CDXUTListBox*)p_pUI->GetControl(IDC_ROOM_LB_CHATLOG);
			AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_INFO);
		}
	}
	//< チャットログに切断メッセージ追加
}

// マスターが設定できるコントロール状態更新(チーム分け、ステージ選択、ルール)
void CGame::UpdateGameMasterPropertyControlsEnabled()
{
	bool b = m_SessionArray[m_nUserIndex].master?true:false;
	// ステージ選択設定
	UpdateStageSelectButton();
	// チーム分け設定
	UpdateTeamRuleProperty();
	// ゲームルールの有効状態
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_1)->SetEnabled(b);
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_2)->SetEnabled(b);
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_3)->SetEnabled(b);
	p_pUI->GetControl(IDC_ROOM_CHK_RULE_4)->SetEnabled(b);

	SetLimitTurnEnabled(b);
	SetActTimeLimitEnabled(b);
}

// チーム設定更新
void CGame::UpdateTeamRuleProperty()
{
	// マスター以外は設定不可
	m_pTeamRulePropertyManager->SetEnable(m_SessionArray[m_nUserIndex].master);
	m_pTeamRulePropertyManager->Update((m_SessionArray[m_nUserIndex].master!=0), m_nTeamCount);
}

// ルール変更ボタン状態
void CGame::UpdateGameRuleState()
{
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_1)->SetEnabled(m_SessionArray[m_nUserIndex].master?true:false);
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_2)->SetEnabled(m_SessionArray[m_nUserIndex].master?true:false);
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_3)->SetEnabled(m_SessionArray[m_nUserIndex].master?true:false);
	p_pUI->GetCheckBox(IDC_ROOM_CHK_RULE_4)->SetEnabled(m_SessionArray[m_nUserIndex].master?true:false);
}

// 準備OKボタン押下
void CGame::OnReadyButtonDown()
{
	WORD			packetSize = 0;
	BYTE				packetData[MAX_PACKET_SIZE];
	
	CDXUTButton* pBtn = p_pUI->GetButton(IDC_ROOM_BTN_READY);
	// マスターの場合
	if (m_SessionArray[m_nUserIndex].master)
	{
		if (!CheckChangeReadyStateOfMaster())
			return;
	}
	else	// マスター以外
	{
		if (!CheckChangeReadyStateOfUser())
			return;
	}

	BOOL bReady = !m_SessionArray[m_nUserIndex].game_ready;
	packetSize = PacketMaker::MakePacketData_RoomReady(bReady, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);
}

// 準備OKにセット可能か確認
BOOL CGame::CheckChangeReadyStateOfMaster()
{
	// 準備OKなら、ゲーム開始中のため変更させない
	if (m_SessionArray[m_nUserIndex].game_ready)	return FALSE;
	// 他ユーザが準備OKか確認
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (i == m_nUserIndex)	continue;
		if (m_SessionArray[i].connect_state != CONN_STATE_AUTHED)	continue;
		// マスターはマスター以外が準備OKでない場合、変更不可
		if (!m_SessionArray[i].game_ready)
			return FALSE;
	}

	// チームわけ確認 //
//	if (m_nTeamCount <= 1)	return TRUE;
	BOOL bReady = !m_SessionArray[m_nUserIndex].game_ready;
	if (bReady)
		return CheckAddTeam();
	return TRUE;
}

BOOL CGame::CheckChangeReadyStateOfUser()
{
	// マスターが準備OKか確認
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_SessionArray[i].connect_state != CONN_STATE_AUTHED)
			continue;
		// マスターが準備OKの場合はゲーム開始中のため、変更不可
		if (m_SessionArray[i].master	&& m_SessionArray[i].game_ready)
			return FALSE;
	}
	// チームわけ確認 //
	if (m_nTeamCount <= 1)	return TRUE;
	BOOL bReady = !m_SessionArray[m_nUserIndex].game_ready;
//> 20101105 端数でもチーム設定できるようにする
//	if (bReady)
//		return CheckAddTeam();
//< 20101105 端数でもチーム設定できるようにする
	return TRUE;
}

// チームに追加確認
BOOL CGame::CheckAddTeam()
{
	// 一人プレイならチェックしない
	if (g_bOneClient)
		return TRUE;

	int nEntryUserCount = 0;
	// チームわけなしなら観戦人数チェック
	if (m_nTeamCount <= 1)
	{
		for (int i=0;i<GetMaxLoginNum();i++)
		{
			if (m_SessionArray[i].connect_state != CONN_STATE_AUTHED)	continue;
			if (m_SessionArray[i].lx >= ROOM_ENTRY_LEFT && m_SessionArray[i].lx < ROOM_ENTRY_RIGHT)
				nEntryUserCount++;
		}
		if (nEntryUserCount <= 1)
		{
			AddChatMessage(L"参加人数が少ないため、ゲームが開始できません", PK_USER_CHAT_SERVER_INFO);
			return FALSE;
		}
		else if (nEntryUserCount  > MAXUSERNUM)
		{
			AddChatMessage(L"最大参加人数(８人）を超過しているため、ゲームが開始できません", PK_USER_CHAT_SERVER_INFO);
			return FALSE;
		}
		return TRUE;
	}
//> 20101105 端数でもチーム設定できるようにする
/*
	// 定員オーバー確認
	int nTeamUserNum = 0;
	for (int j=0;j<MAXUSERNUM;j++)
	{
		if (m_SessionArray[j].connect_state != CONN_STATE_AUTHED)	continue;
		if (!m_SessionArray[j].game_ready) continue;

		int x = m_SessionArray[j].ax;
		int y = m_SessionArray[j].ay;
		// キャラの座標からチーム番号取得
		int nTeamNo = m_pTeamSeparater->GetTeamNoFromPos(x,y);
		// 準備済みの同チームキャラ
		if ( nCheckTeamNo == nTeamNo)
		{
			nTeamUserNum++;
			// 定員を満たしていたら追加不可
			if (nTeamUserNum >= m_nMaxSeats)
				return FALSE;
		}
	}
*/
	int nTeamWidth = (int)(WIN_WIDTH / m_nTeamCount);
	int nTeamSeats[MAX_TEAM_COUNT];
	ZeroMemory(nTeamSeats, sizeof(int)*MAX_TEAM_COUNT);
	for (int j=0;j<GetMaxLoginNum();j++)
	{
		if (m_SessionArray[j].connect_state != CONN_STATE_AUTHED)	continue;
		// マスター以外が準備OKじゃない場合、無効
		if (!m_SessionArray[j].game_ready && !m_SessionArray[j].master)
			return FALSE;

		int x = m_SessionArray[j].lx;
		int y = m_SessionArray[j].ly;
		// キャラの座標からチーム番号取得
		int nTeamNo = m_pTeamSeparater->GetTeamNoFromPos(x,y);
		if (nTeamNo < MAX_TEAM_COUNT)
		{
			nEntryUserCount++;
			nTeamSeats[nTeamNo]++;
		}
	}
	if (nEntryUserCount > MAXUSERNUM)
	{
		AddChatMessage(L"最大参加人数(８人）を超過しているため、ゲームが開始できません", PK_USER_CHAT_SERVER_INFO);
		return FALSE;
	}
	// 人の居ないチームがある場合、チーム分け失敗
	for (int i=0;i<m_nTeamCount;i++)
	{
		if (!nTeamSeats[i])
		{
			AddChatMessage(L"人の居ないチームが在ります、チーム分けを確認してください", PK_USER_CHAT_SERVER_INFO);
			return FALSE;
		}
	}
//< 20101105 端数でもチーム設定できるようにする
	return TRUE;
}

// マスター情報設定
void CGame::SetGameMaster(int nIndex, BOOL bMaster)
{
	ptype_session sess = GetAuthedSessionInfo(nIndex);
	if (!sess) return;
	sess->master = bMaster;
	// 準備状態を解除する
	sess->game_ready = 0;
	// チーム番号をクリアする
	if (sess->obj_state & OBJ_STATE_ROOM)
		sess->team_no = 0;

	m_pRoomCharacters[nIndex]->UpdateState();
	// 自身のマスター情報が変わった場合、有効状態を変更
	UpdateGameMasterPropertyControlsEnabled();
	// 自分がマスターならチャットログ+音で知らせる
	if (nIndex == m_nUserIndex)
	{
		PlaySysSoundSE(SE_sai_SrvInfo);
		if (bMaster)
			AddChatMessage(L"マスター権限に設定されました。ステージ、ゲームルール、チームバトル設定が変更可能です。", PK_USER_CHAT_SERVER_INFO);
		else
			AddChatMessage(L"マスター権限が解除されました。", PK_USER_CHAT_SERVER_INFO);
	}
}


void CGame::UpdatePropertyControlsEnabled()
{
	// 準備OKの場合無効
	bool b = !m_SessionArray[m_nUserIndex].game_ready;

	p_pUI->GetControl(IDC_ROOM_BTN_ADD_ITEM)->SetEnabled(b);
	m_pCharacterBtnList->SetEnable(b);
	m_pItemBtnList->SetEnable(b);
	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
	{
		if (p_pUI->GetControl(IDC_ROOM_BTN_MY_ITEM_BASE+i)->GetUserData())
			p_pUI->GetControl(IDC_ROOM_BTN_MY_ITEM_BASE+i)->SetEnabled(b);
		else
			p_pUI->GetControl(IDC_ROOM_BTN_MY_ITEM_BASE+i)->SetEnabled(false);
	}
}

// 準備OKボタンの状態更新
void CGame::UpdateReadyButtonState()
{
	CDXUTButton *pBtn = p_pUI->GetButton(IDC_ROOM_BTN_READY);
	bool bEnable = true;
	// マスター
	if (m_SessionArray[m_nUserIndex].master)
	{
		if (g_bOneClient) 
		{
			pBtn->SetEnabled(true);
		}
		else
		{
			if (m_nAuthedCount == 1)
				bEnable = false;
			else
			{
				// 他ユーザが準備OKか確認
				for (int i=0;i<GetMaxLoginNum();i++)
				{
					if (i == m_nUserIndex)	continue;
					if (m_SessionArray[i].connect_state != CONN_STATE_AUTHED)	continue;
					// マスターはマスター以外が準備OKでない場合、変更不可
					if (!m_SessionArray[i].game_ready)
					{
						bEnable = false;
						break;
					}
				}
			}
			pBtn->SetEnabled(bEnable);
			// マスター以外が準備OK
			if (bEnable && !m_SessionArray[m_nUserIndex].game_ready)
			{
				AddChatMessage(L"OKボタンを押すとゲームが開始できます", PK_USER_CHAT_SERVER_INFO);
				PlaySysSoundSE(SE_sai_SrvInfo);
			}
		}
	}
	else
	{
		pBtn->SetEnabled(true);
		if (m_SessionArray[m_nUserIndex].game_ready)
			bEnable = false;
	}
	
	// 状態により、画像変更
	CDXUTElement* pElement = pBtn->GetElement(1);
	RECT rcButton;
	if (bEnable)
		SetRect(&rcButton, ROOM_BTN_READY_IMG_RECT);
	else
		SetRect(&rcButton, ROOM_BTN_READY_DIS_IMG_RECT);

	pElement->SetTexture( m_nDefaultGUIResourceIndex, &rcButton);
}

// 準備状態更新
void CGame::SetGameReady(int nIndex, BOOL bReady)
{
	ptype_session pSess = GetAuthedSessionInfo(nIndex);
	if (!pSess) return;
	pSess->game_ready = bReady;

	// マスターがゲーム準備OKの場合、開始
//	if (pSess->game_ready && pSess->master)
//		m_eGameState = eGameRoomRelease;

	m_pRoomCharacters[nIndex]->UpdateState();

	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_MY_TEAMNO);
	// チーム番号設定
	if (nIndex == m_nUserIndex)
	{
		if (bReady && m_nTeamCount>1)
		{
			int nTeamNo =	m_pTeamSeparater->GetTeamNoFromPos(m_SessionArray[m_nUserIndex].lx, m_SessionArray[m_nUserIndex].ly);
			pStatic->SetTextColor(m_pTeamSeparater->GetTeamColor(nTeamNo));
			pStatic->SetText(m_pTeamSeparater->GetTeamWchar(nTeamNo));
		}
		else
		{
			pStatic->SetText(NULL);
		}
		// 設定コントロール状態更新
		UpdatePropertyControlsEnabled();
	}
}

// キャラの再入室処理
void CGame::ReEnterCharacter(int nCharaIndex)
{
	if (nCharaIndex == m_nUserIndex)
	{
		SetRule(m_bytRuleFlg);
		UpdateMyItems();
		p_pUI->GetControl(IDC_ROOM_BTN_STAGE_BASE)->SetUserData((void*)-1);
		SetStageIndex(m_nStageIndex);
	}
	else
		AddLogRoomIn(nCharaIndex);

	SetGameMaster(nCharaIndex, m_SessionArray[nCharaIndex].master);
	SetGameReady(nCharaIndex, m_SessionArray[nCharaIndex].game_ready);
	UpdateReadyButtonState();
	m_pRoomCharacters[nCharaIndex]->UpdateState();
	m_pRoomCharacters[nCharaIndex]->Show();
	
	if (g_bDebug)
	{
		WCHAR log[32];
		SafePrintf(log, 32, L"ReEnter(#%d)", nCharaIndex);
		AddMessageLog(log);
	}

}

// 制限ターン数のコントロールの情報を使って表示更新/パケット送信
void CGame::UpdateLimitTurnFromControl(int modvalue)
{
	// マスターか確認
	if (!m_SessionArray[m_nUserIndex].master)	return;

	int nNewLimitTurn = 0;
	if (modvalue > 0)	// 増加
		nNewLimitTurn = min(999, max(m_nAuthedCount+1, m_nLimitTurn + modvalue));	
	else
		nNewLimitTurn = (m_nLimitTurn+modvalue) <= m_nAuthedCount?0:(m_nLimitTurn+modvalue);

	if (m_nLimitTurn != nNewLimitTurn)
	{
		// パケット送信
		BYTE pkt[MAX_PACKET_SIZE];
		INT pktSize = PacketMaker::MakePacketData_RoomRuleTurnLimit(nNewLimitTurn, pkt);
		AddPacket(pkt, pktSize);
	}
}

// 制限ターン数の有効/無効設定
void CGame::SetLimitTurnEnabled(bool b)
{
	m_pSpinTurnLimit001->SetEnabled(b);
	m_pSpinTurnLimit010->SetEnabled(b);
	m_pSpinTurnLimit100->SetEnabled(b);
}

void CGame::SetActTimeLimitEnabled(bool b)
{
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_L)->SetEnabled(b);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_R)->SetEnabled(b);
}

// 
void CGame::OnActTimeLimitButtonDown(int nNum)
{
	// 上限下限チェック
	int nActTimeLimit = m_nActTimeLimit+nNum;
	if (nActTimeLimit < GAME_TURN_ACT_COUNT_MIN
	|| nActTimeLimit > GAME_TURN_ACT_COUNT_MAX)
		return;

	// パケット送信
	BYTE pkt[MAX_PACKET_SIZE];
	INT pktSize = PacketMaker::MakePacketData_RoomRuleActTimeLimit(GetMySessionInfo()->sess_index, nActTimeLimit, pkt);
	AddPacket(pkt, pktSize);
}

bool CGame::IsTeamRnadomizeCommand(LPCWSTR pStr)
{
	if (!GetMySessionInfo()->master) return false;
	const WCHAR* pCmd = L"/rand";
	size_t size = wcslen(pCmd);
	if (size > wcslen(pStr) || size+1 < wcslen(pStr)) return false;
	if (wcsncmp(pStr, pCmd, size) == 0)
		return true;
	return false;
}

int CGame::GetTeamRnadomizeCount(LPCWSTR pStr)
{
	const WCHAR* pCmdTeamRand = L"/rand";

	if (wcscmp(pStr, pCmdTeamRand) == 0)
		return 2;

	size_t pos = wcslen(pCmdTeamRand);
	int num = pStr[pos] - L'0';
	if (num >= 1 && num <= MAX_TEAM_COUNT)
		return num;
	return 0;
}


