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
        SetRisk(RISK_HAS_STURDY);
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
        SetRisk(RISK_HAS_STURDY);
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

AI_SINGLE_BATTLE_TEST("Risk: Opponent has Sturdy (AI)")
{
    u32 odds = 0;
    bool32 hasSturdy = FALSE;
    PARAMETRIZE { hasSturdy = FALSE; odds = SHOULD_SWITCH_HASBADODDS_PERCENTAGE; }
    PARAMETRIZE { hasSturdy = TRUE; odds = 100; }
    PASSES_RANDOMLY(odds, 100, RNG_AI_SWITCH_HASBADODDS);
    GIVEN {
        if (hasSturdy)
            SetRisk(RISK_HAS_STURDY);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE) { HP(1); MaxHP(400); Moves(MOVE_THUNDERBOLT, MOVE_THUNDER_WAVE, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_PELIPPER) { Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_RHYDON) { Moves(MOVE_EARTHQUAKE); Ability(ABILITY_ROCK_HEAD); }
    } WHEN {
        if (IsRiskActive(RISK_HAS_STURDY))
            TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
        else
            TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_SWITCH(opponent, 1); }
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Mold Breaker")
{
    GIVEN {
        SetRisk(RISK_HAS_MOLD_BREAKER);
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

AI_SINGLE_BATTLE_TEST("Risk: Opponent has Mold Breaker (AI)")
{
    bool32 hasMoldBreaker = FALSE;
    PARAMETRIZE { hasMoldBreaker = FALSE; }
    PARAMETRIZE { hasMoldBreaker = TRUE; }
    GIVEN {
        if (hasMoldBreaker)
            SetRisk(RISK_HAS_MOLD_BREAKER);
        PLAYER(SPECIES_EELEKTROSS) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GOLURK) { Moves(MOVE_TACKLE, MOVE_EARTHQUAKE); }
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
    } WHEN {
        if (IsRiskActive(RISK_HAS_MOLD_BREAKER))
            TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
        else
            TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Filter")
{
    s16 damageFoe;
    s16 damagePlayer;
    GIVEN {
        SetRisk(RISK_HAS_FILTER);
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
        SetRisk(RISK_HAS_ADAPTABILITY);
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
        SetRisk(RISK_HAS_WONDER_GUARD);
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
        SetRisk(RISK_CANT_CRIT);
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
        SetRisk(RISK_OPPONENT_MOVES_FIRST);
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
        SetRisk(RISK_OPPONENT_MOVES_FIRST);
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

AI_SINGLE_BATTLE_TEST("Risk: Opponent moves first (AI)")
{
    u32 odds;
    PARAMETRIZE { odds = 100; SetRisk(RISK_OPPONENT_MOVES_FIRST); }
    PARAMETRIZE { odds = SHOULD_SWITCH_HASBADODDS_PERCENTAGE; ClearRisk(RISK_OPPONENT_MOVES_FIRST); }
    PASSES_RANDOMLY(odds, 100, RNG_AI_SWITCH_HASBADODDS);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE) { HP(1); MaxHP(100); Moves(MOVE_THUNDERBOLT, MOVE_THUNDER_WAVE, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_PELIPPER) { Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_RHYDON) { Moves(MOVE_EARTHQUAKE); Ability(ABILITY_ROCK_HEAD); }
    } WHEN {
        if (!IsRiskActive(RISK_OPPONENT_MOVES_FIRST))
            TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_SWITCH(opponent, 1); }
        else
            TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Regenerator")
{
    //  Also checking that it doesn't break other switchout abilities
    GIVEN {
        SetRisk(RISK_HAS_REGENERATOR);
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
    SetRisk(RISK_OPPONENT_MORE_MONS);
    struct Pokemon *testParty = Alloc(6 * sizeof(struct Pokemon));
    CreateNPCTrainerPartyFromTrainer(testParty, GetTrainerStructFromId(15), TRUE, BATTLE_TYPE_TRAINER);
    EXPECT_NE(GetMonData(&testParty[2], MON_DATA_SPECIES), SPECIES_NONE);
    Free(testParty);
    ClearRisk(RISK_OPPONENT_MORE_MONS);
}

SINGLE_BATTLE_TEST("Risk: Turn Limit 1")
{
    GIVEN {
        SetRisk(RISK_TURN_LIMIT_1);
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
        SetRisk(RISK_TURN_LIMIT_2);
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
        SetRisk(RISK_TURN_LIMIT_3);
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
        SetRisk(RISK_TURN_LIMIT_3);
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
        SetRisk(RISK_FLIP_TYPE_CHART);
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
        SetRisk(RISK_ATTACK_GETS_DROWSY);
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
        SetRisk(RISK_STATUS_GETS_PARA);
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
        SetRisk(RISK_FOE_HAS_METRONOME);
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
        SetRisk(RISK_PLAYER_HAS_NEGATIVE_METRONOME);
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
        SetRisk(RISK_PLAYER_HAS_RECOIL);
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
        SetRisk(RISK_OPPONENT_INFLICTS_GASTRO_ACID);
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
        SetRisk(RISK_OPPONENT_ATTACKS_SWITCHES);
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

AI_SINGLE_BATTLE_TEST("Risk: Opponent moves last but force switches (AI)")
{
    u32 odds;
    PARAMETRIZE { odds = SHOULD_SWITCH_HASBADODDS_PERCENTAGE; SetRisk(RISK_OPPONENT_ATTACKS_SWITCHES); }
    PARAMETRIZE { odds = 100; ClearRisk(RISK_OPPONENT_ATTACKS_SWITCHES); }
    PASSES_RANDOMLY(odds, 100, RNG_AI_SWITCH_HASBADODDS);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ELECTRODE) { HP(1); Speed(5); MaxHP(100); Moves(MOVE_THUNDERBOLT, MOVE_THUNDER_WAVE, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_PELIPPER) { Speed(10); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_RHYDON) { Speed(4); Moves(MOVE_EARTHQUAKE); Ability(ABILITY_ROCK_HEAD); }
    } WHEN {
        if (!IsRiskActive(RISK_OPPONENT_ATTACKS_SWITCHES))
            TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
        else
            TURN { MOVE(player, MOVE_THUNDERBOLT); EXPECT_SWITCH(opponent, 1); }
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent attacks apply disable")
{
    GIVEN {
        SetRisk(RISK_OPPONENT_ATTACKS_DISABLE);
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
        SetRisk(RISK_OPPONENT_ATTACKS_TORMENT);
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
        SetRisk(RISK_PLAYER_STARTS_WITH_BURN);
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
        SetRisk(RISK_PLAYER_STARTS_WITH_PARALYSIS);
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
        SetRisk(RISK_PLAYER_STARTS_WITH_FROSTBITE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gBattleMons[0].status1, STATUS1_FROSTBITE);
    }
}

AI_SINGLE_BATTLE_TEST("Risk: Opponent has Omniscient AI")
{
    bool32 hasOmniscientAi;
    PARAMETRIZE { hasOmniscientAi = FALSE; }
    PARAMETRIZE { hasOmniscientAi = TRUE; }
    GIVEN {
        if (hasOmniscientAi)
            SetRisk(RISK_HAS_OMNISCIENT_AI);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES);
        PLAYER(SPECIES_TYPHLOSION) { Speed(5); Moves(MOVE_TACKLE, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_ZIGZAGOON) { Speed(1); Moves(MOVE_TACKLE); Level(1); }
        OPPONENT(SPECIES_SCIZOR) { Speed(4); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BLISSEY) { Speed(4); Moves(MOVE_TACKLE); }
    } WHEN {
        if (IsRiskActive(RISK_HAS_OMNISCIENT_AI))
            TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); EXPECT_SEND_OUT(opponent, 2); }
        else
            TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); EXPECT_SEND_OUT(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("Risk: Opponent has Prediction AI (switches)")
{
    u32 odds = 0;
    bool32 hasPredictionAi;
    PARAMETRIZE { hasPredictionAi = FALSE; odds = 0; }
    PARAMETRIZE { hasPredictionAi = TRUE; odds = PREDICT_SWITCH_CHANCE; }
    PASSES_RANDOMLY(odds, 100, RNG_AI_PREDICT_SWITCH);
    GIVEN {
        if (hasPredictionAi)
            SetRisk(RISK_HAS_PREDICTION_AI);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES);
        PLAYER(SPECIES_BRONZONG) { Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_CONKELDURR) { Moves(MOVE_HAMMER_ARM); }
        OPPONENT(SPECIES_TYRANITAR) { Moves(MOVE_U_TURN, MOVE_CRUNCH); }
        OPPONENT(SPECIES_TYRANITAR) { Moves(MOVE_U_TURN, MOVE_CRUNCH); }
    } WHEN {
        TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_U_TURN); }
    }
}

AI_SINGLE_BATTLE_TEST("Risk: Opponent has Prediction AI (moves)")
{
    u32 odds = 0;
    bool32 hasPredictionAi;
    PARAMETRIZE { hasPredictionAi = FALSE; odds = 0; }
    PARAMETRIZE { hasPredictionAi = TRUE; odds = PREDICT_MOVE_CHANCE; }
    PASSES_RANDOMLY(odds, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        if (hasPredictionAi)
            SetRisk(RISK_HAS_PREDICTION_AI);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES);
        PLAYER(SPECIES_VAPOREON) { Ability(ABILITY_WATER_ABSORB); Moves(MOVE_SURF, MOVE_TACKLE); }
        OPPONENT(SPECIES_NUMEL) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_VAPOREON) { Ability(ABILITY_WATER_ABSORB); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SURF); EXPECT_SWITCH(opponent, 1); }
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has guaranteed secondary effects")
{
    PARAMETRIZE { SetRisk(RISK_HAS_GUARANTEED_EFFECTS); }
    PARAMETRIZE { ClearRisk(RISK_HAS_GUARANTEED_EFFECTS); }
    if (IsRiskActive(RISK_HAS_GUARANTEED_EFFECTS))
        PASSES_RANDOMLY(100, 100);
    else
        PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEADBUTT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEADBUTT, opponent);
        MESSAGE("Wobbuffet flinched and couldn't move!");
    }
}

AI_SINGLE_BATTLE_TEST("Risk: Opponent has guaranteed secondary effects (AI)")
{
    bool32 hasGuaranteedEffects;
    PARAMETRIZE { hasGuaranteedEffects = TRUE; }
    PARAMETRIZE { hasGuaranteedEffects = FALSE; }
    GIVEN {
        if (hasGuaranteedEffects)
            SetRisk(RISK_HAS_GUARANTEED_EFFECTS);
        PLAYER(SPECIES_AGGRON) { Speed(1); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ZIGZAGOON) { Speed(2); Moves(MOVE_HEADBUTT, MOVE_KARATE_CHOP); }
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
    } WHEN {
        if (IsRiskActive(RISK_HAS_GUARANTEED_EFFECTS))
            TURN { EXPECT_MOVE(opponent, MOVE_HEADBUTT); MOVE(player, MOVE_TACKLE); }
        else
            TURN { EXPECT_MOVE(opponent, MOVE_KARATE_CHOP); MOVE(player, MOVE_TACKLE); }
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent can't miss")
{
    PASSES_RANDOMLY(100, 100);
    GIVEN {
        SetRisk(RISK_HAS_GUARANTEED_ACCURACY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        NONE_OF { MESSAGE("Wobbuffet avoided the attack!"); }
    }
}

AI_SINGLE_BATTLE_TEST("Risk: Opponent can't miss (AI)")
{
    GIVEN {
        SetRisk(RISK_HAS_GUARANTEED_ACCURACY);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZIGZAGOON) { Moves(MOVE_DOUBLE_EDGE, MOVE_FISSURE); }
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_FISSURE); }
    }
}

SINGLE_BATTLE_TEST("Risk: Gen 1 crit chance")
{
    u32 genConfig, passes, trials;
    PARAMETRIZE { genConfig = GEN_9; passes = 1; trials = 16; SetRisk(RISK_HAS_GEN_1_CRIT_CHANCE); } // Override gen config with risk
    PARAMETRIZE { genConfig = GEN_1; passes = 1;  trials = 16; ClearRisk(RISK_HAS_GEN_1_CRIT_CHANCE); }   //  6.25% with Wobbuffet's base speed
    PARAMETRIZE { genConfig = GEN_2; passes = 17; trials = 256; ClearRisk(RISK_HAS_GEN_1_CRIT_CHANCE); }  // ~6.64%
    for (u32 j = GEN_3; j <= GEN_6; j++)
        PARAMETRIZE { genConfig = j; passes = 1,  trials = 16; ClearRisk(RISK_HAS_GEN_1_CRIT_CHANCE); }  //  6.25%
    for (u32 j = GEN_7; j <= GEN_9; j++)
        PARAMETRIZE { genConfig = j; passes = 1,  trials = 24; ClearRisk(RISK_HAS_GEN_1_CRIT_CHANCE); }  // ~4.17%

    PASSES_RANDOMLY(passes, trials, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, genConfig);
        ASSUME(GetSpeciesBaseSpeed(SPECIES_WOBBUFFET) == 33);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Risk: Player uses lower half of damage rolls (85 - 92)")
{
    GIVEN {
        ASSUME(DMG_ROLL_PERCENT_HI - (DMG_ROLL_PERCENT_HI - DMG_ROLL_PERCENT_LOWER_HI) == 92); // Upper bound
        ASSUME(DMG_ROLL_PERCENT_HI - (DMG_ROLL_PERCENT_HI - DMG_ROLL_PERCENT_LO) == 85); // Lower bound
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent uses upper half of damage rolls (93 - 100)")
{
    GIVEN {
        ASSUME(DMG_ROLL_PERCENT_HI == 100); // Upper bound
        ASSUME(DMG_ROLL_PERCENT_HI - (DMG_ROLL_PERCENT_HI - DMG_ROLL_PERCENT_UPPER_LO) == 93); // Lower bound
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    }
}

SINGLE_BATTLE_TEST("Risk: Player can only use 2 moves, but get Parental Bond")
{
    //  Can't test move selection
    s16 dmg1, dmg2;
    GIVEN {
        SetRisk(RISK_PLAYER_HAS_PARENTAL_BOND);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &dmg1);
        HP_BAR(opponent, captureDamage: &dmg2);
    } THEN {
        EXPECT_MUL_EQ(dmg1, Q_4_12(0.25), dmg2);
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with spikes", s16 damage)
{
    u32 divisor;
    u32 spikeLayers;
    PARAMETRIZE { spikeLayers = 1; divisor = 8; }
    PARAMETRIZE { spikeLayers = 2; divisor = 6; }
    PARAMETRIZE { spikeLayers = 3; divisor = 4; }
    GIVEN {
        switch (spikeLayers)
        {
        case 1:
            SetRisk(RISK_PLAYER_SPIKES_1);
            break;
        case 2:
            SetRisk(RISK_PLAYER_SPIKES_2);
            break;
        case 3:
            SetRisk(RISK_PLAYER_SPIKES_3);
            break;
        }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Go! Wynaut!");
        s32 maxHP = GetMonData(&PLAYER_PARTY[1], MON_DATA_MAX_HP);
        HP_BAR(player, damage: maxHP / divisor);
        MESSAGE("Wynaut was hurt by the spikes!");
    } FINALLY {
        ResetStartingStatuses();
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with toxic spikes")
{
    u32 spikeLayers;
    PARAMETRIZE { spikeLayers = 1; }
    PARAMETRIZE { spikeLayers = 2; }
    GIVEN {
        switch (spikeLayers)
        {
        case 1:
            SetRisk(RISK_PLAYER_TOXIC_SPIKES_1);
            break;
        case 2:
            SetRisk(RISK_PLAYER_TOXIC_SPIKES_2);
            break;
        }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
    } SCENE {
        if (IsRiskActive(RISK_PLAYER_TOXIC_SPIKES_2))
        {
            MESSAGE("Go! Wynaut!");
            MESSAGE("Wynaut was badly poisoned!");
            STATUS_ICON(player, badPoison: TRUE);
        }
        else
        {
            MESSAGE("Go! Wynaut!");
            MESSAGE("Wynaut was poisoned!");
            STATUS_ICON(player, poison: TRUE);
            NOT STATUS_ICON(player, badPoison: TRUE);
        }
    } FINALLY {
        ResetStartingStatuses();
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with sticky web")
{
    GIVEN {
        SetRisk(RISK_PLAYER_STICKY_WEB);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Go! Wynaut!");
        MESSAGE("Wynaut was caught in a sticky web!");
        MESSAGE("Wynaut's Speed fell!");
    } THEN {
        ResetStartingStatuses();
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with stealth rock")
{
    GIVEN {
        SetRisk(RISK_PLAYER_STEALTH_ROCK);
        ASSUME(gSpeciesInfo[SPECIES_CHARIZARD].types[0] == TYPE_FIRE);
        ASSUME(gSpeciesInfo[SPECIES_CHARIZARD].types[1] == TYPE_FLYING);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_CHARIZARD);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Go! Charizard!");
        s32 maxHP = GetMonData(&PLAYER_PARTY[1], MON_DATA_MAX_HP);
        HP_BAR(player, damage: maxHP / 2);
        MESSAGE("Pointed stones dug into Charizard!");
    } THEN {
        ResetStartingStatuses();
    }
}

SINGLE_BATTLE_TEST("Risk: Player starts with sharp steel")
{
    GIVEN {
        SetRisk(RISK_PLAYER_SHARP_STEEL);
        ASSUME(gSpeciesInfo[SPECIES_SYLVEON].types[0] == TYPE_FAIRY);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SYLVEON);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Go! Sylveon!");
        s32 maxHP = GetMonData(&PLAYER_PARTY[1], MON_DATA_MAX_HP);
        HP_BAR(player, damage: maxHP / 4);
        MESSAGE("The sharp steel bit into Sylveon!");
    } THEN {
        ResetStartingStatuses();
    }
}

SINGLE_BATTLE_TEST("Risk: All hazard risks at once")
{
    GIVEN {
        SetRisk(RISK_PLAYER_SPIKES_3);
        SetRisk(RISK_PLAYER_TOXIC_SPIKES_2);
        SetRisk(RISK_PLAYER_STEALTH_ROCK);
        SetRisk(RISK_PLAYER_SHARP_STEEL);
        SetRisk(RISK_PLAYER_STICKY_WEB);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        MESSAGE("Wobbuffet was hurt by the spikes!");
        MESSAGE("Wobbuffet was badly poisoned!");
        MESSAGE("Wobbuffet was caught in a sticky web!");
        MESSAGE("Pointed stones dug into Wobbuffet!");
        MESSAGE("The sharp steel bit into Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Risk: Player cannot remove hazards")
{
    // Not handling Court Change, we didn't give it to any mons
    u32 move;
    PARAMETRIZE { move = MOVE_RAPID_SPIN; } // Shared with Mortal Spin
    PARAMETRIZE { move = MOVE_DEFOG; } // Shared with Tidy Up
    GIVEN {
        SetRisk(RISK_PLAYER_HAZARDS_NOT_REMOVABLE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(opponent, MOVE_SPIKES); MOVE(player, move); }
        TURN { MOVE(opponent, MOVE_SPIKES); SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        NONE_OF {
            MESSAGE("The spikes disappeared from the ground around your team!");
            MESSAGE("The sticky web has disappeared from the ground around you!");
            MESSAGE("The poison spikes disappeared from the ground around your team!");
            MESSAGE("The pointed stones disappeared from around your team!");
        }
        MESSAGE("Pointed stones dug into Wynaut!");
        MESSAGE("Wynaut was caught in a sticky web!");
        MESSAGE("Wynaut was poisoned!");
        MESSAGE("Wynaut was hurt by the spikes!");
    }
}

//  Can't test this here since tests doesn't go through the standard path for setting HP
/*
SINGLE_BATTLE_TEST("Risk: Opponent has extra HP", s16 damage)
{
    PARAMETRIZE { }
    PARAMETRIZE { SetRisk(RISK_OPPONENT_HP_1); }
    PARAMETRIZE { SetRisk(RISK_OPPONENT_HP_2); }
    PARAMETRIZE { SetRisk(RISK_OPPONENT_HP_3); }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUPER_FANG); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[3].damage);
    }
}
*/

SINGLE_BATTLE_TEST("Risk: Player can't have non-berry items, item removed")
{
    PARAMETRIZE { }
    PARAMETRIZE { SetRisk(RISK_PLAYER_JUST_BERRIES); }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_AIR_BALLOON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        if (IsRiskActive(RISK_PLAYER_JUST_BERRIES))
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
        }
        else
        {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Risk: Player can't have non-berry items, berry stays")
{
    PARAMETRIZE { }
    PARAMETRIZE { SetRisk(RISK_PLAYER_JUST_BERRIES); }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_SITRUS_BERRY); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUPER_FANG); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Risk: Player has Perish Body")
{
    GIVEN {
        SetRisk(RISK_PLAYER_HAS_PERISH_BODY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCRATCH); }
        TURN { }
        TURN { }
        TURN { }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        HP_BAR(player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Risk: Player has Beast Boost")
{
    GIVEN {
        SetRisk(RISK_PLAYER_HAS_BEAST_BOOST);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_GUILLOTINE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Risk: Opponent has Filter")
{
    s16 damageFoe;
    s16 damagePlayer;
    GIVEN {
        SetRisk(RISK_PLAYER_HAS_FILTER);
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
        EXPECT_MUL_EQ(damagePlayer, Q_4_12(0.75), damageFoe);
    }
}
