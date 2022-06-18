#include <tice.h>
#include <keypadc.h>
#include <string.h>
#include <stdlib.h>
#include <graphx.h>

#include "EngineCore.h"
#include "QuickMath.h"
#include "gfx/gfx.h"

void InitializeMap1(Map* map){
    map->mapHeight = 24;
    map->mapWidth = 24;

    map->worldMap = (unsigned char*)map1WorldMap;
    map->mapColorBindings = (unsigned char*)map1ColorBindings;

    map->numEnemies = 0;
    map->enemies = NULL;
}

void InitializeEnemy1(Enemy* e, float posX, float posY){
    e->enPosX = posX;
    e->enPosY = posY;

    e->leftestPoint = 0;
    e->rightestPoint = 0;

    e->health = 100;
    e->lastAttackTime = 0.0f;
}

void InitializePlayer(Player* p, float pX, float pY){
    p->posX = pX;
    p->posY = pY;

    p->dirX = -1.0f;
    p->dirY = 0.0f;
    p->angleDir = PI;

    p->planeX = 0.0f;
    p->planeY = 0.66f;   

    p->health = 320;

    p->gunX = (LCD_WIDTH / 2) - 40;
    p->gunOrigY = (LCD_HEIGHT) - 80;
    p->gunY = p->gunOrigY;
    p->lastShotTime = -10.0f;
    p->gunAnimTime = 0.0f;
    p->isMoving = 0;
}

//MUST FIX NEXT TO SO EVEN IF PAST MOVES TO NEXT TO WALL
//SOMETIMES IT CANNOT GO BACK OR FORWARD ALL THE WAY BECAUSE DELTA IS TOO LONG, THEREFORE THE DISTANCE IS TOO BIG
void MovePlayerForwards(Player* p, Map* map, float moveSpeed){
    if(map->worldMap[(int)(p->posX + (p->dirX * moveSpeed)) * map->mapHeight + (int)(p->posY)] == 0){
        p->posX += p->dirX * moveSpeed;
    }
    else{

    }
    if(map->worldMap[(int)(p->posX) * map->mapHeight + (int)(p->posY + (p->dirY * moveSpeed))] == 0){
        p->posY += p->dirY * moveSpeed;
    }
    else{
        
    }
}

void MovePlayerBackwards(Player* p, Map* map, float moveSpeed){
    if(map->worldMap[(int)(p->posX - (p->dirX * moveSpeed)) * map->mapHeight + (int)(p->posY)] == 0) p->posX -= p->dirX * moveSpeed;
    if(map->worldMap[(int)(p->posX) * map->mapHeight + (int)(p->posY - (p->dirY * moveSpeed))] == 0) p->posY -= p->dirY * moveSpeed;
}

void RotatePlayerLeft(Player* p, float rotSpeed){
    float cosRotSpeed = cosf_s(rotSpeed);
    float sinRotSpeed = sinf_s(rotSpeed);
    //both camera direction and camera plane must be rotated
    float oldDirX = p->dirX;
    p->dirX = p->dirX * cosRotSpeed - p->dirY * sinRotSpeed;
    p->dirY = oldDirX * sinRotSpeed + p->dirY * cosRotSpeed;
    float oldPlaneX = p->planeX;
    p->planeX = p->planeX * cosRotSpeed - p->planeY * sinRotSpeed;
    p->planeY = oldPlaneX * sinRotSpeed + p->planeY * cosRotSpeed;

    if(p->dirY > 0.0f){
        p->angleDir = arccosf_s(p->dirX);
    }
    else{
        p->angleDir = negf_s(arccosf_s(p->dirX));
    }
}

void RotatePlayerRight(Player* p, float rotSpeed){
    //both camera direction and camera plane must be rotated
    float cosNegRotSpeed = cosf_s(negf_s(rotSpeed));
    float sinNegRotSpeed = sinf_s(negf_s(rotSpeed));
    float oldDirX = p->dirX;
    p->dirX = p->dirX * cosNegRotSpeed - p->dirY * sinNegRotSpeed;
    p->dirY = oldDirX * sinNegRotSpeed + p->dirY * cosNegRotSpeed;
    float oldPlaneX = p->planeX;
    p->planeX = p->planeX * cosNegRotSpeed - p->planeY * sinNegRotSpeed;
    p->planeY = oldPlaneX * sinNegRotSpeed + p->planeY * cosNegRotSpeed;

    if(p->dirY > 0.0f){
        p->angleDir = arccosf_s(p->dirX);
    }
    else{
        p->angleDir = negf_s(arccosf_s(p->dirX));
    }
}

void RemoveEnemyFromMap(Map* map, Enemy* enemy){
    if(map->numEnemies > 0){
        Enemy** oldEnemies = map->enemies;

        int numEnemies = 0;

        if(oldEnemies){
            for(int i = 0; i < map->numEnemies; i++){
                if(oldEnemies[i] != enemy){
                    numEnemies++;
                }
            }
        }

        Enemy** newEnemies = malloc(sizeof(Enemy*) * numEnemies);

        if(oldEnemies){
            int eIt = 0;
            for(int i = 0; i < map->numEnemies; i++){
                if(oldEnemies[i] != enemy){
                    newEnemies[eIt] = oldEnemies[i];
                    eIt++;
                }
            }
            free(oldEnemies);
        }

        map->numEnemies--;
        map->enemies = newEnemies;
        free(enemy);
    }

}

void AddEnemyToMap(Map* map, Enemy* enemy){
    Enemy** oldEnemies = map->enemies;
    map->enemies = malloc(sizeof(Enemy*) * (map->numEnemies + 1));

    if(oldEnemies){
        for(int i = 0; i < map->numEnemies; i++){
            map->enemies[i] = oldEnemies[i];
        }
        free(oldEnemies);
    }

    map->numEnemies++;
    
    map->enemies[map->numEnemies - 1] = enemy;
}

void RenderPlayerInMap(Player* p, Map* map){

    float tmpRayDirXL = p->dirX + negf_s(p->planeX);
    float tmpRayDirYL = p->dirY + negf_s(p->planeY);
    float tmpRayDirXR = p->dirX + p->planeX;
    float tmpRayDirYR = p->dirY + p->planeY;

    for(int i = 0; i < map->numEnemies; i++){
        
        map->enemies[i]->rayToEnX = map->enemies[i]->enPosX - p->posX;
        map->enemies[i]->rayToEnY = map->enemies[i]->enPosY - p->posY;
        float befMult = (map->enemies[i]->rayToEnX*map->enemies[i]->rayToEnX)+(map->enemies[i]->rayToEnY*map->enemies[i]->rayToEnY);
        float ls = isqrt_f(befMult);
        map->enemies[i]->rayToEnX *= ls;
        map->enemies[i]->rayToEnY *= ls;

        if(((map->enemies[i]->rayToEnX * tmpRayDirXL) + (map->enemies[i]->rayToEnY * tmpRayDirYL)) > 0.0f && ((map->enemies[i]->rayToEnX * tmpRayDirXR) + (map->enemies[i]->rayToEnY * tmpRayDirYR)) > 0.0f){

            //DEAR GOD JUST DONT ASK ABOUT THIS PART
            char e = 0;
            if(absf_s(map->enemies[i]->rayToEnX) > absf_s(map->enemies[i]->rayToEnY)){
                e = 1;
            }
            if(e == 1){
                if(map->enemies[i]->rayToEnX >= 0.0f){
                    map->enemies[i]->angleToEn = arcsinf_s(map->enemies[i]->rayToEnY);
                }
                else{
                    map->enemies[i]->angleToEn = negf_s(arcsinf_s(map->enemies[i]->rayToEnY));
                }
            }
            else{
                if(map->enemies[i]->rayToEnY >= 0.0f){
                    //map->enemies[i]->angleToEn = arcsinf_s(map->enemies[i]->rayToEnY);
                    map->enemies[i]->angleToEn = arccosf_s(map->enemies[i]->rayToEnX);
                }
                else{
                    //map->enemies[i]->angleToEn = arcsinf_s(map->enemies[i]->rayToEnY);
                    map->enemies[i]->angleToEn = negf_s(arccosf_s(map->enemies[i]->rayToEnX));
                }
            }
            //JUST KNOW THAT map->enemies[i]->angleToEn NOW HOLDS THE ANGLE FROM THE RAY
            

            float angleDiff = toTrigBounds(p->angleDir - map->enemies[i]->angleToEn);
            float oppositeLengDist = tanf_s(angleDiff);
            map->enemies[i]->rpWall = cosf_s(angleDiff) * sqrt_f(befMult);

            float toXCoord = ((oppositeLengDist + 0.66f) * 0.7575f) * fLCD_WIDTH;
            map->enemies[i]->rX = (int)toXCoord;
            map->enemies[i]->wX = map->enemies[i]->rX - (map->enemies[i]->rX % GAME_SKIP_X);
        }
        else{
            map->enemies[i]->rX = -1;
            map->enemies[i]->wX = -1;
        }

    }

    for(int x = 0; x < LCD_WIDTH; x+=GAME_SKIP_X){
        float realtmpX = (float)x + HALF_GAME_SKIP_X;

        //calculate ray position and direction
        float cameraX = 2.0f * realtmpX / fLCD_WIDTH - 1.0f; //x-coordinate in camera space goes from -1 to 1
        float rayDirX = p->dirX + p->planeX * cameraX; // actual ray in non-euclidean calculation
        float rayDirY = p->dirY + p->planeY * cameraX;

        //which box of the map we're in
        int mapX = (int)(p->posX);
        int mapY = (int)(p->posY);

        //length of ray from current position to next x or y-side
        float sideDistX;
        float sideDistY;

        float deltaDistX = (absf_s(rayDirX) < 0.005) ? 500.0f : absf_s(1.0f / rayDirX);
        float deltaDistY = (absf_s(rayDirY) < 0.005) ? 500.0f : absf_s(1.0f / rayDirY);

        float perpWallDist;

        int stepX;
        int stepY;

        unsigned char hit = 0; //was there a wall hit?
        int side; //was a NS or a EW wall hit?
        //calculate step and initial sideDist
        if(rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (p->posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - p->posX) * deltaDistX;
        }
        if(rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (p->posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - p->posY) * deltaDistY;
        }        

        while(hit == 0)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if(sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            //Check if ray has hit a wall
            hit = map->worldMap[mapX * map->mapHeight + mapY];
        }

        if(side == 0) perpWallDist = (sideDistX - deltaDistX); //depending on whether the xdelt or ydelt was bigger
        else          perpWallDist = (sideDistY - deltaDistY);

        for(int i = 0; i < map->numEnemies; i++){
            if(map->enemies[i]->wX == x){
                map->enemies[i]->pWall = perpWallDist;
            }
        }

        int lineHeight = (int)(fLCD_HEIGHT / perpWallDist);

        int drawStart = ((-1 * lineHeight) / 2) + (LCD_HEIGHT / 2);
        if(drawStart < 0) drawStart = 0;
        int drawEnd = (lineHeight / 2) + (LCD_HEIGHT / 2);
        if(drawEnd >= LCD_HEIGHT) drawEnd = LCD_HEIGHT - 1;

        unsigned char color = 0;
        if(side == 0){
            color = map->mapColorBindings[hit * 2];
        }
        else{
            color = map->mapColorBindings[hit * 2 + 1];
        }

        gfx_SetColor(color);
        gfx_FillRectangle_NoClip(x, drawStart, GAME_SKIP_X, (drawEnd - drawStart));

    }

    for(int i = 0; i < map->numEnemies; i++){
        if(map->enemies[i]->rX > 0 && map->enemies[i]->rX < LCD_WIDTH && map->enemies[i]->rpWall < map->enemies[i]->pWall){
            int lineHeight = (int)(fLCD_HEIGHT / map->enemies[i]->rpWall);
            int lineWidth = lineHeight / 2;
            lineWidth = lineWidth + (lineWidth % 2);

            int lineLeft = map->enemies[i]->rX - (lineWidth / 2);
            int lineTop = (LCD_HEIGHT / 2) - (lineHeight / 2);

            map->enemies[i]->leftestPoint = lineLeft;
            map->enemies[i]->rightestPoint = lineLeft + lineWidth;

            int midX = lineLeft + (lineWidth / 2);
            int midY = lineTop + (lineHeight / 2);

            gfx_SetColor(0x53);
            gfx_FillRectangle(lineLeft, midY - (lineHeight / 4), lineWidth, lineHeight * 3 / 4);
            gfx_SetColor(0x1F);
            gfx_FillCircle(midX, midY - (lineHeight / 4), lineWidth / 2);
            gfx_SetColor(0x00);
            gfx_FillCircle(midX - (lineWidth / 3), midY - (lineHeight / 4), lineWidth / 6);
            gfx_FillCircle(midX + (lineWidth / 3), midY - (lineHeight / 4), lineWidth / 6);


        }
    }


}

void PlayerShoot(Player* p, Map* map, int* enemiesKilled){
    static int middleScreen = LCD_WIDTH / 2;

    for(int i = 0; i < map->numEnemies; i++){
        if(map->enemies[i]->rX > 0 && map->enemies[i]->rX < LCD_WIDTH && map->enemies[i]->rpWall < map->enemies[i]->pWall){
            float aimHelp = (map->enemies[i]->rightestPoint - map->enemies[i]->leftestPoint);
            if(aimHelp < 10){
                aimHelp /= 2;
            }
            else{
                aimHelp /= 3;
            }

            if(map->enemies[i]->leftestPoint - aimHelp <= middleScreen && map->enemies[i]->rightestPoint + aimHelp >= middleScreen){
                map->enemies[i]->health -= 50;
                if(map->enemies[i]->health <= 0){
                    RemoveEnemyFromMap(map, map->enemies[i]);
                    *enemiesKilled += 1;
                }
            }
        }

    }
}


void DrawPlayerWeapon(char state, int xpos, int ypos){
    if(state == 0){
        gfx_ScaledTransparentSprite_NoClip(g1, xpos, ypos, 4, 4);
    }
    else if(state == 1){
        gfx_ScaledTransparentSprite_NoClip(g3, xpos, ypos, 4, 4);
    }
    else if(state == 2){
        gfx_ScaledTransparentSprite_NoClip(g2, xpos, ypos, 4, 4);
    }
    
}

void RandomMapPosition(Map* map, float* x, float* y){

    float retX = 0.0f;
    float retY = 0.0f;
    while(map->worldMap[(int)(retX) * map->mapHeight + (int)(retY)] != 0){
        uint32_t rand = random();
        unsigned short rand2 = *((unsigned short*)(&rand));
        rand2 = 1 + (rand2 % (map->mapWidth - 2));

        unsigned short rand3 = *((unsigned short*)(&rand) + 1);
        rand3 = 1 + (rand3 % (map->mapHeight - 2));

        float frand2 = (float)rand2;
        float frand3 = (float)rand3;
        retX = ((float)frand2) + ((float)rand3 / (float)(map->mapHeight));
        retY = ((float)frand3) + ((float)rand2 / (float)(map->mapWidth));
    }
    *x = retX;
    *y = retY;


}


void SimpleEnemyAI(Player* p, Map* map, Enemy* enemy, float moveSpeed, float delta, float elapsedTime){
    float dirX = p->posX - enemy->enPosX;
    float dirY = p->posY - enemy->enPosY;
    float befsqr = (dirX*dirX)+(dirY*dirY);
    float dist = sqrt_f(befsqr);
    if(dist > 0.4f){
        float ls = isqrt_f(befsqr);
        dirX *= ls;
        dirY *= ls;
        
        float newPosX = enemy->enPosX + (dirX * moveSpeed * delta);
        float newPosY = enemy->enPosY + (dirY * moveSpeed * delta);

        if(map->worldMap[(int)(newPosX) * map->mapHeight + (int)(enemy->enPosY)] == 0){
            enemy->enPosX = newPosX;
        }
        if(map->worldMap[(int)(enemy->enPosX) * map->mapHeight + (int)(newPosY)] == 0){
            enemy->enPosY = newPosY;
        }
    }
    else{
        if(elapsedTime - enemy->lastAttackTime > 4.0f){ 
            enemy->lastAttackTime = elapsedTime;
            p->health -= 20;
        }
    }


}

void DrawPlayerHealth(Player* p){
    gfx_SetColor(0xD8);
    gfx_FillRectangle_NoClip(0, 0, p->health, 15);
}

float getElapsed(){
    return (float)timer_GetSafe(1, TIMER_UP) / 32768;
}

void GameLoop(float* surviveTime, int* enemiesKilled){
    //DISABLE TIMER THEN SET IT
    timer_Disable(1);
    timer_Set(1, 0);
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_UP);
    
    //DELTA STUFF FOR RENDER LOOP
    float lastElapsed = 0.0f;
    float delta = 0.0f;
    float elapsedTime = 0.0f;
    float spawnIncreasedSpeed = 0.0f;

    //DO LOAD OPERATIONS SUCH AS LOAD MAP
    Map map1 = {0};
    Player p = {0};
    int totalEnemiesDead = 0;

    //INITIALIZE MAP AND PLAYER
    InitializeMap1(&map1);
    InitializePlayer(&p, 22.0f, 12.0f);

    srand(rtc_Time());
    uint32_t rand = random();
    float nextSpawnTime = elapsedTime + 7.0f + (float)(*((unsigned char*)(&rand)) / 80);

    while(1){
        kb_Scan();

        if(kb_Data[6] & kb_Clear){
            break;
        }

        elapsedTime = getElapsed();
        delta = elapsedTime - lastElapsed;

        //ENEMY SPAWNING AND AI
        if(elapsedTime > nextSpawnTime){
            if(map1.numEnemies < 10){
                Enemy* enemy = malloc(sizeof(Enemy));
                memset(enemy, 0, sizeof(Enemy));

                float randX = 0.0f;
                float randY = 0.0f;
                RandomMapPosition(&map1, &randX, &randY);
                InitializeEnemy1(enemy, randX, randY);

                AddEnemyToMap(&map1, enemy);

                if(spawnIncreasedSpeed < 6.0f){
                    spawnIncreasedSpeed += 0.6f;
                }
            }

            rand = random();
            nextSpawnTime = elapsedTime + 7.0f - (spawnIncreasedSpeed) + (float)(*((unsigned char*)(&rand)) / 80);
        }

        for(int i = 0; i < map1.numEnemies; i++){
            SimpleEnemyAI(&p, &map1, map1.enemies[i], 2.5f, delta, elapsedTime);
        }

        //DO PRE RENDER OPERATIONS LIKE CHANGING PLAYER DIRECTION BASED ON KEYPRESS
        if(kb_Data[7] & kb_Up){
            MovePlayerForwards(&p, &map1, 5.0f * delta);
            p.isMoving = 1;
		}
		if(kb_Data[7] & kb_Down){
            MovePlayerBackwards(&p, &map1, 5.0f * delta);
            p.isMoving = 1;
		}
		if(kb_Data[7] & kb_Left){
            RotatePlayerLeft(&p, 1.55f * delta);
            p.isMoving = 1;
		}
		if(kb_Data[7] & kb_Right){
            RotatePlayerRight(&p, 1.55f * delta);
            p.isMoving = 1;
		}

        if(kb_Data[1] & kb_2nd && elapsedTime - p.lastShotTime > 0.7f){ //1 second cooldown
            p.lastShotTime = elapsedTime;
            PlayerShoot(&p, &map1, &totalEnemiesDead);
        }

        gfx_ZeroScreen();

        //DO RENDER OPERATIONS
        RenderPlayerInMap(&p, &map1);

        if(elapsedTime - p.lastShotTime < 0.2f){
            //SHOW SHOOTING
            DrawPlayerWeapon(2, p.gunX, p.gunY);
        }
        else if(elapsedTime - p.lastShotTime < 0.6f){
            //SHOW END SHOT
            DrawPlayerWeapon(1, p.gunX, p.gunY);
        }
        else{
            //MOVE WEAPON AND IF MOVING MOVE MORE BASED OFF GUNANIMTIME...
            if(p.isMoving){
                p.gunAnimTime += delta * 2.0f;
            }
            else{
                p.gunAnimTime += delta * 1.5f;
            }

            p.gunY = p.gunOrigY + 5 + (int)(sinf_sc(&p.gunAnimTime) * 5.5f);

            //SHOW IDLE
            DrawPlayerWeapon(0, p.gunX, p.gunY);
        }

        DrawPlayerHealth(&p);

        gfx_SwapDraw();

        lastElapsed = elapsedTime;
        p.isMoving = 0;

        if(p.health <= 0 || kb_Data[6] & kb_Clear){
            for(int i = 0; i < map1.numEnemies; i++){
                RemoveEnemyFromMap(&map1, map1.enemies[i]);
            }
            *surviveTime = elapsedTime;
            *enemiesKilled = totalEnemiesDead;
            break;
        }
    }
}