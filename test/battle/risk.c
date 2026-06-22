#include "global.h"
#include "test/battle.h"
#include "risk.h"

SINGLE_BATTLE_TEST("Risk: Opponent has Sturdy")
{
    GIVEN {
        gRisks.hasSturdy = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Level(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW);
    } THEN {
        EXPECT_EQ(opponent->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Sturdy (breaks)")
{
    GIVEN {
        gRisks.hasSturdy = TRUE;
        PLAYER(SPECIES_RAMPARDOS) { Ability(ABILITY_MOLD_BREAKER); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW);
    } THEN {
        EXPECT_EQ(opponent->hp, 0);
    }
}
