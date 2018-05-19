#include "ext.h"
#include "Game.h"

// -------------------------------------------------------------------
// Class - CGame
// -------------------------------------------------------------------
// #コンストラクタ
CGame::CGame()
{
	m_pD3D= NULL;
	m_pDev = NULL;
	m_pDXFont = NULL;
//	m_pGraph = NULL;
#if USE_DINPUT
	m_pInput	= NULL;
#endif
	m_eSysState = eSysInit;
///////////////////////////////////////
	m_eGameState= eGameTitleInit;

	m_bCreated = FALSE;
	m_bActive = TRUE;

	p_pL = NULL;
	p_pLuah = NULL;
	p_pUI = NULL;

	ZeroMemory(&m_tKeyState, sizeof(TInputState));

//===========================可変============================//
//**定義した変数等の初期値
//	ContLogClear();
	m_nTcpSock = 0;
	m_nUserIndex = -1;
	m_nTeamCount = 0;
	m_nAuthedCount = 0;
	m_bytRuleFlg = GAME_RULE_DEFAULT;
	m_bMouseLButtonDown = FALSE;

	m_nSavedInputX = 0;
	m_nSavedInputY = 0;
	m_nSavedMouseX = 0;
	m_nSavedMouseY = 0;

	m_nMaxLoginNum = 0;

	m_eConnState = CONN_STATE_EMPTY;

	m_nDefaultGUIResourceIndex = -1;
	m_pDefaultGUIResourceTexture = NULL;
	m_pBGTexture = NULL;
	ZeroMemory(m_pBGTexturePath, sizeof(WCHAR)*(_MAX_PATH*2+1));
	m_nStageIndex = -1;
	m_pSelectedStageScrInfo = NULL;

	ZeroMemory(m_rcItemDetailControlSize, sizeof(RECT)*GAME_ITEM_COUNT);
	ZeroMemory(m_SessionArray, sizeof(type_session)*MAXLOGINUSER);
	ZeroMemory(m_pRoomCharacters, sizeof(CRoomCharacter*)*MAXLOGINUSER);
	ZeroMemory(m_pStageCharacters, sizeof(CStageCharacter*)*MAXLOGINUSER);

	m_pSysSoundLibs = NULL;
	m_pScrSoundLibs = NULL;
	m_pSoundPlayer = NULL;

	m_pMyItemBtnList = new CDXUTButtonList();
	m_pCharacterBtnList = new CDXUTButtonList();
	m_pItemBtnList = new CDXUTButtonList();

	m_pTeamRulePropertyManager = NULL;
	m_bytRuleFlg = GAME_RULE_DEFAULT;

	m_pTeamSeparater = NULL;
	m_pStageTexture = NULL;
	m_pStageBGTexture = NULL;
	m_pMainStage = new CMainStage();
	m_pMeterMovable = NULL;
	m_pMeterWindL = NULL;
	m_pMeterWindR = NULL;
	m_pMeterSCExp = NULL;
	m_pRbSC = NULL;
	m_pShotAngle = NULL;
	m_pASpriteTimer = NULL;
	m_pASpriteTimerBG = NULL;
	m_nActiveObjectNo = -1;
	m_pShotAngleIndicator = NULL;

	m_pSpinTurnLimit001 = NULL;
	m_pSpinTurnLimit010 = NULL;
	m_pSpinTurnLimit100 = NULL;
	m_nLimitTurn = 0;

	SetRectEmpty(&m_rcDrawStage);
	SetRectEmpty(&m_rcDrawBGStage);
	m_vecStageViewTrans = D3DXVECTOR3(0,0,0);

	m_hThreadLoading = NULL;
	m_nChatSendInterval = 0;

	m_bytSEVolume = 100;
	m_bytBGMVolume = 100;
	m_nBGMSoundID = -1;
	m_nBGMPlayingID = -1;
	m_bSavePass = FALSE;

	m_pIniConfig = NULL;
	m_nTest = 0;
	m_nWindValue = 0;

	m_bDeviceLost = FALSE;
	m_pSelBulletBtnList = NULL;

	m_bChatFocusFlg = FALSE;
	m_nActiveCharaFocusCounter = 0;
	m_vecActiveCharaFocusStartPos = D3DXVECTOR3(0,0,0);
	m_nMyCharaFocusCounter = 0;
	m_vecMyCharaFocusStartPos = D3DXVECTOR3(0,0,0);

	m_pShotPowerIndicator = NULL;
	m_bBulletFocus = true;
	m_bActChrFocus = true;
	m_pFocusObject = NULL;
	m_nNotifyMyTurn = 0;
	m_bReqLoadComplete = FALSE;
	m_bPauseBulletFocus = FALSE;
	m_bScrFocus = FALSE;

	m_bStageVisible = TRUE;
	m_nStageFadeCounter = GAME_STAGE_FADE_TIME;

	// 弾発射用に記憶
	m_nShootingAngle = 0;
	m_nShootingPower = 0;	
	m_nShootingProcType = 0;
	m_nShootingBltType = 0;
	m_nShootingCharaObjNo = 0;
	m_nShootingIndicatorAngle = 0;
	m_nShootingIndicatorPower = 0;	

	m_nActiveTimeCounter = 0;

	m_nTurnCounter = 0;

	m_nMousePosX = 0;
	m_nMousePosY = 0;
	m_nActTimeLimit = GAME_TURN_ACT_COUNT;

	m_wsRecvScrPath[0] = NULL;
	m_bLoginStart = FALSE;
//===========================================================//
}

// #デストラクタ
CGame::~CGame()
{
	m_bCreated = FALSE;
//===========================可変============================//
//**定義した変数等の破棄
	
	// ロードスレッドが動作中か
	OutputDebugStr(L"Thread");
	DWORD dwParam;
	if (GetExitCodeThread(m_hThreadLoading, &dwParam))
	{
		if (dwParam == STILL_ACTIVE) 
		{
			TerminateThread(m_hThreadLoading, FALSE);
			CloseHandle(m_hThreadLoading);
		}
	}
	m_hThreadLoading = NULL;

	OutputDebugStr(L"Disconnect");
	Disconnect();

	m_mapCharaScrInfo.clear();
	m_mapStageScrInfo.clear();
	DestroyCharacters();
	if (g_pCriticalSection)
		g_pCriticalSection->EnterCriticalSection_Object(L'0');
	ClearStageObjects();
	if (g_pCriticalSection)
		g_pCriticalSection->LeaveCriticalSection_Object();

	if (g_pCriticalSection)
		g_pCriticalSection->EnterCriticalSection_Effect(L'Q');
	ClearStageEffects();
	ClearBGEffects();
	if (g_pCriticalSection)
		g_pCriticalSection->LeaveCriticalSection_Effect();

	OutputDebugStr(L"Del:m_pSelBulletBtnList\n");
	SafeDelete(m_pSelBulletBtnList);
	SafeDelete(m_pShotAngle);
	SafeDelete(m_pShotPowerIndicator);
	SafeDelete(m_pShotAngleIndicator);
	SafeDelete(m_pTeamSeparater);
	SafeDelete(m_pTeamRulePropertyManager);
	SafeDelete(m_pMyItemBtnList);
	SafeDelete(m_pCharacterBtnList);
	SafeDelete(m_pItemBtnList);
	SafeDelete(m_pMainStage);
	SafeDelete(m_pIniConfig);
	OutputDebugStr(L"Del:m_pSpinTurnLimit001\n");

	SafeDelete(m_pSpinTurnLimit001);
	SafeDelete(m_pSpinTurnLimit010);
	SafeDelete(m_pSpinTurnLimit100);

//===========================================================//
	if (g_pCriticalSection)
		g_pCriticalSection->EnterCriticalSection_Sound('_');
	OutputDebugStr(L"Del:m_pSysSoundLibs\n");
	SafeDelete(m_pSysSoundLibs);
	OutputDebugStr(L"Del:m_pScrSoundLibs\n");
	SafeDelete(m_pScrSoundLibs);
	OutputDebugStr(L"Clear:m_pSoundPlayer\n");
	if (m_pSoundPlayer)
		m_pSoundPlayer->ClearPlayingList();
	OutputDebugStr(L"Del:m_pSoundPlayer\n");
	SafeDelete(m_pSoundPlayer);
	if (g_pCriticalSection)
		g_pCriticalSection->LeaveCriticalSection_Sound();


	OutputDebugStr(L"Del:m_pStageTexture\n");
	SafeRelease(m_pStageTexture);
	OutputDebugStr(L"Del:m_pStageBGTexture\n");
	SafeRelease(m_pStageBGTexture);
	OutputDebugStr(L"Del:m_pBGTexture\n");
	SafeRelease(m_pBGTexture);

#if USE_DINPUT
	SafeDelete(m_pInput);
#endif
	SafeRelease(m_pDXFont);
}

void CGame::CreateCharacters()
{
	DestroyCharacters();
	for (int i=0;i<GetMaxLoginNum();i++)
		m_pRoomCharacters[i] = new CRoomCharacter();
	for (int i=0;i<GetMaxLoginNum();i++)
		m_pStageCharacters[i] = new CStageCharacter();
}

void CGame::DestroyCharacters()
{
	for (int i=0;i<MAXLOGINUSER;i++)
		SafeDelete(m_pRoomCharacters[i]);
	for (int i=0;i<MAXLOGINUSER;i++)
		SafeDelete(m_pStageCharacters[i]);
}

BOOL CGame::OnCreateDevice(HWND hWnd,HINSTANCE hInst)
{
	m_hWnd = hWnd;
	m_hInst= hInst;
	// Direct3Dオブジェクトの取得	
	m_pD3D = DXUTGetD3DObject();
	m_pDev = DXUTGetD3DDevice();
	return TRUE;
}

// 生成
BOOL CGame::Create()
{
	if(m_pD3D == NULL)
		return FALSE;
	// 初期化
	// ライトの情報を設定
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.a = 1.0f; light.Ambient.a = 0.0f; light.Specular.a = 0.75f;
	light.Diffuse.r = 1.0f; light.Ambient.r = 0.0f; light.Specular.r = 0.75f;
	light.Diffuse.g = 1.0f; light.Ambient.g = 0.0f; light.Specular.g = 0.75f;
	light.Diffuse.b = 1.0f; light.Ambient.b = 0.0f; light.Specular.b = 0.75f;
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3(-0.4f, 0.4f, 0.8f) );
	m_pDev->SetLight(0, &light);			// ライトを設定
	m_pDev->LightEnable(0, TRUE);
	m_pDev->SetRenderState( D3DRS_LIGHTING, TRUE);
	// 投影マトリックスを設定する
	D3DXMATRIX matProj;
	RECT rcWindow = DXUTGetWindowClientRect();
	FLOAT	   fAspect = (float) ((rcWindow.right-rcWindow.left)/(rcWindow.bottom-rcWindow.top));
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 1000.0f );
	m_pDev->SetTransform( D3DTS_PROJECTION, &matProj );
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	// カリングモード

	m_pSelBulletBtnList = new CDXUTButtonList();

	m_pSoundPlayer = new CSoundPlayer();
	m_pSoundPlayer->Initialize(DXUTGetHWND(), DSSCL_PRIORITY);
	m_pSoundPlayer->SetPrimaryBufferFormat();
	// 音読み込みクラス生成
	m_pSysSoundLibs = new CSoundLibraries(m_pSoundPlayer->GetDirectSound());
	m_pScrSoundLibs = new CSoundLibraries(m_pSoundPlayer->GetDirectSound());

	m_pSpinTurnLimit001 = new CSpinButton();
	m_pSpinTurnLimit010 = new CSpinButton();
	m_pSpinTurnLimit100 = new CSpinButton();

#if	USE_DINPUT
	m_pInput = new CGameControl();
	if (!(m_pInput->Create(hInst, hWnd, TRUE, TRUE)))
		m_pInput = NULL;
#endif

	m_rcDrawStage = DXUTGetWindowClientRect();
	m_rcDrawStage.bottom -= MAIN_CONTROLPANEL_CNT_H_MIN;
	m_rcDrawBGStage = DXUTGetWindowClientRect();
	m_rcDrawBGStage.bottom -= MAIN_CONTROLPANEL_CNT_H_MIN;

//===========================可変============================//
//*初期化
#if USE_DEBUG_FILER
	m_pFiler = new CFiler();
#else
//	m_pFiler = new CZFiler();
//	((CZFiler*)m_pFiler)->AddFileList(ZIP_DATA);
#endif
	m_pIniConfig = new CIniFile();
	m_bSavePass = m_pIniConfig->ReadBool(L"LOGIN", L"SAVE_PASS", FALSE)?true:false;
	m_nPortNo = m_pIniConfig->ReadInt(L"CONFIG", L"PORT", MY_TCP_PORT);
	m_bytBGMVolume = (BYTE)m_pIniConfig->ReadInt(L"CONFIG", L"BGM", 90);
	m_bytSEVolume = (BYTE)m_pIniConfig->ReadInt(L"CONFIG", L"SE", 90);
	m_bEffectEnable = m_pIniConfig->ReadBool(L"CONFIG", L"EFFECT", TRUE);
	m_bBulletFocus = m_pIniConfig->ReadBool(L"CONFIG", L"BLT_FOCUS", TRUE)?true:false;
	m_bActChrFocus = m_pIniConfig->ReadBool(L"CONFIG", L"ACT_FOCUS", TRUE)?true:false;
	
	AddMessageLog(L">Game::Create()::LoadScript()");

	if (!common::scr::LoadLoaderScript(g_L, LUA_FILE))
	{
		DXTRACE_MSG(L"error:LoadLoaderScript()");
		return FALSE;
	}

	AddMessageLog(L">Game::Create()::LoadSound()");
	if (!LoadSystemSound())
	{
		DXTRACE_MSG(L"LoadSystemResouce");
		return FALSE;
	}

	AddMessageLog(L">Game::Create()::CreateGUI()");
	if ( !CreateGUI() )
	{
		DXTRACE_MSG(L"CreateGUI()");
		return FALSE;
	}
//===========================================================//
	m_bCreated = TRUE;
	return TRUE;
}

// Keyイベント
void CALLBACK CGame::OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown)
{
	switch( nChar )
	{
		case VK_SPACE:
			g_pCriticalSection->EnterCriticalSection_Input(L'2');
			g_pCriticalSection->EnterCriticalSection_StageTexture(L'2');
			if (bKeyDown)
			{
				if (m_tKeyState.bytButtons[0] & KEY_BTN_ON)
					m_tKeyState.bytButtons[0] = KEY_BTN_ON;
				else
					m_tKeyState.bytButtons[0] = KEY_BTN_ON|KEY_BTN_CHANGE;
			}
			else
			{
				if (m_tKeyState.bytButtons[0] & KEY_BTN_ON)
					m_tKeyState.bytButtons[0] = KEY_BTN_CHANGE;
				else
					m_tKeyState.bytButtons[0] = 0;
			}
			g_pCriticalSection->LeaveCriticalSection_StageTexture();
			g_pCriticalSection->LeaveCriticalSection_Input();
			break;
		case VK_UP:
			g_pCriticalSection->EnterCriticalSection_Input(L'3');
			if (bKeyDown)
				m_tKeyState.lY = -KEY_CONT_ON;
			else if (GetAsyncKeyState(VK_DOWN)&0x8000)
				m_tKeyState.lY = KEY_CONT_ON;
			else
			{
				m_bContYOnDown = FALSE;
				m_tKeyState.lY = 0;
			}
			g_pCriticalSection->LeaveCriticalSection_Input();
			break;
		case VK_DOWN:
			g_pCriticalSection->EnterCriticalSection_Input(L'4');
			if (bKeyDown)
				m_tKeyState.lY = KEY_CONT_ON;
			else if (GetAsyncKeyState(VK_UP)&0x8000)
				m_tKeyState.lY = -KEY_CONT_ON;
			else
			{
				m_bContYOnDown = FALSE;
				m_tKeyState.lY = 0;
			}
			g_pCriticalSection->LeaveCriticalSection_Input();
			break;
		case VK_LEFT:
			g_pCriticalSection->EnterCriticalSection_Input(L'5');
			if (bKeyDown)
				m_tKeyState.lX = -KEY_CONT_ON;
			else if (GetAsyncKeyState(VK_RIGHT)&0x8000)
				m_tKeyState.lX = KEY_CONT_ON;
			else
				m_tKeyState.lX = 0;
			g_pCriticalSection->LeaveCriticalSection_Input();
			break;
		case VK_RIGHT:
			g_pCriticalSection->EnterCriticalSection_Input(L'6');
			if (bKeyDown)
				m_tKeyState.lX = KEY_CONT_ON;
			else if (GetAsyncKeyState(VK_LEFT)&0x8000)
				m_tKeyState.lX = -KEY_CONT_ON;
			else
				m_tKeyState.lX = 0;
			
			g_pCriticalSection->LeaveCriticalSection_Input();
			break;
		case VK_RETURN:
			if (bKeyDown
			&& !m_bChatFocusFlg
			&& !p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->m_bHasFocus
			&& (GetAsyncKeyState(VK_LSHIFT)&0x8000|GetAsyncKeyState(VK_RSHIFT)&0x8000))
			{
				m_bChatFocusFlg = TRUE;
			}
			else if (!bKeyDown
			&& m_bChatFocusFlg
			&& !p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->m_bHasFocus
			&& (GetAsyncKeyState(VK_LSHIFT)&0x8000|GetAsyncKeyState(VK_RSHIFT)&0x8000))
			{
				p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->GetElement(0)->TextureColor.States[DXUT_STATE_NORMAL] = 0xFFFFFFFF;
				p_pUI->RequestFocus( p_pUI->GetIMEEditBox(IDC_SHARE_EDIT));
				m_bChatFocusFlg = FALSE;
				if (m_eGameState & GAME_STATE_MAIN)
				{
					CDXUTControl *pChatLogBG = p_pUI->GetControl(IDC_MAIN_SPRITE_CHATLOG_BG);
					if (!pChatLogBG->GetVisible())
						pChatLogBG->SetVisible(true);
				}
			}
			else if (bKeyDown
			&& !m_bChatFocusFlg
			&& p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->m_bHasFocus
			&& ((GetAsyncKeyState(VK_CONTROL)&0x8000)||(GetAsyncKeyState(VK_MENU)&0x8000)))
			{
				CDXUTControl* pControl = p_pUI->GetIMEEditBox(IDC_SHARE_EDIT);
				OnGUIEvent( EVENT_EDITBOX_STRING, pControl->GetID(), pControl, NULL );
			}
			break;
		default:
			break;
	}
}

// GUIイべント
void CALLBACK CGame::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	switch (m_eGameState)
	{
	// == Title == //
	case eGameTitle:				// タイトル
		GUITitle(nEvent, nControlID, pControl, pUserContext);
		break;
	case eGameLogin:				// ログイン
		GUILogin(nEvent, nControlID, pControl, pUserContext);
		break;
	case eGameRoom:				// 部屋
		GUIRoom(nEvent, nControlID, pControl, pUserContext);
		break;
	case eGameLoad:				// ロード
		GUILoad(nEvent, nControlID, pControl, pUserContext);
		break;
	case eGameMain:				// メイン
		GUIMain(nEvent, nControlID, pControl, pUserContext);
		break;
	case eGameResult:				// メイン
		GUIResult(nEvent, nControlID, pControl, pUserContext);
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////
// #メイン・ループ
///////////////////////////////////////////////////////////////
BOOL CGame::Game()
{
	if (!m_bCreated) return TRUE;
//	if (!m_bActive) return TRUE;

#if	USE_DINPUT
	if (m_pInput)
		m_pInput->UpdateState();
#endif

	switch (m_eSysState)
	{
	case eSysInit:	// 初期化
//===========================可変============================//
//** 初期化
		if (!Init())	return FALSE;
		m_eSysState = eSysRun;
//===========================================================//
		break;

	case eSysRun:	// メイン
//===========================可変============================//
//** メイン処理
		if (!Run(m_eGameState))
			return FALSE;
//===========================================================//
		break;

	case eSysExit:	// 終わり
//===========================可変============================//
//** 終わり処理
//		if (!Release()) return FALSE;
//===========================================================//
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------
// #Game > Init
//-------------------------------------------------------------
BOOL CGame::Init()
{
//===========================可変============================//
//** ゲーム用意
	srand(timeGetTime()%0xFFFFFFFF);
//	DeleteObject(hFont);
#if USE_DINPUT
	if (m_pInput && m_pInput->bCreated)
	{
		// デフォルトのキー設定を入れておく。
		TRelJoystick trJy;
		for (int i=0;i<MAX_BUTTON;i++)
			trJy.bytBtnIndex[i] = i;
		m_pInput->SetJoystick(trJy);
		TRelKeyCode trKc;
		ZeroMemory(&trKc, sizeof(TRelKeyCode));
		ZeroMemory(&trJy, sizeof(TRelJoystick));
		trKc.left = 0xCB;
		trKc.right = 0xCD;
		trKc.up = 0xC8;
		trKc.down = 0xD0;
		trKc.buttons[CONT_BTN_1] = 57;
		trKc.buttons[CONT_BTN_2] = 45;
		m_pInput->SetKeyCode(trKc);

		// キー設定を読込。
		m_pInput->LoadFile(/*KEY_INI*/);
	}
#endif

//===========================================================//
	return TRUE;
}

//-------------------------------------------------------------
// #Game > Run
//-------------------------------------------------------------
BOOL CGame::Run(EGameState eGameState)
{
//===========================可変============================//
//** Main
	BOOL res;
	switch (eGameState)
	{
	// == Title == //
	case eGameTitleInit:	// タイトル初期化
		m_eAllState = eGameRunTitleInit;
		res = TitleInit();
		if (res) m_eGameState = eGameTitle;
		else	 break;
	case eGameTitle:	// タイトルメイン
		m_eAllState = eGameRunTitle;
		res = Title();
		if (res)	m_eGameState = eGameTitleRelease;
		else break;
	case eGameTitleRelease:	// タイトル破棄
		m_eAllState = eGameRunTitleRelease;
		res = TitleRelease();
		if (res)
			m_eGameState = (EGameState)((int)(m_eGameState&0xFF00)<<1);
		else
			return FALSE;
		break;
	// == Login == //
	case eGameLoginInit:	// ログイン初期化
		m_eAllState = eGameRunLoginInit;
		res = LoginInit();
		if (res) m_eGameState = eGameLogin;
		else
			return FALSE;
		break;
	case eGameLogin:	// ログインメイン
		m_eAllState = eGameRunLogin;
		res = Login();
		if (res) m_eGameState = eGameLoginRelease;
		else break;
	case eGameLoginRelease:	// ログイン破棄
		m_eAllState = eGameRunLoginRelease;
		res = LoginRelease();
		if (res)	m_eGameState = (EGameState)((int)(m_eGameState&0xFF00)<<1);
		else
			return FALSE;
	// == Room == //
	case eGameRoomInit:	// 初期化
		m_eAllState = eGameRunRoomInit;
		res = RoomInit();
		if (res) m_eGameState = eGameRoom;
		break;
	case eGameRoom:	// Room
		m_eAllState = eGameRunRoom;
		res = Room();
		if (res) m_eGameState = eGameRoomRelease;
		else	break;
	case eGameRoomRelease:	// RoomRelease
		m_eAllState = eGameRunRoomRelease;
		res = RoomRelease();
		if (res)	m_eGameState = (EGameState)((int)(m_eGameState&0xFF00)<<1);
		else
			return FALSE;

	// == Load == //
	case eGameLoadInit:	// 初期化
		m_eAllState = eGameRunLoadInit;
		res = LoadInit();
		if (res) m_eGameState = eGameLoad;
		else	break;
	case eGameLoad:	// Main
		m_eAllState = eGameRunLoad;
		res = Load();
		if (res) m_eGameState = eGameLoadRelease;
		else	break;
	case eGameLoadRelease:	// MainRelease
		m_eAllState = eGameRunLoadRelease;
		res = LoadRelease();
		if (res)	m_eGameState = (EGameState)((int)(m_eGameState&0xFF00)<<1);
		else
			return FALSE;

	// == Main == //
	case eGameMainInit:	// 初期化
		m_eAllState = eGameRunMainInit;
		res = MainInit();
		if (res) m_eGameState = eGameMain;
		else	break;
	case eGameMain:	// Main
		m_eAllState = eGameRunMain;
		res = Main();
		if (res) m_eGameState = eGameMainRelease;
		else	break;
	case eGameMainRelease:	// MainRelease
		m_eAllState = eGameRunMainRelease;
		res = MainRelease();
		if (res) m_eGameState = eGameResultInit;
		else
			return FALSE;

	// == Result == //
	case eGameResultInit:	// 初期化
		m_eAllState = eGameRunResultInit;
		res = ResultInit();
		if (res) m_eGameState = eGameResult;
		else	break;
	case eGameResult:	// Result
		m_eAllState = eGameRunResult;
		res = Result();
		if (res) m_eGameState = eGameResultRelease;
		else	break;
	case eGameResultRelease:	// ResultRelease
		m_eAllState = eGameRunResultRelease;
		res = ResultRelease();
		if (res) m_eGameState = eGameRoomInit;
		else
			return FALSE;
		break;
	default:
		return FALSE;
//		break;
	}

	return TRUE;
//===========================================================//
}

//-------------------------------------------------------------
// #Game > Release
//-------------------------------------------------------------
BOOL CGame::Release()
{// ~Cgameへ書くべき
	return TRUE;
}

///////////////////////////////////////////////////////////////
// #アクティブ状態
///////////////////////////////////////////////////////////////
void CGame::OnActive()
{
	if (!m_bCreated) return;
	m_bActive = TRUE;

#if	USE_DINPUT
	if (m_pInput && m_pInput->bCreated)
		m_pInput->OnActive();
#else
	for (int i=0;i<USE_BUTTONS;i++)
	{
		if (m_tKeyState.bytButtons[i] & KEY_BTN_ON)	// 押されていたら
			m_tKeyState.bytButtons[i] = KEY_BTN_CHANGE;	// 離した上体にする
	}
	m_tKeyState.lX = 0;
	m_tKeyState.lY = 0;
	m_tKeyState.lZ = 0;
#endif
	if ( m_eGameState == eGameMain)
	p_pUI->GetControl(IDC_MAIN_CONTROLPANEL)->SetVisible(true);

}


// フォーカス無し
void CGame::OnInactive()
{
	if (!m_bCreated) return;
	m_bActive = FALSE;

#if	USE_DINPUT
	if (m_pInput && m_pInput->bCreated)
		m_pInput->OnInactive();
#else
	ZeroMemory(&m_tKeyState, sizeof(TInputState));
#endif
}

//###########################################################//
//#　描画
//###########################################################//
void CGame::Render(float fElapsedTime, LPDIRECT3DDEVICE9 pdev, EGameState eGameState)
{//** 描画処理 LPDIRECT3DDEVICE8->Begin() End()の間

//===========================可変============================//
	switch (eGameState)
	{
	case eGameTitle:	// Title
		m_eAllState = eGameRenderTitle;
		RenderTitle(fElapsedTime);
		break;
	case eGameLogin:	// Login
		m_eAllState = eGameRenderLogin;
		RenderLogin(fElapsedTime);
		break;
	case eGameRoom:	// Room
		m_eAllState = eGameRenderRoom;
		RenderRoom(fElapsedTime);
		break;
	case eGameLoad:	// Load
		m_eAllState = eGameRenderLoad;
		RenderLoad(fElapsedTime);
		break;
	case eGameMain:	// Main
		m_eAllState = eGameRenderMain;
		RenderMain(fElapsedTime);
		break;
	case eGameResult:	// Result
		m_eAllState = eGameRenderResult;
		RenderResult(fElapsedTime);
		break;
	default:
		break;
	}

//=====================FONT======================================//
#if 1
#ifdef _DEBUG
	WCHAR CData[1024+1];
	RECT rect;
	SetRect(&rect, 1, 1, 800, 300);
//	pDXFont->Begin();
//	SafePrintf(CData, (size_t)1024,  _T("%#fFps"), g_fFps);
//	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
//	rect.top += 20;
#if USE_DINPUT
	if (!m_pInput)
		return ;
	SafePrintf(CData, 1024, _T("[X=%d Y=%d Z=%d]"),
			m_pInput->tInState.lX,m_pInput->tInState.lY,m_pInput->tInState.lZ);
	//TextOut(hDC, 0, y, CData, lstrlen(CData));
	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
	rect.top += 20;

	// ボタン表示
	int d = 0;
	for(int i=0; i<USE_BUTTONS; i++)
	{
		if(m_pInput->tInState.bytButtons[i] & KEY_BTN_ON)
			d += SafePrintf(CData + d, 1024-d, _T("[%d]"),i);
		else
			d += SafePrintf(CData + d, 1024-d, _T("[-]"));
	}
	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
	rect.top+=40;
	d = 0;
	for(int i=0; i<USE_BUTTONS; i++)
	{
		if(m_pInput->tInState.bytButtons[i] & (KEY_BTN_CHANGE))
			d += SafePrintf(CData + d, 1024-d, _T("[%d]"),i);
		else
			d += SafePrintf(CData + d, 1024-d, _T("[-]"));
	}
#else
	SafePrintf(CData, 1024, _T("[X=%d Y=%d]"),
			m_tKeyState.lX,m_tKeyState.lY);
	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
	rect.top += 20;
	// ボタン表示
	int d = 0;
	for(int i=0; i<USE_BUTTONS; i++)
	{
		if(m_tKeyState.bytButtons[i] & KEY_BTN_ON)
			d += SafePrintf(CData + d, 1024-d, _T("[%d]"),i);
		else
			d += SafePrintf(CData + d, 1024-d, _T("[-]"));
	}
	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
	rect.top+=40;
	d = 0;
	for(int i=0; i<USE_BUTTONS; i++)
	{
		if(m_tKeyState.bytButtons[i] & (KEY_BTN_CHANGE))
			d += SafePrintf(CData + d, 1024-d, _T("[%d]"),i);
		else
			d += SafePrintf(CData + d, 1024-d, _T("[-]"));
	}
#endif
	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
	rect.top+=40;
	d = 0;
//	SafePrintf(CData, 1024, _T("Y:%d"),m_nTest);
//	if (GetMySessionInfo())
//	SafePrintf(CData, 1024, _T("A:%d"),GetMySessionInfo()->angle);
//	m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
//	rect.top+=40;

#if 0
	if (!m_mapObjects.empty())
	{
		g_pCriticalSection->EnterCriticalSection_Object();
		/*
		int nCount = 0;
		for (std::map<int, type_obj*>::iterator it = m_mapObjects.begin();
			it != m_mapObjects.end();
			it++)
		{
			type_blt* blt = (type_blt*)(*it).second;
			if (blt->obj_state & OBJ_STATE_MAIN_ACTIVE_FLG)
				nCount++;
		}
		SafePrintf(CData, 1024, _T("BLT ACT[%d]"),	nCount);
		m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
		rect.top += 20;
		*/
		std::map<int, type_obj*>::iterator it = m_mapObjects.begin();
		SafePrintf(CData, 1024, _T("BLT POS[%3d,%3d]/VEC[%3d,%3d]/STATE[%x]"),	(*it).second->ax,	(*it).second->ay,	(*it).second->vx,	(*it).second->vy,((type_blt*)(*it).second)->obj_state);
		m_pDXFont->DrawText(NULL, CData, -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));

		g_pCriticalSection->LeaveCriticalSection_Object();	
	}
#endif
#endif	_DEBUG

//===========================================================//
#endif 0
}
//###########################################################//
//#　マウス
//###########################################################//
void CGame::OnMouseLButton(BOOL bDown, int x, int y)
{
	// 各キャラの情報表示設定
//	SetVisibleStageCharacterInfo(bDown?false:true);
	
	switch (m_eGameState)
	{
	case eGameRoom:
		{
			g_pCriticalSection->EnterCriticalSection_StageTexture(L'6');
			m_bMouseLButtonDown = bDown;
			CDXUTIMEEditBox* pIMEEditBox = p_pUI->GetIMEEditBox(IDC_SHARE_EDIT);
			D3DXCOLOR clr = 0xFFFFFFFF;
			if (!pIMEEditBox->m_bHasFocus)
				clr = 0xFFAAAAAA;
			p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->GetElement(0)->TextureColor.States[DXUT_STATE_NORMAL] = clr;
			g_pCriticalSection->LeaveCriticalSection_StageTexture();
			break;
		}
	case eGameMain:
		{
			g_pCriticalSection->EnterCriticalSection_StageTexture(L'7');
			CDXUTIMEEditBox* pIMEEditBox = p_pUI->GetIMEEditBox(IDC_SHARE_EDIT);
			D3DXCOLOR clr = 0xFFFFFFFF;
			if (!pIMEEditBox->m_bHasFocus)
				clr = 0xFFAAAAAA;
			p_pUI->GetIMEEditBox(IDC_SHARE_EDIT)->GetElement(0)->TextureColor.States[DXUT_STATE_NORMAL] = clr;

			if (m_pShotPowerIndicator)
				m_pShotPowerIndicator->OnMouseClick(x,y);
			if (m_pShotAngleIndicator)
				m_pShotAngleIndicator->OnMouseClick(x,y);
			if (y<WIN_HEIGHT-MAIN_CONTROLPANEL_CNT_H_MIN)
				m_bMouseLButtonDown = bDown;

			// クリックされたら弾フォーカスを一時停止して画面ドラッグ可能にする
			if (m_bMouseLButtonDown)
			{
				m_bPauseBulletFocus = TRUE;
				CDXUTControl* pChatLogBG = p_pUI->GetControl(IDC_MAIN_SPRITE_CHATLOG_BG);
				if (pChatLogBG->GetVisible())
					pChatLogBG->SetVisible(false);

				// ダブルクリックはクリア
				m_nMyCharaFocusCounter = 0;
			}
			g_pCriticalSection->LeaveCriticalSection_StageTexture();
			break;
		}
	}
}

void CGame::OnMouseMButton(BOOL bDown, int x, int y)
{
	if (m_eGameState == eGameMain)
	{
		if (y<WIN_HEIGHT-MAIN_CONTROLPANEL_CNT_H_MIN)
		{
			ptype_session pSelf = GetMySessionInfo();

			// 暗闇の場合は処理しない
			if (pSelf->chara_state[CHARA_STATE_BLIND_INDEX] > 0)	return;

			ptype_session pActSess = GetSessionObj(m_nActiveObjectNo);
			if (pActSess)
			{
				BOOL bCanFocus = FALSE;
				// ステルス状態
				if (pActSess->chara_state[CHARA_STATE_STEALTH_INDEX] > 0)
				{
					// 同チームか観戦なら半透明描画
					if (pSelf && (pSelf->team_no == GALLERY_TEAM_NO || pActSess->team_no == pSelf->team_no))
						bCanFocus = TRUE;
				}
				else
					bCanFocus = TRUE;
				if (bCanFocus)
				{
					m_nActiveCharaFocusCounter = CHARA_FOCUS_TIME;
					m_vecActiveCharaFocusStartPos = m_vecStageViewTrans;
				}
			}
		}
	}
}

void CGame::OnMouseMove(int x,int y)
{
//	if (m_eGameState != eGameMain)	return;
	if (p_pConfig->GetVisible())	return;

	m_nMousePosX = x;
	m_nMousePosY = y;
	
//	m_pntWClick.x = -1;
//	m_pntWClick.y = -1;

	if (m_eGameState == eGameMain)
	{
		BOOL bVis = FALSE;
		CDXUTControl* pChatLogBG = p_pUI->GetControl(IDC_MAIN_SPRITE_CHATLOG_BG);
		if (p_pUI->GetControl(IDC_SHARE_EDIT)->m_bHasFocus)
		{
			bVis = TRUE;
		}
		else if (!m_bMouseLButtonDown)
		{
			if (MAIN_CHATLOG_LB_CNT_X <= x && x <= (MAIN_CHATLOG_LB_CNT_X+MAIN_CHATLOG_LB_CNT_W)
			&& MAIN_CHATLOG_LB_CNT_Y <= y && y <= (MAIN_CHATLOG_LB_CNT_Y+MAIN_CHATLOG_LB_CNT_H))
			{
				bVis = TRUE;
			}
		}
		if (bVis)
		{
			if (!pChatLogBG->GetVisible())
				pChatLogBG->SetVisible(true);
		}
		else
		{
			if (pChatLogBG->GetVisible())
				pChatLogBG->SetVisible(false);
		}
	}

	CDXUTControl* pControl = p_pUI->GetControl(IDC_ROOM_BTN_ITEM_DETAIL);
	if (pControl && pControl->GetVisible())
	{
		RECT rcControl = {pControl->m_x, pControl->m_y,pControl->m_x+pControl->m_width, pControl->m_y+pControl->m_height};
		RECT rcInter;
		RECT rcMouse = {x,y,x+1,y+1};
		if (!IntersectRect(&rcInter, &rcControl, &rcMouse))
			p_pUI->GetControl(IDC_ROOM_BTN_ITEM_DETAIL)->SetVisible(false);
	}

	if (x < 5 || y < 5 || x > WIN_WIDTH-5 || y > WIN_HEIGHT-MAIN_CONTROLPANEL_CNT_H_MIN-5)
	{
		m_bMouseLButtonDown = FALSE;
		SetVisibleStageCharacterInfo(true);
		return;
	}

	if (m_bMouseLButtonDown)
	{
		if (m_nSavedMouseX-x )
			m_vecStageViewTrans.x += m_nSavedMouseX-x;
		if ( m_nSavedMouseY-y)	
			m_vecStageViewTrans.y += m_nSavedMouseY-y;
		CalcViewTransCaps();

		UpdateStageDrawRect();
	}

	m_nSavedMouseX = x;
	m_nSavedMouseY = y;
}

void CGame::OnMouseLButtonDoubleClick(WORD mk, int x,int y)
{
	ptype_session self = GetMySessionInfo();
	if (self)
	{
		if (self->obj_state & OBJ_STATE_MAIN_GALLERY_FLG)	return;	// 観戦の時は、フォーカスしない

		if (y<WIN_HEIGHT-MAIN_CONTROLPANEL_CNT_H_MIN)
		{
			m_nMyCharaFocusCounter = CHARA_FOCUS_TIME;
			m_vecMyCharaFocusStartPos = m_vecStageViewTrans;
		}
	}
}

void CGame::CalcViewTransCaps(BOOL bMain)
{
	if (m_pMainStage->GetStageWidth()-(WIN_WIDTH) > 0)
	{
		if (m_vecStageViewTrans.x < 0.0f)
			m_vecStageViewTrans.x = 0.0f;
		else if (m_vecStageViewTrans.x > (float)(m_pMainStage->GetStageWidth()-(WIN_WIDTH)))
			m_vecStageViewTrans.x = (float)(m_pMainStage->GetStageWidth()-(WIN_WIDTH));
	}
	else
		m_vecStageViewTrans.x = 0.0f;

	int nControlVisible = MAIN_CONTROLPANEL_CNT_H_MIN;
	if (!bMain) nControlVisible = 0;
	if ( (m_pMainStage->GetStageHeight()-(WIN_HEIGHT-nControlVisible)) > 0)
	{
		if (m_vecStageViewTrans.y < 0.0f)
			m_vecStageViewTrans.y = 0.0f;
		else if (m_vecStageViewTrans.y > (float)(m_pMainStage->GetStageHeight()-(WIN_HEIGHT-nControlVisible)))
			m_vecStageViewTrans.y = (float)(m_pMainStage->GetStageHeight()-(WIN_HEIGHT-nControlVisible));
	}
	else
		m_vecStageViewTrans.x = 0.0f;
}

//###########################################################//
//#　入力ログ
//###########################################################//
/*
void CGame::ContLogSet(BYTE cont, BYTE button)
{
	if ((tContLog[0].Cont ^ cont) || (tContLog[0].Button ^ button))
	{
		memcpy(&tContLog[1], &tContLog[0], sizeof(TContLog)*(CONT_LOGS-1));
		tContLog[0].Times= 1;
		tContLog[0].Cont = cont;
		tContLog[0].Button= button;
	}
	else
	{
		tContLog[0].Times++;
	}
}

void CGame::VecLogSet(D3DXVECTOR3 v)
{
	if (tVecLog[0].Vec == v)
		tVecLog[0].Times++;
	else
	{
		memcpy(&tVecLog[1], &tVecLog[0], sizeof(TVecLog)*(CONT_LOGS-1));
		tVecLog[0].Times= 1;
		tVecLog[0].Vec = v;
	}
}
*/


// 切断
void CGame::Disconnect()
{
//	SetConnState(CONN_STATE_DESTROY);
//	SetUserState(OBJ_STATE_EMPTY);
	if (m_nTcpSock)
		closesocket(m_nTcpSock);
	m_nTcpSock = 0;
	WSACleanup();	
	g_bCloseSocket = TRUE;
}

//###########################################################//
//#　リセット
//###########################################################//
void CGame::OnResetDevice()
{
	BYTE *pBuf;
	UINT	nBufSize;
	AddMessageLog(L"OnResetDevice");
	
	m_hWnd = DXUTGetHWND();	// 20101227

	WCHAR *szBGMPath = NULL;
	DXUTTextureNode* pTextureNode = NULL;
#if USE_DINPUT
	if (m_pInput && !m_pInput->bCreated)
		m_pInput->OnReset();
#endif

	p_pUI->SetTexture(m_nDefaultGUIResourceIndex, IMG_GUI_SKIN);
	p_pConfig->SetTexture(m_nDefaultGUIResourceIndex, IMG_GUI_SKIN);
	pTextureNode = g_DialogResourceManager.GetTextureNode(m_nDefaultGUIResourceIndex);
	m_pDefaultGUIResourceTexture = pTextureNode->pTexture;

	if (!m_pDXFont)
	{
		if (FAILED(D3DXCreateFont(
			m_pDev
			,20
			,0
			,FW_BOLD//FW_NORMAL
			,1
			,FALSE
			,DEFAULT_CHARSET
			,OUT_DEFAULT_PRECIS
			,DEFAULT_QUALITY
			,DEFAULT_PITCH | FF_SWISS
			,_T("ＭＳ ゴシック")
			,&m_pDXFont))) {
//			DeleteObject(hFont);
				AddMessageLog(L"!Failed:CreateFont");
				return;
		}
	}
	if (m_pDXFont)
		m_pDXFont->OnResetDevice();

	OnResetSoundDevice();

	switch (m_eGameState)
	{
	case eGameRoomInit:
	case eGameRoom:
		m_pTeamSeparater->SetVisible(true);
		if (!szBGMPath)
			szBGMPath = BGM_ROOM;
		AddMessageLog(L"OnResetDevice:Room");
	case eGameTitleInit:
	case eGameTitle:
	case eGameLoginInit:
	case eGameLogin:
		AddMessageLog(L"OnResetDevice:Title-Login");
		if (!m_pBGTexturePath[0])
		{
			AddMessageLog(L"!NoBGTexturePath");
			SafePrintf(m_pBGTexturePath, sizeof(WCHAR)*_MAX_PATH+1, TITLE_BG_TEXTURE);
		}
		SafeRelease(m_pBGTexture);
		g_pFiler->GetFileMemory(m_pBGTexturePath, &pBuf, &nBufSize);
		while (pBuf)
		{
			if (TextureLoader::LoadTextureFromFileInMemory(&m_pBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL, 0, 1))
				SafeDeleteArray(pBuf);
			else
				MessageBox(g_hWnd, L"背景ロードエラー", L"error", MB_OK);
		}

		g_pCriticalSection->EnterCriticalSection_Session(L'1');
		OnResetCharacterScriptTexture();
		g_pCriticalSection->LeaveCriticalSection_Session();
		if (!szBGMPath)
			szBGMPath = BGM_TITLE;
		break;
	case eGameLoadInit:
	case eGameLoad:
		if (!szBGMPath && m_pSelectedStageScrInfo)
			szBGMPath = m_pSelectedStageScrInfo->bgm;

		AddMessageLog(L"OnResetDevice:Load");
		SafeRelease(m_pBGTexture);
		g_pFiler->GetFileMemory(m_pBGTexturePath, &pBuf, &nBufSize);
		while (pBuf)
		{
			if (TextureLoader::LoadTextureFromFileInMemory(&m_pBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL,0,1))
				SafeDeleteArray(pBuf);
		}
		
		SafeRelease(m_pStageTexture);
		if (!m_pSelectedStageScrInfo)
		{
			MessageBox(g_hWnd, L"ステージ情報が消失しました",L"error", MB_OK);
			break;
		}

		if (!m_pMainStage->IsStageLoad())
		{
			g_pFiler->GetFileMemory(m_pSelectedStageScrInfo->stage.path, &pBuf, &nBufSize);
			m_pMainStage->Init(pBuf, nBufSize, &m_pSelectedStageScrInfo->stage.size);
			SafeDeleteArray(pBuf);
			if (!m_pMainStage->IsStageLoad())
			{
				MessageBox(g_hWnd, L"ステージロード失敗",L"error", MB_OK);
				break;
			}
		}
	case eGameLoadRelease:
		AddMessageLog(L"OnResetDevice:LoadRelease");
	case eGameMainInit:
	case eGameMain:
		{
			AddMessageLog(L"OnResetDevice:Main");
			SafeRelease(m_pStageTexture);
			if (g_nStageLoadType == 1)
			{
				if (FAILED(D3DXCreateTextureFromFileEx(g_pDevice, m_pSelectedStageScrInfo->stage.path, 0,0, 1, 0, D3DDEFAULT_FORMAT, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pStageTexture)))
				{
					MessageBox(NULL, L"ステージテクスチャ作成失敗",L"ステージテクスチャ作成", MB_OK);
					g_bCloseSocket = TRUE;
					break;
				}
			}
			else
			{
				if (!m_pMainStage->CreateTexture(m_pDev, &m_pStageTexture))
				{
					MessageBox(NULL, L"ステージテクスチャ作成失敗",L"ステージテクスチャ作成", MB_OK);
					g_bCloseSocket = TRUE;
					break;
				}
			}
			m_pSelectedStageScrInfo->pTexture = m_pStageTexture;

			SafeRelease(m_pStageBGTexture);
			if (!m_pSelectedStageScrInfo)
			{
				MessageBox(g_hWnd, L"ステージ情報が消失しました",L"error", MB_OK);
				break;
			}
			g_pFiler->GetFileMemory(m_pSelectedStageScrInfo->bg.path, &pBuf, &nBufSize);
			while (pBuf)
			{
				if (TextureLoader::LoadTextureFromFileInMemory(&m_pStageBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL, 0, 1))
					SafeDeleteArray(pBuf);
			}
			g_pCriticalSection->EnterCriticalSection_Session(L'2');
			OnResetCharacterScriptTexture();
			g_pCriticalSection->EnterCriticalSection_Sound(L'1');
			OnResetCharacterScriptSound();
			g_pCriticalSection->LeaveCriticalSection_Sound();
			g_pCriticalSection->LeaveCriticalSection_Session();
			m_bLoadComplete = TRUE;

			if (!szBGMPath && m_pSelectedStageScrInfo)
				szBGMPath = m_pSelectedStageScrInfo->bgm;
			break;
		}
	case eGameResultInit:
	case eGameResult:
		AddMessageLog(L"OnResetDevice:Result");
		if (!szBGMPath)
			szBGMPath = BGM_ROOM;

		SafeRelease(m_pStageTexture);
		if (g_nStageLoadType == 1)
		{
			if (FAILED(D3DXCreateTextureFromFileEx(g_pDevice, m_pSelectedStageScrInfo->stage.path, 0,0, 1, 0, D3DDEFAULT_FORMAT, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pStageTexture)))
			{
				MessageBox(NULL, L"ステージテクスチャ作成失敗",L"ステージテクスチャ作成", MB_OK);
				break;
			}
		}
		else
		{
			if (m_pMainStage->CreateTexture(m_pDev, &m_pStageTexture))
			{
				MessageBox(NULL, L"ステージテクスチャ作成失敗",L"ステージテクスチャ作成", MB_OK);
				break;
			}
		}
		m_pSelectedStageScrInfo->pTexture = m_pStageTexture;
		SafeRelease(m_pStageBGTexture);
		if (!m_pSelectedStageScrInfo)
		{
			MessageBox(g_hWnd, L"ステージ情報が消失しました",L"error", MB_OK);
			break;
		}
		g_pFiler->GetFileMemory(m_pSelectedStageScrInfo->bg.path, &pBuf, &nBufSize);
		while (pBuf)
		{
			if (TextureLoader::LoadTextureFromFileInMemory(&m_pStageBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL, 0, 1))
				SafeDeleteArray(pBuf);
		}
		break;
	}
	// リセットのためもう一度BGMの再生
	if (szBGMPath)
		PlayBGM(szBGMPath, 0);

	m_bDeviceLost = FALSE;	
}

BOOL CGame::OnResetSoundDevice()
{
	if (m_pSoundPlayer && !m_pSoundPlayer->IsCreated())
	{
		g_pCriticalSection->EnterCriticalSection_Sound(L'2');
		m_pSoundPlayer->Initialize(DXUTGetHWND(), DSSCL_PRIORITY);
		m_pSoundPlayer->SetPrimaryBufferFormat();
		// 音読み込みクラス生成
		if (m_pSysSoundLibs)
			m_pSysSoundLibs->ResetDirectSound(m_pSoundPlayer->GetDirectSound());
		if (m_pScrSoundLibs)
			m_pScrSoundLibs->ResetDirectSound(m_pSoundPlayer->GetDirectSound());
		if (m_pSysSoundLibs->GetCount() == 0)
			LoadSystemSound();

		g_pCriticalSection->LeaveCriticalSection_Sound();
	}
	return TRUE;
}

//###########################################################//
//#　ロスト
//###########################################################//
void CGame::OnLostDevice()
{
	m_bDeviceLost = TRUE;
#if	USE_DINPUT
	if (m_pInput)
		m_pInput->OnLost();
#endif
	AddMessageLog(L"OnLostDevice");
	if (m_pDXFont)
		m_pDXFont->OnLostDevice();
	switch (m_eGameState)
	{
	case eGameTitleInit:
	case eGameTitle:
	case eGameTitleRelease:
	case eGameLoginInit:
	case eGameLogin:
	case eGameLoginRelease:
		SafeRelease(m_pBGTexture);
		SafeRelease(m_pStageTexture);
		SafeRelease(m_pStageBGTexture);
		break;
	case eGameRoomInit:
	case eGameRoom:
		g_pCriticalSection->EnterCriticalSection_Session(L'3');
		OnLostCharacterScriptTexture();
		g_pCriticalSection->LeaveCriticalSection_Session();
		SafeRelease(m_pBGTexture);
		break;
	case eGameLoadInit:
	case eGameLoad:
		AddMessageLog(L"OnLost_eGameLoad");
		SafeRelease(m_pBGTexture);
		SafeRelease(m_pStageTexture);
		SafeRelease(m_pStageBGTexture);
//		MessageBox(g_hWnd, L"ロード中にD3Dデバイスがロストしました", L"error", MB_OK);

		break;
	case eGameLoadRelease:
		AddMessageLog(L"OnLost_eGameLoadRelease");
	case eGameMainInit:
	case eGameMain:
	case eGameMainRelease:
		AddMessageLog(L"OnLost_eGameMain");
		g_pCriticalSection->EnterCriticalSection_Session(L'4');
		OnLostCharacterScriptTexture();
		g_pCriticalSection->LeaveCriticalSection_Session();
		m_pSelectedStageScrInfo->pTexture = m_pStageTexture;
		SafeRelease(m_pBGTexture);
		SafeRelease(m_pStageTexture);
		SafeRelease(m_pStageBGTexture);

		break;
	case eGameResultInit:
	case eGameResult:
		SafeRelease(m_pBGTexture);
		SafeRelease(m_pStageTexture);
		SafeRelease(m_pStageBGTexture);
		break;
	}
	// test
	m_mapSysSoundIDHash.clear();
	if (!m_pSysSoundLibs->Clear())
	{
		AddMessageLog(L"!!OnLostDevice() >> m_pSysSoundLibs->Clear()");
	}
	m_mapScrSoundIDHash.clear();
	if (!m_pScrSoundLibs->Clear())
	{
		AddMessageLog(L"!!OnLostDevice() >> m_pScrSoundLibs->Clear()");
	}
	m_pSoundPlayer->Release();
}

//###########################################################//
//#　デストロイ
//###########################################################//
void CGame::OnDestroyDevice()
{
	SafeRelease(m_pDXFont);
}
