#include "risk_ui.h"
#include "constants/species.h"
#include "gba/types.h"
#include "bg.h"
#include "decompress.h"
#include "global.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "line_break.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "move.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "pokemon_icon.h"

#include "constants/abilities.h"
#include "constants/characters.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "palette.h"
#include "even_sprite.h"

#define CURSOR_SPEED 2
#define COORD_TO_TILE(_x, _y) ((_x / 32 * 1024) + (_y / 32 * 2048) + (_x % 32) + ((_y % 32) * 32))

#define PAL_INDEX_INACTIVE 0
#define PAL_INDEX_ACTIVE 1
#define PAL_INDEX_LOCKED 2

struct RiskUiState
{
    MainCallback savedCallback;
    u8 loadState;
    s16 xSelector;
    s16 ySelector;
    s16 xOffset;
    s16 yOffset;
    u8 selectorId;
    u8 totalIconId;
    u8 descriptionOffset;
    bool8 isShowingDescription;
    enum Risk prevRisk;
};

enum WindowIds
{
    WIN_RISK_NAME,
    WIN_RISK_DESCRIPTION,
    WIN_RISK_TOTAL,
    WIN_COUNT
};

struct RiskIcon
{
    const enum Risk *linkedRisks;
    const enum Risk *unlockedRisks;
    u16 tiles[4];
    enum Risk lockRisk:16;
    u8 linkedCount;
    u8 unlockCount;
    const u8 *name;
    const u8 *description;
};

const enum Risk sLinkedHpRisks[] = { RISK_OPPONENT_HP_1, RISK_OPPONENT_HP_2, RISK_OPPONENT_HP_3 };
const enum Risk sLinkedTurnRisks[] = { RISK_TURN_LIMIT_1, RISK_TURN_LIMIT_2, RISK_TURN_LIMIT_3 };
const enum Risk sLinkedTeamRisks[] = { RISK_PARTY_MINUS_1, RISK_PARTY_MINUS_2, RISK_PARTY_MINUS_3 };
const enum Risk sLinkedSwitchRisks[] = { RISK_MUST_SWITCH_1, RISK_MUST_SWITCH_2, RISK_MUST_SWITCH_3, RISK_CANT_SWITCH };
const enum Risk sLinkedStartStatusRisks[] = { RISK_PLAYER_STARTS_WITH_BURN, RISK_PLAYER_STARTS_WITH_FROSTBITE, RISK_PLAYER_STARTS_WITH_PARALYSIS };
const enum Risk sLinkedOppMoreMons[] = { RISK_OPPONENT_MORE_MONS_1, RISK_OPPONENT_MORE_MONS_2 };
const enum Risk sLinkedAi[] = { RISK_HAS_OMNISCIENT_AI, RISK_HAS_PREDICTION_AI };
const enum Risk sLinkedPool[] = { RISK_RANDOM_LEAD, RISK_USES_POOLS };
const enum Risk sLinkedSpikes[] = { RISK_PLAYER_SPIKES_1, RISK_PLAYER_SPIKES_2, RISK_PLAYER_SPIKES_3 };
const enum Risk sLinkedTSpikes[] = { RISK_PLAYER_TOXIC_SPIKES_1, RISK_PLAYER_TOXIC_SPIKES_2 };
const enum Risk sLinkedAbilities[] = { RISK_PLAYER_HAS_PARENTAL_BOND, RISK_PLAYER_HAS_FILTER, RISK_PLAYER_HAS_PERISH_BODY, RISK_PLAYER_HAS_BEAST_BOOST };

const enum Risk sLockedAbilitiyRisks[] =
{
    RISK_PLAYER_HAS_PARENTAL_BOND, RISK_PLAYER_HAS_FILTER, RISK_PLAYER_HAS_PERISH_BODY, RISK_PLAYER_HAS_BEAST_BOOST,
    RISK_HAS_MOLD_BREAKER, RISK_HAS_STURDY, RISK_HAS_REGENERATOR, RISK_HAS_BATTLE_ARMOR,
    RISK_HAS_WONDER_GUARD, RISK_HAS_FILTER, RISK_HAS_ADAPTABILITY,
};

const enum Risk sLockedHazardRisks[] =
{
    RISK_PLAYER_SPIKES_1, RISK_PLAYER_SPIKES_2, RISK_PLAYER_SPIKES_3,
    RISK_PLAYER_TOXIC_SPIKES_1, RISK_PLAYER_TOXIC_SPIKES_2,
    RISK_PLAYER_STEALTH_ROCK, RISK_PLAYER_SHARP_STEEL, RISK_PLAYER_STICKY_WEB,
};

const struct RiskIcon sRiskData[] =
{
    [RISK_RESET] =
    {
        .linkedRisks = NULL,
        .unlockedRisks = NULL,
        .tiles = { COORD_TO_TILE(1, 1), COORD_TO_TILE(1, 2), COORD_TO_TILE(2, 1), COORD_TO_TILE(2, 2) },
        .linkedCount = 0,
        .unlockCount = 0,
        .name = COMPOUND_STRING("Reset Risks"),
        .description = COMPOUND_STRING("Reset all selected risks"),
    },
    [RISK_OPPONENT_HP_1] =
    {
        .linkedRisks = sLinkedHpRisks,
        .tiles = { COORD_TO_TILE(4, 4), COORD_TO_TILE(4, 5), COORD_TO_TILE(5, 4), COORD_TO_TILE(5, 5) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Foe: HP 1"),
        .description = COMPOUND_STRING("Foes have 10% more HP"),
    },
    [RISK_OPPONENT_HP_2] =
    {
        .linkedRisks = sLinkedHpRisks,
        .tiles = { COORD_TO_TILE(4, 7), COORD_TO_TILE(4, 8), COORD_TO_TILE(5, 7), COORD_TO_TILE(5, 8) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Foe: HP 2"),
        .description = COMPOUND_STRING("Foes have 25% more HP"),
    },
    [RISK_OPPONENT_HP_3] =
    {
        .linkedRisks = sLinkedHpRisks,
        .tiles = { COORD_TO_TILE(4, 10), COORD_TO_TILE(4, 11), COORD_TO_TILE(5, 10), COORD_TO_TILE(5, 11) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Foe: HP 3"),
        .description = COMPOUND_STRING("Foes have 25% more HP"),
    },
    [RISK_TURN_LIMIT_1] =
    {
        .linkedRisks = sLinkedTurnRisks,
        .tiles = { COORD_TO_TILE(8, 4), COORD_TO_TILE(8, 5), COORD_TO_TILE(9, 4), COORD_TO_TILE(9, 5) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Turn Limit 1"),
        .description = COMPOUND_STRING("Must win within 20 turns."),
    },
    [RISK_TURN_LIMIT_2] =
    {
        .linkedRisks = sLinkedTurnRisks,
        .tiles = { COORD_TO_TILE(8, 7), COORD_TO_TILE(8, 8), COORD_TO_TILE(9, 7), COORD_TO_TILE(9, 8) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Turn Limit 2"),
        .description = COMPOUND_STRING("Must win within 15 turns."),
    },
    [RISK_TURN_LIMIT_3] =
    {
        .linkedRisks = sLinkedTurnRisks,
        .tiles = { COORD_TO_TILE(8, 10), COORD_TO_TILE(8, 11), COORD_TO_TILE(9, 10), COORD_TO_TILE(9, 11) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Turn Limit 3"),
        .description = COMPOUND_STRING("Must win within 10 turns."),
    },
    [RISK_PARTY_MINUS_1] =
    {
        .linkedRisks = sLinkedTeamRisks,
        .tiles = { COORD_TO_TILE(12, 4), COORD_TO_TILE(12, 5), COORD_TO_TILE(13, 4), COORD_TO_TILE(13, 5) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Part Minus 1"),
        .description = COMPOUND_STRING("Player can only have 5 mons."),
    },
    [RISK_PARTY_MINUS_2] =
    {
        .linkedRisks = sLinkedTeamRisks,
        .tiles = { COORD_TO_TILE(12, 7), COORD_TO_TILE(12, 8), COORD_TO_TILE(13, 7), COORD_TO_TILE(13, 8) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Part Minus 2"),
        .description = COMPOUND_STRING("Player can only have 4 mons."),
    },
    [RISK_PARTY_MINUS_3] =
    {
        .linkedRisks = sLinkedTeamRisks,
        .tiles = { COORD_TO_TILE(12, 10), COORD_TO_TILE(12, 11), COORD_TO_TILE(13, 10), COORD_TO_TILE(13, 11) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Part Minus 3"),
        .description = COMPOUND_STRING("Player can only have 3 mons."),
    },
    [RISK_MUST_SWITCH_1] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(16, 4), COORD_TO_TILE(16, 5), COORD_TO_TILE(17, 4), COORD_TO_TILE(17, 5) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Must Switch 1"),
        .description = COMPOUND_STRING("Player must switch a mon every 4 turns."),
    },
    [RISK_MUST_SWITCH_2] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(16, 7), COORD_TO_TILE(16, 8), COORD_TO_TILE(17, 7), COORD_TO_TILE(17, 8) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Must Switch 2"),
        .description = COMPOUND_STRING("Player must switch a mon every 3 turns."),
    },
    [RISK_MUST_SWITCH_3] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(16, 10), COORD_TO_TILE(16, 11), COORD_TO_TILE(17, 10), COORD_TO_TILE(17, 11) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Must Switch 3"),
        .description = COMPOUND_STRING("Player must switch a mon every 2 turns."),
    },
    [RISK_CANT_SWITCH] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(19, 7), COORD_TO_TILE(19, 8), COORD_TO_TILE(20, 7), COORD_TO_TILE(20, 8) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Can't Switch"),
        .description = COMPOUND_STRING("Player can't switch."),
    },
    [RISK_PLAYER_STARTS_WITH_BURN] =
    {
        .linkedRisks = sLinkedStartStatusRisks,
        .tiles = { COORD_TO_TILE(23, 7), COORD_TO_TILE(23, 8), COORD_TO_TILE(24, 7), COORD_TO_TILE(24, 8) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Start Burned"),
        .description = COMPOUND_STRING("Player mons start battles burned."),
    },
    [RISK_PLAYER_STARTS_WITH_FROSTBITE] =
    {
        .linkedRisks = sLinkedStartStatusRisks,
        .tiles = { COORD_TO_TILE(26, 7), COORD_TO_TILE(26, 8), COORD_TO_TILE(27, 7), COORD_TO_TILE(27, 8) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Start Frost"),
        .description = COMPOUND_STRING("Player mons start battles frostbitten."),
    },
    [RISK_PLAYER_STARTS_WITH_PARALYSIS] =
    {
        .linkedRisks = sLinkedStartStatusRisks,
        .tiles = { COORD_TO_TILE(29, 7), COORD_TO_TILE(29, 8), COORD_TO_TILE(30, 7), COORD_TO_TILE(30, 8) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Start Para"),
        .description = COMPOUND_STRING("Player mons start battles paralyzed."),
    },
    [RISK_OPPONENT_MORE_MONS_1] =
    {
        .linkedRisks = sLinkedOppMoreMons,
        .tiles = { COORD_TO_TILE(4, 14), COORD_TO_TILE(4, 15), COORD_TO_TILE(5, 14), COORD_TO_TILE(5, 15) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Foe more mons 1"),
        .description = COMPOUND_STRING("Opponent has 1 more mon in their party."),
    },
    [RISK_OPPONENT_MORE_MONS_2] =
    {
        .linkedRisks = sLinkedOppMoreMons,
        .tiles = { COORD_TO_TILE(4, 17), COORD_TO_TILE(4, 18), COORD_TO_TILE(5, 17), COORD_TO_TILE(5, 18) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Foe more mons 2"),
        .description = COMPOUND_STRING("Opponent has 2 more mon in their party."),
    },
    [RISK_HAS_OMNISCIENT_AI] =
    {
        .linkedRisks = sLinkedAi,
        .tiles = { COORD_TO_TILE(8, 14), COORD_TO_TILE(8, 15), COORD_TO_TILE(9, 14), COORD_TO_TILE(9, 15) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("AI Omniscient"),
        .description = COMPOUND_STRING("AI knows the players party, moves and abilities."),
    },
    [RISK_HAS_PREDICTION_AI] =
    {
        .linkedRisks = sLinkedAi,
        .tiles = { COORD_TO_TILE(8, 17), COORD_TO_TILE(8, 18), COORD_TO_TILE(9, 17), COORD_TO_TILE(9, 18) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("AI Predicts"),
        .description = COMPOUND_STRING("AI knows the players party, moves and abilities.\nAI predicts the player's action."),
    },
    [RISK_RANDOM_LEAD] =
    {
        .linkedRisks = sLinkedPool,
        .tiles = { COORD_TO_TILE(12, 14), COORD_TO_TILE(12, 15), COORD_TO_TILE(13, 14), COORD_TO_TILE(13, 15) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Random Lead"),
        .description = COMPOUND_STRING("Opponent has random lead."),
    },
    [RISK_USES_POOLS] =
    {
        .linkedRisks = sLinkedPool,
        .tiles = { COORD_TO_TILE(12, 17), COORD_TO_TILE(12, 18), COORD_TO_TILE(13, 17), COORD_TO_TILE(13, 18) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Party Pools"),
        .description = COMPOUND_STRING("Opponent's party is picked from a pool of mons."),
    },
    [RISK_PLAYER_JUST_BERRIES] =
    {
        .tiles = { COORD_TO_TILE(4, 21), COORD_TO_TILE(4, 22), COORD_TO_TILE(5, 21), COORD_TO_TILE(5, 22) },
        .name = COMPOUND_STRING("Just Berries"),
        .description = COMPOUND_STRING("Player can only use berries as held items."),
    },
    [RISK_NO_PP_RESTORE] =
    {
        .tiles = { COORD_TO_TILE(4, 24), COORD_TO_TILE(4, 25), COORD_TO_TILE(5, 24), COORD_TO_TILE(5, 25) },
        .name = COMPOUND_STRING("PP conservation"),
        .description = COMPOUND_STRING("Move PP doesn't restore between battles"),
    },
    [RISK_OPPONENT_HAS_ITEMS] =
    {
        .tiles = { COORD_TO_TILE(8, 21), COORD_TO_TILE(8, 22), COORD_TO_TILE(9, 21), COORD_TO_TILE(9, 22) },
        .name = COMPOUND_STRING("Well Equipped"),
        .description = COMPOUND_STRING("Opponent mons have items."),
    },
    [RISK_NO_ORDER_CHANGE] =
    {
        .tiles = { COORD_TO_TILE(8, 24), COORD_TO_TILE(8, 25), COORD_TO_TILE(9, 24), COORD_TO_TILE(9, 25) },
        .name = COMPOUND_STRING("Locked in"),
        .description = COMPOUND_STRING("Player can't change party or move order\nbetween battles."),
    },
    [RISK_FLIP_TYPE_CHART] =
    {
        .tiles = { COORD_TO_TILE(12, 21), COORD_TO_TILE(12, 22), COORD_TO_TILE(13, 21), COORD_TO_TILE(13, 22) },
        .name = COMPOUND_STRING("Inverse Battle"),
        .description = COMPOUND_STRING("The type chart is flipped."),
    },
    [RISK_ATTACK_GETS_DROWSY] =
    {
        .tiles = { COORD_TO_TILE(12, 24), COORD_TO_TILE(12, 25), COORD_TO_TILE(13, 24), COORD_TO_TILE(13, 25) },
        .name = COMPOUND_STRING("Exhaustion"),
        .description = COMPOUND_STRING("Using and attack makes player mons drowsy."),
    },
    [RISK_HAS_GEN_1_CRIT_CHANCE] =
    {
        .tiles = { COORD_TO_TILE(15, 21), COORD_TO_TILE(15, 22), COORD_TO_TILE(16, 21), COORD_TO_TILE(16, 22) },
        .name = COMPOUND_STRING("Ancient Crits"),
        .description = COMPOUND_STRING("Crit rate is calculated using Gen 1 formulas.\nFaster mons have a higher chance of critting."),
    },
    [RISK_STATUS_GETS_PARA] =
    {
        .tiles = { COORD_TO_TILE(15, 24), COORD_TO_TILE(15, 25), COORD_TO_TILE(16, 24), COORD_TO_TILE(16, 25) },
        .name = COMPOUND_STRING("Lame"),
        .description = COMPOUND_STRING("Using a status move paralyzes player mons."),
    },
    [RISK_PLAYER_LOWER_DAMAGE_ROLLS] =
    {
        .tiles = { COORD_TO_TILE(22, 28), COORD_TO_TILE(22, 29), COORD_TO_TILE(23, 28), COORD_TO_TILE(23, 29) },
        .name = COMPOUND_STRING("Below Average"),
        .description = COMPOUND_STRING("Player attack rolls use only the lower half of results."),
    },
    [RISK_OPPONENT_HIGHER_DAMAGE_ROLLS] =
    {
        .tiles = { COORD_TO_TILE(22, 31), COORD_TO_TILE(22, 32), COORD_TO_TILE(23, 31), COORD_TO_TILE(23, 32) },
        .name = COMPOUND_STRING("Above Average"),
        .description = COMPOUND_STRING("Opponent attack rolls use only the upper half of the results."),
    },
    [RISK_PLAYER_HAS_NEGATIVE_METRONOME] =
    {
        .tiles = { COORD_TO_TILE(26, 28), COORD_TO_TILE(26, 29), COORD_TO_TILE(27, 28), COORD_TO_TILE(27, 29) },
        .name = COMPOUND_STRING("Metronome Minus"),
        .description = COMPOUND_STRING("Player has a negative Metronome item effect of them.\nConsequtive attacking moves does less damage."),
    },
    [RISK_FOE_HAS_METRONOME] =
    {
        .tiles = { COORD_TO_TILE(26, 31), COORD_TO_TILE(26, 32), COORD_TO_TILE(27, 31), COORD_TO_TILE(27, 32) },
        .name = COMPOUND_STRING("Metronome Plus"),
        .description = COMPOUND_STRING("Opponent has a positive Metronome item effect on them.\nConsequtive attacking moves does more damage."),
    },
    [RISK_PLAYER_HAS_RECOIL] =
    {
        .tiles = { COORD_TO_TILE(30, 28), COORD_TO_TILE(30, 29), COORD_TO_TILE(31, 28), COORD_TO_TILE(31, 29) },
        .name = COMPOUND_STRING("Recoil"),
        .description = COMPOUND_STRING("All player moves have recoil equal to 25%\nof damage dealt."),
    },
    [RISK_HAS_GUARANTEED_ACCURACY] =
    {
        .tiles = { COORD_TO_TILE(30, 31), COORD_TO_TILE(30, 32), COORD_TO_TILE(31, 31), COORD_TO_TILE(31, 32) },
        .name = COMPOUND_STRING("Accurate"),
        .description = COMPOUND_STRING("Opponent can't miss moves"),
    },
    [RISK_OPPONENT_MOVES_FIRST] =
    {
        .tiles = { COORD_TO_TILE(22, 35), COORD_TO_TILE(22, 36), COORD_TO_TILE(23, 35), COORD_TO_TILE(23, 36) },
        .name = COMPOUND_STRING("Foe Fast"),
        .description = COMPOUND_STRING("Opponents moves first in their priority bracket."),
    },
    [RISK_OPPONENT_ATTACKS_SWITCHES] =
    {
        .tiles = { COORD_TO_TILE(22, 38), COORD_TO_TILE(22, 39), COORD_TO_TILE(23, 38), COORD_TO_TILE(23, 39) },
        .name = COMPOUND_STRING("Force Switch"),
        .description = COMPOUND_STRING("Opponent attacks foribly switches the player.\nOpponents also moves last."),
    },
    [RISK_HAS_GUARANTEED_EFFECTS] =
    {
        .tiles = { COORD_TO_TILE(26, 35), COORD_TO_TILE(26, 36), COORD_TO_TILE(27, 35), COORD_TO_TILE(27, 36) },
        .name = COMPOUND_STRING("Graceful"),
        .description = COMPOUND_STRING("Opponent's moves with secondary effects are\nguaranteed to proc those effects."),
    },
    [RISK_OPPONENT_ATTACKS_DISABLE] =
    {
        .tiles = { COORD_TO_TILE(26, 38), COORD_TO_TILE(26, 39), COORD_TO_TILE(27, 38), COORD_TO_TILE(27, 38) },
        .name = COMPOUND_STRING("Disabling"),
        .description = COMPOUND_STRING("Opponents attacks apply the Disable effect."),
    },
    [RISK_OPPONENT_INFLICTS_GASTRO_ACID] =
    {
        .tiles = { COORD_TO_TILE(30, 35), COORD_TO_TILE(30, 36), COORD_TO_TILE(31, 35), COORD_TO_TILE(31, 36) },
        .name = COMPOUND_STRING("Suppressing"),
        .description = COMPOUND_STRING("Opponents attacks apply Gastro Acid\nwhich suppresses abilities."),
    },
    [RISK_OPPONENT_ATTACKS_TORMENT] =
    {
        .tiles = { COORD_TO_TILE(30, 38), COORD_TO_TILE(30, 39), COORD_TO_TILE(31, 38), COORD_TO_TILE(31, 39) },
        .name = COMPOUND_STRING("Tormenting"),
        .description = COMPOUND_STRING("Opponents attacks apply the Torment effect,\npreventing mons from repeating attacks."),
    },
    [RISK_PLAYER_HAZARDS_NOT_REMOVABLE] =
    {
        .unlockedRisks = sLockedHazardRisks,
        .tiles = { COORD_TO_TILE(26, 17), COORD_TO_TILE(26, 18), COORD_TO_TILE(27, 17), COORD_TO_TILE(27, 18) },
        .unlockCount = 8,
        .name = COMPOUND_STRING("Sticky Hazards"),
        .description = COMPOUND_STRING("Hazards can't be removed from the player's\nside of the field."),
    },
    [RISK_PLAYER_SPIKES_1] =
    {
        .linkedRisks = sLinkedSpikes,
        .linkedCount = 3,
        .tiles = { COORD_TO_TILE(23, 13), COORD_TO_TILE(23, 14), COORD_TO_TILE(24, 13), COORD_TO_TILE(24, 14) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Spikes 1"),
        .description = COMPOUND_STRING("Player starts with 1 layer of spikes\non their side of the field."),
    },
    [RISK_PLAYER_SPIKES_2] =
    {
        .linkedRisks = sLinkedSpikes,
        .linkedCount = 3,
        .tiles = { COORD_TO_TILE(23, 16), COORD_TO_TILE(23, 17), COORD_TO_TILE(24, 16), COORD_TO_TILE(24, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Spikes 2"),
        .description = COMPOUND_STRING("Player starts with 2 layer of spikes\non their side of the field."),
    },
    [RISK_PLAYER_SPIKES_3] =
    {
        .linkedRisks = sLinkedSpikes,
        .linkedCount = 3,
        .tiles = { COORD_TO_TILE(23, 19), COORD_TO_TILE(23, 20), COORD_TO_TILE(24, 19), COORD_TO_TILE(24, 20) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Spikes 3"),
        .description = COMPOUND_STRING("Player starts with 3 layer of spikes\non their side of the field."),
    },
    [RISK_PLAYER_TOXIC_SPIKES_1] =
    {
        .linkedRisks = sLinkedTSpikes,
        .linkedCount = 2,
        .tiles = { COORD_TO_TILE(29, 16), COORD_TO_TILE(29, 17), COORD_TO_TILE(30, 16), COORD_TO_TILE(30, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Toxic Spikes 1"),
        .description = COMPOUND_STRING("Player starts with 1 layer of toxic spikes\non their side of the field."),
    },
    [RISK_PLAYER_TOXIC_SPIKES_2] =
    {
        .linkedRisks = sLinkedTSpikes,
        .linkedCount = 2,
        .tiles = { COORD_TO_TILE(29, 19), COORD_TO_TILE(29, 20), COORD_TO_TILE(30, 19), COORD_TO_TILE(30, 20) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Toxic Spikes 2"),
        .description = COMPOUND_STRING("Player starts with 2 layer of toxic spikes\non their side of the field."),
    },
    [RISK_PLAYER_STEALTH_ROCK] =
    {
        .tiles = { COORD_TO_TILE(23, 22), COORD_TO_TILE(23, 23), COORD_TO_TILE(24, 22), COORD_TO_TILE(24, 23) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Stealth Rock"),
        .description = COMPOUND_STRING("Player starts with Stealth Rock\non their side of the field."),
    },
    [RISK_PLAYER_SHARP_STEEL] =
    {
        .tiles = { COORD_TO_TILE(26, 22), COORD_TO_TILE(26, 23), COORD_TO_TILE(27, 22), COORD_TO_TILE(27, 23) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Sharp Steel"),
        .description = COMPOUND_STRING("Player starts with Sharp Steel\non their side of the field."),
    },
    [RISK_PLAYER_STICKY_WEB] =
    {
        .tiles = { COORD_TO_TILE(29, 22), COORD_TO_TILE(29, 23), COORD_TO_TILE(30, 22), COORD_TO_TILE(30, 23) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Sticky Web"),
        .description = COMPOUND_STRING("Player starts with Sticky Web\non their side of the field."),
    },
    [RISK_MINUS_1_MOVE] =
    {
        .unlockedRisks = sLockedAbilitiyRisks,
        .tiles = { COORD_TO_TILE(10, 33), COORD_TO_TILE(10, 34), COORD_TO_TILE(11, 33), COORD_TO_TILE(11, 34) },
        .unlockCount = 11,
        .name = COMPOUND_STRING("Limited Moves"),
        .description = COMPOUND_STRING("Player mons can only use the first 3 moves."),
    },
    [RISK_PLAYER_HAS_PARENTAL_BOND] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(5, 29), COORD_TO_TILE(5, 30), COORD_TO_TILE(6, 29), COORD_TO_TILE(6, 30) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Parental Bond"),
        .description = COMPOUND_STRING("Player mons have Parental Bond.\nPlayer can only use the first 2 moves."),
    },
    [RISK_PLAYER_HAS_FILTER] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(8, 29), COORD_TO_TILE(8, 30), COORD_TO_TILE(9, 29), COORD_TO_TILE(9, 30) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Filter"),
        .description = COMPOUND_STRING("Player mons have Filter.\nPlayer can only use the first 2 moves."),
    },
    [RISK_PLAYER_HAS_PERISH_BODY] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(12, 29), COORD_TO_TILE(12, 30), COORD_TO_TILE(13, 29), COORD_TO_TILE(13, 30) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Perish Body"),
        .description = COMPOUND_STRING("Player mons have Perish Body.\nPlayer can only use the first 2 moves."),
    },
    [RISK_PLAYER_HAS_BEAST_BOOST] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(15, 29), COORD_TO_TILE(15, 30), COORD_TO_TILE(16, 29), COORD_TO_TILE(16, 30) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Beast Boost"),
        .description = COMPOUND_STRING("Player mons have Beast Boost.\nPlayer can only use the first 2 moves."),
    },
    [RISK_HAS_MOLD_BREAKER] =
    {
        .tiles = { COORD_TO_TILE(5, 36), COORD_TO_TILE(5, 37), COORD_TO_TILE(6, 36), COORD_TO_TILE(6, 37) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Mold Breaker"),
        .description = COMPOUND_STRING("Opponent mons have Mold Breaker."),
    },
    [RISK_HAS_STURDY] =
    {
        .tiles = { COORD_TO_TILE(8, 36), COORD_TO_TILE(8, 37), COORD_TO_TILE(9, 36), COORD_TO_TILE(9, 37) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Sturdy"),
        .description = COMPOUND_STRING("Opponent mons have Sturdy."),
    },
    [RISK_HAS_REGENERATOR] =
    {
        .tiles = { COORD_TO_TILE(12, 36), COORD_TO_TILE(12, 37), COORD_TO_TILE(13, 36), COORD_TO_TILE(13, 37) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Regenerator"),
        .description = COMPOUND_STRING("Opponent mons have Regenerator."),
    },
    [RISK_HAS_BATTLE_ARMOR] =
    {
        .tiles = { COORD_TO_TILE(15, 36), COORD_TO_TILE(15, 37), COORD_TO_TILE(16, 36), COORD_TO_TILE(16, 37) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Battle Armor"),
        .description = COMPOUND_STRING("Opponent mons have Battle Armor."),
    },
    [RISK_HAS_WONDER_GUARD] =
    {
        .tiles = { COORD_TO_TILE(6, 39), COORD_TO_TILE(6, 40), COORD_TO_TILE(7, 39), COORD_TO_TILE(7, 40) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Wonder Guard"),
        .description = COMPOUND_STRING("Opponent mons have Wonder Guard.\nThere's a reason why only Shedinja\nhas this normally…"),
    },
    [RISK_HAS_FILTER] =
    {
        .tiles = { COORD_TO_TILE(10, 39), COORD_TO_TILE(10, 40), COORD_TO_TILE(11, 39), COORD_TO_TILE(11, 40) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Filter"),
        .description = COMPOUND_STRING("Opponent mons have Filter."),
    },
    [RISK_HAS_ADAPTABILITY] =
    {
        .tiles = { COORD_TO_TILE(14, 39), COORD_TO_TILE(14, 40), COORD_TO_TILE(15, 39), COORD_TO_TILE(15, 40) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Foe Adaptability"),
        .description = COMPOUND_STRING("Opponent mons have Adaptability."),
    },
};

const enum Risk sRiskMap[64][64] =
{
    [1][1] = RISK_RESET,
    [1][2] = RISK_RESET,
    [2][1] = RISK_RESET,
    [2][2] = RISK_RESET,

    [4][4] = RISK_OPPONENT_HP_1,
    [4][5] = RISK_OPPONENT_HP_1,
    [5][4] = RISK_OPPONENT_HP_1,
    [5][5] = RISK_OPPONENT_HP_1,

    [4][7] = RISK_OPPONENT_HP_2,
    [4][8] = RISK_OPPONENT_HP_2,
    [5][7] = RISK_OPPONENT_HP_2,
    [5][8] = RISK_OPPONENT_HP_2,

    [4][10] = RISK_OPPONENT_HP_3,
    [4][11] = RISK_OPPONENT_HP_3,
    [5][10] = RISK_OPPONENT_HP_3,
    [5][11] = RISK_OPPONENT_HP_3,

    [8][4] = RISK_TURN_LIMIT_1,
    [8][5] = RISK_TURN_LIMIT_1,
    [9][4] = RISK_TURN_LIMIT_1,
    [9][5] = RISK_TURN_LIMIT_1,

    [8][7] = RISK_TURN_LIMIT_2,
    [8][8] = RISK_TURN_LIMIT_2,
    [9][7] = RISK_TURN_LIMIT_2,
    [9][8] = RISK_TURN_LIMIT_2,

    [8][10] = RISK_TURN_LIMIT_3,
    [8][11] = RISK_TURN_LIMIT_3,
    [9][10] = RISK_TURN_LIMIT_3,
    [9][11] = RISK_TURN_LIMIT_3,

    [12][4] = RISK_PARTY_MINUS_1,
    [12][5] = RISK_PARTY_MINUS_1,
    [13][4] = RISK_PARTY_MINUS_1,
    [13][5] = RISK_PARTY_MINUS_1,

    [12][7] = RISK_PARTY_MINUS_2,
    [12][8] = RISK_PARTY_MINUS_2,
    [13][7] = RISK_PARTY_MINUS_2,
    [13][8] = RISK_PARTY_MINUS_2,

    [12][10] = RISK_PARTY_MINUS_3,
    [12][11] = RISK_PARTY_MINUS_3,
    [13][10] = RISK_PARTY_MINUS_3,
    [13][11] = RISK_PARTY_MINUS_3,

    [16][4] = RISK_MUST_SWITCH_1,
    [16][5] = RISK_MUST_SWITCH_1,
    [17][4] = RISK_MUST_SWITCH_1,
    [17][5] = RISK_MUST_SWITCH_1,

    [16][7] = RISK_MUST_SWITCH_2,
    [16][8] = RISK_MUST_SWITCH_2,
    [17][7] = RISK_MUST_SWITCH_2,
    [17][8] = RISK_MUST_SWITCH_2,

    [16][10] = RISK_MUST_SWITCH_3,
    [16][11] = RISK_MUST_SWITCH_3,
    [17][10] = RISK_MUST_SWITCH_3,
    [17][11] = RISK_MUST_SWITCH_3,

    [19][7] = RISK_CANT_SWITCH,
    [19][8] = RISK_CANT_SWITCH,
    [20][7] = RISK_CANT_SWITCH,
    [20][8] = RISK_CANT_SWITCH,

    [23][7] = RISK_PLAYER_STARTS_WITH_BURN,
    [23][8] = RISK_PLAYER_STARTS_WITH_BURN,
    [24][7] = RISK_PLAYER_STARTS_WITH_BURN,
    [24][8] = RISK_PLAYER_STARTS_WITH_BURN,

    [26][7] = RISK_PLAYER_STARTS_WITH_FROSTBITE,
    [26][8] = RISK_PLAYER_STARTS_WITH_FROSTBITE,
    [27][7] = RISK_PLAYER_STARTS_WITH_FROSTBITE,
    [27][8] = RISK_PLAYER_STARTS_WITH_FROSTBITE,

    [29][7] = RISK_PLAYER_STARTS_WITH_PARALYSIS,
    [29][8] = RISK_PLAYER_STARTS_WITH_PARALYSIS,
    [30][7] = RISK_PLAYER_STARTS_WITH_PARALYSIS,
    [30][8] = RISK_PLAYER_STARTS_WITH_PARALYSIS,

    [4][14] = RISK_OPPONENT_MORE_MONS_1,
    [4][15] = RISK_OPPONENT_MORE_MONS_1,
    [5][14] = RISK_OPPONENT_MORE_MONS_1,
    [5][15] = RISK_OPPONENT_MORE_MONS_1,

    [4][17] = RISK_OPPONENT_MORE_MONS_2,
    [4][18] = RISK_OPPONENT_MORE_MONS_2,
    [5][17] = RISK_OPPONENT_MORE_MONS_2,
    [5][18] = RISK_OPPONENT_MORE_MONS_2,

    [8][14] = RISK_HAS_OMNISCIENT_AI,
    [8][15] = RISK_HAS_OMNISCIENT_AI,
    [9][14] = RISK_HAS_OMNISCIENT_AI,
    [9][15] = RISK_HAS_OMNISCIENT_AI,

    [8][17] = RISK_HAS_PREDICTION_AI,
    [8][18] = RISK_HAS_PREDICTION_AI,
    [9][17] = RISK_HAS_PREDICTION_AI,
    [9][18] = RISK_HAS_PREDICTION_AI,

    [12][14] = RISK_RANDOM_LEAD,
    [12][15] = RISK_RANDOM_LEAD,
    [13][14] = RISK_RANDOM_LEAD,
    [13][15] = RISK_RANDOM_LEAD,

    [12][17] = RISK_USES_POOLS,
    [12][18] = RISK_USES_POOLS,
    [13][17] = RISK_USES_POOLS,
    [13][18] = RISK_USES_POOLS,

    [4][21] = RISK_PLAYER_JUST_BERRIES,
    [4][22] = RISK_PLAYER_JUST_BERRIES,
    [5][21] = RISK_PLAYER_JUST_BERRIES,
    [5][22] = RISK_PLAYER_JUST_BERRIES,

    [4][24] = RISK_NO_PP_RESTORE,
    [4][25] = RISK_NO_PP_RESTORE,
    [5][24] = RISK_NO_PP_RESTORE,
    [5][25] = RISK_NO_PP_RESTORE,

    [8][21] = RISK_OPPONENT_HAS_ITEMS,
    [8][22] = RISK_OPPONENT_HAS_ITEMS,
    [9][21] = RISK_OPPONENT_HAS_ITEMS,
    [9][22] = RISK_OPPONENT_HAS_ITEMS,

    [8][24] = RISK_NO_ORDER_CHANGE,
    [8][25] = RISK_NO_ORDER_CHANGE,
    [9][24] = RISK_NO_ORDER_CHANGE,
    [9][25] = RISK_NO_ORDER_CHANGE,

    [12][21] = RISK_FLIP_TYPE_CHART,
    [12][22] = RISK_FLIP_TYPE_CHART,
    [13][21] = RISK_FLIP_TYPE_CHART,
    [13][22] = RISK_FLIP_TYPE_CHART,

    [12][24] = RISK_ATTACK_GETS_DROWSY,
    [12][25] = RISK_ATTACK_GETS_DROWSY,
    [13][24] = RISK_ATTACK_GETS_DROWSY,
    [13][25] = RISK_ATTACK_GETS_DROWSY,

    [15][21] = RISK_HAS_GEN_1_CRIT_CHANCE,
    [15][22] = RISK_HAS_GEN_1_CRIT_CHANCE,
    [16][21] = RISK_HAS_GEN_1_CRIT_CHANCE,
    [16][22] = RISK_HAS_GEN_1_CRIT_CHANCE,

    [15][24] = RISK_STATUS_GETS_PARA,
    [15][25] = RISK_STATUS_GETS_PARA,
    [16][24] = RISK_STATUS_GETS_PARA,
    [16][25] = RISK_STATUS_GETS_PARA,

    [22][28] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    [22][29] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    [23][28] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    [23][29] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,

    [22][31] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    [22][32] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    [23][31] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    [23][32] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,

    [26][28] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    [26][29] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    [27][28] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    [27][29] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,

    [26][31] = RISK_FOE_HAS_METRONOME,
    [26][32] = RISK_FOE_HAS_METRONOME,
    [27][31] = RISK_FOE_HAS_METRONOME,
    [27][32] = RISK_FOE_HAS_METRONOME,

    [30][28] = RISK_PLAYER_HAS_RECOIL,
    [30][29] = RISK_PLAYER_HAS_RECOIL,
    [31][28] = RISK_PLAYER_HAS_RECOIL,
    [31][29] = RISK_PLAYER_HAS_RECOIL,

    [30][31] = RISK_HAS_GUARANTEED_ACCURACY,
    [30][32] = RISK_HAS_GUARANTEED_ACCURACY,
    [31][31] = RISK_HAS_GUARANTEED_ACCURACY,
    [31][32] = RISK_HAS_GUARANTEED_ACCURACY,

    [22][35] = RISK_OPPONENT_MOVES_FIRST,
    [22][36] = RISK_OPPONENT_MOVES_FIRST,
    [23][35] = RISK_OPPONENT_MOVES_FIRST,
    [23][36] = RISK_OPPONENT_MOVES_FIRST,

    [22][38] = RISK_OPPONENT_ATTACKS_SWITCHES,
    [22][39] = RISK_OPPONENT_ATTACKS_SWITCHES,
    [23][38] = RISK_OPPONENT_ATTACKS_SWITCHES,
    [23][39] = RISK_OPPONENT_ATTACKS_SWITCHES,

    [26][35] = RISK_HAS_GUARANTEED_EFFECTS,
    [26][36] = RISK_HAS_GUARANTEED_EFFECTS,
    [27][35] = RISK_HAS_GUARANTEED_EFFECTS,
    [27][36] = RISK_HAS_GUARANTEED_EFFECTS,

    [26][38] = RISK_OPPONENT_ATTACKS_DISABLE,
    [26][39] = RISK_OPPONENT_ATTACKS_DISABLE,
    [27][38] = RISK_OPPONENT_ATTACKS_DISABLE,
    [27][39] = RISK_OPPONENT_ATTACKS_DISABLE,

    [30][35] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    [30][36] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    [31][35] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    [31][36] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,

    [30][38] = RISK_OPPONENT_ATTACKS_TORMENT,
    [30][39] = RISK_OPPONENT_ATTACKS_TORMENT,
    [31][38] = RISK_OPPONENT_ATTACKS_TORMENT,
    [31][39] = RISK_OPPONENT_ATTACKS_TORMENT,

    [26][17] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    [26][18] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    [27][17] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    [27][18] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,

    [23][13] = RISK_PLAYER_SPIKES_1,
    [23][14] = RISK_PLAYER_SPIKES_1,
    [24][13] = RISK_PLAYER_SPIKES_1,
    [24][14] = RISK_PLAYER_SPIKES_1,

    [23][16] = RISK_PLAYER_SPIKES_2,
    [23][17] = RISK_PLAYER_SPIKES_2,
    [24][16] = RISK_PLAYER_SPIKES_2,
    [24][17] = RISK_PLAYER_SPIKES_2,

    [23][19] = RISK_PLAYER_SPIKES_3,
    [23][20] = RISK_PLAYER_SPIKES_3,
    [24][19] = RISK_PLAYER_SPIKES_3,
    [24][20] = RISK_PLAYER_SPIKES_3,

    [29][16] = RISK_PLAYER_TOXIC_SPIKES_1,
    [29][17] = RISK_PLAYER_TOXIC_SPIKES_1,
    [30][16] = RISK_PLAYER_TOXIC_SPIKES_1,
    [30][17] = RISK_PLAYER_TOXIC_SPIKES_1,

    [29][19] = RISK_PLAYER_TOXIC_SPIKES_2,
    [29][20] = RISK_PLAYER_TOXIC_SPIKES_2,
    [30][19] = RISK_PLAYER_TOXIC_SPIKES_2,
    [30][20] = RISK_PLAYER_TOXIC_SPIKES_2,

    [23][22] = RISK_PLAYER_STEALTH_ROCK,
    [23][23] = RISK_PLAYER_STEALTH_ROCK,
    [24][22] = RISK_PLAYER_STEALTH_ROCK,
    [24][23] = RISK_PLAYER_STEALTH_ROCK,

    [26][22] = RISK_PLAYER_SHARP_STEEL,
    [26][23] = RISK_PLAYER_SHARP_STEEL,
    [27][22] = RISK_PLAYER_SHARP_STEEL,
    [27][23] = RISK_PLAYER_SHARP_STEEL,

    [29][22] = RISK_PLAYER_STICKY_WEB,
    [20][23] = RISK_PLAYER_STICKY_WEB,
    [30][22] = RISK_PLAYER_STICKY_WEB,
    [30][23] = RISK_PLAYER_STICKY_WEB,

    [10][33] = RISK_MINUS_1_MOVE,
    [10][34] = RISK_MINUS_1_MOVE,
    [11][33] = RISK_MINUS_1_MOVE,
    [11][34] = RISK_MINUS_1_MOVE,

    [5][29] = RISK_PLAYER_HAS_PARENTAL_BOND,
    [5][30] = RISK_PLAYER_HAS_PARENTAL_BOND,
    [6][29] = RISK_PLAYER_HAS_PARENTAL_BOND,
    [6][30] = RISK_PLAYER_HAS_PARENTAL_BOND,

    [8][29] = RISK_PLAYER_HAS_FILTER,
    [8][30] = RISK_PLAYER_HAS_FILTER,
    [9][29] = RISK_PLAYER_HAS_FILTER,
    [9][30] = RISK_PLAYER_HAS_FILTER,

    [12][29] = RISK_PLAYER_HAS_PERISH_BODY,
    [12][30] = RISK_PLAYER_HAS_PERISH_BODY,
    [13][29] = RISK_PLAYER_HAS_PERISH_BODY,
    [13][30] = RISK_PLAYER_HAS_PERISH_BODY,

    [15][29] = RISK_PLAYER_HAS_BEAST_BOOST,
    [15][30] = RISK_PLAYER_HAS_BEAST_BOOST,
    [16][29] = RISK_PLAYER_HAS_BEAST_BOOST,
    [16][30] = RISK_PLAYER_HAS_BEAST_BOOST,

    [5][36] = RISK_HAS_MOLD_BREAKER,
    [5][37] = RISK_HAS_MOLD_BREAKER,
    [6][36] = RISK_HAS_MOLD_BREAKER,
    [6][37] = RISK_HAS_MOLD_BREAKER,

    [8][36] = RISK_HAS_STURDY,
    [8][37] = RISK_HAS_STURDY,
    [9][36] = RISK_HAS_STURDY,
    [9][37] = RISK_HAS_STURDY,

    [12][36] = RISK_HAS_REGENERATOR,
    [12][37] = RISK_HAS_REGENERATOR,
    [13][36] = RISK_HAS_REGENERATOR,
    [13][37] = RISK_HAS_REGENERATOR,

    [16][36] = RISK_HAS_BATTLE_ARMOR,
    [16][37] = RISK_HAS_BATTLE_ARMOR,
    [17][36] = RISK_HAS_BATTLE_ARMOR,
    [17][37] = RISK_HAS_BATTLE_ARMOR,

    [6][39] = RISK_HAS_WONDER_GUARD,
    [6][40] = RISK_HAS_WONDER_GUARD,
    [7][39] = RISK_HAS_WONDER_GUARD,
    [7][40] = RISK_HAS_WONDER_GUARD,

    [10][39] = RISK_HAS_FILTER,
    [10][40] = RISK_HAS_FILTER,
    [11][39] = RISK_HAS_FILTER,
    [11][40] = RISK_HAS_FILTER,

    [14][39] = RISK_HAS_ADAPTABILITY,
    [14][40] = RISK_HAS_ADAPTABILITY,
    [15][39] = RISK_HAS_ADAPTABILITY,
    [15][40] = RISK_HAS_ADAPTABILITY,
};

static EWRAM_DATA struct RiskUiState *sRiskUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;

static const u32 sBackgroundTiles[] = INCGFX_U32("graphics/risk_ui/tiles.png", ".4bpp.smol");
static const u32 sBackgroundTilemap[] = INCBIN_U32("graphics/risk_ui/tiles.bin.smolTM");
static const u16 sBackgroundPalette[] = INCGFX_U16("graphics/risk_ui/tiles.png", ".gbapal");

static const u32 sSelectorGfx[] = INCGFX_U32("graphics/risk_ui/selector.png", ".4bpp");
static const u16 sSelectorPal[] = INCGFX_U16("graphics/risk_ui/selector.png", ".gbapal");

static const u32 sFrameGfx[] = INCGFX_U32("graphics/risk_ui/frame_new.png", ".4bpp.smol");
static const u32 sFrameTilemap[] = INCBIN_U32("graphics/risk_ui/frame_new.bin.smolTM");
static const u16 sFramePal[] = INCGFX_U16("graphics/risk_ui/frame_new.png", ".gbapal");

static const u32 sTotalIconGfx[] = INCGFX_U32("graphics/risk_ui/total_icon.png", ".4bpp");
static const u16 sTotalIconPal[] = INCGFX_U16("graphics/risk_ui/total_icon.png", ".gbapal");

static const struct BgTemplate sRiskUiBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .priority = 1,
        .screenSize = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 20,
        .priority = 2,
        .screenSize = 3,
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .priority = 0,
        .screenSize = 0,
    },
};

#define NAME_WIDTH 20
#define NAME_HEIGHT 2
#define DESCRIPTION_WIDTH 29
#define DESCRIPTION_HEIGHT 11
#define TOTAL_WIDTH 2
#define TOTAL_HEIGHT 2

#define NAME_SIZE NAME_WIDTH * NAME_HEIGHT
#define DESCRIPTION_SIZE DESCRIPTION_WIDTH * DESCRIPTION_HEIGHT
#define TOTAL_SIZE TOTAL_WIDTH * TOTAL_HEIGHT

#define NAME_BASEBLOCK 1
#define DESCRIPTION_BASEBLOCK NAME_BASEBLOCK + NAME_SIZE
#define TOTAL_BASEBLOCK DESCRIPTION_BASEBLOCK + DESCRIPTION_SIZE

static const struct WindowTemplate sRiskUiWindowTemplates[] =
{
    [WIN_RISK_NAME] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 18,
        .width = NAME_WIDTH,
        .height = NAME_HEIGHT,
        .paletteNum = 14,
        .baseBlock = NAME_BASEBLOCK,
    },
    [WIN_RISK_DESCRIPTION] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 20,
        .width = DESCRIPTION_WIDTH,
        .height = DESCRIPTION_HEIGHT,
        .paletteNum = 14,
        .baseBlock = DESCRIPTION_BASEBLOCK
    },
    [WIN_RISK_TOTAL] =
    {
        .bg = 0,
        .tilemapLeft = 26,
        .tilemapTop = 18,
        .width = TOTAL_WIDTH,
        .height = TOTAL_HEIGHT,
        .paletteNum = 14,
        .baseBlock = TOTAL_BASEBLOCK
    },
    DUMMY_WIN_TEMPLATE
};

enum FontColor
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_FADED,
    FONT_BLUE,
};

static const u8 sRiskUiWindowFontColors[][3] =
{
    [FONT_BLACK]  = {2, 5,  3},
    [FONT_WHITE]  = {5, 1,  6},
    [FONT_FADED]  = {2, 5,  6},
    [FONT_BLUE]   = {2, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
};

static void RiskUi_SetupCB(void);
static void RiskUi_ResetGpuRegsAndBgs(void);
static bool8 RiskUi_InitBgs(void);
static void RiskUi_FadeAndBail(void);
static void Task_RiskUiWaitFadeAndBail(u8 taskId);
static void RiskUi_VBlankCB(void);
static void RiskUi_FreeResources(void);
static void RiskUi_MainCB(void);
static bool8 RiskUi_LoadGraphics(void);
static void RiskUi_InitWindows(void);
static void Task_RiskUiWaitFadeIn(u8 taskId);
static void Task_RiskUiMainInput(u8 taskId);
static void LoadSelector(void);
static void LoadTotalIcon(void);
static void MoveSelectorX(s32 distance);
static void MoveSelectorY(s32 distance);
static void GetSelectedTiles(u16 *tiles);
static void TrySelectRiskUnderCursor(void);
static inline void SetRiskInactive(enum Risk risk);
static inline void SetRiskActive(enum Risk risk);
static void ChangeTilemapPalettesBeforeLoad(void);
static enum Risk GetRiskUnderCursor(void);
static void PrintRiskData(enum Risk risk);
static void ToggleLock(enum Risk risk);
static void PrintTotalOnIcon(void);

static void Task_RiskUiWaitFadeAndExitGracefully(u8 taskId);

static void SetTilePalette(u32 tile, u32 palette);

void RiskUi_Init(MainCallback callback)
{
    sRiskUiState = AllocZeroed(sizeof(struct RiskUiState));
    if (sRiskUiState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sRiskUiState->savedCallback = callback;
    sRiskUiState->loadState = 0;
    sRiskUiState->selectorId = SPRITE_NONE;

    SetMainCallback2(RiskUi_SetupCB);
}

void RiskUi_InitFromScript(struct ScriptContext *ctx)
{
    RiskUi_Init(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

static void RiskUi_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        RiskUi_ResetGpuRegsAndBgs();
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        if (RiskUi_InitBgs())
        {
            sRiskUiState->loadState = 0;
            gMain.state++;
        }
        else
        {
            RiskUi_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (RiskUi_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        RiskUi_InitWindows();
        gMain.state++;
        break;
    case 5:
        CreateTask(Task_RiskUiWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(RiskUi_VBlankCB);
        SetMainCallback2(RiskUi_MainCB);
        break;
    }
}

static void RiskUi_ResetGpuRegsAndBgs(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT, 0);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);

    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    ChangeBgX(0, 0, BG_COORD_SET);
    ChangeBgY(0, 0, BG_COORD_SET);
    ChangeBgX(1, 0, BG_COORD_SET);
    ChangeBgY(1, 0, BG_COORD_SET);
    ChangeBgX(2, 0, BG_COORD_SET);
    ChangeBgY(2, 0, BG_COORD_SET);
    ChangeBgX(3, 0, BG_COORD_SET);
    ChangeBgY(3, 0, BG_COORD_SET);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WIN1H, 0);
    SetGpuReg(REG_OFFSET_WIN1V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    CpuFill16(0, (void *)VRAM, VRAM_SIZE);
    CpuFill32(0, (void *)OAM, OAM_SIZE);
}

static bool8 RiskUi_InitBgs(void)
{
    const u32 TILEMAP_BUFFER_SIZE = (1024 * 8);

    ResetAllBgsCoordinates();

    u16 *tiles = AllocZeroed(TILEMAP_BUFFER_SIZE);
    sBg1TilemapBuffer = (u8 *)tiles;
    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    sBg2TilemapBuffer = AllocZeroed(1024 * 2);
    if (sBg2TilemapBuffer == NULL)
        return FALSE;

    ResetBgsAndClearDma3BusyFlags(0);

    InitBgsFromTemplates(0, sRiskUiBgTemplates, NELEMS(sRiskUiBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    SetBgTilemapBuffer(2, sBg2TilemapBuffer);

    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);

    ShowBg(0);
    ShowBg(1);
    ShowBg(2);

    return TRUE;
}

static void RiskUi_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_RiskUiWaitFadeAndBail, 0);

    SetVBlankCallback(RiskUi_VBlankCB);
    SetMainCallback2(RiskUi_MainCB);
}

static void Task_RiskUiWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sRiskUiState->savedCallback);
        RiskUi_FreeResources();
        DestroyTask(taskId);
    }
}

static void RiskUi_VBlankCB(void)
{

    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void RiskUi_FreeResources(void)
{
    if (sRiskUiState != NULL)
    {
        Free(sRiskUiState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    if (sBg2TilemapBuffer != NULL)
    {
        Free(sBg2TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}

static void RiskUi_MainCB(void)
{
    AnimateSprites();
    RunTasks();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static bool8 RiskUi_LoadGraphics(void)
{
    switch (sRiskUiState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sBackgroundTiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(2, sFrameGfx, 0, 0, 0);
        sRiskUiState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sBackgroundTilemap, sBg1TilemapBuffer);
            DecompressDataWithHeaderWram(sFrameTilemap, sBg2TilemapBuffer);
            //  Set tile palettes for active thing here
            ChangeTilemapPalettesBeforeLoad();
            sRiskUiState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sBackgroundPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP * 4);
        LoadPalette(sFramePal, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        LoadSelector();
        LoadTotalIcon();

        sRiskUiState->loadState++;
    default:
        sRiskUiState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void RiskUi_InitWindows(void)
{
    InitWindows(sRiskUiWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);

    for (u32 i = 0; i < WIN_COUNT; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(2));
        if (i == WIN_RISK_TOTAL) 
        {
            FillWindowPixelBuffer(i, PIXEL_FILL(5));
        }
        PutWindowTilemap(i);
        CopyWindowToVram(i, COPYWIN_FULL);
    }
}

static void Task_RiskUiWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {

        gTasks[taskId].func = Task_RiskUiMainInput;
    }
}

static void Task_HideDescription(u8 taskId)
{
    if (sRiskUiState->descriptionOffset > 8)
    {
        sRiskUiState->descriptionOffset -= 8;
        SetGpuReg(REG_OFFSET_BG0VOFS, sRiskUiState->descriptionOffset);
        SetGpuReg(REG_OFFSET_BG2VOFS, sRiskUiState->descriptionOffset);
    }
    else
    {
        sRiskUiState->descriptionOffset = 0;
        SetGpuReg(REG_OFFSET_BG0VOFS, sRiskUiState->descriptionOffset);
        SetGpuReg(REG_OFFSET_BG2VOFS, sRiskUiState->descriptionOffset);

        sRiskUiState->isShowingDescription = FALSE;
        gSprites[sRiskUiState->selectorId].invisible = FALSE;
        gTasks[taskId].func = Task_RiskUiMainInput;
    }
}

static void Task_DisplayDescription(u8 taskId)
{
    if (sRiskUiState->descriptionOffset < 88)
    {
        sRiskUiState->descriptionOffset += 8;
        SetGpuReg(REG_OFFSET_BG0VOFS, sRiskUiState->descriptionOffset);
        SetGpuReg(REG_OFFSET_BG2VOFS, sRiskUiState->descriptionOffset);
    }

    if (JOY_NEW(START_BUTTON))
    {
        gTasks[taskId].func = Task_HideDescription;
    }
}

static void Task_RiskUiMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_RiskUiWaitFadeAndExitGracefully;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        TrySelectRiskUnderCursor();
    }
    else if (JOY_NEW(START_BUTTON))
    {
        sRiskUiState->isShowingDescription = TRUE;
        gSprites[sRiskUiState->selectorId].invisible = TRUE;
        gTasks[taskId].func = Task_DisplayDescription;
    }
    else if (JOY_NEW(DPAD_ANY) || JOY_HELD(DPAD_ANY))
    {
        if (gSprites[sRiskUiState->selectorId].invisible)
            return;

        if (JOY_NEW(DPAD_UP) || JOY_HELD(DPAD_UP))
        {
            MoveSelectorY(-CURSOR_SPEED);
        }
        else if (JOY_NEW(DPAD_DOWN) || JOY_HELD(DPAD_DOWN))
        {
            MoveSelectorY(CURSOR_SPEED);
        }

        if (JOY_NEW(DPAD_LEFT) || JOY_HELD(DPAD_LEFT))
        {
            MoveSelectorX(-CURSOR_SPEED);
        }
        else if (JOY_NEW(DPAD_RIGHT) || JOY_HELD(DPAD_RIGHT))
        {
            MoveSelectorX(CURSOR_SPEED);
        }

        //  Detect if new position is under a new risk
        enum Risk risk = GetRiskUnderCursor();
        if (risk != sRiskUiState->prevRisk)
        {
            sRiskUiState->prevRisk = risk;
            PrintRiskData(risk);
        }
    }
}

static void Task_RiskUiWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sRiskUiState->savedCallback);
        RiskUi_FreeResources();
        DestroyTask(taskId);
    }
}

static void LoadSelector(void)
{
    struct Even_CreateSpriteStruct cs = {0};
    cs.sprite = sSelectorGfx;
    cs.tileTag = 1;
    cs.palette = sSelectorPal;
    cs.palTag = 1;
    cs.spriteSize = SPRITE_SIZE(32x32);
    cs.spriteShape = SPRITE_SHAPE(32x32);
    cs.posX = 120;
    cs.posY = 80;
    sRiskUiState->selectorId = Even_CreateSprite(&cs);
    sRiskUiState->xSelector = 120;
    sRiskUiState->ySelector = 80;
}

static void LoadTotalIcon(void)
{
    struct Even_CreateSpriteStruct cs = {0};
    cs.sprite = sTotalIconGfx;
    cs.tileTag = 2;
    cs.palette = sTotalIconPal;
    cs.palTag = 2;
    cs.spriteSize = SPRITE_SIZE(32x32);
    cs.spriteShape = SPRITE_SHAPE(32x32);
    cs.posX = 224;
    cs.posY = 16;
    sRiskUiState->totalIconId = Even_CreateSprite(&cs);

    const u32 *srcs[1] = { sTotalIconGfx };
    SetupSpritesForTextPrinting(&sRiskUiState->totalIconId, srcs, 1, 1);

    PrintTotalOnIcon();
}

static void MoveSelectorX(s32 distance)
{
    if (distance > 0)
    {
        if (sRiskUiState->xSelector == 240 - 16)
        {
            if (sRiskUiState->xOffset != 272)
            {
                sRiskUiState->xOffset += distance;
                SetGpuReg(REG_OFFSET_BG1HOFS, sRiskUiState->xOffset);
            }
        }
        else
        {
            sRiskUiState->xSelector += distance;
            gSprites[sRiskUiState->selectorId].x = sRiskUiState->xSelector;
        }
    }
    else
    {
        if (sRiskUiState->xSelector == 16)
        {
            if (sRiskUiState->xOffset != 0)
            {
                sRiskUiState->xOffset += distance;
                SetGpuReg(REG_OFFSET_BG1HOFS, sRiskUiState->xOffset);
            }
        }
        else
        {
            sRiskUiState->xSelector += distance;
            gSprites[sRiskUiState->selectorId].x = sRiskUiState->xSelector;
        }
    }
}

static void MoveSelectorY(s32 distance)
{
    if (distance > 0)
    {
        if (sRiskUiState->ySelector == 160 - 40)
        {
            if (sRiskUiState->yOffset != 352)
            {
                sRiskUiState->yOffset += distance;
                SetGpuReg(REG_OFFSET_BG1VOFS, sRiskUiState->yOffset);
            }
        }
        else
        {
            sRiskUiState->ySelector += distance;
            gSprites[sRiskUiState->selectorId].y = sRiskUiState->ySelector;
        }
    }
    else
    {
        if (sRiskUiState->ySelector == 16)
        {
            if (sRiskUiState->yOffset != 0)
            {
                sRiskUiState->yOffset += distance;
                SetGpuReg(REG_OFFSET_BG1VOFS, sRiskUiState->yOffset);
            }
        }
        else
        {
            sRiskUiState->ySelector += distance;
            gSprites[sRiskUiState->selectorId].y = sRiskUiState->ySelector;
        }
    }
}

static void SetTilePalette(u32 tile, u32 palette)
{
    u16 *tilemapPtr = (u16 *)(BG_VRAM + sRiskUiBgTemplates[1].mapBaseIndex * BG_SCREEN_SIZE);
    u16 palMask = palette << 12;
    u16 currVal = tilemapPtr[tile] & 0xFFF;
    tilemapPtr[tile] = palMask | currVal;
}

static void GetSelectedTiles(u16 *tiles)
{
    u32 xSel = (sRiskUiState->xSelector + sRiskUiState->xOffset) / 8 - 1;
    u32 ySel = (sRiskUiState->ySelector + sRiskUiState->yOffset) / 8 - 1;


    for (u32 x = 0; x < 2; x++)
    {
        for (u32 y = 0; y < 2; y++)
        {
            u32 currX = xSel + x;
            u32 currY = ySel + y;
            tiles[y * 2 + x] = COORD_TO_TILE(currX, currY);
        }
    }
}

static enum Risk GetRiskUnderCursor(void)
{
    u32 xSel = (sRiskUiState->xSelector + sRiskUiState->xOffset) / 8 - 1;
    u32 ySel = (sRiskUiState->ySelector + sRiskUiState->yOffset) / 8 - 1;

    enum Risk risk = RISK_NONE;

    if (sRiskMap[xSel][ySel] != RISK_NONE)
    {
        risk = sRiskMap[xSel][ySel];
    }
    else if (sRiskMap[xSel + 1][ySel] != RISK_NONE)
    {
        risk = sRiskMap[xSel + 1][ySel];
    }
    else if (sRiskMap[xSel][ySel + 1] != RISK_NONE)
    {
        risk = sRiskMap[xSel][ySel + 1];
    }
    else if (sRiskMap[xSel + 1][ySel + 1] != RISK_NONE)
    {
        risk = sRiskMap[xSel + 1][ySel + 1];
    }

    return risk;
}

static void TrySelectRiskUnderCursor(void)
{
    enum Risk risk = GetRiskUnderCursor();

    if (risk == RISK_RESET)
    {
        for (enum Risk risk = RISK_RESET + 1; risk < RISK_COUNT; risk++)
        {
            if (IsRiskActive(risk))
            {
                SetRiskInactive(risk);
            }
        }
        PrintTotalOnIcon();
    }
    else if (risk != RISK_NONE)
    {
        if (sRiskData[risk].lockRisk != RISK_NONE && !IsRiskActive(sRiskData[risk].lockRisk))
        {
            PlaySE(SE_WALL_HIT);
            return;
        }
        if (IsRiskActive(risk))
        {
            SetRiskInactive(risk);
        }
        else
        {
            for (u32 i = 0; i < sRiskData[risk].linkedCount; i++)
            {
                SetRiskInactive(sRiskData[risk].linkedRisks[i]);
            }
            SetRiskActive(risk);
        }
        PrintTotalOnIcon();
    }
}

static inline void SetRiskInactive(enum Risk risk)
{
    SetTilePalette(sRiskData[risk].tiles[0], PAL_INDEX_INACTIVE);
    SetTilePalette(sRiskData[risk].tiles[1], PAL_INDEX_INACTIVE);
    SetTilePalette(sRiskData[risk].tiles[2], PAL_INDEX_INACTIVE);
    SetTilePalette(sRiskData[risk].tiles[3], PAL_INDEX_INACTIVE);
    ClearRisk(risk);
    if (sRiskData[risk].unlockCount > 0)
        ToggleLock(risk);
}

static inline void SetRiskActive(enum Risk risk)
{
    SetTilePalette(sRiskData[risk].tiles[0], PAL_INDEX_ACTIVE);
    SetTilePalette(sRiskData[risk].tiles[1], PAL_INDEX_ACTIVE);
    SetTilePalette(sRiskData[risk].tiles[2], PAL_INDEX_ACTIVE);
    SetTilePalette(sRiskData[risk].tiles[3], PAL_INDEX_ACTIVE);
    SetRisk(risk);
    if (sRiskData[risk].unlockCount > 0)
        ToggleLock(risk);
}

static void ChangeTilemapPalettesBeforeLoad(void)
{
    for (enum Risk risk = RISK_RESET + 1; risk < RISK_COUNT; risk++)
    {
        if (IsRiskActive(risk))
        {
            for (u32 i = 0; i < 4; i++)
            {
                u32 tileNum = sRiskData[risk].tiles[i];
                u16 *tilemapPtr = (u16 *)sBg1TilemapBuffer;
                u16 palMask = PAL_INDEX_ACTIVE << 12;
                u16 currVal = tilemapPtr[tileNum] & 0xFFF;
                tilemapPtr[tileNum] = palMask | currVal;
            }
        }
        else if (sRiskData[risk].lockRisk != RISK_NONE)
        {
            if (!IsRiskActive(sRiskData[risk].lockRisk))
            {
                for (u32 i = 0; i < 4; i++)
                {
                    u32 tileNum = sRiskData[risk].tiles[i];
                    u16 *tilemapPtr = (u16 *)sBg1TilemapBuffer;
                    u16 palMask = PAL_INDEX_LOCKED << 12;
                    u16 currVal = tilemapPtr[tileNum] & 0xFFF;
                    tilemapPtr[tileNum] = palMask | currVal;
                }
            }
        }
    }
}

static void PrintRiskData(enum Risk risk)
{
    //  First clear out windows
    FillWindowPixelBuffer(WIN_RISK_NAME, PIXEL_FILL(2));
    FillWindowPixelBuffer(WIN_RISK_DESCRIPTION, PIXEL_FILL(2));

    //  Then if risk is not RISK_NONE
    //  print new risk text
    if (risk != RISK_NONE)
    {
        AddTextPrinterParameterized4(WIN_RISK_NAME,
                                     FONT_NORMAL,
                                     0, 0, 0, 0,
                                     sRiskUiWindowFontColors[FONT_BLACK],
                                     TEXT_SKIP_DRAW,
                                     sRiskData[risk].name);
        AddTextPrinterParameterized4(WIN_RISK_DESCRIPTION,
                                     FONT_NORMAL,
                                     0, 0, 0, 0,
                                     sRiskUiWindowFontColors[FONT_BLACK],
                                     TEXT_SKIP_DRAW,
                                     sRiskData[risk].description);
        u8 str[2];
        ConvertIntToDecimalStringN(str, GetRiskValue(risk), STR_CONV_MODE_LEFT_ALIGN, 1);
        AddTextPrinterParameterized4(WIN_RISK_TOTAL,
                                     FONT_NORMAL,
                                     4, 0, 0, 0,
                                     sRiskUiWindowFontColors[FONT_WHITE],
                                     TEXT_SKIP_DRAW,
                                     str);
    }
    CopyWindowToVram(WIN_RISK_NAME, COPYWIN_GFX);
    CopyWindowToVram(WIN_RISK_DESCRIPTION, COPYWIN_GFX);
    CopyWindowToVram(WIN_RISK_TOTAL, COPYWIN_GFX);
}

static void ToggleLock(enum Risk risk)
{
    assertf(sRiskData[risk].unlockCount > 0, "Attempting to unlock non-lock risk")
    {
        return;
    }
    u32 palette;
    if (IsRiskActive(risk))
    {
        palette = PAL_INDEX_INACTIVE;
    }
    else
    {
        palette = PAL_INDEX_LOCKED;
    }

    for (u32 i = 0; i < sRiskData[risk].unlockCount; i++)
    {
        enum Risk lockRisk = sRiskData[risk].unlockedRisks[i];
        if (IsRiskActive(lockRisk))
            SetRiskInactive(lockRisk);

        SetTilePalette(sRiskData[lockRisk].tiles[0], palette);
        SetTilePalette(sRiskData[lockRisk].tiles[1], palette);
        SetTilePalette(sRiskData[lockRisk].tiles[2], palette);
        SetTilePalette(sRiskData[lockRisk].tiles[3], palette);
    }
}

static const union TextColor sTotalIconTextColor =
{
    .background = 0,
    .foreground = 6,
    .shadow = 3,
    .accent = 0
};

static void PrintTotalOnIcon(void)
{
    u32 total = GetTotalTiskValue();
    FillSpriteRectSprite(sRiskUiState->totalIconId, 8, 8, 16, 16);
    u8 str[3];
    ConvertIntToDecimalStringN(str, total, STR_CONV_MODE_LEFT_ALIGN, 2);
    u32 width = GetStringWidth(FONT_NORMAL, str, 0);
    AddSpriteTextPrinterParameterized6(sRiskUiState->totalIconId, FONT_NORMAL, 16 - width / 2, 8, 0, 0, sTotalIconTextColor, 0, str);
}
