#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Contagion inflicts poison when using a draining move")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_CONTAGION);
        STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Contagion doesn't inflict poison when using a draining move at full HP")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Contagion is blocked by Shield Dust")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(SPECIES_VIVILLON) { Ability(ABILITY_SHIELD_DUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Contagion is blocked by Covert Cloak")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_COVERT_CLOAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Contagion doesn't work on poison or steel types")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_MUK; }
    PARAMETRIZE { species = SPECIES_BRONZONG; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}
