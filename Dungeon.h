#pragma once
#include <vector>
#include <stdexcept>
using namespace std;

// ----- 타일 -----
struct Tile {
    char glyph;        // ' # ': 벽, ' . ': 바닥, ' + ': 문
    bool walkable;
    //bool transparent;
    //bool discovered;
    //bool visible;

    Tile() : glyph('#'), walkable(false) {
    }
};

// ----- 직사각형(방 영역) -----
struct Rect { int x, y, w, h; };

// ----- 방 종류 -----
enum class RoomKind {
    Start,
    MiniGame,
    Boss
};

// ----- 방 메타데이터 -----
struct Room {
    Rect area;
    RoomKind kind;
    int miniGameId; // MiniGame 방일 때만 1..N, 그 외 -1

    Room() : area{ 0,0,0,0 }, kind(RoomKind::Start), miniGameId(-1) {}
    Room(Rect r, RoomKind k, int id = -1) : area(r), kind(k), miniGameId(id) {}
};

// ----- 맵 -----
class Map {
public:
    Map(int w, int h);

    // 크기
    inline int width()  const { return W; }
    inline int height() const { return H; }

    // 좌표 검사 / 접근
    bool in(int x, int y) const;
    Tile& at(int x, int y);
    const Tile& at(int x, int y) const;

    // 생성: 일자형 던전 (미니게임 방 N개 후 보스방)
    void generateLinearDungeon(int numMiniRooms = 5);

    // 방 정보 접근
    const vector<Room>& getRooms() const { return rooms; }

    void drawMap(const Map& m);

    // 맵 깜빡임 방지
    static void gotoxy(int x, int y);
    static void hideCursor();

private:
    int W, H;
    vector<Tile> tiles;
    vector<Room> rooms;

    // 타일 조작
    void fillWall();
    void carveRoom(const Rect& r);
    void carveHCorridor(int x1, int x2, int y);
    void carveVCorridor(int y1, int y2, int x);

    // 유틸
    inline int idx(int x, int y) const { return y * W + x; }
};

class Monster {
private:
    int X, Y;
    int roomId;
    string symbol;
    int minigameId;
    bool isActive;

public:
    Monster(int x, int y, int roomId, int minigameId, string symbol = "○")
    {
        X = x;
        Y = y;
        this->roomId = roomId;
        this->minigameId = minigameId;
        this->symbol = symbol;
        isActive = true;
    }

    void drawMonster();
    bool isNearPlayer(int px, int py);
    void interact(Map& dungeon);
    void startMiniGame();
    int getX() const { return X; }
    int getY() const { return Y; }
};