#ifndef H_ROOM_CHARACTER_CLASS___
#define H_ROOM_CHARACTER_CLASS___
// ===================================================================
// CRoomCharacter.h
//	部屋にいるキャラクラス・ヘッダー
// ===================================================================

#include <windows.h>
#include <TCHAR.h>
#include "../include/types.h"
#include "../include/define.h"
#include "util.h"
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <map>
#include <iostream>
#include <vector>

#include "../lib/DXUT/dxstdafx.h"
#include "../lib/DXUT/DXUT.h"

#include "TextureLoader.h"

// 部屋を移動
// キャラを変えられる
// アニメーションする
// 発言が頭の上にでる
// 準備OKとき移動できなくする

// 部屋のキャラクラス
class CRoomCharacter
{
public:
	CRoomCharacter();					// 
	virtual ~CRoomCharacter() 
	{ 
		if (m_bCreated)
			Destroy();
	};

	// 生成
	// pDev			: D3Dデバイス
	// pTexture	: テクスチャ
	// nGUIResourceIndex : 基本のGUIリソース番号
	// pStatic		: スタティック
	// pSess		: オブジェクト情報
	// nSpriteWidth,nSpriteHeight : テクスチャサイズ
	// nCharaWidth,nCharaHeight : 表示サイズ
	// nAnimationCount : ｱﾆﾒｰｼｮﾝ数
	BOOL Create(LPDIRECT3DTEXTURE9 pTexture, int nGUIResourceIndex, CDXUTDialog* pDialog, type_session* pSess, int nSpriteWidth, int nSpriteHeight, int nAnimationCount, int nCharaWidth, int nCharaHeight);
	BOOL Create(LPDIRECT3DTEXTURE9 pTexture, int nGUIResourceIndex,  CDXUTDialog* pDialog, type_session* pSess, int nSpriteWidth, int nSpriteHeight, int nCharaLeft, int nCharaTop, int nCharaWidth, int nCharaHeight);

	// テクスチャ更新
	// pTexture	: テクスチャ
	// nSpriteWidth,nSpriteHeight : 表示サイズ
	// nAnimationCount : ｱﾆﾒｰｼｮﾝ数
	BOOL UpdateTexture(LPDIRECT3DTEXTURE9 pTexture, int nSpriteWidth, int nSpriteHeight, int nAnimationCount, int nCharaWidth, int nCharaHeight);

	// テクスチャ更新
	BOOL UpdateTexture(LPDIRECT3DTEXTURE9 pTexture, int nSpriteWidth, int nSpriteHeight, int nLeft, int nTop, int nRight, int nBottom);

	// 状態更新
	void UpdateState();

	void Destroy();

	void Frame();
	// 発言
	void Say(WCHAR *message);

	BOOL IsCreated()
	{ return m_bCreated;	};
	
	// 描画
	// pDev	: D3Dデバイス
	// fElapsedTime : 経過時間
	void Render(LPDIRECT3DDEVICE9 pDev, float fElapsedTime);
	
	void SetVisible(bool bVisible);	
	BOOL GetVisible(bool bVisible)
	{	return m_bVisible;			};

	void Hide();
	void Show();
	
	// OnLostDeviceEvent
	void OnLost();
	// OnResetDeviceEvent
	void OnReset();

protected:
	BOOL CreateControls(LPDIRECT3DTEXTURE9 pTexture, int nGUIResourceIndex,  CDXUTDialog* pDialog, type_session* pSess, int nSpriteWidth, int nSpriteHeight, int nAnimationCount, int nCharaWidth, int nCharaHeight);
	void CalcDrawRect();								// 描画範囲計算

	type_session*	p_pSession;					// オブジェクト情報
	CDXUTDialog*	p_pDialog;
	
	int m_nAnimationIndex;							// ｱﾆﾒｰｼｮﾝ描画範囲インデックス
	int m_nAnimationCount;							// ｱﾆﾒｰｼｮﾝ描画範囲数

	int m_nAnimationTimeCounter;				// ｱﾆﾒｰｼｮﾝｲﾝﾃﾞｯｸｽの数値を増やす時間
	int m_nAnimationTime;							// ｱﾆﾒｰｼｮﾝｲﾝﾃﾞｯｸｽの数値を増やす時間
	
	int m_nTextureWidth;							// テクスチャ横幅
	int m_nTextureHeight;							//	テクスチャ縦幅
	LPDIRECT3DTEXTURE9 p_pTexture;		// テクスチャ
	CDXUTButton*	m_pControlWord;			// 発言表示用コントロール
	CDXUTStatic*	m_pControlName;			// 名前表示用コントロール
	int m_nControlNameWidth;
	int m_nControlNameHeight;

	D3DXMATRIXA16	m_matScale;				// 表示サイズへのスケーリングマトリックス

	int m_nWordViewTimeCounter;				// 発言表示時間カウンタ
	int m_nWordViewTime;							// 発言表示時間

	D3DXVECTOR3	m_vecDrawCharaCenter;
	int m_nControlNameAdjustX;
	int m_nControlNameAdjustY;

	int m_nCharaWidth;
	int m_nCharaHeight;

	BOOL m_bCreated;								// 生成済み

	RECT m_recDraw;									// 描画範囲	
	bool m_bVisible;									// 表示状態
};

#endif
