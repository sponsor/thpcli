#include "CShotAngle.h"

CShotAngle::CShotAngle()
{
	m_bCreated = FALSE;
	m_bVisible = false;
	m_nX = 0;
	m_nY = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	
	m_nAngle = 0;
	m_nBaseLineAngle = 0;
	m_nRangeAngleMin = 0;
	m_nRangeAngleMax = 0;

	p_pDialog = NULL;
	m_pBaseLineFront = NULL;		// ’n–Ê‘O
	m_pBaseLineBack = NULL;			// ’n–ÊŒã‚ë
	m_pShotRangeMin = NULL;		// Å’á”­ŽËŠp“x
	m_pShotRangeMax = NULL;		// Å’á”­ŽËŠp“x
	m_pShotAngle = NULL;				// ”­ŽËŠp“x

}

void CShotAngle::Destroy()
{
	if (m_bCreated)
	{
		p_pDialog->RemoveControl(m_pBaseLineFront->GetID());
		m_pBaseLineFront = NULL;
		p_pDialog->RemoveControl(m_pBaseLineBack->GetID());
		m_pBaseLineBack = NULL;
		p_pDialog->RemoveControl(m_pShotRangeMin->GetID());
		m_pShotRangeMin = NULL;
		p_pDialog->RemoveControl(m_pShotRangeMax->GetID());
		m_pShotRangeMax = NULL;
		p_pDialog->RemoveControl(m_pShotAngle->GetID());
		m_pShotAngle = NULL;
		m_bCreated = FALSE;
	}
	m_bVisible = false;
}

BOOL CShotAngle::Create(CDXUTDialog* pDialog, int nResourceIndex, int x, int y, int w,int h)
{
	if (!pDialog || m_bCreated)	return FALSE;

	p_pDialog = pDialog;

	pDialog->AddAngleLine(IDC_MAIN_AL_SHOT_ANGLE_BASE+3,
		0, 0,
		x,y,w,h,
		false, &m_pBaseLineFront);
//	SetRect( &rcTexture, ROOM_BTN_STAGE_R_RECT);	// •`‰æ”ÍˆÍ
	m_pBaseLineFront->GetElement(0)->TextureColor.Init( 0,0,0 );
	m_pBaseLineFront->SetLineColor(SHOT_ANGLE_BASE_COLOR);

	pDialog->AddAngleLine(IDC_MAIN_AL_SHOT_ANGLE_BASE+4,
		0, 0,
		x,y,w,h,
		false, &m_pBaseLineBack);
//	SetRect( &rcTexture, ROOM_BTN_STAGE_R_RECT);	// •`‰æ”ÍˆÍ
	m_pBaseLineBack->GetElement(0)->TextureColor.Init( 0,0,0 );
	m_pBaseLineBack->SetLineColor(SHOT_ANGLE_BASE_COLOR);

	pDialog->AddAngleLine(IDC_MAIN_AL_SHOT_ANGLE_BASE+1,
		0, 0,
		x,y,w,h,
		false, &m_pShotRangeMin);
//	SetRect( &rcTexture, ROOM_BTN_STAGE_R_RECT);	// •`‰æ”ÍˆÍ
	m_pShotRangeMin->GetElement(0)->TextureColor.Init( 0,0,0 );
	m_pShotRangeMin->SetLineColor(SHOT_ANGLE_RANGE_COLOR);

	pDialog->AddAngleLine(IDC_MAIN_AL_SHOT_ANGLE_BASE+2,
		0, 0,
		x,y,w,h,
		false, &m_pShotRangeMax);
//	SetRect( &rcTexture, ROOM_BTN_STAGE_R_RECT);	// •`‰æ”ÍˆÍ
	m_pShotRangeMax->GetElement(0)->TextureColor.Init( 0,0,0 );
	m_pShotRangeMax->SetLineColor(SHOT_ANGLE_RANGE_COLOR);

	pDialog->AddAngleLine(IDC_MAIN_AL_SHOT_ANGLE_BASE,
		0, 0,
		x,y,w,h,
		false, &m_pShotAngle);
//	SetRect( &rcTexture, ROOM_BTN_STAGE_R_RECT);	// •`‰æ”ÍˆÍ
	m_pShotAngle->GetElement(0)->TextureColor.Init( 0,0,0 );
	m_pShotAngle->SetLineColor(SHOT_ANGLE_COLOR);

	m_bCreated = TRUE;
	SetLocation(x,y);
	SetSize(w,h);

	return TRUE;
}

void CShotAngle::OnRender(IDirect3DDevice9* pd3dDevice, float fElapsedTime)
{
	m_pBaseLineBack->Render(pd3dDevice, fElapsedTime);
	m_pBaseLineFront->Render(pd3dDevice, fElapsedTime);
	m_pShotRangeMin->Render(pd3dDevice, fElapsedTime);
	m_pShotRangeMax->Render(pd3dDevice, fElapsedTime);
	m_pShotAngle->Render(pd3dDevice, fElapsedTime);
}


void CShotAngle::UpdateAngle()
{
	int ns = (m_nBaseLineAngle > 90 && m_nBaseLineAngle <= 270)?-1:1;

	m_pShotAngle->SetAngle(m_nAngle*ns+m_nBaseLineAngle);
	m_pShotRangeMin->SetAngle(m_nRangeAngleMin*ns+m_nBaseLineAngle);
	m_pShotRangeMax->SetAngle(m_nRangeAngleMax*ns+m_nBaseLineAngle);
	m_pBaseLineFront->SetAngle(m_nBaseLineAngle);
	m_pBaseLineBack->SetAngle( (((m_nBaseLineAngle>180)?-180:180)+m_nBaseLineAngle)%360);
}