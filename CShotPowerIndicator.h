#ifndef H_SHOT_POWER_INDICATOR___
#define H_SHOT_POWER_INDICATOR___
// ===================================================================
// CShotPowerIndicator.h
//	パワーの目安線を表示するクラス
// ===================================================================

#include <windows.h>
#include <TCHAR.h>

#include "util.h"
#include "../include/define.h"
#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"
#include "resource.h"

class CShotPowerIndicator
{
public:
	CShotPowerIndicator()
	{
		m_nCX = 0;
		m_nID = -1;
		p_pDialog = NULL;
		m_pSprite = NULL;
		SetRectEmpty(&m_rcIndicateRange);
	};

	virtual ~CShotPowerIndicator() { Destroy(); };

	virtual BOOL Create(CDXUTDialog* pDialog, int nID, int nResourceIndex, RECT* prcIndicateRange, RECT* prcCntRange, RECT* prcTexture);

	virtual void Destroy()
	{
		if (m_bCreated)
		{
			CDXUTControl* pControl = p_pDialog->GetControl(m_nID);
			if (pControl)
				p_pDialog->RemoveControl(m_nID);
			m_bCreated = FALSE;
		}
		m_nID = -1;
		p_pDialog = NULL;
		m_pSprite = NULL;
		SetRectEmpty(&m_rcIndicateRange);
	};

	virtual void OnMouseClick(int nX, int nY);
	virtual void SetVisible(bool value)
	{
		if (m_bCreated)
			m_pSprite->SetVisible(value);
	};

	virtual bool GetVisible()
	{
		if (m_bCreated)
			return m_pSprite->GetVisible();
		return false;
	};

	int GetCXForView(void) { 
		if (m_nCX == 0)
			return 0;
		return (int)((float)(m_nCX + 1) / (float)MAX_SHOT_INDICATOR * (float)MAX_SHOT_POWER);
	};
#ifdef HOGE
	int GetPosX() { return m_pSprite->m_x;	};
#endif
protected:
	int m_nCX;
	BOOL m_bCreated;
	CDXUTDialog* p_pDialog;
	int m_nID;
	CDXUTSprite* m_pSprite;
	RECT m_rcIndicateRange;
	RECT m_rcControl;

};

#endif
