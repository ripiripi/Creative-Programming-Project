#pragma once

#include "myHeader.h"

class GameState {
private:
	PuyoColor Board[6][12];//î’ñ èÓïÒ
public:
	GameState();
	int PuyoDownSimulation(int next, int pos, bool flag);
	int ValueState();
	bool DownPuyo(int xPos, PuyoColor col);
	void PutsPuyo(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo);
};
