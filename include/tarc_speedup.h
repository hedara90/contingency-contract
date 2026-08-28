#ifndef GUARD_TARC_SPEEDUP
#define GUARD_TARC_SPEEDUP

#include "global.h"
#include "gba/types.h"

#define MAX_SPEEDUP 5

void StartSpeedup(void);
void StopSpeedup(void);
void CheckSpeedupControls(void);
bool32 SpeedupShouldSkip(void);
bool32 SpeedupIsPaused(void);
void ResetChangedSpeedup(void);

void CheckSpeedupBeforeLeavingBattleScreen(void);
void CheckSpeedupBeforeEnteringBattleScreen(void);

#endif
