#include "CShotAngleIndicator.h"


BOOL CShotAngleIndicator::Create(CDXUTDialog* pDialog, int nID, int nPosX, int nPosY, int nLineLength)
{
	if (!pDialog)	return FALSE;

	CDXUTAngleLine* pAngle = NULL;
	if (FAILED(pDialog->AddAngleLine(nID, 0, nLineLength, nPosX, nPosY, nLineLength, nLineLength, false, &pAngle)))
		return FALSE;
	pAngle->GetElement(0)->TextureColor.Init( 0,0,0 );
	pAngle->SetLineColor(SHOT_ANGLE_INDICATOR_COLOR);
	p_pDialog = pDialog;
	m_nID = nID;
	m_pLine = pAngle;
	m_nAngle = m_pLine->GetAngle();
	m_bCreated = TRUE;
	return TRUE;
}

void CShotAngleIndicator::OnMouseClick(int nX, int nY)
{
	if (!m_bCreated)	return; 
	int mx = MAIN_AL_SHOT_ANGLE_INDICATOR_CX-nX;
	int my = MAIN_AL_SHOT_ANGLE_INDICATOR_CY-nY;
	
	if (MAIN_AL_SHOT_ANGLE_INDICATOR_LENGTH*MAIN_AL_SHOT_ANGLE_INDICATOR_LENGTH >= mx*mx 
	&& MAIN_AL_SHOT_ANGLE_INDICATOR_LENGTH*MAIN_AL_SHOT_ANGLE_INDICATOR_LENGTH >= my*my
	)
	{
		int nAngle = GetAngle(MAIN_AL_SHOT_ANGLE_INDICATOR_CX-nX,MAIN_AL_SHOT_ANGLE_INDICATOR_CY-nY);
		m_pLine->SetAngle(nAngle+REVERSE_ANGLE);
		m_nAngle = m_pLine->GetAngle();
	}
}


void CShotAngleIndicator::OnRender(IDirect3DDevice9* pd3dDevice, float fElapsedTime)
{
	if (m_pLine)
		m_pLine->Render(pd3dDevice, fElapsedTime);
}
