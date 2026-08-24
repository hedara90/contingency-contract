#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("AI will use Focus Energy to boost crit rate")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SWIFT_SWIM; }
    PARAMETRIZE { ability = ABILITY_SNIPER; }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_FOCUS_ENERGY) == EFFECT_FOCUS_ENERGY);
        ASSUME_MOVE_EFFECT_STAT_CHANGE(MOVE_DRACO_METEOR, self: TRUE, spAtk: -2);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KINGDRA) { Moves(MOVE_DRAGON_PULSE, MOVE_DRACO_METEOR, MOVE_FOCUS_ENERGY); Ability(ability); }
    } WHEN {
        if (ability == ABILITY_SWIFT_SWIM)
        {
            TURN { EXPECT_MOVE(opponent, MOVE_DRACO_METEOR); }
            TURN { EXPECT_MOVE(opponent, MOVE_FOCUS_ENERGY); }
            TURN { EXPECT_MOVE(opponent, MOVE_DRACO_METEOR); }
        }
        else
        {
            TURN { EXPECT_MOVE(opponent, MOVE_FOCUS_ENERGY); }
            TURN { EXPECT_MOVE(opponent, MOVE_DRACO_METEOR); }
            TURN { EXPECT_MOVE(opponent, MOVE_DRACO_METEOR); }
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI won't boost into Foul Play some percentage of the time")
{
    PASSES_RANDOMLY(1, 2, RNG_AI_DONT_BOOST_ATTACK_INTO_FOUL_PLAY);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) Moves(MOVE_SCRATCH, MOVE_FOUL_PLAY);
        OPPONENT(SPECIES_ZIGZAGOON) Moves(MOVE_SCRATCH, MOVE_SWORDS_DANCE); 
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_PLAY); EXPECT_MOVE(opponent, MOVE_SWORDS_DANCE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI will use wind moves if it has Stormcaller")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_DRAMPA) { Ability(ABILITY_STORMCALLER); Moves(MOVE_TWISTER, MOVE_DRAGON_PULSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_TWISTER); }
    }
}

AI_SINGLE_BATTLE_TEST("AI will predict Physical Moves and use Steel Roller if it has Seed Sower")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_SMART_TRAINER | AI_FLAG_PREDICTION);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_FIRE_PUNCH, MOVE_CONFUSION); }
        OPPONENT(SPECIES_FERROTHORN) { Moves(MOVE_STEEL_ROLLER, MOVE_GYRO_BALL); Ability(ABILITY_SEED_SOWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_PUNCH); EXPECT_MOVE(opponent, MOVE_STEEL_ROLLER); }
    }
}
