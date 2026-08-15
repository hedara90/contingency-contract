#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Stormcaller sets Rain after using a wind move")
{
    GIVEN {
        PLAYER(SPECIES_DRAMPA) { Ability(ABILITY_STORMCALLER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FAIRY_WIND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAIRY_WIND, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_STORMCALLER);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_RAIN_CONTINUES);
    }
}
