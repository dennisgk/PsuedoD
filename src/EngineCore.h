#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#define GAME_SKIP_X 8 // must be able to divide 320 by this number to get whole number
#define HALF_GAME_SKIP_X 4.0f
#define SKIP_RENDER_COLOR 88

#define DEFAULT_HIGHSCORE_TIME 0.0f
#define DEFAULT_HIGHSCORE_ENKILLED 0

typedef struct {
    float posX;
    float posY;

    float dirX;
    float dirY;
    float angleDir;

    float planeX;
    float planeY;

    int gunX;
    int gunOrigY;
    int gunY;

    float lastShotTime;
    float gunAnimTime;
    char isMoving;

    short health;
} Player;

typedef struct{
    float enPosX;
    float enPosY;

    float rayToEnX;
    float rayToEnY;
    float angleToEn;

    int leftestPoint;
    int rightestPoint;

    short health;
    float lastAttackTime;

    float pWall; //the distance to the closest wall in the same direction
    float rpWall; //the real distance from the player
    int rX; //the real x value where the enemy should be drawn
    int wX; //the x where the distance must be captured
} Enemy;

typedef struct{
    char* data;
    uint16_t sz;
} Data_Hold;

typedef struct{
    int mapHeight;
    int mapWidth;

    unsigned char* worldMap;
    unsigned char* mapColorBindings;

    Enemy** enemies;
    int numEnemies;
} Map;

typedef struct{
    float surviveTime;
    int enemiesKilled;
} ScoreInfo;

//must be 16 or greater
unsigned char map1ColorBindings[6][2] = {
    {0x00, 0x00}, //this is for zero --> nothing goes here
    {0x47, 0x43}, //this is for one --> lighter color then darker shadow color
    {0xE9, 0xE5},
    {0xB6, 0xB2},
    {0x67, 0x63},
    {0x3D, 0x3A}
};

unsigned char map1WorldMap[24][24]=
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void InitializeMap1(Map* map);
void InitializeEnemy1(Enemy* e, float posX, float posY);

void InitializePlayer(Player* p, float pX, float pY);

void MovePlayerForwards(Player* p, Map* map, float moveSpeed);
void MovePlayerBackwards(Player* p, Map* map, float moveSpeed);

void RotatePlayerLeft(Player* p, float rotSpeed);
void RotatePlayerRight(Player* p, float rotSpeed);

void RenderPlayerInMap(Player* p, Map* map);

void AddEnemyToMap(Map* map, Enemy* enemy);
void RemoveEnemyFromMap(Map* map, Enemy* enemy);

void PlayerShoot(Player* p, Map* map, int* enemiesKilled);
void DrawPlayerWeapon(char state, int xpos, int ypos);

void RandomMapPosition(Map* map, float* x, float* y);
void SimpleEnemyAI(Player* p, Map* map, Enemy* enemy, float moveSpeed, float delta, float elapsedTime);
void DrawPlayerHealth(Player* p);

float getElapsed();
void GameLoop(float* surviveTime, int* enemiesKilled);

#endif