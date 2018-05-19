#ifndef H_MAIN___
#define H_MAIN___
// ===================================================================
// main.h
//	main・ヘッダー
// ===================================================================

#include <windows.h>
#include <TCHAR.h>
#include <time.h>
#include <winsock.h>
#include <mmsystem.h>
#include <io.h>
#include <errno.h>
#include "../include/define.h"
#include "../include/types.h"
#include "util.h"

#include "../lib/DXUT/DXUT.h"
#include "../common/CPacketQueue.h"
#include "../common/CCriticalSection.h"

#include <png.h>
#include <lua.hpp>
#include "tolua++.h"
#include "tolua_glue/thg_glue.h"
#include "LuaHelper.h"

#include "../common/CPacketQueue.h"
#include "../common/CCriticalSection.h"


#include "Game.h"

#include "Input.h"
//#include "CGraphic.h"
//#include "CLinkageList.h"
#include "CFiler.h"
#include "TextureLoader.h"
#include "../lib/DXUT/dxstdafx.h"

#include "resource.h"

//#include "Game.h"

//#include "resource.h"

/* =================================== */
/* =====  外部参照グローバル変数 ===== */
/* =================================== */


/* =================================== */
/* ===== 外部参照関数プロトタイプ===== */
/* =================================== */
float ToRadian(float fAngle);
//template <class Type> inline void SafeDelete(Type& obj);

/* =================================== */
/* ===== Lua 参照関数プロトタイプ===== */
/* =================================== */
//void C_ParseMessage(char *str);

/* =================================== */
/* =====     プロトタイプ宣言    ===== */
/* =================================== */


void DrawGame(HWND hWnd);
static void __cdecl RenderGame(LPDIRECT3DDEVICE9 pdev);
void OnKeyDown(HWND hWnd, WORD wKey);
void OnKeyUp(HWND hWnd, WORD wKey);
void OnMouseLBDown(HWND hWnd);
void OnMouseRBDown(HWND hWnd);
void OnActivate(HWND hWnd, UINT wParam);


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );

// main.cpp
BOOL InitSystem();
int EndProgram();

// cli.cpp
BOOL StartClient(int nSocket);
void StopClient(void);

// PacketReceiver.cpp
DWORD __stdcall Thread_PacketReceiver(PVOID param);
// PacketSender.cpp
DWORD __stdcall Thread_PacketSender(PVOID param);
//DWORD __stdcall Thread_Synchronizer(LPVOID param);

#endif
