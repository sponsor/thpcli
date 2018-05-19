#include "PacketMaker.h"

// ACKパケット
// msg	: out作成パケット
INT PacketMaker::MakePacketData_ACK(BYTE* msg)
{
	int datIndex = 2;
	if (!msg)	return 0;

	datIndex += SetByteData(&msg[datIndex], PK_ACK);
	// 1
	datIndex += SetByteData(&msg[datIndex], 1);
	// 1
	datIndex += SetDwordData(&msg[datIndex], 1);
	// エンドマーカ
	datIndex += SetEndMarker(&msg[datIndex]);
	SetWordData(&msg[0], datIndex);

	return datIndex;
}

//> 認証要求パケット 
INT PacketMaker::MakePacketData_Authentication(WCHAR* name, WCHAR* pass, BYTE* msg)
{
	int datIndex = 2;
	if (!msg || !name)	return 0;
	int nNameLen = (_tcslen(name)) * sizeof(WCHAR);
	int nPassLen = (_tcslen(pass)) * sizeof(WCHAR);

	datIndex += SetByteData(&msg[datIndex], PK_USER_AUTH);
	// キャラ名長
	datIndex += SetByteData(&msg[datIndex], nNameLen);
	// キャラ名
	datIndex += SetMultiByteData(&msg[datIndex], (BYTE*)name, nNameLen, MAX_USER_NAME*sizeof(WCHAR));
	// パスワード長
	datIndex += SetByteData(&msg[datIndex], nPassLen);
	// パスワード
	datIndex += SetMultiByteData(&msg[datIndex], (BYTE*)pass, nPassLen, MAX_SRV_PASS*sizeof(WCHAR));
	// エンドマーカ
	datIndex += SetEndMarker(&msg[datIndex]);
	SetWordData(&msg[0], datIndex);

	return datIndex;
}
//< 認証要求パケット


//> チャットパケット 
INT PacketMaker::MakePacketData_ChatMessage(BYTE type, WCHAR* message, BYTE* data)
{
	int datIndex = 2;
	if (!data || !message)	return 0;

	int nMessageLen = _tcslen(message)*sizeof(WCHAR);
	
	datIndex += SetByteData(&data[datIndex], PK_USER_CHAT);
	// 送信範囲
	datIndex += SetByteData(&data[datIndex], type);
	// メッセージ長
	datIndex += SetByteData(&data[datIndex], (BYTE)nMessageLen);
	// メッセージ
	datIndex += SetMultiByteData(&data[datIndex], (BYTE*)message, nMessageLen, MAX_CHAT_MSG*sizeof(WCHAR));

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< チャットパケット 

//>> チームランダム化コマンドパケット
// data				: out作成パケット
INT PacketMaker::MakePacketData_TeamRandomize(BYTE* data, BYTE teams)
{
	int datIndex = 2;
	if (!data)	return 0;
	datIndex += SetByteData(&data[datIndex], PK_REQ_TEAM_RAND);
	datIndex += SetByteData(&data[datIndex], teams);	// extra data
	
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//<< チームランダム化コマンドパケット


//> 部屋移動パケット
// PK_USER_ROOMINFO_MOVE
// size			: 2	0
// header		: 1	2
// mv_x			: 1	3	(char)
// mv_y			: 1	4	(char)
// footer		: 2	6
INT PacketMaker::MakePacketData_RoomInfoMove(short vx, short vy, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_MV);
	// 移動値X
	datIndex += SetShortData(&data[datIndex], vx);
	// 移動値Y
	datIndex += SetShortData(&data[datIndex], vy);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< 部屋移動パケット

#if RTRIAL
#else
//> キャラ選択パケット
// id					: キャラID
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomCharacterSelect(BYTE id, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_CHARA_SEL);
	// キャラID
	datIndex += SetByteData(&data[datIndex], id);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;

}
//< キャラ選択パケット
#endif
//> 準備OKパケット
// b					: 準備状態変更要求
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomReady(BOOL b, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_READY);
	// 移動値X
	datIndex += SetByteData(&data[datIndex], (BYTE)b);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;	
}
//< 準備OKパケット

//> アイテム選択パケット
// index				: アイテム使用欄インデックス
// item_flg			: アイテム
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomSelectItem(int index, DWORD item_flg, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_ITEM_SEL);
	// アイテム使用欄インデックス
	datIndex += SetByteData(&data[datIndex], (BYTE)index);
	// アイテム使用欄インデックス
	datIndex += SetDwordData(&data[datIndex], item_flg);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;	
}
//< アイテム選択パケット

//> チーム設定パケット
// team_count	: チーム数変更値
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomTeamCount(int team_count, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_TEAM_COUNT);
	// アイテム使用欄インデックス
	datIndex += SetByteData(&data[datIndex], (BYTE)team_count);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;	
}
//< チーム設定パケット

//> ルール設定パケット
// rule_flg			: ルール変更フラグ
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomRule(BYTE flg, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_RULE);
	// アイテム使用欄インデックス
	datIndex += SetByteData(&data[datIndex], flg);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;	
}
//< ルール設定パケット

//> ルール（制限ターン数）設定パケット
// turn				: 制限ターン数
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomRuleTurnLimit(int turn, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_RULE_TURN_LIMIT);
	// 制限ターン数
	datIndex += SetWordData(&data[datIndex], (short)turn);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< ルール設定パケット

//> 制限時間設定パケット
INT PacketMaker::MakePacketData_RoomRuleActTimeLimit(int obj_no, int time, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_RULE_ACT_TIME_LIMIT);
	// obj_no
	datIndex += SetByteData(&data[datIndex], (BYTE)obj_no);
	// 制限ターン数
	datIndex += SetByteData(&data[datIndex], (BYTE)time);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< 制限時間設定パケット

//> ステージ選択パケット
// stage_index	: ステージインデックス値
// data				: out作成パケット
INT PacketMaker::MakePacketData_RoomSelectStage(int stage_index, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_ROOM_STAGE_SEL);
	// ステージインデックス値
	datIndex += SetByteData(&data[datIndex], stage_index);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;	
}
//< ステージ選択パケット

//> ロード完了パケット
// data				: out作成パケット
INT PacketMaker::MakePacketData_LoadComplete(BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_LOAD_COMPLETE);
	// 1
	datIndex += SetByteData(&data[datIndex], 1);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;	
}
//> ロード完了パケット

//> ステージ移動パケット
// vx					: 移動値x
// data				: out作成パケット
INT PacketMaker::MakePacketData_MainInfoMove(short vx, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_MV);
	// 移動値X
	datIndex += SetShortData(&data[datIndex], vx);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//> ステージ移動パケット

//> 更新パケット
// vx				: 移動値x
// data				: out作成パケット
INT PacketMaker::MakePacketData_MainInfoFlip(E_TYPE_USER_DIRECTION dir, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_FLIP);
	// 向き
	datIndex += SetByteData(&data[datIndex], dir);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//> ステージ移動パケット

//> 発射パケット
// angle				: 角度
// power			: 初速
// blt_type			: 弾の種類
// data				: out作成パケット
INT PacketMaker::MakePacketData_MainInfoShot(short angle, int power, int blt_type, int proc_type, int indicatorAngle, int indicatorPower, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_SHOT);
	// 角度
	datIndex += SetShortData(&data[datIndex], angle);
	// 
	datIndex += SetByteData(&data[datIndex], (BYTE)power);
	// 弾の種類
	datIndex += SetByteData(&data[datIndex], (BYTE)blt_type);
	// 処理の種類
	datIndex += SetByteData(&data[datIndex], (BYTE)proc_type);
	// インジケーター角度
	datIndex += SetShortData(&data[datIndex], (short)indicatorAngle);
	// インジケーターパワー
	datIndex += SetByteData(&data[datIndex], (BYTE)indicatorPower);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< 発射パケット

//> ターンパスパケット
// data				: out作成パケット
INT PacketMaker::MakePacketData_MainInfoTurnPass(int obj_no, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_TURN_PASS);
	// 1
	datIndex += SetShortData(&data[datIndex], (short)obj_no);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< ターンパスパケット

// アイテム使用要求パケット
// item_index
// data				: out作成パケット
INT PacketMaker::MakePacketData_MainInfoItemOrder(int item_index, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_ITEM);
	// item index
	datIndex += SetByteData(&data[datIndex], (BYTE)item_index);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}

// トリガーパケット
INT PacketMaker::MakePacketData_MainInfoTrigger(ptype_session sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, int nShotIndicatorAngle, int nShotIndicatorPower, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_TRIGGER);
	// キャラ番号
	datIndex += SetShortData(&data[datIndex], sess->obj_no );
	// スクリプト/アイテムのタイプ
	datIndex += SetByteData(&data[datIndex], (BYTE)nProcType);
	// 演出のタイプ
	datIndex += SetByteData(&data[datIndex], (BYTE)nBltType);
	// 角度
	datIndex += SetShortData(&data[datIndex], (short)nShotAngle);
	// パワー
	datIndex += SetShortData(&data[datIndex], (short)nShotPower);
	// indicator角度
	datIndex += SetShortData(&data[datIndex], (short)nShotIndicatorAngle);
	// indicatorパワー
	datIndex += SetShortData(&data[datIndex], (short)nShotIndicatorPower);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}

// トリガー終了
INT PacketMaker::MakePacketData_MainInfoTriggerEnd(ptype_session sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, int nShotIndicatorAngle, int nShotIndicatorPower, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_USER_MAIN_TRIGGER_END);
	// キャラ番号
	datIndex += SetShortData(&data[datIndex], sess->obj_no );
	// スクリプト/アイテムのタイプ
	datIndex += SetByteData(&data[datIndex], (BYTE)nProcType);
	// 演出のタイプ
	datIndex += SetByteData(&data[datIndex], (BYTE)nBltType);
	// 角度
	datIndex += SetShortData(&data[datIndex], (short)nShotAngle);
	// パワー
	datIndex += SetShortData(&data[datIndex], (short)nShotPower);
	// indicator角度
	datIndex += SetShortData(&data[datIndex], (short)nShotIndicatorAngle);
	// indicatorパワー
	datIndex += SetShortData(&data[datIndex], (short)nShotIndicatorPower);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}

// ショットパワースタートパケット
INT PacketMaker::MakePacketData_MainInfoShotPowerStart(ptype_session sess, int nProcType, int nBltType, int nShotAngle, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_MAIN_SHOTPOWER);
	// キャラ番号
	datIndex += SetShortData(&data[datIndex], sess->obj_no );
	// スクリプト/アイテムのタイプ
	datIndex += SetByteData(&data[datIndex], (BYTE)nProcType);
	// 演出のタイプ
	datIndex += SetByteData(&data[datIndex], (BYTE)nBltType);
	// 角度
	datIndex += SetShortData(&data[datIndex], (short)nShotAngle);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}

//> 結果確認パケット
// data				: out作成パケット
INT PacketMaker::MakePacketData_Confirm(int nCharaIndex, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CMD_CONFIRM);
	// chara index
	datIndex += SetByteData(&data[datIndex], (BYTE)nCharaIndex);
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
//< 結果確認パケット

// ハッシュ値を返すパケットパケット
INT PacketMaker::MakePacketData_AuthRetHash(int hash_group, int hash_id, const char* code, int code_size, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_RET_HASH);
	// hash_group
	datIndex += SetByteData(&data[datIndex], (BYTE)hash_group);
	// hash_id
	datIndex += SetWordData(&data[datIndex], (WORD)hash_id);
	// hash_code_size
	datIndex += SetByteData(&data[datIndex], (BYTE)code_size);
	// hash_code
	datIndex += SetMultiByteData(&data[datIndex], (BYTE*)code, code_size, MAX_PACKET_SIZE);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}

// ファイル要求パケット
INT PacketMaker::MakePacketData_ReqFileData(BOOL bScrChara, int id, int no, int file_index, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_REQ_FILE_DATA);
	// chara
	datIndex += SetByteData(&data[datIndex], (BYTE)bScrChara);
	// id
	datIndex += SetWordData(&data[datIndex], (WORD)id);
	// fileno
	datIndex += SetByteData(&data[datIndex], (BYTE)no);
	// file_index
	datIndex += SetDwordData(&data[datIndex], (DWORD)file_index);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}

// ファイル要求パケット
INT PacketMaker::MakePacketData_FileHash(BOOL bScrChara, std::map < int, TCHARA_SCR_INFO > *pCharaScrInfo, std::map <int, TSTAGE_SCR_INFO> *pStageScrInfo, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;
	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_CHK_FILE_HASH);
	// chara count
	datIndex += SetWordData(&data[datIndex], (WORD)pCharaScrInfo->size());

	std::map < int, TCHARA_SCR_INFO >::iterator itCharaEnd = pCharaScrInfo->end();
	for (std::map < int, TCHARA_SCR_INFO >::iterator itChara = pCharaScrInfo->begin();
		itChara != itCharaEnd;
		++itChara)
	{
		// id
		datIndex += SetWordData(&data[datIndex], (WORD)(*itChara).second.ID);
		// hash
		BytesFromHexString(&data[datIndex], (*itChara).second.md5);
		datIndex += 16;
//		datIndex += SetMultiByteData(&data[datIndex], (BYTE*)md5.code(), len, MAX_PACKET_SIZE);
	}

	// stage count
	datIndex += SetWordData(&data[datIndex], (WORD)pStageScrInfo->size());

	std::map < int, TSTAGE_SCR_INFO >::iterator itStageEnd = pStageScrInfo->end();
	for (std::map < int, TSTAGE_SCR_INFO >::iterator itStage = pStageScrInfo->begin();
		itStage != itStageEnd;
		++itStage)
	{
		// id
		datIndex += SetWordData(&data[datIndex], (WORD)(*itStage).second.ID);
		// hash
		BytesFromHexString(&data[datIndex], (*itStage).second.md5);
		datIndex += 16;
//		datIndex += SetMultiByteData(&data[datIndex], (BYTE*)md5.code(), len, MAX_PACKET_SIZE);
	}
	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);
	return datIndex;
}

// ファイル要求パケット
INT PacketMaker::MakePacketData_ReqFileHash(BOOL bScrChara, int id, int no, BYTE* data)
{
	int datIndex = 2;
	if (!data)	return 0;

	// ヘッダ
	datIndex += SetByteData(&data[datIndex], PK_REQ_FILE_HASH);
	// chara
	datIndex += SetByteData(&data[datIndex], (BYTE)bScrChara);
	// id
	datIndex += SetWordData(&data[datIndex], (WORD)id);
	// fileno
	datIndex += SetByteData(&data[datIndex], (BYTE)no);

	// エンドマーカ
	datIndex += SetEndMarker(&data[datIndex]);
	SetWordData(&data[0], datIndex);

	return datIndex;
}
