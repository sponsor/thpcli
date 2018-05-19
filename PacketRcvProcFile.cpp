#include "ext.h"
#include "Shlobj.h"
#include "main.h"

static FILE* s_pfRecvFile = NULL;

BOOL PacketProc_File_PK_USER_FILEINFO_OPEN(BYTE* data);
BOOL PacketProc_File_PK_USER_FILEINFO_SEND(BYTE* data);
BOOL PacketProc_File_PK_USER_FILEINFO_CLOSE(BYTE* data);
BOOL SetOnDirectory(std::wstring* file);
BOOL CheckFileHash(WCHAR* path);

BOOL PacketProc_File(BYTE *data)
{
	BOOL ret = FALSE;
	switch (data[3])
	{
	case PK_USER_FILEINFO_OPEN:
		ret = PacketProc_File_PK_USER_FILEINFO_OPEN(data);
		break;
	case PK_USER_FILEINFO_SEND:
		ret = PacketProc_File_PK_USER_FILEINFO_SEND(data);
		break;
	case PK_USER_FILEINFO_CLOSE:
		ret = PacketProc_File_PK_USER_FILEINFO_CLOSE(data);
		break;
	}
	return ret;
}

BOOL SetOnDirectory(std::wstring* file)
{
	WCHAR pDrive[_MAX_DRIVE+1];
	WCHAR pDir[_MAX_DIR+1];
	WCHAR pSetDirPath[_MAX_PATH*2+1];
	int nLen = (int)file->length();
	for (int i=0;i<nLen;i++)
	{
		if ((*file)[i] == L'/')
			(*file)[i] = L'\\';
	}
	_wsplitpath(file->c_str(), pDrive, pDir, NULL, NULL);
	_wmakepath(pSetDirPath, pDrive, pDir, NULL, NULL);
	return (SHCreateDirectoryEx(g_hWnd, pSetDirPath, NULL) == ERROR_SUCCESS);
}

BOOL CheckFileHash(WCHAR* path, char* md5)
{
	WCHAR exedir[_MAX_PATH*2+1];
	common::GetExeDir(exedir);
	std::wstring wstr = exedir;
	wstr += path;
	BOOL ret = FALSE;

	FILE* fp = _wfopen(wstr.c_str(), L"rb");
	if (!fp) return FALSE;
	int nFileLength = _filelength(_fileno(fp));
	char* buffer = new char[nFileLength];
	if (!buffer)
	{
		fclose(fp);
		return FALSE;
	}
	if (fread(buffer, 1, nFileLength, fp) == nFileLength)
	{
		clx::md5* md = new clx::md5;
		md->update(buffer, nFileLength);
		md->finish();
		OutputDebugStringA(md->to_string().c_str());
		ret = (strncmp(md->to_string().c_str(), md5, MD5_LENGTH) == 0);
		SafeDelete(md);
	}
	fclose(fp);
	SafeDeleteArray(buffer);
	return ret;
}

BOOL PacketProc_File_PK_USER_FILEINFO_OPEN(BYTE* data)
{
	int nIndex = 4;
	BOOL bCharaScr = data[nIndex];
	nIndex++;
	WORD nID = 0;
	memcpy(&nID, &data[nIndex], sizeof(WORD));
	nIndex += sizeof(WORD);
	int nFileNo = data[nIndex];
	nIndex++;
	char md5_srv[MD5_LENGTH+1];
	md5_srv[MD5_LENGTH] = NULL;
	memcpy(md5_srv, &data[nIndex], MD5_LENGTH);
	nIndex+=MD5_LENGTH;
	WORD wRecvPathLen;
	memcpy(&wRecvPathLen, &data[nIndex], sizeof(WORD));
	nIndex += sizeof(WORD);
	WCHAR* recvPath = new WCHAR[wRecvPathLen+1];
	recvPath[wRecvPathLen] = NULL;
	memcpy(recvPath, &data[nIndex], sizeof(WCHAR)*wRecvPathLen);

	WCHAR alog[300];
	SafePrintf(alog, 300, L"FILEINFO_OPEN:%s", recvPath);
	OutputDebugStr(alog);
	AddMessageLog(alog);

	BOOL bComp = CheckFileHash(recvPath, md5_srv);
	if (nFileNo == 0)
		g_pGame->SetRecvScrFile(recvPath);
	// ファイルが見つからなかったか,ファイルハッシュ不一致
	if (!bComp)
	{
		WCHAR exedir[_MAX_PATH*2+1];
		common::GetExeDir(exedir);
		std::wstring wstr = exedir;
		wstr += recvPath;
		SetOnDirectory(&wstr);
		
		if (_wfopen_s(&s_pfRecvFile, wstr.c_str(), L"wb+"))
		{
			SafeDeleteArray(recvPath);
			s_pfRecvFile = NULL;
			AddMessageLog(L"FILEINFO_SEND:_wfopen_s error");
			g_bCloseSocket = TRUE;
			return FALSE;
		}
		SafeDeleteArray(recvPath);
		BYTE pkt[MAX_PACKET_SIZE];
		INT pktsize = PacketMaker::MakePacketData_ReqFileData(bCharaScr, nID, nFileNo, 0, pkt);
		return g_pGame->AddPacket(pkt, pktsize);
	}
	else
	{
		SafeDeleteArray(recvPath);
		// 次のファイルのハッシュ値を要求
		BYTE pkt[MAX_PACKET_SIZE];
		INT pktsize = PacketMaker::MakePacketData_ReqFileHash(bCharaScr, nID, nFileNo+1, pkt);
		return g_pGame->AddPacket(pkt, pktsize);
	}

	return TRUE;
}

BOOL PacketProc_File_PK_USER_FILEINFO_SEND(BYTE* data)
{
	// size			:	2
	// header		:	1
	// scr			:	1
	// id				:	2
	// fileno			:	1
	// fiileindex	:	4
	// size			:	4		// size=0で終わり
	// data			:	size

	int nIndex = 4;
	BOOL bCharaScr = (BOOL)data[nIndex];
	nIndex++;
	WORD nID = 0;
	memcpy(&nID, &data[nIndex], sizeof(WORD));
	nIndex+= sizeof(WORD);
	int nFileNo = data[nIndex];
	nIndex++;
	BOOL bEof = data[nIndex];
	nIndex++;
	DWORD dwFileIndex = 0;
	memcpy(&dwFileIndex, &data[nIndex], sizeof(DWORD));
	nIndex+=sizeof(DWORD);
	DWORD dwSize = 0;
	memcpy(&dwSize, &data[nIndex], sizeof(DWORD));
	nIndex+=sizeof(DWORD);
	
	_ASSERT(s_pfRecvFile);

	// size=0 fclose
	if (dwSize)
	{
		fflush(s_pfRecvFile);
		BYTE* buf = new BYTE[dwSize];
		memset(buf, 0, dwSize);
		memcpy(buf, &data[nIndex], dwSize);
//		fwrite(&data[nIndex], 1, dwSize, s_pfRecvFile);
		fwrite(buf, 1, dwSize, s_pfRecvFile);
		SafeDeleteArray(buf);
	}
	int nTotalWrite = ftell(s_pfRecvFile);
	// 終端
	if (bEof)
	{
		// ファイルを閉じる
		if (s_pfRecvFile)
			fclose(s_pfRecvFile);
		s_pfRecvFile = NULL;
		AddMessageLog(L"FileClose & ReqNextHash");
		// 次のファイルのハッシュ値を要求
		BYTE pkt[MAX_PACKET_SIZE];
		INT pktsize = PacketMaker::MakePacketData_ReqFileHash(bCharaScr, nID, nFileNo+1, pkt);
		return g_pGame->AddPacket(pkt, pktsize);
	}
	else
	{
		BYTE pkt[MAX_PACKET_SIZE];
		INT pktsize = PacketMaker::MakePacketData_ReqFileData(bCharaScr, nID, nFileNo, nTotalWrite, pkt);
		return g_pGame->AddPacket(pkt, pktsize);
	}
	return TRUE;
}

BOOL PacketProc_File_PK_USER_FILEINFO_CLOSE(BYTE* data)
{
	int nIndex = 4;
	BOOL bCharaScr = (BOOL)data[nIndex];
	nIndex++;
	WORD nID = 0;
	memcpy(&nID, &data[nIndex], sizeof(WORD));
	WCHAR msg[64];
	if (bCharaScr)
		SafePrintf(msg, 64, L"キャラスクリプト(ID:%d)の受信が終わりました。", nID);
	else
		SafePrintf(msg, 64, L"ステージスクリプト(ID:%d)の受信が終わりました。", nID);
	AddMessageLog(msg);
	MessageBox(g_hWnd, msg, L"ログイン", MB_ICONINFORMATION|MB_OK);

	return TRUE;
}
