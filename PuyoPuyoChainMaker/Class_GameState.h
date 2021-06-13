#pragma once

#include "myHeader.h"

class GameState {
private:
	int RenScore;
	bool CanFlag;
	
public:
	signed char Board[6][12];//î’ñ èÓïÒ
	int max_rensa;
	int MaxScore;
	int FirstOperation;
	GameState();
	int Count(int x,int y);
	void debug();
	void init();
	int OperationAndValueState(int OperationNumber,const std::pair<signed char, signed char>& PairPuyo,bool Flag);
	bool PutPuyo(int xPos, signed char col);
	void PutPairPuyo(int xPos, int dir,const std::pair<signed char, signed char>& PairPuyo);
	int RensaSimulation();
	bool DownPuyo();
	bool operator< (const GameState& another)const;
};
