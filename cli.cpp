
#include "main.h"
#include "ext.h"
//HANDLE g_hThreadSync = NULL;


HANDLE g_hThreadPacketReceiver = NULL;
HANDLE g_hThreadPacketSender = NULL;


void StopThread(HANDLE handle)
{
	DWORD dwParam;
	GetExitCodeThread(handle, &dwParam);
	if (dwParam == STILL_ACTIVE) 
	{
		TerminateThread(handle , FALSE);
		WaitForSingleObject(handle, 100);        /* プロセスの終了を待つ */
		CloseHandle(handle);
	}
}

void StopClient(void)
{
	AddMessageLog(L"StopClient");
	if (g_hThreadPacketReceiver)
		StopThread(g_hThreadPacketReceiver);
	AddMessageLog(L"PacketSender");
	if (g_hThreadPacketSender)
		StopThread(g_hThreadPacketSender);
//	if (g_hThreadSync)
//		StopThread(g_hThreadSync);

	AddMessageLog(L"DeletePacketQueue");
//	DEBUG_DELETE(g_pNetSess, L"StopServer");
	DEBUG_DELETE(g_pPacketQueue, L"Stop_Server");
}

BOOL StartClient(int nSocket)
{
	DWORD		dwThreadId;
/*
	DEBUG_DELETE(g_pNetSess, L"Start_Server");
	DEBUG_NEW(g_pNetSess,CNetworkSession(), L"StartServer" );
	g_pNetSess->InitSesssionArray();
	if (!g_pNetSess->InitSock())
	{
		AddMessageLog(L"ソケット初期化失敗");
		return 0;
	}
*/
	DEBUG_DELETE(g_pPacketQueue, L"Start_Server");
	DEBUG_NEW(g_pPacketQueue, CPacketQueue(), L"StartServer");

//	g_pPPAuth->Init(g_pNetSess);
//	g_pPPChat->Init(g_pNetSess);
//	g_pPPRoom->Init(g_pNetSess);

	dwThreadId=1;
	g_hThreadPacketReceiver=CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)Thread_PacketReceiver,&nSocket,0,&dwThreadId);
	Sleep(200);

	dwThreadId=2;
	g_hThreadPacketSender=CreateThread(NULL,0,Thread_PacketSender,&nSocket,0,&dwThreadId);
	Sleep(200);

//	dwThreadId=3;
//	g_hThreadSync=CreateThread(NULL,0,Thread_Synchronizer,g_pCriticalSection,0,&dwThreadId);
//	Sleep(500);

	return TRUE;
}

