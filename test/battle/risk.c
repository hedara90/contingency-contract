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

SINGLE_BATTLE_TEST("Risk: Opponent has Mold Breaker")
{
    GIVEN {
        gRisks.hasMoldBreaker = TRUE;
        PLAYER(SPECIES_ONIX) { Ability(ABILITY_STURDY); Level(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN);
        NOT ABILITY_POPUP(player, ABILITY_STURDY);
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Filter")
{
    s16 damageFoe;
    s16 damagePlayer;
    GIVEN {
        gRisks.hasFilter = TRUE;
        PLAYER(SPECIES_AGGRON);
        OPPONENT(SPECIES_AGGRON);
    } WHEN {
        TURN { MOVE(player, MOVE_AURA_SPHERE); MOVE(opponent, MOVE_AURA_SPHERE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AURA_SPHERE, player);
        HP_BAR(opponent, captureDamage: &damagePlayer);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AURA_SPHERE, opponent);
        HP_BAR(player, captureDamage: &damageFoe);
    } THEN {
        EXPECT_MUL_EQ(damageFoe, Q_4_12(0.75), damagePlayer);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Adaptability")
{
    s16 damageFoe;
    s16 damagePlayer;
    GIVEN {
        gRisks.hasAdaptability = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        HP_BAR(opponent, captureDamage: &damagePlayer);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);
        HP_BAR(player, captureDamage: &damageFoe);
    } THEN {
        EXPECT_MUL_EQ(damagePlayer, Q_4_12(1.33), damageFoe);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Wonder Guard")
{
    GIVEN {
        gRisks.hasWonderGuard = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_PSYCHIC); }
        TURN { MOVE(player, MOVE_CRUNCH); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
            HP_BAR(opponent);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent can't miss")
{
    PASSES_RANDOMLY(100, 100);
    GIVEN {
        gRisks.hasGuaranteedAccuracy = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        NONE_OF { MESSAGE("Wobbuffet avoided the attack!"); }
    }
}

AI_SINGLE_BATTLE_TEST("Risk: Opponent can't miss (AI)")
{
    GIVEN {
        gRisks.hasGuaranteedAccuracy = TRUE;
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZIGZAGOON) { Moves(MOVE_DOUBLE_EDGE, MOVE_FISSURE); }
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_FISSURE); }
    }
}
