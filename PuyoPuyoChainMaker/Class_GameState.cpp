#include "myHeader.h"
#include "Class_GameState.h"
#include "Class_TimeController.h"

const int GameWidth = 6;
const int GameHeight = 12;

GameState::GameState() {
	MaxScore = -1000;
	max_rensa = 0;
	FirstOperation = 1;
	for (int i = 0; i < GameWidth; i++)for (int j = 0; j < GameHeight; j++) {
		Board[i][j] = signed char(PuyoColor::none);
	}
}
void GameState::init() {
	MaxScore = -1000;
	max_rensa = 0;
}

int GameState::OperationAndValueState(int OperationNumber,const std::pair<signed char, signed char>& PairPuyo,bool Flag) {//�����̔Ֆʂ̕]���֐�

	int score = 0;
	
	if (Flag)FirstOperation = OperationNumber;
	int dir = OperationNumber % 4;
	int xSetPos = OperationNumber / 4;
	//�w�肳�ꂽ�Ղ��u���A�Ղ悪������Ώ����A�A��������Ε]���_��������B
	//�������A���݂͘A����������������(8�A���ȏ�)�̂ŁA2^9�A���̎��̓y�i���e�B������(���̕]���_)
	

	PutPairPuyo(xSetPos, dir, PairPuyo);

	int rensa =  RensaSimulation();
	max_rensa = max(rensa, max_rensa);
	score += 1000 * max(rensa-2, 0);
	
	int StateHeight[6];
	int HeightAve = 0;
	for(int xPos = 0;xPos<GameWidth;xPos++){
		for (int height = 0; height < GameHeight; height++) {
			StateHeight[xPos] = height;
			if (Board[xPos][GameHeight - 1 - height] == -1) {
				break;
			}
		}
		HeightAve += StateHeight[xPos];
	}
	HeightAve /= GameWidth;
	int hosei[6] = {1,0,-1,-1,0,1};
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		score -= 60 * abs(hosei[xPos] * 2 + HeightAve - StateHeight[xPos]);//1 + 
	}
	if (Board[2][0] != -1) {
		score -= 1000000;
	}
	//�Ղ��u������̔Ֆʂ�]������B
	//�e���1�R�܂���2�R�̂Ղ���u���A�A�����N����ꍇ�͘A���������߁A�]���_�̍ő�l��������
	//memo:�Ƃ肠����2���F�̂Ղ���^�e�ɒu�����Ƃɂ���
	/*
	signed char save[6][12];

	memcpy(save, Board, sizeof(Board));//caution
	int MaxRensa = 0;
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		for (signed char PuyoKind = 0; PuyoKind < 4; PuyoKind++) {

			std::pair<signed char, signed char> NextPairPuyo = std::make_pair(PuyoKind, PuyoKind);
			PutPairPuyo(xPos, 1, NextPairPuyo);//���F�Ղ��u��
			MaxRensa = max(MaxRensa, RensaSimulation());

			memcpy(Board, save, sizeof(save));//�O�̏�Ԃɖ߂�
		}
	}
	score += 1000 * MaxRensa;//���ۂ̘A���{�[�i�X��菭���Ⴍ���Ă���
	*/
	MaxScore = max(MaxScore,score);
	return score;
}
//�N�̏ꍇ���o�̓t�F�[�Y�͗ǂ��������ǖ��炩�ɒT���ł܂������Ƃ�����Ă�����
//���݂̔ՖʂɂՂ��1�R�u��(PutsPuyo����Ăяo��)
bool GameState::PutPuyo(int xPos, signed char col) {
	
	for (int yPos = GameHeight-1; yPos >= 0; yPos--) {
		if (Board[xPos][yPos] == signed char(PuyoColor::none)) {
		Board[xPos][yPos] = col;//bug?
		return true;
		}
	}
	
	return false;
}

//�g�Ղ��u�� pair.first::�g�Ղ�̂�����̂Ղ�@second::���̂Ղ�
void GameState::PutPairPuyo(int xPos, int dir,const  std::pair<signed char, signed char>& PairPuyo) {
	
	if (dir != 2) {
		PutPuyo(xPos, PairPuyo.second);
		switch (dir) {
		case 1:
			PutPuyo(xPos, PairPuyo.first);
			break;
		case 0:
			PutPuyo(xPos - 1, PairPuyo.first);
			break;
		case 3:
			PutPuyo(xPos + 1, PairPuyo.first);
			break;
		}
	}
	else {
		PutPuyo(xPos, PairPuyo.first);
		PutPuyo(xPos, PairPuyo.second);
	}
	
}

//�Ղ悪4�ȏ゠�܂��ď�������ɁA�����Ă�Ղ��n�ʂɗ��Ƃ�����
bool GameState::DownPuyo() {
	bool DownFlag = false;
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		int yNonePos = -1;//PuyoColor::none�̂��̂̂����A���y���W���傫������ -1�Ȃ疳��
		for (int yPos = GameHeight - 1; yPos >= 0; --yPos) {
			if (Board[xPos][yPos] == signed char(PuyoColor::none) && yNonePos == -1) {
				yNonePos = yPos;
			}
			if (Board[xPos][yPos] != signed char(PuyoColor::none) && yNonePos != -1) {//�󔒂̏�ɂՂ悪����Ƃ�
				Board[xPos][yNonePos] = Board[xPos][yPos];
				Board[xPos][yPos] = signed char(PuyoColor::none);
				yNonePos--;
				DownFlag = true;
			}
		}
	}
	return DownFlag;
}

void GameState::debug() {
}


int GameState::Count(int x, int y) {
	int cnt = 0;
	signed char c = Board[x][y];
	Board[x][y] = -1; cnt++;
	if (x + 1 < GameWidth && Board[x + 1][y] == c) cnt += Count(x + 1, y);
	if (y + 1 < GameHeight && Board[x][y + 1] == c)cnt += Count(x, y + 1);
	if (x - 1 >= 0 && Board[x - 1][y] == c)cnt +=  Count(x - 1, y);
	if (y - 1 >= 0 && Board[x][y - 1] == c)cnt +=  Count(x, y - 1);

	Board[x][y] = c;
	return cnt;
}

int cnt = 0;
//�Ղ��u������ɌĂяo�����A���V���~���[�V�����֐��i�Ֆʂւ̕ύX�܃��j�@�Ԃ�l�F�A����
int GameState::RensaSimulation() {//TODO:�v�Z�ʂ̉��P
	int RensaNum = 0;
	bool RensaFlag = false;
	int dx[4] = { 1,-1,0, 0};
	int dy[4] = { 0, 0,1,-1 };

	do {
		RensaFlag = false;
		
		bool isVisited[GameWidth][GameHeight];
		for (int yPos = 0; yPos < GameHeight; ++yPos)
			for (int xPos = 0; xPos < GameWidth; xPos++)isVisited[xPos][yPos] = false;//������

		for (int yPos = 0; yPos < GameHeight; ++yPos)
			for (int xPos = 0; xPos < GameWidth; ++xPos) {
				if (isVisited[xPos][yPos])continue;//�T���ς݂Ȃ疳��
				else isVisited[xPos][yPos] = true;//�T��
				if (Board[xPos][yPos] == signed char(PuyoColor::none) || Board[xPos][yPos] == signed char(PuyoColor::jamma)) continue;//���ז��������͋󔒂̏ꍇ�͖���

				std::queue<std::pair<int, int>> que;
				std::vector<std::pair<int,int>> ErasePuyoPos;
				ErasePuyoPos.emplace_back(std::make_pair(xPos,yPos));
				que.push(std::make_pair(xPos, yPos));
				
				while (!que.empty()) {//bfs
					std::pair<int, int> Pos = que.front();
					que.pop();
					for (int idx = 0; idx < 4; ++idx) {
						int nxPos = Pos.first + dx[idx];
						int nyPos = Pos.second + dy[idx];
						if (nxPos < 0 || nxPos >= GameWidth || nyPos < 0 || nyPos >= GameHeight)continue;//�ՖʊO�ɍs������_��
						if (isVisited[nxPos][nyPos])continue;//�T���ς݂Ȃ�_��
						if (Board[nxPos][nyPos] != Board[Pos.first][Pos.second])continue;//�Ȃ����ĂȂ��ƃ_��
						//�Ȃ����Ă�̂Œǉ��A�T���ς݂ɂ���
						que.push(std::make_pair(nxPos,nyPos));
						ErasePuyoPos.emplace_back(std::make_pair(nxPos, nyPos));
						isVisited[nxPos][nyPos] = true;
					}
				}
				
				
				if (ErasePuyoPos.size() >= 4) {//4�ȏ�q�����Ă���A�A������

					for (auto Pos : ErasePuyoPos)Board[Pos.first][Pos.second] = -1;
					RensaFlag = true;
				}
				

			}

		if (RensaFlag) {
			RensaNum++;
			if (!DownPuyo()) {
				RensaFlag = false;
			}
		}
	} while (RensaFlag);
	
	return RensaNum;
}

bool GameState::operator<(const GameState& another) const
{
	return MaxScore < another.MaxScore;
};
