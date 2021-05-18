#pragma once

#include "myHeader.h"

class GameState {
private:
	PuyoColor Board[6][12];//î’ñ èÓïÒ
public:
	GameState();
	int ValueState(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo);
	bool DownPuyo(int xPos, PuyoColor col);
	void PutsPuyo(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo);
	int RensaSimulation();
};
