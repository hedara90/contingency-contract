#ifndef GUARD_RISK
#define GUARD_RISK

#include "gba/types.h"
#include "global.h"

struct Risks
{
    bool32 hasSturdy:1;
    bool32 hasMoldBreaker:1;
    bool32 hasFilter:1;
    bool32 hasAdaptability:1;
    bool32 hasWonderGuard:1;
    bool32 hasGuaranteedEffects:1;
    bool32 padding:26;
};

extern struct Risks gRisks;

void ClearRisks(void);

#endif // GUARD_RISK
