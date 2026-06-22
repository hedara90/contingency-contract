#ifndef GUARD_RISK
#define GUARD_RISK

#include "gba/types.h"
#include "global.h"

struct Risks
{
    bool32 hasSturdy:1;
    bool32 padding:31;
};

extern struct Risks gRisks;

void ClearRisks(void);

#endif // GUARD_RISK
