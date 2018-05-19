#include "ext.h"
#include "Game.h"

// ========================================================================= //
// CGame::Render >
// ========================================================================= //

// ------------------------------------------------------------------------- //
// Render
// ------------------------------------------------------------------------- //
void CGame::RenderTitle(float fElapsedTime )
{
	HRESULT hr;
	// 背景
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'8');
	if (m_pBGTexture
	&& SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	{
		g_pSprite->Draw(m_pBGTexture, NULL, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), 0xFFFFFFFF);
		g_pSprite->End();
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	V( p_pUI->OnRender( fElapsedTime ) );
	V( p_pConfig->OnRender( fElapsedTime ) );
}

// ------------------------------------------------------------------------- //
// Login
// ------------------------------------------------------------------------- //
void CGame::RenderLogin(float fElapsedTime )
{
	HRESULT hr;
	// 背景
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'8');
	if (m_pBGTexture
	&& SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	{
		g_pSprite->Draw(m_pBGTexture, NULL, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), 0xFFFFFFFF);
		g_pSprite->End();
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();

	V( p_pUI->OnRender( fElapsedTime ) );
	V( p_pConfig->OnRender( fElapsedTime ) );
}

// ------------------------------------------------------------------------- //
// Room
// ------------------------------------------------------------------------- //
void CGame::RenderRoom(float fElapsedTime )
{
	HRESULT hr;
	// 背景
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'9');
	if (m_pBGTexture
//	&& SUCCEEDED(g_DialogResourceManager.m_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	&& SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	{
		g_pSprite->Draw(m_pBGTexture, NULL, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1.0f), 0xFFFFFFFF);
		g_pSprite->End();
	}
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
		V( p_pUI->OnRender( fElapsedTime ) );

	// config表示中はキャラ表示しない
	if (p_pConfig->GetVisible())
	{
		V( p_pConfig->OnRender( fElapsedTime ) );
		return;
	}
	if (SUCCEEDED(g_DialogResourceManager.m_pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
	{
		m_pTeamSeparater->Render();
		if (SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE ) ))
		{
			for (int i=0;i<GetMaxLoginNum();i++)
				m_pRoomCharacters[i]->Render(m_pDev, fElapsedTime);
			g_pSprite->End();
		}
		g_DialogResourceManager.m_pSprite->End();
	}
}

// ------------------------------------------------------------------------- //
// Load
// ------------------------------------------------------------------------- //
void CGame::RenderLoad(float fElapsedTime )
{
	HRESULT hr;
	
	m_nLoadingTextureCounter+=4;

	D3DXMATRIX   matWorld, matTransA, matTransB, matRot, matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	D3DXMatrixTranslation(&matTransA, 16, 16, 0.0f);
	D3DXMatrixTranslation(&matTransB,  LOAD_SPRITE_ROT_CNT_X,  LOAD_SPRITE_ROT_CNT_Y, 0.0f);
	D3DXMatrixRotationZ(&matRot, D3DXToRadian(m_nLoadingTextureCounter%360));
	
//	D3DXVECTOR3 pos = D3DXVECTOR3(LOAD_SPRITE_ROT_CNT_X,LOAD_SPRITE_ROT_CNT_Y,0.0f);
	D3DXVECTOR3 pos = D3DXVECTOR3(0,0,0.0f);


	RECT rcTexture;
	SetRect(&rcTexture, LOAD_SPRITE_ROT_IMG_RECT);

	matWorld = matTransA*matRot*matTransB;
	if (SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	{
		g_pSprite->SetTransform(&matIdentity);
		// 背景
		if (m_pBGTexture)
			g_pSprite->Draw(m_pBGTexture, NULL, &pos, &pos, 0xFFFFFFFF);
		g_pSprite->SetTransform(&matWorld);
		// Loading...
		g_pSprite->Draw(m_pDefaultGUIResourceTexture, &rcTexture, &D3DXVECTOR3(LOAD_SPRITE_ROT_CNT_W,LOAD_SPRITE_ROT_CNT_H,0),&pos, 0xFFFFFFFF);
		g_pSprite->End();
	}

	V( p_pUI->OnRender( fElapsedTime ) );
//	V( p_pConfig->OnRender( fElapsedTime ) );
}

// ------------------------------------------------------------------------- //
// Main
// ------------------------------------------------------------------------- //
void CGame::RenderMain(float fElapsedTime )
{
	if (!m_bMainInitFlg)	return;
	HRESULT hr;

	D3DXMATRIX   matIdentity, matWorld;
	D3DXMatrixIdentity(&matIdentity);

	if (m_nActiveCharaFocusCounter)
	{
		m_nActiveCharaFocusCounter--;
		ptype_session act_sess = GetSessionObj(m_nActiveObjectNo);
		if (act_sess)
		{
			float fVec = (float)(CHARA_FOCUS_TIME-m_nActiveCharaFocusCounter) / (float)CHARA_FOCUS_TIME;
			m_vecStageViewTrans = ((D3DXVECTOR3((float)(act_sess->ax-WIN_WIDTH/2), (float)(act_sess->ay-WIN_HEIGHT/2),0 )-m_vecActiveCharaFocusStartPos)*fVec)+m_vecActiveCharaFocusStartPos;
			CalcViewTransCaps();
//			m_nSavedMouseX = (int)m_vecStageViewTrans.x;
//			m_nSavedMouseY = (int)m_vecStageViewTrans.y;
			UpdateStageDrawRect();
			if (!m_nActiveCharaFocusCounter)
				m_vecActiveCharaFocusStartPos = m_vecStageViewTrans;
		}
	}
	else if (m_nMyCharaFocusCounter)
	{
		m_nMyCharaFocusCounter--;
		float fVec = (float)(CHARA_FOCUS_TIME-m_nMyCharaFocusCounter) / (float)CHARA_FOCUS_TIME;
		m_vecStageViewTrans = ((D3DXVECTOR3((float)(GetMySessionInfo()->ax-WIN_WIDTH/2), (float)(GetMySessionInfo()->ay-WIN_HEIGHT/2),0 )-m_vecMyCharaFocusStartPos)*fVec)+m_vecMyCharaFocusStartPos;
		CalcViewTransCaps();
		UpdateStageDrawRect();
		if (!m_vecMyCharaFocusStartPos)
			m_vecMyCharaFocusStartPos = m_vecStageViewTrans;
	}
	else if (m_bBulletFocus && !m_bPauseBulletFocus && !m_bScrFocus && m_pFocusObject && (m_pFocusObject->obj_state & OBJ_STATE_MAIN_ACTIVE_FLG) )
	{
		m_vecStageViewTrans = D3DXVECTOR3((float)(m_pFocusObject->ax-WIN_WIDTH/2), (float)(m_pFocusObject->ay-WIN_HEIGHT/2),0 );

		CalcViewTransCaps();
//		m_nSavedMouseX = (int)m_vecStageViewTrans.x;
//		m_nSavedMouseY = (int)m_vecStageViewTrans.y;
		UpdateStageDrawRect();
	}
	D3DXMatrixTranslation(&matWorld, -m_vecStageViewTrans.x, -m_vecStageViewTrans.y, m_vecStageViewTrans.z);

	if (!g_pSprite)	return;

	m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	RECT rcStage = DXUTGetWindowClientRect();
	rcStage.bottom -= MAIN_CONTROLPANEL_CNT_H_MIN;
	if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_REVERSE_INDEX])
		rcStage.bottom += MAIN_CONTROLPANEL_CNT_H_MIN-MAIN_CONTROLPANEL_CNT_HIDE;

	OffsetRect(&rcStage, (int)m_vecStageViewTrans.x, (int)m_vecStageViewTrans.y);

	if (SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	{
		D3DXMATRIX matRev;
		g_pCriticalSection->EnterCriticalSection_Session(L'5');
		GetReverseStateMatrix(&matRev, &rcStage);
		matWorld *= matRev;
		g_pSprite->SetTransform(&matRev);

		if (!m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_BLIND_INDEX])
		{
			DWORD dwStageColor = GetStageColor();
			// ステージ描画
			// 背景
			g_pSprite->Draw(m_pStageBGTexture, &m_rcDrawBGStage, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), dwStageColor);

			g_pCriticalSection->EnterCriticalSection_Effect(L'2');
			// 背景エフェクト描画
			for ( std::map< int, CStageEffect* >::iterator it = m_mapBGEffects.begin();
				it != m_mapBGEffects.end();
				it++)
			{
				(*it).second->Render(g_pSprite, &matRev);
			}
			g_pCriticalSection->LeaveCriticalSection_Effect();

			g_pSprite->SetTransform(&matRev);
			// ステージ
			g_pCriticalSection->EnterCriticalSection_StageTexture(L'0');
			g_pSprite->Draw(m_pStageTexture, &m_rcDrawStage, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), dwStageColor);
			g_pCriticalSection->LeaveCriticalSection_StageTexture();
			// キャラ描画
			p_pUI->GetManager()->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

//			m_nTest = m_SessionArray[0].frame_count;
			int nTeamNo = m_SessionArray[m_nUserIndex].team_no;
			// 死体を先に描画
			for (int i=0;i<GetMaxLoginNum();i++)
			{
				if (m_SessionArray[i].entity)
				{
					if (!(m_SessionArray[i].obj_state & OBJ_STATE_MAIN_NOLIVE_FLG))	continue;
					m_pStageCharacters[i]->Render(m_pDev, fElapsedTime, &matWorld, &matWorld);
				}
			}
			int nActiveIndex = GetSessionIndex(m_nActiveObjectNo);
			// 生きてるキャラを後から描画
			for (int i=0;i<GetMaxLoginNum();i++)
			{
				if (m_SessionArray[i].entity)
				{
					if (m_SessionArray[i].obj_state & OBJ_STATE_MAIN_NOLIVE_FLG)	continue;
#if (E0401 == 0)
					if (i == nActiveIndex) continue;	// アクティブキャラは描画を後にする
#endif
					// ステルス状態
					if (m_SessionArray[i].chara_state[CHARA_STATE_STEALTH_INDEX] > 0)
					{
						// 同チームか観戦なら半透明描画
						if (nTeamNo == GALLERY_TEAM_NO || m_SessionArray[i].team_no == nTeamNo)
							m_pStageCharacters[i]->Render(m_pDev, fElapsedTime, &matWorld, &matWorld, 0x7FFFFFFF);
					}
					else
						m_pStageCharacters[i]->Render(m_pDev, fElapsedTime, &matWorld, &matWorld);
				}
			}
#if (E0401==0)
			if (ptype_session act_sess = GetAuthedSessionInfo(nActiveIndex))
			{
				if (act_sess->chara_state[CHARA_STATE_STEALTH_INDEX] > 0)
				{
					// 同チームなら半透明描画
					if (nTeamNo == GALLERY_TEAM_NO || act_sess->team_no == nTeamNo)
						m_pStageCharacters[nActiveIndex]->Render(m_pDev, fElapsedTime, &matWorld, &matWorld, 0x7FFFFFFF);
				}
				else
					m_pStageCharacters[nActiveIndex]->Render(m_pDev, fElapsedTime, &matWorld, &matWorld);
			}
#endif
			p_pUI->GetManager()->m_pSprite->End();
		}
		else
		{
			// 周りが暗い時の描画
			RenderMainBlindState(fElapsedTime, &matWorld);
		}

		// 自分がアクティブなら発射角度表示させる
		if (GetMySessionInfo()->obj_state & OBJ_STATE_ACT_FLG)
		{
			ptype_session mysess = &m_SessionArray[m_nUserIndex];
			// 体の中心より高めで発射させる
			short sHeadAngle = 90;
			if (mysess->dir != USER_DIRECTION_LEFT)
				sHeadAngle = 270;
			double dRad = D3DXToRadian( (mysess->angle+sHeadAngle)%360);

			float fBodyOffsetX = (float)(cos(dRad) * (((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h));
			float fBodyOffsetY = (float)(sin(dRad) * (((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h));

			D3DXMATRIX matAngle, matTrans1,matTrans2;
			D3DXMatrixTranslation(&matTrans1, (float)-mysess->ax+fBodyOffsetX,(float)-mysess->ay+fBodyOffsetY,0.0f );
			D3DXMatrixRotationZ(&matAngle, D3DXToRadian(m_pShotAngle->GetShotAngle()));
			D3DXMatrixTranslation(&matTrans2, (float)mysess->ax-fBodyOffsetX,(float)mysess->ay-fBodyOffsetY,0.0f );

			g_pSprite->SetTransform(&(matTrans1*matAngle*matTrans2*matWorld));
			RECT rcAngleTexture = {MAIN_SPRITE_CHARA_ANGLE_IMG_RECT};
			D3DXVECTOR3 pos = D3DXVECTOR3((float)mysess->ax-fBodyOffsetX, (float)mysess->ay-fBodyOffsetY,0.0f);
			g_pSprite->Draw(m_pDefaultGUIResourceTexture, &rcAngleTexture, &D3DXVECTOR3(-16.0f,9.0f,0), &pos, 0xFFFFFFFF);
			g_pSprite->SetTransform(&matWorld);
		}
#ifdef HOGE
#if 1
	static int a =0;
	a++;
	if (a%5 == 0)
	{
		ptype_session mysess = &m_SessionArray[m_nUserIndex];
		if (mysess->team_no	!= GALLERY_TEAM_NO && mysess->HP_c > 0)
		{

			int nWind = 1;
			if (mysess->scrinfo->ID == 138 && m_nSelectBulletNo == 1){
				nWind = -1;
			}
			RECT rcAngleTexture;
			SetRect(&rcAngleTexture,2,26,4,28);
			
			int angle;
			if (mysess->dir != USER_DIRECTION_LEFT)
			{
				angle = (360+((360-m_pShotAngle->GetAngle()) - 180 + (mysess->angle-180)))%360;
			}else{
				angle = (360+(m_pShotAngle->GetAngle()/* - 180*/ + mysess->angle))%360;
			}
			double dRad = D3DXToRadian( (angle)%360);
			D3DXVECTOR3 v, av;
			v.z = 0.0f;
			av = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			av.y = 20.0f * BLT_VEC_FACT_F* BLT_VEC_FACT_F;
			av.x = (float)(m_nWindValue * nWind) * BLT_VEC_FACT_F* BLT_VEC_FACT_F;
			float power = (float)(m_pShotPowerIndicator->GetPosX()-126) / 320.0f * (float)m_pShotMeter->GetMaxValue();
			power = (int)power+2;
			double dx = cos(dRad);
			double dy = sin(dRad);
			int nFirstRange = CHARA_HIT_RANGE+CHARA_BODY_RANGE+1;

			short sHeadAngle = 90;
			if (mysess->dir != USER_DIRECTION_LEFT)
				sHeadAngle = 270;
			double dPosRad = D3DXToRadian( (mysess->angle+sHeadAngle)%360);
			double dBodyOffsetX = cos(dPosRad) * ((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h;
			double dBodyOffsetY = sin(dPosRad) * ((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h;
	//		double dBodyOffsetX = cos(dRad) * ((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h;
	//		double dBodyOffsetY = sin(dRad) * ((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h;
			D3DXVECTOR3 pos;
			pos.z = 0.0f;
			pos.x = (float)mysess->ax + dx*(double)nFirstRange - dBodyOffsetX;
			pos.y = (float)mysess->ay + dy*(double)nFirstRange - dBodyOffsetY;;
			int fx = AddEffect(DEF_SYSTEM_ID, &rcAngleTexture, &pos, 300, TRUE, TRUE);
			CStageEffect* pfx = FindEffectFromEffectNo(fx, TRUE);

			v.x = (cos(dRad) * power * BLT_VEC_FACT_F) + av.x;
			v.y = (sin(dRad) * power * BLT_VEC_FACT_F) + av.y;
			
			pfx->SetEffectVector(&(v), &(av));
			g_pSprite->SetTransform(&matWorld);

			//static int b=0;
			//b++;
			//if (b%3 == 0){
			//	WCHAR msg[32];
			//	SafePrintf(msg, 32, L"sht:%d/pos:%d - %d\n", angle, mysess->angle+sHeadAngle , power);
			//	OutputDebugStr(msg);
			//}
		}
	}
#else
		{
			ptype_session mysess = &m_SessionArray[m_nUserIndex];
			RECT rcAngleTexture;
			D3DXVECTOR3 vecCenter;
			
			bool bDraw = false;
			int angle;
			if (mysess->dir != USER_DIRECTION_LEFT)
			{
				angle = (360+(m_pShotAngle->GetAngle() - 180 - (mysess->angle-180)))%360;
			}else{
				angle = (360+(m_pShotAngle->GetAngle() - 180 + mysess->angle))%360;
			}
			static int a = 0;
			if ((++a % 30) == 0){
				WCHAR msg[64];
				SafePrintf(msg, 64, L"res:%d, angle:%d, chara:%d\n", angle, m_pShotAngle->GetAngle(), GetMySessionInfo()->angle);
				OutputDebugStr(msg);
			}
			int reg = angle ? (angle / 10) : 0;
			if ((reg) == 4 || (reg) == 22
			|| (reg) == 13)
			{
				int dir = mysess->dir;
				if ((reg) == 13)
					dir = (dir+1)%USER_DIRECTION_MAX;
				switch (m_nWindValue)
				{
				case 7: case 6: case 5: case 4: case 3:
					if (dir == USER_DIRECTION_LEFT)
					{
						vecCenter = D3DXVECTOR3(1013.0f,327.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_45_L_R5_RECT);
					}
					else
					{
						vecCenter = D3DXVECTOR3(24.5,339.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_45_R_R5_RECT);
					}
					bDraw = true;
					break;
				case 2: case 1: case 0: case -1: case -2:
					if (dir == USER_DIRECTION_LEFT)
					{
						vecCenter = D3DXVECTOR3(1014.0f,336.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_45_L_R0_RECT);
					}
					else
					{
						vecCenter = D3DXVECTOR3(24.5,336.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_45_R_R0_RECT);
					}
					bDraw = true;
					break;
				case -3: case -4: case -5: case -6: case -7:
					if (dir == USER_DIRECTION_LEFT)
					{
						vecCenter = D3DXVECTOR3(1025.0f,339.0f,0);
						SetRect(&rcAngleTexture, HOGE_ANGLE_45_L_L5_RECT);
					}
					else
					{
						vecCenter = D3DXVECTOR3(24.5f,327.0f,0);
						SetRect(&rcAngleTexture, HOGE_ANGLE_45_R_L5_RECT);
					}
					bDraw = true;
					break;
				}
			}else if ((reg) == 6 || (reg) == 26
			|| (reg) == 11)
			{
				int dir = mysess->dir;
				if ((reg) == 11)
					dir = (dir+1)%USER_DIRECTION_MAX;
				switch (m_nWindValue)
				{
				case 7: case 6: case 5: case 4: case 3:
					if (dir == USER_DIRECTION_LEFT)
					{
						vecCenter = D3DXVECTOR3(435.0f,483.0f,0.0f);	//
						SetRect(&rcAngleTexture, HOGE_ANGLE_66_L_R5_RECT);
					}
					else
					{
						vecCenter = D3DXVECTOR3(25,475.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_66_R_R5_RECT);
					}
					bDraw = true;
					break;
				case 2: case 1: case 0: case -1: case -2:
					if (dir == USER_DIRECTION_LEFT)
					{
						vecCenter = D3DXVECTOR3(1034.0f,492.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_66_L_R0_RECT);
					}
					else
					{
						vecCenter = D3DXVECTOR3(24,492.0f,0.0f);
						SetRect(&rcAngleTexture, HOGE_ANGLE_66_R_R0_RECT);
					}
					bDraw = true;
					break;
				case -3: case -4: case -5: case -6: case -7:
					if (dir == USER_DIRECTION_LEFT)
					{
						vecCenter = D3DXVECTOR3(1074.0f,475.0f,0);
						SetRect(&rcAngleTexture, HOGE_ANGLE_66_L_L5_RECT);
					}
					else
					{
						vecCenter = D3DXVECTOR3(60.0f,483.0f,0);
						SetRect(&rcAngleTexture, HOGE_ANGLE_66_R_L5_RECT);
					}
					bDraw = true;
					break;
				}
			}

//			else if ((mysess->angle / 10) == 6)
			if (bDraw)
			{
				short sHeadAngle = 90;
				if (mysess->dir != USER_DIRECTION_LEFT)
					sHeadAngle = 270;
				double dRad = D3DXToRadian( (mysess->angle+sHeadAngle)%360);
				float fBodyOffsetX = (float)(cos(dRad) * (((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h));
				float fBodyOffsetY = (float)(sin(dRad) * (((TCHARA_SCR_INFO*)mysess->scrinfo)->shot_h));

				g_pSprite->SetTransform(&(matWorld));
				
				D3DXVECTOR3 pos = D3DXVECTOR3((float)mysess->ax-fBodyOffsetX, (float)mysess->ay-fBodyOffsetY,0.0f);
				g_pSprite->Draw(m_pDefaultGUIResourceTexture, &rcAngleTexture, &vecCenter, &pos, 0xFFFFFFFF);
				g_pSprite->SetTransform(&matWorld);
			}
		}
#endif
#endif
		g_pCriticalSection->LeaveCriticalSection_Session();

		// エフェクト描画
		g_pCriticalSection->EnterCriticalSection_Effect(L'3');
		for ( std::map< int, CStageEffect* >::iterator it = m_mapStageEffects.begin();
			it != m_mapStageEffects.end();
			it++)
		{
			(*it).second->Render(g_pSprite, &matWorld);
		}
		g_pCriticalSection->LeaveCriticalSection_Effect();

		// オブジェクト(弾とか)描画
		g_pCriticalSection->EnterCriticalSection_Object(L'1');
		std::vector< int > vecFrame;
		vecFrame.assign(m_vecObjectNo.begin(), m_vecObjectNo.end());
		for ( std::vector<int>::iterator itno = vecFrame.begin();
			itno != vecFrame.end();
			itno++)
		{
			std::map<int ,type_obj*>::iterator it = m_mapObjects.find( (*itno));
			if (it == m_mapObjects.end())	continue;
			// 削除フラグ立っているか
			if ((*it).second->proc_flg & PROC_FLG_OBJ_REMOVE)	continue;
			RenderObject( (*it).second, &matWorld );
		}

		g_pSprite->End();
		g_pCriticalSection->LeaveCriticalSection_Object();
	}
	V( p_pUI->OnRender( fElapsedTime ) );
	V( p_pConfig->OnRender( fElapsedTime ) );

}

void CGame::GetReverseStateMatrix(D3DXMATRIX* matRev, RECT* rcWindow)
{
	D3DXMatrixIdentity(matRev);
#ifdef HOGE
	return;
#endif
	// 逆さ状態確認
	if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_REVERSE_INDEX])
	{
		D3DXMATRIX mat, matR, matT, matT2;
		// 傾きD3DXMATRIX matR, matT, matT2;
		// 変換行列初期化
		::D3DXMatrixIdentity(&mat);
		// 1. 一度スプライトの中心点をウィンドウの原点(0,0)にあわせる
		::D3DXMatrixTranslation(&matT, -(rcWindow->right-rcWindow->left)/2.0f, -(rcWindow->bottom-rcWindow->top)/2.0f, 0.0f);
		// 2. スプライトを回転させる
		::D3DXMatrixRotationZ(&matR, D3DXToRadian(180));
		// 3. 1でずらした分を元に戻す
		::D3DXMatrixTranslation(&matT2, (rcWindow->right-rcWindow->left)/2.0f, (rcWindow->bottom-rcWindow->top)/2.0f/*-MAIN_CONTROLPANEL_CNT_HIDE*/, 0.0f);
		// 行列の合成
		*matRev = matT * matR * matT2;
	}
}

void CGame::RenderMainBlindState(float fElapsedTime, D3DXMATRIX* matWorld)
{
//	HRESULT hr;
	D3DXMATRIX   matIdentity, matTrans, matChara;
	D3DXMatrixIdentity(&matIdentity);
	D3DXMatrixTranslation(&matTrans, m_vecStageViewTrans.x, m_vecStageViewTrans.y, m_vecStageViewTrans.z);
	matChara = *matWorld;
	*matWorld *= matTrans;

	D3DXVECTOR3 pos;
	pos.x = m_SessionArray[m_nUserIndex].ax-m_vecStageViewTrans.x;
	pos.y = m_SessionArray[m_nUserIndex].ay-m_vecStageViewTrans.y;
	pos.z = 0.0f;
	
	RECT rcWindow;// = DXUTGetWindowClientRect();
	rcWindow.left = m_SessionArray[m_nUserIndex].ax-GAME_BLIND_VISIBLE_RANGE_HALF;
	rcWindow.top = m_SessionArray[m_nUserIndex].ay-GAME_BLIND_VISIBLE_RANGE_HALF;
	rcWindow.right = rcWindow.left+GAME_BLIND_VISIBLE_RANGE;
	rcWindow.bottom = rcWindow.top+GAME_BLIND_VISIBLE_RANGE;
	// ステージ描画
	DWORD dwStageColor = GetStageColor();
	// 背景
	g_pSprite->Draw(m_pBGTexture, &rcWindow, &D3DXVECTOR3(GAME_BLIND_VISIBLE_RANGE_HALF,GAME_BLIND_VISIBLE_RANGE_HALF,0), &pos, dwStageColor);

	// ステージ
	g_pCriticalSection->EnterCriticalSection_StageTexture(L'-');
	g_pSprite->Draw(m_pStageTexture, &rcWindow, &D3DXVECTOR3(GAME_BLIND_VISIBLE_RANGE_HALF,GAME_BLIND_VISIBLE_RANGE_HALF,0), &pos, dwStageColor);
	g_pCriticalSection->LeaveCriticalSection_StageTexture();
	// キャラ
	DWORD dwCharacterColor = 0xFFFFFFFF;
	if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_STEALTH_INDEX] > 0)	// 半透明化
		dwCharacterColor = 0x7FFFFFFF;
	g_pCriticalSection->EnterCriticalSection_Session(L'6');
	p_pUI->GetManager()->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_pStageCharacters[m_nUserIndex]->Render(m_pDev, fElapsedTime, &matChara, matWorld, dwCharacterColor);
	p_pUI->GetManager()->m_pSprite->End();

	g_pCriticalSection->LeaveCriticalSection_Session();

	RECT rcTexture;
	SetRect(&rcTexture, TEXTURE_BLT_BLINDLIGHT_RECT);

	if (m_SessionArray[m_nUserIndex].chara_state[CHARA_STATE_REVERSE_INDEX])
	{
		pos.x = m_SessionArray[m_nUserIndex].ax+m_vecStageViewTrans.x;
		pos.y = m_SessionArray[m_nUserIndex].ay+m_vecStageViewTrans.y;
	}
	// 黒いやつ
	g_pSprite->Draw(m_pDefaultGUIResourceTexture, &rcTexture, &D3DXVECTOR3(GAME_BLIND_VISIBLE_RANGE_HALF,GAME_BLIND_VISIBLE_RANGE_HALF,0),&pos, 0xFFFFFFFF);
	// オブジェクトはキャラと同じマトリックスを使用
	*matWorld = matChara;
}

// ------------------------------------------------------------------------- //
// Result
// ------------------------------------------------------------------------- //
void CGame::RenderResult(float fElapsedTime )
{
	HRESULT hr;
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pSprite->SetTransform(&matWorld);
	// ステージ描画
	// 背景
	if (SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND ) ))
	{
		g_pSprite->Draw(m_pStageBGTexture, &m_rcDrawBGStage, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), 0xFFAAAAAA);
		// ステージ
		g_pCriticalSection->EnterCriticalSection_StageTexture(L'^');
		g_pSprite->Draw(m_pStageTexture, &m_rcDrawStage, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), 0xFFAAAAAA);
		g_pCriticalSection->LeaveCriticalSection_StageTexture();
		g_pSprite->End();
	}
	V( p_pUI->OnRender( fElapsedTime ) );
	V( p_pConfig->OnRender( fElapsedTime ) );
}

