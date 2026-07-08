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
    bool32 padding:1;
};

extern struct Risks gRisks;

void ClearRisks(void);

#endif // GUARD_RISK
