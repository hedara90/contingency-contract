#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Combustion deals double damage to foes with a status", s16 damage)
{
    u32 status1;
    PARAMETRIZE { status1 = STATUS1_NONE; }
    PARAMETRIZE { status1 = STATUS1_SLEEP; }
    PARAMETRIZE { status1 = STATUS1_POISON; }
    PARAMETRIZE { status1 = STATUS1_BURN; }
    PARAMETRIZE { status1 = STATUS1_FREEZE; }
    PARAMETRIZE { status1 = STATUS1_PARALYSIS; }
    PARAMETRIZE { status1 = STATUS1_TOXIC_POISON; }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_COMBUSTION) == EFFECT_DOUBLE_POWER_ON_ARG_STATUS);
        ASSUME(GetMoveEffectArg_Status(MOVE_COMBUSTION) == STATUS1_ANY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status1); }
    } WHEN {
        TURN { MOVE(player, MOVE_COMBUSTION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COMBUSTION, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        if (i > 0)
            EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[i].damage);
        if (i > 1)
            EXPECT_EQ(results[i-1].damage, results[i].damage);
    }
}

SINGLE_BATTLE_TEST("Algae Bloom sets up Grassy Terrain")
{
    GIVEN {
        OPPONENT(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ALGAE_BLOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Algae Bloom!");
        MESSAGE("Grass grew to cover the battlefield!");
    }
}

SINGLE_BATTLE_TEST("Steel Spike sets up hazards after hitting the target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STEEL_SPIKE); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        s32 maxHP = GetMonData(&OPPONENT_PARTY[1], MON_DATA_MAX_HP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        HP_BAR(opponent);
        MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");
        MESSAGE("2 sent out Wobbuffet!");
        HP_BAR(opponent, damage: maxHP / 8);
        MESSAGE("The sharp steel bit into the opposing Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Steel Spike can set up pointed stones only once")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_STEEL_SPIKE); }
        TURN { MOVE(player, MOVE_STEEL_SPIKE); }
        TURN { MOVE(player, MOVE_STEEL_SPIKE); }
        TURN { MOVE(player, MOVE_STEEL_SPIKE); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        s32 maxHP = GetMonData(&OPPONENT_PARTY[1], MON_DATA_MAX_HP);

        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        HP_BAR(opponent);
        MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");

        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");

        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");

        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");

        MESSAGE("2 sent out Wynaut!");
        HP_BAR(opponent, damage: maxHP / 8);
        MESSAGE("The sharp steel bit into the opposing Wynaut!");
    }
}

SINGLE_BATTLE_TEST("Steel Spike sets up hazards after any ability activation")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SKARMORY) { Ability(ABILITY_WEAK_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_STEEL_SPIKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        ABILITY_POPUP(opponent, ABILITY_WEAK_ARMOR);
        MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");
    }
}

SINGLE_BATTLE_TEST("Steel Spike fails to set up hazards if user faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); }
    } WHEN {
        TURN { MOVE(player, MOVE_STEEL_SPIKE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, player);
        HP_BAR(player);
        MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rocky Helmet!");
        NOT MESSAGE("Sharp-pointed pieces of steel started floating around the opposing Pokémon!");
    }
}

SINGLE_BATTLE_TEST("Steel Spike will set up rocks if the target is behind a Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_STEEL_SPIKE); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEEL_SPIKE, opponent);
        SUB_HIT(player);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Sand Blast deals 1.5x damage in a Sandstorm", s16 damage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_SANDSTORM; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_SANDSHREW);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_SAND_BLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_BLAST, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Sand Blast sets Sandstorm")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_SANDSHREW);
    } WHEN {
        TURN { MOVE(player, MOVE_SAND_BLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_BLAST, player);
        MESSAGE("A sandstorm kicked up!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SANDSTORM_CONTINUES);
    }
}

SINGLE_BATTLE_TEST("Protect: Boreal Bastion frostbites Pokémon for moves making contact")
{
    enum Move usedMove = MOVE_NONE;

    PARAMETRIZE { usedMove = MOVE_SCRATCH; }
    PARAMETRIZE { usedMove = MOVE_LEER; }
    PARAMETRIZE { usedMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BOREAL_BASTION); MOVE(player, usedMove); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOREAL_BASTION, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
        MESSAGE("The opposing Wobbuffet protected itself!");
        if (usedMove == MOVE_SCRATCH) {
            NOT HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_FROSTBITE);
        } else {
            NONE_OF {
                HP_BAR(opponent);
                STATUS_ICON(player, STATUS1_FROSTBITE);
            }
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Boreal Bastion can't frostbite Pokémon if they are already statused")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TOXIC); }
        TURN { MOVE(opponent, MOVE_BOREAL_BASTION); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, opponent);
        STATUS_ICON(player, STATUS1_TOXIC_POISON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOREAL_BASTION, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_FROSTBITE);
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Boreal Bastion doesn't frostbite attacker when charging a two turn move")
{
    u32 move;
    PARAMETRIZE { move = MOVE_BOUNCE; }
    PARAMETRIZE { move = MOVE_DIG; }

    GIVEN {
        ASSUME(MoveMakesContact(MOVE_BOUNCE));
        ASSUME(MoveMakesContact(MOVE_DIG));
        ASSUME(gBattleMoveEffects[GetMoveEffect(MOVE_BOUNCE)].twoTurnEffect);
        ASSUME(gBattleMoveEffects[GetMoveEffect(MOVE_DIG)].twoTurnEffect);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BOREAL_BASTION); MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOREAL_BASTION, player);

        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        NONE_OF {
            HP_BAR(player);
            STATUS_ICON(opponent, STATUS1_FROSTBITE);
        }
    }
}
