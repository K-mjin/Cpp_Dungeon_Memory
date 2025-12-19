#pragma once
#include<vector>
#include"MemoryCard.h"
using namespace std;

class MemoryBoard {
private:
	static const int SIZE = 4;
	vector<vector<MemoryCard>> cardBoard;

public:
	MemoryBoard();
	void setup() ;		// 보드 세팅
	void print() const;
	bool allMatched() const;
	bool isHidden(int x, int y) const;
	void reveal(int x, int y);
	void hide(int x, int y);
	void match(int x, int y);
	char getSymbol(int x, int y) const;
	int getSize() const {
		return SIZE;
	}
};
