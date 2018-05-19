#ifndef H_TEAM_SEPARATER_CLASS___
#define H_TEAM_SEPARATER_CLASS___
// ===================================================================
// CTeamSeparater.h
//	チームわけの線を出すクラス・ヘッダー
// ===================================================================

#include <windows.h>
#include <TCHAR.h>
#include "../include/types.h"
#include "../include/define.h"
#include "util.h"
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <map>
#include <iostream>
#include <vector>
#include "../include/define.h"

#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"

// チーム番号（①②③…）の位置取り
// チームの区切り線の表示

#define ROOM_SEPARATER_OFFSET_X	0
#define ROOM_SEPARATER_OFFSET_Y	(ROOM_CHARA_BASE_OFFSET_Y+32)
#define ROOM_SEPARATER_OFFSET_LINE_Y	(ROOM_CHARA_BASE_OFFSET_Y+16)
#define ROOM_SEPARATER_OFFSET_H	(ROOM_CHARA_BASE_MAX_MOVE_H+16)

#define IDC_TEAM_SEPARAT_BG			8999
#define IDC_TEAM_SEPARAT_BASE		9000

#define ROOM_SEPARATER_RES_X			0
#define ROOM_SEPARATER_RES_Y			0
#define ROOM_SEPARATER_RES_W		(ROOM_SEPARATER_RES_X+WIN_WIDTH)
#define ROOM_SEPARATER_RES_H			(ROOM_SEPARATER_RES_Y+ROOM_SEPARATER_OFFSET_H)

#define ROOM_SEPARATER_LINE_COLOR	0xFF000000

// チームわけクラス
class CTeamSeparater
{
public:
	CTeamSeparater()
	{
//		m_pBGImage = NULL;
		m_nSeparate = 0;
		m_nGUIResourceIndex = -1;
		p_pDialog = NULL;
		ZeroMemory(m_pArrayTeamNum, sizeof(CDXUTButton*)*MAX_TEAM_COUNT);
		m_bCreated = FALSE;
		SetRectEmpty(&m_recTeam);
		m_bVisible = false;
	};

	virtual ~CTeamSeparater() 
	{ 
		if (m_bCreated)	Destroy();
	};

	// 生成
	// nGUIResourceIndex	: リソース番号
	// pDialog						: スタティック
	// nSeparate					: オブジェクト情報
	HRESULT Create(int nGUIResourceIndex, CDXUTDialog* pDialog, int nSeparate);

	// 更新
	// nSeparate	:	チーム数
	void Update()
	{	Update(m_nSeparate);	};
	// nSeparate	:	チーム数
	void Update(int nSeparate);
		
	void Destroy();
	
	BOOL IsCreated()	{ return m_bCreated;	};

	// 座標からチーム番号を取得
	int GetTeamNoFromPos(int x, int y);
	WCHAR* GetTeamWchar(int num);
	D3DCOLOR GetTeamColor(int num);

	// 描画
	void Render();
	
	void SetVisible(bool bVisible);
	BOOL GetVisible()
	{	return m_bVisible;	};

protected:

	// チーム番号表示用コントロール
	CDXUTButton*	m_pArrayTeamNum[MAX_TEAM_COUNT+1];
	CDXUTDialog*	p_pDialog;
//	CDXUTButton*	m_pBGImage;
	
	BOOL	m_bCreated;								// 生成済み
	RECT		m_recTeam;								// １チームの描画範囲
	bool		m_bVisible;								// 表示状態
	int			m_nGUIResourceIndex;				// リソース番号
	int			m_nSeparate;							// 分割数

	POINT	m_pntLine[MAX_TEAM_COUNT][2];	// 分割線
};

#endif
