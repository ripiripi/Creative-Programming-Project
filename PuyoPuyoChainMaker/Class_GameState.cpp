#include "myHeader.h"
#include "Class_GameState.h"

const int GameWidth = 6;
const int GameHeight = 12;

GameState::GameState() {
	for (int i = 0; i < GameWidth; i++)for (int j = 0; j < GameHeight; j++) {
		Board[i][j] = PuyoColor::none;
	}
}

int GameState::PuyoDownSimulation(int next, int pos, bool flag) {
	//���݂̏�Ԃ���Anext��pos�ɗ��Ƃ����󋵂��V���~���[�V��������
	return -1;
}

int GameState::ValueState() {//�Ֆʂ̕]���֐�
	return 0;
}

bool GameState::DownPuyo(int xPos, PuyoColor col) {//���݂̔ՖʂɂՂ��1�R�u��(PutsPuyo����Ăяo��)
	for (int yPos = 1; yPos < GameHeight; yPos++) {
		if (Board[xPos][yPos] != PuyoColor::none) {//�Ղ悪��������A���̏�Ɏw�肳�ꂽ�Ղ��u��
		Board[xPos][yPos] = col;
		return true;
		}
	}
	return false;
}

void GameState::PutsPuyo(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo) {//�g�Ղ��u�� pair.first::�g�Ղ�̂�����̂Ղ�@second::���̂Ղ�
	if (dir != 2) {
		DownPuyo(xPos, PairPuyo.second);
		switch (dir) {
		case 1:
			DownPuyo(xPos, PairPuyo.first);
			break;
		case 0:
			DownPuyo(xPos - 1, PairPuyo.first);
			break;
		case 3:
			DownPuyo(xPos + 1, PairPuyo.first);
			break;
		}
	}
	else {
		DownPuyo(xPos, PairPuyo.first);
		DownPuyo(xPos, PairPuyo.second);
	}
}
	
