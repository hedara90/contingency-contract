#include "global.h"
#include "test/battle.h"
#include "risk.h"
#include "malloc.h"
#include "data.h"
#include "battle.h"
#include "battle_main.h"

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

SINGLE_BATTLE_TEST("Risk: Player can't crit")
{
    s16 damageFoe;
    s16 damagePlayer;
    GIVEN {
        gRisks.cantCrit = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WICKED_BLOW); MOVE(opponent, MOVE_WICKED_BLOW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WICKED_BLOW, player);
        HP_BAR(opponent, captureDamage: &damagePlayer);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WICKED_BLOW, opponent);
        HP_BAR(player, captureDamage: &damageFoe);
    } THEN {
        EXPECT_MUL_EQ(damagePlayer, Q_4_12(1.5), damageFoe);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent moves first (Single)")
{
    GIVEN {
        gRisks.opponentMovesFirst = TRUE;
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
    }
}

DOUBLE_BATTLE_TEST("Risk: Opponent moves first (Double)")
{
    GIVEN {
        gRisks.opponentMovesFirst = TRUE;
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft);
            MOVE(opponentRight, MOVE_SCRATCH, target: playerRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Regenerator")
{
    //  Also checking that it doesn't break other switchout abilities
    GIVEN {
        gRisks.hasRegenerator = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); HP(100); MaxHP(300); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } SCENE {
        MESSAGE("The effects of the neutralizing gas wore off!");
    } THEN {
        EXPECT_EQ(opponent->hp, 200);
    }
}

TEST("Risk: Opponent has 1 extra mon in party")
{
    gRisks.opponentPartyPlus1 = TRUE;
    struct Pokemon *testParty = Alloc(6 * sizeof(struct Pokemon));
    CreateNPCTrainerPartyFromTrainer(testParty, GetTrainerStructFromId(15), TRUE, BATTLE_TYPE_TRAINER);
    EXPECT_NE(GetMonData(&testParty[2], MON_DATA_SPECIES), SPECIES_NONE);
    Free(testParty);
    gRisks.opponentPartyPlus1 = FALSE;
}

SINGLE_BATTLE_TEST("Risk: Turn Limit 1")
{
    GIVEN {
        gRisks.turnLimit1 = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
        TURN { }
        TURN { }
    } SCENE {
        MESSAGE("You lost against 2!");
    }
}

SINGLE_BATTLE_TEST("Risk: Turn Limit 2")
{
    GIVEN {
        gRisks.turnLimit2 = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
        TURN { }
    } SCENE {
        MESSAGE("You lost against 2!");
    }
}

SINGLE_BATTLE_TEST("Risk: Turn Limit 3")
{
    GIVEN {
        gRisks.turnLimit3 = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        MESSAGE("You lost against 2!");
    }
}

SINGLE_BATTLE_TEST("Risk: Turn Limit win")
{
    GIVEN {
        gRisks.turnLimit3 = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GUILLOTINE); }
    } SCENE {
        MESSAGE("You defeated 2!");
    }
}

SINGLE_BATTLE_TEST("Risk: Flip type chart")
{
    GIVEN {
        gRisks.flipTypeChart = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Risk: Attacker gets Drowsy")
{
    GIVEN {
        gRisks.attackGetsDrowsy = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { }
        TURN { }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        STATUS_ICON(player, sleep: TRUE);
    }
}

SINGLE_BATTLE_TEST("Risk: Status gets Para")
{
    GIVEN {
        gRisks.statusGetsPara = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        STATUS_ICON(player, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Risk: Foe has Metronome (item)")
{
    s16 damage[7];
    GIVEN {
        gRisks.foeHasMetronome = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[1]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[2]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[3]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[4]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[5]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage[6]);
    } THEN {
        EXPECT_MUL_EQ(damage[0], Q_4_12(1.2), damage[1]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(1.4), damage[2]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(1.6), damage[3]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(1.8), damage[4]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(2.0), damage[5]);
        EXPECT_EQ(damage[0], damage[6]);
    }
}

SINGLE_BATTLE_TEST("Risk: Player has negative Metronome (item)")
{
    s16 damage[7];
    GIVEN {
        gRisks.playerHasNegativeMetronome = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[2]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[3]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[4]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[5]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[6]);
    } THEN {
        EXPECT_MUL_EQ(damage[0], Q_4_12(0.8), damage[1]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(0.6), damage[2]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(0.4), damage[3]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(0.2), damage[4]);
        EXPECT_MUL_EQ(damage[0], Q_4_12(0.0), damage[5]);
        EXPECT_EQ(damage[0], damage[6]);
    }
}

SINGLE_BATTLE_TEST("Risk: Player has recoil")
{
    s16 damageAtk;
    s16 damageRecoil;
    GIVEN {
        gRisks.playerHasRecoil = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damageAtk);
        HP_BAR(player, captureDamage: &damageRecoil);
    } THEN {
        EXPECT_MUL_EQ(damageAtk, Q_4_12(0.25), damageRecoil);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent inflicts Gastro Acid on attack")
{
    GIVEN {
        gRisks.opponentInflictsGastroAcid = TRUE;
        PLAYER(SPECIES_FLYGON) { Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent moves last but force switches")
{
    GIVEN {
        gRisks.opponentAttacksSwitches = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        MESSAGE("Wynaut was dragged out!");
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent attacks apply disable")
{
    GIVEN {
        gRisks.opponentAttacksDisable = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        MESSAGE("Wobbuffet's move is no longer disabled!");
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent attacks apply torment")
{
    GIVEN {
        gRisks.opponentAttacksTorment = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        MESSAGE("Wobbuffet is no longer tormented!");
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with burn")
{
    GIVEN {
        gRisks.playerStartsWithBurn = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gBattleMons[0].status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with paralysis")
{
    GIVEN {
        gRisks.playerStartsWithParalysis = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gBattleMons[0].status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with frostbite")
{
    GIVEN {
        gRisks.playerStartsWithFrostbite = TRUE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gBattleMons[0].status1, STATUS1_FROSTBITE);
    }
}
