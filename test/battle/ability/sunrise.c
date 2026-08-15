#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sunbreak sets sun after using a Fire attack")
{
    GIVEN {
        PLAYER(SPECIES_FLAREON) { Ability(ABILITY_SUNBREAK); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_INCINERATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INCINERATE, player);
        ABILITY_POPUP(player, ABILITY_SUNBREAK);
    }
}

SINGLE_BATTLE_TEST("Sunbreak can only be set once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_FLAREON) { Ability(ABILITY_SUNBREAK); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_INCINERATE); }
        TURN { }
        TURN { }
        TURN { }
        TURN { }
        TURN { MOVE(player, MOVE_INCINERATE); MOVE(opponent, MOVE_ROAR); }
        TURN { SWITCH(player, 0); }
        TURN { MOVE(player, MOVE_INCINERATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INCINERATE, player);
        ABILITY_POPUP(player, ABILITY_SUNBREAK);
        MESSAGE("The sun breaks through!");
        MESSAGE("The sunlight is strong.");
        MESSAGE("The sunlight is strong.");
        MESSAGE("The sunlight is strong.");
        MESSAGE("The sunlight is strong.");
        NOT MESSAGE("The sunlight is strong.");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INCINERATE, player);
        NOT ABILITY_POPUP(player, ABILITY_SUNBREAK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR, opponent);
        SEND_IN_MESSAGE("Flareon");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INCINERATE, player);
        ABILITY_POPUP(player, ABILITY_SUNBREAK);
        MESSAGE("The sun breaks through!");
    }
}
