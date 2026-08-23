#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("AI will use Focus Energy to boost crit rate")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KINGDRA) { Moves(MOVE_DRAGON_PULSE, MOVE_DRACO_METEOR, MOVE_FOCUS_ENERGY); Ability(ABILITY_SNIPER); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_FOCUS_ENERGY); }
        TURN { EXPECT_MOVE(opponent, MOVE_DRACO_METEOR); }
        TURN { EXPECT_MOVE(opponent, MOVE_DRACO_METEOR); }
    }
}
