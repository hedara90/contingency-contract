#include "gba/types.h"
#include "global.h"
#include "risk.h"

void ClearRisks(void)
{
    CpuFill32(0, &gSaveBlock1Ptr->risks, sizeof(struct Risks));
}

bool32 IsRiskActive(enum Risk risk)
{
    switch (risk)
    {
    case RISK_HAS_STURDY:
        return gSaveBlock1Ptr->risks.hasSturdy;
    case RISK_HAS_MOLD_BREAKER:
        return gSaveBlock1Ptr->risks.hasMoldBreaker;
    case RISK_HAS_FILTER:
        return gSaveBlock1Ptr->risks.hasFilter;
    case RISK_HAS_ADAPTABILITY:
        return gSaveBlock1Ptr->risks.hasAdaptability;
    case RISK_HAS_WONDER_GUARD:
        return gSaveBlock1Ptr->risks.hasWonderGuard;
    case RISK_HAS_REGENERATOR:
        return gSaveBlock1Ptr->risks.hasRegenerator;
    case RISK_CANT_CRIT:
        return gSaveBlock1Ptr->risks.cantCrit;
    case RISK_OPPONENT_MOVES_FIRST:
        return gSaveBlock1Ptr->risks.opponentMovesFirst;
    case RISK_OPPONENT_MORE_MONS:
        return gSaveBlock1Ptr->risks.opponentPartyPlus1;
    case RISK_TURN_LIMIT_1:
        return gSaveBlock1Ptr->risks.turnLimit1;
    case RISK_TURN_LIMIT_2:
        return gSaveBlock1Ptr->risks.turnLimit2;
    case RISK_TURN_LIMIT_3:
        return gSaveBlock1Ptr->risks.turnLimit3;
    case RISK_FLIP_TYPE_CHART:
        return gSaveBlock1Ptr->risks.flipTypeChart;
    case RISK_ATTACK_GETS_DROWSY:
        return gSaveBlock1Ptr->risks.attackGetsDrowsy;
    case RISK_STATUS_GETS_PARA:
        return gSaveBlock1Ptr->risks.statusGetsPara;
    case RISK_FOE_HAS_METRONOME:
        return gSaveBlock1Ptr->risks.foeHasMetronome;
    case RISK_PLAYER_HAS_NEGATIVE_METRONOME:
        return gSaveBlock1Ptr->risks.playerHasNegativeMetronome;
    case RISK_PLAYER_HAS_RECOIL:
        return gSaveBlock1Ptr->risks.playerHasRecoil;
    case RISK_OPPONENT_INFLICTS_GASTRO_ACID:
        return gSaveBlock1Ptr->risks.opponentInflictsGastroAcid;
    case RISK_OPPONENT_ATTACKS_SWITCHES:
        return gSaveBlock1Ptr->risks.opponentAttacksSwitches;
    case RISK_OPPONENT_ATTACKS_DISABLE:
        return gSaveBlock1Ptr->risks.opponentAttacksDisable;
    case RISK_OPPONENT_ATTACKS_TORMENT:
        return gSaveBlock1Ptr->risks.opponentAttacksTorment;
    case RISK_PLAYER_STARTS_WITH_BURN:
        return gSaveBlock1Ptr->risks.playerStartsWithBurn;
    case RISK_PLAYER_STARTS_WITH_PARALYSIS:
        return gSaveBlock1Ptr->risks.playerStartsWithParalysis;
    case RISK_PLAYER_STARTS_WITH_FROSTBITE:
        return gSaveBlock1Ptr->risks.playerStartsWithFrostbite;
    case RISK_PERMANENT_SUN:
        return gSaveBlock1Ptr->risks.permanentSun;
    case RISK_HAS_OMNISCIENT_AI:
        return gSaveBlock1Ptr->risks.hasOmniscientAi;
    case RISK_HAS_PREDICTION_AI:
        return gSaveBlock1Ptr->risks.hasPredictionAi;
    case RISK_HAS_GUARANTEED_EFFECTS:
        return gSaveBlock1Ptr->risks.hasGuaranteedEffects;
    case RISK_HAS_GUARANTEED_ACCURACY:
        return gSaveBlock1Ptr->risks.hasGuaranteedAccuracy;
    case RISK_HAS_GEN_1_CRIT_CHANCE:
        return gSaveBlock1Ptr->risks.hasGen1CritChance;
    case RISK_PLAYER_LOWER_DAMAGE_ROLLS:
        return gSaveBlock1Ptr->risks.playerLowerHalfDamageRolls;
    case RISK_OPPONENT_HIGHER_DAMAGE_ROLLS:
        return gSaveBlock1Ptr->risks.opponentUpperHalfDamageRolls;
    case RISK_CAN_ONLY_USE_TOP_MOVES:
        return gSaveBlock1Ptr->risks.canOnlyUseTopMoves1;
    case RISK_PLAYER_SPIKES_1:
        return gSaveBlock1Ptr->risks.playerStartsSpikes1;
    case RISK_PLAYER_SPIKES_2:
        return gSaveBlock1Ptr->risks.playerStartsSpikes2;
    case RISK_PLAYER_SPIKES_3:
        return gSaveBlock1Ptr->risks.playerStartsSpikes3;
    case RISK_PLAYER_TOXIC_SPIKES_1:
        return gSaveBlock1Ptr->risks.playerStartsTSpikes1;
    case RISK_PLAYER_TOXIC_SPIKES_2:
        return gSaveBlock1Ptr->risks.playerStartsTSpikes2;
    case RISK_PLAYER_STICKY_WEB:
        return gSaveBlock1Ptr->risks.playerStartsStickyWeb;
    case RISK_PLAYER_STEALTH_ROCK:
        return gSaveBlock1Ptr->risks.playerStartsStealthRock;
    case RISK_PLAYER_SHARP_STEEL:
        return gSaveBlock1Ptr->risks.playerStartsSharpSteel;
    case RISK_PLAYER_HAZARDS_NOT_REMOVABLE:
        return gSaveBlock1Ptr->risks.playerHazardsNotRemovable;
    case RISK_OPPONENT_HP_1:
        return gSaveBlock1Ptr->risks.opponentHP1;
    case RISK_OPPONENT_HP_2:
        return gSaveBlock1Ptr->risks.opponentHP2;
    case RISK_OPPONENT_HP_3:
        return gSaveBlock1Ptr->risks.opponentHP3;
    case RISK_CANT_SWITCH:
        return gSaveBlock1Ptr->risks.cantSwitch;
    case RISK_MUST_SWITCH_1:
        return gSaveBlock1Ptr->risks.mustSwitch1;
    case RISK_MUST_SWITCH_2:
        return gSaveBlock1Ptr->risks.mustSwitch2;
    case RISK_MUST_SWITCH_3:
        return gSaveBlock1Ptr->risks.mustSwitch3;
    case RISK_PLAYER_JUST_BERRIES:
        return gSaveBlock1Ptr->risks.playerJustBerries;
    case RISK_OPPONENT_HAS_ITEMS:
        return gSaveBlock1Ptr->risks.opponentHasItems;
    }
    return FALSE;
}

void SetRisk(enum Risk risk)
{
    switch (risk)
    {
    case RISK_HAS_STURDY:
        gSaveBlock1Ptr->risks.hasSturdy = TRUE;
        break;
    case RISK_HAS_MOLD_BREAKER:
        gSaveBlock1Ptr->risks.hasMoldBreaker = TRUE;
        break;
    case RISK_HAS_FILTER:
        gSaveBlock1Ptr->risks.hasFilter = TRUE;
        break;
    case RISK_HAS_ADAPTABILITY:
        gSaveBlock1Ptr->risks.hasAdaptability = TRUE;
        break;
    case RISK_HAS_WONDER_GUARD:
        gSaveBlock1Ptr->risks.hasWonderGuard = TRUE;
        break;
    case RISK_HAS_REGENERATOR:
        gSaveBlock1Ptr->risks.hasRegenerator = TRUE;
        break;
    case RISK_CANT_CRIT:
        gSaveBlock1Ptr->risks.cantCrit = TRUE;
        break;
    case RISK_OPPONENT_MOVES_FIRST:
        gSaveBlock1Ptr->risks.opponentMovesFirst = TRUE;
        break;
    case RISK_OPPONENT_MORE_MONS:
        gSaveBlock1Ptr->risks.opponentPartyPlus1 = TRUE;
        break;
    case RISK_TURN_LIMIT_1:
        gSaveBlock1Ptr->risks.turnLimit1 = TRUE;
        break;
    case RISK_TURN_LIMIT_2:
        gSaveBlock1Ptr->risks.turnLimit2 = TRUE;
        break;
    case RISK_TURN_LIMIT_3:
        gSaveBlock1Ptr->risks.turnLimit3 = TRUE;
        break;
    case RISK_FLIP_TYPE_CHART:
        gSaveBlock1Ptr->risks.flipTypeChart = TRUE;
        break;
    case RISK_ATTACK_GETS_DROWSY:
        gSaveBlock1Ptr->risks.attackGetsDrowsy = TRUE;
        break;
    case RISK_STATUS_GETS_PARA:
        gSaveBlock1Ptr->risks.statusGetsPara = TRUE;
        break;
    case RISK_FOE_HAS_METRONOME:
        gSaveBlock1Ptr->risks.foeHasMetronome = TRUE;
        break;
    case RISK_PLAYER_HAS_NEGATIVE_METRONOME:
        gSaveBlock1Ptr->risks.playerHasNegativeMetronome = TRUE;
        break;
    case RISK_PLAYER_HAS_RECOIL:
        gSaveBlock1Ptr->risks.playerHasRecoil = TRUE;
        break;
    case RISK_OPPONENT_INFLICTS_GASTRO_ACID:
        gSaveBlock1Ptr->risks.opponentInflictsGastroAcid = TRUE;
        break;
    case RISK_OPPONENT_ATTACKS_SWITCHES:
        gSaveBlock1Ptr->risks.opponentAttacksSwitches = TRUE;
        break;
    case RISK_OPPONENT_ATTACKS_DISABLE:
        gSaveBlock1Ptr->risks.opponentAttacksDisable = TRUE;
        break;
    case RISK_OPPONENT_ATTACKS_TORMENT:
        gSaveBlock1Ptr->risks.opponentAttacksTorment = TRUE;
        break;
    case RISK_PLAYER_STARTS_WITH_BURN:
        gSaveBlock1Ptr->risks.playerStartsWithBurn = TRUE;
        break;
    case RISK_PLAYER_STARTS_WITH_PARALYSIS:
        gSaveBlock1Ptr->risks.playerStartsWithParalysis = TRUE;
        break;
    case RISK_PLAYER_STARTS_WITH_FROSTBITE:
        gSaveBlock1Ptr->risks.playerStartsWithFrostbite = TRUE;
        break;
    case RISK_PERMANENT_SUN:
        gSaveBlock1Ptr->risks.permanentSun = TRUE;
        break;
    case RISK_HAS_OMNISCIENT_AI:
        gSaveBlock1Ptr->risks.hasOmniscientAi = TRUE;
        break;
    case RISK_HAS_PREDICTION_AI:
        gSaveBlock1Ptr->risks.hasPredictionAi = TRUE;
        break;
    case RISK_HAS_GUARANTEED_EFFECTS:
        gSaveBlock1Ptr->risks.hasGuaranteedEffects = TRUE;
        break;
    case RISK_HAS_GUARANTEED_ACCURACY:
        gSaveBlock1Ptr->risks.hasGuaranteedAccuracy = TRUE;
        break;
    case RISK_HAS_GEN_1_CRIT_CHANCE:
        gSaveBlock1Ptr->risks.hasGen1CritChance = TRUE;
        break;
    case RISK_PLAYER_LOWER_DAMAGE_ROLLS:
        gSaveBlock1Ptr->risks.playerLowerHalfDamageRolls = TRUE;
        break;
    case RISK_OPPONENT_HIGHER_DAMAGE_ROLLS:
        gSaveBlock1Ptr->risks.opponentUpperHalfDamageRolls = TRUE;
        break;
    case RISK_CAN_ONLY_USE_TOP_MOVES:
        gSaveBlock1Ptr->risks.canOnlyUseTopMoves1 = TRUE;
        break;
    case RISK_PLAYER_SPIKES_1:
        gSaveBlock1Ptr->risks.playerStartsSpikes1 = TRUE;
        break;
    case RISK_PLAYER_SPIKES_2:
        gSaveBlock1Ptr->risks.playerStartsSpikes2 = TRUE;
        break;
    case RISK_PLAYER_SPIKES_3:
        gSaveBlock1Ptr->risks.playerStartsSpikes3 = TRUE;
        break;
    case RISK_PLAYER_TOXIC_SPIKES_1:
        gSaveBlock1Ptr->risks.playerStartsTSpikes1 = TRUE;
        break;
    case RISK_PLAYER_TOXIC_SPIKES_2:
        gSaveBlock1Ptr->risks.playerStartsTSpikes2 = TRUE;
        break;
    case RISK_PLAYER_STICKY_WEB:
        gSaveBlock1Ptr->risks.playerStartsStickyWeb = TRUE;
        break;
    case RISK_PLAYER_STEALTH_ROCK:
        gSaveBlock1Ptr->risks.playerStartsStealthRock = TRUE;
        break;
    case RISK_PLAYER_SHARP_STEEL:
        gSaveBlock1Ptr->risks.playerStartsSharpSteel = TRUE;
        break;
    case RISK_PLAYER_HAZARDS_NOT_REMOVABLE:
        gSaveBlock1Ptr->risks.playerHazardsNotRemovable = TRUE;
        break;
    case RISK_OPPONENT_HP_1:
        gSaveBlock1Ptr->risks.opponentHP1 = TRUE;
        break;
    case RISK_OPPONENT_HP_2:
        gSaveBlock1Ptr->risks.opponentHP2 = TRUE;
        break;
    case RISK_OPPONENT_HP_3:
        gSaveBlock1Ptr->risks.opponentHP3 = TRUE;
        break;
    case RISK_CANT_SWITCH:
        gSaveBlock1Ptr->risks.cantSwitch = TRUE;
        break;
    case RISK_MUST_SWITCH_1:
        gSaveBlock1Ptr->risks.mustSwitch1 = TRUE;
        break;
    case RISK_MUST_SWITCH_2:
        gSaveBlock1Ptr->risks.mustSwitch2 = TRUE;
        break;
    case RISK_MUST_SWITCH_3:
        gSaveBlock1Ptr->risks.mustSwitch3 = TRUE;
        break;
    case RISK_PLAYER_JUST_BERRIES:
        gSaveBlock1Ptr->risks.playerJustBerries = TRUE;
        break;
    case RISK_OPPONENT_HAS_ITEMS:
        gSaveBlock1Ptr->risks.opponentHasItems = TRUE;
        break;
    }
}

void ClearRisk(enum Risk risk)
{
    switch (risk)
    {
    case RISK_HAS_STURDY:
        gSaveBlock1Ptr->risks.hasSturdy = FALSE;
        break;
    case RISK_HAS_MOLD_BREAKER:
        gSaveBlock1Ptr->risks.hasMoldBreaker = FALSE;
        break;
    case RISK_HAS_FILTER:
        gSaveBlock1Ptr->risks.hasFilter = FALSE;
        break;
    case RISK_HAS_ADAPTABILITY:
        gSaveBlock1Ptr->risks.hasAdaptability = FALSE;
        break;
    case RISK_HAS_WONDER_GUARD:
        gSaveBlock1Ptr->risks.hasWonderGuard = FALSE;
        break;
    case RISK_HAS_REGENERATOR:
        gSaveBlock1Ptr->risks.hasRegenerator = FALSE;
        break;
    case RISK_CANT_CRIT:
        gSaveBlock1Ptr->risks.cantCrit = FALSE;
        break;
    case RISK_OPPONENT_MOVES_FIRST:
        gSaveBlock1Ptr->risks.opponentMovesFirst = FALSE;
        break;
    case RISK_OPPONENT_MORE_MONS:
        gSaveBlock1Ptr->risks.opponentPartyPlus1 = FALSE;
        break;
    case RISK_TURN_LIMIT_1:
        gSaveBlock1Ptr->risks.turnLimit1 = FALSE;
        break;
    case RISK_TURN_LIMIT_2:
        gSaveBlock1Ptr->risks.turnLimit2 = FALSE;
        break;
    case RISK_TURN_LIMIT_3:
        gSaveBlock1Ptr->risks.turnLimit3 = FALSE;
        break;
    case RISK_FLIP_TYPE_CHART:
        gSaveBlock1Ptr->risks.flipTypeChart = FALSE;
        break;
    case RISK_ATTACK_GETS_DROWSY:
        gSaveBlock1Ptr->risks.attackGetsDrowsy = FALSE;
        break;
    case RISK_STATUS_GETS_PARA:
        gSaveBlock1Ptr->risks.statusGetsPara = FALSE;
        break;
    case RISK_FOE_HAS_METRONOME:
        gSaveBlock1Ptr->risks.foeHasMetronome = FALSE;
        break;
    case RISK_PLAYER_HAS_NEGATIVE_METRONOME:
        gSaveBlock1Ptr->risks.playerHasNegativeMetronome = FALSE;
        break;
    case RISK_PLAYER_HAS_RECOIL:
        gSaveBlock1Ptr->risks.playerHasRecoil = FALSE;
        break;
    case RISK_OPPONENT_INFLICTS_GASTRO_ACID:
        gSaveBlock1Ptr->risks.opponentInflictsGastroAcid = FALSE;
        break;
    case RISK_OPPONENT_ATTACKS_SWITCHES:
        gSaveBlock1Ptr->risks.opponentAttacksSwitches = FALSE;
        break;
    case RISK_OPPONENT_ATTACKS_DISABLE:
        gSaveBlock1Ptr->risks.opponentAttacksDisable = FALSE;
        break;
    case RISK_OPPONENT_ATTACKS_TORMENT:
        gSaveBlock1Ptr->risks.opponentAttacksTorment = FALSE;
        break;
    case RISK_PLAYER_STARTS_WITH_BURN:
        gSaveBlock1Ptr->risks.playerStartsWithBurn = FALSE;
        break;
    case RISK_PLAYER_STARTS_WITH_PARALYSIS:
        gSaveBlock1Ptr->risks.playerStartsWithParalysis = FALSE;
        break;
    case RISK_PLAYER_STARTS_WITH_FROSTBITE:
        gSaveBlock1Ptr->risks.playerStartsWithFrostbite = FALSE;
        break;
    case RISK_PERMANENT_SUN:
        gSaveBlock1Ptr->risks.permanentSun = FALSE;
        break;
    case RISK_HAS_OMNISCIENT_AI:
        gSaveBlock1Ptr->risks.hasOmniscientAi = FALSE;
        break;
    case RISK_HAS_PREDICTION_AI:
        gSaveBlock1Ptr->risks.hasPredictionAi = FALSE;
        break;
    case RISK_HAS_GUARANTEED_EFFECTS:
        gSaveBlock1Ptr->risks.hasGuaranteedEffects = FALSE;
        break;
    case RISK_HAS_GUARANTEED_ACCURACY:
        gSaveBlock1Ptr->risks.hasGuaranteedAccuracy = FALSE;
        break;
    case RISK_HAS_GEN_1_CRIT_CHANCE:
        gSaveBlock1Ptr->risks.hasGen1CritChance = FALSE;
        break;
    case RISK_PLAYER_LOWER_DAMAGE_ROLLS:
        gSaveBlock1Ptr->risks.playerLowerHalfDamageRolls = FALSE;
        break;
    case RISK_OPPONENT_HIGHER_DAMAGE_ROLLS:
        gSaveBlock1Ptr->risks.opponentUpperHalfDamageRolls = FALSE;
        break;
    case RISK_CAN_ONLY_USE_TOP_MOVES:
        gSaveBlock1Ptr->risks.canOnlyUseTopMoves1 = FALSE;
        break;
    case RISK_PLAYER_SPIKES_1:
        gSaveBlock1Ptr->risks.playerStartsSpikes1 = FALSE;
        break;
    case RISK_PLAYER_SPIKES_2:
        gSaveBlock1Ptr->risks.playerStartsSpikes2 = FALSE;
        break;
    case RISK_PLAYER_SPIKES_3:
        gSaveBlock1Ptr->risks.playerStartsSpikes3 = FALSE;
        break;
    case RISK_PLAYER_TOXIC_SPIKES_1:
        gSaveBlock1Ptr->risks.playerStartsTSpikes1 = FALSE;
        break;
    case RISK_PLAYER_TOXIC_SPIKES_2:
        gSaveBlock1Ptr->risks.playerStartsTSpikes2 = FALSE;
        break;
    case RISK_PLAYER_STICKY_WEB:
        gSaveBlock1Ptr->risks.playerStartsStickyWeb = FALSE;
        break;
    case RISK_PLAYER_STEALTH_ROCK:
        gSaveBlock1Ptr->risks.playerStartsStealthRock = FALSE;
        break;
    case RISK_PLAYER_SHARP_STEEL:
        gSaveBlock1Ptr->risks.playerStartsSharpSteel = FALSE;
        break;
    case RISK_PLAYER_HAZARDS_NOT_REMOVABLE:
        gSaveBlock1Ptr->risks.playerHazardsNotRemovable = FALSE;
        break;
    case RISK_OPPONENT_HP_1:
        gSaveBlock1Ptr->risks.opponentHP1 = FALSE;
        break;
    case RISK_OPPONENT_HP_2:
        gSaveBlock1Ptr->risks.opponentHP2 = FALSE;
        break;
    case RISK_OPPONENT_HP_3:
        gSaveBlock1Ptr->risks.opponentHP3 = FALSE;
        break;
    case RISK_CANT_SWITCH:
        gSaveBlock1Ptr->risks.cantSwitch = FALSE;
        break;
    case RISK_MUST_SWITCH_1:
        gSaveBlock1Ptr->risks.mustSwitch1 = FALSE;
        break;
    case RISK_MUST_SWITCH_2:
        gSaveBlock1Ptr->risks.mustSwitch2 = FALSE;
        break;
    case RISK_MUST_SWITCH_3:
        gSaveBlock1Ptr->risks.mustSwitch3 = FALSE;
        break;
    case RISK_PLAYER_JUST_BERRIES:
        gSaveBlock1Ptr->risks.playerJustBerries = FALSE;
        break;
    case RISK_OPPONENT_HAS_ITEMS:
        gSaveBlock1Ptr->risks.opponentHasItems = FALSE;
        break;
    }
}
