#include "global.h"
#include "gba/types.h"
#include "main.h"
#include "tarc_speedup.h"
#include "palette.h"

EWRAM_DATA u32 sSkipCounter = 0;
EWRAM_DATA bool32 sDoSpeedup = FALSE;
EWRAM_DATA u32 sNumSkips = 0;
EWRAM_DATA bool32 sPause = FALSE;
EWRAM_DATA bool32 sWasSpeedingUp = FALSE;
EWRAM_DATA u32 sChangedSpeedup = 0;

const u32 sSpeedupTiles[] = INCGFX_U32("graphics/tarc_speedup/speedup_tiles.png", ".4bpp");

void SetSpeedUpIcon(void)
{
    u32 *vramPtr = (u32 *)(BG_VRAM + TILE_SIZE_4BPP * 107);
    if (sDoSpeedup)
    {
        //  Set icon to current speedup
        for (u32 i = 0; i < 8; i++)
            vramPtr[i] = sSpeedupTiles[sNumSkips * 8 + i];
    }
    else
    {
        //  Remove speedup icon
        for (u32 i = 0; i < 8; i++)
            vramPtr[i] = 0;
    }
}

void StartSpeedup(void)
{
    if (gSaveBlock2Ptr->speedup != 0 && !sDoSpeedup)
    {
        sDoSpeedup = TRUE;
        if (sChangedSpeedup == 0)
            sNumSkips = gSaveBlock2Ptr->speedup;
        else
            sNumSkips = sChangedSpeedup;
        //  Set bg0 tilemap top left corner
        SetSpeedUpIcon();
    }
}

void StopSpeedup(void)
{
    sDoSpeedup = FALSE;
    sNumSkips = 0;
    //  Remove icon in top left corner
    SetSpeedUpIcon();
}

void CheckSpeedupControls(void)
{
    if (sDoSpeedup)
    {
        if (JOY_NEW(L_BUTTON))
        {
            if (sNumSkips > 0)
                sNumSkips--;
            //  Change speedup number in sprite
            sChangedSpeedup = sNumSkips;
            SetSpeedUpIcon();
        }
        else if (JOY_NEW(R_BUTTON))
        {
            if (sNumSkips < MAX_SPEEDUP)
                sNumSkips++;
            sChangedSpeedup = sNumSkips;
            //  Change speedup number in sprite
            SetSpeedUpIcon();
        }
    }
}

bool32 SpeedupShouldSkip(void)
{
    if (!sDoSpeedup)
        return FALSE;

    if (sSkipCounter + 1 >= sNumSkips)
    {
        sSkipCounter = 0;
        return FALSE;
    }
    else
    {
        UpdatePaletteFade();
        sSkipCounter++;
        return TRUE;
    }
}

bool32 SpeedupIsPaused(void)
{
    return sPause;
}

void CheckSpeedupBeforeLeavingBattleScreen(void)
{
    if (sDoSpeedup)
    {
        sDoSpeedup = FALSE;
        sWasSpeedingUp = TRUE;
    }
}

void CheckSpeedupBeforeEnteringBattleScreen(void)
{
    if (sWasSpeedingUp)
    {
        sDoSpeedup = TRUE;
        sWasSpeedingUp = FALSE;
        SetSpeedUpIcon();
    }
}

void ResetChangedSpeedup(void)
{
    sChangedSpeedup = 0;
}
