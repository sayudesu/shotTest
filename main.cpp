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

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//�E�B���h�E���[�h�ݒ�
	ChangeWindowMode(Game::kWindowMode);
	//�E�B���h�E���ݒ�
	SetMainWindowText(Game::kTitelText);
	//��ʃT�C�Y�ݒ�
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	//�_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);
	
	Vec2 pos = {0.0f,0.0f};//�v���C���[�̍��W
	Vec2 enemyPos = { Game::kScreenWidth - 100 ,Game::kScreenHeight / 2 };//�v���C���[�̍��W

	Vec2 shotPos[Shot::kShotMaxNum];//�e�̈ʒu
	int shot[Shot::kShotMaxNum];//�e�̐�
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
		shotPos[i] = { 0.0f,0.0f };//�e�̈ʒu
		shot[i] = 0;//�e�̐�
		shotNow[i] = false;
	}


	while (ProcessMessage() == 0)
	{
		const LONGLONG time = GetNowHiPerformanceCount();

		//��ʂ��N���A����
		ClearDrawScreen();

		const int PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		Pad::update();
		/////////////////////*����*/////////////////////
		//player�̑���
		if (PadInput&(PAD_INPUT_UP))pos.y -= Player::kPlayerSpeed;//��ɓ���
		if (PadInput&(PAD_INPUT_DOWN))pos.y += Player::kPlayerSpeed;//���ɓ���
		if (PadInput & (PAD_INPUT_LEFT))pos.x -= Player::kPlayerSpeed;//��ɓ���
		if (PadInput & (PAD_INPUT_RIGHT))pos.x += Player::kPlayerSpeed;//���ɓ���

		if (Pad::isTrigger(PAD_INPUT_3))//XBOX X�{�^���@�đ��U
		{
			isReLoadTime = true;//�đ��U�J�n
			
		}
		if (isReLoadTime)//�đ��U����
		{
			reLoadTimer++;//�đ��U��
			if (reLoadTimer == 60)//�đ��U����
			{
				shotNum = 0;//�e��������
				reLoadTimer = 0;//�đ��U���Ԃ�������
				isReLoadTime = false;//�đ��U�I��
			}
		}
		if (Pad::isTrigger(PAD_INPUT_1)|| (PadInput & PAD_INPUT_2))//XBOX A�{�^���@���C
		{
			if (shotNum < Shot::kShotNum && !isReLoadTime)//�e���ˏ���
			{
				shotNum++;//�e��
				for (int i = 0; i < shotNum; i++)
				{
					if (!shotNow[i])//�܂����˂���Ă��Ȃ��e���甭��
					{
						shotNow[i] = true;//�e���ˊm�F

						//player�̈ʒu���甭��
						shotPos[i].x = pos.x;
						shotPos[i].y = pos.y + 50;

						break;
					}
				}
			}
		}

		/////////////////////*�G�ɓ���*/////////////////////
		if (!isEnemyMove)//�G�̓���
		{
			enemyPos.y -= Enemy::kSpeed;
			if (enemyPos.y < 0)isEnemyMove = true;
		}
		else
		{
			enemyPos.y += Enemy::kSpeed;
			if (enemyPos.y > Game::kScreenHeight - Enemy::kSize)isEnemyMove = false;
		}

		/////////////////////*�e�̊m�F*/////////////////////
		for (int i = 0; i < shotNum;  i++)
		{
			if(shotNow[i])//�m�F�����܂����E�ɔ���
			{
				shotPos[i].x += Shot::kShotSpeed;//�E�Ɍ���
			}

			if (shotNow[i])//���˂���Ă���e�̈ʒu���m�F
			{
				if (shotPos[i].x > Game::kScreenWidth)//��ʊO�ɔ�񂾒e���\��
				{
					shotNow[i] = false;//��\��
					//player�̈ʒu�ɖ߂�
					shotPos[i].x = pos.x;
					shotPos[i].y = pos.y + 50;
				}
			}

			/////////////////////*�����蔻��*/////////////////////
			//�e�ƓG�ɔ���
			if ((enemyPos.x + Enemy::kSize > shotPos[i].x) &&
				(enemyPos.x < shotPos[i].x + Shot::kSize))
			{
				if ((enemyPos.y + Enemy::kSize > shotPos[i].y) &&
					(enemyPos.y < shotPos[i].y + Shot::kSize))
				{
					isEnemyDamage = true;
					//�G�̐F��Ԃ�
					
				}
			}
		}
		//enemy���_���[�W���󂯂��ꍇ
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
			//�G�̐F�𔒂�
			enemyColor = Color::kWhite;
		}
	

		/////////////////////*�`��*/////////////////////
		for (int i = 0; i < shotNum; i++)//�e�̕`��
		{
			if (shotNow[i])//���˂����e��`��
			{
				//player�̒e��`��
				DrawBox(static_cast<int>(shotPos[i].x), static_cast<int>(shotPos[i].y), static_cast<int>(shotPos[i].x) + Shot::kSize, static_cast<int>(shotPos[i].y) + Shot::kSize, 0xff0000, true);
			}
		}
		
		//player�`��
		DrawBox(pos.x, pos.y, pos.x + Player::kSize , pos.y + Player::kSize, 0xffffff, true);
		//player�̒e��
		DrawFormatString(pos.x + 30, pos.y + 40, 0x0000ff, "%d", shotNum);
		DrawFormatString(pos.x + 30, pos.y + 60, 0x0000ff, "%d", reLoadTimer);

		//enemy�`��
		DrawBox(static_cast<int>(enemyPos.x) + enemyHitRand, static_cast<int>(enemyPos.y) + enemyHitRand, 
			static_cast<int>(enemyPos.x) + Enemy::kSize + enemyHitRand, static_cast<int>(enemyPos.y) + Enemy::kSize + enemyHitRand, enemyColor, true);

		//����ʂ�\��ʂ����ւ���
		ScreenFlip();

		//esc�������ďI��
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;

		//60fps�Œ�
		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}
	}

	DxLib_End();			// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}