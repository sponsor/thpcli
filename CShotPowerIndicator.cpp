#include "CShotPowerIndicator.h"


BOOL CShotPowerIndicator::Create(CDXUTDialog* pDialog, int nID, int nResourceIndex, RECT* prcIndicateRange, RECT* prcCntRange, RECT* prcTexture)
{
	if (!pDialog || !prcIndicateRange || !prcCntRange || !prcTexture)
		return FALSE;

	CDXUTSprite* pSprite = NULL;
	if (FAILED(pDialog->AddSprite(nID, prcCntRange->left, prcCntRange->top, prcCntRange->right, prcCntRange->bottom, false, &pSprite)))
		return FALSE;

	pSprite->GetElement(0)->SetTexture(nResourceIndex, prcTexture);
	CopyRect(&m_rcIndicateRange, prcIndicateRange);
	CopyRect(&m_rcControl, prcCntRange);

	p_pDialog = pDialog;
	m_nID = nID;
	m_pSprite = pSprite;
	m_bCreated = TRUE;
	return TRUE;
}

void CShotPowerIndicator::OnMouseClick(int nX, int nY)
{
	if (!m_bCreated)	return; 

	if (m_rcIndicateRange.left <= nX && nX <= m_rcIndicateRange.right
	&& m_rcIndicateRange.top <= nY && nY <= m_rcIndicateRange.bottom)
	{
		int nCX = nX - ((m_rcControl.right) / 2);
		m_nCX = nX-127;
		m_pSprite->SetLocation(nCX, m_pSprite->m_y);
	}
}

