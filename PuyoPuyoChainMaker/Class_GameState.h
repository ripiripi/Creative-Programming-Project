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
	void debug();
	void init();
	int OperationAndValueState(int OperationNumber,const std::pair<signed char, signed char>& PairPuyo,bool Flag);
	std::pair<int,int> PutPuyo(int xPos, signed char col);
	std::vector<std::pair<int, int>> PutPairPuyo(int xPos, int dir,const std::pair<signed char, signed char>& PairPuyo);
	int RensaSimulation();
	int RensaSimulationVer2(std::vector<std::pair<int,int>> PutPuyoPos);
	bool RensaSearch(std::pair<int, int> Pos, bool isVisited[6][12],bool nextSearchPos[6]);
	bool DownPuyoAfterRensa();
	bool operator< (const GameState& another)const;
};
