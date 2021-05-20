#include "myHeader.h"
#include "Class_GameState.h"

const int GameWidth = 6;
const int GameHeight = 12;

GameState::GameState() {
	StateScore = 0;
	for (int i = 0; i < GameWidth; i++)for (int j = 0; j < GameHeight; j++) {
		Board[i][j] = PuyoColor::none;
	}
}

int GameState::OperationAndValueState(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo) {//�����̔Ֆʂ̕]���֐�
	int score = 0;
	//�w�肳�ꂽ�Ղ��u���A�Ղ悪������Ώ����A�A��������Ε]���_��������B
	//�������A���݂͘A����������������(8�A���ȏ�)�̂ŁA2^9�A���̎��̓y�i���e�B������(���̕]���_)
	PutPairPuyo(xPos, dir, PairPuyo);
	score += 1500 * max(RensaSimulation() - 7, 0);//
	//�Ղ��u������̔Ֆʂ�]������B
	//�e���1�R�܂���2�R�̂Ղ���u���A�A�����N����ꍇ�͘A���������߁A�]���_�̍ő�l��������
	//memo:�Ƃ肠����2���F�̂Ղ���^�e�ɒu�����Ƃɂ���
	PuyoColor save[6][12];

	memcpy(save, Board, sizeof(Board));//caution
	int MaxRensa = 0;
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		for (int PuyoKind = 0; PuyoKind < 5; PuyoKind++) {

			std::pair<PuyoColor, PuyoColor> NextPairPuyo = std::make_pair(PuyoColor(PuyoKind), PuyoColor(PuyoKind));
			PutPairPuyo(xPos, 1, NextPairPuyo);//���F�Ղ��u��
			MaxRensa = max(MaxRensa, RensaSimulation());

			memcpy(Board, save, sizeof(save));//�O�̏�Ԃɖ߂�
		}
	}
	score += 1000 * MaxRensa;//���ۂ̘A���{�[�i�X��菭���Ⴍ���Ă���

	StateScore = score;
	return score;
}

//���݂̔ՖʂɂՂ��1�R�u��(PutsPuyo����Ăяo��)
bool GameState::PutPuyo(int xPos, PuyoColor col) {
	for (int yPos = 1; yPos < GameHeight; yPos++) {
		if (Board[xPos][yPos] != PuyoColor::none) {//�Ղ悪��������A���̏�Ɏw�肳�ꂽ�Ղ��u��
		Board[xPos][yPos] = col;
		return true;
		}
	}
	return false;
}

//�g�Ղ��u�� pair.first::�g�Ղ�̂�����̂Ղ�@second::���̂Ղ�
void GameState::PutPairPuyo(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo) {
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
		for (int yPos = GameHeight - 1; yPos >= 0; yPos--) {
			if (Board[xPos][yPos] == PuyoColor::none && yNonePos == -1) {
				yNonePos = yPos;
			}
			if (Board[xPos][yPos] != PuyoColor::none && yNonePos != -1) {//�󔒂̏�ɂՂ悪����Ƃ�
				Board[xPos][yNonePos] = Board[xPos][yPos];
				yNonePos--;
				DownFlag = true;
			}
		}
	}
	return DownFlag;
}

//�Ղ��u������ɌĂяo�����A���V���~���[�V�����֐��i�Ֆʂւ̕ύX�܃��j�@�Ԃ�l�F�A����
int GameState::RensaSimulation() {
	int RensaNum = 0;
	bool RensaFlag = false;
	int dx[4] = { 1,-1,0, 0};
	int dy[4] = { 0, 0,1,-1 };

	do {
		RensaFlag = false;
		bool isVisited[GameWidth][GameHeight];
		for (int yPos = 0; yPos < GameHeight; yPos++)
			for (int xPos = 0; xPos < GameWidth; xPos++)isVisited[xPos][yPos] = false;//������

		for (int yPos = 0; yPos < GameHeight; yPos++)
			for (int xPos = 0; xPos < GameWidth; xPos++) {
				if (isVisited[xPos][yPos])continue;//�T���ς݂Ȃ疳��
				else isVisited[xPos][yPos] = true;//�T��
				if (Board[xPos][yPos] == PuyoColor::none || Board[xPos][yPos] == PuyoColor::jamma) continue;//���ז��������͋󔒂̏ꍇ�͖���
				
				std::queue<std::pair<int, int>> que;
				std::vector<std::pair<int,int>> ErasePuyoPos;
				ErasePuyoPos.emplace_back(std::make_pair(xPos,yPos));
				que.push(std::make_pair(xPos, yPos));

				while (!que.empty()) {//bfs
					std::pair<int, int> Pos = que.front();
					que.pop();
					for (int idx = 0; idx < 4; idx++) {
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
					for (auto Pos : ErasePuyoPos)Board[Pos.first][Pos.second] = PuyoColor::none;
					RensaFlag = true;
				}
				
			}

		if (RensaFlag) {
			RensaNum++;
			DownPuyo();
		}
	} while (RensaFlag);
	return RensaNum++;
}
