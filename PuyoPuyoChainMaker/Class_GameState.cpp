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
	//現在の状態から、nextをposに落とした状況をシュミレーションする
	return -1;
}

int GameState::ValueState() {//盤面の評価関数
	return 0;
}

bool GameState::DownPuyo(int xPos, PuyoColor col) {//現在の盤面にぷよを1コ置く(PutsPuyoから呼び出す)
	for (int yPos = 1; yPos < GameHeight; yPos++) {
		if (Board[xPos][yPos] != PuyoColor::none) {//ぷよがあったら、その上に指定されたぷよを置く
		Board[xPos][yPos] = col;
		return true;
		}
	}
	return false;
}

void GameState::PutsPuyo(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo) {//組ぷよを置く pair.first::組ぷよのうち上のぷよ　second::下のぷよ
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
	
