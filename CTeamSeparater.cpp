#include "CTeamSeparater.h"

WCHAR* c_tTeamTextTable[] =
{
	L"‡@",	L"‡A",	L"‡B",	L"‡C",	L"‡D",
	L"‡E",	L"‡F",	L"‡H",	L"‡H",	L"‡H"
};

D3DCOLOR c_tTeamColorTable[] =
{
	0xFFFF0000	,0xFF0000FF	,0xFF00e000	,0xFFFF7FFF,0xFFFFFF00,
	0xFFffae00	,0xFF00ffff		,0xFF000000 ,0xFFd8d8d8	,0xFFc000c0,
};

HRESULT CTeamSeparater::Create(int nGUIResourceIndex, CDXUTDialog *pDialog, int nSeparate)
{
	m_nGUIResourceIndex = nGUIResourceIndex;
	p_pDialog = pDialog;
	m_nSeparate = nSeparate;
	m_bVisible = false;
	
	// ì¬Ï‚Ý‚È‚çƒ|ƒCƒ“ƒ^Ý’è‚Ì‚Ý
	if (pDialog->GetButton(IDC_TEAM_SEPARAT_BASE))
	{
		for (int i=0;i<MAX_TEAM_COUNT;i++)
			 m_pArrayTeamNum[i] = pDialog->GetButton(IDC_TEAM_SEPARAT_BASE);
//		m_pBGImage = pDialog->GetButton(IDC_TEAM_SEPARAT_BG);
		return S_OK;
	}

	// ”wŒi‰æ‘œ—pƒ{ƒ^ƒ“‚ðì¬
//	p_pDialog->AddButton(IDC_TEAM_SEPARAT_BASE,
//		L"",
//		ROOM_SEPARATER_OFFSET_X,
//		ROOM_SEPARATER_OFFSET_Y,
//		WIN_WIDTH,
//		ROOM_SEPARATER_OFFSET_H,
//		0,false, &m_pBGImage
//	);

//	m_pBGImage->GetElement(0)->TextureColor.Init( 0xFFFFFFFF, 0xFFFFFFFF,0 );
//	m_pBGImage->GetElement(1)->TextureColor.Init(0, 0);
//	m_pBGImage->SetStateMouserOverOffset(0,0);
//	m_pBGImage->SetStatePressedOverOffset(0,0);
	for (int i=0;i<MAX_TEAM_COUNT;i++)
	{
		CDXUTButton* pBtn;
		p_pDialog->AddButton(IDC_TEAM_SEPARAT_BASE, c_tTeamTextTable[i], 0,0, 0,0,0,false, &pBtn);
		m_pArrayTeamNum[i] = pBtn;

		pBtn->SetTextColor(0x00);
		pBtn->GetElement(0)->TextureColor.Init( 0,0,0 );
		pBtn->GetElement(1)->TextureColor.Init(0, 0);
		pBtn->GetElement(1)->SetFont(1,0,0);
		pBtn->SetStateMouserOverOffset(0,0);
		pBtn->SetStatePressedOverOffset(0,0);
		CDXUTElement* pElement = pBtn->GetElement(0);
		pElement->SetFont(4, 0xFF000000, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		// •ªŠ„ü
		m_pntLine[i][0].y = ROOM_SEPARATER_OFFSET_LINE_Y;
		m_pntLine[i][1].y = ROOM_SEPARATER_OFFSET_LINE_Y+ROOM_SEPARATER_OFFSET_H;

		pBtn->SetVisible(false);
	}

	// ”wŒi‰æ‘œŽw’è
//	RECT rcTexture;
//	SetRect(&rcTexture,
//		ROOM_SEPARATER_RES_X,
//		ROOM_SEPARATER_RES_Y,
//		ROOM_SEPARATER_RES_W,
//		ROOM_SEPARATER_RES_H);
//	CDXUTElement* pElement = m_pBGImage->GetElement(0);
//	pElement->SetTexture(m_nGUIResourceIndex,&rcTexture);

	m_bCreated = TRUE;
	Update(nSeparate);
	return S_OK;
}

// À•W‚©‚çƒ`[ƒ€”Ô†‚ðŽæ“¾
int CTeamSeparater::GetTeamNoFromPos(int x, int y)
{
	if (!m_bCreated)	return 0;
	if (!m_nSeparate) return 0;
	if (m_nSeparate == 1) return 1;
	
	int nSeparateWidth = ROOM_ENTRY_WIDTH / m_nSeparate;

	if (x >= ROOM_ENTRY_LEFT && x < ROOM_ENTRY_RIGHT)
		return min(MAX_TEAM_COUNT-1,min((int)((x-ROOM_ENTRY_LEFT) / nSeparateWidth), m_nSeparate-1));
	return MAX_TEAM_COUNT;
}

// XV
// nSeparate	:	ƒ`[ƒ€”
void CTeamSeparater::Update(int nSeparate)
{
	if (!m_bCreated)	return;
	if (nSeparate <= 1)
	{
		m_nSeparate = 1;
		// ‰B‚·
		for (int i=0;i < MAX_TEAM_COUNT;i++)
			m_pArrayTeamNum[i]->SetVisible(false);
		return;
	}
	m_nSeparate = nSeparate;

	int nSeparateWidth = ROOM_ENTRY_WIDTH / m_nSeparate;

	RECT rcButton;
	rcButton.top = ROOM_SEPARATER_OFFSET_Y;
	rcButton.bottom = ROOM_SEPARATER_OFFSET_H;
	
	for (int i=0;i < MAX_TEAM_COUNT;i++)
	{
		// •ªŠ„”ˆÈã‚Ìê‡‰B‚·
		if (i >= nSeparate)
		{
			m_pArrayTeamNum[i]->SetVisible(false);
			continue;
		}
		if (!m_pArrayTeamNum[i]->GetVisible())
			m_pArrayTeamNum[i]->SetVisible(m_bVisible);
		
		// ƒ{ƒ^ƒ“ˆÊ’uAƒTƒCƒY‚ÌÝ’è
		rcButton.left = i*nSeparateWidth + ROOM_ENTRY_LEFT;
		rcButton.right = nSeparateWidth*(i+1);
		m_pArrayTeamNum[i]->SetSize(nSeparateWidth, ROOM_SEPARATER_OFFSET_H);
		m_pArrayTeamNum[i]->SetLocation(rcButton.left, ROOM_CHARA_BASE_OFFSET_Y+16);
		// •ªŠ„ü
		if (i>0)
		{
			m_pntLine[i-1][0].x = rcButton.left;
			m_pntLine[i-1][1].x = rcButton.left;
		}
	}
//	if (!m_pBGImage->GetVisible())
//		m_pBGImage->SetVisible(true);
}

// •`‰æ
void CTeamSeparater::Render()
{
	if (!m_bCreated)	return;
	if (!m_bVisible) return;
	if (m_nSeparate <= 1) return;
	
	for (int i=0;i<m_nSeparate-1;i++)
		p_pDialog->DrawPolyLine(&m_pntLine[i][0], 2, ROOM_SEPARATER_LINE_COLOR);
}

void CTeamSeparater::Destroy()
{
	m_bCreated = FALSE;
}

WCHAR* CTeamSeparater::GetTeamWchar(int num)
{
	return c_tTeamTextTable[num];
}

D3DCOLOR CTeamSeparater::GetTeamColor(int num)
{
	return c_tTeamColorTable[num];
}

void CTeamSeparater::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
	if (bVisible)
	{
		for (int i=0;i<m_nSeparate;i++)
		{
			if (m_pArrayTeamNum[i])
				m_pArrayTeamNum[i]->SetVisible(true);
		}
	}
	else
	{
		for (int i=0;i<MAX_TEAM_COUNT;i++)
		{
			if (m_pArrayTeamNum[i])
				m_pArrayTeamNum[i]->SetVisible(false);
		}
	}
}

