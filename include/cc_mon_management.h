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
    union {
        const enum Species *const mons4Star;
        const enum Item *const items4Star;
    };
    union {
        const enum Species *const mons5Star;
        const enum Item *const items5Star;
    };
    union {
        const enum Species *const mons6Star;
        const enum Item *const items6Star;
    };
};

struct GachaResult
{
    union {
        enum Species species;
        enum Item item;
    };
    enum GiveResult result;
    u32 stars;
};

enum GiveResult GiveGachaMon(enum Species species, u32 star);
enum Species RollGachaMon(enum Banner banner, u32 *star);
void DoSinglePull(enum Banner banner);
void Do10Pull(enum Banner banner);

bool32 CheckRarities(void);

extern struct GachaResult gGachaResults[10];

#endif
