#include "ext.h"
#include "Game.h"
// parts
// CGame::xxx > Funcsions

void CGame::GenerateDatMsg(WCHAR* msglog, BYTE* data, WORD size)
{
	int nMsgIndex = 0;
	int nBuf = MAX_MSG_BUFFER;

	for (int i=0;i<size;i++)
	{
		_itow_s((int)data[i], &msglog[nMsgIndex], nBuf, 16);
		if (data[i] < 16)
			msglog[nMsgIndex+1] = L' ';
		msglog[nMsgIndex+2] = L' ';
		nMsgIndex+=3;
		nBuf-=3;
		if (size > 1 && nBuf <= 4)
		{
			msglog[nMsgIndex] = L'.';
			msglog[nMsgIndex+1] = L'.';
			msglog[nMsgIndex+2] = L'\n';
			msglog[nMsgIndex+3] = NULL;
			return;
		}
	}
	msglog[nMsgIndex] = '\n';
	msglog[nMsgIndex+1] = NULL;
}

//> セッションへ送るパケット作成
BOOL CGame::AddPacket(BYTE* data, WORD size)
{
	if (!m_nTcpSock || !data || size > MAX_PACKET_SIZE)
		return FALSE;
	ptype_packet ppkt = NewPacket();
	if (!ppkt) return FALSE;

	ppkt->cli_sock = m_nTcpSock;
	ppkt->session = NULL;
	ppkt->size = size;

	ZeroMemory(ppkt->data,sizeof(char)*MAX_PACKET_SIZE);
	CopyMemory(ppkt->data,data,size);
	g_pCriticalSection->EnterCriticalSection_Packet(L'2');

#ifdef _DEBUG
//#if ADD_LOG_PACKET_INFO
	if (!g_pPacketQueue->EnqueueRaw(ppkt))
	{
		AddMessageLog(L"パケットキュー追加失敗");
		switch (data[2])
		{
		case PK_SYN:
			AddMessageLog(L"PK_SYN");	break;
		case PK_ACK:
			AddMessageLog(L"PK_ACK");	break;
		case PK_NOOP:
			AddMessageLog(L"PK_NOOP");	break;
		case PK_USER_AUTH:
			AddMessageLog(L"PK_USER_AUTH");	break;
		case PK_USER_CHAT:
			AddMessageLog(L"PK_USER_CHAT");	break;
		case PK_USER_ROOMINFO:
			AddMessageLog(L"PK_USER_ROOMINFO");	break;
		case PK_USER_LOAD:
			AddMessageLog(L"PK_USER_LOAD");	break;
		case PK_USER_MAININFO:
			AddMessageLog(L"LOG_PK_USER_MAININFO");	break;
		case PK_REQ_LOC:
			AddMessageLog(L"PK_REQ_LOC");	break;
		case PK_CMD_MV:
			AddMessageLog(L"PK_CMD_MV");	break;
		case PK_CMD_ROOM_CHARA_SEL:
			AddMessageLog(L"PK_CMD_ROOM_CHARA_SEL");	break;
		case PK_CMD_ROOM_READY:
			AddMessageLog(L"PK_CMD_ROOM_READY");	break;
		case PK_CMD_ROOM_RULE:
			AddMessageLog(L"PK_CMD_ROOM_RULE");	break;
		case PK_CMD_ROOM_MV:
			AddMessageLog(L"PK_CMD_ROOM_MV");	break;
		case PK_CMD_ROOM_ITEM_SEL:
			AddMessageLog(L"PK_CMD_ROOM_ITEM_SEL");	break;
		case PK_CMD_ROOM_TEAM_COUNT:
			AddMessageLog(L"PK_CMD_ROOM_TEAM_COUNT");	break;
		case PK_CMD_ROOM_STAGE_SEL:
			AddMessageLog(L"PK_CMD_ROOM_STAGE_SEL");	break;
		case PK_CMD_LOAD_COMPLETE:
			AddMessageLog(L"PK_CMD_LOAD_COMPLETE");	break;
		case PK_CMD_MAIN_MV:
			AddMessageLog(L"PK_CMD_MAIN_MV\n");	break;
		case PK_OBJ_UPDATE_ACT:
			AddMessageLog(L"PK_OBJ_UPDATE_ACT");	break;
		case PK_OBJ_REMOVE:
			AddMessageLog(L"PK_OBJ_REMOVE");	break;
		case PK_USER_DISCON:
			AddMessageLog(L"PK_USER_DISCON");	break;
		}
		WCHAR	msglog[MAX_MSG_BUFFER*4+1];
		GenerateDatMsg(msglog, data, size);
		AddMessageLog(msglog);
	}
//#endif
#else
	g_pPacketQueue->EnqueueRaw(ppkt);
#endif
	g_pCriticalSection->LeaveCriticalSection_Packet();
//		EnqueuePacket(&m_tQueue, ppkt);
	return TRUE;
}
//< セッションへ送るパケット作成

// チャットにメッセージ追加
void CGame::AddChatMessage(WCHAR *msg, E_TYPE_PACKET_CHAT_HEADER chat)
{
	HRESULT hr;
	if (!p_pUI || !msg) return;
	
	CDXUTColorListBox *pColorListBox = (CDXUTColorListBox*)p_pUI->GetControl(IDC_ROOM_LB_CHATLOG);
	if (!pColorListBox) return;

	BYTE bytColorElement = 1;
	switch (chat)
	{
	case PK_USER_CHAT_TEAM:					// チームへ
		bytColorElement = 3;
		break;
	case PK_USER_CHAT_SERVER_INFO:			// サーバから情報
		bytColorElement = 4;
		AddMessageLog(msg);
		break;
	case PK_USER_CHAT_SERVER_WARNING:	// サーバから注意
		bytColorElement = 5;
		AddMessageLog(msg);
		break;
	case PK_USER_CHAT_ALL:						// 全員へ
		bytColorElement = 2;
		break;
	case PK_USER_CHAT_WIS:
	default:
		bytColorElement = 6;							// 個人
		break;
	}
	V(pColorListBox->AddItem(msg, NULL, bytColorElement));
	(pColorListBox->GetScrollBar())->SetTrackPos(pColorListBox->GetSize());
}

int CGame::AddResourceTexture(WCHAR* path)
{
	LPDIRECT3DTEXTURE9 pTex = NULL;
	int nImageWidth,nImageHeight;
	// リソースマネージャとダイアログにテクスチャを追加
	int nNewTextureNodeIndex = g_DialogResourceManager.AddTexture(L"");
	p_pUI->SetTexture(nNewTextureNodeIndex, L"");
	if (FAILED(LoadTextureFromFiler(path, &pTex, &nImageWidth, &nImageHeight)))
	{
		return -1;
	}
	
	DXUTTextureNode* pTextureNode = g_DialogResourceManager.GetTextureNode(nNewTextureNodeIndex);
	SafeRelease(pTextureNode->pTexture);
	pTextureNode->pTexture = pTex;
	pTextureNode->dwWidth = nImageWidth;
	pTextureNode->dwHeight = nImageHeight;
	wcscpy_s(pTextureNode->strFilename, MAX_PATH, path);
	return nNewTextureNodeIndex;
}

// Login
void CGame::SuccessAuth(BYTE* data)
{
	SetUserIndex(data[4]);
	SetConnState(CONN_STATE_AUTHED);
	SetUserState(OBJ_STATE_ROOM_READY);
	m_SessionArray[m_nUserIndex].master = data[9];
	m_SessionArray[m_nUserIndex].team_no = data[10];
	SetMaxLoginNum(data[11]);
	g_nMaxLoginNum = data[11];
	memcpy(&m_SessionArray[m_nUserIndex].obj_state, &data[5], sizeof(DWORD));
	CreateCharacters();
}

// chat
BYTE CGame::GetChatMessageRange()
{
	DXUTComboBoxItem *pItem = ((CDXUTComboBox*)p_pUI->GetControl(IDC_ROOM_CMB_CHAT))->GetSelectedItem();
	if (!pItem)	return (BYTE)PK_USER_CHAT_NONE;
	DWORD pb = (DWORD)pItem->pData;

	// CTRLキー押下しているならチームチャットにする
	if (GetAsyncKeyState(VK_CONTROL)&0x8000)
		return (BYTE)PK_USER_CHAT_TEAM;
	// ALTキー押下しているなら全体チャットにする
	if (GetAsyncKeyState(VK_MENU)&0x8000)
		return (BYTE)PK_USER_CHAT_ALL;

	// 発言範囲コンボボックスの選択
	switch ((E_TYPE_PACKET_CHAT_HEADER)pb )
	{
	case PK_USER_CHAT_ALL:
		return (BYTE)PK_USER_CHAT_ALL;
	case PK_USER_CHAT_TEAM:
		return (BYTE)PK_USER_CHAT_TEAM;
	default:
		return (BYTE)pb;	// pDataのユーザIDを返す
//		break;
	}
	return (BYTE)PK_USER_CHAT_NONE;
}

// 切断通知
void CGame::OnDisconnectUser(int nCharaIndex)
{
	WCHAR wsName[MAX_USER_NAME+1];
	WCHAR wsMessage[MAX_CHAT_MSG+1];

	//> チャットログに切断メッセージ追加
	common::session::GetSessionName(&m_SessionArray[nCharaIndex], wsName);
	SafePrintf(wsMessage, MAX_CHAT_MSG, L"%sさんが切断しました", wsName);
	CDXUTListBox* pListBox = (CDXUTListBox*)p_pUI->GetControl(IDC_ROOM_LB_CHATLOG);
	AddChatMessage(wsMessage, PK_USER_CHAT_SERVER_WARNING);
	AddMessageLog(wsMessage);
	//< チャットログに切断メッセージ追加

	////////////////////////////
	m_SessionArray[nCharaIndex].connect_state = CONN_STATE_EMPTY;
	m_nAuthedCount = CalcAuthedUserCount();
	switch (m_eGameState)
	{
	case eGameRoomInit:
	case eGameRoom:
	case eGameRoomRelease:
		m_SessionArray[nCharaIndex].entity = 0;
		m_SessionArray[nCharaIndex].game_ready = 0;
		m_SessionArray[nCharaIndex].frame_count = 0;
		m_SessionArray[nCharaIndex].live_count = 0;

		// アバターキャラ削除
		m_pRoomCharacters[nCharaIndex]->Destroy();
		// ユーザーリスト更新
		UpdateUserList(nCharaIndex);
		// 個人チャットコンボボックス更新
		UpdateWisperList(nCharaIndex);
		m_pTeamRulePropertyManager->Update((GetMySessionInfo()->master!=0), m_nTeamCount);

		UpdateReadyButtonState();
		break;
	case eGameLoad:
	case eGameLoadInit:
	case eGameLoadRelease:
		// 既に死んでいる場合は設定しないようにチェック
		if (!(m_SessionArray[nCharaIndex].obj_state & OBJ_STATE_MAIN_NOLIVE_FLG))
		{
			m_SessionArray[nCharaIndex].obj_state = OBJ_STATE_EMPTY;
			OnCharaDisconnect(nCharaIndex);		// キャラを横たわらせる
		}
		//> 20110201 部屋に切断したユーザの何かが残っている
		// アバターキャラ削除
		m_pRoomCharacters[nCharaIndex]->Destroy();
		//< 20110201 部屋に切断したユーザの何かが残っている
		// ユーザーリスト更新
		UpdateUserList(nCharaIndex);
		// 個人チャットコンボボックス更新
		UpdateWisperList(nCharaIndex);
		EraseDisconnectCharacter(nCharaIndex);
		break;
	case eGameMainInit:
	case eGameMain:
	case eGameMainRelease:
		// 既に死んでいる場合は設定しないようにチェック
		if (!(m_SessionArray[nCharaIndex].obj_state & OBJ_STATE_MAIN_NOLIVE_FLG))
		{
			m_SessionArray[nCharaIndex].obj_state = OBJ_STATE_MAIN_DEAD;
			OnCharaDisconnect(nCharaIndex);		// キャラを横たわらせる
		}
		//> 20110201 部屋に切断したユーザの何かが残っている
		// アバターキャラ削除
		m_pRoomCharacters[nCharaIndex]->Destroy();
		//< 20110201 部屋に切断したユーザの何かが残っている
		// ユーザーリスト更新
		UpdateUserList(nCharaIndex);
		// 個人チャットコンボボックス更新
		UpdateWisperList(nCharaIndex);
		EraseDisconnectCharacter(nCharaIndex);
		break;
	case eGameResult:
	case eGameResultRelease:
		//> 20110201 部屋に切断したユーザの何かが残っている
		// アバターキャラ削除
		m_pRoomCharacters[nCharaIndex]->Destroy();
		//< 20110201 部屋に切断したユーザの何かが残っている
		m_SessionArray[nCharaIndex].entity = 0;
		m_SessionArray[nCharaIndex].game_ready = 0;
		m_SessionArray[nCharaIndex].frame_count = 0;
		m_SessionArray[nCharaIndex].live_count = 0;
		// 個人チャットコンボボックス更新
		UpdateWisperList(nCharaIndex);
		// ユーザーリスト更新
		UpdateUserList(nCharaIndex);
		break;
	case eGameResultInit:
	default:
		//> 20110201 部屋に切断したユーザの何かが残っている
		// アバターキャラ削除
		m_pRoomCharacters[nCharaIndex]->Destroy();
		//< 20110201 部屋に切断したユーザの何かが残っている
		// 個人チャットコンボボックス更新
		UpdateWisperList(nCharaIndex);
		// ユーザーリスト更新
		UpdateUserList(nCharaIndex);
		break;
	}
}

BOOL CGame::LoadStageCharacter()
{
	m_vecCharacters.clear();
	WCHAR loadlog[64];
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'\\');
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_SessionArray[i].obj_state != OBJ_STATE_LOADING)
			continue;

		// スクリプト関連付け再設定
		m_pRoomCharacters[i]->Hide();

//		if (!m_pStageCharacters[i]->Create(&m_mapCharaScrInfo, m_nDefaultGUIResourceIndex, p_pUI, &m_SessionArray[i]))
//			return FALSE;
		if (m_SessionArray[i].team_no != GALLERY_TEAM_NO)
		{
			m_SessionArray[i].scrinfo = common::scr::FindCharaScrInfoFromCharaType(m_SessionArray[i].chara_type, &m_mapCharaScrInfo);
			if (m_pStageCharacters[i]->Create(&m_mapCharaScrInfo, m_nDefaultGUIResourceIndex, p_pUI, &m_SessionArray[i], m_pSelectedStageScrInfo->stage.size.cx, m_pSelectedStageScrInfo->stage.size.cy))
				SafePrintf(loadlog, 64, L"stg_chr_load(obj/id):%d/%d", m_SessionArray[i].obj_no, m_SessionArray[i].scrinfo->ID);
			else
				SafePrintf(loadlog, 64, L"stg_chr_fail(obj/id):%d/%d", m_SessionArray[i].obj_no, m_SessionArray[i].scrinfo->ID);
			AddMessageLog(loadlog);
			m_vecCharacters.push_back(&m_SessionArray[i]);
		}
		else
		{
			if (m_SessionArray[i].chara_type == ROOM_CHARA_RANDOM_ID)
				m_SessionArray[i].scrinfo = common::scr::FindCharaScrInfoFromCharaType(m_mapCharaScrInfo[genrand_int32()%m_mapCharaScrInfo.size()].ID, &m_mapCharaScrInfo);
			else
				m_SessionArray[i].scrinfo = common::scr::FindCharaScrInfoFromCharaType(m_SessionArray[i].chara_type, &m_mapCharaScrInfo);

			AddMessageLog(loadlog);
			m_SessionArray[i].entity = 0;
		}
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
	return TRUE;
}

void CGame::ClearStageObjects()
{
	OutputDebugStr(L"ClearStageObjects");
	for ( std::map < int, type_obj* >::iterator it = m_mapObjects.begin();
		it != m_mapObjects.end();
		it++)
	{
		switch ((*it).second->obj_type)
		{
		default:
		case OBJ_TYPE_CHARA:
		{
			type_session* sess = (type_session*)((*it).second);
			SafeDelete(sess);
			break;
		}
		case OBJ_TYPE_BLT_LIQUID:
		case OBJ_TYPE_BLT_SOLID:
		case OBJ_TYPE_BLT_GAS:
		{
			type_blt* blt = (type_blt*)((*it).second);
			DEBUG_DELETE(blt, L"ClearStageObjects");
			break;
		}
		case OBJ_TYPE_ITEM_GAS:
		case OBJ_TYPE_ITEM_SOLID:
		case OBJ_TYPE_ITEM_LIQUID:
		{
			type_obj* obj = (type_obj*)((*it).second);
			DEBUG_DELETE(obj, L"ClearStageObjects");
			break;
		}
		}
	}

	m_pFocusObject = NULL;
	m_mapObjects.clear();
	UpdateObjectNo();
}

void CGame::OnLostCharacterScriptTexture()
{
	for (std::map<int, TCHARA_SCR_INFO>::iterator it = m_mapCharaScrInfo.begin();
		it != m_mapCharaScrInfo.end();
		it++)
	{
		(*it).second.pTexture = NULL;
	}
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_SessionArray[i].entity)
			((TCHARA_SCR_INFO*)m_SessionArray[i].scrinfo)->pTexture = NULL;
		if (m_pRoomCharacters[i]->IsCreated())
			m_pRoomCharacters[i]->OnLost();
		if (m_pStageCharacters[i]->IsCreated())
			m_pStageCharacters[i]->OnLost();
	}
}

void CGame::OnResetCharacterScriptTexture()
{
	for (std::map<int, TCHARA_SCR_INFO>::iterator it = m_mapCharaScrInfo.begin();
		it != m_mapCharaScrInfo.end();
		it++)
	{
/*
		// テクスチャをリソースに登録
		int nScrTextureNodeIndex = AddResourceTexture((*it).second.tex_path);
		if (nScrTextureNodeIndex == -1)
		{
			MessageBox(NULL, L"スクリプトに定義された画像のリセットに失敗しました", L"script or image error", MB_OK);
			return;
		}
		(*it).second.res_index = nScrTextureNodeIndex;
*/
		(*it).second.pTexture = g_DialogResourceManager.GetTextureNode((*it).second.res_index)->pTexture;
	}
	for (int i=0;i<GetMaxLoginNum();i++)
	{
		if (m_SessionArray[i].entity)
		{
			TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(m_SessionArray[i].chara_type, &m_mapCharaScrInfo);
			m_SessionArray[i].scrinfo = pCharaScrInfo;
		}

		if (m_pRoomCharacters[i]->IsCreated())
			m_pRoomCharacters[i]->OnReset();
		if (m_pStageCharacters[i]->IsCreated())
			m_pStageCharacters[i]->OnReset();
	}
}

void CGame::OnResetCharacterScriptSound()
{
	std::map<int, TBASE_SCR_INFO*> mapLoadedCharacters;
	for (std::vector< type_session* >::iterator it = m_vecCharacters.begin();
		it != m_vecCharacters.end();
		it++)
	{
		// スクリプトの音声読込み済み確認
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
				MessageBox(g_hWnd, wsSEPath, L"Script Sound Load Error", MB_OK);
			else
				m_mapScrSoundIDHash.insert(std::map<std::string, int>::value_type(csSEPath, nResID));
			SafeDeleteArray(wsSEPath);
		}
		mapLoadedCharacters.insert(std::map<int, TBASE_SCR_INFO* >::value_type( (*it)->scrinfo->scr_index, (*it)->scrinfo ));
	}
}

// ステージロードスレッド
DWORD __stdcall CGame::Thread_Loading(LPVOID param)
{
	TLoadingParam* pParam = (TLoadingParam*)param;

	g_pCriticalSection->EnterCriticalSection_StageTexture(L'!');

	BYTE *pStageBuf = NULL;
	UINT	nStageBufSize;
	BYTE *pBGBuf = NULL;
	UINT	nBGBufSize;

	g_pFiler->GetFileMemory(pParam->pStageScrInfo->stage.path, &pStageBuf, &nStageBufSize);

	if (!pStageBuf)
	{
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		MessageBox(g_hWnd, L"ステージ用ファイルロード失敗", L"error", MB_OK);
		g_bCloseSocket = TRUE;
		return 0;
	}
	if (g_bKillLoadingThread)
	{
		SafeDeleteArray(pStageBuf);
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		AddMessageLog(L"!ステージ用画像ロード前にDeviceLost");
		return 0;
	}
	if (!pParam->pMainStage->Init(pStageBuf, nStageBufSize, &pParam->pStageScrInfo->stage.size))
	{
		SafeDeleteArray(pStageBuf);
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		MessageBox(g_hWnd, L"ステージ用画像ロード失敗", L"error", MB_OK);
		g_bCloseSocket = TRUE;
		return 0;
	}
	SafeDeleteArray(pStageBuf);

	if (g_bKillLoadingThread)
	{
		SafeDeleteArray(pBGBuf);
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		AddMessageLog(L"!ステージ用ファイルロード前にDeviceLost");
		return 0;
	}
	g_pFiler->GetFileMemory(pParam->pStageScrInfo->bg.path, &pBGBuf, &nBGBufSize);

	if (!pBGBuf)
	{
		SafeDeleteArray(pStageBuf);
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		MessageBox(g_hWnd, L"ステージ用ファイルロード失敗", L"error", MB_OK);
		DestroyWindow(g_hWnd);
		return 0;
	}

	if (g_bKillLoadingThread)
	{
		SafeDeleteArray(pBGBuf);
		AddMessageLog(L"!背景ファイルロード後にDeviceLost");
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		return 0;
	}

	AddMessageLog(L">ステージロード");
	if (g_nStageLoadType == 1)
	{
		if (FAILED(D3DXCreateTextureFromFileEx(g_pDevice, pParam->pStageScrInfo->stage.path, 0,0, 1, 0, D3DDEFAULT_FORMAT, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, pParam->ppStageTexture)))
		{
			MessageBox(NULL, L"ステージテクスチャ作成失敗",L"ステージテクスチャ作成", MB_OK);
			Sleep(1);
			if (g_bKillLoadingThread)
			{
				AddMessageLog(L"!ステージロード中にDeviceLost");
				g_pCriticalSection->LeaveCriticalSection_StageTexture();
				return 0;
			}
			else
				AddMessageLog(L"!ステージテクスチャ作成失敗");
		}
	}
	else
	{
		if (!(*pParam->ppStageTexture))
		{
	//		while (!TextureLoader::LoadTextureFromFileInMemory(pParam->ppStageTexture, g_pDevice, pBuf, nBufSize, NULL, NULL, 0, D3DUSAGE_DYNAMIC, D3DPOOL_DEFAULT))
			while (!pParam->pMainStage->CreateTexture(g_pDevice, pParam->ppStageTexture))
			{
				AddMessageLog(L"(ステージテクスチャ作成失敗)");
				Sleep(1);
				if (g_bKillLoadingThread)
				{
					AddMessageLog(L"!ステージロード中にDeviceLost");
					g_pCriticalSection->LeaveCriticalSection_StageTexture();
					return 0;
				}
				else
					AddMessageLog(L"!ステージテクスチャ作成失敗");
			}
		}
	}
	AddMessageLog(L"<ステージロード");

	if (g_bKillLoadingThread)
	{
		SafeDeleteArray(pStageBuf);
		SafeDeleteArray(pBGBuf);
		AddMessageLog(L"!背景ロード前にDeviceLost");
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		return 0;
	}
	// ステージ背景ロード
	// ステージロード
	if (!(*pParam->ppStageBGTexture))
	{
		AddMessageLog(L">背景ロード");
		BOOL ret = FALSE;
		while (!TextureLoader::LoadTextureFromFileInMemory(pParam->ppStageBGTexture, g_pDevice, pBGBuf, nBGBufSize, NULL, NULL, 0, 1))
//		while (!PngLoader::CreateTextureFromFileInMemory(g_pDevice, pBGBuf, nBGBufSize, NULL, NULL, pParam->ppStageBGTexture, D3DPOOL_DEFAULT))
		{
			AddMessageLog(L">背景ロード失敗");
			Sleep(1);
			if (g_bKillLoadingThread)
				break;
		}
		SafeDeleteArray(pBGBuf);
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
	AddMessageLog(L"<背景ロード");

	g_pCriticalSection->EnterCriticalSection_Session(L'7');
	g_pCriticalSection->EnterCriticalSection_Sound(L'4');
	
	AddMessageLog(L">キャラスクリプト音声ロード");

	if (pParam->pVecCharacters->empty())
		AddMessageLog(L"!Empty:LoadChara");

	std::map<int, TBASE_SCR_INFO*> mapLoadedCharacters;
	for (std::vector< type_session* >::iterator it = pParam->pVecCharacters->begin();
		it != pParam->pVecCharacters->end();
		it++)
	{
		if (g_bKillLoadingThread)
		{
			AddMessageLog(L"!キャラスクリプト音声ロード中1:DeviceLost");
			g_pCriticalSection->LeaveCriticalSection_Sound();
			g_pCriticalSection->LeaveCriticalSection_Session();
			return 0;
		}
		// スクリプトの音声読込み済み確認
		if (mapLoadedCharacters.find((*it)->scrinfo->ID) != mapLoadedCharacters.end())
			continue;
		
		int nScrIndex = (*it)->scrinfo->scr_index;
		LuaFuncParam luaParams,luaResults;
		luaParams.Number(nScrIndex);
		common::scr::CallLuaFunc(g_pLuah, "getChara_SEFilesCount", &luaResults, 1, &luaParams, g_pCriticalSection);
		int nSECount = (int)luaResults.GetNumber(0);

		WCHAR log[48];
		SafePrintf(log, 48, L"scr(%d,%d)_CharaSECount:%d",(*it)->scrinfo->ID, nScrIndex, nSECount);
		AddMessageLog(log);

		WCHAR* wsSEPath;
		int wsSEPathLen = 0;
		for (int i=0;i<nSECount;i++)
		{
			if (g_bKillLoadingThread)
			{
				AddMessageLog(L"!キャラスクリプト音声ロード中2:DeviceLost");
				g_pCriticalSection->LeaveCriticalSection_Sound();
				g_pCriticalSection->LeaveCriticalSection_Session();
				return 0;
			}
			luaParams.Clear();
			luaResults.Clear();
			luaParams.Number(nScrIndex).Number(i);
			common::scr::CallLuaFunc(g_pLuah, "getChara_SEFile", &luaResults, 1, &luaParams, g_pCriticalSection);

			char csSEPath[MAX_PATH*2+1];
			SafePrintfA(csSEPath, MAX_PATH*2, luaResults.GetString(0));
			if ( !luaResults.GetWString(0, &wsSEPath, &wsSEPathLen) )
			{
				AddMessageLog(wsSEPath);
				MessageBox(g_hWnd, wsSEPath, L"lua", MB_OK);
				SafeDeleteArray(wsSEPath);
				continue;
			}
			int nResID = pParam->pSoundLibs->AddFromFile(wsSEPath);
			if (nResID == -1)
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"Script Sound Load Error:%s", wsSEPath);
				AddMessageLog(log);
				MessageBox(g_hWnd, wsSEPath, L"Script Sound Load Error", MB_OK);
			}
			else
			{
				std::string stemp(csSEPath);
				pParam->pSoundIDHash->insert(std::map<std::string, int>::value_type(csSEPath, nResID));
			}
			SafeDeleteArray(wsSEPath);
		}
		mapLoadedCharacters.insert(std::map<int, TBASE_SCR_INFO* >::value_type( (*it)->scrinfo->ID, (*it)->scrinfo ));
	}

	AddMessageLog(L"<キャラスクリプト音声ロード>ステージ");
	{
		int nScrIndex = pParam->pStageScrInfo->scr_index;
		LuaFuncParam luaParams,luaResults;
		luaParams.Number(nScrIndex);
		common::scr::CallLuaFunc(g_pLuah, "getStage_SEFilesCount", &luaResults, 1, &luaParams, g_pCriticalSection);
		int nSECount = (int)luaResults.GetNumber(0);

		WCHAR log[48];
		SafePrintf(log, 48, L"scr_index(%d)_StageSECount:%d", nScrIndex, nSECount);
		AddMessageLog(log);

		WCHAR* wsSEPath;
		int wsSEPathLen = 0;
		for (int i=0;i<nSECount;i++)
		{
			if (g_bKillLoadingThread)
			{
				AddMessageLog(L"!ステージスクリプト音声ロード中2:DeviceLost");
				g_pCriticalSection->LeaveCriticalSection_Sound();
				g_pCriticalSection->LeaveCriticalSection_Session();
				return 0;
			}
			luaParams.Clear();
			luaResults.Clear();
			luaParams.Number(nScrIndex).Number(i);
			common::scr::CallLuaFunc(g_pLuah, "getStage_SEFile", &luaResults, 1, &luaParams, g_pCriticalSection);

			char csSEPath[MAX_PATH*2+1];
			SafePrintfA(csSEPath, MAX_PATH*2, luaResults.GetString(0));
			if ( !luaResults.GetWString(0, &wsSEPath, &wsSEPathLen) )
			{
				AddMessageLog(wsSEPath);
				MessageBox(g_hWnd, wsSEPath, L"lua", MB_OK);
				SafeDeleteArray(wsSEPath);
				continue;
			}
			int nResID = pParam->pSoundLibs->AddFromFile(wsSEPath);
			if (nResID == -1)
			{
				WCHAR log[64];
				SafePrintf(log, 64, L"Script Sound Load Error:%s", wsSEPath);
				AddMessageLog(log);
				MessageBox(g_hWnd, wsSEPath, L"Script Sound Load Error", MB_OK);
			}
			else
			{
				std::string stemp(csSEPath);
				pParam->pSoundIDHash->insert(std::map<std::string, int>::value_type(csSEPath, nResID));
			}
			SafeDeleteArray(wsSEPath);
		}
	}
	AddMessageLog(L"<ステージスクリプト音声ロード>BGMロード");
	// BGM
	*pParam->pBgmSoundID = pParam->pSoundLibs->AddFromFile(pParam->pStageScrInfo->bgm);
	if (*pParam->pBgmSoundID == -1)
	{
		WCHAR log[64];
		SafePrintf(log, 64, L"ステージBGMロードエラー:%s", pParam->pStageScrInfo->bgm);
		AddMessageLog(log);
	}

	g_pCriticalSection->LeaveCriticalSection_Sound();
	g_pCriticalSection->LeaveCriticalSection_Session();

	AddMessageLog(L"<ロード完了");
	return 0;
}

void CGame::UpdateRankView()
{
	CDXUTScrollBar* pScrollBar = p_pUI->GetScrollBar(IDC_RESULT_SB_RANK);
//	pScrollBar->SetTrackRange(0, (int)m_vecCharacters.size()-1);
	int nNo = pScrollBar->GetTrackPos();
	CDXUTControl* pNoControl = NULL;
	CDXUTControl* pIconControl = NULL;
	CDXUTControl* pNameControl = NULL;
	CDXUTControl* pTeamControl = NULL;
	bool bVisible = false;

	for (int i=0;i<m_nRankItemCount;i++)
	{
//		if (i+nNo > m_nRankItemCount-1) break;
		bVisible = (i>=nNo) && (i-nNo<=RESULT_RANK_VIEW_RANGE-1);
		pNoControl = p_pUI->GetControl(IDC_RESULT_SPRITE_NO_BASE+i);
		pIconControl = p_pUI->GetControl(IDC_RESULT_SPRITE_ICON_BASE+i);
		pNameControl = p_pUI->GetControl(IDC_RESULT_STATIC_NAME_BASE+i);
		if (m_nTeamCount > 1)
			pTeamControl = p_pUI->GetControl(IDC_RESULT_STATIC_TEAM_BASE+i);
		
		if (pNoControl->GetVisible() != bVisible)
		{
			pNoControl->SetVisible(bVisible);
			pNoControl->Refresh();
			pIconControl->SetVisible(bVisible);
			pIconControl->Refresh();
			pNameControl->SetVisible(bVisible);
			pNameControl->Refresh();
			// チーム番号
			if (pTeamControl)
			{
				pNameControl->SetVisible(bVisible);
				pNameControl->Refresh();
			}
		}

		int nOffsetX = 0;
		if (pTeamControl)
			nOffsetX = RESULT_STATIC_TEAM_CNT_W;
		if (bVisible)
		{
			int nY = RESULT_SPRITE_RANK_NO_CNT_Y_BASE+((RESULT_SPRITE_RANK_NO_CNT_Y_OFFSET)*i);
			pNoControl->SetLocation(
				RESULT_SPRITE_RANK_NO_CNT_X_BASE,
				RESULT_SPRITE_RANK_NO_CNT_Y_BASE+((RESULT_SPRITE_RANK_NO_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*(i-nNo)) );
			pIconControl->SetLocation(
				RESULT_SPRITE_ICON_CNT_X_BASE,
				RESULT_SPRITE_ICON_CNT_Y_BASE+((RESULT_SPRITE_ICON_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*(i-nNo)) );
			if (pTeamControl)
				pTeamControl->SetLocation(
					RESULT_STATIC_TEAM_CNT_X_BASE,
					RESULT_STATIC_TEAM_CNT_Y_BASE+((RESULT_STATIC_TEAM_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*(i-nNo)) );
			pNameControl->SetLocation(
				RESULT_STATIC_NAME_CNT_X_BASE+nOffsetX,
				RESULT_STATIC_NAME_CNT_Y_BASE+((RESULT_STATIC_NAME_CNT_Y_OFFSET+RESULT_SPRITE_LINE_CNT_H)*(i-nNo)) );
			pNameControl->SetSize(
				RESULT_STATIC_NAME_CNT_W-nOffsetX,
				RESULT_STATIC_NAME_CNT_H);
		}
	}
}

void CGame::LoadSound(WCHAR* pPath, CSoundLibraries* pSoundLibs, std::map<std::wstring, int>* pMapSoundIDHash)
{
	int nResID = 0;
	if ((nResID = pSoundLibs->AddFromFile(pPath)) == -1)
		MessageBox(g_hWnd, pPath, L"Sound Load Error", MB_OK);
	else
		pMapSoundIDHash->insert(std::map<std::wstring, int>::value_type(pPath, nResID));
}

void CGame::PlaySysSoundSE(std::wstring wsResouce, int fade)
{
	if (wsResouce.empty())	return;
	if (!m_bytSEVolume)	return;

	g_pCriticalSection->EnterCriticalSection_Sound(L'5');
	std::map<std::wstring, int>::iterator itfind = m_mapSysSoundIDHash.find(wsResouce);
	if (itfind == m_mapSysSoundIDHash.end())
	{
		g_pCriticalSection->LeaveCriticalSection_Sound();
		WCHAR log[64];
		SafePrintf(log, 64, L"NotFound,SysSoundIDHash:%s", wsResouce.c_str());
		AddMessageLog(log);
		return;
	}
	CSoundBuffer* pSoundBuffer = m_pSysSoundLibs->GetDuplicatedFromID((*itfind).second);
	if (!pSoundBuffer)
	{
		g_pCriticalSection->LeaveCriticalSection_Sound();
		WCHAR log[64];
		SafePrintf(log, 64, L"NoGet,SysSoundBuffer:%s", wsResouce.c_str());
		AddMessageLog(log);
	}
	else
		m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytSEVolume, 0, fade);
	g_pCriticalSection->LeaveCriticalSection_Sound();
}

void CGame::SaveConfig()
{
	// BGM VOLUME
	m_pIniConfig->WriteInt(L"CONFIG", L"BGM", m_bytBGMVolume = p_pConfig->GetSlider(IDC_CONFIG_SLIDER_BGM)->GetValue());
	// SE VOLUME
	m_pIniConfig->WriteInt(L"CONFIG", L"SE", m_bytSEVolume = p_pConfig->GetSlider(IDC_CONFIG_SLIDER_SE)->GetValue());
	// EFFECT ON/OFF
	m_pIniConfig->WriteBool(L"CONFIG", L"EFFECT", m_bEffectEnable = p_pConfig->GetCheckBox(IDC_CONFIG_CHK_EFFECT)->GetChecked());
	// BLT_FOCUS ON/OFF
	m_pIniConfig->WriteBool(L"CONFIG", L"BLT_FOCUS", m_bBulletFocus = p_pConfig->GetCheckBox(IDC_CONFIG_CHK_BLT_FOCUS)->GetChecked());
	// ACT_FOCUS ON/OFF
	m_pIniConfig->WriteBool(L"CONFIG", L"ACT_FOCUS", m_bActChrFocus = p_pConfig->GetCheckBox(IDC_CONFIG_CHK_ACT_FOCUS)->GetChecked());
}

BOOL CGame::ReqLoadComplete()
{
	if (m_bReqLoadComplete)
		return FALSE;
	AddMessageLog(L"ReqLoadComplete");
	m_bReqLoadComplete = TRUE;
	return TRUE;
}

void CGame::ShowItemDetail(int nControlID)
{
	CDXUTButton* pBtn = p_pUI->GetButton(nControlID);
	if (!pBtn) return;
	CDXUTButton* pBtnDetail = p_pUI->GetButton(IDC_ROOM_BTN_ITEM_DETAIL);
	DWORD dwItemFlg = (DWORD)pBtn->GetUserData();	
	if (!dwItemFlg)	return;
	int i=0;
	for (;i<GAME_ITEM_COUNT;i++)
	{
		if (dwItemFlg == c_tItemResouceInfoTable[i].flg)
			break;
	}

	if (i >= GAME_ITEM_COUNT) return;
	const WCHAR* wsText = c_tItemResouceInfoTable[i].text;
	pBtnDetail->SetText(wsText);
	pBtnDetail->SetLocation(min(WIN_WIDTH-m_rcItemDetailControlSize[i].right,m_nMousePosX-4),m_nMousePosY-m_rcItemDetailControlSize[i].bottom+4);
	pBtnDetail->SetSize(m_rcItemDetailControlSize[i].right, m_rcItemDetailControlSize[i].bottom);
	if (!pBtnDetail->GetVisible())
		pBtnDetail->SetVisible(true);

}

BOOL CGame::ReqPacketHash(BYTE* data)
{
	BYTE pkt[MAX_PACKET_SIZE];
//	INT pktsize = PacketMaker::MakePacketData_AuthRetHash(hash_group, hash_id, hashcode.c_str(), hashcode.length(), pkt);
	int index = 3;
	WORD wCount = 0;
	memcpy(&wCount, &data[index], sizeof(WORD));
	index += sizeof(WORD);
	for (int i=0;i<wCount;++i)
	{
		WORD wID = 0;
		memcpy(&wID, &data[index], sizeof(WORD));
		index += sizeof(WORD);
		std::map <int, TCHARA_SCR_INFO>::iterator itfind = m_mapCharaScrInfo.find(wID);
		if (itfind == m_mapCharaScrInfo.end())
		{
			WCHAR alog[128];
			SafePrintf(alog, 128, L"キャラデータ不足しています。\nキャラスクリプトID[%d]が見つかりませんでした。", wID);
			g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
			AddMessageLog(alog);
			MessageBox(g_hWnd, alog, L"認証結果", MB_OK);
			if (g_bCloseSocket) return FALSE;
			SafePrintf(alog, 128, L"サーバが使用しているキャラスクリプトID[%d]を要求しますか？\n(「はい」を押すと受信完了まで応答が無くなります)", wID);
			if (MessageBox(g_hWnd, alog, L"データ不足", MB_YESNO) == IDYES)
			{
				if (g_bCloseSocket) return FALSE;
				BYTE pkt[MAX_PACKET_SIZE];
				INT pktsize = PacketMaker::MakePacketData_ReqFileHash(FALSE, wID, 0, pkt);
				return AddPacket(pkt, pktsize);
			}
			else
				g_bCloseSocket = TRUE;
			return FALSE;
		}
		else
			(*itfind).second.flg = TRUE;
	}

	memcpy(&wCount, &data[index], sizeof(WORD));
	index += sizeof(WORD);
	for (int i=0;i<wCount;++i)
	{
		WORD wID = 0;
		memcpy(&wID, &data[index], sizeof(WORD));
		index += sizeof(WORD);
		std::map <int, TSTAGE_SCR_INFO>::iterator itfind = m_mapStageScrInfo.find(wID);
		if (itfind == m_mapStageScrInfo.end())
		{
			WCHAR alog[128];
			SafePrintf(alog, 128, L"ステージデータ不足しています。\nステージスクリプトID[%d]が見つかりませんでした。", wID);
			g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
			AddMessageLog(alog);
			MessageBox(g_hWnd, alog, L"認証結果", MB_OK);
			if (g_bCloseSocket) return FALSE;
			SafePrintf(alog, 128, L"サーバが使用しているステージスクリプトID[%d]を要求しますか？\n(「はい」を押すと受信完了まで応答が無くなります)", wID);
			if (MessageBox(g_hWnd, alog, L"データ不足", MB_YESNO) == IDYES)
			{
				if (g_bCloseSocket) return FALSE;
				BYTE pkt[MAX_PACKET_SIZE];
				INT pktsize = PacketMaker::MakePacketData_ReqFileHash(FALSE, wID, 0, pkt);
				return AddPacket(pkt, pktsize);
			}
			else
				g_bCloseSocket = TRUE;
			return FALSE;
		}
		else
			(*itfind).second.flg = TRUE;
	}
	INT pktsize = PacketMaker::MakePacketData_FileHash(FALSE, &m_mapCharaScrInfo, &m_mapStageScrInfo, pkt);
	return AddPacket(pkt, pktsize);
	int hash_group  = data[3];
	WORD hash_id=0;
	memcpy(&hash_id, &data[4], sizeof(WORD));
	std::string hashcode;

	if (hash_group == 0)
	{
		std::map <int, TSTAGE_SCR_INFO>::iterator itfind = m_mapStageScrInfo.find(hash_id);
		if (itfind == m_mapStageScrInfo.end())
		{
			WCHAR alog[128];
			SafePrintf(alog, 128, L"スクリプトデータ不足しています。\nステージスクリプトID[%d]が見つかりませんでした。", hash_id);
			g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
			AddMessageLog(alog);
			MessageBox(g_hWnd, alog, L"認証結果", MB_OK);
#if TRIAL==0
			if (g_bCloseSocket) return FALSE;
			SafePrintf(alog, 128, L"サーバが使用しているステージスクリプトID[%d]を要求しますか？\n(「はい」を押すと受信完了まで応答が無くなります)", hash_id);
			if (/*hash_id>=100 && */MessageBox(g_hWnd, alog, L"データ不足", MB_YESNO) == IDYES)
			{
				if (g_bCloseSocket) return FALSE;
				BYTE pkt[MAX_PACKET_SIZE];
				INT pktsize = PacketMaker::MakePacketData_ReqFileHash(FALSE, hash_id, 0, pkt);
				return AddPacket(pkt, pktsize);
			}
			else
#endif
				g_bCloseSocket = TRUE;
			return FALSE;
		}
		hashcode = (*itfind).second.md5;
		(*itfind).second.flg = TRUE;
	}
	else
	{
		std::map <int, TCHARA_SCR_INFO>::iterator itfind = m_mapCharaScrInfo.find(hash_id);
		if (itfind == m_mapCharaScrInfo.end())
		{
			WCHAR alog[128];
			SafePrintf(alog, 128, L"スクリプトデータ不足しています。\nキャラスクリプトID[%d]が見つかりませんでした。", hash_id);
			g_pGame->PlaySysSoundSE(SE_sai_SrvInfo);
			AddMessageLog(alog);
			MessageBox(g_hWnd, alog, L"認証結果", MB_OK);
//#if (TRIAL==0)
			SafePrintf(alog, 128, L"サーバが使用しているキャラスクリプトID[%d]を要求しますか？\n(「はい」を押すと受信完了まで応答が無くなります)", hash_id);
			if (/*hash_id>=100 && */MessageBox(g_hWnd, alog, L"データ不足", MB_YESNO) == IDYES)
			{
				BYTE pkt[MAX_PACKET_SIZE];
				INT pktsize = PacketMaker::MakePacketData_ReqFileHash(TRUE, hash_id, 0, pkt);
				return AddPacket(pkt, pktsize);
			}
			else
//#endif
				g_bCloseSocket = TRUE;
			return FALSE;
		}
		hashcode = (*itfind).second.md5;
		(*itfind).second.flg = TRUE;
	}

//	BYTE pkt[MAX_PACKET_SIZE];
//	INT pktsize = PacketMaker::MakePacketData_AuthRetHash(hash_group, hash_id, hashcode.c_str(), hashcode.length(), pkt);
	return AddPacket(pkt, pktsize);
}

BOOL CGame::InitControl()
{
//#if SCR_HASH_CHECK
	// 使用しないスクリプトの登録削除
	for (std::map <int, TSTAGE_SCR_INFO>::iterator it = m_mapStageScrInfo.begin();
		it != m_mapStageScrInfo.end();
		)
	{
		if (!(*it).second.flg)
			it = m_mapStageScrInfo.erase(it);
		else
			it++;
	}
	if (m_mapStageScrInfo.empty())
		return FALSE;

	for (std::map <int, TCHARA_SCR_INFO>::iterator it = m_mapCharaScrInfo.begin();
		it != m_mapCharaScrInfo.end();
		)
	{
		if (!(*it).second.flg)
			it = m_mapCharaScrInfo.erase(it);
		else
			it++;
	}

	if (m_mapCharaScrInfo.empty())
		return FALSE;

//#endif
	for (std::map<int, TSTAGE_SCR_INFO>::iterator it = m_mapStageScrInfo.begin();
		it != m_mapStageScrInfo.end();
		it++)
	{
		// テクスチャをリソースに登録
		int nScrTextureNodeIndex = AddResourceTexture((*it).second.thumnail.path);
		if (nScrTextureNodeIndex == -1)
		{
			MessageBox(g_hWnd, L"ステージスクリプトに定義された画像のロードに失敗しました", L"script or image error", MB_OK);
			return FALSE;
		}
		(*it).second.res_index = nScrTextureNodeIndex;
	}

	for (std::map<int, TCHARA_SCR_INFO>::iterator it = m_mapCharaScrInfo.begin();
		it != m_mapCharaScrInfo.end();
		it++)
	{
		// テクスチャをリソースに登録
		int nScrTextureNodeIndex = AddResourceTexture((*it).second.tex_path);
		if (nScrTextureNodeIndex == -1)
		{
			MessageBox(g_hWnd, L"キャラスクリプトに定義された画像のロードに失敗しました", L"script or image error", MB_OK);
			return FALSE;
		}
		(*it).second.res_index = nScrTextureNodeIndex;
		(*it).second.pTexture = g_DialogResourceManager.GetTextureNode(nScrTextureNodeIndex)->pTexture;
	}

	if (m_mapCharaScrInfo.empty())
		return FALSE;

	// ユーザーリスト //
	SetUserList();
	// キャラ用アイコンを設定する
	SetCharacterIconButtonList();
	// キャラ初期設定
	SetChacterSelectInfo( (* m_mapCharaScrInfo.begin()).first);	
	return TRUE;
}

BOOL CGame::PlayBGM(const WCHAR* wsResouceName, int nFade)
{
#if BGM_ON
	// 再生中のBGMがあった場合停止しておく
	if (m_nBGMPlayingID != -1)	StopBGM(FALSE);

	m_nBGMSoundID = m_pSysSoundLibs->AddFromFile(wsResouceName);
	return PlayBGM(m_nBGMSoundID, nFade);
#else
	return TRUE;
#endif
}

BOOL CGame::PlayBGM(int nSoundID, int nFade)
{
#if BGM_ON
	CSoundLibraries* pSoundLibs = m_pSysSoundLibs;
	if (((DWORD)m_eGameState&GAME_STATE_PHASE_MASK) == GAME_STATE_MAIN)
		pSoundLibs = m_pScrSoundLibs;

	if (m_nBGMSoundID == -1)
	{
		MessageBox(g_hWnd, L"BGMSoundID(-1)", L"error", MB_OK);
		return FALSE;
	}
	if (m_bytBGMVolume > 0)
	{
		CSoundBuffer* pSoundBuffer = pSoundLibs->GetDuplicatedFromID(m_nBGMSoundID);
		if (pSoundBuffer)
			m_nBGMPlayingID = m_pSoundPlayer->PlaySoundBuffer(pSoundBuffer, m_bytBGMVolume, SOUNDLIB_ENDLESSLOOP, nFade);
		return (m_nBGMPlayingID != -1);
	}
	return TRUE;
#else
	return TRUE;
#endif
}

void CGame::StopBGM(BOOL bRemoveFromLib, int nFade)
{
#if BGM_ON
	CSoundLibraries* pSoundLibs = m_pSysSoundLibs;
	if (((DWORD)m_eGameState&GAME_STATE_PHASE_MASK) == GAME_STATE_MAIN)
		pSoundLibs = m_pScrSoundLibs;
	// プレイヤーから再生停止
	CSoundBuffer* pSoundBuffer = m_pSoundPlayer->GetSoundBufferFromID(m_nBGMPlayingID);
	if (pSoundBuffer)
		m_pSoundPlayer->StopSoundBuffer(pSoundBuffer);
	m_nBGMPlayingID = -1;
	// 音声ライブラリから削除
	if (bRemoveFromLib)
	{
		if (!m_pScrSoundLibs->Remove(m_nBGMSoundID)){
			AddMessageLog(L"m_nBGMSoundID remove error");
		}
		m_nBGMSoundID = -1;
	}
#endif
}

void CGame::ClearScrSound()
{
	g_pCriticalSection->EnterCriticalSection_Sound(L'6');
	if (!m_pScrSoundLibs->Clear())
	{
		AddMessageLog(L"!ClearScrSound() > m_pScrSoundLibs->Clear()");
	}
	m_mapScrSoundIDHash.clear();
	g_pCriticalSection->LeaveCriticalSection_Sound();
}

BOOL CGame::LoadLoadingImage()
{
	//ファイルの検索
	_wfinddata_t	fileinfo;
	WCHAR filetype[] = LOAD_IMG_FIND_WC;
	long handle= _wfindfirst(filetype,&fileinfo);
	if (handle != -1)
	{
		std::vector<std::wstring> vecPath;
		std::wstring wsImagePath;

		wsImagePath = LOAD_IMG_FIND_DIR;
		wsImagePath += fileinfo.name;

		vecPath.push_back(wsImagePath);	
		while(!_wfindnext(handle, &fileinfo))
		{
			wsImagePath = LOAD_IMG_FIND_DIR;
			wsImagePath += fileinfo.name;
			vecPath.push_back(wsImagePath);
		}
		_findclose(handle);

		// ランダムのインデックス取得
		if (vecPath.size() > 0)
		{
			int nIndex = genrand_int32()%vecPath.size();

			BYTE* pBuf = NULL;
			size_t nBufSize = 0;

			SafePrintf(m_pBGTexturePath, _MAX_PATH, vecPath[nIndex].c_str());
			if (!g_pFiler->GetFileMemory(m_pBGTexturePath, &pBuf, &nBufSize))
			{
				AddMessageLog(L"LoadError BG image");
				MessageBox(g_hWnd, L"ロード画像の読み込みに失敗しました", L"loading error", MB_OK);
			}
			else
			{
				if (!PngLoader::CreateTextureFromFileInMemory(m_pDev, pBuf, nBufSize,  NULL, NULL, &m_pBGTexture, D3DPOOL_DEFAULT))
			//	while (!TextureLoader::LoadTextureFromFileInMemory(&m_pBGTexture, m_pDev, pBuf, nBufSize, NULL, NULL, 0,1))
				{
					m_pBGTexture = NULL;
					AddMessageLog(L"LoadError BG texture");
					MessageBox(g_hWnd, L"ロード画像の作成に失敗しました", L"loading error", MB_OK);
				}
				SafeDeleteArray(pBuf);
			}
		}
	}

	return TRUE;
}

void CGame::SaveIMEText()
{
	CDXUTIMEEditBox* pIMEEditBox = g_SysUI.GetIMEEditBox(IDC_SHARE_EDIT);
	int nLength = pIMEEditBox->GetTextLength();
	// 入力なしなら終了
	if (!nLength)	return;
	WCHAR wszText[MAX_CHAT_MSG+1];
	if (FAILED(pIMEEditBox->GetTextCopy(wszText, MAX_CHAT_MSG)))
		wszText[0] = NULL;
	pIMEEditBox->ClearText();
	pIMEEditBox->SetText(wszText);
}

BOOL CGame::ConnectServer()
{
  	struct sockaddr_in server_addr;
	WSADATA		wsaData;
	unsigned long arg = 1L;
	const char val = 1;
    WCHAR serverip[64];
	char	serverip_c[64];
	size_t	serverip_c_len = 0;
	m_nTcpSock = 0;

	p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(false);
	PlaySysSoundSE(SE_sub_Login);
	if (WSAStartup( MAKEWORD(2,2) , &wsaData ) <0)
	{
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}
	/* ソケット生成 */
	if ((m_nTcpSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		AddMessageLog(L"Client : Can't open stream socket.");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}

	if (setsockopt(m_nTcpSock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) < 0)
	{
		AddMessageLog(L"error in sockopt SO_REUSEADDR");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}

	if (setsockopt(m_nTcpSock, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(int)) < 0)
	{
		AddMessageLog(L"error in sockopt SO_KEEPALIVE");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}

	if (setsockopt(m_nTcpSock, SOL_SOCKET, SO_LINGER, &val, sizeof(int)) < 0)
	{
		AddMessageLog(L"error in sockopt SO_LINGER");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}
/*
	if (setsockopt(m_nTcpSock, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int)) < 0)
	{
		AddMessageLog(L"error in sockopt TCP_NODELAY");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}
*/
	if (g_bCmdSetup)
	{
		_tcscpy_s(serverip, 64, g_wsCmdIP );
	}
	else
	{
		_tcscpy_s(serverip, 64, ((CDXUTEditBox*)p_pUI->GetControl(IDC_TITLE_EDT_IP))->GetText() );
	}
	wcstombs_s(&serverip_c_len, serverip_c, 64, serverip, _TRUNCATE);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if (g_bCmdSetup)
		server_addr.sin_port = htons(g_nCmdPort);
	else
		server_addr.sin_port = htons(m_nPortNo);
	server_addr.sin_addr.s_addr = inet_addr(serverip_c);
	if (connect(m_nTcpSock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
#ifndef _DEBUG
		MessageBox(g_hWnd, L"サーバに接続できませんでした", L"connect", MB_OK);
#endif
		AddMessageLog(L"Client : サーバに接続できませんでした");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE;
	}
	else
		AddMessageLog(L"サーバに接続しました");

	// 0:ブロッキングモード(デフォルト)  1:非ブロッキングモード
	if(0!=ioctlsocket(m_nTcpSock,FIONBIO,&arg))
	{
		AddMessageLog(L"Client : Can't change FIONBIO.");
		p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
		return FALSE; // エラー
	}
	// IP保存
	WCHAR wsIP[64];
	((CDXUTEditBox*)p_pUI->GetControl(IDC_TITLE_EDT_IP))->GetTextCopy(wsIP, 63);
	m_pIniConfig->WriteString(L"LOGIN", L"IP", wsIP);
	m_eGameState = eGameTitleRelease;

	p_pUI->GetControl(IDC_TITLE_BTN_CONNECT)->SetEnabled(true);
	return TRUE;
}

BOOL CGame::LoginServer()
{
	int			nLen = 0;
    WCHAR wsName[MAX_USER_NAME+1];
    WCHAR wsPass[MAX_SRV_PASS+1];
	WORD	packetSize = 0;
	BYTE		packetData[MAX_PACKET_SIZE];

	// 二度押し禁止
	p_pUI->GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(false);
//			ZeroMemory(wsName, MAX_USER_NAME*sizeof(WCHAR));
//			ZeroMemory(wsPass, MAX_SRV_PASS*sizeof(WCHAR));
	BOOL bCntrl = FALSE;
	WCHAR* pW;
	if (g_bCmdSetup)
		((CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT))->SetText(g_wsCmdName);

	nLen = ((CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT))->GetTextLength();
	SafeMemCopy(wsName,
		((CDXUTIMEEditBox*)p_pUI->GetControl(IDC_SHARE_EDIT))->GetText(),
		nLen*sizeof(WCHAR),
		MAX_USER_NAME*sizeof(WCHAR)
	);
	wsName[nLen] = NULL;
	// 空白チェック
	if (wsName[0] == L' ' || wsName[nLen-1] == L' ')
	{
		PlaySysSoundSE(SE_sai_SrvInfo);
		MessageBox(g_hWnd, L"半角スペースが前後にあるユーザー名は使用できません", L"error", MB_OK);
		return FALSE;
	}

	pW = wsName;
	while (*pW)
	{
		if (iswcntrl(*pW))
		{
			bCntrl = TRUE;
			break;
		}
		pW++;
	}
	if (bCntrl)
	{
		PlaySysSoundSE(SE_sai_SrvInfo);
		MessageBox(g_hWnd, L"ユーザー名に不正な文字が含まれています。", L"error", MB_OK);
		return FALSE;
	}

	if (g_bCmdSetup)
		((CDXUTEditBox*)p_pUI->GetControl(IDC_LOGIN_EDT_PASSWORD))->SetText(g_wsCmdPass);

	((CDXUTEditBox*)p_pUI->GetControl(IDC_LOGIN_EDT_PASSWORD))->GetTextCopy(wsPass, MAX_SRV_PASS*sizeof(WCHAR)+1);

	pW = wsPass;
	while (*pW)
	{
		if (iswcntrl(*pW))
		{
			bCntrl = TRUE;
			break;
		}
		if (isspace(*pW))
		{
			bCntrl = TRUE;
			break;
		}
		pW++;
	}
	if (bCntrl)
	{
		PlaySysSoundSE(SE_sai_SrvInfo);
		MessageBox(g_hWnd, L"パスワードに不正な文字が含まれています。", L"error", MB_OK);
		return FALSE;
	}
	if (g_bDebug)
	{
		WCHAR	loginlog[32];
		SafePrintf(loginlog, 32, L"TryLogin:%s", wsName);
		AddMessageLog(loginlog);
	}
	m_pIniConfig->WriteString(L"LOGIN", L"ID", wsName);
	m_bSavePass = p_pUI->GetCheckBox(IDC_LOGIN_CHK_SAVE_PASS)->GetChecked();
	m_pIniConfig->WriteBool(L"LOGIN", L"SAVE_PASS", m_bSavePass);
	if (m_bSavePass)
		m_pIniConfig->WriteString(L"LOGIN", L"PASS", wsPass);
	else
		m_pIniConfig->WriteString(L"LOGIN", L"PASS", L"");
	packetSize = PacketMaker::MakePacketData_Authentication(wsName, wsPass, packetData);
	if (packetSize)
		AddPacket(packetData, packetSize);

	g_SysUI.GetControl(IDC_LOGIN_BTN_LOGIN)->SetEnabled(false);
	m_bLoginStart = TRUE;
	return TRUE;
}

WCHAR* CGame::GetScrName(int id, BOOL bCharaScr)
{
	if (bCharaScr)
	{
		std::map < int, TCHARA_SCR_INFO >::iterator itfind = m_mapCharaScrInfo.find(id);
		if (itfind == m_mapCharaScrInfo.end())
			return NULL;
		return (*itfind).second.name;
	}
	else
	{
		std::map < int, TSTAGE_SCR_INFO >::iterator itfind = m_mapStageScrInfo.find(id);
		if (itfind == m_mapStageScrInfo.end())
			return NULL;
		return (*itfind).second.name;
	}
	return NULL;
}

void CGame::DeleteRecvScrFile()
{
	if (m_wsRecvScrPath)
	{
		WCHAR scrpath[_MAX_PATH*2+1];
		GetModuleDir(scrpath, _MAX_PATH*2+1);
		wcscat_s(scrpath, _MAX_PATH, m_wsRecvScrPath);
		DeleteFile(scrpath);
	}
}

void CGame::SetRecvScrFile(WCHAR* path)
{
	_ASSERT(path);
	wcscpy_s(m_wsRecvScrPath, _MAX_PATH*2 ,path);
}