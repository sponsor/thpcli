#ifndef H_STAGE_EFFECT_CLASS___
#define H_STAGE_EFFECT_CLASS___
// ===================================================================
// CStageEffect.h
//	ステージエフェクトクラス・ヘッダー
// ===================================================================

#include <windows.h>
#include <TCHAR.h>
#include "../include/types.h"
#include "../include/define.h"
#include "util.h"
#include <map>
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"
#include "resource.h"

#include "TextureLoader.h"
#include "LuaHelper.h"

/*
エフェクト
　・アニメーションする
　・消えてく
　・座標、拡大、縮小、回転する
　X 後ろの画面を暗くする状態を作る（カットイン用？）
*/
class CStageEffect
{
public:
	CStageEffect();
	virtual ~CStageEffect();
#ifdef HOGE
	BOOL Create(int nEffectNo, LPDIRECT3DTEXTURE9 pTexture, RECT* rcTexture, D3DXVECTOR3* pos, int nAge, BOOL force);
#else
	BOOL Create(int nEffectNo, LPDIRECT3DTEXTURE9 pTexture, RECT* rcTexture, D3DXVECTOR3* pos, int nAge);
#endif
	int	GetEffectNo() { return m_nEffectNo;	};
//	void SetEffectNo(int value) { m_nEffectNo = value;	};

	void Render(LPD3DXSPRITE pSprite, D3DXMATRIX* matWorld);
	BOOL Frame();

	void SetEffectVector(D3DXVECTOR3* vec, D3DXVECTOR3* add, int nEffectTime=0)
	{
		m_nVectorTimeCounter = 0;
		m_nVectorEffectTime = nEffectTime;
		m_vecVector = *vec;
		m_vecAddVec = *add;
	};
	void SetEffectAlpha(BYTE bytAlpha)	{ m_bytAlpha = bytAlpha;	};
	void SetEffectFade(int nFade, int nEffectTime=0)
	{
		m_nFadeTimeCounter = 0;
		m_nFadeEffectTime = nEffectTime;
		m_nFade = nFade;
	};
	void SetEffectFadeInOut(int nFadeInOutTime) {m_nFadeInOut = nFadeInOutTime;	};
	void SetEffectRotate(float fRot){ m_fRotateDraw = fRot; };
	void SetEffectRotation(float fRot, int nEffectTime=0){ m_fRotation = fRot; };
	void SetEffectScale(float fScaleX, float fScaleY)
	{
		m_vecScallingDraw.x = fScaleX;
		m_vecScallingDraw.y = fScaleY;
	};
	void SetEffectScalling(float fScaleX, float fScaleY, int nEffectTime=0)
	{
		m_nScallingTimeCounter = 0;
		m_nScallingEffectTime = nEffectTime;
		m_vecScalling.x = fScaleX;
		m_vecScalling.y = fScaleY;
	};
	inline float safeFloatAdd(float fa, float fb, float fdef)
	{
		float fres = fa + fb;
		if (!_finite(fres)) { return fdef;	}
		return fres;
	};
	void SetEffectAnimation(int nATime, int nACount, BOOL bLoop, int nEffectTime=0)
	{
		m_nAnimationEffectTime = nEffectTime;
		m_nAnimationIndex = 0;
		m_nAnimationTimeCounter = 0;
		m_nAnimationTime = nATime;
		m_nAnimationCount = nACount;
		m_bAnimationLoop = bLoop;
	};
	void SetEffectTexture(RECT* rcTexture);

	int GetAgeCount(){ return m_nAgeCounter;	};

protected:
	int						m_nAge;
	int						m_nAgeCounter;
	BOOL				m_bReady;
	int						m_nEffectNo;			// 管理番号

	LPDIRECT3DTEXTURE9	p_pTexture;	// 使用するテクスチャ
	RECT					m_rcTexture;				// 描画サイズ
	D3DXVECTOR3	m_vecPosition;			// 位置
	D3DXVECTOR3	m_vecVector;			// 移動値
	D3DXVECTOR3	m_vecAddVec;			// 加速値

	D3DXVECTOR3	m_vecScalling;			// 拡大縮小

	int						m_nAnimationTime;	// 次アニメーションインデックスへ移る時間
	int						m_nAnimationTimeCounter;	// 時間のカウンタ

	int						m_nAnimationCount;	// アニメーション数
	int						m_nAnimationIndex;	// アニメーションインデックス
	BOOL				m_bAnimationLoop;

	int						m_nFade;					// フェード
	int						m_nFadeInOut;			// フェードインアウト時間
	
	RECT					m_rcDraw;				// 描画範囲
	D3DXVECTOR3	m_vecCenter;			// 回転の中心
	D3DCOLOR		m_clrDraw;				// 描画色
	BYTE					m_bytAlpha;				// アルファ
	FLOAT				m_fRotateDraw;		// 回転値
	FLOAT				m_fRotation;				// 自転値

	D3DXVECTOR3	m_vecScallingDraw;	// 拡大縮小
	
	int						m_nFadeTimeCounter;
	int						m_nVectorTimeCounter;
	int						m_nScallingTimeCounter;
	int						m_nRotationTimeCounter;
	int						m_nRotationEffectTime;
	int						m_nFadeEffectTime;			// フェード効果時間
	int						m_nAnimationEffectTime;	// アニメーション効果時間
	int						m_nScallingEffectTime;		// 拡大縮小効果時間
	int						m_nVectorEffectTime;		// 移動効果時間
#ifdef HOGE
	BOOL					m_bForce;
#endif
};

#endif