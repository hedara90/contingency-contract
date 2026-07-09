#ifndef GUARD_CC_MON_MANAGEMENT
#define GUARD_CC_MON_MANAGEMENT

#include "global.h"
#include "gba/types.h"
#include "constants/species.h"
#include "constants/species.h"

enum GiveResult
{
    GIVE_RESULT_FIRST,
    GIVE_RESULT_DUPE,
    GIVE_RESULT_CAP,
};

struct GachaBanner
{
    u16 num4Stars;
    u16 num5Stars;
    u16 num6Stars;
    const enum Species *const mons4Star;
    const enum Species *const mons5Star;
    const enum Species *const mons6Star;
};

enum GiveResult GiveGachaMon(enum Species species);
enum Species RollGachaMon(enum Banner banner);
void DoSinglePull(enum Banner banner);
void Do10Pull(enum Banner banner);

#endif
