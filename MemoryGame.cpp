#include"MemoryGame.h"
#include <iostream>
using namespace std;

MemoryGame::MemoryGame() : turnCount(0) {}

int MemoryGame::Coins() const {
	if (turnCount <= 15)
		return 8;
	else if (turnCount <= 20)
		return 5;
	else if (turnCount <= 30)
		return 2;
	return 0;
}

void MemoryGame::run() {
	cout << "카드 맞추기 게임을 시작합니다!\n";

	while (!board.allMatched()) {
		board.print();

		// 첫 카드 선택
		int pos1;
		cout << "첫 번째 카드 번호 입력 (1 ~ " << board.getSize() * board.getSize() << ") : ";
		cin >> pos1;

		// 번호 → (row, col)
		int x1 = (pos1 - 1) / board.getSize();
		int y1 = (pos1 - 1) % board.getSize();

		if (!board.isHidden(x1, y1)) {
			cout << "이미 뒤집혔습니다. 다시 시도하세요.\n";
			continue;
		}

		board.reveal(x1, y1);
		board.print();

		// 두 번째 카드 선택
		int pos2;
		cout << "두 번째 카드 번호 입력 (1 ~ " << board.getSize() * board.getSize() << ") : ";
		cin >> pos2;

		// 번호 → (row, col)
		int x2 = (pos2 - 1) / board.getSize();
		int y2 = (pos2 - 1) % board.getSize();

		if (!board.isHidden(x2, y2)) {
			cout << "이미 뒤집혔습니다. 다시 시도하세요.\n";
			continue;
		}

		board.reveal(x2, y2);
		board.print();

		// 카드 비교
		if (board.getSymbol(x1, y1) == board.getSymbol(x2, y2)) {
			cout << "정답입니다.\n";
			board.match(x1, y1);
			board.match(x2, y2);
		}
		else {
			cout << "틀렸습니다.\n";
			board.hide(x1, y1);
			board.hide(x2, y2);
		}
		turnCount++;
		cout << endl;
	}
	cout << "모든 카드를 맞췄습니다! 게임을 종료합니다!\n";
	cout << "총 " << turnCount << " 턴 걸렸습니다. 획득 코인 : " << Coins() << "개\n";
	cout << "엔터를 누르면 종료됩니다...";
	cin.ignore();
	cin.get();
}