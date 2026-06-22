#include "gba/types.h"
#include "global.h"
#include "risk.h"

EWRAM_DATA struct Risks gRisks;

void ClearRisks(void)
{
    CpuFill32(0, &gRisks, sizeof(struct Risks));
}
