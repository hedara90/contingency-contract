#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ice Wisps works")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_WISPS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_WISPS, player);
        STATUS_ICON(opponent, frostbite: TRUE);
    }
}
