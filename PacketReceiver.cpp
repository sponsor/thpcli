#include "ext.h"
#include "main.h"

BOOL recvall(int sock, char* pkt, int* recvsize, int bufsize);
//
BOOL PacketProc_Chat(BYTE *data);
BOOL PacketProc_Room(BYTE *data);
BOOL PacketProc_Load(BYTE* data);
BOOL PacketProc_Main(BYTE* data);
BOOL PacketProc_File(BYTE *data);

BOOL RecvPacketProc(BYTE *data)
{
	BOOL ret = FALSE;
	switch (data[2])
	{
	case PK_SYN:
		{
			AddMessageLog(L"recv SYN");
			BYTE pkt[MAX_PACKET_SIZE];
			INT packetSize = PacketMaker::MakePacketData_ACK(pkt);
			ret = g_pGame->AddPacket(pkt, packetSize);
			break;
		}
	case PK_USER_AUTH:
		// 認証前のログイン画面なら
		if (g_pGame->GetConnState() == CONN_STATE_EMPTY
			&& g_pGame->GetState() == eGameLogin)
		{
			switch (data[3])
			{
			case AUTH_RESULT_SUCCESS:	// 認証OK
				g_pGame->PlaySysSoundSE(SE_sub_Login);
				AddMessageLog(L"認証結果：AUTH_RESULT_SUCCESS");
				g_pCriticalSection->EnterCriticalSection_Session(L'a');
				g_pGame->SuccessAuth(data);
				if (!g_pGame->InitControl())
				{
					MessageBox(g_hWnd, L"認証結果：データ不足", L"認証結果", MB_OK);
					g_bCloseSocket = TRUE;
				}
				g_pGame->SetState(eGameLoginRelease);
				g_pCriticalSection->LeaveCriticalSection_Session();
				break;
			case AUTH_RESULT_INVALID_PWD:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：不正なパスワード");
				MessageBox(g_hWnd, L"認証結果：不正なパスワード", L"認証結果", MB_OK);
				// ログイン二度押し禁止のため無効にしていたのを解除
				g_SysUI.GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(true);
				g_pGame->m_bLoginStart = FALSE;
				break;
			case AUTH_RESULT_INVALID_USER_NAME:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：ユーザ名が短すぎる、または長すぎます");
				MessageBox(g_hWnd, L"認証結果：ユーザ名が短すぎる、または長すぎます", L"認証結果", MB_OK);
				// ログイン二度押し禁止のため無効にしていたのを解除
				g_SysUI.GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(true);
				g_pGame->m_bLoginStart = FALSE;
				break;
			case AUTH_RESULT_NO_UNIQUE_USER:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：既に同じユーザ名がログインしています");
				MessageBox(g_hWnd, L"認証結果：既に同じユーザ名がログインしています", L"認証結果", MB_OK);
				// ログイン二度押し禁止のため無効にしていたのを解除
				g_SysUI.GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(true);
				g_pGame->m_bLoginStart = FALSE;
				break;
			case AUTH_RESULT_GAME_PHASE:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：現在ゲーム中のため入れません");
				MessageBox(g_hWnd, L"認証結果：現在ゲーム中のため入れません", L"認証結果", MB_OK);
				// ログイン二度押し禁止のため無効にしていたのを解除
				g_SysUI.GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(true);
				g_pGame->m_bLoginStart = FALSE;
				break;
			case AUTH_RESULT_GAME_LOAD:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：ゲームが開始されたため入れません");
				MessageBox(g_hWnd, L"認証結果：ゲームが開始されたため入れません", L"認証結果", MB_OK);
				// ログイン二度押し禁止のため無効にしていたのを解除
				g_SysUI.GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(true);
				g_pGame->m_bLoginStart = FALSE;
				break;
			case AUTH_RESULT_GAME_INVALID:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：サーバが閉じられている可能性があります");
				MessageBox(g_hWnd, L"認証結果：サーバが閉じられている可能性があります", L"認証結果", MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_TIMEOUT:
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				AddMessageLog(L"認証結果：ログインタイムアウトしました");
				MessageBox(g_hWnd, L"認証結果：ログインタイムアウトしました", L"認証結果", MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_INVALID_SCR_CHARA_COUNT:
				AddMessageLog(L"キャラスクリプト不足");
				MessageBox(g_hWnd, L"サーバに接続するためのキャラスクリプトが不足しています。", L"スクリプトチェック", MB_ICONINFORMATION|MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_INVALID_SCR_STAGE_COUNT:
				AddMessageLog(L"ステージスクリプト不足");
				MessageBox(g_hWnd, L"サーバに接続するためのステージスクリプトが不足しています。", L"スクリプトチェック", MB_ICONINFORMATION|MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_INVALID_SCR_CHARA_NOT_FOUND:
				AddMessageLog(L"キャラスクリプト不足");
				{
					BYTE group = data[4];
					WORD id = 0;
					memcpy(&id, &data[5], sizeof(WORD));
					WCHAR log[128];
					SafePrintf(log, 128, L"サーバに接続するためのキャラスクリプトが見つかりませんでした(ID=%d)", id);
					MessageBox(g_hWnd, log, L"スクリプトチェック", MB_ICONINFORMATION|MB_OK);
				}
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_INVALID_SCR_STAGE_NOT_FOUND:
				AddMessageLog(L"ステージスクリプト不足");
				{
					BYTE group = data[4];
					WORD id = 0;
					memcpy(&id, &data[5], sizeof(WORD));
					WCHAR log[128];
					SafePrintf(log, 128, L"サーバに接続するためのステージスクリプトが見つかりませんでした(ID=%d)", id);
					MessageBox(g_hWnd, log, L"スクリプトチェック", MB_ICONINFORMATION|MB_OK);
				}
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_INVALID_HASH:
				{
					WCHAR* pName = NULL;
					WCHAR alog[128];
#if TRIAL==0
					WCHAR nlog[128];
#endif
					BYTE group = data[4];
					WORD id = 0;
					memcpy(&id, &data[5], sizeof(WORD));
					if (group == 0)
					{
						pName = g_pGame->GetScrName(id, FALSE);
						SafePrintf(alog, 128, L"認証結果：サーバのデータとクライアントのデータが一致しませんでした\n（ステージスクリプトID[%d][%s]のデータ不一致）\nサーバにご確認ください", id, pName);
#if TRIAL==0
						SafePrintf(nlog, 128, L"サーバが使用しているステージスクリプトID[%d][%s]を要求しますか？\n(「はい」を押すと受信完了まで応答が無くなります)", id, pName);
#endif
					}
					else
					{
						pName = g_pGame->GetScrName(id, TRUE);
						SafePrintf(alog, 128, L"認証結果：サーバのデータとクライアントのデータが一致しませんでした\n（キャラクタスクリプトID[%d][%s]のデータ不一致）\nサーバにご確認ください", id, pName);
#if TRIAL==0
						SafePrintf(nlog, 128, L"サーバが使用しているキャラクタスクリプトID[%d][%s]を要求しますか？\n(「はい」を押すと受信完了まで応答が無くなります)", id, pName);
#endif
					}
					g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
					AddMessageLog(alog);
					MessageBox(g_hWnd, alog, L"認証結果", MB_OK);
#if TRIAL==0
					if (g_bCloseSocket) return FALSE;
					if (id >= 100 && MessageBox(g_hWnd, nlog, L"データ不一致", MB_YESNO) == IDYES)
					{
						if (g_bCloseSocket) return FALSE;
						BYTE pkt[MAX_PACKET_SIZE];
						INT pktsize = PacketMaker::MakePacketData_ReqFileHash(group==0, id, 0, pkt);
						return g_pGame->AddPacket(pkt, pktsize);
					}
					else
#endif
					{
						g_bCloseSocket = TRUE;
						ret = TRUE;
					}
					break;
				}
			case AUTH_RESULT_FILESEND_ERROR:
				g_pGame->DeleteRecvScrFile();
				AddMessageLog(L"ファイル受信エラー");
				MessageBox(g_hWnd, L"ファイル受信中エラーが発生しました。\n受信したスクリプトファイルのみ削除します。", L"データ受信", MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_FILESEND_END:
				AddMessageLog(L"データ受信終わり");
				MessageBox(g_hWnd, L"データ受信が終わりました。\nもう一度起動し接続してください", L"データ受信", MB_ICONINFORMATION|MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			case AUTH_RESULT_FILESEND_DISABLE:
				AddMessageLog(L"送信不可のデータ");
				MessageBox(g_hWnd, L"送信不可のデータです。", L"データ受信", MB_ICONINFORMATION|MB_OK);
				g_bCloseSocket = TRUE;
				ret = TRUE;
				break;
			default:
				break;
			}
		}
		break;
	case PK_REQ_HASH:
		ret = g_pGame->ReqPacketHash(data);
		break;
	case PK_USER_CHAT:	// チャットパケット
		ret = PacketProc_Chat(data);
		break;
	case PK_USER_ROOMINFO:
		ret = PacketProc_Room(data);
		break;
	case PK_USER_LOAD:
		ret = PacketProc_Load(data);
		break;
	case PK_REQ_LOAD_COMPLETE:
		ret = g_pGame->ReqLoadComplete();
		break;
	case PK_USER_MAININFO:
		//> 20110220 EnterCriticalSection_Session追加
		g_pCriticalSection->EnterCriticalSection_Session(L'M');
		ret = PacketProc_Main(data);
		g_pCriticalSection->LeaveCriticalSection_Session();
		//< 20110220
		break;
	case PK_USER_DISCON:
		g_pCriticalSection->EnterCriticalSection_Session(L's');
		g_pGame->OnDisconnectUser(data[3]);
		g_pCriticalSection->LeaveCriticalSection_Session();
		break;
	case PK_USER_CONFIRMED:
		{
			OutputDebugStr(L"PK_USER_CONFIRMED");
			g_pCriticalSection->EnterCriticalSection_Session(L'D');
//			g_pGame->SetUserIndex(data[3]);
			ptype_session sess = g_pGame->GetAuthedSessionInfo(data[3]);
			if (!sess)
			{
				g_pCriticalSection->LeaveCriticalSection_Session();
				break;
			}
			memcpy(&sess->obj_state, &data[4], sizeof(E_TYPE_OBJ_STATE));
			g_pGame->GetMySessionInfo()->game_ready = 0;
			g_pGame->SetState(eGameResultRelease);
			g_pCriticalSection->LeaveCriticalSection_Session();
		break;
		}
	case PK_USER_FILEINFO:
		PacketProc_File(data);
		break;
	default:
		break;
	}
	return ret;
}

DWORD __stdcall Thread_PacketReceiver(PVOID param)
{
	BOOL bAuth = FALSE;
	const int c_nPacketRecvCount = 8;
	const int c_nPacketStockCount = 64;
	BYTE message[MAX_PACKET_SIZE*c_nPacketRecvCount];
	BYTE messa[c_nPacketStockCount][MAX_PACKET_SIZE];
//	BYTE msg[MAX_PACKET_SIZE];
	WORD rPsize[c_nPacketStockCount];
	int nPacketProcIndex = 0;
	int nPacketStockIndex = 0;
	BOOL bStock = FALSE;

	int *pSockNo = (int*)(param);
	int	nSockNo = *pSockNo;
	fd_set	rfds, efds;

	unsigned short	nEnd = PEND,rEnd;
	int nDivIndex = 0;
	int nRecvStockSize = 0;
	int packetCount = 0; 

	if (!nSockNo)
		return 0;
	g_bCloseSocket = FALSE;

	while(TRUE)
	{
		FD_ZERO(&rfds);
		FD_ZERO(&efds);
		FD_SET(nSockNo, &rfds);

		memset(message,0,64);
		FD_ZERO(&rfds);
		int maxfds = nSockNo+1;
		FD_ZERO(&efds);
	//		FD_SET(0, &read_fds);
		FD_SET(nSockNo, &rfds);

		if(select(nSockNo+1, &rfds, NULL, &efds, (struct timeval *)0) < 0)
		{
			AddMessageLog(L"select:closesocket");
			closesocket(nSockNo);
#ifndef _DEBUG
			g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
			if (g_pGame->GetState() & (GAME_STATE_LOGIN))
				MessageBox(g_hWnd, L"接続数が満員です。\nクライアントを終了します。", L"connect error", MB_OK);
			else
				MessageBox(g_hWnd, L"サーバとの接続が切断されました。\nクライアントを終了します。", L"connect error", MB_OK);
#endif
			g_bCloseSocket = TRUE;
			g_pGame->SetState( (EGameState)(((DWORD)g_pGame->GetState()&GAME_STATE_PHASE_MASK)|GAME_STATE_RELEASE));
//			CloseWindow(g_hWnd);
//> 20110301
//			DestroyWindow(g_hWnd);
//< 20110301
//			DXUTShutdown(DXUTGetExitCode());
			return 0;
		}

		if (FD_ISSET(nSockNo, &rfds))
		{
			int recv_size=0;
//			if ((recv_size = recv(nSockNo, (char*)&message[0], MAX_PACKET_SIZE,0)) > 0)
			if (recvall(nSockNo, (char*)message, &recv_size, sizeof(message)-nRecvStockSize))
			{
				if (recv_size == 0)
				{
					AddMessageLog(L"サイズ0Byte受信");
					continue;
				}

				// ストックが0ならサイズ取得
				bStock = (nRecvStockSize>0);
				if (!bStock)
					memcpy(&rPsize[nPacketStockIndex],message,2);

				packetCount = 0;
				// パケットストックしてない
				if (!bStock)
				{
					// 1パケットのみ受信
					if( recv_size==rPsize[nPacketStockIndex])
					{
						memset(messa[nPacketStockIndex],0,MAX_PACKET_SIZE);
						memcpy(messa[nPacketStockIndex],message,rPsize[nPacketStockIndex]);
						memcpy(&rEnd,&messa[nPacketStockIndex][rPsize[nPacketStockIndex] -2],2);
						if(rEnd != nEnd)	// パケット末尾コードチェック
						{
							WCHAR msglog[64];
							SafePrintf(msglog, 64, L"パケット末尾にエラー...size=%d\n", (int)rPsize[nPacketProcIndex]);
							AddMessageLog(msglog);
							continue;
						}
						nRecvStockSize = 0;	// ストックなし
						packetCount=1;			// 1パケット
						nPacketStockIndex = (nPacketStockIndex+1)%c_nPacketStockCount;
					}
					else
					{
						packetCount=0;
						nDivIndex=0;
						nRecvStockSize = recv_size;
						for(;;)
						{
							if(nRecvStockSize <= 0)	break;
							if (rPsize[nPacketStockIndex] > nRecvStockSize)	// 受信バッファよりパケットが大きい
							{
								// 受信バッファを先頭に移動する必要がある
								memset(messa[nPacketStockIndex],0,MAX_PACKET_SIZE);
								memcpy(messa[nPacketStockIndex], &message[nDivIndex], nRecvStockSize);
								break;
							}
							memset(messa[nPacketStockIndex],0,MAX_PACKET_SIZE);
							memcpy(messa[nPacketStockIndex],&message[nDivIndex],rPsize[nPacketStockIndex]);
							memcpy(&rEnd,&messa[nPacketStockIndex][ (int)(rPsize[nPacketStockIndex] -2)],2);
							if(rEnd != nEnd)	// パケット末尾コードチェック
							{
								WCHAR msglog[MAX_MSG_BUFFER*4+1];
								SafePrintf(msglog, MAX_MSG_BUFFER*4+1, L"パケット末尾にエラー(分割中エラー)...size=%d/index=%d/stock=%d", rPsize[nPacketStockIndex],nDivIndex,nRecvStockSize);
								AddMessageLog(msglog);
								g_pGame->GenerateDatMsg(msglog, (BYTE*)messa[nPacketStockIndex], rPsize[nPacketStockIndex]);
								AddMessageLog(msglog);
								g_pGame->GenerateDatMsg(msglog, (BYTE*)&message[nDivIndex], nRecvStockSize);
								AddMessageLog(msglog);
								nRecvStockSize = 0;
								break;
							}
							nDivIndex += rPsize[nPacketStockIndex];
							nRecvStockSize -= rPsize[nPacketStockIndex];
							packetCount++;
							nPacketStockIndex = (nPacketStockIndex+1)%c_nPacketStockCount;
							// 次のパケットサイズ
							memcpy(&rPsize[nPacketStockIndex],&message[nDivIndex],2);
						}
					}
				}
				else	// パケットストック中
				{
					// ストック+受信サイズが1パケット分
					if( (nRecvStockSize+recv_size)==rPsize[nPacketStockIndex])
					{
						memcpy(&messa[nPacketStockIndex][nRecvStockSize],message,rPsize[nPacketStockIndex]-nRecvStockSize);
						memcpy(&rEnd,&messa[nPacketStockIndex][rPsize[nPacketStockIndex] -2],2);
						if(rEnd != nEnd)	// パケット末尾コードチェック
						{
							WCHAR msglog[64];
							SafePrintf(msglog, 64, L"パケット末尾にエラー...size=%d\n", (int)rPsize[nPacketProcIndex]);
							AddMessageLog(msglog);
							continue;
						}
						nRecvStockSize = 0;	// ストックなし
						packetCount=1;			// 1パケット
						nPacketStockIndex = (nPacketStockIndex+1)%c_nPacketStockCount;
					}
					else	// パケット分割
					{
						packetCount=0;
						nDivIndex=0;
						int nRecvStockIndex = nRecvStockSize;
						nRecvStockSize = recv_size;
						for (;;)
						{
							if(nRecvStockSize <= 0)	break;
							if ((rPsize[nPacketStockIndex]-nRecvStockIndex) > (nRecvStockSize))	// 受信バッファよりパケットが大きい
							{
								// 受信バッファを先頭に移動する必要がある
								memcpy(&messa[nPacketStockIndex][nRecvStockIndex], &message[nDivIndex], nRecvStockSize);
								nRecvStockSize += nRecvStockIndex;
								break;
							}
//							memset(messa[nPacketStockIndex],0,MAX_PACKET_SIZE);
							memcpy(&messa[nPacketStockIndex][nRecvStockIndex],&message[nDivIndex],rPsize[nPacketStockIndex]-nRecvStockIndex);
							memcpy(&rEnd,&messa[nPacketStockIndex][ (int)(rPsize[nPacketStockIndex] -2)],2);
							if(rEnd != nEnd)	// パケット末尾コードチェック
							{
								WCHAR msglog[MAX_MSG_BUFFER*4+1];
								SafePrintf(msglog, MAX_MSG_BUFFER*4+1, L"パケット末尾にエラー(分割中エラー)...size=%d/index=%d/stock=%d", rPsize[nPacketStockIndex],nDivIndex,nRecvStockSize);
								AddMessageLog(msglog);
								g_pGame->GenerateDatMsg(msglog, (BYTE*)messa[nPacketStockIndex], rPsize[nPacketStockIndex]);
								AddMessageLog(msglog);
								g_pGame->GenerateDatMsg(msglog, (BYTE*)&message[nDivIndex], nRecvStockSize);
								AddMessageLog(msglog);
								nRecvStockSize = 0;
								break;
							}
							nDivIndex += rPsize[nPacketStockIndex];
							nRecvStockIndex = 0;
							nRecvStockSize -= rPsize[nPacketStockIndex];
							packetCount++;
							nPacketStockIndex = (nPacketStockIndex+1)%c_nPacketStockCount;
							// 次のパケットサイズ
							memcpy(&rPsize[nPacketStockIndex],&message[nDivIndex],2);
						}
					}
				}

				for(int j=0;j<packetCount;j++)
//				for(int j=0;j<c_nPacketProcCount;j++)
				{
//					if (nPacketStockIndex == nPacketProcIndex)
//						break;
///					WCHAR msglog[64];
///					SafePrintf(msglog, 64, L"nPacketProcIndex=%d", (int)nPacketProcIndex);
///					AddMessageLog(msglog);
//					memcpy(&msg[0],&messa[nPacketProcIndex][0],rPsize[nPacketProcIndex]);
//					RecvPacketProc((BYTE*)&msg[0]);
					RecvPacketProc((BYTE*)&messa[nPacketProcIndex][0]);
					nPacketProcIndex = (nPacketProcIndex+1)%c_nPacketStockCount;
//					Sleep(1);
				}
			}
			else
			{
				AddMessageLog(L"recv:closesocket");
//#ifndef _DEBUG
				g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
				if (g_pGame->GetState() & (GAME_STATE_LOGIN|GAME_STATE_TITLE))
					MessageBox(g_hWnd, L"接続人数が満員です。\nクライアントを終了します。", L"connect error", MB_OK);
				else
					MessageBox(g_hWnd, L"サーバとの接続が切断されました。\nクライアントを終了します。", L"connect error", MB_OK);
//#endif
				
				g_bCloseSocket = TRUE;
				g_pGame->SetState( (EGameState)(((DWORD)g_pGame->GetState()&GAME_STATE_PHASE_MASK)|GAME_STATE_RELEASE));
//				CloseWindow(g_hWnd);
//> 20110301
//				DestroyWindow(g_hWnd);
//< 20110301
//				DXUTShutdown(DXUTGetExitCode()); return 0;
			}
		}

	}
	return 1;
}


BOOL recvall(int sock, char* pkt, int* recvsize, int bufsize)
{
	int nBufferSize = bufsize;
	*recvsize=0;
	int nRecvd = 0;
	//> パケット受信
	while (SOCKET_ERROR != (nRecvd = recv(sock, &pkt[*recvsize], nBufferSize, 0)))
	{
		// ソケットが閉じられた
		if (!nRecvd)
		{
			if (g_bDebug)
			{
				DWORD dwErrCode = WSAGetLastError();
				WCHAR pc[16];
				SafePrintf(pc, 16, L"LastErr:%d", dwErrCode);
				AddMessageLog(pc);
			}
			return FALSE;
		}
//		// 受信0
//		if (!nRecvd)
//			break;
		// 受信残りがある
		*recvsize += nRecvd;
		nBufferSize -= nRecvd;
	}
	DWORD dwErrCode = WSAGetLastError();
	// エラー
	if(dwErrCode && WSAEWOULDBLOCK != dwErrCode)
	{
#if ADD_WSAERROR_LOG
		if (g_bDebug)
		{
			WCHAR pc[16];
			SafePrintf(pc, 16, L"LastErr:%d", dwErrCode);
			AddMessageLog(pc);
		}
#endif
		return FALSE;
	}
	//< パケット受信
	return TRUE;
}

// チャットパケット処理
BOOL PacketProc_Chat(BYTE *data)
{
// PK_USER_CHAT
// size				: 2 0
// header			: 1 2
// chat_header	: 1 3
// obj_no			: 1 4
// name_len		: 1 5
// name			: name_len 6
// message_len	: 1 name_len+1
// message		: message_len name_len+2
	WCHAR addmsg[MAX_CHAT_MSG+MAX_USER_NAME+3];
	WCHAR name[MAX_USER_NAME+1];
	WCHAR chatmsg[MAX_CHAT_MSG+1];
	int nCharaIndex = data[4];
	int nIndex = 5;
	int nNameLen = DecodeSizeStrFromRaw(name, &data[nIndex], (MAX_USER_NAME+1)*sizeof(WCHAR), (MAX_USER_NAME+1)*sizeof(WCHAR));
	name[nNameLen/sizeof(WCHAR)] = NULL;	
	nIndex += nNameLen+1;
	int nChatLen = DecodeSizeStrFromRaw(chatmsg, &data[nIndex], (MAX_CHAT_MSG+1)*sizeof(WCHAR), (MAX_CHAT_MSG+1)*sizeof(WCHAR));
	chatmsg[nChatLen/sizeof(WCHAR)] = NULL;	
	nIndex += nChatLen+1;

	if (!nChatLen)
	{
		AddMessageLog(L"無効なチャットパケットを受信");
		return FALSE;
	}
//		swprintf_s(addmsg, nNameLen+nChatLen+3, L"%s: %s", name, chatmsg);
	SafePrintf(addmsg, (nNameLen+nChatLen) / sizeof(WCHAR) + 3, L"%s: %s", name, chatmsg);

	switch ( (E_TYPE_PACKET_CHAT_HEADER) data[3] )
	{
	case PK_USER_CHAT_ALL:
		g_pGame->AddChatMessage(addmsg, PK_USER_CHAT_ALL);
		g_pGame->PlaySysSoundSE(SE_sai_Chat);
		break;
	case PK_USER_CHAT_TEAM:
		g_pGame->AddChatMessage(addmsg, PK_USER_CHAT_TEAM);
		g_pGame->PlaySysSoundSE(SE_sai_Chat);
		break;
	case PK_USER_CHAT_SERVER_INFO:
		g_pGame->AddChatMessage(addmsg, PK_USER_CHAT_SERVER_INFO);
		break;
	case PK_USER_CHAT_SERVER_WARNING:
		g_pGame->AddChatMessage(addmsg, PK_USER_CHAT_SERVER_WARNING);
		break;
	default:
	case PK_USER_CHAT_WIS:
		g_pGame->AddChatMessage(addmsg, PK_USER_CHAT_WIS);
		g_pGame->PlaySysSoundSE(SE_sai_Chat);
		break;
	}

	g_pGame->CharacterSay(nCharaIndex, chatmsg);

	return TRUE;
}

BOOL PacketProc_Load(BYTE* data)
{
	//> pakcet
	// size			:	2
	// header		:	1
	// teams		:	1
	// stage			:	1
	// rule			:	1
	// user_count:	users
	//> users
	// obj_no		:	1
	// chara_type	:	1
	// team_no	:	1
	//< users
	//	msg			:	msg_len
	int nIndex = 3;
	// teams
	g_pGame->SetTeamCount((int)data[nIndex]);
	nIndex++;
	// stage
	g_pGame->SetStageIndex((int)data[nIndex]);
	nIndex++;
	// rule
	g_pGame->SetRule(data[nIndex]);
	nIndex++;

	int nAuthedUserCount = data[nIndex];
	nIndex++;
	for (int i=0;i<nAuthedUserCount;i++)
	{
		int nUserIndex = data[nIndex];
		nIndex++;
		ptype_session sess = g_pGame->GetAuthedSessionInfo(nUserIndex);
		if (!sess)	continue;
		if (sess->chara_type == ROOM_CHARA_RANDOM_ID)
		{
			sess->chara_type = data[nIndex];
			if (nUserIndex == g_pGame->GetUserIndex())
			{
#if RTRIAL
				g_bCloseSocket = TRUE;	// 終了
#else
				g_pGame->SetChacterSelectInfo(sess->chara_type);
#endif
			}

			g_pGame->UpdateRoomCharacter(nUserIndex);
			g_pGame->UpdateUserList(nUserIndex);
		}
		else
			sess->chara_type = data[nIndex];
		nIndex++;
		sess->team_no = data[nIndex];
		nIndex++;
		sess->obj_state = OBJ_STATE_LOADING;

		sess->live_count = 0;
		// 異常状態などクリア
		ZeroMemory(sess->chara_state, sizeof(char)* CHARA_STATE_COUNT);
	}
	BOOL ret = g_pGame->LoadStageCharacter();
	g_pGame->SetState(eGameRoomRelease);
	return ret;
}
