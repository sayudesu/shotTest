#include <DxLib.h>
#include "Vec2.h"
#include "game.h"
#include "Pad.h"


namespace Shot
{
	constexpr int kSize = 10;
	constexpr int kShotMaxNum = 100;
	constexpr int kShotNum = 100;//31;
	constexpr int kShotSpeed = 30;
}
namespace Player
{
	constexpr int kSize = 100;
	constexpr int kPlayerSpeed = 10;
}
namespace Enemy
{
	constexpr int kSize = 30;
	constexpr int kSpeed = 5;
}
namespace Color	
{
	constexpr int kRed = 0xff0000;
	constexpr int kWhite = 0xffffff;
}

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//ウィンドウモード設定
	ChangeWindowMode(Game::kWindowMode);
	//ウィンドウ名設定
	SetMainWindowText(Game::kTitelText);
	//画面サイズ設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	//ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);
	
	Vec2 pos = {0.0f,0.0f};//プレイヤーの座標
	Vec2 enemyPos = { Game::kScreenWidth - 100 ,Game::kScreenHeight / 2 };//プレイヤーの座標

	Vec2 shotPos[Shot::kShotMaxNum];//弾の位置
	int shot[Shot::kShotMaxNum];//弾の数
	int shotNum = 0;
	int reLoadTimer = 0;
	int enemyColor = Color::kWhite;
	int enemyDamageCount = 0;
	int enemyHitRand = 0;
	bool isReLoadTime = false;
	bool isEnemyMove = false;
	bool isEnemyDamage = false;
	bool shotNow[Shot::kShotMaxNum];
	for (int i = 0; i < Shot::kShotMaxNum; i++)
	{
		shotPos[i] = { 0.0f,0.0f };//弾の位置
		shot[i] = 0;//弾の数
		shotNow[i] = false;
	}


	while (ProcessMessage() == 0)
	{
		const LONGLONG time = GetNowHiPerformanceCount();

		//画面をクリアする
		ClearDrawScreen();

		const int PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		Pad::update();
		/////////////////////*操作*/////////////////////
		//playerの操作
		if (PadInput&(PAD_INPUT_UP))pos.y -= Player::kPlayerSpeed;//上に動く
		if (PadInput&(PAD_INPUT_DOWN))pos.y += Player::kPlayerSpeed;//下に動く
		if (PadInput & (PAD_INPUT_LEFT))pos.x -= Player::kPlayerSpeed;//上に動く
		if (PadInput & (PAD_INPUT_RIGHT))pos.x += Player::kPlayerSpeed;//下に動く

		if (Pad::isTrigger(PAD_INPUT_3))//XBOX Xボタン　再装填
		{
			isReLoadTime = true;//再装填開始
			
		}
		if (isReLoadTime)//再装填準備
		{
			reLoadTimer++;//再装填中
			if (reLoadTimer == 60)//再装填完了
			{
				shotNum = 0;//弾を初期化
				reLoadTimer = 0;//再装填時間を初期化
				isReLoadTime = false;//再装填終了
			}
		}
		if (Pad::isTrigger(PAD_INPUT_1)|| (PadInput & PAD_INPUT_2))//XBOX Aボタン　発砲
		{
			if (shotNum < Shot::kShotNum && !isReLoadTime)//弾発射条件
			{
				shotNum++;//弾数
				for (int i = 0; i < shotNum; i++)
				{
					if (!shotNow[i])//まだ発射されていない弾から発射
					{
						shotNow[i] = true;//弾発射確認

						//playerの位置から発射
						shotPos[i].x = pos.x;
						shotPos[i].y = pos.y + 50;

						break;
					}
				}
			}
		}

		/////////////////////*敵に動き*/////////////////////
		if (!isEnemyMove)//敵の動き
		{
			enemyPos.y -= Enemy::kSpeed;
			if (enemyPos.y < 0)isEnemyMove = true;
		}
		else
		{
			enemyPos.y += Enemy::kSpeed;
			if (enemyPos.y > Game::kScreenHeight - Enemy::kSize)isEnemyMove = false;
		}

		/////////////////////*弾の確認*/////////////////////
		for (int i = 0; i < shotNum;  i++)
		{
			if(shotNow[i])//確認したまたを右に発射
			{
				shotPos[i].x += Shot::kShotSpeed;//右に撃つ
			}

			if (shotNow[i])//発射されている弾の位置を確認
			{
				if (shotPos[i].x > Game::kScreenWidth)//画面外に飛んだ弾を非表示
				{
					shotNow[i] = false;//非表示
					//playerの位置に戻す
					shotPos[i].x = pos.x;
					shotPos[i].y = pos.y + 50;
				}
			}

			/////////////////////*当たり判定*/////////////////////
			//弾と敵に判定
			if ((enemyPos.x + Enemy::kSize > shotPos[i].x) &&
				(enemyPos.x < shotPos[i].x + Shot::kSize))
			{
				if ((enemyPos.y + Enemy::kSize > shotPos[i].y) &&
					(enemyPos.y < shotPos[i].y + Shot::kSize))
				{
					isEnemyDamage = true;
					//敵の色を赤に
					
				}
			}
		}
		//enemyがダメージを受けた場合
		if (isEnemyDamage)
		{
			enemyDamageCount++;
			enemyColor = Color::kRed;
			enemyHitRand = GetRand(30);
			if (enemyDamageCount == 30)
			{
				isEnemyDamage = false;
				enemyColor = Color::kWhite;
				enemyHitRand = 0;
				enemyDamageCount = 0;
			}
		}
		else
		{
			//敵の色を白に
			enemyColor = Color::kWhite;
		}
	

		/////////////////////*描画*/////////////////////
		for (int i = 0; i < shotNum; i++)//弾の描画
		{
			if (shotNow[i])//発射される弾を描画
			{
				//playerの弾を描画
				DrawBox(static_cast<int>(shotPos[i].x), static_cast<int>(shotPos[i].y), static_cast<int>(shotPos[i].x) + Shot::kSize, static_cast<int>(shotPos[i].y) + Shot::kSize, 0xff0000, true);
			}
		}
		
		//player描画
		DrawBox(pos.x, pos.y, pos.x + Player::kSize , pos.y + Player::kSize, 0xffffff, true);
		//playerの弾数
		DrawFormatString(pos.x + 30, pos.y + 40, 0x0000ff, "%d", shotNum);
		DrawFormatString(pos.x + 30, pos.y + 60, 0x0000ff, "%d", reLoadTimer);

		//enemy描画
		DrawBox(static_cast<int>(enemyPos.x) + enemyHitRand, static_cast<int>(enemyPos.y) + enemyHitRand, 
			static_cast<int>(enemyPos.x) + Enemy::kSize + enemyHitRand, static_cast<int>(enemyPos.y) + Enemy::kSize + enemyHitRand, enemyColor, true);

		//裏画面を表画面を入れ替える
		ScreenFlip();

		//escを押して終了
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;

		//60fps固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}
	}

	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}