#include "CDXUTButtonList.h"

CDXUTButtonList::CDXUTButtonList()
{
	Clear();
	m_bVisible = FALSE;
	m_bytStyle = BUTTON_LIST_HORIZON;
	m_bLRBtnAlwaysVisible = FALSE;
	m_bEnable = true;
}

void CDXUTButtonList::Clear()
{
	m_bCreated = FALSE;
	m_vecButtons.clear();
	ZeroMemory(&m_pntLocate, sizeof(POINT));
	m_nDrawRange = 0;
	m_bytStyle = BUTTON_LIST_HORIZON;
	m_pBtnArrowL = NULL;
	m_pBtnArrowR = NULL;
	m_nViewIndex = 0;
	m_nSpace = 0;
	m_nTotalRange = 0;
	m_nLRPadding = 0;
}

void CDXUTButtonList::Create(CDXUTButton *pBtnArrowL, CDXUTButton *pBtnArrowR, POINT* pntLocate, int nDrawRange, BYTE bytStyle, int nSpace)
{
	m_pBtnArrowL = pBtnArrowL;
	m_pBtnArrowR = pBtnArrowR;
	m_pntLocate = *pntLocate;
	m_nDrawRange = nDrawRange;
	m_bytStyle = bytStyle; 
	m_vecButtons.clear();
	m_nViewIndex = 0;
	m_nSpace = nSpace;
	m_bCreated = TRUE;
}

int CDXUTButtonList::AddButton(CDXUTControl* pBtn)
{
	if (!m_bCreated) return -1;
	m_vecButtons.push_back(pBtn);
	m_nTotalRange += pBtn->m_width;

	UpdateDrawRange();
	return m_vecButtons.size();
}

int CDXUTButtonList::InsertButton(int index, CDXUTControl* pBtn)
{
	if (!m_bCreated) return -1;
	std::vector< CDXUTControl* >::iterator itr = m_vecButtons.begin();
	for (int i=0;i<index;i++)
	{
		if (itr == m_vecButtons.end())
			return -1;
	}
	m_vecButtons.insert(itr, pBtn);
	if (m_nViewIndex > index)
		m_nViewIndex--;

	m_nTotalRange += pBtn->m_width;
	UpdateDrawRange();
	return index;
}

void CDXUTButtonList::OnRightButtonDown()
{
	if (!m_bCreated) return;
	if ((DWORD)m_nViewIndex < m_vecButtons.size()-1)
	{
		m_nViewIndex++;
		CalcViewIndex();
		UpdateDrawRange();
	}
}

void CDXUTButtonList::OnLeftButtonDown()
{
	if (!m_bCreated) return;
	if (m_nViewIndex > 0)
	{
		m_nViewIndex--;
		CalcViewIndex();
		UpdateDrawRange();
	}
}

void CDXUTButtonList::CalcViewIndex()
{
	if (!m_bCreated) return;
	// 矢印ボタン不必要な場合
	if (!IsVisibleArrowButton())
	{
		m_nViewIndex = 0;
		return;
	}

	// 矢印ボタンが必要
	int nArrowRange = 0;
	int nDrawRange = 0;

	switch (m_bytStyle)
	{
	default:
	case BUTTON_LIST_HORIZON:
		nArrowRange = m_pBtnArrowL->m_width + m_pBtnArrowR->m_width;	break;
	case BUTTON_LIST_VERTICAL:
		nArrowRange = m_pBtnArrowL->m_width + m_pBtnArrowR->m_width;	break;
	}

	int nTotalRange = 0;
	BOOL bVisArrowButton = FALSE;
	int nSpace = 0;
	int nIndex=0;
	std::vector< CDXUTControl* >::iterator itIndex;
	for (std::vector< CDXUTControl* >::iterator itr = itIndex= m_vecButtons.begin();
		itr!=m_vecButtons.end();
		itr++)
	{
		if (nIndex == m_nViewIndex)
			itIndex = itr;

		if (nIndex >= m_nViewIndex)
		{
			switch (m_bytStyle)
			{
			default:
			case BUTTON_LIST_HORIZON:
				nDrawRange += (*itr)->m_width + nSpace;	break;
			case BUTTON_LIST_VERTICAL:
				nDrawRange += (*itr)->m_height + nSpace;	break;
			}
			// ボタンが表示範囲を超えたら、矢印ボタンを必要にしてループ終了
			if (nDrawRange > m_nDrawRange)
			{
				bVisArrowButton = TRUE;
				break;
			}
		}
		nSpace = m_nSpace;
		nIndex++;
	}
	
	if (!bVisArrowButton || nDrawRange+nArrowRange < m_nDrawRange)
	{
		while (itIndex != m_vecButtons.begin())
		{
			switch (m_bytStyle)
			{
			default:
			case BUTTON_LIST_HORIZON:
				nDrawRange += (*itIndex)->m_width + nSpace;	break;
			case BUTTON_LIST_VERTICAL:
				nDrawRange += (*itIndex)->m_height + nSpace;	break;
			}
			if (nDrawRange+nArrowRange > m_nDrawRange)
				break;

			m_nViewIndex--;
			itIndex--;
		}		
	}

}

void CDXUTButtonList::UpdateDrawRange()
{
	if (!m_bCreated) return;

	int nArrowRange = 0;
	int nLocateOffset = 0;
	int nDrawRange = m_nDrawRange;

	switch (m_bytStyle)
	{
	default:
	case BUTTON_LIST_HORIZON:
		nLocateOffset = m_pntLocate.x;
		nArrowRange = m_pBtnArrowL->m_width + m_pBtnArrowR->m_width+(m_nLRPadding*2);	break;
	case BUTTON_LIST_VERTICAL:
		nLocateOffset = m_pntLocate.y;
		nArrowRange = m_pBtnArrowL->m_width + m_pBtnArrowR->m_width+(+m_nLRPadding*2);	break;
	}

	// 矢印ボタンが必要か確認
	int nTotalRange = 0;

	// 矢印表示設定
	if (IsVisibleArrowButton())
	{
		m_pBtnArrowL->SetVisible(true);
		m_pBtnArrowR->SetVisible(true);
		switch (m_bytStyle)
		{
		default:
		case BUTTON_LIST_HORIZON:
			m_pBtnArrowL->SetLocation(nLocateOffset, m_pntLocate.y);
			m_pBtnArrowR->SetLocation(nLocateOffset+nDrawRange-m_pBtnArrowR->m_width-m_nSpace+m_nLRPadding*2, m_pntLocate.y);
			nDrawRange -= (m_pBtnArrowL->m_width+m_pBtnArrowR->m_width /*+ (m_nSpace*2)*/ );
			nLocateOffset += m_pBtnArrowL->m_width+m_nLRPadding;//+m_nSpace;
			break;
		case BUTTON_LIST_VERTICAL:
			m_pBtnArrowL->SetLocation(m_pntLocate.x, nLocateOffset);
			m_pBtnArrowR->SetLocation(m_pntLocate.x, nLocateOffset-m_pBtnArrowR->m_height+nDrawRange-m_nSpace+m_nLRPadding*2);
			nDrawRange -= (m_pBtnArrowL->m_height+m_pBtnArrowR->m_height /*+ (m_nSpace*2)*/ );
			nLocateOffset += m_pBtnArrowL->m_height;//+m_nSpace;
			break;
		}
	}
	else
	{
		m_pBtnArrowL->SetVisible(false);
		m_pBtnArrowR->SetVisible(false);
	}

	int nSpace = m_nSpace;
	int nIndex=-1;
	bool bVisFlg = true;
	if (!m_vecButtons.empty())
	{
		std::vector< CDXUTControl* >::iterator itr = m_vecButtons.begin();
		while (itr!=m_vecButtons.end())
		{
			int nRange = 0;

			nIndex++;
			if (nIndex < m_nViewIndex || !bVisFlg)
			{
				if ((*itr)->GetVisible())
					(*itr)->SetVisible(false);
				itr++;
				continue;
			}

			switch (m_bytStyle)
			{
			default:
			case BUTTON_LIST_HORIZON:
				if (!(*itr)->GetVisible())
					(*itr)->SetVisible(true);
				(*itr)->SetLocation(nLocateOffset, m_pntLocate.y);
				nRange = ((*itr)->m_width + nSpace);
				nDrawRange -= nRange;
				nLocateOffset += nRange;

				itr++;
				if ( itr != m_vecButtons.end()
				&& (nDrawRange+nSpace) - (*itr)->m_width < 0 )
					bVisFlg = false;
				break;
			case BUTTON_LIST_VERTICAL:
				if (!(*itr)->GetVisible())
					(*itr)->SetVisible(true);
				(*itr)->SetLocation(m_pntLocate.x, nLocateOffset);
				nRange = ((*itr)->m_height + nSpace);
				nDrawRange -= nRange;
				nLocateOffset += nRange;
				itr++;
				if ( itr != m_vecButtons.end()
				&& (nDrawRange+nSpace) - (*itr)->m_width < 0)
					bVisFlg = false;
				break;
			}
		}
	}
}

// 
void CDXUTButtonList::SetVisible(bool b)
{
	if (!m_bCreated) return;
	if (m_pBtnArrowL)
		m_pBtnArrowL->SetVisible(b);
	if (m_pBtnArrowR)
		m_pBtnArrowR->SetVisible(b);

	// 登録済みボタンの表示状態を変える
	if (!b)
	{
		for (std::vector< CDXUTControl* >::iterator itr = m_vecButtons.begin();
			itr!=m_vecButtons.end();
			itr++)
		{
			(*itr)->SetVisible(b);
		}
	}
	else
		UpdateDrawRange();
	m_bVisible = b?TRUE:FALSE;
}

void CDXUTButtonList::SetEnable(bool b)
{
	if (!m_bCreated) return;
	if (m_pBtnArrowL)
		m_pBtnArrowL->SetEnabled(b);
	if (m_pBtnArrowR)
		m_pBtnArrowR->SetEnabled(b);

	// 登録済みボタンの表示状態を変える
	for (std::vector< CDXUTControl* >::iterator itr = m_vecButtons.begin();
		itr!=m_vecButtons.end();
		itr++)
	{
		(*itr)->SetEnabled(b);
	}
	m_bEnable = b;
}
