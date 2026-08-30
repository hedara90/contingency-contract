#ifndef GUARD_VICTORY_SCREEN
#define GUARD_VICTORY_SCREEN

#include "gba/types.h"
#include "main.h"
#include "cc_mon_management.h"
#include "script.h"
#include "constants/cc_version.h"

void VictoryScreen_Init(MainCallback callback, enum Gauntlet gauntlet, bool32 fromSave);
void VictoryScreen_InitFromScript(struct ScriptContext *ctx);

#endif
