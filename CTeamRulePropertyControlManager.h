#ifndef H_TEAM_RULE_PROPERTY_CONTROL_CLASS___
#define H_TEAM_RULE_PROPERTY_CONTROL_CLASS___
// ===================================================================
// CTeamRulePropertyControlManager.h
//	チーム分け設定用コントロール管理クラス・ヘッダー
// 左右ボタンが押されたらテキストを変更するだけ
// ===================================================================

#include <windows.h>
#include <TCHAR.h>
#include <vector>

#include "util.h"
#include "../include/define.h"
#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"

#define MAX_TEAMRULEPROPCONT_TEXT	(16)

#define TEAMRULEPROPCONT_TEAM_ZERO	L"個人バトル"
#define TEAMRULEPROPCONT_TEXTFORMAT	L"%dチームバトル"
#define TEAMRULEPROPCONT_FONT	(4)

class CTeamRulePropertyControlManager
{
public:
	CTeamRulePropertyControlManager();
	virtual ~CTeamRulePropertyControlManager() { };
	// 生成
	// pBtnArrowL	: 左矢印ボタン
	// pBtnArrowR	: 右矢印ボタン
	// pStaticText	: 文字表示用スタティック
	// pntLocate		: 左上位置
	void Create(CDXUTButton *pBtnArrowL, CDXUTButton *pBtnArrowR, CDXUTStatic* pStaticText, int *pCount);
	void Destroy();

	void Update(bool b, int nTeamCount);
	void OnRightButtonDown();
	void OnLeftButtonDown();

	void SetEnable(BOOL b);
	BOOL GetEnable(BOOL b)	{	m_bEnable = b;	};
	void SetVisible(bool b);
	BOOL GetVisible()	{	return m_bVisible;	};

protected:
	void UpdateText(int nTeamCount);
	void SetButtonEnable(CDXUTButton* pBtn, bool b);

	BOOL	m_bCreated;
	BOOL	m_bEnable;
	BOOL	m_bVisible;
	POINT	m_pntLocate;
	CDXUTButton*	m_pBtnArrowL;
	CDXUTButton*	m_pBtnArrowR;
	CDXUTStatic*	m_pStaticText;
	int			*p_pUserCount;
};

#endif
