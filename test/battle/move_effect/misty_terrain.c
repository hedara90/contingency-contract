#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Misty Terrain protects grounded battlers from non-volatile status conditions")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CLAYDOL) { Ability(ABILITY_LEVITATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MISTY_TERRAIN); MOVE(opponent, MOVE_TOXIC); }
        TURN { MOVE(player, MOVE_TOXIC); }
    } SCENE {
        MESSAGE("Wobbuffet used Misty Terrain!");
        MESSAGE("The opposing Claydol used Toxic!");
        MESSAGE("Wobbuffet surrounds itself with a protective mist!");
        NOT { STATUS_ICON(opponent, badPoison: TRUE); }
        MESSAGE("Wobbuffet used Toxic!");
        STATUS_ICON(opponent, badPoison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Misty Terrain does not increase the power of Fairy-type moves", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_MISTY_TERRAIN); }
        TURN { MOVE(player, MOVE_MOONBLAST); }
    } SCENE {
        MESSAGE("Wobbuffet used Moonblast!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Misty Terrain decreases power of Dragon-type moves by 50 percent", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_MISTY_TERRAIN); }
        TURN { MOVE(player, MOVE_DRAGON_CLAW); }
    } SCENE {
        MESSAGE("Wobbuffet used Dragon Claw!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Misty Terrain lasts for 8 turns when set manually")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MISTY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, player);
        MESSAGE("Mist swirled around the battlefield!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        MESSAGE("The mist disappeared from the battlefield.");
    }
}

SINGLE_BATTLE_TEST("Misty Terrain lasts for 5 turns when set by an ability")
{
    GIVEN {
        PLAYER(SPECIES_TAPU_FINI) { Ability(ABILITY_MISTY_SURGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MISTY_SURGE);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);

        MESSAGE("The mist disappeared from the battlefield.");
    }
}

SINGLE_BATTLE_TEST("Misty Terrain will fail if there is one already on the field")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MISTY_TERRAIN); MOVE(opponent, MOVE_MISTY_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, opponent);
    }
}
