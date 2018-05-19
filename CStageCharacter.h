#ifndef H_STAGE_CHARACTER_CLASS___
#define H_STAGE_CHARACTER_CLASS___
// ===================================================================
// CStageCharacter.h
//	ステージにいるキャラクラス・ヘッダー
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

// キャラが表示できる
// HPのバーが表示できる
// キャラ名が表示できる
// ステージを移動できる
// 移動する時アニメーションする
// 弾が撃てる
// アイテムが使える

// ステージのキャラクラス
class CStageCharacter
{
public:
	CStageCharacter();
	virtual ~CStageCharacter() 
	{ 
		if (m_bCreated)
			Destroy();
	};

	// 生成
	// pDev			: D3Dデバイス
	// pTexture	: テクスチャ
	// nGUIResourceIndex : 基本のGUIリソース番号
	// pStatic		: スタティック(キャラ名用)
	// pSess		: オブジェクト情報
	HRESULT Create(std::map < int, TCHARA_SCR_INFO > *mapScrInfo, int nGUIResourceIndex, CDXUTDialog* pDialog, type_session* pSess, int nStageWidth, int nStageHeight);

	// 更新
	// pTexture	: テクスチャ
	// nGUIResourceIndex : 基本のGUIリソース番号
	// nSpriteWidth,nSpriteHeight : 表示サイズ
	// nAnimationCount : ｱﾆﾒｰｼｮﾝ数
//	HRESULT Update(LPDIRECT3DTEXTURE9 pTexture, int nGUIResourceIndex, int nSpriteWidth, int nSpriteHeight, int nAnimationCount);

	// 状態更新
//	void UpdateState();
	// 破棄
	void Destroy();
	// 毎フレーム
	void Frame();

	BOOL IsCreated()
	{ return m_bCreated;	};

	// 死んだ
	void SetDead();

	void Update()
	{
		if (m_bCreated)
		{
			CalcDrawPos();
			CalcDrawRect();
			UpdateState();
		}
	}
	void CalcDrawRect();
	void CalcDrawPos();
	// 状態更新
	void UpdateState();
	// ターン状態セット
	void SetMyTurn(bool bTurn);

	void ChangeHP(int value);

	// 描画
	// pDev	: D3Dデバイス
	// fElapsedTime : 経過時間
	void Render(LPDIRECT3DDEVICE9 pDev, float fElapsedTime, D3DXMATRIX* matChara, D3DXMATRIX* matStage, D3DCOLOR color=0xFFFFFFFF);
	
	void SetVisible(bool bVisible);
	bool GetVisible()
	{	return m_bVisible;			};
	void SetVisibleInfo(bool bVisible){	m_bVisibleInfo = bVisible;	};
	bool GetVisibleInfo(){	return m_bVisibleInfo;	};

	void SetTurn(bool bTurn)
	{	m_pMyTurn->SetVisible(bTurn);	};

	void SetTexture(RECT* prcTexture);

	// OnLostDeviceEvent
	void OnLost();
	// OnResetDeviceEvent
	void OnReset();

	ptype_session	GetSession();

protected:
	type_session*	p_pSession;					// オブジェクト情報
	
	int m_nAnimationIndex;							// ｱﾆﾒｰｼｮﾝ描画範囲インデックス
	int m_nAnimationCount;							// ｱﾆﾒｰｼｮﾝ描画範囲数

	int m_nAnimationTimeCounter;				// ｱﾆﾒｰｼｮﾝｲﾝﾃﾞｯｸｽの数値を増やす時間
	int m_nAnimationTime;							// ｱﾆﾒｰｼｮﾝｲﾝﾃﾞｯｸｽの数値を増やす時間
	
	int m_nTextureWidth;							// テクスチャ横幅
	int m_nTextureHeight;							//	テクスチャ縦幅
	LPDIRECT3DTEXTURE9 p_pTexture;		// テクスチャ

	int m_nCharaWidth;
	int m_nCharaHeight;
	D3DXMATRIXA16	m_matScale;				// 表示サイズへのスケーリングマトリックス
	float m_fAdjustPosY;

	CDXUTDialog*	p_pDialog;
	CDXUTStatic*	m_pControlName;			// 名前表示用コントロール
	CDXUTMeter*	m_pHPBar;					// HPバー
	CDXUTSprite*	m_pMyTurn;					// ターン表示
	CDXUTStatic*	m_pStatusInfo;				// ダメージ表示
	int	m_nStatuInfoTime;

	int m_nControlNameWidth;					// 
	int m_nControlNameHeight;
	int m_nGUIResourceIndex;						// リソース番号

	BOOL m_bCreated;								// 生成済み
	TCHARA_SCR_INFO*  m_pScrInfo;			// スクリプト情報

	D3DXMATRIX		m_matDrawCharaRadian;
	D3DXVECTOR3	m_vecDrawCharaCenter;	// 描画位置
	D3DXVECTOR3	m_vecDrawCharaPos;	// 描画位置
	RECT m_recDraw;									// 描画範囲	
	bool m_bVisible;									// 表示状態
	bool m_bVisibleInfo;								// キャラの情報表示
	int m_nBodyRange;								// 当たり判定用半径
	BOOL m_bMyTurn;
	void UpdateMyTurnTexture();

	int m_nStageWidth;
	int m_nStageHeight;

};

#endif
