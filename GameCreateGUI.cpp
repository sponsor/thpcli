#include "ext.h"
#include "Game.h"

//> GUIコントロールの作成
BOOL CGame::CreateGUI()
{
	if (!p_pUI)	return FALSE;
	RECT rcTexture;
	CDXUTElement* pElement = NULL;
	CDXUTButton *pBtn = NULL;
	CDXUTSprite* pSprite = NULL;
	CDXUTStatic* pStatic = NULL;
	CDXUTComboBox *pCombo = NULL;
	CDXUTColorListBox *pColorListBox = NULL;
	CDXUTCheckBox* pChkBox = NULL;
	CDXUTRadioButton* pRadioButton = NULL;
	CDXUTScrollBar* pScrollBar = NULL;
	CDXUTMeter*	pMeter = NULL;
	
	// アイテム詳細表示サイズ作成
	CreateItemDetailControlSize();

	// リソースとスクリプトのインデックス情報クリア
	m_mapCharaScrInfo.clear();

	// デフォルトコントロール画像
	m_nDefaultGUIResourceIndex = AddResourceTexture(IMG_GUI_SKIN);
	if (m_nDefaultGUIResourceIndex == -1)
		return FALSE;
	DXUTTextureNode* pTextureNode = p_pUI->GetTexture(m_nDefaultGUIResourceIndex);
	m_pDefaultGUIResourceTexture = pTextureNode->pTexture;

	// ステージ画像
	if (!common::scr::LoadAllStageScript(g_L, g_pLuah, &m_mapStageScrInfo))
	{
		MessageBox(g_hWnd, L"ステージスクリプトロードエラー", L"script error", MB_OK);
		return FALSE;
	}

	// リソースマネージャとダイアログにキャラ用のGUIパーツ追加
	if (!common::scr::LoadAllCharaScript(p_pL, p_pLuah, &m_mapCharaScrInfo))
	{
		MessageBox(g_hWnd, L"キャラクタースクリプトロードエラー", L"script error", MB_OK);
		return FALSE;
	}

	// メイン コントロールパネル
	p_pUI->AddSprite( IDC_MAIN_CONTROLPANEL,
		MAIN_CONTROLPANEL_CNT_RECT,
		false, &pSprite );
	pSprite->GetElement(0)->TextureColor.Init( 0xFFFFFFFF );
	SetRect( &rcTexture, MAIN_CONTROLPANEL_IMG_RECT);											// 描画範囲
	pSprite->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pSprite->SetVisible(false);

	// Room //////////////////////////////////////////////////////////////////////
	// チャットログ用背景
	p_pUI->AddSprite( IDC_MAIN_SPRITE_CHATLOG_BG,
		MAIN_SPRITE_CHATLOG_BG_CNT_RECT,
		false, &pSprite );
	SetRect( &rcTexture, MAIN_SPRITE_CHATLOG_BG_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// チャットログ //
    // List box
	p_pUI->AddColorListBox( IDC_ROOM_LB_CHATLOG,
		ROOM_CHATLOG_LB_CNT_RECT,
		0, &pColorListBox );

	pColorListBox->SetChangeSelectColor(false);
	pColorListBox->SetBorder(2,6);
	pColorListBox->m_bAcceptKeyboard = false;

	// 背景枠
	pElement = pColorListBox->GetElement(1);
    pElement->SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	// リストアイテム１
	pElement = pColorListBox->GetElement(2);
    pElement->SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	// リストアイテム２
	pElement = pColorListBox->GetElement(3);
    pElement->SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 0 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	// リストアイテム３
	CDXUTElement* pNewElement = new CDXUTElement(*pElement);
	pNewElement->SetFont(0, D3DCOLOR_ARGB( 255, 0, 255, 0 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	pColorListBox->SetElement(4, pNewElement);
	SafeDelete(pNewElement);
	// リストアイテム４
	pNewElement = new CDXUTElement(*pElement);
	pNewElement->SetFont(0, D3DCOLOR_ARGB( 255, 255, 0, 0 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	pColorListBox->SetElement(5, pNewElement);
	SafeDelete(pNewElement);
	pColorListBox->SetVisible(false);
	// リストアイテム５
	pNewElement = new CDXUTElement(*pElement);
	pNewElement->SetFont(0, D3DCOLOR_ARGB( 255, 0, 255, 255 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	pColorListBox->SetElement(6, pNewElement);
	SafeDelete(pNewElement);
	pColorListBox->SetVisible(false);

	pScrollBar = pColorListBox->GetScrollBar();
	SetRect( &rcTexture, ROOM_CHATLOG_SB_IMG_U_RECT);	// 描画範囲
	pScrollBar->GetElement(1)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, ROOM_CHATLOG_SB_IMG_D_RECT);	// 描画範囲
	pScrollBar->GetElement(2)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, BTN_VBAR_RECT);	// 描画範囲
	pScrollBar->GetElement(3)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	// チャットバー //
	CDXUTIMEEditBox* pIMEEdit = (CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT);
	pElement = pIMEEdit->GetElement(9);
	SetRect(&rcTexture, ROOM_CHAT_EDIT_IMG_9_RECT);
//	pElement->SetFont(2,0xFFFFFFFF,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
	pElement->SetTexture(m_nDefaultGUIResourceIndex,&rcTexture,0xFFFFFFFF);
	// ComboBox
	p_pUI->AddComboBox( IDC_ROOM_CMB_CHAT,
		ROOM_CHAT_CMB_CNT_RECT,
		0, false, &pCombo );
	// Text
	pCombo->AddItem( L"ALL", (LPVOID)PK_USER_CHAT_ALL );
	pCombo->AddItem( L"TEAM", (LPVOID)PK_USER_CHAT_TEAM );
	pElement = pCombo->GetElement(0);
	pElement->TextureColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pElement->TextureColor.States[ DXUT_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pElement->TextureColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);
	pElement->FontColor.States[ DXUT_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 0, 0, 0);
	pElement->FontColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 0, 0, 0);
	pElement->FontColor.States[ DXUT_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 128, 128, 128);
	SetRect( &rcTexture, ROOM_CHAT_CMB_IMG_0_RECT);	// 描画範囲
	pElement->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	// Button
	pElement = pCombo->GetElement(1);
	SetRect( &rcTexture, ROOM_CHAT_CMB_IMG_1_RECT );
	pElement->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	// cmb scrollbar
	pScrollBar = pCombo->GetScrollBar();
	SetRect( &rcTexture, ROOM_CHATLOG_SB_IMG_U_RECT);	// 描画範囲
	pScrollBar->GetElement(1)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, ROOM_CHATLOG_SB_IMG_D_RECT);	// 描画範囲
	pScrollBar->GetElement(2)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, BTN_VBAR_RECT);	// 描画範囲
	pScrollBar->GetElement(3)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pCombo->SetVisible(false);

	// 退室ボタン
	p_pUI->AddButton( IDC_ROOM_BTN_DISC, L"EXIT",
		ROOM_EXIT_BTN_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);
	
	SetRect( &rcTexture, ROOM_EXIT_BTN_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetVisible(false);

	// 準備OKボタン
	p_pUI->AddButton( IDC_ROOM_BTN_READY, L"",
		ROOM_BTN_READY_CNT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, ROOM_BTN_READY_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetVisible(false);

	// config
	p_pUI->AddButton( IDC_ROOM_BTN_CONFIG, L"config",
		ROOM_BTN_CONFIG_CNT_RECT_X,ROOM_BTN_CONFIG_CNT_RECT_Y,ROOM_BTN_CONFIG_CNT_RECT_W,ROOM_BTN_CONFIG_CNT_RECT_H,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, ROOM_BTN_CONFIG_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetVisible(false);

	// キャラ、アイテム、ルール画面表示ボタン //
	// キャラ
	p_pUI->AddButton( IDC_ROOM_BTN_VIEW_CHARA, L"キャラ",
		ROOM_BTN_VIEW_CHARA_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);
	
	SetRect( &rcTexture, IDC_ROOM_BTN_VIEW_CHARA_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,0);
	pBtn->SetVisible(false);
	// アイテム
	p_pUI->AddButton( IDC_ROOM_BTN_VIEW_ITEM, L"アイテム",
		ROOM_BTN_VIEW_ITEM_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);
	
	SetRect( &rcTexture, IDC_ROOM_BTN_VIEW_ITEM_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,0);
	pBtn->SetVisible(false);
	// ルール
	p_pUI->AddButton( IDC_ROOM_BTN_VIEW_RULE, L"ルール",
		ROOM_BTN_VIEW_RULE_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);
	
	SetRect( &rcTexture, IDC_ROOM_BTN_VIEW_RULE_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,0);
	pBtn->SetVisible(false);

	// キャラ 名前
	p_pUI->AddStatic( IDC_ROOM_STATIC_CHARA_NAME,L"",
		ROOM_STATIC_CHARA_NAME_CNT_RECT,
		false, &pStatic );
	pStatic->GetElement(0)->SetFont(IDC_ROOM_BTN_CHARA_SEL_FONT);
	pStatic->GetElement(0)->dwTextFormat = DT_CENTER;
	pStatic->SetVisible(false);

	// キャラ 速度、防御情報
	p_pUI->AddStatic( IDC_ROOM_STATIC_CHARA_INFO,L"",
		ROOM_STATIC_CHARA_INFO_CNT_RECT,
		false, &pStatic );
	pStatic->GetElement(0)->SetFont(7, 0xFFFFFFFF, DT_CENTER);
	pStatic->SetVisible(false);

	// キャラ 弾リスト
	p_pUI->AddColorListBox( IDC_ROOM_LIST_CHARA_BLT_INFO,
		ROOM_LIST_CHARA_BLT_INFO_CNT_RECT,
		0, &pColorListBox );
	pColorListBox->SetChangeSelectColor(false);
	pColorListBox->GetElement(0)->SetFont(IDC_ROOM_BTN_CHARA_SEL_FONT, 0xFFFFFFFF, DT_LEFT);
	pColorListBox->GetElement(1)->SetFont(IDC_ROOM_BTN_CHARA_SEL_FONT, 0xFFFFFFFF, DT_LEFT);

	pScrollBar = pColorListBox->GetScrollBar();
	SetRect( &rcTexture, ROOM_LIST_CHARA_BLT_SB_U_RECT);	// 描画範囲
	pScrollBar->GetElement(1)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, ROOM_LIST_CHARA_BLT_SB_D_RECT);	// 描画範囲
	pScrollBar->GetElement(2)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, BTN_VBAR_RECT);	// 描画範囲
	pScrollBar->GetElement(3)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);

	pElement = pColorListBox->GetElement(1);
    pElement->SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pElement->TextureColor.Init(0,0,0);
	pColorListBox->SetBorder(0, ROOM_LIST_CHARA_BLT_ICON_W+4);
	pColorListBox->SetDrawTextShadow(true);
	pColorListBox->SetVisible(false);

	p_pUI->AddSprite(IDC_ROOM_SPRITE_SC,
		ROOM_SPRITE_SC_CNT_RECT,
		false, &pSprite );
	pSprite->SetVisible(false);
	/*
	// SpellCard
	p_pUI->AddStatic( IDC_ROOM_STATIC_SC_TEXT,L"SpellCard",
		ROOM_STATIC_SC_TEXT_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(2, 0xFFFFFFFF, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
	pStatic->SetVisible(false);
	*/
	// sc名
	p_pUI->AddStatic( IDC_ROOM_STATIC_SC_NAME,L"",
		ROOM_STATIC_SC_NAME_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(1, 0xFFFFFFFF, DT_TOP|DT_LEFT|DT_SINGLELINE);
	pStatic->SetVisible(false);

	// アイテムリスト //
	SetItemIconButtonList();

	// アイテム情報
	p_pUI->AddStatic( IDC_ROOM_STATIC_ITEM_INFO,L"",
		ROOM_STATIC_ITEM_INFO_CNT_RECT,
		false, &pStatic );
	p_pUI->GetStatic(IDC_ROOM_STATIC_ITEM_INFO)->GetElement(0)->SetFont(IDC_ROOM_BTN_CHARA_SEL_FONT, 0xFFFFFFFF, DT_TOP|DT_LEFT|DT_WORDBREAK);
	pStatic->SetVisible(false);

	// 選択したアイテムアイコン
	p_pUI->AddButton( IDC_ROOM_BTN_ITEM_SEL_ICON, L"",
		ROOM_BTN_ITEM_SEL_ICON_CNT_RECT,
		0, false, &pBtn );
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,0);
	// 描画範囲
	SetRect( &rcTexture,	GAME_ITEM_BG_RECT);
	pBtn->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture ,0xFFFFFFFF);
	SetRect( &rcTexture,	GAME_ITEM_01_RECT);
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture ,0xFFFFFFFF);
	pBtn->SetUserData((void*)0);
	pBtn->SetVisible(false);
	// 一つ目をチェックしておく
	p_pUI->GetRadioButton(IDC_ROOM_RB_ITEM_BASE_INDEX)->SetChecked(true);
	OnItemIconButtonDown(p_pUI->GetRadioButton(IDC_ROOM_RB_ITEM_BASE_INDEX));

	// 選択アイテム追加ボタン
	p_pUI->AddButton( IDC_ROOM_BTN_ADD_ITEM, L"",
		ROOM_BTN_ADD_ITEM_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
	pBtn->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, ROOM_BTN_ADD_ITEM_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetVisible(false);

	// ルール //
	// ルール１
	p_pUI->AddCheckBox( IDC_ROOM_CHK_RULE_1,
		ROOM_CHK_RULE_1_DETAIL,
		ROOM_CHK_RULE_1_CNT_RECT,
		false, 0, false, &pChkBox );
	SetRect( &rcTexture, ROOM_CHK_RULE_1_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, ROOM_CHK_RULE_1_CHK_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->GetElement(1)->TextureColor.States[DXUT_STATE_DISABLED] = ROOM_CHK_RULE_DISABLE_COLOR;
	pChkBox->SetUserData((void*)GAME_RULE_01);
	pChkBox->SetVisible(false);
	// ルール２
	p_pUI->AddCheckBox( IDC_ROOM_CHK_RULE_2,
		ROOM_CHK_RULE_2_DETAIL,
		ROOM_CHK_RULE_2_CNT_RECT,
		false, 0, false, &pChkBox );
	SetRect( &rcTexture, ROOM_CHK_RULE_2_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, ROOM_CHK_RULE_2_CHK_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->GetElement(1)->TextureColor.States[DXUT_STATE_DISABLED] = ROOM_CHK_RULE_DISABLE_COLOR;
	pChkBox->SetUserData((void*)GAME_RULE_02);
	pChkBox->SetVisible(false);
	// ルール３
	p_pUI->AddCheckBox( IDC_ROOM_CHK_RULE_3,
		ROOM_CHK_RULE_3_DETAIL,
		ROOM_CHK_RULE_3_CNT_RECT,
		false, 0, false, &pChkBox );
	SetRect( &rcTexture, ROOM_CHK_RULE_3_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, ROOM_CHK_RULE_3_CHK_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->GetElement(1)->TextureColor.States[DXUT_STATE_DISABLED] = ROOM_CHK_RULE_DISABLE_COLOR;
	pChkBox->SetUserData((void*)GAME_RULE_03);
	pChkBox->SetVisible(false);

	// ルール４
	p_pUI->AddCheckBox( IDC_ROOM_CHK_RULE_4,
		ROOM_CHK_RULE_4_DETAIL,
		ROOM_CHK_RULE_4_CNT_RECT,
		false, 0, false, &pChkBox );
	SetRect( &rcTexture, ROOM_CHK_RULE_4_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, ROOM_CHK_RULE_4_CHK_IMG_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->GetElement(1)->TextureColor.States[DXUT_STATE_DISABLED] = ROOM_CHK_RULE_DISABLE_COLOR;
	pChkBox->SetUserData((void*)GAME_RULE_04);
	pChkBox->SetVisible(false);

	// チーム分けのコントロール
	SetTeamSeparateControl();

	// ステージセレクト
	SetStageSelectControl();

	// 自キャラ情報 //
	// 自キャラ名
	// 手前に表示するため後に作成

	// 自チーム番号
	p_pUI->AddStatic( IDC_ROOM_STATIC_MY_TEAMNO,L"",
		ROOM_STATIC_MY_TEAMNO_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_MY_TEAMNO_FONT, 0xFFFFFFFF, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	pStatic->GetElement(0)->FontColor.Init(0xFF000000);
	pStatic->SetDrawShadow(true);
	pStatic->SetUserData(NULL);
	pStatic->SetVisible(false);

	// 自キャラ使用アイテムのアイコン //
	SetMyItemButtonList();

	// 制限ターン数
	p_pUI->AddStatic( IDC_STATIC_SPIN_LIMIT_TURN_TEXT,ROOM_STATIC_SPIN_LIMIT_TURN_TEXT_MSG,
		ROOM_STATIC_SPIN_LIMIT_TURN_TEXT_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_SPIN_LIMIT_TURN_TEXT_FONT, 0xFFFFFFFF, DT_LEFT);
	pStatic->SetVisible(false);

	m_pSpinTurnLimit001->Create(p_pUI, IDC_ROOM_SPIN_LIMIT_TURN_001, m_nDefaultGUIResourceIndex, ROOM_SPIN_LIMIT_TURN_001_CNT_X, ROOM_SPIN_LIMIT_TURN_001_CNT_Y, ROOM_SPIN_LIMIT_TURN_CNT_W, ROOM_SPIN_LIMIT_TURN_CNT_H, ROOM_STATIC_LIMIG_TURN_CNT_W, ROOM_STATIC_LIMIG_TURN_CNT_H);
	m_pSpinTurnLimit010->Create(p_pUI, IDC_ROOM_SPIN_LIMIT_TURN_010, m_nDefaultGUIResourceIndex, ROOM_SPIN_LIMIT_TURN_010_CNT_X, ROOM_SPIN_LIMIT_TURN_010_CNT_Y, ROOM_SPIN_LIMIT_TURN_CNT_W, ROOM_SPIN_LIMIT_TURN_CNT_H, ROOM_STATIC_LIMIG_TURN_CNT_W, ROOM_STATIC_LIMIG_TURN_CNT_H);
	m_pSpinTurnLimit100->Create(p_pUI, IDC_ROOM_SPIN_LIMIT_TURN_100, m_nDefaultGUIResourceIndex, ROOM_SPIN_LIMIT_TURN_100_CNT_X, ROOM_SPIN_LIMIT_TURN_100_CNT_Y, ROOM_SPIN_LIMIT_TURN_CNT_W, ROOM_SPIN_LIMIT_TURN_CNT_H, ROOM_STATIC_LIMIG_TURN_CNT_W, ROOM_STATIC_LIMIG_TURN_CNT_H);

	// ルール：制限時間
	p_pUI->AddStatic( IDC_ROOM_STATIC_ACT_TIME_LIMIT_TEXT,ROOM_STATIC_ACT_TIME_LIMIT_TEXT_MSG,
		ROOM_STATIC_ACT_TIME_LIMIT_TEXT_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_ACT_TIME_LIMIT_TEXT_FONT, 0xFFFFFFFF, DT_LEFT);
	pStatic->SetVisible(false);

	p_pUI->AddButton( IDC_ROOM_BTN_ACT_TIME_LIMIT_L, L"",
		ROOM_BTN_ACT_TIME_LIMIT_L_CNT_RECT,
		0, false, &pBtn );
	SetRect(&rcTexture, ROOM_BTN_ACT_TIME_LIMIT_L_IMG_RECT);
	pBtn->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(-1,0);
	pBtn->SetUserData((void*)-1);
	pBtn->SetVisible(false);

	p_pUI->AddButton( IDC_ROOM_BTN_ACT_TIME_LIMIT_R, L"",
		ROOM_BTN_ACT_TIME_LIMIT_R_CNT_RECT,
		0, false, &pBtn );
	SetRect(&rcTexture, ROOM_BTN_ACT_TIME_LIMIT_R_IMG_RECT);
	pBtn->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(1,0);
	pBtn->SetUserData((void*)1);
	pBtn->SetVisible(false);

	p_pUI->AddStatic( IDC_ROOM_STATIC_ACT_TIME_LIMIT_VALUE,L"20",
		ROOM_STATIC_ACT_TIME_LIMIT_VALUE_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_ACT_TIME_LIMIT_VALUE_FONT, 0xFFFFFFFF, DT_RIGHT);
	pStatic->SetVisible(false);

	// 残アイテムコスト
	p_pUI->AddStatic( IDC_ROOM_STATIC_ITEM_REST_COST,L"",
		ROOM_STATIC_ITEM_REST_COST_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_ITEM_REST_COST_FONT, 0xFFFFFFFF, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	pStatic->GetElement(0)->FontColor.Init(0xFF000000);
	pStatic->SetDrawShadow(false);
	pStatic->SetVisible(false);

	// LoadingStage /////////////////////////////////////////////////////////////
	// Loading...
	p_pUI->AddSprite( IDC_LOAD_SPRITE_TEXT,
		LOAD_SPRITE_TEXT_CNT_RECT,
		false, &pSprite );
	SetRect( &rcTexture, LOAD_SPRITE_TEXT_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// Main //////////////////////////////////////////////////////////////////////
	// ショットアングル
	m_pShotAngle = new CShotAngle();
	m_pShotAngle->Create(p_pUI, m_nDefaultGUIResourceIndex,MAIN_AL_ANGLE_CNT_RECT);
	m_pShotAngle->SetBaseLineAngle(0+REVERSE_ANGLE);
	m_pShotAngle->SetRangeAngleMin(30+REVERSE_ANGLE);
	m_pShotAngle->SetRangeAngleMax(95+REVERSE_ANGLE);
	m_pShotAngle->SetVisible(false);

	// ショットアングルインジケーター
	m_pShotAngleIndicator = new CShotAngleIndicator();
	if (!m_pShotAngleIndicator->Create(p_pUI, IDC_MAIN_AL_SHOT_ANGLE_INDICATOR, MAIN_AL_SHOT_ANGLE_INDICATOR_X, MAIN_AL_SHOT_ANGLE_INDICATOR_Y, MAIN_AL_SHOT_ANGLE_INDICATOR_LENGTH))
		return FALSE;
	m_pShotAngleIndicator->SetVisible(false);

	// ショットパワー
	p_pUI->AddMeter( IDC_MAIN_METER_SHOTPOWER , 0,MAX_SHOT_POWER,
		MAIN_METER_SHOTPOWER_CNT_RECT,
		false, &pMeter );
	m_pShotMeter = pMeter;
	pMeter->GetElement(0)->TextureColor.Init( 0, 0 );
	pMeter->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, MAIN_METER_SHOTPOWER_IMG_RECT);	// 描画範囲
	pMeter->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, MAIN_METER_CONTROLPANEL_FRAME_IMG_RECT);	// 描画範囲
	pMeter->GetElement(2)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pMeter->SetMargin(0,0);
	pMeter->SetVisible(false);

	SafeDelete(m_pShotPowerIndicator);
	m_pShotPowerIndicator = new CShotPowerIndicator();
	RECT rcSPICnt = {MAIN_SPRITE_SPI_CNT_RECT};
	RECT rcSPIInd = {MAIN_SPRITE_SPI_INDICATE_RECT};
	RECT rcSPIImg = {MAIN_SPRITE_SPI_IMG_RECT};
	if (m_pShotPowerIndicator->Create(p_pUI, IDC_MAIN_SPRITE_SPI, m_nDefaultGUIResourceIndex, &rcSPIInd, &rcSPICnt, &rcSPIImg))
		m_pShotPowerIndicator->SetVisible(false);

	// 移動メータ
	p_pUI->AddMeter( IDC_MAIN_METER_MOVABLE, 1000,1000,
		MAIN_METER_MOVABLE_CNT_RECT,
		false, &pMeter );
	m_pMeterMovable = pMeter;
	pMeter->GetElement(0)->TextureColor.Init( 0, 0 );
	pMeter->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, MAIN_METER_MOVABLE_IMG_RECT);	// 描画範囲
	pMeter->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, MAIN_METER_CONTROLPANEL_FRAME_IMG_RECT);	// 描画範囲
	pMeter->GetElement(2)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pMeter->SetMargin(0,0);
	pMeter->SetVisible(false);

	// HPメータ
	p_pUI->AddMeter( IDC_MAIN_METER_HP, 1000,1000,
		MAIN_METER_HP_CNT_RECT,
		false, &pMeter );
	m_pMeterHP = pMeter;
	pMeter->GetElement(0)->TextureColor.Init( 0, 0 );
	pMeter->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, MAIN_METER_HP_IMG_RECT);	// 描画範囲
	pMeter->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, MAIN_METER_CONTROLPANEL_FRAME_IMG_RECT);	// 描画範囲
	pMeter->GetElement(2)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pMeter->SetMargin(0,0);
	pMeter->SetVisible(false);

	// 風メータL
	p_pUI->AddMeter( IDC_MAIN_METER_WIND_L, 0,MAX_WIND_VALUE,
		MAIN_METER_WIND_L_CNT_RECT,
		false, &pMeter );
	m_pMeterWindL = pMeter;
	pMeter->GetElement(0)->TextureColor.Init( 0, 0 );		
	pMeter->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, MAIN_METER_WIND_L_IMG_RECT);		// 描画範囲
	pMeter->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pMeter->SetMargin(0,0);
	pMeter->SetVisible(false);
	// 風メータR
	p_pUI->AddMeter( IDC_MAIN_METER_WIND_R, 0,MAX_WIND_VALUE,
		MAIN_METER_WIND_R_CNT_RECT,
		false, &pMeter );
	m_pMeterWindR = pMeter;
	pMeter->GetElement(0)->TextureColor.Init( 0, 0 );		
	pMeter->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, MAIN_METER_WIND_R_IMG_RECT);		// 描画範囲
	pMeter->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pMeter->SetMargin(0,0);
	pMeter->SetVisible(false);

	// 前ターンの風向
	p_pUI->AddSprite( IDC_MAIN_SPRITE_PREV_WIND_INDICATOR,
		MAIN_SPRITE_PREV_WIND_INDICATOR_CNT_RECT,
		false, &pSprite );
	SetRect( &rcTexture, MAIN_SPRITE_PREV_WIND_INDICATOR_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// ターン残り秒数の後ろ
	CDXUTAnimationSprite* pAnimationSprite;
	p_pUI->AddAnimationSprite( IDC_MAIN_ASPRITE_TIMER_BG,
		MAIN_ASPRITE_TIMER_CNT_RECT_BG,
		false, TRUE, &pAnimationSprite );
	pAnimationSprite->SetAutoCountOnRender(FALSE);
	pAnimationSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pAnimationSprite->SetVisible(false);
	m_pASpriteTimerBG = pAnimationSprite;

	// ターン残り秒数
	p_pUI->AddAnimationSprite( IDC_MAIN_ASPRITE_TIMER ,
		MAIN_ASPRITE_TIMER_CNT_RECT,
		false, FALSE, &pAnimationSprite );
	pAnimationSprite->SetAutoCountOnRender(FALSE);
	m_pASpriteTimer = pAnimationSprite;
	SetTimerAnimation(m_pASpriteTimer, m_pASpriteTimerBG);
	pAnimationSprite->SetVisible(false);

	// 弾選択ボタン
	SetRect( &rcTexture,
		IDC_ROOM_BTN_CHARA_SEL_CUR_X,
		IDC_ROOM_BTN_CHARA_SEL_CUR_Y,
		(IDC_ROOM_BTN_CHARA_SEL_CUR_X+IDC_ROOM_BTN_CHARA_SEL_CUR_W),
		(IDC_ROOM_BTN_CHARA_SEL_CUR_Y+IDC_ROOM_BTN_CHARA_SEL_CUR_H)
	);
	for (int i=0;i<MAX_CHARA_BULLET_TYPE;i++)
	{
		p_pUI->AddRadioButton( IDC_MAIN_BTN_SEL_BULLET_BASE+i,
			IDC_MAIN_BTN_SEL_BULLET_BASE,
			L"",
			MAIN_BTN_SEL_BULLET_X+i*MAIN_BTN_SEL_BULLET_W,
			MAIN_BTN_SEL_BULLET_Y,
			MAIN_BTN_SEL_BULLET_W,
			MAIN_BTN_SEL_BULLET_H,
			false, 0, false, &m_pBulletSelectButtons[i]);
		m_pBulletSelectButtons[i]->GetElement(0)->SetTexture(0,NULL, 0);
		m_pBulletSelectButtons[i]->GetElement(0)->TextureColor.States[DXUT_STATE_DISABLED] = 0xFFFFFFFF;
		m_pBulletSelectButtons[i]->GetElement(1)->TextureColor.States[DXUT_STATE_DISABLED] = 0xFFFFFFFF;
		m_pBulletSelectButtons[i]->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);
		m_pBulletSelectButtons[i]->SetUserData((void*)i);
		m_pBulletSelectButtons[i]->SetHandleKeyboardEnabled(false);
		m_pBulletSelectButtons[i]->SetVisible(false);
	}

	// DELAY値
	p_pUI->AddStatic( IDC_MAIN_STATIC_DELAY,L"0",
		MAIN_STATIC_DELAY_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(MAIN_STATIC_DELAY_CNT_FONT, 0xFFFFFFFF, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	pStatic->GetElement(0)->FontColor.Init(0xFF000000);
	pStatic->SetDrawShadow(false);
	pStatic->SetUserData(NULL);
	pStatic->SetVisible(false);

	// TURN値
	p_pUI->AddStatic( IDC_MAIN_STATIC_TURN,L"0",
		MAIN_STATIC_TURN_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(MAIN_STATIC_TURN_CNT_FONT, 0xFFFFFFFF, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	pStatic->GetElement(0)->FontColor.Init(0xFF000000);
	pStatic->SetDrawShadow(false);
	pStatic->SetUserData(0);
	pStatic->SetVisible(false);

	p_pUI->AddButton( IDC_MAIN_RB_SEL_BULLET_L,L"",
		MAIN_RB_SEL_BULLET_LR_CNT_X,
		MAIN_RB_SEL_BULLET_LR_CNT_Y,
		MAIN_RB_SEL_BULLET_LR_CNT_W,
		MAIN_RB_SEL_BULLET_LR_CNT_H,
		0, false, &pBtn );
	SetRect( &rcTexture,	IDC_ROOM_BTN_CHARA_SEL_LIST_L_IMG_RECT);
	pBtn->GetElement(0)->TextureColor.Init(0,0,0);
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(-1,0);
	pBtn->SetVisible(false);

	p_pUI->AddButton( IDC_MAIN_RB_SEL_BULLET_R,L"",
		MAIN_RB_SEL_BULLET_LR_CNT_X,
		MAIN_RB_SEL_BULLET_LR_CNT_Y,
		MAIN_RB_SEL_BULLET_LR_CNT_W,
		MAIN_RB_SEL_BULLET_LR_CNT_H,
		0, false, &pBtn );
	SetRect( &rcTexture,	IDC_ROOM_BTN_CHARA_SEL_LIST_R_IMG_RECT);
	pBtn->GetElement(0)->TextureColor.Init(0,0,0);
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(1,0);
	pBtn->SetVisible(false);

	// FACE
	p_pUI->AddSprite( IDC_MAIN_SPRITE_FACE,
		MAIN_SPRITE_FACE_CNT_X,MAIN_SPRITE_FACE_CNT_Y,MAIN_SPRITE_FACE_CNT_W,MAIN_SPRITE_FACE_CNT_H,
		false, &pSprite );
	SetRect( &rcTexture, MAIN_SPRITE_FACE_IMG_RECT_DUMMY);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// ターンパスボタン
	p_pUI->AddButton( IDC_MAIN_BTN_TURN_PASS,L"",
		MAIN_BTN_TURN_PASS_CNT_RECT,
		0, false, &pBtn );
	SetRect( &rcTexture,	MAIN_BTN_TURN_PASS_IMG_ENABLE_RECT);
	pBtn->GetElement(0)->TextureColor.Init(0,0,0);
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(1,1);
	pBtn->SetVisible(false);

	p_pUI->AddStatic( IDC_MAIN_STATIC_NOTIFY_TURN, L"",
		MAIN_STATIC_NOTIFY_TURN_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(MAIN_STATIC_NOTIFY_TURN_FONT, 0xFFFFFFFF, DT_TOP|DT_CENTER|DT_SINGLELINE);
	pStatic->GetElement(0)->FontColor.Init(0xFFFFFFFF);
	pStatic->SetDrawShadow(true);
	pStatic->SetVisible(false);

	p_pUI->AddStatic( IDC_MAIN_STATIC_NOTIFY_TURN_TEXT, MAIN_STATIC_NOTIFY_TURN_TEXT,
		MAIN_STATIC_NOTIFY_TURN_TEXT_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(MAIN_STATIC_NOTIFY_TURN_TEXT_FONT, 0xFFFFFFFF, DT_TOP|DT_CENTER|DT_SINGLELINE);
	pStatic->GetElement(0)->FontColor.Init(0xFFFFFFFF);
	pStatic->SetDrawShadow(true);
	pStatic->SetVisible(false);

	p_pUI->AddMeter( IDC_MAIN_METER_SPELL_EXP, 0,1000,
		MAIN_METER_SPELL_EXP_CNT_RECT,
		false, &pMeter );
	m_pMeterSCExp = pMeter;
//	SetRect( &rcTexture,	480,200,512,232);
//	pMeter->GetElement(0)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
//	pMeter->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);
	pMeter->GetElement(0)->TextureColor.Init( 0, 0 );	
	pMeter->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture,	MAIN_METER_SPELL_EXP_IMG_RECT);
	pMeter->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);
	pMeter->SetMargin(0,0);
	pMeter->SetMeterStyle(CDXUTMeterStyleBottomToTop);
	pMeter->SetVisible(false);

	p_pUI->AddRadioButton(IDC_MAIN_RB_SEL_SPELL,
		IDC_MAIN_BTN_SEL_BULLET_BASE,L"",
		MAIN_RB_SPELL_CNT_RECT,false, 0, false, &pRadioButton);
	m_pRbSC = pRadioButton;
	pRadioButton->GetElement(0)->TextureColor.States[DXUT_STATE_DISABLED] = 0xFFFFFFFF;
	pRadioButton->GetElement(1)->TextureColor.States[DXUT_STATE_DISABLED] = 0xFFFFFFFF;
	SetRect( &rcTexture,
		IDC_ROOM_BTN_CHARA_SEL_CUR_X,
		IDC_ROOM_BTN_CHARA_SEL_CUR_Y,
		(IDC_ROOM_BTN_CHARA_SEL_CUR_X+IDC_ROOM_BTN_CHARA_SEL_CUR_W),
		(IDC_ROOM_BTN_CHARA_SEL_CUR_Y+IDC_ROOM_BTN_CHARA_SEL_CUR_H)
	);
	pRadioButton->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);
	pRadioButton->SetUserData((void*)MAX_CHARA_BULLET_TYPE);
	pRadioButton->SetVisible(false);

	// アイテム説明
	p_pUI->AddButton( IDC_ROOM_BTN_ITEM_DETAIL, L"",
		0,0,200,24,
		0, false, &pBtn );
	SetRect(&rcTexture, ROOM_BTN_ITEM_DETAIL_IMG_RECT);
	pBtn->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pBtn->GetElement(0)->SetFont(2, 0xFFFFFFFF, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
	pBtn->GetElement(1)->TextureColor.Init(0,0);
	pBtn->GetElement(1)->SetFont(0,0);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,0);
	pBtn->SetUserData(0);
	pBtn->SetVisible(false);

	//> Result
	// ランキング背景
	p_pUI->AddSprite( IDC_RESULT_SPRITE_BG_SCREEN,RESULT_SPRITE_BG_SCREEN_CNT_RECT, false, &pSprite);
	SetRect( &rcTexture, RESULT_SPRITE_BG_SCREEN_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);
	
	// ランキング用の線
	SetRect(&rcTexture, RESULT_SPRITE_LINE_IMG_RECT);
	for (int i=0;i<RESULT_RANK_VIEW_RANGE-1;i++)
	{
		const DWORD cLineColorTable[RESULT_RANK_VIEW_RANGE-1] = 
		{ 0xFF0000FF, 0xFF2000DF, 0xFF4000BF, 0xFF60009F, 0xFF800080,0xFFA00060, 0xFFC00040,0xFFE00020, 0xFFFF0000 };
		p_pUI->AddSprite(IDC_RESULT_SPRITE_LINE_BASE+i, 
			RESULT_SPRITE_RANK_NO_CNT_X_BASE,
			RESULT_SPRITE_RANK_NO_CNT_Y_BASE+(RESULT_SPRITE_RANK_NO_CNT_H)*(i+1)+(RESULT_SPRITE_LINE_CNT_H*i),
			RESULT_SPRITE_RANK_NO_CNT_W+RESULT_SPRITE_ICON_CNT_W+RESULT_STATIC_NAME_CNT_W+RESULT_STATIC_TEAM_CNT_W,
			RESULT_SPRITE_LINE_CNT_H,
			false, &pSprite);
		pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture, cLineColorTable[i]);
		pSprite->SetVisible(false);
	}
	// 結果確認ボタン
	// Butons
	p_pUI->AddButton( IDC_RESULT_BTN_CONFIRM, L"",
		RESULT_BTN_CONFIRM_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pBtn->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	pBtn->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);	
	SetRect( &rcTexture, RESULT_BTN_CONFIRM_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetVisible(false);

	// ランキング用スクロールバー
	if (g_bOneClient)
	{
		p_pUI->AddScrollBar(IDC_RESULT_SB_RANK, RESULT_SB_RANK_CNT_X, RESULT_SB_RANK_CNT_Y, RESULT_SB_RANK_CNT_W, RESULT_SB_RANK_CNT_H, 0, MAXUSERNUM-RESULT_RANK_VIEW_RANGE+1, 0, false, &pScrollBar);
		m_nRankItemCount = MAXUSERNUM;
	}
	else
	{
		p_pUI->AddScrollBar(IDC_RESULT_SB_RANK, RESULT_SB_RANK_CNT_X, RESULT_SB_RANK_CNT_Y, RESULT_SB_RANK_CNT_W, RESULT_SB_RANK_CNT_H, 0, (int)m_vecCharacters.size()-RESULT_RANK_VIEW_RANGE+1, 0, false, &pScrollBar);
		m_nRankItemCount = m_vecCharacters.size();
	}
	SetRect( &rcTexture, RESULT_SB_IMG_U_IMG_RECT);	// 描画範囲
	pScrollBar->GetElement(1)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, RESULT_SB_IMG_D_IMG_RECT);	// 描画範囲
	pScrollBar->GetElement(2)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, RESULT_SB_IMG_B_IMG_RECT);	// 描画範囲
	pScrollBar->GetElement(3)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pScrollBar->SetVisible(false);

	// ランキングタイトル
	p_pUI->AddSprite( IDC_RESULT_SPRITE_RANK_TITLE, RESULT_SPRITE_RANK_TITLE_CNT_RECT, false, &pSprite);
	SetRect( &rcTexture, RESULT_SPRITE_RANK_TITLE_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// テキスト背景
	p_pUI->AddSprite( IDC_RESULT_SPRITE_TEXT_BG, RESULT_SPRITE_TEXT_BG_CNT_RECT, false, &pSprite);
	SetRect( &rcTexture, RESULT_SPRITE_TEXT_BG_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);
	
	// 顔背景
	p_pUI->AddSprite( IDC_RESULT_SPRITE_FACE_BG, RESULT_SPRITE_FACE_BG_CNT_RECT, false, &pSprite);
	SetRect( &rcTexture, RESULT_SPRITE_FACE_BG_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// 顔
	p_pUI->AddSprite( IDC_RESULT_SPRITE_FACE, RESULT_SPRITE_FACE_CNT_RECT, false, &pSprite);
	SetRect( &rcTexture, RESULT_SPRITE_FACE_IMG_RECT);	// 描画範囲
	pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	pSprite->SetVisible(false);

	// テキスト
	p_pUI->AddStatic( IDC_RESULT_STATIC_TEXT,L"",
		RESULT_STATIC_TEXT_CNT_RECT,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(RESULT_STATIC_TEXT_CNT_FONT, 0xFFFFFFFF, DT_LEFT|DT_TOP|DT_WORDBREAK);
	pStatic->SetDrawShadow(false);
	pStatic->SetVisible(false);

	// 自キャラ名
	p_pUI->AddStatic( IDC_ROOM_STATIC_MY_NAME,L"",
		ROOM_STATIC_MY_NAME_CNT_X,ROOM_STATIC_MY_NAME_CNT_Y,ROOM_STATIC_MY_NAME_CNT_W,ROOM_STATIC_MY_NAME_CNT_H,
		false, &pStatic);
	pStatic->GetElement(0)->SetFont(ROOM_STATIC_MY_NAME_FONT, 0xFF000000, DT_TOP|DT_LEFT|DT_SINGLELINE);
	pStatic->SetDrawShadow(false);
	pStatic->SetUserData(NULL);
	pStatic->SetVisible(false);

	CreateConfigGUI();

	return TRUE;
}
//< GUIコントロールの作成

// チーム分けのコントロール //
void CGame::SetTeamSeparateControl()
{
	CDXUTStatic *pStatic;
	RECT rcTexture;
	p_pUI->AddStatic( IDC_ROOM_STATIC_RULE_TEAM,L"",
		ROOM_STATIC_RULE_TEAM_CNT_RECT,
		false, &pStatic );
	pStatic->GetElement(0)->SetFont(IDC_ROOM_BTN_CHARA_SEL_FONT, 0xFFFFFFFF, DT_TOP|DT_CENTER|DT_WORDBREAK);
	pStatic->SetVisible(false);
	// チームわけLボタン
	CDXUTButton* pButtonL;
	p_pUI->AddButton( IDC_ROOM_BTN_RULE_TEAM_L, L"",
		ROOM_BTN_RULE_TEAM_L_CNT_RECT,
		0, false, &pButtonL );
	pButtonL->GetElement(0)->TextureColor.Init( 0,0,0 );
	pButtonL->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, ROOM_BTN_RULE_TEAM_L_IMG_RECT);	// 描画範囲
	pButtonL->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pButtonL->SetUserData((void*)(-1));
	// チームわけRボタン
	CDXUTButton* pButtonR;
	p_pUI->AddButton( IDC_ROOM_BTN_RULE_TEAM_R, L"",
		ROOM_BTN_RULE_TEAM_R_CNT_RECT,
		0, false, &pButtonR );
	pButtonR->GetElement(0)->TextureColor.Init( 0,0,0 );
	pButtonR->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	SetRect( &rcTexture, ROOM_BTN_RULE_TEAM_R_IMG_RECT);	// 描画範囲
	pButtonR->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pButtonR->SetUserData((void*)(1));

	SafeDelete(m_pTeamRulePropertyManager);
	m_pTeamRulePropertyManager = new CTeamRulePropertyControlManager();
	m_pTeamRulePropertyManager->Create(pButtonL, pButtonR, pStatic, &m_nAuthedCount);
	m_pTeamRulePropertyManager->SetVisible(false);

	// チーム分割線用コントロール
	SafeDelete(m_pTeamSeparater);
	m_pTeamSeparater = new CTeamSeparater();
	int nGround = AddResourceTexture(IDC_ROOM_GROUND_IMAGE);
	m_pTeamSeparater->Create(nGround, p_pUI, 0);
	m_pTeamSeparater->SetVisible(false);
}

// ユーザーリストコントロール //
void CGame::SetUserList()
{
	CDXUTElement* pElement = NULL;
	RECT rcTexture;
	CDXUTColorListBox* pColorListBox = NULL;
    // List box
	p_pUI->AddColorListBox( IDC_ROOM_LB_USERLIST,
		ROOM_USERLIST_LB_CNT_RECT,
		0, &pColorListBox );

	pColorListBox->SetChangeSelectColor(false);
	pColorListBox->SetBorder(4,24);
	pColorListBox->m_bAcceptKeyboard = false;

	pElement = pColorListBox->GetElement(0);
	SetRect(&rcTexture, ROOM_USERLIST_LB_IMG_RECT);
	pElement->SetFont( 1, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_VCENTER );
	pElement->SetTexture(m_nDefaultGUIResourceIndex,&rcTexture, D3DCOLOR_ARGB( 0, 0, 0, 0 ));
	// 背景枠
	pElement = pColorListBox->GetElement(1);
	SetRect(&rcTexture, ROOM_CHATLOG_LB_IMG_1_RECT);
    pElement->SetFont( 1, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pElement->SetTexture(m_nDefaultGUIResourceIndex,&rcTexture, D3DCOLOR_ARGB( 0,0,0,0 ));

	CDXUTScrollBar* pScrollBar = pColorListBox->GetScrollBar();
	// スクロールバー
	SetRect( &rcTexture, ROOM_USERLIST_SB_U_RECT);	// 描画範囲
	pScrollBar->GetElement(1)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, ROOM_USERLIST_SB_D_RECT);	// 描画範囲
	pScrollBar->GetElement(2)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);
	SetRect( &rcTexture, BTN_VBAR_RECT);	// 描画範囲
	pScrollBar->GetElement(3)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);

	// リストアイテム１
	pElement = pColorListBox->GetElement(2);
	SetRect(&rcTexture, ROOM_BTN_CHARA_SEL_RANDOM_ICON_IMG_RECT);
    pElement->SetFont( 1, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pElement->SetTexture(m_nDefaultGUIResourceIndex,&rcTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ));

	// ランダム
	CDXUTElement* pNewElement = new CDXUTElement(*pElement);
	SetRect(&rcTexture, ROOM_BTN_CHARA_SEL_RANDOM_ICON_IMG_RECT);
	pNewElement->SetFont(1, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pNewElement->SetTexture(m_nDefaultGUIResourceIndex,&rcTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
	pColorListBox->SetElement(2, pNewElement);
	SafeDelete(pNewElement);

	pNewElement = new CDXUTElement(*pElement);
	SetRect(&rcTexture, ROOM_BTN_CHARA_SEL_RANDOM_ICON_IMG_RECT);
	pNewElement->SetFont(1, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
	pNewElement->SetTexture(m_nDefaultGUIResourceIndex,&rcTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
	pColorListBox->SetElement(3, pNewElement);
	SafeDelete(pNewElement);

	pColorListBox = (CDXUTColorListBox*)p_pUI->GetControl(IDC_ROOM_LB_USERLIST);
	int nElementIndex = 3;	// 2 = 0,1,2が背景、選択色、ランダムで登録済みのため 2 スタート
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	// 2をデフォルト設定
//	pElement = pColorListBox->GetElement(nElementIndex);
	for (	std::map<int, TCHARA_SCR_INFO>::iterator it = m_mapCharaScrInfo.begin();
			it != m_mapCharaScrInfo.end();
			it++)
	{
		RECT rcTexture;

		luaParams.Clear();
		luaResults.Clear();
		// アイコンテクスチャ範囲
		luaParams.Number(it->second.scr_index);

		if (!common::scr::CallLuaFunc(g_pLuah, "getChara_TexFace", &luaResults, 2, &luaParams, g_pCriticalSection))
			return;

		rcTexture.left = (int)luaResults.GetNumber(0);
		rcTexture.top = (int)luaResults.GetNumber(1);
		rcTexture.right = rcTexture.left + ROOM_USERLIST_LB_ICON_W;
		rcTexture.bottom = rcTexture.top + ROOM_USERLIST_LB_ICON_H;

		// リストアイテム登録
		CDXUTElement* pNewElement = new CDXUTElement(*pElement);
		pNewElement->SetFont(1, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_LEFT | DT_VCENTER );
		pNewElement->SetTexture(it->second.res_index,&rcTexture, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
		pColorListBox->SetElement(it->second.res_index, pNewElement);
		SafeDelete(pNewElement);

		nElementIndex++;
	}
	pColorListBox->SetVisible(false);
}

// キャラクタ用スクリプト分アイコンリストに追加 //
void CGame::SetMyItemButtonList()
{
	RECT rcTexture;
	CDXUTButton* pBtn  = NULL;
	CDXUTButton* pBtnL  = NULL;
	CDXUTButton* pBtnR  = NULL;
	POINT pnt;
	pnt.x = ROOM_BTN_MY_ITEM_LIST_X;
	pnt.y = ROOM_BTN_MY_ITEM_LIST_Y;

	p_pUI->AddButton( IDC_ROOM_BTN_MY_ITEM_SEL_L, L"",
		ROOM_BTN_MY_ITEM_SEL_L_CNT,
		0, false, &pBtnL );
	SetRect( &rcTexture,	IDC_ROOM_BTN_CHARA_SEL_LIST_L_IMG_RECT);
	pBtnL->GetElement(0)->TextureColor.Init(0,0,0);
	pBtnL->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pBtnL->SetStateMouserOverOffset(0,0);
	pBtnL->SetStatePressedOverOffset(0,0);
	pBtnL->SetUserData(0);
	pBtnL->SetEnabledRightClickEvent(true);
	pBtnL->SetVisible(false);
	p_pUI->AddButton( IDC_ROOM_BTN_MY_ITEM_SEL_R, L"",
		ROOM_BTN_MY_ITEM_SEL_R_CNT,
		0, false, &pBtnR );
	SetRect( &rcTexture,	IDC_ROOM_BTN_CHARA_SEL_LIST_R_IMG_RECT);
	pBtnR->GetElement(0)->TextureColor.Init(0,0,0);
	pBtnR->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pBtnR->SetStateMouserOverOffset(0,0);
	pBtnR->SetStatePressedOverOffset(0,0);
	pBtnR->SetUserData(0);
	pBtnR->SetEnabledRightClickEvent(true);
	pBtnR->SetVisible(false);
	m_pMyItemBtnList->Create(pBtnL, pBtnR, &pnt, ROOM_BTN_MY_ITEM_LIST_W);
	m_pMyItemBtnList->SetLRBtnAlwaysVisible(TRUE);
	m_pMyItemBtnList->SetLRPadding(4);

	SetRect(&rcTexture, ROOM_BTN_MY_ITEM_BASE_IMG_RECT);

	for (int i=0;i<GAME_ITEM_STOCK_MAX_COUNT;i++)
	{
		p_pUI->AddButton( IDC_ROOM_BTN_MY_ITEM_BASE+i, L"",
			ROOM_BTN_MY_ITEM_BASE_LEFT+(i*(ROOM_BTN_MY_ITEM_BASE_WIDTH+ROOM_BTN_MY_ITEM_BASE_OFFSET)),
			ROOM_BTN_MY_ITEM_BASE_TOP,
			ROOM_BTN_MY_ITEM_BASE_WIDTH,
			ROOM_BTN_MY_ITEM_BASE_HEIGHT,
			0, false, &pBtn );
		pBtn->GetElement(0)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
		pBtn->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
		pBtn->GetElement(1)->TextureColor.Init(0, 0);
		pBtn->SetStateMouserOverOffset(0,0);
		pBtn->SetStatePressedOverOffset(0,0);
		pBtn->SetUserData(0);
		pBtn->SetEnabledRightClickEvent(true);
		pBtn->SetVisible(false);
		m_pMyItemBtnList->AddButton(pBtn);
	}
	m_pMyItemBtnList->SetVisible(false);
}

// キャラクタ用スクリプト分アイコンリストに追加 //
void CGame::SetCharacterIconButtonList()
{
	RECT rcTexture;
	CDXUTButton *pCharaSelBtnL;
	p_pUI->AddButton( IDC_ROOM_BTN_CHARA_SEL_LIST_L,L"",
		IDC_ROOM_BTN_CHARA_SEL_LIST_X,
		IDC_ROOM_BTN_CHARA_SEL_LIST_Y,
		IDC_ROOM_BTN_CHARA_SEL_LIST_W,
		IDC_ROOM_BTN_CHARA_SEL_LIST_H,
		0, false, &pCharaSelBtnL );
	SetRect( &rcTexture,	IDC_ROOM_BTN_CHARA_SEL_LIST_L_IMG_RECT);
	pCharaSelBtnL->GetElement(0)->TextureColor.Init(0,0,0);
	pCharaSelBtnL->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pCharaSelBtnL->SetStateMouserOverOffset(0,0);
	pCharaSelBtnL->SetStatePressedOverOffset(-1,0);
	pCharaSelBtnL->SetEnabledRightClickEvent(true);

	CDXUTButton *pCharaSelBtnR;
	p_pUI->AddButton( IDC_ROOM_BTN_CHARA_SEL_LIST_R,L"",
		IDC_ROOM_BTN_CHARA_SEL_LIST_X,
		IDC_ROOM_BTN_CHARA_SEL_LIST_Y,
		IDC_ROOM_BTN_CHARA_SEL_LIST_W,
		IDC_ROOM_BTN_CHARA_SEL_LIST_H,
		0, false, &pCharaSelBtnR );
	SetRect( &rcTexture,	IDC_ROOM_BTN_CHARA_SEL_LIST_R_IMG_RECT);
	pCharaSelBtnR->GetElement(0)->TextureColor.Init(0,0,0);
	pCharaSelBtnR->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	pCharaSelBtnR->SetStateMouserOverOffset(0,0);
	pCharaSelBtnR->SetStatePressedOverOffset(1,0);
	pCharaSelBtnR->SetEnabledRightClickEvent(true);

	POINT pnt;
	pnt.x = IDC_ROOM_BTN_CHARA_SEL_LIST_X;
	pnt.y = IDC_ROOM_BTN_CHARA_SEL_LIST_Y;
	m_pCharacterBtnList->Create(pCharaSelBtnL, pCharaSelBtnR, &pnt, IDC_ROOM_BTN_CHARA_SEL_LIST_WIDTH);
	m_pCharacterBtnList->SetLRBtnAlwaysVisible(TRUE);

	CDXUTRadioButton *pRadioButton;
	// ランダムボタンを作成
	p_pUI->AddRadioButton( IDC_ROOM_BTN_CHARA_SEL_RANDOM ,
		IDC_ROOM_BTN_CHARA_SEL_BTN_GROUP,L"",
		IDC_ROOM_BTN_CHARA_SEL_LIST_X,
		IDC_ROOM_BTN_CHARA_SEL_LIST_Y,
		ROOM_BTN_CHARA_SEL_RANDOM_CNT_W,
		ROOM_BTN_CHARA_SEL_RANDOM_CNT_H,
		0, 0, false, &pRadioButton );
	pRadioButton->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	pRadioButton->GetElement(0)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	pRadioButton->GetElement(0)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	// 描画範囲
	SetRect( &rcTexture,ROOM_BTN_CHARA_SEL_RANDOM_IMG_RECT);
	// 追加した管理テクスチャ番号を指定
	pRadioButton->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
	SetRect( &rcTexture,
		IDC_ROOM_BTN_CHARA_SEL_CUR_X,
		IDC_ROOM_BTN_CHARA_SEL_CUR_Y,
		(IDC_ROOM_BTN_CHARA_SEL_CUR_X+IDC_ROOM_BTN_CHARA_SEL_CUR_W),
		(IDC_ROOM_BTN_CHARA_SEL_CUR_Y+IDC_ROOM_BTN_CHARA_SEL_CUR_H)
	);
	pRadioButton->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);
	pRadioButton->SetUserData( (void*)ROOM_CHARA_RANDOM_ID );
	m_pCharacterBtnList->AddButton((CDXUTButton*)pRadioButton);

	// 要素を出力する
	for (map< int, TCHARA_SCR_INFO >::iterator it = m_mapCharaScrInfo.begin();
		it != m_mapCharaScrInfo.end();
		it++)
	{
		TCHARA_SCR_INFO* scrinfo = & ( (*it).second);
		p_pUI->AddRadioButton( IDC_ROOM_BTN_CHARA_SEL_LIST_BASE+(*it).first,
			IDC_ROOM_BTN_CHARA_SEL_BTN_GROUP,L"",
			IDC_ROOM_BTN_CHARA_SEL_LIST_X,
			IDC_ROOM_BTN_CHARA_SEL_LIST_Y,
			scrinfo->rec_sel_icon.right,
			scrinfo->rec_sel_icon.bottom,
			0, 0, false, &pRadioButton );

		pRadioButton->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
		pRadioButton->GetElement(0)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
		pRadioButton->GetElement(0)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
		// 描画範囲
		SetRect( &rcTexture,
			scrinfo->rec_sel_icon.left,
			scrinfo->rec_sel_icon.top,
			scrinfo->rec_sel_icon.left+scrinfo->rec_sel_icon.right,
			scrinfo->rec_sel_icon.top+scrinfo->rec_sel_icon.bottom);
		// 追加した管理テクスチャ番号を指定
		pRadioButton->GetElement(0)->SetTexture( scrinfo->res_index, &rcTexture );
		// ボタンとスクリプトID情報を関連付ける

		SetRect( &rcTexture,
			IDC_ROOM_BTN_CHARA_SEL_CUR_X,
			IDC_ROOM_BTN_CHARA_SEL_CUR_Y,
			(IDC_ROOM_BTN_CHARA_SEL_CUR_X+IDC_ROOM_BTN_CHARA_SEL_CUR_W),
			(IDC_ROOM_BTN_CHARA_SEL_CUR_Y+IDC_ROOM_BTN_CHARA_SEL_CUR_H)
		);
		pRadioButton->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);

		pRadioButton->SetUserData( (void*)(*it).first );
		m_pCharacterBtnList->AddButton((CDXUTButton*)pRadioButton);
	}

	m_pCharacterBtnList->SetVisible(false);
}

// ステージセレクトコントロール //
void CGame::SetStageSelectControl()
{
	RECT rcTexture;
	CDXUTButton* pBtn = NULL;
	// ステージタイトル
	p_pUI->AddStatic( IDC_ROOM_STATIC_STAGE_TITLE,L"",
		ROOM_STATIC_STAGE_TITLE_CNT_RECT,
		false, NULL);
	p_pUI->GetStatic(IDC_ROOM_STATIC_STAGE_TITLE)->GetElement(0)->SetFont(ROOM_STATIC_STAGE_TITLE_FONT, 0xFFFFFFFF, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	p_pUI->GetStatic(IDC_ROOM_STATIC_STAGE_TITLE)->SetVisible(false);

	// ステージ選択ボタンL
	p_pUI->AddButton( IDC_ROOM_BTN_STAGE_L, L"",
		ROOM_BTN_STAGE_L_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
	pBtn->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(-1,0);
	SetRect( &rcTexture, ROOM_BTN_STAGE_L_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetUserData((void*)-1);
	pBtn->SetVisible(false);

	// ステージ選択ボタンR
	p_pUI->AddButton( IDC_ROOM_BTN_STAGE_R, L"",
		ROOM_BTN_STAGE_R_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
	pBtn->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(1,0);
	SetRect( &rcTexture, ROOM_BTN_STAGE_R_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetUserData((void*)1);
	pBtn->SetVisible(false);

	// ステージ選択ボタンRND
	p_pUI->AddButton( IDC_ROOM_BTN_STAGE_RND, L"",
		ROOM_BTN_STAGE_RND_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
	pBtn->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,1);
	SetRect( &rcTexture, ROOM_BTN_STAGE_RND_IMG_RECT);	// 描画範囲
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pBtn->SetUserData((void*)1);
	pBtn->SetVisible(false);

	// ステージ選択
	p_pUI->AddButton( IDC_ROOM_BTN_STAGE_BASE, L"",
		ROOM_BTN_STAGE_BASE_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0xFFFFFFFF,0xFFFFFFFF,0 );
	pBtn->GetElement(1)->TextureColor.Init(0, 0);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(0,0);
	pBtn->SetUserData((void*)-1);
	pBtn->SetVisible(false);
}

void CGame::SetTimerAnimation(CDXUTAnimationSprite* pASprite, CDXUTAnimationSprite* pASpriteBG)
{
	RECT t_rcTimerAnimationTable[] =
	{
		{MAIN_ASPRITE_TIMER_RECT_00},
		{MAIN_ASPRITE_TIMER_RECT_01},		{MAIN_ASPRITE_TIMER_RECT_02},
		{MAIN_ASPRITE_TIMER_RECT_03},		{MAIN_ASPRITE_TIMER_RECT_04},
		{MAIN_ASPRITE_TIMER_RECT_05},		{MAIN_ASPRITE_TIMER_RECT_06},
		{MAIN_ASPRITE_TIMER_RECT_07},		{MAIN_ASPRITE_TIMER_RECT_08},
		{MAIN_ASPRITE_TIMER_RECT_09},		{MAIN_ASPRITE_TIMER_RECT_10},
		{MAIN_ASPRITE_TIMER_RECT_11},		{MAIN_ASPRITE_TIMER_RECT_12},
		{MAIN_ASPRITE_TIMER_RECT_13},		{MAIN_ASPRITE_TIMER_RECT_14},
		{MAIN_ASPRITE_TIMER_RECT_15},		{MAIN_ASPRITE_TIMER_RECT_16},
		{MAIN_ASPRITE_TIMER_RECT_17},		{MAIN_ASPRITE_TIMER_RECT_18},
		{MAIN_ASPRITE_TIMER_RECT_19},		{MAIN_ASPRITE_TIMER_RECT_20},
		{MAIN_ASPRITE_TIMER_RECT_21},		{MAIN_ASPRITE_TIMER_RECT_22},
		{MAIN_ASPRITE_TIMER_RECT_23},		{MAIN_ASPRITE_TIMER_RECT_24},
		{MAIN_ASPRITE_TIMER_RECT_25},
	};
	RECT rcTexture = {MAIN_ASPRITE_TIMER_RECT_19};
	pASprite->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pASprite->SetAnimationTime(RUN_FRAMES);
	for (int i=GAME_TURN_ACT_COUNT_MAX-1;i>=0;i--)
		pASprite->AddAnimationRect(&t_rcTimerAnimationTable[i]);

	RECT t_rcTimerBGAnimationTable[] =
	{
		{MAIN_ASPRITE_TIMER_BG_RECT_00},
		{MAIN_ASPRITE_TIMER_BG_RECT_01},		{MAIN_ASPRITE_TIMER_BG_RECT_02},
		{MAIN_ASPRITE_TIMER_BG_RECT_03},		{MAIN_ASPRITE_TIMER_BG_RECT_04},
		{MAIN_ASPRITE_TIMER_BG_RECT_05},		{MAIN_ASPRITE_TIMER_BG_RECT_06},
		{MAIN_ASPRITE_TIMER_BG_RECT_07},
	};
	SetRect(&rcTexture,MAIN_ASPRITE_TIMER_BG_RECT_00);
	pASpriteBG->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture,0xFFFFFFFF);	// 追加した管理テクスチャ番号を指定
	pASpriteBG->SetAnimationTime(RUN_FRAMES);
	for (int i=MAIN_ASPRITE_TIMER_BG_RECT_TABLE_COUNT-1;i>=0;i--)
		pASpriteBG->AddAnimationRect(&t_rcTimerBGAnimationTable[i]);
}

const RECT c_rcRankNoImgRectTable[] =
{
	RESULT_RANK_NO_01_IMG_RECT,	RESULT_RANK_NO_02_IMG_RECT,	RESULT_RANK_NO_03_IMG_RECT,	RESULT_RANK_NO_04_IMG_RECT,
	RESULT_RANK_NO_05_IMG_RECT,	RESULT_RANK_NO_06_IMG_RECT,	RESULT_RANK_NO_07_IMG_RECT,	RESULT_RANK_NO_08_IMG_RECT,
	RESULT_RANK_NO_09_IMG_RECT,	RESULT_RANK_NO_10_IMG_RECT,	RESULT_RANK_NO_11_IMG_RECT,	RESULT_RANK_NO_12_IMG_RECT,
	RESULT_RANK_NO_13_IMG_RECT,	RESULT_RANK_NO_14_IMG_RECT,	RESULT_RANK_NO_15_IMG_RECT,	RESULT_RANK_NO_16_IMG_RECT,
	RESULT_RANK_NO_17_IMG_RECT,	RESULT_RANK_NO_18_IMG_RECT,	RESULT_RANK_NO_19_IMG_RECT,	RESULT_RANK_NO_20_IMG_RECT,
};

BOOL SessionNo_Asc( const type_session* left, const type_session* right )
{
	// Noが同じ値の場合、生存ターン数で並べる
	if (left->frame_count == right->frame_count)
		return ((left->live_count) >= (right->live_count));
	return ((left->frame_count) < (right->frame_count));
}

void CGame::CreateResultRank()
{
	CDXUTSprite* pSprite = NULL;
	CDXUTButton* pBtn = NULL;
	CDXUTStatic* pStatic = NULL;
	CDXUTScrollBar* pScrollBar = NULL;
//	cmpByCriterion cmp;
	RECT rcTexture;
	pScrollBar = p_pUI->GetScrollBar(IDC_RESULT_SB_RANK);
#if 0
	m_nRankItemCount = MAXUSERNUM;
#else
	m_nRankItemCount = 0;

	m_vecCharacters.clear();
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_SessionArray[i].entity)
		{
			m_nRankItemCount++;
			m_vecCharacters.push_back(&m_SessionArray[i]);
		}
	}
	ptype_session* pSort = new ptype_session[m_nRankItemCount];
	
	int i=0;
	for (std::vector<ptype_session>::iterator it = m_vecCharacters.begin();
		it != m_vecCharacters.end();
		it++)
	{
		pSort[i] = (*it);
		i++;
	}

	for (i=0;i<m_nRankItemCount;i++)
	{
		for (int j=i+1;j<m_nRankItemCount;j++)
		{
			// Noが同じ値の場合、生存ターン数で並べる
			if (pSort[i]->frame_count == pSort[j]->frame_count)
			{
				if ((pSort[i]->live_count) < (pSort[j]->live_count))
				{
					ptype_session temp = pSort[i];
					pSort[i] = pSort[j];
					pSort[j] = temp;
				}
			}
			else if (pSort[i]->frame_count > pSort[j]->frame_count)
			{
				ptype_session temp = pSort[i];
				pSort[i] = pSort[j];
				pSort[j] = temp;
			}
		}
	}

#endif

#if 0
#else
	if ( (int)m_vecCharacters.size() < RESULT_RANK_VIEW_RANGE)
		pScrollBar->SetVisible(false);
	else
#endif
		pScrollBar->SetVisible(true);

	int nOffsetX = 0;
	int nNameWidthAdd = 0;
	if (m_nTeamCount > 1)
	{
		nOffsetX = RESULT_STATIC_TEAM_CNT_W;
		nNameWidthAdd = -RESULT_STATIC_TEAM_CNT_W;

	}
	int nIndex = 0;
	for (i=0;i<m_nRankItemCount;i++)
	{
		TCHARA_SCR_INFO* pCharaScrInfo = (TCHARA_SCR_INFO*)pSort[i]->scrinfo;
		int nNo = pSort[i]->frame_count;
		int nTeamNo = pSort[i]->team_no;
		WCHAR name[MAX_USER_NAME+1];
		common::session::GetSessionName(pSort[i], name);
		p_pUI->AddSprite(IDC_RESULT_SPRITE_NO_BASE+nIndex,RESULT_SPRITE_RANK_NO_CNT_X_BASE, RESULT_SPRITE_RANK_NO_CNT_Y_BASE+((RESULT_SPRITE_RANK_NO_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*nNo), RESULT_SPRITE_RANK_NO_CNT_W, RESULT_SPRITE_RANK_NO_CNT_H, false, &pSprite);
		CopyRect(&rcTexture, &c_rcRankNoImgRectTable[nNo]);
		pSprite->GetElement(0)->SetTexture(m_nDefaultGUIResourceIndex, &rcTexture);

		p_pUI->AddSprite(IDC_RESULT_SPRITE_ICON_BASE+nIndex,RESULT_SPRITE_ICON_CNT_X_BASE, RESULT_SPRITE_ICON_CNT_Y_BASE+((RESULT_SPRITE_ICON_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*nNo), RESULT_SPRITE_ICON_CNT_W, RESULT_SPRITE_ICON_CNT_H, false, &pSprite);
		SetRect(&rcTexture, pCharaScrInfo->rec_sel_icon.left, pCharaScrInfo->rec_sel_icon.top, pCharaScrInfo->rec_sel_icon.right, pCharaScrInfo->rec_sel_icon.bottom);
		pSprite->GetElement(0)->SetTexture(pCharaScrInfo->res_index, &rcTexture);

		// チーム番号	
		if (m_nTeamCount > 1)
		{
			p_pUI->AddStatic(IDC_RESULT_STATIC_TEAM_BASE+nIndex, m_pTeamSeparater->GetTeamWchar(nTeamNo), RESULT_STATIC_TEAM_CNT_X_BASE, RESULT_STATIC_TEAM_CNT_Y_BASE+((RESULT_STATIC_TEAM_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*nNo), RESULT_STATIC_TEAM_CNT_W, RESULT_STATIC_TEAM_CNT_H, false, &pStatic);
			pStatic->GetElement(0)->SetFont(4,m_pTeamSeparater->GetTeamColor(nTeamNo), DT_CENTER|DT_SINGLELINE|DT_VCENTER);
		}

		p_pUI->AddStatic(IDC_RESULT_STATIC_NAME_BASE+nIndex, name, RESULT_STATIC_NAME_CNT_X_BASE+nOffsetX, RESULT_STATIC_NAME_CNT_Y_BASE+((RESULT_STATIC_NAME_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*nNo), RESULT_STATIC_NAME_CNT_W+nNameWidthAdd, RESULT_STATIC_NAME_CNT_H, false, &pStatic);
		pStatic->GetElement(0)->SetFont(4,0xFFFFFFFF, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

		nIndex++;
	}

	SafeDeleteArray(pSort);

	SetResultText();
	UpdateRankView();
	
}


// 結果テキスト表示
void CGame::SetResultText()
{
	int nResultType = CHARA_FACE_FINE;
	int nMaxNo = 0;
	ptype_session sess = &m_SessionArray[m_nUserIndex];

	if (GetMySessionInfo()->team_no != GALLERY_TEAM_NO)
	{
		// 最低順位を取得
		for (std::vector<ptype_session>::iterator it = m_vecCharacters.begin();
			it != m_vecCharacters.end();
			it++)
			nMaxNo = max(nMaxNo, (*it)->frame_count);
		// 最低順位が3位以上
		if (nMaxNo >= 2)
		{
			float fRankBorderA = (((float)nMaxNo+1.0f) / 3.0f) * 1.0f;		// 結果A
			float fRankBorderB = (((float)nMaxNo+1.0f) / 3.0f) * 2.0f;		// 結果B

			if (m_SessionArray[m_nUserIndex].frame_count+1 > fRankBorderA)
				nResultType = CHARA_FACE_NORMAL;
			if (m_SessionArray[m_nUserIndex].frame_count+1 > fRankBorderB)
				nResultType = CHARA_FACE_HURT;
		}
		else
		{
			if (sess->frame_count != 0)
				nResultType = CHARA_FACE_HURT;
		}
	}
	
	TCHARA_SCR_INFO* pCharaScrInfo = (TCHARA_SCR_INFO*)sess->scrinfo;

	RECT rcTexture;
	LuaFuncParam luaParams, luaResults;
	luaParams.Number(sess->scrinfo->scr_index).Number(nResultType);
	common::scr::CallLuaFunc(g_pLuah, "getChara_Face", &luaResults, 4, &luaParams, g_pCriticalSection);
	rcTexture.left = (int)luaResults.GetNumber(0);
	rcTexture.top = (int)luaResults.GetNumber(1);
	rcTexture.right = rcTexture.left + (int)luaResults.GetNumber(2);
	rcTexture.bottom = rcTexture.top + (int)luaResults.GetNumber(3);

	CDXUTElement* pElement = p_pUI->GetControl(IDC_RESULT_SPRITE_FACE)->GetElement(0);
	pElement->iTexture = sess->scrinfo->res_index;
	CopyRect(&(pElement->rcTexture), &rcTexture);

	luaResults.Clear();
//	luaParams.Number(sess->scrinfo->scr_index).Number(nResultType);
	common::scr::CallLuaFunc(g_pLuah, "getChara_ResultMessage", &luaResults, 1, &luaParams, g_pCriticalSection);
	WCHAR* wstr = NULL;
	int wstr_len = 0;
	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDeleteArray(wstr);
		MessageBox(g_hWnd, wstr, L"lua", MB_OK);
		return;
	}
	p_pUI->GetStatic(IDC_RESULT_STATIC_TEXT)->SetText(wstr);
	SafeDeleteArray(wstr);
	switch (nResultType)
	{
	case CHARA_FACE_FINE:		g_pGame->PlaySysSoundSE(SE_sri_ClapA); break;
	case CHARA_FACE_NORMAL:	g_pGame->PlaySysSoundSE(SE_sri_ClapB); break;
	case CHARA_FACE_HURT:		g_pGame->PlaySysSoundSE(SE_sri_ClapC); break;
	}

}


BOOL CGame::CreateConfigGUI()
{
	CDXUTButton*		pBtn = NULL;
	CDXUTSprite*		pSprite = NULL;
	CDXUTCheckBox*	pChkBox = NULL;
	CDXUTSlider*		pSlider = NULL;
	CDXUTStatic*		pStatic = NULL;
	RECT rcTexture;

	p_pConfig->SetLocation(0,0);
	p_pConfig->SetSize(WIN_WIDTH, WIN_HEIGHT);
	p_pConfig->SetTexture(m_nDefaultGUIResourceIndex, IMG_GUI_SKIN);

	p_pConfig->AddSprite( IDC_CONFIG_SPRITE_BGM, CONFIG_SPRITE_BGM_CNT_RECT, false, &pSprite);
	SetRect( &rcTexture, CONFIG_SPRITE_BGM_IMG_RECT);	// 描画範囲	
	pSprite->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture);
/*
	// OK BUTTON
	p_pConfig->AddButton( IDC_CONFIG_BTN_OK, L"",
		CONFIG_BTN_OK_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
	pBtn->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(1,1);
	SetRect( &rcTexture, CONFIG_BTN_OK_IMG_RECT);	// 描画範囲	
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );
*/
	// CLOSE BUTTON
	p_pConfig->AddButton( IDC_CONFIG_BTN_CLOSE, L"close",
		CONFIG_BTN_CLOSE_CNT_RECT,
		0, false, &pBtn );
	pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
	pBtn->GetElement(1)->TextureColor.Init(0xFFFFFFFF, 0xFFFFFFFF);
	pBtn->SetStateMouserOverOffset(0,0);
	pBtn->SetStatePressedOverOffset(1,1);
	SetRect( &rcTexture, CONFIG_BTN_CLOSE_IMG_RECT);	// 描画範囲	
	pBtn->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );

	// EFFECT ON/OFF
	p_pConfig->AddCheckBox( IDC_CONFIG_CHK_EFFECT,
		CONFIG_CHK_EFFECT_TEXT,
		CONFIG_CHK_EFFECT_CNT_RECT,
		m_bEffectEnable?true:false, 0, false, &pChkBox );
	SetRect( &rcTexture, CONFIG_CHK_EFFECT_IMG_ON_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, CONFIG_CHK_EFFECT_IMG_OFF_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->SetDrawShadow(false);
	pChkBox->GetElement(0)->SetFont(1, 0xFFFFFFFF, DT_LEFT|DT_VCENTER);

	// BLT FOCUS ON/OFF
	p_pConfig->AddCheckBox( IDC_CONFIG_CHK_BLT_FOCUS,
		CONFIG_CHK_BLT_FOCUS_TEXT,
		CONFIG_CHK_BLT_FOCUS_CNT_RECT,
		m_bBulletFocus, 0, false, &pChkBox );
	SetRect( &rcTexture, CONFIG_CHK_BLT_FOCUS_IMG_ON_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, CONFIG_CHK_BLT_FOCUS_IMG_OFF_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->SetDrawShadow(false);
	pChkBox->GetElement(0)->SetFont(1, 0xFFFFFFFF, DT_LEFT|DT_VCENTER);

	// ACT FOCUS ON/OFF
	p_pConfig->AddCheckBox( IDC_CONFIG_CHK_ACT_FOCUS,
		CONFIG_CHK_ACT_FOCUS_TEXT,
		CONFIG_CHK_ACT_FOCUS_CNT_RECT,
		m_bActChrFocus, 0, false, &pChkBox );
	SetRect( &rcTexture, CONFIG_CHK_ACT_FOCUS_IMG_ON_RECT);	// 描画範囲
	pChkBox->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	SetRect( &rcTexture, CONFIG_CHK_ACT_FOCUS_IMG_OFF_RECT);	// 描画範囲
	pChkBox->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	pChkBox->SetDrawShadow(false);
	pChkBox->GetElement(0)->SetFont(1, 0xFFFFFFFF, DT_LEFT|DT_VCENTER);


	// BGM VOLUME SLIDER
	p_pConfig->AddSlider( IDC_CONFIG_SLIDER_BGM, 
		CONFIG_SLIDER_BGM_CNT_RECT, 0, 100, m_bytBGMVolume, false, &pSlider);
	// SLIDER TRACK
	SetRect( &rcTexture, CONFIG_SLIDER_BGM_IMG_TRK_RECT);	// 描画範囲
	pSlider->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	// SLIDER BUTTON
	SetRect( &rcTexture, CONFIG_SLIDER_BGM_IMG_BTN_RECT);	// 描画範囲
	pSlider->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定

	// SE VOLUME SLIDER
	p_pConfig->AddSlider( IDC_CONFIG_SLIDER_SE, 
		CONFIG_SLIDER_SE_CNT_RECT, 0, 100, m_bytSEVolume, false, &pSlider);
	// SLIDER TRACK
	SetRect( &rcTexture, CONFIG_SLIDER_SE_IMG_TRK_RECT);	// 描画範囲
	pSlider->GetElement(0)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	// SLIDER BUTTON
	SetRect( &rcTexture, CONFIG_SLIDER_SE_IMG_BTN_RECT);	// 描画範囲
	pSlider->GetElement(1)->SetTexture( m_nDefaultGUIResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定

	// FRAME CAPTION
	p_pConfig->AddStatic( IDC_CONFIG_STATIC_CAPTION, L"CONFIG MENU",
		CONFIG_STATIC_CAPTION_CNT_RECT, false, &pStatic);
	pStatic->GetElement(0)->SetFont(1);
	pStatic->SetTextColor(0xFFFFFFFF);
	pStatic->SetDrawShadow(false);

	// BGM VOLUME CAPTION
	p_pConfig->AddStatic( IDC_CONFIG_STATIC_BGM_TEXT, L"BGM音量",
		CONFIG_STATIC_BGM_TEXT_CNT_RECT, false, &pStatic);
	pStatic->GetElement(0)->SetFont(1);
	pStatic->SetTextColor(0xFFFFFFFF);
	pStatic->SetDrawShadow(false);

	// SE VOLUME CAPTION
	p_pConfig->AddStatic( IDC_CONFIG_STATIC_SE_TEXT, L"SE音量",
		CONFIG_STATIC_SE_TEXT_CNT_RECT, false, &pStatic);
	pStatic->GetElement(0)->SetFont(1);
	pStatic->SetTextColor(0xFFFFFFFF);
	pStatic->SetDrawShadow(false);

	p_pConfig->SetVisible(false);
	return TRUE;
}

// システム音声読込み
BOOL CGame::LoadSystemSound()
{
	m_pSysSoundLibs->Clear();
	m_pSoundPlayer->ClearPlayingList();
	m_mapSysSoundIDHash.clear();
	LoadSound(SE_sub_Login, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sub_Arrow, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sub_StageSel, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sub_ChangeView, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sur_ListSel, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_suc_OFF, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_suc_ON, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sai_SrvInfo, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sai_Chat, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sai_Count, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_MyTurn, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgr_BltSel, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Repair, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Double, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Stealth, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Blind, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Pain, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Shield, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_NoMove, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_ItemUse, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgr_Pass, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sgi_Shot, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sri_ClapA, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sri_ClapB, m_pSysSoundLibs, &m_mapSysSoundIDHash);
	LoadSound(SE_sri_ClapC, m_pSysSoundLibs, &m_mapSysSoundIDHash);

	return TRUE;
}

// スクリプト音声読込み
BOOL CGame::LoadScriptSound()
{
	std::map<int, TBASE_SCR_INFO*> mapLoadedCharacters;
	
	for (std::vector< type_session* >::iterator it = m_vecCharacters.begin();
		it != m_vecCharacters.end();
		it++)
	{
		// スクリプトの音声読込み済み
		if (mapLoadedCharacters.find((*it)->scrinfo->scr_index) != mapLoadedCharacters.end())
			continue;
		
		int nScrIndex = (*it)->scrinfo->scr_index;
		LuaFuncParam luaParams,luaResults;
		luaParams.Number(nScrIndex);
		common::scr::CallLuaFunc(g_pLuah, "getChara_SEFilesCount", &luaResults, 1, &luaParams, g_pCriticalSection);
		int nSECount = (int)luaResults.GetNumber(0);

		WCHAR* wsSEPath;
		int wsSEPathLen = 0;
		for (int i=0;i<nSECount;i++)
		{
			luaParams.Clear();
			luaResults.Clear();
			luaParams.Number(nScrIndex).Number(i);
			common::scr::CallLuaFunc(g_pLuah, "getChara_SEFile", &luaResults, 1, &luaParams, g_pCriticalSection);
			char csSEPath[_MAX_PATH*2+1];
			SafePrintfA(csSEPath, _MAX_PATH*2, luaResults.GetString(0));

			if ( !luaResults.GetWString(0, &wsSEPath, &wsSEPathLen) )
			{
				MessageBox(g_hWnd, wsSEPath, L"lua", MB_OK);
				SafeDeleteArray(wsSEPath);
				continue;;
			}
			int nResID = 0;
			if ((nResID = m_pScrSoundLibs->AddFromFile(wsSEPath)) == -1)
				MessageBox(g_hWnd, wsSEPath, L"Sound Load Error", MB_OK);
			else
			{
				std::string stemp(csSEPath);
				m_mapScrSoundIDHash.insert(std::map<std::string, int>::value_type(stemp, nResID));
			}
//			LoadSound(wsSEPath, m_pScrSoundLibs, &m_mapScrSoundIDHash);
			SafeDeleteArray(wsSEPath);
		}
		mapLoadedCharacters.insert(std::map<int, TBASE_SCR_INFO* >::value_type( (*it)->scrinfo->scr_index, (*it)->scrinfo ));
	}
	return TRUE;
}



void CGame::CreateItemDetailControlSize()
{
	RECT rcText;
	SetRect(&rcText, 0,0,200,24);
	DXUTFontNode* pFontNode = p_pUI->GetFont(2);
	HDC hDC = pFontNode->pFont->GetDC();

	for (int i=0;i<GAME_ITEM_COUNT;i++)
	{
		const WCHAR *wsText = c_tItemResouceInfoTable[i].text;
		int nTextLen = _tcslen(wsText);
		DrawText( hDC, wsText, nTextLen,  &rcText,  DT_INTERNAL|DT_EDITCONTROL|DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		rcText.right += 4;
		CopyRect(&m_rcItemDetailControlSize[i], &rcText);
	}
	ReleaseDC(DXUTGetHWND(), hDC);	
}
