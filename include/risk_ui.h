#ifndef GUARD_RISK_UI
#define GUARD_RISK_UI

#include "gba/types.h"
#include "main.h"
#include "script.h"
#include "constants/cc_version.h"

void RiskUi_Init(MainCallback callback);
void RiskUi_InitFromScript(struct ScriptContext *ctx);

#endif
