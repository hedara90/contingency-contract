#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Riptide increases the damage taken from binding effects", s16 damage)
{
    enum Ability ability;

    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_RIPTIDE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TENTACRUEL) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WRAP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRAP, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_TURN_TRAP, player);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Riptide increases the damage taken from binding effects even if holder's ability gets nullified after", s16 damage)
{
    enum Ability ability;

    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_RIPTIDE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TENTACRUEL) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WRAP); MOVE(player, MOVE_WORRY_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRAP, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORRY_SEED, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_TURN_TRAP, player);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}
