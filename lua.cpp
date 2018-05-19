#include "ext.h"
#include "main.h"
#include <math.h>

static char g_GetUserNameBuffer[MAX_USER_NAME*2+2];

// lua event
int C_CreateBullet(double proc_type, double chr_obj_no, double chara_type, double blt_type, double obj_type, double posx, double posy, double vecx, double vecy, double addx, double addy, double hit_range, double extdata1, double extdata2)
{
//	type_obj* bltobj = g_pGame->GetScrProcObject();
//	if (bltobj)	return bltobj->obj_no;
	return -1;
}

bool C_Shot(int chr_obj_no, int proc_type, int chr_id, int blt_type, int vec_angle, int power, int frame, int indicator_angle, int indicator_power)
{
	return g_pGame->ShotCommand(chr_obj_no, proc_type, chr_id, blt_type, vec_angle, (int)(power*BLT_POS_FACT_F), frame, indicator_angle, indicator_power);
}

bool C_RemoveBullet(double obj_no, int rm_type)
{
	WCHAR log[32];
	SafePrintf(log, 32, L"C_RemoveBullet(%d,%d)", obj_no, rm_type);
	AddMessageLog(log);
	bool res = false;
	g_pCriticalSection->EnterCriticalSection_Object(L'(');
	type_blt* blt = g_pGame->GetBulletInfo((int)obj_no);
	if (blt)
	{
		res = true;
		common::obj::SetLuaFlg(blt, PROC_FLG_OBJ_REMOVE, TRUE);
		g_pGame->PauseSetObjectWaitForServerEvent((int)obj_no, TRUE);
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_IsRemovedObject(double obj_no)
{
	bool res = false;
	g_pCriticalSection->EnterCriticalSection_Object(L'J');
	type_blt* blt = g_pGame->GetBulletInfo((int)obj_no);
	if (blt)
	{
		if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)
			res = true;
	}
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_BombObject(int scr_id, int blt_type, int blt_chr_no, int blt_no, int pos_x, int pos_y, int erase)
{
	return true;
}

bool C_DamageCharaHP(int assailant_no, int victim_no, int hp)
{
	return true;
}

bool C_UpdateCharaStatus(int obj_no, int hp, int mv, int delay, int exp)
{
	return true;
}

bool C_UpdateCharaPos(int obj_no, int x,int y)
{
	return true;
}

bool C_SetBulletOptionData(int obj_no, int index, int data)
{
	g_pCriticalSection->EnterCriticalSection_Object(L')');
	bool res = g_pGame->SetBulletOptionData(obj_no, index, (DWORD)(data));
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

unsigned int C_GetBulletOptionData(int obj_no, int index)
{
	DWORD res = 0;
	g_pCriticalSection->EnterCriticalSection_Object(L'r');
	ptype_blt blt = g_pGame->GetBulletInfo(obj_no);
	if (index >= OBJ_OPTION_COUNT)
	{
		MessageBox(NULL, L"SetBulletOptionDataのindex値を超えています", L"lua error", MB_OK); 
		return 0;
	}
	if (blt)	res = blt->option[index];
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_SetCharaOptionData(int obj_no, int index, int data)
{
	g_pCriticalSection->EnterCriticalSection_Object(L')');
	bool res = g_pGame->SetCharaOptionData(obj_no, index, (DWORD)data);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

unsigned int C_GetCharaOptionData(int obj_no, int index)
{
	unsigned int res = 0;
	g_pCriticalSection->EnterCriticalSection_Object(L')');
	res = g_pGame->GetCharaOptionData(obj_no, index);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_SetBulletExtData1(int obj_no, unsigned long extdata1)
{
	g_pCriticalSection->EnterCriticalSection_Object(L')');
	bool res = g_pGame->SetBulletExtData1(obj_no, (DWORD)(extdata1));
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_SetBulletExtData2(int obj_no, unsigned long extdata2)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'=');
	bool res = g_pGame->SetBulletExtData2(obj_no, (DWORD)(extdata2));
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

unsigned int C_GetCharaExtData1(int obj_no)
{
	unsigned int res = 0;
	g_pCriticalSection->EnterCriticalSection_Session(L'!');
	res = g_pGame->GetCharaExtData1(obj_no);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

unsigned int C_GetCharaExtData2(int obj_no)
{
	unsigned int res = 0;
	g_pCriticalSection->EnterCriticalSection_Session(L'"');
	res = g_pGame->GetCharaExtData2(obj_no);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

bool C_SetCharaExtData1(int obj_no, unsigned long extdata1)
{
	g_pCriticalSection->EnterCriticalSection_Session(L'#');
	bool res = g_pGame->SetCharaExtData1(obj_no, (DWORD)(extdata1));
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

bool C_SetCharaExtData2(int obj_no, unsigned long extdata2)
{
	g_pCriticalSection->EnterCriticalSection_Session(L'$');
	bool res = g_pGame->SetCharaExtData2(obj_no, (DWORD)(extdata2));
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

bool C_AddCharaItem(int obj_no, int item_flg)
{
	return true;
}

bool C_SetCharaState(int obj_no, int chr_stt, int val)
{
	return true;
}

int C_GetCharaState(int obj_no, int chr_stt)
{
	int res = -1;
	g_pCriticalSection->EnterCriticalSection_Session(L'!');
	res = g_pGame->GetCharaState(obj_no, chr_stt);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

unsigned long C_GetCharaItem(int obj_no, int item_index)
{
	int res = 0;
	g_pCriticalSection->EnterCriticalSection_Session(L'!');
	res = g_pGame->GetCharaItem(obj_no, item_index);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

bool C_UpdateBulletState(double obj_no, double state)
{
	AddMessageLog(L"C_UpdateBulletState");
	bool res = false;
	g_pCriticalSection->EnterCriticalSection_Object(L'~');
	if (res = g_pGame->SetObjectLuaFlg((int)obj_no, PROC_FLG_OBJ_UPDATE_STATE, TRUE))
		g_pGame->PauseSetObjectWaitForServerEvent((int)obj_no, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_UpdateBulletPosition(double obj_no, double px, double py, double vx, double vy, double adx, double ady)
{
	AddMessageLog(L"C_UpdateBulletPositoin");
	bool res = false;
	if (res = g_pGame->SetObjectLuaFlg((int)obj_no, PROC_FLG_OBJ_UPDATE_POS, TRUE))
		g_pGame->PauseSetObjectWaitForServerEvent((int)obj_no, TRUE);
	return res;
}

bool C_UpdateBulletVector(double obj_no, double vx, double vy, double adx, double ady)
{
	AddMessageLog(L"C_UpdateBulletVector");
	g_pCriticalSection->EnterCriticalSection_Object(L'|');
	bool res = g_pGame->SetObjectLuaFlg((int)obj_no, PROC_FLG_OBJ_UPDATE_VEC, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_UpdateObjectType(int obj_no, int type)
{
	AddMessageLog(L"C_UpdateObjType");
	bool res = false;
	g_pCriticalSection->EnterCriticalSection_Object(L'q');
	if (res = g_pGame->SetObjectLuaFlg((int)obj_no, PROC_FLG_OBJ_UPDATE_TYPE, TRUE))
		g_pGame->PauseSetObjectWaitForServerEvent((int)obj_no, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

// キャラタイプ、ステージステージXY、テクスチャXYWH
void C_PasteTextureOnStage(int scr_id, int sx,int sy, int tx,int ty,int tw,int th)
{
//	g_pGame->PasteTextureOnStage(scr_id, sx,sy,tx,ty,tw,th);
	return;
}

//> get
int C_GetRand(int min, int max)
{
	if (min>max)
		MessageBox(g_hWnd, L"C_GetRand() min > max", L"script error", MB_OK);
	return (genrand_int32() % (max-min)) + min;
}
int C_GetEntityCharacters()
{
	int characters = g_pGame->GetEntityCharacters();
	return characters;
}

int C_GetLivingCharacters()
{
	g_pCriticalSection->EnterCriticalSection_Session(L'%');
	int characters = g_pGame->GetLivingCharacters();
	g_pCriticalSection->LeaveCriticalSection_Session();
	return characters;
}

type_session C_GetCharacterAtVector(int index)
{
	g_pCriticalSection->EnterCriticalSection_Session(L'&');
	type_session* sess = g_pGame->GetCharacterAtVector(index);
	g_pCriticalSection->LeaveCriticalSection_Session();
	if (!sess)
		MessageBox(g_hWnd, L"C_GetCharacterAtVector()", L"script error", MB_OK);
	return *sess;
}

type_session C_GetCharacterFromObjNo(int obj_no)
{
	g_pCriticalSection->EnterCriticalSection_Session(L'\'');
	type_session* sess = g_pGame->GetSessionObj(obj_no);
	g_pCriticalSection->LeaveCriticalSection_Session();
	AddMessageLog(L"C_GetCharacterFromObjNo");
	if (!sess)
		MessageBox(g_hWnd, L"C_GetCharacterFromObjNo()", L"script error", MB_OK);
	return *sess;
}

type_blt C_GetBulletInfo(int obj_no)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'w');
	type_blt* blt = g_pGame->GetBulletInfo(obj_no);
	g_pCriticalSection->LeaveCriticalSection_Object();
	if (!blt)
		MessageBox(g_hWnd, L"C_GetBulletInfo()", L"script error", MB_OK);
	return *blt;
}

int C_GetBulletAtkValue(int blt_no)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'e');
	int ret = g_pGame->GetBulletAtkValue(blt_no);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return ret;
}

void C_SetCameraFocusToBullet(int blt_no)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'e');
	g_pGame->SetCameraFocusToBullet(blt_no);
	g_pCriticalSection->LeaveCriticalSection_Object();
}

void C_SetCameraFocusToChara(int obj_no)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'e');
	g_pGame->SetCameraFocusToChara(obj_no);
	g_pCriticalSection->LeaveCriticalSection_Object();
}

unsigned int C_GetBulletExtData1(int obj_no)
{
	DWORD res = 0;
	g_pCriticalSection->EnterCriticalSection_Object(L'r');
	ptype_blt blt = g_pGame->GetBulletInfo(obj_no);
	if (blt)	res = blt->extdata1;
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

unsigned int C_GetBulletExtData2(int obj_no)
{
	DWORD res = 0;
	g_pCriticalSection->EnterCriticalSection_Object(L't');
	ptype_blt blt = g_pGame->GetBulletInfo(obj_no);
	if (blt)	res = blt->extdata2;
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

int C_GetWindValue()
{
	return g_pGame->GetWindValue();
}

void C_SetWindValue(int wind, int dir)
{
//	g_pSyncMain->SetWind(wind);
}

int C_GetStageWidth()
{
	return g_pGame->GetStageWidth();
}

int C_GetStageHeight()
{
	return g_pGame->GetStageWidth();
}

//> draw
bool C_UpdateBulletAngle(double blt_no, double angle)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'y');
	bool res = g_pGame->UpdateBulletAngle((int)blt_no, (int)angle);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_SetBulletTextureIndex(double blt_no, double index)
{
	g_pCriticalSection->EnterCriticalSection_Object(L'u');
	bool res = g_pGame->SetBulletTextureIndex((int)blt_no, (int)index);
	g_pCriticalSection->LeaveCriticalSection_Object();
	return res;
}

bool C_SetCharaTexture(int chr_no, int left, int top, int right, int bottom)
{
	RECT rcTexture = {left,top,right,bottom};
	g_pCriticalSection->EnterCriticalSection_Session(L')');
	bool res = g_pGame->SetCharaTexture(chr_no, &rcTexture);
	g_pCriticalSection->LeaveCriticalSection_Session();
	return res;
}

//> stage effect
int C_AddEffect(double chr_id, double left, double top, double right, double bottom, double x, double y, double age)
{
	if (!g_pGame->m_bEffectEnable)	return -1;
	RECT rcTexture = {(long)left, (long)top, (long)right, (long)bottom};
	D3DXVECTOR3 vecPos = D3DXVECTOR3((float)x,(float)y,0.0f);
	g_pCriticalSection->EnterCriticalSection_Effect('1');
	int nEffectNo = g_pGame->AddEffect((int)chr_id, &rcTexture, &vecPos, (int)age, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return nEffectNo;
}

bool C_RemoveEffect(double effect_no)
{
	g_pCriticalSection->EnterCriticalSection_Effect('2');
	bool res = g_pGame->RemoveEffect((int)effect_no, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectVector(double effect_no, double vx, double vy, double ax, double ay, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect('3');
	bool res = g_pGame->SetEffectVector((int)effect_no, &D3DXVECTOR3((float)vx,(float)vy,0.0f), &D3DXVECTOR3((float)ax,(float)ay,0.0f), TRUE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectAlpha(double effect_no, double alpha)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'4');
	bool res = g_pGame->SetEffectAlpha((int)effect_no, (BYTE)alpha, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectFade(double effect_no, double fade, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'5');
	bool res = g_pGame->SetEffectFade((int)effect_no, (int)fade, TRUE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectFadeInOut(double effect_no, double fadeinout)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'6');
	bool res = g_pGame->SetEffectFadeInOut((int)effect_no, (int)fadeinout, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectRotate(double effect_no, double rot)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'7');
	bool res = g_pGame->SetEffectRotate((int)effect_no, (float)rot, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectRotation(double effect_no, double rot, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'8');
	bool res = g_pGame->SetEffectRotation((int)effect_no, (float)rot, TRUE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectScale(double effect_no, double scalex, double scaley)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	bool res = false;
	if (_finite(scalex) && _finite(scaley)){
		g_pCriticalSection->EnterCriticalSection_Effect(L'9');
		res = g_pGame->SetEffectScale((int)effect_no, (float)scalex, (float)scaley,TRUE);
		g_pCriticalSection->LeaveCriticalSection_Effect();
	}
	return res;
}

bool C_SetEffectScalling(double effect_no, double scalex, double scaley, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'0');
	bool res = g_pGame->SetEffectScalling((int)effect_no, (float)scalex, (float)scaley, TRUE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectAnimation(double effect_no, double atime, double acount, bool loop, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'-');
	bool res = g_pGame->SetEffectAnimation((int)effect_no, (int)atime, (int)acount, loop?TRUE:FALSE, TRUE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetEffectTexture(int effect_no, double left, double top, double right, double bottom)
{
	RECT rcTexture = {(long)left, (long)top, (long)right, (long)bottom};
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'^');
	bool res = g_pGame->SetEffectTexture((int)effect_no, &rcTexture, TRUE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

// bg effect
int C_AddBGEffect(double chr_id, double left, double top, double right, double bottom, double x, double y, double age)
{
	if (!g_pGame->m_bEffectEnable)	return -1;
	RECT rcTexture = {(long)left, (long)top, (long)right, (long)bottom};
	D3DXVECTOR3 vecPos = D3DXVECTOR3((float)x,(float)y,0.0f);
	g_pCriticalSection->EnterCriticalSection_Effect(L'\\');
	int nEffectNo = g_pGame->AddEffect((int)chr_id, &rcTexture, &vecPos, (int)age, FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return nEffectNo;
}

bool C_RemoveBGEffect(double effect_no)
{
	g_pCriticalSection->EnterCriticalSection_Effect(L'!');
	bool res = g_pGame->RemoveEffect((int)effect_no, FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectVector(double effect_no, double vx, double vy, double ax, double ay, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'\"');
	bool res = g_pGame->SetEffectVector((int)effect_no, &D3DXVECTOR3((float)vx,(float)vy,0.0f), &D3DXVECTOR3((float)ax,(float)ay,0.0f), FALSE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectAlpha(double effect_no, double alpha)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'#');
	bool res = g_pGame->SetEffectAlpha((int)effect_no, (BYTE)alpha, FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectFade(double effect_no, double fade, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'$');
	bool res = g_pGame->SetEffectFade((int)effect_no, (int)fade, FALSE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectFadeInOut(double effect_no, double fadeinout)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'&');
	bool res = g_pGame->SetEffectFadeInOut((int)effect_no, (int)fadeinout, FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectRotate(double effect_no, double rot)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'\'');
	bool res = g_pGame->SetEffectRotate((int)effect_no, (float)rot, FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectRotation(double effect_no, double rot, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'(');
	bool res = g_pGame->SetEffectRotation((int)effect_no, (float)rot, FALSE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectScale(double effect_no, double scalex, double scaley)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L')');
	bool res = g_pGame->SetEffectScale((int)effect_no, (float)scalex, (float)scaley,FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectScalling(double effect_no, double scalex, double scaley, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'=');
	bool res = g_pGame->SetEffectScalling((int)effect_no, (float)scalex, (float)scaley, FALSE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectAnimation(double effect_no, double atime, double acount, bool loop, double effect_time)
{
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'~');
	bool res = g_pGame->SetEffectAnimation((int)effect_no, (int)atime, (int)acount, loop?TRUE:FALSE, FALSE, (int)effect_time);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}

bool C_SetBGEffectTexture(int effect_no, double left, double top, double right, double bottom)
{
	RECT rcTexture = {(long)left, (long)top, (long)right, (long)bottom};
	if (!g_pGame->m_bEffectEnable)	return true;
	g_pCriticalSection->EnterCriticalSection_Effect(L'|');
	bool res = g_pGame->SetEffectTexture((int)effect_no, &rcTexture, FALSE);
	g_pCriticalSection->LeaveCriticalSection_Effect();
	return res;
}


// sp effect
void C_HideStage()
{
	g_pGame->HideStage();
}

void C_ShowStage()
{
	g_pGame->ShowStage();
}

// sound
// リソースの登録
void C_RegistSoundSE(char* rc_name)
{
	g_pGame->RegistSoundSE(rc_name);
}

// リソースの再生
void C_PlaySoundSE(char* rc_name, int loop, double fade)
{
	if (rc_name)
		g_pGame->PlayScrSoundSE(rc_name, loop, (int)fade);
	else
		AddMessageLog(L"C_PlaySoundSE() resouce name is null!!");
}

// クライントのキャラNoを得る(得られないなら-1)
int C_GetMyCharaNo()
{
	if (ptype_session sess = g_pGame->GetMySessionInfo())
		return (int)sess->obj_no;
	return -1;
}

// 文字列をログに追加
void C_AddMsgLog(char* str)
{
	int len = strlen(str);
	WCHAR *wstr = new wchar_t[len +1]();
	if (wstr)
	{
		if (MultiByteToWideChar(CP_THREAD_ACP, 0, str, len, wstr, len))
			g_pGame->AddChatMessage(wstr, PK_USER_CHAT_SERVER_INFO);
	}
	SafeDeleteArray(wstr);
}

int C_GetAngle(int x, int y)
{
	return GetAngle(x,y);
}

int C_GetScrIDFromChrNo(int obj_no)
{
	g_pCriticalSection->EnterCriticalSection_Session(L')');
	ptype_session sess= g_pGame->GetSessionObj(obj_no);
	int ret = -1;
	if (sess)
		ret = sess->scrinfo->ID;
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

int C_GetUserTeamNo(int obj_no)
{
	g_pCriticalSection->EnterCriticalSection_Session(L')');
	ptype_session sess= g_pGame->GetSessionObj(obj_no);
	int ret = -1;
	if (sess)
		ret = sess->team_no;
	g_pCriticalSection->LeaveCriticalSection_Session();
	return ret;
}

char* C_GetUserName(int obj_no)
{
	g_pCriticalSection->EnterCriticalSection_Session(L'=');
	ptype_session sess= g_pGame->GetSessionObj(obj_no);
	g_GetUserNameBuffer[0] = NULL;
	if (sess)
	{
		WCHAR wsName[MAX_USER_NAME+1];
		common::session::GetSessionName(sess, wsName);
		WideCharToMultiByte(CP_THREAD_ACP, 0, wsName, -1, g_GetUserNameBuffer, MAX_USER_NAME*2+2,NULL,NULL);
	}
	g_pCriticalSection->LeaveCriticalSection_Session();
	return g_GetUserNameBuffer;
}

void C_DbgOutputStr(char* str)
{
	MessageBoxA(NULL, str, "C_DbgOutputStr", MB_OK);
}

void C_DbgOutputNum(int num)
{
	WCHAR msg[64];
	SafePrintf(msg, 64, L"C_DbgOutputNum:%d", num);
	MessageBox(g_hWnd, msg, L"C_DbgOutputNum", MB_OK);
}

void C_DbgAddLogNum(int num)
{
	WCHAR msg[64];
	SafePrintf(msg, 64, L"C_DbgAddLogNum:%d", num);
	AddMessageLog(msg);
}


type_ground C_GetRandomStagePos()
{
	type_ground res = {0,0,false};
	
	std::vector<int> vecXPos;
	D3DXVECTOR2	vecGround = D3DXVECTOR2(0,0);
	CMainStage* pMainStage = g_pGame->GetMainStage();
	pMainStage->GetGroundsXPos(&vecXPos);

	int nRndWidth = vecXPos.size();
	if (!nRndWidth)
		return res;

	D3DXVECTOR2 vecPos;
	D3DXVECTOR2 vecPut;
	int nStageHeight = pMainStage->GetStageHeight();
	vecPos.x = (float)( vecXPos[genrand_int32()%nRndWidth]);
	vecPos.y = (float)(genrand_int32()%nStageHeight);
	// 地面を探す
	while (!pMainStage->FindGround(&vecPut, &vecGround, &vecPos, CHARA_BODY_RANGE))
	{
		vecPos.x = (float)(vecXPos[genrand_int32()%nRndWidth]);
		vecPos.y = (float)(genrand_int32()%nStageHeight);
	}
	res.x = (int)vecPut.x;
	res.y = (int)vecPut.y;
	res.ground = true;
	return res;
}

type_ground C_GetGroundPos(int x, int y)
{
	type_ground res = {0,0,false};
	D3DXVECTOR2 vecPut;
	D3DXVECTOR2 vecGround = D3DXVECTOR2(0,0);
	D3DXVECTOR2 vecPos = D3DXVECTOR2((float)x,(float)y);

	if (g_pGame->GetMainStage()->FindGround(&vecPut, &vecGround, &vecPos, CHARA_BODY_RANGE))
	{
		res.x = (int)vecPut.x;
		res.y = (int)vecPut.y;
		res.ground = true;
	}
	return res;
}
