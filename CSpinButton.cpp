#include "CSpinButton.h"

CSpinButton::CSpinButton()
{
	m_bCreated = FALSE;
	m_bVisible = false;
	m_nValue = 0;
	m_bEnabled = true;

	p_pDialog = NULL;
	m_pIncrement = NULL;				// 
	m_pDecrement = NULL;				// 
}

void CSpinButton::Destroy()
{
	if (m_bCreated)
	{
		p_pDialog->RemoveControl(m_pIncrement->GetID());
		m_pIncrement = NULL;
		p_pDialog->RemoveControl(m_pDecrement->GetID());
		m_pDecrement = NULL;
		m_bCreated = FALSE;
	}
	m_nValue = 0;
	m_bVisible = false;
}

BOOL CSpinButton::Create(CDXUTDialog* pDialog, int nBaseID,int nResourceIndex, int x, int y, int bw, int bh, int sw,int sh)
{
	if (!pDialog || m_bCreated)	return FALSE;

	RECT rcTexture;
	p_pDialog = pDialog;

	// Inc
	pDialog->AddButton(nBaseID, L"",x,y,bw, bh, 0, false, &m_pIncrement);
	m_pIncrement->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	m_pIncrement->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	m_pIncrement->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_pIncrement->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);

	SetRect( &rcTexture, ROOM_BTN_INC_IMG_RECT);	// 描画範囲
	m_pIncrement->GetElement(1)->SetTexture( nResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	m_pIncrement->SetStateMouserOverOffset(0,0);
	m_pIncrement->SetStatePressedOverOffset(0,-1);
	m_pIncrement->SetVisible(false);

	// Dec
	pDialog->AddButton(nBaseID+1, L"",x,y+bh+sh,bw, bh, 0, false, &m_pDecrement);
	m_pDecrement->GetElement(0)->TextureColor.Init( D3DCOLOR_ARGB( 0, 255, 255, 255 ) );  // Transparent center
	m_pDecrement->GetElement(1)->TextureColor.States[ DXUT_STATE_NORMAL ] =D3DCOLOR_ARGB(255,255,255,255);
	m_pDecrement->GetElement(1)->TextureColor.States[ DXUT_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_pDecrement->GetElement(1)->FontColor.States[ DXUT_STATE_NORMAL ]=D3DCOLOR_ARGB(255,255,255,255);
	
	SetRect( &rcTexture, ROOM_BTN_DEC_IMG_RECT);	// 描画範囲
	m_pDecrement->GetElement(1)->SetTexture( nResourceIndex, &rcTexture );	// 追加した管理テクスチャ番号を指定
	m_pDecrement->SetStateMouserOverOffset(0,0);
	m_pDecrement->SetStatePressedOverOffset(0,1);
	m_pDecrement->SetVisible(false);

	// Digit
	pDialog->AddStatic( nBaseID+2,L"0", x, y+bh, sw, sh, false, &m_pDigit);
	m_pDigit->GetElement(0)->SetFont(ROOM_STATIC_TURN_LIMIT_FONT);
	m_pDigit->SetVisible(false);

	m_nValue = 0;
	m_bCreated = TRUE;
	m_bVisible = false;
	return TRUE;
}

void CSpinButton::UpdateDigit()
{
	WCHAR wsDigit[3];
	_itow_s(m_nValue, wsDigit, sizeof(WCHAR)*2, 10);
	if (m_pDigit)
		m_pDigit->SetText(wsDigit);
}
