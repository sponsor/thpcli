#include "Inc.h"
#include "CSpriteColorMeter.h"

CSpriteColorMeter::CSpriteColorMeter()
{
	m_pGraph = NULL;
	m_pMeterTex = NULL;	// 画像
	m_pFrameTex = NULL;
	m_nMeterHeight = 0;
	m_nMeterWidth = 0;

	Init(D3DXVECTOR2(0,0),D3DXVECTOR2(0,0));
}

CSpriteColorMeter::~CSpriteColorMeter()
{
	SafeDelete(m_pMeterTex);
	SafeDelete(m_pFrameTex);
}

BOOL CSpriteColorMeter::Create(LPDIRECT3DDEVICE9 pDev, CGraphic *pGraph, CSplitTextures *pSpTex, CFiler* pFiler, char* flamefile, char* meterfile)
{
	if (!pDev || !pGraph || !pSpTex || !pFiler)
		return FALSE;

	m_pGraph = pGraph;

	// SE読込み
	BYTE *pImgBuffer;
	size_t nImgBufferSize = 0;

	if (!pFiler->GetFileMemory(meterfile, &pImgBuffer, &nImgBufferSize))
		return FALSE;

	BOOL ret = pSpTex->CreateSpTexsFromFileInMemoryEx(&m_pMeterTex,pImgBuffer, (int)nImgBufferSize, 0,0,0);
	SafeDeleteArray(pImgBuffer);		// バッファクリア
	if (!ret)	return FALSE;

	D3DSURFACE_DESC desc;
	m_pMeterTex->GetDesc(&desc);
	m_nMeterHeight = desc.Height;
	m_nMeterWidth = SPRITE_COLOR_METER_WIDTH;

	if (flamefile)
	{
		if (!pFiler->GetFileMemory(flamefile, &pImgBuffer, &nImgBufferSize))
		{
			SafeDelete(m_pMeterTex);
			return FALSE;
		}

		ret = pSpTex->CreateSpTexsFromFileInMemoryEx(&m_pFrameTex ,pImgBuffer, (int)nImgBufferSize, 0,0,0);
		SafeDeleteArray(pImgBuffer);		// バッファクリア
		if (!ret)	return FALSE;
	}

	return TRUE;
}

void CSpriteColorMeter::Draw()
{
	D3DXVECTOR2 vec11 = D3DXVECTOR2(1,1);
	D3DXVECTOR2 vec00 = D3DXVECTOR2(0,0);

	if (!m_nMaxValue)	return;

	// メータ
	if (m_nValue)
	{
		RECT rec;
		int nWidth = ((float)m_nValue/(float)m_nMaxValue) * (float)m_nMeterWidth;
		SetRect(&rec, 0,0, nWidth, m_nMeterHeight);
		m_pGraph->RegistSpriteList(*m_pMeterTex->Get(0), &rec, vec11, vec00, 0.0f, m_vecMeterPos, SPRITE_COLOR_METER_NORM_COLOR);

	}	
	if (m_nDecValue)
	{
		RECT rec;
		int nWidth = ((float)min(m_nValue,m_nMaxValue)/(float)m_nMaxValue) * (float)m_nMeterWidth;
		SetRect(&rec, 0,0, nWidth, m_nMeterHeight);
		D3DXVECTOR2 vec = m_vecMeterPos;
		rec.left = rec.right;
		rec.right = rec.left
				+ ((float)(max(min(m_nDecValue,m_nMaxValue),0))/(float)m_nMaxValue) * (float)m_nMeterWidth;
		if (rec.right)
		{
			vec.x += rec.left;
			m_pGraph->RegistSpriteList(
				*m_pMeterTex->Get(0),
				&rec,
				vec11,
				vec00,
				0.0f,
				vec,
				SPRITE_COLOR_METER_DEC_COLOR);
		}
	}
	// 枠
	if (m_pFrameTex)
		m_pGraph->RegistSpriteList(*m_pFrameTex->Get(0), NULL, vec11, vec00, 0.0f, m_vecFlamePos, 0xFFFFFFFF);
}
