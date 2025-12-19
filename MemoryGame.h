#pragma once
#include "MemoryBoard.h"

class MemoryGame {
private:
	MemoryBoard board;
	int turnCount;

public:
	MemoryGame();
	void run();
	int Coins() const;
};