#ifndef H_SHOT_ANGLE_INDICATOR___
#define H_SHOT_ANGLE_INDICATOR___
// ===================================================================
// CShotAngleIndicator.h
//	角度の目安線を表示するクラス
// ===================================================================

#include <windows.h>
#include <TCHAR.h>

#include "util.h"
#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"
#include "resource.h"

#define SHOT_ANGLE_INDICATOR_COLOR		(0xDD00FF00)

class CShotAngleIndicator
{
public:
	CShotAngleIndicator()
	{
		m_bVisible = true;
		m_nID = -1;
		p_pDialog = NULL;
		m_pLine = NULL;
	};

	virtual ~CShotAngleIndicator() { Destroy(); };

	virtual BOOL Create(CDXUTDialog* pDialog, int nID, int nPosX, int nPosY, int nLineLength);

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
		m_pLine = NULL;
	};

	virtual void OnRender(IDirect3DDevice9* pd3dDevice, float fElapsedTime);
	virtual void OnMouseClick(int nX, int nY);
	virtual void SetVisible(bool value)
	{
		m_bVisible = value;
		if (m_bCreated)
			m_pLine->SetVisible(value);
	};

	virtual bool GetVisible()
	{
		return m_bVisible;
	};
	virtual int GetCurrentAngle()
	{
		return m_nAngle;
	}
protected:
	BOOL m_bCreated;
	CDXUTDialog* p_pDialog;
	CDXUTAngleLine* m_pLine;		// 地面前;
	int	m_nID;
	bool m_bVisible;
	int m_nAngle;
//	int m_nCenterPosX;
//	int m_nCenterPosY;
//	int m_nLength;
};

#endif
