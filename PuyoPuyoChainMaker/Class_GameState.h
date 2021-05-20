#pragma once

#include "myHeader.h"

class GameState {
private:
	PuyoColor Board[6][12];//î’ñ èÓïÒ
	int StateScore;
public:
	GameState();
	int OperationAndValueState(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo);
	bool PutPuyo(int xPos, PuyoColor col);
	void PutPairPuyo(int xPos, int dir, std::pair<PuyoColor, PuyoColor> PairPuyo);
	int RensaSimulation();
	bool DownPuyo();
};
