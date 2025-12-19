#pragma once
#include <iostream>

enum CardState { HIDDEN, REVEALED, MATCHED };

struct MemoryCard {
	char symbol;
	CardState state;

	MemoryCard(char s = '*') : symbol(s), state(HIDDEN) {}
};