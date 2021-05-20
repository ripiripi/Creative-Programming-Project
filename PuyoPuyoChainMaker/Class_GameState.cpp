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

int GameState::OperationAndValueState(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo) {//操作後の盤面の評価関数
	int score = 0;
	//指定されたぷよを置き、ぷよが消えれば消し、連鎖があれば評価点を加える。
	//ただし、現在は連鎖数を高くしたい(8連鎖以上)ので、2^9連鎖の時はペナルティをつける(負の評価点)
	PutPairPuyo(xPos, dir, PairPuyo);
	score += 1500 * max(RensaSimulation() - 7, 0);//
	//ぷよを置いた後の盤面を評価する。
	//各列に1コまたは2コのぷよ列を置き、連鎖が起こる場合は連鎖数を求め、評価点の最大値を加える
	//memo:とりあえず2個同色のぷよをタテに置くことにする
	PuyoColor save[6][12];

	memcpy(save, Board, sizeof(Board));//caution
	int MaxRensa = 0;
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		for (int PuyoKind = 0; PuyoKind < 5; PuyoKind++) {

			std::pair<PuyoColor, PuyoColor> NextPairPuyo = std::make_pair(PuyoColor(PuyoKind), PuyoColor(PuyoKind));
			PutPairPuyo(xPos, 1, NextPairPuyo);//同色ぷよを置く
			MaxRensa = max(MaxRensa, RensaSimulation());

			memcpy(Board, save, sizeof(save));//前の状態に戻す
		}
	}
	score += 1000 * MaxRensa;//実際の連鎖ボーナスより少し低くしている

	StateScore = score;
	return score;
}

//現在の盤面にぷよを1コ置く(PutsPuyoから呼び出す)
bool GameState::PutPuyo(int xPos, PuyoColor col) {
	for (int yPos = 1; yPos < GameHeight; yPos++) {
		if (Board[xPos][yPos] != PuyoColor::none) {//ぷよがあったら、その上に指定されたぷよを置く
		Board[xPos][yPos] = col;
		return true;
		}
	}
	return false;
}

//組ぷよを置く pair.first::組ぷよのうち上のぷよ　second::下のぷよ
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

//ぷよが4つ以上あつまって消えた後に、浮いてるぷよを地面に落とす処理
bool GameState::DownPuyo() {
	bool DownFlag = false;
	for (int xPos = 0; xPos < GameWidth; xPos++) {
		int yNonePos = -1;//PuyoColor::noneのもののうち、一番y座標が大きいもの -1なら無し
		for (int yPos = GameHeight - 1; yPos >= 0; yPos--) {
			if (Board[xPos][yPos] == PuyoColor::none && yNonePos == -1) {
				yNonePos = yPos;
			}
			if (Board[xPos][yPos] != PuyoColor::none && yNonePos != -1) {//空白の上にぷよがあるとき
				Board[xPos][yNonePos] = Board[xPos][yPos];
				yNonePos--;
				DownFlag = true;
			}
		}
	}
	return DownFlag;
}

//ぷよを置いた後に呼び出される連鎖シュミレーション関数（盤面への変更含ム）　返り値：連鎖数
int GameState::RensaSimulation() {
	int RensaNum = 0;
	bool RensaFlag = false;
	int dx[4] = { 1,-1,0, 0};
	int dy[4] = { 0, 0,1,-1 };

	do {
		RensaFlag = false;
		bool isVisited[GameWidth][GameHeight];
		for (int yPos = 0; yPos < GameHeight; yPos++)
			for (int xPos = 0; xPos < GameWidth; xPos++)isVisited[xPos][yPos] = false;//初期化

		for (int yPos = 0; yPos < GameHeight; yPos++)
			for (int xPos = 0; xPos < GameWidth; xPos++) {
				if (isVisited[xPos][yPos])continue;//探索済みなら無視
				else isVisited[xPos][yPos] = true;//探索
				if (Board[xPos][yPos] == PuyoColor::none || Board[xPos][yPos] == PuyoColor::jamma) continue;//お邪魔もしくは空白の場合は無視
				
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
						if (nxPos < 0 || nxPos >= GameWidth || nyPos < 0 || nyPos >= GameHeight)continue;//盤面外に行ったらダメ
						if (isVisited[nxPos][nyPos])continue;//探索済みならダメ
						if (Board[nxPos][nyPos] != Board[Pos.first][Pos.second])continue;//つながってないとダメ
						//つながってるので追加、探索済みにする
						que.push(std::make_pair(nxPos,nyPos));
						ErasePuyoPos.emplace_back(std::make_pair(nxPos, nyPos));
						isVisited[nxPos][nyPos] = true;
					}
				}
				if (ErasePuyoPos.size() >= 4) {//4つ以上繋がってたら、連鎖判定
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
