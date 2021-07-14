#include "myHeader.h"
#include "Class_GameState.h"
#include "Class_TimeController.h"

const int GameWidth = 6;
const int GameHeight = 12;

int nowtime;

GameState::GameState() {
	RenScore = 0;
	JudgeScore = 0;
	MaxScore = -1000;
	max_rensa = 0;
	FirstOperation = 1;
	CanFlag = true;
	for (int i = 0; i < GameWidth; i++)for (int j = 0; j < GameHeight; j++) {
		Board[i][j] = signed char(PuyoColor::none);
	}
}
void GameState::init() {
	CanFlag = true;
	RenScore = 0;
	MaxScore = -1000;
	max_rensa = 0;
}

int GameState::OperationAndValueState(int OperationNumber,const std::pair<signed char, signed char>& PairPuyo,bool Flag) {//�����̔Ֆʂ̕]���֐�
	if (!CanFlag) {
		TCHAR coldebug[50];
		_stprintf_s(coldebug, 50, TEXT("%d"), -2);
		//SetWindowText(hWnd, coldebug);
		return -10000;
	}
	RenScore = 0;
	int score = 0;
	
	if (Flag)FirstOperation = OperationNumber;
	int dir = OperationNumber % 4;
	int xSetPos = OperationNumber / 4;

	std::vector<std::pair<int, int>> PutPuyoPos = PutPairPuyo(xSetPos, dir, PairPuyo);

	int rensa =  RensaSimulationVer2(PutPuyoPos);
	max_rensa = max(rensa, max_rensa);

	if (Board[xSetPos][1] != -1) {
		//MaxScore = -10000;
		CanFlag = false;
		return -10000;
	}
	if (xSetPos == 0 && Board[1][1] != -1) {
		//MaxScore = -10000;
		CanFlag = false;
		return -10000;
	}
	if (xSetPos > 1 && Board[xSetPos - 1][1] != -1) {
		//MaxScore = -10000;
		CanFlag = false;
		return -10000;
	}
	
	int StateHeight[6];
	int HeightAve = 0;
	for(int xPos = 0;xPos<GameWidth;xPos++){
		for (int height = 0; height < GameHeight; height++) {
			StateHeight[xPos] = height;
			if (Board[xPos][GameHeight - 1 - height] == -1) {
				break;
			}
		}
		if (StateHeight[xPos] >= 10) {
			//MaxScore = -10000;
			CanFlag = false;
			return -10000;
		}
		HeightAve += StateHeight[xPos];
	}
	HeightAve /= GameWidth;
	int hosei[6] = {1,0,-1,-1,0,1};
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		score -= 90 * abs(hosei[xPos] * 2 + HeightAve - StateHeight[xPos]);
	}
	if (Board[2][1] != -1) {//�΂��񂫂�[�͉��
		score -= 10000;
		MaxScore = -10000;
		CanFlag = false;
		return -10000;
	}
	score += RenScore;

	//�Ղ��u������̔Ֆʂ�]������S�B
	//�e���1�R�܂���2�R�̂Ղ���u���A�A�����N����ꍇ�͘A���������߁A�]���_�̍ő�l��������
	//memo:�Ƃ肠����2���F�̂Ղ���^�e�ɒu�����Ƃɂ���
	
	signed char save[6][12];
	
	memcpy(save, Board, sizeof(Board));//caution
	int MaxRensa = 0;
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		int Candidate = PuyoCandidate(xPos);
		for (signed char PuyoKind = 0; PuyoKind < 4; PuyoKind++) {
			if (((Candidate>>PuyoKind)& 1) == 0)continue;
			//continue;
			std::vector<std::pair<int, int>> PutPuyoPos2 = PutPairPuyo(xPos, 1, std::make_pair(PuyoKind, PuyoKind));//���F2�R�Ղ��u��
			MaxRensa = max(MaxRensa, RensaSimulationVer2(PutPuyoPos2));

			memcpy(Board, save, sizeof(save));//�O�̏�Ԃɖ߂�
		}
	}
	score += 1500 * MaxRensa;//���ۂ̘A���{�[�i�X��菭���Ⴍ���Ă���
	
	JudgeScore = score;
	MaxScore = max(MaxScore,score);
	return rensa;
}

int GameState::PuyoCandidate(int xPos) {
	int dx[5] = { -1,1,-1,1,0};
	int dy[5] = { 0,0,-1,-1,1};
	int PuyoCount[4] = { 0,0,0,0 };
	int ret = 0;

	for (int yPos = GameHeight - 1; yPos >= 0; --yPos) {
		if (Board[xPos][yPos] == signed char(PuyoColor::none)) {
			for (int k = 0; k < 4; ++k) {
				int nxPos = xPos + dx[k];
				int nyPos = yPos + dy[k];
				if (nxPos < 0 || nxPos >= GameWidth || nyPos < 0 || nyPos >= GameHeight)continue;
				int P = Board[nxPos][nyPos];
				if(P>=0)++PuyoCount[P];
			}
			for (int k = 0; k < 4; ++k) {
				if (PuyoCount[k] >= 2)ret += (1 << k);
			}
			return ret;
		}
	}
	return ret;
}

//���݂̔ՖʂɂՂ��1�R�u��(PutsPuyo����Ăяo��)
std::pair<int,int> GameState::PutPuyo(int xPos, signed char col) {
	for (int yPos = GameHeight-1; yPos >= 0; yPos--) {
		if (Board[xPos][yPos] == signed char(PuyoColor::none)) {
		Board[xPos][yPos] = col;//bug?
		return std::make_pair(xPos,yPos);
		}
	}
	debug(true);
	return std::make_pair(0,0);
}
//�g�Ղ��u�� pair.first::�g�Ղ�̂�����̂Ղ�@second::���̂Ղ�
std::vector<std::pair<int,int>> GameState::PutPairPuyo(int xPos, int dir,const  std::pair<signed char, signed char>& PairPuyo) {
	std::vector<std::pair<int, int>> PutPuyoPos;

	if (dir != 2) {
		PutPuyoPos.emplace_back(PutPuyo(xPos, PairPuyo.second));
		switch (dir) {
		case 1:
			PutPuyoPos.emplace_back(PutPuyo(xPos, PairPuyo.first));
			break;
		case 0:
			PutPuyoPos.emplace_back(PutPuyo(xPos - 1, PairPuyo.first));
			break;
		case 3:
			PutPuyoPos.emplace_back(PutPuyo(xPos + 1, PairPuyo.first));
			break;
		}
	}
	else {
		PutPuyoPos.emplace_back(PutPuyo(xPos, PairPuyo.first));
		PutPuyoPos.emplace_back(PutPuyo(xPos, PairPuyo.second));
	}
	return PutPuyoPos;
}

//�Ղ悪4�ȏ゠�܂��ď�������ɁA�����Ă�Ղ��n�ʂɗ��Ƃ�����
bool GameState::DownPuyoAfterRensa() {
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

void GameState::debug(bool flag) {
	if (flag) {
		/*
		TCHAR coldebug[50];
		_stprintf_s(coldebug, 50, TEXT("%d"),-100);
		SetWindowText(hWnd, coldebug);
		*/
		nowtime = 0;

	}
	else {
		TCHAR coldebug[50];
		_stprintf_s(coldebug, 50, TEXT("%d"), nowtime);
		//_stprintf_s(coldebug, 50, TEXT("%d"), 100000 * (Board[0][11] + 2) + 10000 * (Board[0][10] + 2) + 1000 * (Board[0][9] + 2) + 100 * (Board[1][11] + 2) + 10 * (Board[1][10] + 2) + (Board[1][9] + 2));
		SetWindowText(hWnd, coldebug);
	}
}

bool GameState::RensaSearch(const std::pair<int,int>& Pos,bool isVisited[6][12],bool nextSearchPos[6]) {
	int xPos = Pos.first, yPos = Pos.second;
	if (isVisited[xPos][yPos])return false;//�T���ς݂Ȃ疳��
	else isVisited[xPos][yPos] = true;//�T��
	if (Board[xPos][yPos] == signed char(PuyoColor::none) || Board[xPos][yPos] == signed char(PuyoColor::jamma)) return false;//���ז��������͋󔒂̏ꍇ�͖���
	TimeController rensatime;
	
	std::queue<int> que;
	std::vector<int> ErasePuyoPos;
	ErasePuyoPos.reserve(80);
	ErasePuyoPos.emplace_back(xPos * GameHeight +  yPos);
	que.push(xPos * GameHeight + yPos);
	int dx[4] = { 1,-1,0, 0 };
	int dy[4] = { 0, 0,-1,1 };
	rensatime.Reset();
	
	while (!que.empty()) {
		
		int Pos = que.front();
		que.pop();
		for (int idx = 0; idx < 4; ++idx) {
			int nxPos = Pos / GameHeight + dx[idx];
			int nyPos = Pos % GameHeight + dy[idx];
			if (nxPos < 0 || nxPos >= GameWidth || nyPos < 0 || nyPos >= GameHeight)continue;//�ՖʊO�ɍs������_��
			if (isVisited[nxPos][nyPos])continue;//�T���ς݂Ȃ�_��
			if (Board[nxPos][nyPos] != Board[Pos / GameHeight][Pos % GameHeight])continue;//�Ȃ����ĂȂ��ƃ_��
			//�Ȃ����Ă�̂Œǉ��A�T���ς݂ɂ���
			que.push(nxPos * GameHeight + nyPos);
			ErasePuyoPos.emplace_back(nxPos * GameHeight + nyPos);
			isVisited[nxPos][nyPos] = true;
		}
	}


	if (ErasePuyoPos.size() > 1) {
		int EraseNum = ErasePuyoPos.size();
		if (EraseNum >= 4) {//4�ȏ�q�����Ă���A�A������
			for (auto EPos : ErasePuyoPos) {
				Board[EPos/ GameHeight][EPos % GameHeight] = signed char(PuyoColor::none);
				nextSearchPos[EPos / GameHeight] = true;
			}
			//RenScore = 0;
			return true;
		}
		else if (EraseNum == 3)RenScore += 120;
		else if (EraseNum == 2)RenScore += 30;
	}
	return false;
}

int GameState::RensaSimulationVer2(std::vector<std::pair<int, int>> PutPuyoPos) {
	int RensaNum = 0;
	bool RensaFlag;
	int dx[4] = { 1,-1,0, 0 };
	int dy[4] = { 0, 0,-1,1 };
	bool nextSearchPos[GameWidth] = { false,false,false,false,false,false };

	
	
	do {
		RensaFlag = false;
		
		bool isVisited[GameWidth][GameHeight];
		for (int yPos = 0; yPos < GameHeight; ++yPos)
			for (int xPos = 0; xPos < GameWidth; ++xPos)isVisited[xPos][yPos] = false;//������
		
		if (RensaNum == 0) {//�A����������
			
			for (auto Pos : PutPuyoPos) {
				RensaFlag = RensaFlag || RensaSearch(Pos,isVisited,nextSearchPos) ;
			}
			
			nowtime++;
		}
		else {//�A��������
			bool SearchPos[GameWidth] = { false,false,false,false,false,false };
			for (int xPos = 0; xPos < GameWidth; ++xPos) {
				if (!nextSearchPos[xPos])continue;
				for (int yPos = 0; yPos < GameHeight; ++yPos) {
					RensaFlag = RensaFlag || RensaSearch(std::make_pair(xPos, yPos), isVisited, SearchPos);
				}
			}
			for (int xPos = 0; xPos < GameWidth; ++xPos)nextSearchPos[xPos] = SearchPos[xPos];
		}

		if (RensaFlag) {
			RensaNum++;
			if (!DownPuyoAfterRensa()) {
				RensaFlag = false;
			}
		}
	} while (RensaFlag);
	
	return RensaNum;
}

int cnt = 0;


bool GameState::operator<(const GameState& another) const
{
	return JudgeScore > another.JudgeScore;
}
