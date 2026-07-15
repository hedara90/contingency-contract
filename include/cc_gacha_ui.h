#ifndef GUARD_CC_GACHA_UI
#define GUARD_CC_GACHA_UI

#include "gba/types.h"
#include "main.h"
#include "cc_mon_management.h"
#include "script.h"
#include "constants/cc_version.h"

void Gacha_Init(MainCallback callback, enum Banner banner);
void Gacha_InitFromScript(struct ScriptContext *ctx);

#endif
