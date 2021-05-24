#pragma once

#include "myHeader.h"

class GameState {
private:
	signed char Board[6][12];//�Ֆʏ��
	int max_rensa;
	
public:
	int StateScore;
	int FirstOperation;
	GameState();
	int Count(int x,int y);
	int OperationAndValueState(int OperationNumber,const std::pair<signed char, signed char>& PairPuyo,bool Flag);
	bool PutPuyo(int xPos, signed char col);
	void PutPairPuyo(int xPos, int dir,const std::pair<signed char, signed char>& PairPuyo);
	int RensaSimulation();
	bool DownPuyo();
	bool operator< (const GameState& another)const;
};
