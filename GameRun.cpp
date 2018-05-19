#include "ext.h"
#include "Game.h"
// CGame::Run >

// ========================================================================= //
// CGame::Run > TitleInit
// ========================================================================= //
BOOL CGame::TitleInit()
{
//===========================可変============================//
	RECT rcTexture;
	if (!p_pUI)	return FALSE;
	// frame
	CDXUTSprite* pSprite;
	p_pUI->AddSprite( IDC_DLG_FRAME, 280, 300, 240, 120, false, &pSprite );
	SetRect( &rcTexture, TITLE_RRAME_RECT_RECT	);	// 描画範囲
	pSprite->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定

	// Edit box
    CDXUTEditBox *pEdit;
	WCHAR wsIP[64];
	
	m_pIniConfig->ReadString(L"LOGIN", L"IP", L"127.0.0.1", wsIP, 64);
	wsIP[63] = NULL;

    if( SUCCEEDED( p_pUI->AddEditBox( IDC_TITLE_EDT_IP, wsIP, TITLE_EDIT_RECT, false, &pEdit ) ) )
	{
        pEdit->GetElement( 0 )->iFont = 1;
        pEdit->GetElement( 1 )->iFont = 1;
		pEdit->GetElement( 0 )->TextureColor.Init( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );  // Transparent center
		pEdit->SetBorderWidth(0);
        pEdit->SetTextColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		pEdit->SetCaretColor(D3DCOLOR_ARGB( 255, 0, 0, 0 ));
        pEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
        pEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}

	// 接続Butons
	CDXUTButton *pBtn;
	p_pUI->AddButton( IDC_TITLE_BTN_CONNECT, L"connect", 410, 375, 48, 32, L'I', false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,0);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 0, 0);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, BTN_W48_RECT	);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定

	// exit
	p_pUI->AddButton( IDC_TITLE_BTN_EXIT, L"exit", 460, 375, 48, 32, L'I', false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,0);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 0, 0);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, BTN_W48_RECT	);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定

	// config
	p_pUI->AddButton( IDC_TITLE_BTN_CONFIG, L"config", 300, 375, 48, 32, L'I', false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,0);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 0, 0);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, BTN_W48_RECT	);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定

	// IP static
	CDXUTStatic* pStatic; 
	p_pUI->AddStatic( IDC_TITLE_STATIC_IP, L"IP", TITLE_STATIC_ID_RECT, false, &pStatic );
	pStatic->GetElement( 0 )->iFont = 1;	

	// 背景
	SafeRelease(m_pBGTexture);
	BYTE *pBuf;
	UINT	nBufSize;
	SafePrintf(m_pBGTexturePath, sizeof(WCHAR)*_MAX_PATH+1, TITLE_BG_TEXTURE);
	g_pFiler->GetFileMemory(m_pBGTexturePath, &pBuf, &nBufSize);
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'$');
	while (pBuf)
	{
		if (TextureLoader::LoadTextureFromFileInMemory(&m_pBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL, 0, 1))
			SafeDeleteArray(pBuf);
		else
			MessageBox(g_hWnd, L"背景ロードエラー", L"error", MB_OK);
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	if (g_bCmdSetup)
	{
		if (ConnectServer())
			return FALSE;
		else
		{
			g_bCmdSetup = FALSE;
		}
	}
	// BGM再生
	PlayBGM(BGM_TITLE);
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > Title
// ========================================================================= //
BOOL CGame::Title()
{
	BOOL ret = FALSE;
//===========================可変============================//

//===========================================================//
	g_pCriticalSection->EnterCriticalSection_Sound(L'-');
	m_pSoundPlayer->Frame();
	g_pCriticalSection->LeaveCriticalSection_Sound();
	return ret;
}

// ========================================================================= //
// CGame::Run > TitleRelease
// ========================================================================= //
BOOL CGame::TitleRelease()
{
//===========================可変============================//
	// UI破棄
	p_pUI->RemoveControl(IDC_TITLE_EDT_IP);
	p_pUI->RemoveControl(IDC_TITLE_BTN_CONNECT);
	p_pUI->RemoveControl(IDC_TITLE_STATIC_IP);
	if (g_bCloseSocket)
		return FALSE;
	p_pUI->ClearFocus();
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > LoginInit
// ========================================================================= //
BOOL CGame::LoginInit()
{
//===========================可変============================//
	RECT rcTexture;
	if (!StartClient(m_nTcpSock))
		return FALSE;

	// ユーザーIndexを無効化
	g_pGame->SetUserIndex(-1);

	// IME Edit box
	CDXUTIMEEditBox* pIMEEdit = (CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT);
	pIMEEdit->SetLocation(LOGIN_USERNAME_EDIT_X, LOGIN_USERNAME_EDIT_Y);
	pIMEEdit->SetSize(LOGIN_USERNAME_EDIT_W, LOGIN_USERNAME_EDIT_H);
	pIMEEdit->SetVisible(true);

	WCHAR wsPass[MAX_SRV_PASS+1];
	m_pIniConfig->ReadString(L"LOGIN", L"PASS", L"", wsPass, MAX_SRV_PASS);
	wsPass[MAX_SRV_PASS] = NULL;

	// パスワード
    CDXUTEditBox *pEdit;
    if( SUCCEEDED( p_pUI->AddEditBox(
			IDC_LOGIN_EDT_PASSWORD,
			wsPass,
			LOGIN_PASSWORD_EDIT_X,
			LOGIN_PASSWORD_EDIT_Y,
			LOGIN_PASSWORD_EDIT_W,
			LOGIN_PASSWORD_EDIT_H, false, &pEdit ) ) )
	{
        pEdit->GetElement( 0 )->iFont = 1;
        pEdit->GetElement( 1 )->iFont = 1;
		pEdit->GetElement( 0 )->TextureColor.Init( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );  // Transparent center
		pEdit->SetBorderWidth(0);
        pEdit->SetTextColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
        pEdit->SetCaretColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ));
        pEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
        pEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}

	// Butons
	CDXUTButton *pBtn;
	p_pUI->AddButton( IDC_LOGIN_BTN_LOGIN, L"login", 410, 375, 48, 32, L'I', false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,0);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 0, 0);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, BTN_W48_RECT	);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetEnabled(false);

	CDXUTCheckBox* pChkBox=NULL;
	p_pUI->AddCheckBox( IDC_LOGIN_CHK_SAVE_PASS, L"", LOGIN_CHK_SAVE_PASS_CNT_RECT, m_bSavePass, 0, false, &pChkBox);
	SetRect( &rcTexture, LOGIN_CHK_SAVE_PASS_IMG_BOX_RECT	);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, LOGIN_CHK_SAVE_PASS_IMG_CHK_RECT	);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);

	// Name static
	CDXUTStatic* pStatic; 
	p_pUI->AddStatic( IDC_LOGIN_STATIC_NAME, L"Name", LOGIN_STATIC_NAME_RECT, false, &pStatic );
	pStatic->GetElement( 0 )->iFont = 1;	
	// Pass static
	p_pUI->AddStatic( IDC_LOGIN_STATIC_PASS, L"Pass", LOGIN_STATIC_PASS_RECT, false, &pStatic );
	pStatic->GetElement( 0 )->iFont = 1;

	if (g_bCmdSetup)
	{
		if (!LoginServer())
			g_bCmdSetup = FALSE;
	}
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > Login
// ========================================================================= //
BOOL CGame::Login()
{
	BOOL ret = FALSE;
//===========================可変============================//
	if (!p_pUI->GetControl(IDC_LOGIN_BTN_LOGIN)->GetEnabled())
		p_pUI->GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(true);
//===========================================================//
	g_pCriticalSection->EnterCriticalSection_Sound(L'^');
	m_pSoundPlayer->Frame();
	g_pCriticalSection->LeaveCriticalSection_Sound();
	return ret;
}

// ========================================================================= //
// CGame::Run > RoomRelease
// ========================================================================= //
BOOL CGame::LoginRelease()
{
//===========================可変============================//
	// UI破棄
	p_pUI->RemoveControl(IDC_LOGIN_EDT_PASSWORD);
	p_pUI->RemoveControl(IDC_LOGIN_BTN_LOGIN);
	p_pUI->RemoveControl(IDC_LOGIN_STATIC_NAME);
	p_pUI->RemoveControl(IDC_LOGIN_STATIC_PASS);
	p_pUI->RemoveControl(IDC_TITLE_BTN_EXIT);
	p_pUI->RemoveControl(IDC_TITLE_BTN_CONFIG);
	p_pUI->RemoveControl(IDC_DLG_FRAME);
	p_pUI->RemoveControl(IDC_LOGIN_CHK_SAVE_PASS);
	p_pUI->GetControl(IDC_SHARE_EDIT)->SetVisible(false);
	p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->ClearText();
	
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'%');
	SafeRelease(m_pBGTexture);
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
	if (g_bCloseSocket)
		return FALSE;

	p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->GetElement(0)->TextureColor.States[DXUT_STATE_NORMAL] = 0xFFAAAAAA;
	p_pUI->ClearFocus();

	// タイトル・ログインBGM停止
	StopBGM(TRUE);

//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > RoomInit
// ========================================================================= //
BOOL CGame::RoomInit()
{
//===========================可変============================//
	m_nSavedInputX = 0;
	m_nSavedInputY = 0;
	Sleep(10);
	// キャラ選択
	SetChacterSelectInfo( m_SessionArray[m_nUserIndex].chara_type);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->SetVisible(true);
	// 使用アイテム情報
	UpdateMyItems();

	CDXUTIMEEditBox* pIMEEdit = (CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT);
	SaveIMEText();
	pIMEEdit->SetLocation(ROOM_CHAT_EDIT_X, ROOM_CHAT_EDIT_Y);
	pIMEEdit->SetSize(ROOM_CHAT_EDIT_W, ROOM_CHAT_EDIT_H);
	pIMEEdit->SetVisible(true);

	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_ROOM_STATIC_MY_NAME);
	pStatic->SetLocation(ROOM_STATIC_MY_NAME_CNT_X,ROOM_STATIC_MY_NAME_CNT_Y);
	pStatic->SetSize(ROOM_STATIC_MY_NAME_CNT_W,ROOM_STATIC_MY_NAME_CNT_H);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_MY_NAME_FONT,0xFF000000, DT_LEFT|DT_TOP|DT_SINGLELINE);
	pStatic->SetDrawShadow(false);
	pStatic->SetVisible(true);

	p_pUI->GetControl(IDC_ROOM_BTN_DISC)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_CMB_CHAT)->SetLocation(ROOM_CHAT_CMB_CNT_X, ROOM_CHAT_CMB_CNT_Y);
	p_pUI->GetControl(IDC_ROOM_CMB_CHAT)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetLocation(ROOM_CHATLOG_LB_CNT_X, ROOM_CHATLOG_LB_CNT_Y);
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetSize(ROOM_CHATLOG_LB_CNT_W, ROOM_CHATLOG_LB_CNT_H);
	p_pUI->GetColorListBox(IDC_ROOM_LB_CHATLOG)->SetDrawTextShadow(false);
	p_pUI->GetColorListBox(IDC_ROOM_LB_CHATLOG)->GetScrollBar()->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetVisible(true);
	p_pUI->GetColorListBox(IDC_ROOM_LB_USERLIST)->GetScrollBar()->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_LB_USERLIST)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_READY)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_STATIC_MY_TEAMNO)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_STATIC_STAGE_TITLE)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_STAGE_L)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_STAGE_R)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_STAGE_RND)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_CONFIG)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_SPRITE_SC)->SetVisible(true);
//	p_pUI->GetControl(IDC_ROOM_STATIC_SC_TEXT)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_STATIC_SC_NAME)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_STATIC_ITEM_REST_COST)->SetVisible(true);

	// 選択アイテムの位置,データをリセット
//	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
//	{
//		p_pUI->GetControl(IDC_ROOM_BTN_MY_ITEM_BASE+i)->SetLocation(
//			ROOM_BTN_MY_ITEM_BASE_LEFT+(i*(ROOM_BTN_MY_ITEM_BASE_WIDTH+ROOM_BTN_MY_ITEM_BASE_OFFSET)),
//			ROOM_BTN_MY_ITEM_BASE_TOP);
//	}
	m_pMyItemBtnList->SetLocation(ROOM_BTN_MY_ITEM_LIST_X, ROOM_BTN_MY_ITEM_LIST_Y);
	m_pMyItemBtnList->SetDrawRange(ROOM_BTN_MY_ITEM_LIST_W);
	m_pMyItemBtnList->SetVisible(true);

	p_pUI->GetControl(IDC_ROOM_BTN_CONFIG)->SetLocation(ROOM_BTN_CONFIG_CNT_RECT_X,ROOM_BTN_CONFIG_CNT_RECT_Y);
	
	m_pTeamRulePropertyManager->SetVisible(true);
	m_pTeamSeparater->SetVisible(true);					// 20100929③
	m_pTeamSeparater->Update(m_nTeamCount);		// 20100929③

	p_pUI->GetControl(IDC_STATIC_SPIN_LIMIT_TURN_TEXT)->SetVisible(true);
	m_pSpinTurnLimit001->SetVisible(true);
	m_pSpinTurnLimit010->SetVisible(true);
	m_pSpinTurnLimit100->SetVisible(true);

	p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_TEXT)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_VALUE)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_L)->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_R)->SetVisible(true);

	OnClickViewButton(IDC_ROOM_BTN_VIEW_CHARA);

	SafePrintf(m_pBGTexturePath, _MAX_PATH*2, L"data/img/room/bg.png");
	BYTE *pBuf = NULL;
	UINT	nBufSize = 0;
	while (!g_pFiler->GetFileMemory(m_pBGTexturePath, &pBuf, &nBufSize))
	{
		Sleep(1);
		AddMessageLog(L"GetFileMemory BGtexture error");
	}
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'&');
	SafeRelease(m_pBGTexture);

//	while (!PngLoader::CreateTextureFromFileInMemory(m_pDev, pBuf, nBufSize, NULL, NULL, &m_pBGTexture, D3DPOOL_DEFAULT) )
	while (!TextureLoader::LoadTextureFromFileInMemory(&m_pBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL, 0, 1))
	{
		Sleep(1);
		AddMessageLog(L"CreateTextureFromFileInMemory error");
	}
	SafeDeleteArray(pBuf);
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	CDXUTColorListBox *pColorListBox = (CDXUTColorListBox*)p_pUI->GetControl(IDC_ROOM_LB_CHATLOG);
	if (!pColorListBox) return FALSE;
	pColorListBox->GetScrollBar()->SetTrackPos(pColorListBox->GetSize());

	// BGM再生
	if (m_nBGMPlayingID == -1)
		PlayBGM(BGM_ROOM);

//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > Room
// ========================================================================= //
BOOL CGame::Room()
{
	BOOL ret = FALSE;
//===========================可変============================//
	WORD	packetSize = 0;
	BYTE		packetData[MAX_PACKET_SIZE];

	// チャット間隔
	if (m_nChatSendInterval)
		m_nChatSendInterval--;

	// フォーカス確認
	if (m_bActive
	&& !m_SessionArray[m_nUserIndex].game_ready
	&& !p_pUI->GetControl(IDC_SHARE_EDIT)->m_bHasFocus
	&& !p_pConfig->GetVisible())
	{
		// 方向キー変化
		if (	(m_tKeyState.lX != m_nSavedInputX )
			||	(m_tKeyState.lY != m_nSavedInputY ) 
			)
		{
			short x = 0;	short y = 0;
			// →方向
			if (m_tKeyState.lX > 0)
				x = 1;
			else if (m_tKeyState.lX < 0)
				x = -1;
			// ↑方向
			if (m_tKeyState.lY > 0)			y = 1;
			else if (m_tKeyState.lY < 0)	y = -1;

			// パケット作成
			packetSize = PacketMaker::MakePacketData_RoomInfoMove(x,y, packetData);
			if (packetSize)
				AddPacket(packetData, packetSize);
		}
		// 入力更新
		m_nSavedInputX = m_tKeyState.lX;
		m_nSavedInputY = m_tKeyState.lY;
	}	// フォーカス無しの場合、移動をしない
	else if ( m_SessionArray[m_nUserIndex].vx != 0
			|| m_SessionArray[m_nUserIndex].vy != 0)
	{
			// パケット作成
			packetSize = PacketMaker::MakePacketData_RoomInfoMove(0,0, packetData);
			if (packetSize)
				AddPacket(packetData, packetSize);
		// 入力更新
		m_nSavedInputX = 0;
		m_nSavedInputY = 0;
	}

	// 各キャラ処理
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_pRoomCharacters[i] && m_pRoomCharacters[i]->IsCreated())
			m_pRoomCharacters[i]->Frame();
	}
//===========================================================//
	g_pCriticalSection->EnterCriticalSection_Sound(L'^');
	m_pSoundPlayer->Frame();
	g_pCriticalSection->LeaveCriticalSection_Sound();
	return ret;
}

// ========================================================================= //
// CGame::Run > RoomRelease
// ========================================================================= //
BOOL CGame::RoomRelease()
{
//===========================可変============================//
	// UI非表示
	p_pUI->GetControl(IDC_SHARE_EDIT)->SetVisible(false);
	SaveIMEText();
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
	p_pUI->GetControl(IDC_ROOM_BTN_CONFIG)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_ITEM_REST_COST)->SetVisible(false);
	m_pItemBtnList->SetVisible(false);
	m_pCharacterBtnList->SetVisible(false);

	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->SetVisible(false);

	p_pUI->GetControl(IDC_ROOM_BTN_DISC)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_CMB_CHAT)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_LB_USERLIST)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_READY)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_MY_TEAMNO)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_MY_NAME)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_STAGE_TITLE)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_STAGE_L)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_STAGE_R)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_STAGE_RND)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_CHARA)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_ITEM)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_VIEW_RULE)->SetVisible(false);

//	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
//		p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetVisible(false);
	m_pMyItemBtnList->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_CONFIG)->SetVisible(false);

	p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_TEXT)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_ACT_TIME_LIMIT_VALUE)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_L)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_BTN_ACT_TIME_LIMIT_R)->SetVisible(false);

	p_pUI->GetControl(IDC_STATIC_SPIN_LIMIT_TURN_TEXT)->SetVisible(false);
	m_pSpinTurnLimit001->SetVisible(false);
	m_pSpinTurnLimit010->SetVisible(false);
	m_pSpinTurnLimit100->SetVisible(false);

	p_pUI->GetControl(IDC_ROOM_SPRITE_SC)->SetVisible(false);
//	p_pUI->GetControl(IDC_ROOM_STATIC_SC_TEXT)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_STATIC_SC_NAME)->SetVisible(false);

	m_pTeamRulePropertyManager->SetVisible(false);
	m_pTeamSeparater->SetVisible(false);

	p_pUI->GetButton(IDC_ROOM_BTN_STAGE_BASE)->SetVisible(false);
	// 各キャラ非表示
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_pRoomCharacters[i] && m_pRoomCharacters[i]->IsCreated())
			m_pRoomCharacters[i]->SetVisible(false);
	}

	g_pCriticalSection->EnterCriticalSection_StageTexture(L'\'');
	SafeRelease(m_pBGTexture);
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	// 退室ボタンが押されたらタイトルへ戻る
	if (g_bCloseSocket)
		return FALSE;

	// ロビーBGM停止
	StopBGM(FALSE, FRAMES);
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > LoadInit
// ========================================================================= //
BOOL CGame::LoadInit()
{
//===========================可変============================//
	m_nLoadingTextureCounter = 0;

	m_pTeamSeparater->SetVisible(false);
	p_pUI->GetControl(IDC_LOAD_SPRITE_TEXT)->SetVisible(true);
	p_pUI->Refresh();
	p_pUI->SetVisible(true);

	g_pCriticalSection->EnterCriticalSection_StageTexture(L'(');
	SafeRelease(m_pBGTexture);
	SafeRelease(m_pStageTexture);
	SafeRelease(m_pStageBGTexture);

	if (!LoadLoadingImage())
	{
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		g_bCloseSocket = TRUE;
		return FALSE;
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	// スクリプトのサウンドクリア
	ClearScrSound();

	m_hThreadLoading = NULL;
	m_bReqLoadComplete = FALSE;
	m_bLoadComplete = FALSE;
	m_bMainInitFlg = FALSE;

//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > Load
// ========================================================================= //
BOOL CGame::Load()
{
	BOOL ret = FALSE;
//===========================可変============================//
	DWORD dwThreadExitCode = 0;

	if (m_bLoadComplete)
	{
		g_pCriticalSection->EnterCriticalSection_StageTexture(L')');		// ロストした時に待つ用
		if (m_bDeviceLost)
		{
			g_pCriticalSection->LeaveCriticalSection_StageTexture();
			return ret;
		}

		if (m_bReqLoadComplete)
		{
			m_bLoadComplete = TRUE;
			WORD			packetSize = 0;
			BYTE				packetData[MAX_PACKET_SIZE];
			packetSize = PacketMaker::MakePacketData_LoadComplete(packetData);
			if (packetSize)
				AddPacket(packetData, packetSize);
			AddMessageLog(L"Send:LoadComplete");
			m_bReqLoadComplete = FALSE;
		}
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
	}
	else
	{
		// スレッド開始済み確認
		if (!m_hThreadLoading)
		{
			// ロードスレッド開始
			// ステージロード
			SafeRelease(m_pStageBGTexture);
			SafeRelease(m_pStageTexture);
			
			TSTAGE_SCR_INFO* pStageScrInfo = m_pSelectedStageScrInfo;
			if (!pStageScrInfo) return FALSE;
			m_tThreadParam.pStageScrInfo = pStageScrInfo;
			m_tThreadParam.pMainStage = m_pMainStage;
			m_tThreadParam.ppStageTexture = &m_pStageTexture;
			m_tThreadParam.ppStageBGTexture = &m_pStageBGTexture;
			m_tThreadParam.pVecCharacters = &m_vecCharacters;
			m_tThreadParam.pSoundLibs = m_pScrSoundLibs;
			m_tThreadParam.pSoundIDHash = &m_mapScrSoundIDHash;
			m_tThreadParam.pBgmSoundID = &m_nBGMSoundID;

			DWORD dwThreadId = 1;
			AddMessageLog(L"CreateThread");
			g_bKillLoadingThread = FALSE;
			if (!m_hThreadLoading)
				m_hThreadLoading = CreateThread(NULL,0,Thread_Loading, &m_tThreadParam,0,&dwThreadId);
			// スレッド開始確認
			if (m_hThreadLoading)
			{
				for (;;)
				{
					if (GetExitCodeThread(m_hThreadLoading, &dwThreadExitCode))
					{
						if (dwThreadExitCode == STILL_ACTIVE)
							break;
						else
							Sleep(1);
					}
				}
			}
		}
		else
		{
			// ロードスレッドの終了確認
			if (GetExitCodeThread(m_hThreadLoading, &dwThreadExitCode))
			{
				if (dwThreadExitCode != STILL_ACTIVE)
				{
#if 0	// ロストテスト
//					m_bLoadComplete = TRUE;
#else
					m_bLoadComplete = TRUE;	///test時はコメントアウト
#endif
					if (m_hThreadLoading != NULL)
					{
						CloseHandle(m_hThreadLoading);
						m_hThreadLoading = NULL;
					}
					AddMessageLog(L"Exit:LoadingThread");
					/// test
#if 0 // ロストテスト
					return ret;
#endif
				}
			}
		}
	}

//===========================================================//
	g_pCriticalSection->EnterCriticalSection_Sound(L'\\');
	m_pSoundPlayer->Frame();
	g_pCriticalSection->LeaveCriticalSection_Sound();
	return ret;
}

// ========================================================================= //
// CGame::Run > LoadRelease
// ========================================================================= //
BOOL CGame::LoadRelease()
{
//===========================可変============================//
	AddMessageLog(L"<LoadRelease>");
	p_pUI->GetControl(IDC_LOAD_SPRITE_TEXT)->SetVisible(false);
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'=');
	SafeRelease(m_pBGTexture);
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > MainInit
// ========================================================================= //
BOOL CGame::MainInit()
{
//===========================可変============================//
	AddMessageLog(L"<MainInit>");
	m_bContYOnDown = FALSE;
	m_nSavedInputX = 0;

	m_nActiveCharaFocusCounter = 0;
	m_vecActiveCharaFocusStartPos = D3DXVECTOR3(0,0,0);
	ptype_session self = GetMySessionInfo();
	if (self)
	{
		if (!(self->obj_state & OBJ_STATE_MAIN_GALLERY_FLG))
		{
			m_nMyCharaFocusCounter = CHARA_FOCUS_TIME;
			m_vecMyCharaFocusStartPos = m_vecStageViewTrans;
		}
	}
	else
	{
		m_nMyCharaFocusCounter = 0;
		m_vecMyCharaFocusStartPos = D3DXVECTOR3(0,0,0);
	}

	// 表示
	CDXUTIMEEditBox* pIMEEdit = (CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT);
	pIMEEdit->SetLocation(MAIN_CHAT_EDIT_X, MAIN_CHAT_EDIT_Y);
	pIMEEdit->SetSize(MAIN_CHAT_EDIT_W, MAIN_CHAT_EDIT_H);
	pIMEEdit->SetVisible(true);
	
	p_pUI->GetControl(IDC_MAIN_CONTROLPANEL)->SetVisible(true);
	m_pShotMeter->SetVisible(true);
	m_pMeterMovable->SetVisible(true);
	m_pMeterWindL->SetVisible(true);
	m_pMeterWindR->SetVisible(true);
	p_pUI->GetControl(IDC_ROOM_CMB_CHAT)->SetLocation(MAIN_CHAT_CMB_X, MAIN_CHAT_CMB_Y);
	p_pUI->GetControl(IDC_ROOM_CMB_CHAT)->SetVisible(true);

	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetLocation(MAIN_CHATLOG_LB_CNT_X, MAIN_CHATLOG_LB_CNT_Y);
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetSize(MAIN_CHATLOG_LB_CNT_W, MAIN_CHATLOG_LB_CNT_H);

	p_pUI->GetControl(IDC_MAIN_SPRITE_FACE)->SetVisible(true);
	p_pUI->GetControl(IDC_MAIN_METER_HP)->SetVisible(true);
	p_pUI->GetControl(IDC_MAIN_STATIC_DELAY)->SetVisible(true);
	p_pUI->GetControl(IDC_MAIN_STATIC_TURN)->SetVisible(true);
	CDXUTStatic* pStatic = p_pUI->GetStatic(IDC_MAIN_STATIC_NAME);
	pStatic->SetLocation(MAIN_STATIC_NAME_CNT_X,MAIN_STATIC_NAME_CNT_Y);
	pStatic->SetSize(MAIN_STATIC_NAME_CNT_W,MAIN_STATIC_NAME_CNT_H);
	pStatic->GetElement(0)->SetFont(5, 0xFFFFFFFF, DT_LEFT|DT_TOP|DT_SINGLELINE);
	pStatic->SetDrawShadow(true);
	pStatic->SetVisible(true);

	m_pShotAngle->SetRangeAngleMin( ((TCHARA_SCR_INFO*)GetMySessionInfo()->scrinfo)->angle_range_min );
	m_pShotAngle->SetRangeAngleMax( ((TCHARA_SCR_INFO*)GetMySessionInfo()->scrinfo)->angle_range_max );
	m_pShotAngle->SetVisible(true);
	m_pShotAngleIndicator->SetVisible(true);

	// スペルの上限をメーターに設定
	m_pMeterSCExp->SetMaxValue( ((TCHARA_SCR_INFO*)GetMySessionInfo()->scrinfo)->sc_info.max_exp );

	CDXUTScrollBar *pScrollBar = p_pUI->GetColorListBox(IDC_ROOM_LB_CHATLOG)->GetScrollBar();
	p_pUI->GetColorListBox(IDC_ROOM_LB_CHATLOG)->SetDrawTextShadow(true);
	pScrollBar->SetVisible(false);
	pScrollBar->Refresh();
	pScrollBar->SetTrackPos(p_pUI->GetColorListBox(IDC_ROOM_LB_CHATLOG)->GetSize());
	
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetVisible(true);
	p_pUI->GetColorListBox(IDC_ROOM_LB_CHATLOG)->GetScrollBar()->SetVisible(false);
	
	p_pUI->GetColorListBox(IDC_ROOM_LB_USERLIST)->GetScrollBar()->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_LB_USERLIST)->SetVisible(false);

	p_pUI->GetControl(IDC_MAIN_ASPRITE_TIMER)->SetVisible(true);
	p_pUI->GetControl(IDC_MAIN_ASPRITE_TIMER_BG)->SetVisible(true);

	// 選択アイテム位置設定
	/*
	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
	{
		p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetLocation(
			MAIN_BTN_MY_ITEM_BASE_LEFT+(i*(MAIN_BTN_MY_ITEM_BASE_WIDTH+MAIN_BTN_MY_ITEM_BASE_OFFSET)),
			MAIN_BTN_MY_ITEM_BASE_TOP);
		if (p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->GetUserData())
		{
			p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetEnabled(true);
			p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetVisible(true);
		}
	}
	*/
	m_pMyItemBtnList->SetLocation(MAIN_BTN_MY_ITEM_BASE_LEFT, MAIN_BTN_MY_ITEM_BASE_TOP);
	m_pMyItemBtnList->SetDrawRange(MAIN_BTN_MY_ITEM_LIST_W);

	SetMyIteamsEnabled(m_bytRuleFlg & GAME_RULE_ITEM_ENABLE);
	m_pMyItemBtnList->SetVisible(true);

	p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetLocation(MAIN_BTN_CONFIG_CNT_RECT_X,MAIN_BTN_CONFIG_CNT_RECT_Y);
	p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetVisible(true);

	// ショットパワーの線
	m_pShotPowerIndicator->SetVisible(true);

	// フォーカス弾をクリア
	m_pFocusObject = NULL;

	// 自ターンパス有効表示テクスチャ設定
	SetRect(&p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_DISABLE_RECT);
	p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)FALSE);
	p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetVisible(true);

	if (m_nLimitTurn)
		p_pUI->GetStatic(IDC_MAIN_STATIC_TURN)->GetElement(0)->iFont = MAIN_STATIC_TURN_CNT_FONT_LIMIT;
	else
		p_pUI->GetStatic(IDC_MAIN_STATIC_TURN)->GetElement(0)->iFont = MAIN_STATIC_TURN_CNT_FONT;

	m_nTurnCounter = 0;
	// 前ターン風向き表示
	p_pUI->GetControl(IDC_MAIN_SPRITE_PREV_WIND_INDICATOR)->SetVisible(true);

	// マウス入力初期化
	m_vecStageViewTrans = D3DXVECTOR3(0,0,0);
	m_nSavedInputX = 0;
	m_nSavedInputY = 0;
	m_nSavedMouseX = 0;
	m_nSavedMouseY = 0;
	m_eMainPhase = GAME_MAIN_PHASE_NONE;

	m_pScrProcObject = NULL;
	m_nSelectBulletNo = 0;

	ClearStageObjects();
	SetVisibleBulletSelectButtons(true);

	// チャット間隔
	m_nChatSendInterval = 0;

	// ステージテクスチャ
	m_pSelectedStageScrInfo->pTexture = m_pStageTexture;

	m_bMainInitFlg = TRUE;
	
	// 発射角度を初期化
	m_pShotAngle->SetAngle((m_pShotAngle->GetRangeAngleMax()-m_pShotAngle->GetRangeAngleMin())/2+m_pShotAngle->GetRangeAngleMin());
	SetMyBulletsEnabled(true);
	// ステージBGM再生
	if (m_nBGMSoundID != -1 && m_bytBGMVolume > 0)
	{
		CSoundBuffer* pSoundBuffer = m_pScrSoundLibs->GetDuplicatedFromID(m_nBGMSoundID);
		if (pSoundBuffer)
			m_nBGMPlayingID = m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytBGMVolume, SOUNDLIB_ENDLESSLOOP);
		if (m_nBGMPlayingID == -1)
			AddMessageLog(L"ステージBGM再生失敗");
	}
//	PlayBGM(m_pSelectedStageScrInfo->bgm);

//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > Main
// ========================================================================= //
BOOL CGame::Main()
{
	BOOL ret = FALSE;
//===========================可変============================//
	// チャット間隔
	if (m_nChatSendInterval)
		m_nChatSendInterval--;

	if ( !p_pConfig->GetVisible() )
	{
		// Y方向キー変化
		if ( !p_pUI->GetControl(IDC_SHARE_EDIT)->m_bHasFocus
		&& ((m_tKeyState.lY != m_nSavedInputY)
		&& (!m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_NOANGLE_INDEX])
		&& (m_SessionArray[m_nUserIndex].obj_state & (OBJ_STATE_MAIN_WAIT_FLG|OBJ_STATE_MAIN_ACTIVE_FLG|OBJ_STATE_MAIN_SHOOTING_FLG|OBJ_STATE_MAIN_SHOTPOWER_FLG))
		))
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				if (!m_bContYOnDown)
				{
					m_bContYOnDown = TRUE;
					m_pShotAngle->SetAngle(((m_tKeyState.lY>0)?-1:1)+m_pShotAngle->GetAngle());
				}
			}
			else
				m_pShotAngle->SetAngle(((m_tKeyState.lY>0)?-1:1)+m_pShotAngle->GetAngle());
#if TEST_CAPTION
	WCHAR caption[65];
	SafePrintf(caption, 64, L"%d / %d / %d", m_nWindValue, m_pShotAngle->GetShotAngle(), m_pShotPowerIndicator->GetCX());
	SetWindowText(g_hWnd, caption);
#endif
		}

//		m_nSavedInputY = m_tKeyState.lY;
	}

	g_pCriticalSection->EnterCriticalSection_Session(L'^');
	if (m_eGameState != eGameMain)		// GAME_ENDパケットを受信したか
	{
		g_pCriticalSection->LeaveCriticalSection_Session();
		return TRUE;
	}
	if (m_SessionArray[m_nUserIndex].obj_state & OBJ_STATE_ACT_FLG)
		ret = RunMainActive();
	else
		ret = RunMainNoneActive();
	g_pCriticalSection->LeaveCriticalSection_Session();

	// 入力更新
	m_nSavedInputX = m_tKeyState.lX;

	// オブジェクトの処理
	FrameObjects();

	// 自ターン知らせるコントロール更新
	UpdateNotifyMyTurn();

	// エフェクト処理
	g_pCriticalSection->EnterCriticalSection_Effect(L'5');
	for ( std::map< int, CStageEffect* >::iterator it = m_mapStageEffects.begin();
		it != m_mapStageEffects.end();
		)
	{
		if (!(*it).second->Frame())	// 寿命の場合：FALSE
		{
			CStageEffect* pStageEffect = (*it).second;
			it = m_mapStageEffects.erase(it);
			DEBUG_DELETE(pStageEffect, L"Main()");
		}
		else
		{
			it++;
		}
	}
	// 背景エフェクト処理
	for ( std::map< int, CStageEffect* >::iterator it = m_mapBGEffects.begin();
		it != m_mapBGEffects.end();
		)
	{
		if (!(*it).second->Frame())	// 寿命の場合：FALSE
		{
			CStageEffect* pStageEffect = (*it).second;
			it = m_mapBGEffects.erase(it);
			DEBUG_DELETE(pStageEffect, L"Main()");
		}
		else
		{
			it++;
		}
	}
	g_pCriticalSection->LeaveCriticalSection_Effect();

	// 各キャラ処理
	FrameStageCharacter();

//===========================================================//
	g_pCriticalSection->EnterCriticalSection_Sound(L'!');
	m_pSoundPlayer->Frame();
	g_pCriticalSection->LeaveCriticalSection_Sound();
	return ret;
}

// ========================================================================= //
// CGame::Run > Main (Active)
// ========================================================================= //
BOOL CGame::RunMainActive()
{
	BOOL ret = FALSE;

	WORD	packetSize = 0;
	BYTE		packetData[MAX_PACKET_SIZE];
	switch (m_eMainPhase)
	{
	case GAME_MAIN_PHASE_ACT:
		// X方向キー変化
		if ( m_bActive	// ウィンドウがアクティブ
			&& !p_pUI->GetControl(IDC_SHARE_EDIT)->m_bHasFocus
			&& (m_tKeyState.lX != m_nSavedInputX)
			&& !p_pConfig->GetVisible()
//			&& GetMySessionInfo()->vy == 0
			)
		{
			char x = 0;
			if (GetAsyncKeyState(VK_CONTROL))
			{
				if (m_tKeyState.lX < 0)
					packetSize = PacketMaker::MakePacketData_MainInfoFlip(USER_DIRECTION_LEFT, packetData);
				else if (m_tKeyState.lX > 0)
					packetSize = PacketMaker::MakePacketData_MainInfoFlip(USER_DIRECTION_RIGHT, packetData);
			}
			else
			{
				// →方向
				if (m_tKeyState.lX > 0)
					packetSize = PacketMaker::MakePacketData_MainInfoMove(1, packetData);
				else if (m_tKeyState.lX < 0)
					packetSize = PacketMaker::MakePacketData_MainInfoMove(-1, packetData);
			}
			if (packetSize)
				AddPacket(packetData, packetSize);
		}// ショットパワーメータ開始ボタン押下
		else if ( m_bActive	// ウィンドウがアクティブ
			&& !p_pUI->GetControl(IDC_SHARE_EDIT)->m_bHasFocus
			&& m_tKeyState.bytButtons[0] == (KEY_BTN_CHANGE|KEY_BTN_ON)
			&& !(p_pConfig->GetVisible())
			&& (GetMySessionInfo()->vy == 0)
			)
		{
			// ショットパワーメータ初期化
			m_pShotMeter->SetValue(0);
			ptype_session sess = GetMySessionInfo();
			if (!sess)	break;

			// 範囲チェック
			BOOL bRejTrigger = FALSE;
			if (m_nTriggerType == MAX_CHARA_BULLET_TYPE && sess->EXP_c < ((TCHARA_SCR_INFO*)sess->scrinfo)->sc_info.max_exp)
				bRejTrigger = TRUE;
			else if (m_nTriggerType > ((TCHARA_SCR_INFO*)sess->scrinfo)->blt_sel_count && m_nTriggerType < 0)
				bRejTrigger = TRUE;

			if (bRejTrigger)
			{
				m_pBulletSelectButtons[0]->SetChecked(true);
				m_nTriggerType = 0;
				break;
			}

			// コンフィグ設定ボタン無効
			p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetEnabled(false);
			// 自ターンパス無効表示テクスチャ設定
			SetRect( &p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->GetElement(1)->rcTexture, MAIN_BTN_TURN_PASS_IMG_DISABLE_RECT);
			p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetUserData((void*)FALSE);
			// アイテム使用を無効化
///			SetMyIteamsEnabled(false);

			// ショットパワーメータ開始のパケット
			SetSelectBulletInfo();
			packetSize = PacketMaker::MakePacketData_MainInfoShotPowerStart( sess, m_nShootingProcType, m_nShootingBltType, m_nShootingAngle, packetData);
			if (packetSize)
				AddPacket(packetData, packetSize);
			// ショットパワーメータ開始
			m_eMainPhase = GAME_MAIN_PHASE_SHOTPOWER;
			sess->obj_state = OBJ_STATE_MAIN_SHOTPOWER;
			sess->frame_count = 0;

			// 弾選択を無効化
			SetMyBulletsEnabled(false);		// 20101122

			if (g_bDebug)
			{
				WCHAR logf[64];
				SafePrintf(logf, 64, L"SHOTPOWER_START(X:%d,Y:%d)", sess->ax, sess->ay);
				AddMessageLog(logf);
			}
		}
		else if (m_nSavedInputX == 0
			&& (GetMySessionInfo()->vy == 0)
			&& (GetMySessionInfo()->vx != 0)
			)
		{
			// パケット作成
			packetSize = PacketMaker::MakePacketData_MainInfoMove(0, packetData);
			if (packetSize)
				AddPacket(packetData, packetSize);
		}
		// 残り時間カウンタ
		m_pASpriteTimer->StepTimeCount();
		m_pASpriteTimerBG->StepTimeCount();
		m_nActiveTimeCounter++;
		if (m_nActiveTimeCounter < (m_nActTimeLimit*RUN_FRAMES)
			&&	(m_nActiveTimeCounter >= ((m_nActTimeLimit*RUN_FRAMES) - GAME_TURN_ACT_TIMEUP_WARNING))
			&&	(!(m_nActiveTimeCounter % RUN_FRAMES))
			)
		{
			g_pGame->PlaySysSoundSE(SE_sai_Count);
		}
		break;
	case GAME_MAIN_PHASE_SHOTPOWER:		// メーター増やす
		{
			BOOL bTrigger = FALSE;
#ifdef HOGE
			float fIX = (float)(m_pShotPowerIndicator->GetPosX()-126) / 320.0f * (float)m_pShotMeter->GetMaxValue();
			int nPX = m_pShotMeter->GetValue();
			if ((int)fIX <= m_pShotMeter->GetValue()-2)
			{
 				bTrigger = TRUE;
			}
			else
#endif
			if (m_pShotMeter->GetMaxValue() <= m_pShotMeter->GetValue())
				bTrigger = TRUE;
			else if ( m_bActive	// ウィンドウがアクティブ
				&& !p_pUI->GetControl(IDC_SHARE_EDIT)->m_bHasFocus
				&& (m_tKeyState.bytButtons[0] == (KEY_BTN_CHANGE|KEY_BTN_ON))
				&& (GetMySessionInfo()->vy == 0)
				)
				bTrigger = TRUE;

			if (bTrigger)
			{
				// パケット作成
				ptype_session sess = GetMySessionInfo();
				SetTrigger(m_nUserIndex, m_nShootingProcType, m_nShootingBltType, m_pShotAngle->GetShotAngle(), m_pShotMeter->GetValue(), m_pShotAngleIndicator->GetCurrentAngle(), m_pShotPowerIndicator->GetCXForView());

				packetSize = PacketMaker::MakePacketData_MainInfoTrigger( sess, m_nShootingProcType, m_nShootingBltType, m_nShootingAngle, m_nShootingPower, m_nShootingIndicatorAngle, m_nShootingIndicatorPower, packetData);
				if (packetSize)
					AddPacket(packetData, packetSize);
				// 20101105
				m_eMainPhase = GAME_MAIN_PHASE_TRIGGER;
				break;
			}

			m_pShotMeter->SetValue(m_pShotMeter->GetValue()+MAX_SHOT_COUNTUP);
			break;
		}
	case GAME_MAIN_PHASE_TRIGGER:

		break;
	default:
		break;
	}
	return ret;
}

// ========================================================================= //
// CGame::Run > Main (NoneActive)
// ========================================================================= //
BOOL CGame::RunMainNoneActive()
{
	BOOL ret = FALSE;
	WORD	packetSize = 0;
	BYTE		packetData[MAX_PACKET_SIZE];

	if ( m_SessionArray[m_nUserIndex].vx != 0)
	{
		m_SessionArray[m_nUserIndex].vx = 0;
		// パケット作成
		packetSize = PacketMaker::MakePacketData_MainInfoMove(0, packetData);
		if (packetSize)
			AddPacket(packetData, packetSize);
	}
	return ret;
}

// ========================================================================= //
// CGame::Run > MainRelease
// ========================================================================= //
BOOL CGame::MainRelease()
{
//===========================可変============================//
	AddMessageLog(L"<MainRelease>");

	g_pCriticalSection->EnterCriticalSection_Object(L'&');
	ClearStageObjects();
	g_pCriticalSection->LeaveCriticalSection_Object();
	g_pCriticalSection->EnterCriticalSection_Effect(L'6');
	ClearStageEffects();
	ClearBGEffects();
	g_pCriticalSection->LeaveCriticalSection_Effect();
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'~');
	m_pSelectedStageScrInfo->pTexture = NULL;
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_pStageCharacters[i]->IsCreated())
			m_pStageCharacters[i]->SetVisible(false);
	}

	p_pUI->GetControl(IDC_SHARE_EDIT)->SetVisible(false);
	SaveIMEText();

	m_pShotMeter->SetVisible(false);
	m_pMeterMovable->SetVisible(false);
	m_pMeterWindL->SetVisible(false);
	m_pMeterWindR->SetVisible(false);
	m_pShotAngle->SetVisible(false);
	m_pShotAngleIndicator->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_CMB_CHAT)->SetVisible(false);
	p_pUI->GetControl(IDC_ROOM_LB_CHATLOG)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_ASPRITE_TIMER)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_ASPRITE_TIMER_BG)->SetVisible(false);

	// 選択アイテム位置設定
	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
		p_pUI->GetControl(IDC_MAIN_BTN_MY_ITEM_BASE+i)->SetVisible(false);

	p_pUI->GetControl(IDC_MAIN_CONTROLPANEL)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_STATIC_DELAY)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_STATIC_TURN)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_METER_HP)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_SPRITE_FACE)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_BTN_CONFIG)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_BTN_TURN_PASS)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_RB_SEL_SPELL)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_METER_SPELL_EXP)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_SPRITE_CHATLOG_BG)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_SPRITE_PREV_WIND_INDICATOR)->SetVisible(false);

	SetVisibleBulletSelectButtons(false);

	// ショットパワーの線
	m_pShotPowerIndicator->SetVisible(false);

	// フォーカス弾をクリア20100929⑤
	m_pFocusObject = NULL;
	p_pUI->GetControl(IDC_MAIN_STATIC_NOTIFY_TURN)->SetVisible(false);
	p_pUI->GetControl(IDC_MAIN_STATIC_NOTIFY_TURN_TEXT)->SetVisible(false);

	// 自ターン知らせるコントロール非表示
	m_nNotifyMyTurn = 0;

	// マウス入力初期化
	m_vecStageViewTrans = D3DXVECTOR3(0,0,0);
	m_nSavedInputX = 0;
	m_nSavedInputY = 0;
	m_nSavedMouseX = 0;
	m_nSavedMouseY = 0;
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	p_pUI->GetManager()->m_pSprite->SetTransform(&mat);
	g_pSprite->SetTransform(&mat);

	if (g_bCloseSocket)
		return FALSE;

	// スクリプト用音声破棄
//	m_pSoundPlayer->ClearPlayingList();
	ClearScrSound();

	// メインBGM停止
	StopBGM(TRUE);
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > ResultInit
// ========================================================================= //
BOOL CGame::ResultInit()
{
//===========================可変============================//

	p_pUI->GetControl(IDC_RESULT_BTN_CONFIRM)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_SB_RANK)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_BTN_DISC)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_SPRITE_BG_SCREEN)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_SPRITE_RANK_TITLE)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_SPRITE_TEXT_BG)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_SPRITE_FACE_BG)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_SPRITE_FACE)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_STATIC_TEXT)->SetVisible(true);
	p_pUI->GetControl(IDC_RESULT_STATIC_NAME)->SetLocation(RESULT_STATIC_NAME_CNT_X, RESULT_STATIC_NAME_CNT_Y);

	for (int i=0;i<RESULT_RANK_VIEW_RANGE-1;i++)
		p_pUI->GetControl(IDC_RESULT_SPRITE_LINE_BASE+i)->SetVisible(true);

	m_vecStageViewTrans = D3DXVECTOR3( (float)((m_pMainStage->GetStageWidth()/2)-(WIN_WIDTH/2)), (float)((m_pMainStage->GetStageHeight()/2)-(WIN_HEIGHT/2)),0 );
	CalcViewTransCaps(FALSE);
	UpdateStageDrawRect(FALSE);

	g_pCriticalSection->EnterCriticalSection_Session(L'\\');
	// ランキング作成
	CreateResultRank();
	g_pCriticalSection->LeaveCriticalSection_Session();
	
	m_nResultReturnCounter = 0;
	// BGM再生
	PlayBGM(BGM_ROOM, FRAMES*2);
//===========================================================//
	return TRUE;
}

// ========================================================================= //
// CGame::Run > Result
// ========================================================================= //
BOOL CGame::Result()
{
	BOOL ret = FALSE;
//===========================可変============================//
	/*
	m_nResultReturnCounter++;
	if (m_nResultReturnCounter == FRAMES*60)
	{
		WORD	packetSize = 0;
		BYTE	packetData[MAX_PACKET_SIZE];
		packetSize = PacketMaker::MakePacketData_Confirm(m_nUserIndex, packetData);
		AddPacket(packetData, packetSize);
	}
	*/
//===========================================================//
	g_pCriticalSection->EnterCriticalSection_Sound(L'\"');
	m_pSoundPlayer->Frame();
	g_pCriticalSection->LeaveCriticalSection_Sound();
	return ret;
}

// ========================================================================= //
// CGame::Run > ResultRelease
// ========================================================================= //
BOOL CGame::ResultRelease()
{
//===========================可変============================//
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'|');
	SafeRelease(m_pStageBGTexture);
	SafeRelease(m_pStageTexture);
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	p_pUI->GetControl(IDC_RESULT_BTN_CONFIRM)->SetVisible(false);
	m_vecCharacters.clear();

	for (int i=0;i<m_nRankItemCount;i++)
	{
		CDXUTControl* pControl = p_pUI->GetControl(IDC_RESULT_SPRITE_NO_BASE+i);
		if (pControl)
		{
			p_pUI->RemoveControl(IDC_RESULT_SPRITE_NO_BASE+i);
			p_pUI->RemoveControl(IDC_RESULT_SPRITE_ICON_BASE+i);
			p_pUI->RemoveControl(IDC_RESULT_STATIC_NAME_BASE+i);
		}
		pControl = p_pUI->GetControl(IDC_RESULT_STATIC_TEAM_BASE+i);
		if (pControl)
			p_pUI->RemoveControl(IDC_RESULT_STATIC_TEAM_BASE+i);
	}
	p_pUI->GetControl(IDC_RESULT_BTN_DISC)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_SB_RANK)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_BTN_CONFIRM)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_SPRITE_BG_SCREEN)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_SPRITE_RANK_TITLE)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_SPRITE_TEXT_BG)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_SPRITE_FACE_BG)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_SPRITE_FACE)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_STATIC_TEXT)->SetVisible(false);
	p_pUI->GetControl(IDC_RESULT_STATIC_NAME)->SetVisible(false);
	for (int i=0;i<RESULT_RANK_VIEW_RANGE-1;i++)
		p_pUI->GetControl(IDC_RESULT_SPRITE_LINE_BASE+i)->SetVisible(false);
//===========================================================//

	return TRUE;
}
