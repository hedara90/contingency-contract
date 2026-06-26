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
    bool32 hasRegenerator:1;
    bool32 cantCrit:1;
    bool32 opponentMovesFirst:1;
    bool32 opponentPartyPlus1:1;
    bool32 padding:23;
};

extern struct Risks gRisks;

void ClearRisks(void);

#endif // GUARD_RISK
