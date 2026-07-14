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

struct GachaResult
{
    enum Species species;
    enum GiveResult result;
    u32 stars;
};

enum GiveResult GiveGachaMon(enum Species species, u32 star);
enum Species RollGachaMon(enum Banner banner, u32 *star);
void DoSinglePull(enum Banner banner);
void Do10Pull(enum Banner banner);

extern struct GachaResult gGachaResults[10];

#endif
