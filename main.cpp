// ===================================================================
/** main
 @brief メイン：main.cpp
*/
// ===================================================================
#pragma once

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shell32.lib")

#pragma comment(lib, "d3d9.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif
//#pragma comment(lib, "Dxerr.lib")
//#pragma comment(lib, "dinput8.lib")
//#pragma comment(lib, "dinput.lib")
//#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "dxerr9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Dsound.lib")
#pragma comment(lib, "comctl32.lib")
//#if USE_DEBUG_FILER		// CFilerは個々のファイルなのでクローズが必要


#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "luahelper_d_uni.lib")
//#pragma comment ( lib, "libogg_static_d.lib" )
//#pragma comment ( lib, "libvorbis_static_d.lib" )
//#pragma comment ( lib, "libvorbisfile_static_d.lib" )
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "util_d.lib")
//#pragma comment(lib, "Input_d.lib")
#pragma comment(lib, "D:/prg/VS/thp/lib/DXUT_d.lib")
#pragma comment(lib, "LinkageList_d.lib")
#pragma comment(lib, "Filer_d.lib")
#pragma comment(lib, "TextureLoader_d.lib")
#pragma comment(lib, "SoundLib_d.lib")
#pragma comment(lib, "IniFile_d.lib")
#else
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "luahelper_uni.lib")
//#pragma comment ( lib, "libogg_static.lib" )
//#pragma comment ( lib, "libvorbis_static.lib" )
//#pragma comment ( lib, "libvorbisfile_static.lib" )
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "util.lib")
//#pragma comment(lib, "Input.lib")
#pragma comment(lib, "D:/prg/VS/thp/lib/DXUT.lib")
#pragma comment(lib, "LinkageList.lib")
#pragma comment(lib, "Filer.lib")
#pragma comment(lib, "TextureLoader.lib")
#pragma comment(lib, "SoundLib.lib")
#pragma comment(lib, "IniFile.lib")
#endif

#include <string>
#include "minizip/ioapi.h"
#include "uzip.h"
#include "zconf.h"
#include "zlib.h"
#include "bit_limits.h"
using namespace std;
#include "ext.h"
#include "main.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAPALLOC	1
#endif

/* =================================== */
/* =====      グローバル変数     ===== */
/* =================================== */
HWND			g_hWnd = NULL;
HINSTANCE	g_hInst = NULL;
LPDIRECT3DDEVICE9 g_pDevice;	// Device

CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CDXUTDialog				g_SysUI;             // dialog for sample specific controls
CDXUTDialog				g_SysConfig;
CPacketQueue*			g_pPacketQueue=NULL;
CCriticalSection*		g_pCriticalSection=NULL;
LPD3DXSPRITE			g_pSprite = NULL;
CFiler*						g_pFiler = NULL;
BOOL						g_bKillLoadingThread = FALSE;
BOOL						g_bDeviceReady = FALSE;
BOOL						g_bCloseSocket = FALSE;
//#ifdef _DEBUG
BOOL						g_bOneClient = FALSE;
BOOL						g_bDebug = FALSE;
CTextFile*					g_pLogFile=NULL;
//#endif
BOOL						g_bCmdSetup = FALSE;
WCHAR						g_wsCmdName[MAX_USER_NAME+1];
WCHAR						g_wsCmdPass[MAX_SRV_PASS+1];
WCHAR						g_wsCmdIP[64];
int								g_nCmdPort;
int								g_nStageLoadType = 0;
/*	===================================
  	=====      静的変数     =====
  	===================================*/
CGame*	g_pGame = NULL;
#if USE_LUA
lua_State*	g_L = NULL;
LuaHelper*	g_pLuah = NULL;
#endif
/*===================================
	=====        プログラム       =====
	===================================*/
void png_memread_func(png_structp png_ptr, png_bytep buf, png_size_t size);
void png_data_read(png_structp png_ptr, my_png_buffer *png_buff);
BOOL ParseCmd();
BOOL AddData(WCHAR* path);

HANDLE g_hThreadMainLoop = NULL;
DWORD __stdcall Thread_MainLoop(LPVOID param);

void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

// -------------------------------------------------------------------
// メイン
// -------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	switch (__argc)
	{
	case 0:
	case 1:
		break;
	default:
		if (!ParseCmd())
			return 0;
		break;
	}

	g_hInst = hInst;
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if (!InitSystem())
		return 0;
	
#if USE_LUA
	// LuaのVMを生成する
	g_L = lua_open();
	// Luaの標準ライブラリを開く
	luaL_openlibs(g_L);
	// bit演算ライブラリ
	luaopen_bit(g_L);
	// グルーコードを実行
	tolua_thg_open(g_L);

	g_pLuah = new LuaHelper();
	g_pLuah->SetLua(g_L);
#endif

	AddMessageLog(CLI_VER);
	AddMessageLog(L"＞起動");

	{
		WCHAR caption[64];
		if (g_bOneClient)
			SafePrintf(caption, 64, L"%s ONE CLIENT", NAME_CAPTION_CLI);
		else
			SafePrintf(caption, 64, L"%s %s", NAME_CAPTION_CLI, CLI_VER);

		g_hInst = hInst;
#if (E0401==0)
		DXUTInit( true, false, true, false ); // Parse the command line, handle the default hotkeys, and show msgboxes
#else
		DXUTInit( true, true, true, false ); // Parse the command line, handle the default hotkeys, and show msgboxes
#endif
		DXUTCreateWindow( caption, hInst );
	}

	g_hWnd = DXUTGetHWND();
	DXUTSetWindow(g_hWnd, g_hWnd, g_hWnd);
	DXUTCreateDevice( D3DADAPTER_DEFAULT, true, WIN_WIDTH, WIN_HEIGHT, IsDeviceAcceptable, ModifyDeviceSettings );
    // Custom main loop
	timeBeginPeriod(1);
//	DXMyMainLoop(0);

	DWORD dwParam = STILL_ACTIVE-1;
	DWORD dwThreadId = 1;
	g_hThreadMainLoop = CreateThread(NULL,0,Thread_MainLoop, 0,0,&dwThreadId);
	while (dwParam != STILL_ACTIVE)
	{
		GetExitCodeThread(g_hThreadMainLoop, &dwParam);
		Sleep(1);
	}

	HWND hWnd = DXUTGetHWND();
    BOOL bGotMsg;
    MSG  msg;
	
	while( (bGotMsg = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{
		if (bGotMsg == -1)
		{
			AddMessageLog(L"GetMessage(-1)");
			g_bCloseSocket  = TRUE;
			break;
		}
		else if (GetExitCodeThread(g_hThreadMainLoop, &dwParam))
		{
			if (dwParam == STILL_ACTIVE) 
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
			else
				break;
		}
	}
	timeEndPeriod(1);
	AddMessageLog(L"EndProgram");
	// 後片付け
	if (EndProgram())
		DXUTShutdown();
	return 0;
}

// メインループ
DWORD __stdcall Thread_MainLoop(LPVOID param)
{
	MSG msg;
	while( 1 )
	{ 
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))// メッセージの確認
		{
			if (msg.message == WM_QUIT) {					// WM_QUIT なら
				AddMessageLog(L"Thread_MainLoop:WM_QUIT");	
				DXUTSetCallbackMsgProc(NULL);
				g_bCloseSocket = TRUE;
//> 20110301
//				DestroyWindow(msg.hwnd);
//< 20110301
//				DXUTShutdown(0);
				break;										// ループを抜ける
			}
		}

		if (g_bCloseSocket)
		{
			AddMessageLog(L"Thread_MainLoop:g_bCloseSocket");
			DXUTSetCallbackMsgProc(NULL);
			DestroyWindow(msg.hwnd);
//			DXUTShutdown(0);
			break;
		}
		DXMyRender3DEnvironment();
	}

	return 0;
}

//--------------------------------------------------------------------------------------
// 初期化
//--------------------------------------------------------------------------------------
BOOL InitSystem()
{
	_set_fmode(_O_BINARY);
	setlocale(LC_ALL,"japanese");

	if (CWuVorbisDecoder::WuVorbisInitialize())
	{
		MessageBox(g_hWnd, L"wuvorbis.dll LoadError", L"error", MB_OK);
		return FALSE;
	}
#if RANDOMIZE
	init_genrand(timeGetTime());
#endif

//	BaseVector::InitBaseVector();
	DXUTSetCallbackDeviceCreated( OnCreateDevice );
	DXUTSetCallbackDeviceReset( OnResetDevice );
	DXUTSetCallbackDeviceLost( OnLostDevice );
	DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackFrameRender( OnFrameRender );
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackKeyboard( OnKeyboard );
// Show the cursor and clip it when in full screen
//	DXUTSetCursorSettings( true, true );
//	DXUTSetConstantFrameTime(true, ((float)DRAW_FRAMES / 1000.0f));

    // Initialize dialogs
    g_SysUI.Init( &g_DialogResourceManager );
	g_SysConfig.Init( &g_DialogResourceManager );

	g_SysConfig.SetCallback( OnGUIControlEvent, &g_SysConfig );
	g_SysConfig.SetFont( 1, L"ＭＳ ゴシック", 16, FW_BOLD );
	g_SysUI.SetCallback( OnGUIControlEvent, &g_SysUI );
	g_SysUI.SetFont( 1, L"ＭＳ ゴシック", 16, FW_NORMAL );
	g_SysUI.SetFont( 2, L"ＭＳ ゴシック", 11, FW_NORMAL );
	g_SysUI.SetFont( 3, L"ＭＳ ゴシック", 16, FW_NORMAL );
	g_SysUI.SetFont( 4, L"ＭＳ ゴシック", 24, FW_NORMAL );
	g_SysUI.SetFont( 5, L"ＭＳ ゴシック", 18, FW_BOLD );
	g_SysUI.SetFont( 6, L"ＭＳ ゴシック", 54, FW_BOLD );
	g_SysUI.SetFont( 7, L"ＭＳ ゴシック", 13, FW_NORMAL );

    // IME-enabled edit box
	WCHAR wsName[MAX_USER_NAME+1];
	CIniFile* pIniFile = new CIniFile();
	pIniFile->ReadString(L"LOGIN", L"ID", L"guest", wsName, MAX_USER_NAME+1);
#ifdef _DEBUG
	g_bDebug = pIniFile->ReadBool(L"CONFIG", L"DEBUG", TRUE);
#else
	g_bDebug = FALSE;	//pIniFile->ReadBool(L"CONFIG", L"DEBUG", FALSE);
#endif
	g_nStageLoadType = pIniFile->ReadInt(L"CONFIG", L"STAGE_LOAD", 0);
	g_bOneClient = pIniFile->ReadBool(L"CONFIG", L"ONE_CLIENT", FALSE);
#if ONE_CLIIENT
	g_bOneClient = TRUE;
#endif
	SafeDelete(pIniFile);

	if (g_bDebug)
	{
		static WCHAR szDate[32];
		static WCHAR szTime[32];
//		SYSTEMTIME sysTime;
//		GetLocalTime(&sysTime);                        // 現在の時間を求める
		if ( GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, NULL, L"HHmm", szTime, 32)
		&& GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0,NULL, L"yyMMdd", szDate, 32))
		{
			WCHAR logfile[_MAX_PATH*2+1];
			SafePrintf(logfile, _MAX_PATH*2, L"clog%s%s.txt", szDate, szTime);
			g_pLogFile = new CTextFile(logfile, L"a+");
			if (!g_pLogFile->IsOpened())
			{
				MessageBox(NULL, L"ログファイルのオープンに失敗しました。",L"error", MB_OK);
				EndProgram();
				return FALSE;
			}
		}
	}

	wsName[MAX_USER_NAME] = NULL;
    CDXUTIMEEditBox *pIMEEdit;
    if( SUCCEEDED( g_SysUI.AddIMEEditBox( IDC_SHARE_EDIT, wsName, 300, 200, 400, 45, false, &pIMEEdit ) ) )
    {
		pIMEEdit->GetElement( 0 )->iFont = 1;
		pIMEEdit->GetElement( 1 )->iFont = 1;
		pIMEEdit->GetElement( 0 )->TextureColor.Init( 0xFFFFFFFF );  // Transparent center
		pIMEEdit->GetElement(0)->TextureColor.States[DXUT_STATE_NORMAL] = 0xFFFFFFFF;
		pIMEEdit->SetSpacing(3);
		pIMEEdit->SetBorderWidth(0);
		pIMEEdit->SetTextColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		pIMEEdit->SetCaretColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		pIMEEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		pIMEEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 0, 0, 128 ) );
		pIMEEdit->SetVisible(false);
    }
	CDXUTIMEEditBox::EnableImeSystem( true );

	DEBUG_NEW(g_pCriticalSection, CCriticalSection(), L"InitSystem");
	g_pGame = new CGame();
	g_pFiler = new CFiler();

	return TRUE;
}

//--------------------------------------------------------------------------------------
// 後片付け
//--------------------------------------------------------------------------------------
int EndProgram()
{
	DWORD dwParam;
	g_bKillLoadingThread = TRUE;
	if (GetExitCodeThread(g_hThreadMainLoop, &dwParam))
	{
		if (dwParam == STILL_ACTIVE) 
		{
			TerminateThread(g_hThreadMainLoop, FALSE);
			WaitForSingleObject(g_hThreadMainLoop, 100);        /* プロセスの終了を待つ */
			CloseHandle(g_hThreadMainLoop);
		}
	}

	g_SysUI.ClearFocus();
	g_SysConfig.ClearFocus();
//	g_SysUI.GetIMEEditBox(IDC_SHARE_EDIT)->ClearText();

	if (g_pGame)	g_pGame->Disconnect();

	if (g_pCriticalSection)
	{
#ifdef DEBUGCS
		WCHAR log[16];
		std::stack<WCHAR> ss;
		WCHAR w=NULL;
		ss = g_pCriticalSection->GetSessionWord();
		while (!ss.empty())
		{
			SafePrintf(log,16,L"cs_sess(%C)", ss.top());
			AddMessageLog(log);
			ss.pop();
		}
		ss = g_pCriticalSection->GetObjectWord();
		while (!ss.empty())
		{
			SafePrintf(log,16,L"cs_obj(%C)", ss.top());
			AddMessageLog(log);
			ss.pop();
		}
#endif
		AddMessageLog(L">StopClient");
//		g_pCriticalSection->EnterCriticalSection_Packet();
//		g_pCriticalSection->EnterCriticalSection_Session(L'~');
		StopClient();
//		g_pCriticalSection->LeaveCriticalSection_Session();
//		g_pCriticalSection->LeaveCriticalSection_Packet();
		AddMessageLog(L"<StopClient>CriticalSection");
		DEBUG_DELETE(g_pCriticalSection,  L"EndProgram");
	}
	else
		StopClient();

	SafeDelete(g_pFiler);

	AddMessageLog(L"<CriticalSection");
	OutputDebugStr(L">Lua");
#if USE_LUA
	SafeDelete(g_pLuah);	// 内部でlua_close(g_L);呼び出し
#endif
//	g_SysUI.RemoveAllControls();
//	g_SysConfig.RemoveAllControls();
	HANDLE  hProcess = NULL;
	DWORD dwProcessId = NULL;
	GetWindowThreadProcessId(DXUTGetHWND(), &dwProcessId);
	if (dwProcessId)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if(hProcess)
		{
			AddMessageLog(L"Terminate");
			TerminateProcess(hProcess, 0);
		}
	}
	SafeDelete(g_pGame);
	SafeRelease(g_pSprite);
	AddMessageLog(L"＞終了");
	SafeDelete(g_pLogFile);
	CWuVorbisDecoder::WuVorbisFinalize();
	CHECK_DEBUG_MEMORY_LEAK;
	if (hProcess)
	{
		WaitForSingleObject(hProcess, 500);        /* プロセスの終了を待つ */
		CloseHandle(hProcess);
	}
	return DXUTGetExitCode();
//	int ret= DXUTGetExitCode();
//	DXUTShutdown();
//	return ret;
}

// フレーム処理
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
#if ON_CLOSE_SOCKET_EXIT
	if (g_bCloseSocket)
	{
		AddMessageLog(L"OnFrameMove:g_bCloseSocket");	
		PostQuitMessage(0);
		return;
	}
#endif
	// wait
	static DWORD dwNextTick = FRAME_RATE;
	DWORD dwNowTick = 0;
	while(dwNextTick-3 > (dwNowTick=timeGetTime()))
		Sleep(1);
	while(dwNextTick > (dwNowTick=timeGetTime()))
		Sleep(0);
	dwNextTick = dwNowTick+FRAME_RATE;

	if (!g_pGame)
		return;

#if	!USE_DINPUT
	g_pCriticalSection->EnterCriticalSection_Input(L'1');
#endif
	if (!(g_pGame->Game()))
	{
		PostQuitMessage(0);
	}
#if	!USE_DINPUT
	g_pGame->UpdateKeyState();
	g_pCriticalSection->LeaveCriticalSection_Input();
#endif
}

// 描画処理
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
	if (!g_pGame) return;
	if (!g_pGame->IsCreated()) return;
	if (g_bCloseSocket)	return;
	if (!g_bDeviceReady) return;	// デバイスは描画しない

//	WCHAR wcfps[16];
//	SafePrintf(wcfps, 16, L"%f/%d", DXUTGetFPS(), timeGetTime() );
	
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0) );
	
	// Render the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		g_pGame->Render(fElapsedTime);
//		V( g_SysUI.OnRender( fElapsedTime ) );
		V( pd3dDevice->EndScene() );
	}
}

// デバイス確認
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

//	// Must support pixel shader 1.1
//	if( pCaps->PixelShaderVersion < D3DPS_VERSION( 1, 1 ) )
//		return false;

    return true;
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
	// バックバッファフォーマットを固定
	D3DDISPLAYMODE Mode;
	DXUTGetD3DObject()->GetAdapterDisplayMode(0, &Mode);
	if (Mode.Format != D3DDEFAULT_FORMAT)
		pDeviceSettings->pp.BackBufferFormat = D3DDEFAULT_FORMAT;

	pDeviceSettings->BehaviorFlags |=  D3DCREATE_MULTITHREADED;
    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }
/*
	if (pDeviceSettings->pp.BackBufferWidth < WIN_WIDTH || pDeviceSettings->pp.BackBufferHeight < WIN_HEIGHT)
	{
		MessageBox(NULL, L"ゲームの解像度が800x600以下に設定されたため終了します。", L"reset", MB_OK);
		g_bCloseSocket = TRUE;
		DestroyWindow(g_hWnd);
		return false;
	}
*/
	// FPS 固定化
	pDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    return true;
}

HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
	AddMessageLog(L"OnCreateDevice");

	SetWindowLong(g_hWnd, GWL_STYLE, WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX/*|WS_MAXIMIZEBOX*/);

    V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
	g_pDevice = pd3dDevice;

	const D3DCAPS9* pCaps = DXUTGetDeviceCaps();

	if (pCaps->MaxTextureHeight < NEED_MAX_TEXTURE_HEIGHT
		|| pCaps->MaxTextureWidth < NEED_MAX_TEXTURE_WIDTH)
	{
		MessageBox(g_hWnd,
			L"お使いのパソコンはスペックが不足しております。\n誠に恐縮ですがゲームを終了してください。",
			L"error",
			MB_OK);
	}

	//スプライト作成
	if (FAILED(D3DXCreateSprite(pd3dDevice, &g_pSprite ) ) )
		return FALSE;

	if (g_pGame)
	{
		AddMessageLog(L"<Game::OnCreateDevice()>");
		g_pGame->OnCreateDevice(g_hWnd, g_hInst);
		if (!g_pGame->IsCreated())
		{
			g_pGame->SetLua( g_L, g_pLuah);
			g_pGame->SetUI( &g_SysUI, &g_SysConfig);
			AddMessageLog(L"<Game::Create()>");
			if (!g_pGame->Create())
			{
				g_bCloseSocket = TRUE;
				return S_FALSE;
			}
		}
	}
	
	return S_OK;
}

HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
	AddMessageLog(L">CALLBACK OnResetDevice");

	g_pCriticalSection->EnterCriticalSection_StageTexture(L'1');
	g_hWnd = DXUTGetHWND();

	if( g_pSprite )
		g_pSprite->OnResetDevice();
	DXUTDeviceSettings setting = DXUTGetDeviceSettings();
	if (setting.pp.BackBufferWidth < WIN_WIDTH || setting.pp.BackBufferHeight < WIN_HEIGHT)
	{
		MessageBox(NULL, L"ゲームの解像度が800x600以下に設定されたため終了します。", L"reset", MB_OK);
		g_bCloseSocket = TRUE;
		DestroyWindow(g_hWnd);
		return E_FAIL;
	}

	hr = g_DialogResourceManager.OnResetDevice();
	if (FAILED(hr))
	{
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		g_bKillLoadingThread = FALSE;
		return hr;
	}
	if (g_pGame)	g_pGame->OnResetDevice();
    g_SysUI.SetLocation( 0, 0 );
    g_SysUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

//    g_SysUI.GetControl( IDC_FPS )->SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height * 6 / 10 );
	
	g_SysConfig.SetLocation( 0, 0 );
    g_SysConfig.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

	g_pCriticalSection->LeaveCriticalSection_StageTexture();
	
	g_bKillLoadingThread = FALSE;
	g_bDeviceReady = TRUE;
	DXUTPause(false, false);
	AddMessageLog(L"<CALLBACK OnResetDevice");
	return S_OK;
}

void CALLBACK OnLostDevice( void* pUserContext )
{
	g_bDeviceReady = FALSE;
	g_bKillLoadingThread = TRUE;
	DXUTPause(true, false);

	AddMessageLog(L"CALLBACK OnLostDevice");

	WCHAR p[32];
	if (g_pGame)
	{
		SafePrintf(p, 32, L"state:%d\n", (DWORD)g_pGame->m_eAllState);
		OutputDebugStr(p);
	}

	if (g_pCriticalSection)
		g_pCriticalSection->EnterCriticalSection_StageTexture(L'2');
    g_DialogResourceManager.OnLostDevice();
	if( g_pGame ) g_pGame->OnLostDevice();
	if (g_pSprite)  g_pSprite->OnLostDevice();
	if (g_pCriticalSection)
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
}

void CALLBACK OnDestroyDevice( void* pUserContext )
{
	AddMessageLog(L"CALLBACK OnDestroyDevice");
	SafeRelease(g_pSprite);
	if( g_pGame ) g_pGame->OnDestroyDevice();
    g_DialogResourceManager.OnDestroyDevice();
}

//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	switch (uMsg)
	{
//	case SC_CLOSE:
	case WM_QUIT:
	case WM_CLOSE:
		AddMessageLog(L"CLOSE");
		CloseWindow(hWnd);
//		DestroyWindow(hWnd);
		break;
	case WM_ACTIVATE:		// ウインドウのアクティブ状態が変化
		if (g_pGame)
		{
			if(wParam != WA_INACTIVE)
				g_pGame->OnActive();
			else
				g_pGame->OnInactive();
		}
		break;
	case WM_MBUTTONDOWN:
		if (g_pGame)
			g_pGame->OnMouseMButton(TRUE, LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_LBUTTONDBLCLK:
		if (g_pGame)
			g_pGame->OnMouseLButtonDoubleClick(wParam, LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_LBUTTONDOWN:
		if (g_pGame)
			g_pGame->OnMouseLButton(TRUE, LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_LBUTTONUP:
		if (g_pGame)
			g_pGame->OnMouseLButton(FALSE, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		if (g_pGame)
			g_pGame->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	}
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	// コンフィグ表示中はメインUIのメッセージ処理しない
	if (!g_SysConfig.GetVisible())
	{
	    *pbNoFurtherProcessing = g_SysUI.MsgProc( hWnd, uMsg, wParam, lParam );
		if( *pbNoFurtherProcessing )
			return 0;
	}

	*pbNoFurtherProcessing = g_SysConfig.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    return 0;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIControlEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	if (pUserContext == &g_SysUI)
		g_pGame->OnGUIEvent(nEvent, nControlID, pControl, pUserContext);
	else
		g_pGame->OnConfigEvent(nEvent, nControlID, pControl, pUserContext);
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if (g_pGame)
		g_pGame->OnKeyEvent(nChar, bKeyDown, bAltDown);
}

#if USE_LUA
// -------------------------------------------------------------------
// Lua
// -------------------------------------------------------------------
#endif

//#ifdef _DEBUG
void AddMessageLog(TCHAR*	plog)
{
	if (!g_bDebug) return;
	if (g_pCriticalSection) g_pCriticalSection->EnterCriticalSection_Log(L'1');
#ifdef _DEBUG
	static WCHAR szDbgStr[64];
	SafePrintf(szDbgStr, 64, L"%s\n", plog);
	OutputDebugStr(szDbgStr);
#endif
	if (!g_pLogFile)
	{
		if (g_pCriticalSection) g_pCriticalSection->LeaveCriticalSection_Log();
		return;
	}
    SYSTEMTIME sysTime;
	WCHAR pMsg[MAX_LOG_BUFFER];
	static WCHAR szDate[32];
    static WCHAR szTime[32];
	GetLocalTime(&sysTime);                        // 現在の時間を求める

	GetDateFormat( LOCALE_USER_DEFAULT,
							0,	&sysTime,
							L"yyyy'/'MM'/'dd",
							szDate,sizeof(szDate)); 
	GetTimeFormat(LOCALE_USER_DEFAULT,
							0, 	&sysTime,
							L"HH':'mm':'ss''",
							szTime,sizeof(szTime));
	SafePrintf(pMsg, MAX_LOG_BUFFER, L"[%s-%s] %s", szDate, szTime, plog);
	if (g_pLogFile)
		g_pLogFile->WriteLine(pMsg);
	if (g_pCriticalSection) g_pCriticalSection->LeaveCriticalSection_Log();
}
//#endif


void png_memread_func(png_structp png_ptr, png_bytep buf, png_size_t size)
{
    my_png_buffer *png_buff = (my_png_buffer *)png_get_io_ptr(png_ptr);
    if (png_buff->data_offset + size <= png_buff->data_len) {
        memcpy(buf, png_buff->data + png_buff->data_offset, size);
        png_buff->data_offset += size;
    } else {
        png_error(png_ptr,"png_mem_read_func failed");
    }
}

void png_data_read(png_structp png_ptr, my_png_buffer *png_buff)
{
    png_set_read_fn(png_ptr, (png_voidp) png_buff,
                    (png_rw_ptr)png_memread_func);
}

// コマンドラインから起動
BOOL ParseCmd()
{
	int nIndex = 0;
	int nArgIndex = 1;
	int nArgBufIndex = 0;
	g_bCmdSetup = TRUE;
	WCHAR wsCmdline[_MAX_PATH*2+1];
	wsCmdline[0] = NULL;
	SafePrintf(wsCmdline, _MAX_PATH*2, L"%s", __wargv[1]);
	for (int nCmdIndex = 2;nCmdIndex < __argc;nCmdIndex++)
		SafePrintf(wsCmdline, _MAX_PATH*2, L"%s %s", wsCmdline, __wargv[nCmdIndex]);
	if (IsFilePath(wsCmdline))
	{
		if (!AddData(wsCmdline))
		{
			MessageBox(NULL, L"データの解凍に失敗しました", L"データ追加エラー", MB_OK);
			return FALSE;
		}
		else
		{
			MessageBox(NULL, L"データの解凍に成功しました", L"データ追加", MB_OK);
			return FALSE;
		}
	}
	

	ZeroMemory(g_wsCmdName, sizeof(g_wsCmdName));
	ZeroMemory(g_wsCmdPass, sizeof(g_wsCmdPass));
	ZeroMemory(g_wsCmdIP, sizeof(g_wsCmdIP));

	SafePrintf(g_wsCmdIP, 64, L"%s", __wargv[nArgIndex]);

	nArgIndex++;
	g_nCmdPort = StrToInt(__wargv[nArgIndex]);
	nArgIndex++;
	nIndex = 0;
	nArgBufIndex = 0;
	int nBufferLen = StrToInt(__wargv[nArgIndex]);
	nArgIndex++;
	if (nBufferLen >= MAX_USER_NAME || nBufferLen < MIN_USER_NAME)
	{
		MessageBox(NULL, L"引数が不正です", L"error", MB_OK);
		return FALSE;
	}
	while (nIndex < nBufferLen)
	{
		if (!__wargv[nArgIndex][nArgBufIndex])
		{
			g_wsCmdName[nIndex] = L' ';
			nArgBufIndex = 0;
			nArgIndex++;
			if (nArgIndex >= __argc)
			{
				MessageBox(NULL, L"引数が不正です", L"error", MB_OK);
				return FALSE;
			}
			nIndex++;
		}
		else
		{
			g_wsCmdName[nIndex] = __wargv[nArgIndex][nArgBufIndex];
			nArgBufIndex++;
			nIndex++;
		}
	}
	
	nArgIndex++;
	nIndex = 0;
	nArgBufIndex = 0;
	nBufferLen = StrToInt(__wargv[nArgIndex]);
	nArgIndex++;
	if (nBufferLen >= MAX_SRV_PASS)
	{
		MessageBox(NULL, L"引数が不正です", L"error", MB_OK);
		return FALSE;
	}
	while (nIndex < nBufferLen)
	{
		if (!__wargv[nArgIndex][nArgBufIndex])
		{
			g_wsCmdPass[nIndex] = L' ';
			nArgBufIndex = 0;
			nArgIndex++;
			if (nArgIndex >= __argc)
			{
				MessageBox(NULL, L"引数が不正です", L"error", MB_OK);
				return FALSE;
			}
			nIndex++;
		}
		else
		{
			g_wsCmdPass[nIndex] = __wargv[nArgIndex][nArgBufIndex];
			nArgBufIndex++;
			nIndex++;
		}
	}

	return TRUE;
}

// 追加データ
BOOL AddData(WCHAR* path)
{
	WCHAR wsStartPath[_MAX_PATH*2+1];
	WCHAR wsDataPath[_MAX_PATH*2+1];
	SafePrintf(wsStartPath, _MAX_PATH*2, L"%s", __wargv[0]);

	// ファイルパス確認

	int nLen = _tcslen(wsStartPath)-1;
	if (nLen<=0)	return FALSE;
	while(wsStartPath[nLen] != NULL && wsStartPath[nLen] != L'/' && wsStartPath[nLen] != L'\\')
	{
		if (nLen)	nLen--;
		else			return FALSE;
	}
	// exeのフォルダパスを取得
	if (wsStartPath[nLen])		wsStartPath[nLen+1] = NULL;
	else									return FALSE;
	
	SafePrintf(wsDataPath, _MAX_PATH*2, L"%sdata\\", wsStartPath);

	// .zipで判断
	if (_tccmp(&path[_tcslen(path)-4], L".zip") != 0)
		return FALSE;

	// ZipのパスをMBSに変換
	char*	pszZipPath;
	nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,path,-1,NULL,0,NULL,NULL);
	pszZipPath = new char[nLen];

	if(pszZipPath == NULL)
		return FALSE;
	//変換
	nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,path,wcslen(path)+1,pszZipPath,nLen,NULL,NULL);
	if(!nLen)
	{
		SafeDeleteArray(pszZipPath);
		return FALSE;
	}

	// dataのパスをMBSに変換
	char*	pszDataPath;
	nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,wsDataPath,-1,NULL,0,NULL,NULL);
	pszDataPath = new char[nLen];

	if(pszDataPath == NULL)
		return FALSE;
	//変換
	nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,wsDataPath,wcslen(wsDataPath)+1,pszDataPath,nLen,NULL,NULL);
	if(!nLen)
	{
		SafeDeleteArray(pszZipPath);
		return FALSE;
	}

	BOOL res = Unzip(pszZipPath, pszDataPath )?TRUE:FALSE;
	
	SafeDeleteArray(pszZipPath);
	SafeDeleteArray(pszDataPath);
	return res;
}
