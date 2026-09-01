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
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_CONTAGION);
        STATUS_ICON(opponent, poison: TRUE);
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
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
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
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
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
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Contagion works with Stregnth Sap")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STRENGTH_SAP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRENGTH_SAP, player);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_CONTAGION);
        STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Contagion doesn't work if the move failed for any reason (status move)")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_STRENGTH_SAP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STRENGTH_SAP, player);
            HP_BAR(player);
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Contagion doesn't work if the move failed for any reason (attacking move)")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Ability(ABILITY_CONTAGION); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
            HP_BAR(player);
            ABILITY_POPUP(player, ABILITY_CONTAGION);
            STATUS_ICON(opponent, poison: TRUE);
        }
    }
}
