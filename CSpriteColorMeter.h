#ifndef H_SPRITE_COLOR_METER_CLASS___
#define H_SPRITE_COLOR_METER_CLASS___
// ===================================================================
// CSpriteColorMeter.h
//	色付けメータ・ヘッダー
//	Author : ue
// ===================================================================

#include "math.h"
#include "util.h"
#include "define.h"

#include "CFiler.h"
#include "CSplitTextures.h"
#include "CGraphic.h"

#define SPRITE_COLOR_METER_LEFT			(2)
#define SPRITE_COLOR_METER_TOP			(2)
#define SPRITE_COLOR_METER_WIDTH		(220)
#define SPRITE_COLOR_METER_HEIGHT		(40)

#define SPRITE_COLOR_METER_DEC_COLOR	(0xFFFF0000)
#define SPRITE_COLOR_METER_NORM_COLOR	(0xFFFFFFFF)

#define SPRITE_COLOR_METER_MAX_VALUE	(100)


class CSpriteColorMeter
{
public:
	CSpriteColorMeter();
	virtual ~CSpriteColorMeter();

	BOOL Create(LPDIRECT3DDEVICE9 pDev, CGraphic *pGraph, CSplitTextures *pSpTex, CFiler* pFiler, char* flamefile, char* meterfile);
	inline void Init(D3DXVECTOR2 vecMeterPos, D3DXVECTOR2 vecFlamePos, int nValue = SPRITE_COLOR_METER_MAX_VALUE,int nMaxValue=SPRITE_COLOR_METER_MAX_VALUE, int nDecValue=0)
	{
		SetPos(vecMeterPos, vecFlamePos);
		SetValue(nValue, nMaxValue, nDecValue);
	};
	inline void SetPos(D3DXVECTOR2 vecMeterPos, D3DXVECTOR2 vecFlamePos)
	{
		m_vecMeterPos = vecMeterPos;
		m_vecFlamePos = vecFlamePos;
	};
	inline void SetValue(int nValue, int nMaxValue, int nDecValue)
	{		
		m_nMaxValue = nMaxValue;
		m_nValue = nValue;
		m_nDecValue = nDecValue;
	};

	// Draw
	void Draw();

public:
	// 変数

protected:
	// 関数
protected:
	// 変数
	CGraphic*		m_pGraph;
	CSpTexs*		m_pMeterTex;	// 画像
	CSpTexs*		m_pFrameTex;

	D3DXVECTOR2		m_vecMeterPos;
	D3DXVECTOR2		m_vecFlamePos;

	int				m_nMaxValue;
	int				m_nValue;
	int				m_nDecValue;
	float			m_nMeterHeight;
	float			m_nMeterWidth;
	
};

#endif
