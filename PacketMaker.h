#ifndef H_PACKET_MAKER___
#define H_PACKET_MAKER___

#include "windows.h"
#include <cassert>
#include "../include/define.h"
#include "../include/types.h"
#include "ext.h"


namespace PacketMaker
{
	// ACKパケット
	// msg	: out作成パケット
	INT MakePacketData_ACK(BYTE* msg);

	// 認証要求パケット
	// name	: ユーザ名
	// pass		: パスワード
	// msg	: out作成パケット
	INT MakePacketData_Authentication(WCHAR* name, WCHAR* pass, BYTE* msg);

	// チャットパケット
	// type				: 送信範囲
	// message		: メッセージ内容
	// data				: out作成パケット
	INT MakePacketData_ChatMessage(BYTE type, WCHAR* message, BYTE* data);

	// チームランダム化コマンドパケット
	// data				: out作成パケット
	INT MakePacketData_TeamRandomize(BYTE* data, BYTE teams);

	// 部屋移動パケット
	// vx					: 移動値x
	// vy					: 移動値y
	// data				: out作成パケット
	INT MakePacketData_RoomInfoMove(short vx, short vy, BYTE* data);

#if RTRIAL
#else
	// キャラ選択パケット
	// id					: キャラID
	// data				: out作成パケット
	INT MakePacketData_RoomCharacterSelect(BYTE id, BYTE* data);
#endif
	// 準備OKパケット
	// b					: 準備状態変更要求
	// data				: out作成パケット
	INT MakePacketData_RoomReady(BOOL b, BYTE* data);

	// アイテム選択パケット
	// index				: アイテム使用欄インデックス
	// item_flg			: アイテム
	// data				: out作成パケット
	INT MakePacketData_RoomSelectItem(int index, DWORD item_flg, BYTE* data);

	// チーム設定パケット
	// team_count	: チーム数変更値
	// data				: out作成パケット
	INT MakePacketData_RoomTeamCount(int team_count, BYTE* data);

	// ルール設定パケット
	// rule_flg			: ルール変更フラグ
	// data				: out作成パケット
	INT MakePacketData_RoomRule(BYTE flg, BYTE* data);

	// ステージ選択パケット
	// stage_index	: ステージインデックス値
	// data				: out作成パケット
	INT MakePacketData_RoomSelectStage(int stage_index, BYTE* data);

	// ロード完了パケット
	// data				: out作成パケット
	INT MakePacketData_LoadComplete(BYTE* data);

	// ステージ移動パケット
	// vx					: 移動値x
	// data				: out作成パケット
	INT MakePacketData_MainInfoMove(short vx, BYTE* data);

	// 振り向きパケット
	// vx					: 移動値x
	// data				: out作成パケット
	INT MakePacketData_MainInfoFlip(E_TYPE_USER_DIRECTION dir, BYTE* data);

	// 発射パケット
	// angle				: 角度
	// power			: 初速
	// blt_type			: 弾の種類
	// data				: out作成パケット
	INT MakePacketData_MainInfoShot(short angle, int power, int blt_type, int proc_type, int indicatorAngle, int indicatorPower, BYTE* data);

	// アイテム使用要求パケット
	// item_index
	// data				: out作成パケット
	INT MakePacketData_MainInfoItemOrder(int item_index, BYTE* data);

	// ターンパスパケット
	// data				: out作成パケット
	INT MakePacketData_MainInfoTurnPass(int obj_no, BYTE* data);

	// トリガーパケット
	INT MakePacketData_MainInfoTrigger(ptype_session sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, int nShotIndicatorAngle, int nShotIndicatorPower, BYTE* data);
	
	// トリガー終了
	INT MakePacketData_MainInfoTriggerEnd(ptype_session sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, int nShotIndicatorAngle, int nShotIndicatorPower, BYTE* data);

	// ショットパワースタートパケット
	INT MakePacketData_MainInfoShotPowerStart(ptype_session sess, int nProcType, int nBltType, int nShotAngle, BYTE* data);

	// ハッシュ値を返すパケットパケット
	INT MakePacketData_FileHash(BOOL bScrChara, std::map < int, TCHARA_SCR_INFO > *pCharaScrInfo, std::map <int, TSTAGE_SCR_INFO> *pStageScrInfo, BYTE* data);
	// ハッシュ値を返すパケットパケット
	INT MakePacketData_AuthRetHash(int hash_group, int hash_id, const char* code, int code_size, BYTE* data);

	// 結果確認パケット
	INT MakePacketData_Confirm(int nCharaIndex, BYTE* data);

	// ルール（制限ターン数）設定パケット
	INT MakePacketData_RoomRuleTurnLimit(int turn, BYTE* data);

	// ルール（制限時間）設定パケット
	INT MakePacketData_RoomRuleActTimeLimit(int obj_no,int time, BYTE* data);
	
	// ファイル
	INT MakePacketData_ReqFileData(BOOL bScrChara, int id, int no, int file_index, BYTE* data);
	// ファイル
	INT MakePacketData_ReqFileHash(BOOL bScrChara, int id, int no, BYTE* data);
};



#endif