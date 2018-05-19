#ifndef H_SHOT_ANGLE___
#define H_SHOT_ANGLE___
// ===================================================================
// CShotAngle.h
//	弾撃つ角度を視覚化するクラス
// ===================================================================

#include <windows.h>
#include <TCHAR.h>

#include "util.h"

#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"
#include "resource.h"

#define SHOT_ANGLE_MAX	(360)
#define SHOT_ANGLE_BASE_COLOR		(0xFF0000FF)
#define SHOT_ANGLE_RANGE_COLOR	(0xFFFF7F7F)
#define SHOT_ANGLE_COLOR				(0xFF000000)

class CShotAngle
{
public:
	CShotAngle();
	virtual ~CShotAngle() { Destroy(); };

	BOOL Create(CDXUTDialog* pDialog, int nResourceIndex, int x, int y, int w,int h);
	void Destroy();

	bool GetVisible() { return m_bVisible; };
	void SetVisible(bool b)
	{
		if (m_bCreated)
		{
			if (m_pBaseLineFront->GetVisible() != b)
				m_pBaseLineFront->SetVisible(b);
			if (m_pBaseLineBack->GetVisible() != b)
				m_pBaseLineBack->SetVisible(b);
			if (m_pShotRangeMin->GetVisible() != b)
				m_pShotRangeMin->SetVisible(b);
			if (m_pShotRangeMax->GetVisible() != b)
				m_pShotRangeMax->SetVisible(b);
			if (m_pShotAngle->GetVisible() != b)
				m_pShotAngle->SetVisible(b);
		}
		m_bVisible = b;
	};
	void SetSize(int w,int h)
	{
		m_nWidth = w;	m_nHeight = h;
		int len = w?(int)(w/2):0;	
		if (m_bCreated)
		{
			m_pBaseLineFront->SetSize(w,h);
			m_pBaseLineFront->SetLength((int)(len*0.9));
			m_pBaseLineBack->SetSize(w,h);
			m_pBaseLineBack->SetLength((int)(len*0.9));
			m_pShotRangeMin->SetSize(w,h);
			m_pShotRangeMin->SetLength((int)(len*0.9));
			m_pShotRangeMax->SetSize(w,h);
			m_pShotRangeMax->SetLength((int)(len*0.9));
			m_pShotAngle->SetSize(w,h);
			m_pShotAngle->SetLength(len);
		}
	};
	void SetLocation(int x,int y)
	{
		m_nX = x;	m_nY = y;
		if (m_bCreated)
		{
			m_pBaseLineFront->SetLocation(x,y);
			m_pBaseLineBack->SetLocation(x,y);
			m_pShotRangeMin->SetLocation(x,y);
			m_pShotRangeMax->SetLocation(x,y);
			m_pShotAngle->SetLocation(x,y);
		}
	};
	int GetShotAngle() { return m_pShotAngle->GetAngle();	 };
	int GetAngle() { return m_nAngle;	 };
	void SetAngle(int value)
	{
		m_nAngle = min(m_nRangeAngleMax%360, max( m_nRangeAngleMin%360, value%360));
		UpdateAngle();
	};
	int GetBaseLineAngle() { return m_nBaseLineAngle;	 };
	void SetBaseLineAngle(int value)
	{
		m_nBaseLineAngle = value%360;
		UpdateAngle();
	};
	int GetRangeAngleMin() { return m_nRangeAngleMin;	 };
	void SetRangeAngleMin(int value)
	{
		value = value%360;
		m_nRangeAngleMin = value;
		if (m_nRangeAngleMax<m_nRangeAngleMin)
			m_nAngle = m_nRangeAngleMax = m_nRangeAngleMin;
		UpdateAngle();
	};
	int GetRangeAngleMax() { return m_nRangeAngleMax;	 };
	void SetRangeAngleMax(int value)
	{
		value = value%360;
		m_nRangeAngleMax = value;
		if (m_nRangeAngleMax<m_nRangeAngleMin)
			m_nAngle = m_nRangeAngleMin = m_nRangeAngleMax;
		UpdateAngle();
	};
	void UpdateAngle();

	void OnRender(IDirect3DDevice9* pd3dDevice, float fElapsedTime);

protected:
	BOOL m_bCreated;
	bool m_bVisible;
	int m_nX;
	int m_nY;
	int m_nWidth;
	int m_nHeight;

	int m_nAngle;
	int m_nBaseLineAngle;
	int m_nRangeAngleMin;
	int m_nRangeAngleMax;

	CDXUTDialog* p_pDialog;
	CDXUTAngleLine* m_pBaseLineFront;		// 地面前
	CDXUTAngleLine* m_pBaseLineBack;		// 地面後ろ
	CDXUTAngleLine* m_pShotRangeMin;		// 最低発射角度
	CDXUTAngleLine* m_pShotRangeMax;	// 最低発射角度
	CDXUTAngleLine* m_pShotAngle;			// 発射角度

};

#endif
