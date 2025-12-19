#include "Dungeon.h"
#include <algorithm> // std::swap
#include <windows.h> // gotoxy용
#include <iostream>
using namespace std;

void Map:: gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Map:: hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = false;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

Map::Map(int w, int h) : W(w), H(h), tiles(w* h) {
    if (w <= 0 || h <= 0) throw invalid_argument("Map size must be positive");
}

bool Map::in(int x, int y) const {
    return 0 <= x && x < W && 0 <= y && y < H;
}

Tile& Map::at(int x, int y) {
    return tiles[idx(x, y)];
}

const Tile& Map::at(int x, int y) const {
    return tiles[idx(x, y)];
}

void Map::fillWall() {
    for (auto& c : tiles) {
        c.glyph = ' ';          // 기본은 빈 공간
        c.walkable = false;
        //c.transparent = true;   // 공백은 시야 통과 가능하게 할 수도 있음
        //c.discovered = false;
        //c.visible = false;
    }
}

void Map::carveRoom(const Rect& r) {
    for (int y = r.y; y < r.y + r.h; ++y) {
        for (int x = r.x; x < r.x + r.w; ++x) {
            if (!in(x, y)) continue;
            Tile& c = at(x, y);

            // 테두리 → 벽 '#'
            if (y == r.y || y == r.y + r.h - 1 ||
                x == r.x || x == r.x + r.w - 1) {
                c.glyph = '#';
                c.walkable = false;
                //c.transparent = false;
            }
            // 내부 → 바닥 '.'
            else {
                c.glyph = '.';
                c.walkable = true;
                //c.transparent = true;
            }
        }
    }
}

void Map::carveHCorridor(int x1, int x2, int y) {
    if (x1 > x2) std::swap(x1, x2);
    for (int x = x1; x <= x2; ++x) {
        if (!in(x, y)) continue;

        // 가운데 길
        Tile& c = at(x, y);
        c.glyph = '.';
        c.walkable = true;
        //c.transparent = true;

        // 위쪽 벽
        if (in(x, y - 1)) {
            Tile& up = at(x, y - 1);
            if (up.glyph == ' ') { // 아직 빈 공간이면 벽으로 설정
                up.glyph = '#';
                up.walkable = false;
                //up.transparent = false;
            }
        }

        // 아래쪽 벽
        if (in(x, y + 1)) {
            Tile& down = at(x, y + 1);
            if (down.glyph == ' ') {
                down.glyph = '#';
                down.walkable = false;
                //down.transparent = false;
            }
        }
    }
}

void Map::generateLinearDungeon(int numMiniRooms) {
    if (numMiniRooms < 0) throw invalid_argument("numMiniRooms must be >= 0");

    fillWall();
    rooms.clear();

    // 레이아웃 파라미터
    const int roomW = 12;
    const int roomH = 9;
    const int spacing = 4;      // 방 사이 복도/공백
    const int bossW = 18;
    const int bossH = 11;

    // 맵 폭이 충분한지 체크 (대략적인 필요 폭 계산)
    // 시작방 1 + 미니방 N + 보스방 1, 사이사이 spacing
    const int neededWidth =
        2 + // 여유
        roomW * (1 + numMiniRooms) +
        bossW +
        spacing * (1 + numMiniRooms + 1);
    if (W < neededWidth || H < max(roomH, bossH) + 4) {
        throw std::runtime_error("Map is too small for the requested linear dungeon.");
    }

    // 기준 Y (수평 일자 배치)
    const int startX = 3;
    const int startY = H / 2 - roomH / 2;

    // 시작방
    Rect start = { startX, startY, roomW, roomH };
    carveRoom(start);
    rooms.emplace_back(start, RoomKind::Start, -1);

    int prevCX = start.x + start.w / 2;
    int prevCY = start.y + start.h / 2;

    // 미니게임 방들
    for (int i = 0; i < numMiniRooms; ++i) {
        int x = startX + (roomW + spacing) * (i + 1);
        Rect r{ x, startY, roomW, roomH };
        carveRoom(r);
        rooms.emplace_back(r, RoomKind::MiniGame, i + 1);

        // 이전 방 중심과 현재 방 중심을 복도로 연결
        int curCX = r.x + r.w / 2;
        int curCY = r.y + r.h / 2;
        carveHCorridor(prevCX, curCX, prevCY);

        // 문(+) : 현재 방의 왼쪽 쪽 중앙
        int doorX = r.x - spacing / 2;
        int doorY = r.y + r.h / 2;
        if (in(doorX, doorY)) {
            Tile& d = at(doorX, doorY);
            d.glyph = '+';
            d.walkable = true;
            //d.transparent = true;
        }

        prevCX = curCX;
        prevCY = curCY;
    }

    // 보스방
    const int bossX = startX + (roomW + spacing) * (numMiniRooms + 1);
    const int bossY = H / 2 - bossH / 2;
    Rect boss{ bossX, bossY, bossW, bossH };
    carveRoom(boss);
    rooms.push_back(Room(boss, RoomKind::Boss, -1));

    // 마지막 미니게임 방 중심에서 보스방 중심까지 복도
    int bossCX = boss.x + boss.w / 2;
    int bossCY = boss.y + boss.h / 2;
    carveHCorridor(prevCX, bossCX, prevCY);

    // 보스방 입구 문(+) : 보스방 왼쪽
    int bDoorX = boss.x - spacing / 2;
    int bDoorY = boss.y + boss.h / 2;
    if (in(bDoorX, bDoorY)) {
        Tile& d = at(bDoorX, bDoorY);
        d.glyph = '+';
        d.walkable = true;
        //d.transparent = true;
    }
}

// 맵 전체 출력 (처음 1회만)
 void Map::drawMap(const Map& m) 
 {
    gotoxy(0, 0);
    for (int y = 0; y < m.height(); ++y) {
        for (int x = 0; x < m.width(); ++x) {
            cout << m.at(x, y).glyph;
        }
        cout << '\n';
    }
    cout << "W,A,S,D 이동, Q or ESC 종료\n";
}

 // 몬스터 기능
 void Monster::drawMonster() 
 {
     Map::gotoxy(X, Y);
     cout << symbol;
 }

 // 플레이어 접근 여부 확인
 bool Monster::isNearPlayer(int px, int py) {
     return (abs(px - X) + abs(py - Y) == 1);
 }

 // 몬스터와 플레이어 상호작용
 void Monster::interact(Map& dungeon) {
     if (isActive) {
         Map::gotoxy(0, 21);
         cout << "스토리내용 ~~~ 미니게임 시작!\n";
         startMiniGame();
         isActive = false;  // 한번만 실행
         /*cout << string(40, ' ');
         Map::gotoxy(0, 21);*/

         dungeon.at(X, Y).walkable = true;
         Map::gotoxy(X, Y);
         cout << '.';
     }
 }

 void Monster::startMiniGame() {
     switch (minigameId) {
     case 1:
        //runMiniGame1();    // 미니게임 임의 이름 나중에 설정하셈
         break;
     case 2:
         //runMiniGame2();    // 미니게임 임의 이름 나중에 설정하셈
         break;
     case 3:
        // runMiniGame3();    // 미니게임 임의 이름 나중에 설정하셈
         break;
     case 4:
        // runMiniGame4();    // 미니게임 임의 이름 나중에 설정하셈
         break;
     case 5:
        // runMiniGame5();    // 미니게임 임의 이름 나중에 설정하셈
         break;
     }
 }