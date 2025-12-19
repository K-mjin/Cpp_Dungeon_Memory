#include "MemoryBoard.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <vector>

using namespace std;

MemoryBoard::MemoryBoard() {
	setup();
}

void MemoryBoard::setup()  {
	vector<char> symbols;

	for (char c = 'A'; c < 'A' + (SIZE * SIZE) / 2; ++c) {
		symbols.push_back(c);
		symbols.push_back(c);
	}

	srand(time(0));
	random_shuffle(symbols.begin(), symbols.end());

	cardBoard.clear();
	int idx = 0;
	for (int i = 0; i < SIZE; ++i) {
		vector<MemoryCard> row;
		for (int j = 0; j < SIZE; ++j) {
			row.push_back(MemoryCard(symbols[idx++]));
		}
		cardBoard.push_back(row);
	}
}

void MemoryBoard::print() const {
	cout << "  ";
	for (int j = 0; j < SIZE; ++j)
		cout << " ";
	cout << endl;

	for (int i = 0; i < SIZE; ++i) {
		cout << " ";
		for (int j = 0; j < SIZE; ++j) {
			if (cardBoard[i][j].state == HIDDEN)
				cout << "* ";
			else
				cout << cardBoard[i][j].symbol << " ";
		}
		cout << endl;
	}
}

bool MemoryBoard::allMatched() const {
	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; j++)
			if (cardBoard[i][j].state != MATCHED)
				return false;
	return true;
}

bool MemoryBoard::isHidden(int x, int y) const {
	return cardBoard[x][y].state == HIDDEN;
}

void MemoryBoard::reveal(int x, int y) {
	cardBoard[x][y].state = REVEALED;
}

void MemoryBoard::hide(int x, int y) {
	cardBoard[x][y].state = HIDDEN;
}

void MemoryBoard::match(int x, int y) {
	cardBoard[x][y].state = MATCHED;
}

char MemoryBoard::getSymbol(int x, int y) const {
	return cardBoard[x][y].symbol;
}