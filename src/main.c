/*
 *--------------------------------------
 * Program Name: PseudoD
 * Author: den
 * License: ur mom
 * Description:
 *--------------------------------------
*/

#include <tice.h>
#include <keypadc.h>
#include <string.h>
#include <stdlib.h>
#include <graphx.h>
#include <fontlibc.h>
#include <fileioc.h>

#include "QuickMath.h"
#include "EngineCore.h"
#include "gfx/gfx.h"

char* CombineStrings(Data_Hold* strings, uint16_t numStrings){
    uint16_t retStrLeng = 0;

    for(uint16_t i = 0; i < numStrings; i++){
        for(uint16_t j = 0; j < strings[i].sz; j++){ //because of null character
            if(strings[i].data[j] != '\0'){
                retStrLeng++;
            }
        }
    }
    retStrLeng += 1;

    char* ret = malloc(sizeof(char) * retStrLeng);
    int it = 0;
    for(uint16_t i = 0; i < numStrings; i++){
        for(uint16_t j = 0; j < strings[i].sz; j++){ //because of null character
            if(strings[i].data[j] != '\0'){
                ret[it] = strings[i].data[j];
                it++;
            }
        }
    }
    ret[it] = '\0';
    return ret;
}

void float2str(float value, char* str){
    real_t tmp_real = os_FloatToReal(value);
    os_RealToStr(str, &tmp_real, 8, 1, 2);
}

void PrintScores(ScoreInfo timeHS, ScoreInfo killedHS){
    char realTimeTimeHSStr[14] = {0};
    char realTimeKilledHSStr[14] = {0};
    char realKilledTimeHSStr[14] = {0};
    char realKilledKilledHSStr[14] = {0};

    float2str(timeHS.surviveTime, realTimeTimeHSStr);
    float2str(killedHS.surviveTime, realTimeKilledHSStr);
    float2str((float)timeHS.enemiesKilled, realKilledTimeHSStr);
    float2str((float)killedHS.enemiesKilled, realKilledKilledHSStr);

    Data_Hold tHST11 = {0};
    tHST11.data = "Time Highscore: ";
    tHST11.sz = 17;
    Data_Hold tHST12 = {0};
    tHST12.data = realTimeTimeHSStr;
    tHST12.sz = 14;
    Data_Hold tHST13 = {0};
    tHST13.data = " seconds";
    tHST13.sz = 9;
    Data_Hold tHST21 = {0};
    tHST21.data = "with ";
    tHST21.sz = 6;
    Data_Hold tHST22 = {0};
    tHST22.data = realKilledTimeHSStr;
    tHST22.sz = 14;
    Data_Hold tHST23 = {0};
    tHST23.data = " enemies killed";
    tHST23.sz = 16;
    Data_Hold tHS1StrData[5] = {tHST11, tHST12, tHST13};
    Data_Hold tHS2StrData[5] = {tHST21, tHST22, tHST23};

    Data_Hold ekHST11 = {0};
    ekHST11.data = "Enemies Killed Highscore: ";
    ekHST11.sz = 27;
    Data_Hold ekHST12 = {0};
    ekHST12.data = realTimeKilledHSStr;
    ekHST12.sz = 14;
    Data_Hold ekHST13 = {0};
    ekHST13.data = " seconds";
    ekHST13.sz = 9;
    Data_Hold ekHST21 = {0};
    ekHST21.data = "with ";
    ekHST21.sz = 6;
    Data_Hold ekHST22 = {0};
    ekHST22.data = realKilledKilledHSStr;
    ekHST22.sz = 14;
    Data_Hold ekHST23 = {0};
    ekHST23.data = " enemies killed";
    ekHST23.sz = 16;
    Data_Hold ekHS1StrData[5] = {ekHST11, ekHST12, ekHST13};
    Data_Hold ekHS2StrData[5] = {ekHST21, ekHST22, ekHST23};

    char* tHS1Str = CombineStrings(tHS1StrData, 5);
    char* tHS2Str = CombineStrings(tHS2StrData, 5);
    char* ekHS1Str = CombineStrings(ekHS1StrData, 5);
    char* ekHS2Str = CombineStrings(ekHS2StrData, 5);
    
    gfx_PrintStringXY(tHS1Str, 20, 90);
    gfx_PrintStringXY(tHS2Str, 20, 100);
    gfx_PrintStringXY(ekHS1Str, 20, 130);
    gfx_PrintStringXY(ekHS2Str, 20, 140);

    free(tHS1Str);
    free(tHS2Str);
    free(ekHS1Str);
    free(ekHS2Str);
}

void MenuLoop(){

    ti_var_t scoreFile = ti_Open("PseudoDScores", "r");
    ScoreInfo defaults = {0};
    defaults.enemiesKilled = DEFAULT_HIGHSCORE_ENKILLED;
    defaults.surviveTime = DEFAULT_HIGHSCORE_TIME;
    if(scoreFile == 0){
        scoreFile = ti_Open("PseudoDScores", "w+");

        ti_Write((void*)&defaults, sizeof(ScoreInfo), 1, scoreFile);
        ti_Write((void*)&defaults, sizeof(ScoreInfo), 1, scoreFile);

        ti_SetArchiveStatus(true, scoreFile);
        ti_Close(scoreFile);

        scoreFile = ti_Open("PseudoDScores", "r");
    }
    ScoreInfo timeHS = {0};
    ScoreInfo killedHS = {0};
    ti_Read((void*)&timeHS, sizeof(ScoreInfo), 1, scoreFile);
    ti_Read((void*)&killedHS, sizeof(ScoreInfo), 1, scoreFile);

    ti_Close(scoreFile);

    gfx_ZeroScreen();
    gfx_SetTextFGColor(0x0F);
    gfx_PrintStringXY("Press 2nd to play PseudoD", 20, 50);
    gfx_PrintStringXY("Press Clear to Quit", 20, 70);

    PrintScores(timeHS, killedHS);

    gfx_SwapDraw();
    while(1){
        kb_Scan();
        if(kb_Data[6] & kb_Clear){
            break;
        }
        else if(kb_Data[1] & kb_2nd){
            float surviveTime = 0.0f;
            int enemiesKilled = 0;
            GameLoop(&surviveTime, &enemiesKilled);

            ScoreInfo inf = {0};
            inf.enemiesKilled = enemiesKilled;
            inf.surviveTime = surviveTime;
            char mustRefresh = 0;

            if(enemiesKilled > killedHS.enemiesKilled){
                killedHS = inf;
                mustRefresh = 1;
            }
            if(surviveTime > timeHS.surviveTime){
                timeHS = inf;
                mustRefresh = 1;
            }

            if(mustRefresh){
                scoreFile = ti_Open("PseudoDScores", "w");
                ti_Write((void*)&timeHS, sizeof(ScoreInfo), 1, scoreFile);
                ti_Write((void*)&killedHS, sizeof(ScoreInfo), 1, scoreFile);
                ti_SetArchiveStatus(true, scoreFile);
                ti_Close(scoreFile);
            }

            MenuLoop();
        }
    }
}

int main(void)
{
    //INITIALIZE GFX
    gfx_Begin();

    //SETS PALETTE
    gfx_SetPalette(gtia_palette, sizeof_gtia_palette, 0);
    gfx_SetTransparentColor(SKIP_RENDER_COLOR);

    //ZERO ON SCREEN BUFFER
    gfx_SetDrawBuffer();
    gfx_ZeroScreen();

    //ZERO OTHER BUFFER
    gfx_SwapDraw();
    gfx_ZeroScreen();

    //LOOP THE MENU AND GAME
    MenuLoop();    

    gfx_End();

    return 0;
}
