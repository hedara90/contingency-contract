#ifndef GUARD_RISK
#define GUARD_RISK

#include "gba/types.h"
#include "global.h"

#if TESTING
#define TURN_LIMIT_1 3
#define TURN_LIMIT_2 2
#define TURN_LIMIT_3 1
#else
#define TURN_LIMIT_1 20
#define TURN_LIMIT_2 15
#define TURN_LIMIT_3 10
#endif

#define MUST_SWITCH_TURN_LIMIT_1 4
#define MUST_SWITCH_TURN_LIMIT_2 3
#define MUST_SWITCH_TURN_LIMIT_3 2

enum Risk
{
    RISK_HAS_STURDY,
    RISK_HAS_MOLD_BREAKER,
    RISK_HAS_FILTER,
    RISK_HAS_ADAPTABILITY,
    RISK_HAS_WONDER_GUARD,
    RISK_HAS_REGENERATOR,
    RISK_CANT_CRIT,
    RISK_OPPONENT_MOVES_FIRST,
    RISK_OPPONENT_MORE_MONS,
    RISK_TURN_LIMIT_1,
    RISK_TURN_LIMIT_2,
    RISK_TURN_LIMIT_3,
    RISK_FLIP_TYPE_CHART,
    RISK_ATTACK_GETS_DROWSY,
    RISK_STATUS_GETS_PARA,
    RISK_FOE_HAS_METRONOME,
    RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    RISK_PLAYER_HAS_RECOIL,
    RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    RISK_OPPONENT_ATTACKS_SWITCHES,
    RISK_OPPONENT_ATTACKS_DISABLE,
    RISK_OPPONENT_ATTACKS_TORMENT,
    RISK_PLAYER_STARTS_WITH_BURN,
    RISK_PLAYER_STARTS_WITH_PARALYSIS,
    RISK_PLAYER_STARTS_WITH_FROSTBITE,
    RISK_PERMANENT_SUN,
    RISK_HAS_OMNISCIENT_AI,
    RISK_HAS_PREDICTION_AI,
    RISK_HAS_GUARANTEED_EFFECTS,
    RISK_HAS_GUARANTEED_ACCURACY,
    RISK_HAS_GEN_1_CRIT_CHANCE,
    RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    RISK_PLAYER_HAS_PARENTAL_BOND,
    RISK_PLAYER_SPIKES_1,
    RISK_PLAYER_SPIKES_2,
    RISK_PLAYER_SPIKES_3,
    RISK_PLAYER_TOXIC_SPIKES_1,
    RISK_PLAYER_TOXIC_SPIKES_2,
    RISK_PLAYER_STICKY_WEB,
    RISK_PLAYER_STEALTH_ROCK,
    RISK_PLAYER_SHARP_STEEL,
    RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    RISK_OPPONENT_HP_1,
    RISK_OPPONENT_HP_2,
    RISK_OPPONENT_HP_3,
    RISK_CANT_SWITCH,
    RISK_MUST_SWITCH_1,
    RISK_MUST_SWITCH_2,
    RISK_MUST_SWITCH_3,
    RISK_PLAYER_JUST_BERRIES,
    RISK_OPPONENT_HAS_ITEMS,
    RISK_PARTY_MINUS_1,
    RISK_PARTY_MINUS_2,
    RISK_PARTY_MINUS_3,
    RISK_MINUS_1_MOVE,
    RISK_PLAYER_HAS_PERISH_BODY,
    RISK_PLAYER_HAS_BEAST_BOOST,
};

struct Risks
{
    bool32 hasSturdy:1;
    bool32 hasMoldBreaker:1;
    bool32 hasFilter:1;
    bool32 hasAdaptability:1;
    bool32 hasWonderGuard:1;
    bool32 hasRegenerator:1;
    bool32 cantCrit:1;
    bool32 opponentMovesFirst:1;
    bool32 opponentPartyPlus1:1;
    bool32 turnLimit1:1;
    bool32 turnLimit2:1;
    bool32 turnLimit3:1;
    bool32 flipTypeChart:1;
    bool32 attackGetsDrowsy:1;
    bool32 statusGetsPara:1;
    bool32 foeHasMetronome:1;
    bool32 playerHasNegativeMetronome:1;
    bool32 playerHasRecoil:1;
    bool32 opponentInflictsGastroAcid:1;
    bool32 opponentAttacksSwitches:1;
    bool32 opponentAttacksDisable:1;
    bool32 opponentAttacksTorment:1;
    bool32 playerStartsWithBurn:1;
    bool32 playerStartsWithParalysis:1;
    bool32 playerStartsWithFrostbite:1;
    bool32 permanentSun:1;  //  Not implemented yet
    bool32 hasOmniscientAi:1;
    bool32 hasPredictionAi:1;
    bool32 hasGuaranteedEffects:1;
    bool32 hasGuaranteedAccuracy:1;
    bool32 hasGen1CritChance:1;
    bool32 playerLowerHalfDamageRolls:1;
    bool32 opponentUpperHalfDamageRolls:1;
    bool32 playerHasParentalBond:1;
    bool32 playerStartsSpikes1:1;
    bool32 playerStartsSpikes2:1;
    bool32 playerStartsSpikes3:1;
    bool32 playerStartsTSpikes1:1;
    bool32 playerStartsTSpikes2:1;
    bool32 playerStartsStickyWeb:1;
    bool32 playerStartsStealthRock:1;
    bool32 playerStartsSharpSteel:1;
    bool32 playerHazardsNotRemovable:1;
    bool32 opponentHP1:1;
    bool32 opponentHP2:1;
    bool32 opponentHP3:1;
    bool32 cantSwitch:1;
    bool32 mustSwitch1:1;
    bool32 mustSwitch2:1;
    bool32 mustSwitch3:1;
    bool32 playerJustBerries:1;
    bool32 opponentHasItems:1;
    bool32 partyMinus1:1;
    bool32 partyMinus2:1;
    bool32 partyMinus3:1;
    bool32 minus1Move:1;
    bool32 playerPerishBody:1;
    bool32 playerBeastBoost:1;
    bool32 padding:7;
};

void ClearRisks(void);

bool32 IsRiskActive(enum Risk risk);
void SetRisk(enum Risk risk);
void ClearRisk(enum Risk risk);

#endif // GUARD_RISK
