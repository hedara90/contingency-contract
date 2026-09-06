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

#define FLIP_H (1 << 10)
#define FLIP_V (1 << 11)

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
    u16 lockTiles[6];
    u16 lockTilemap[6];
    u16 unlockTilemap[6];
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
        .tiles = { COORD_TO_TILE(6, 2), COORD_TO_TILE(6, 3), COORD_TO_TILE(7, 2), COORD_TO_TILE(7, 3) },
        .linkedCount = 0,
        .unlockCount = 0,
        .name = COMPOUND_STRING("Reset Risks"),
        .description = COMPOUND_STRING("Reset all selected risks"),
    },
    [RISK_OPPONENT_HP_1] =
    {
        .linkedRisks = sLinkedHpRisks,
        .tiles = { COORD_TO_TILE(12, 4), COORD_TO_TILE(12, 5), COORD_TO_TILE(13, 4), COORD_TO_TILE(13, 5) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Fortification I"),
        .description = COMPOUND_STRING("Foes have 10% more HP"),
    },
    [RISK_OPPONENT_HP_2] =
    {
        .linkedRisks = sLinkedHpRisks,
        .tiles = { COORD_TO_TILE(12, 6), COORD_TO_TILE(12, 7), COORD_TO_TILE(13, 6), COORD_TO_TILE(13, 7) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Fortification II"),
        .description = COMPOUND_STRING("Foes have 25% more HP"),
    },
    [RISK_OPPONENT_HP_3] =
    {
        .linkedRisks = sLinkedHpRisks,
        .tiles = { COORD_TO_TILE(12, 8), COORD_TO_TILE(12, 9), COORD_TO_TILE(13, 8), COORD_TO_TILE(13, 9) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Fortification III"),
        .description = COMPOUND_STRING("Foes have 50% more HP"),
    },
    [RISK_TURN_LIMIT_1] =
    {
        .linkedRisks = sLinkedTurnRisks,
        .tiles = { COORD_TO_TILE(14, 4), COORD_TO_TILE(14, 5), COORD_TO_TILE(15, 4), COORD_TO_TILE(15, 5) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("High Pressure I"),
        .description = COMPOUND_STRING("Must win within 20 turns."),
    },
    [RISK_TURN_LIMIT_2] =
    {
        .linkedRisks = sLinkedTurnRisks,
        .tiles = { COORD_TO_TILE(14, 6), COORD_TO_TILE(14, 7), COORD_TO_TILE(15, 6), COORD_TO_TILE(15, 7) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("High Pressure II"),
        .description = COMPOUND_STRING("Must win within 15 turns."),
    },
    [RISK_TURN_LIMIT_3] =
    {
        .linkedRisks = sLinkedTurnRisks,
        .tiles = { COORD_TO_TILE(14, 8), COORD_TO_TILE(14, 9), COORD_TO_TILE(15, 8), COORD_TO_TILE(15, 9) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("High Pressure III"),
        .description = COMPOUND_STRING("Must win within 10 turns."),
    },
    [RISK_PARTY_MINUS_1] =
    {
        .linkedRisks = sLinkedTeamRisks,
        .tiles = { COORD_TO_TILE(33, 25), COORD_TO_TILE(34, 25), COORD_TO_TILE(33, 26), COORD_TO_TILE(34, 26) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Covert Operation I"),
        .description = COMPOUND_STRING("Player can only have 5 mons."),
    },
    [RISK_PARTY_MINUS_2] =
    {
        .linkedRisks = sLinkedTeamRisks,
        .tiles = { COORD_TO_TILE(33, 27), COORD_TO_TILE(34, 27), COORD_TO_TILE(33, 28), COORD_TO_TILE(34, 28) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Covert Operation II"),
        .description = COMPOUND_STRING("Player can only have 4 mons."),
    },
    [RISK_PARTY_MINUS_3] =
    {
        .linkedRisks = sLinkedTeamRisks,
        .tiles = { COORD_TO_TILE(33, 29), COORD_TO_TILE(34, 29), COORD_TO_TILE(33, 30), COORD_TO_TILE(34, 30) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Covert Operation III"),
        .description = COMPOUND_STRING("Player can only have 3 mons."),
    },
    [RISK_MUST_SWITCH_1] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(18, 4), COORD_TO_TILE(18, 5), COORD_TO_TILE(19, 4), COORD_TO_TILE(19, 5) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Fatigue I"),
        .description = COMPOUND_STRING("Player must switch a mon every 4 turns."),
    },
    [RISK_MUST_SWITCH_2] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(18, 6), COORD_TO_TILE(18, 7), COORD_TO_TILE(19, 6), COORD_TO_TILE(19, 7) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Fatigue II"),
        .description = COMPOUND_STRING("Player must switch a mon every 3 turns."),
    },
    [RISK_MUST_SWITCH_3] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(18, 8), COORD_TO_TILE(18, 9), COORD_TO_TILE(19, 8), COORD_TO_TILE(19, 9) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Fatigue III"),
        .description = COMPOUND_STRING("Player must switch a mon every 2 turns."),
    },
    [RISK_CANT_SWITCH] =
    {
        .linkedRisks = sLinkedSwitchRisks,
        .tiles = { COORD_TO_TILE(20, 6), COORD_TO_TILE(20, 7), COORD_TO_TILE(21, 6), COORD_TO_TILE(21, 7) },
        .linkedCount = 4,
        .name = COMPOUND_STRING("Lockdown"),
        .description = COMPOUND_STRING("Player can't switch."),
    },
    [RISK_PLAYER_STARTS_WITH_BURN] =
    {
        .linkedRisks = sLinkedStartStatusRisks,
        .tiles = { COORD_TO_TILE(26, 8), COORD_TO_TILE(26, 9), COORD_TO_TILE(27, 8), COORD_TO_TILE(27, 9) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Debilitation: BRN"),
        .description = COMPOUND_STRING("Player mons start battles burned."),
    },
    [RISK_PLAYER_STARTS_WITH_FROSTBITE] =
    {
        .linkedRisks = sLinkedStartStatusRisks,
        .tiles = { COORD_TO_TILE(30, 8), COORD_TO_TILE(30, 9), COORD_TO_TILE(31, 8), COORD_TO_TILE(31, 9) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Debilitation: FRB"),
        .description = COMPOUND_STRING("Player mons start battles frostbitten."),
    },
    [RISK_PLAYER_STARTS_WITH_PARALYSIS] =
    {
        .linkedRisks = sLinkedStartStatusRisks,
        .tiles = { COORD_TO_TILE(28, 8), COORD_TO_TILE(28, 9), COORD_TO_TILE(29, 8), COORD_TO_TILE(29, 9) },
        .linkedCount = 3,
        .name = COMPOUND_STRING("Debilitation: PRZ"),
        .description = COMPOUND_STRING("Player mons start battles paralyzed."),
    },
    [RISK_OPPONENT_MORE_MONS_1] =
    {
        .linkedRisks = sLinkedOppMoreMons,
        .tiles = { COORD_TO_TILE(31, 25), COORD_TO_TILE(31, 26), COORD_TO_TILE(32, 25), COORD_TO_TILE(32, 26) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Target: Battleplan I"),
        .description = COMPOUND_STRING("Opponent has 1 more mon in their party."),
    },
    [RISK_OPPONENT_MORE_MONS_2] =
    {
        .linkedRisks = sLinkedOppMoreMons,
        .tiles = { COORD_TO_TILE(31, 27), COORD_TO_TILE(31, 28), COORD_TO_TILE(32, 27), COORD_TO_TILE(32, 28) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Target: Battleplan II"),
        .description = COMPOUND_STRING("Opponent has 2 more mon in their party."),
    },
    [RISK_HAS_OMNISCIENT_AI] =
    {
        .linkedRisks = sLinkedAi,
        .tiles = { COORD_TO_TILE(27, 23), COORD_TO_TILE(27, 24), COORD_TO_TILE(28, 23), COORD_TO_TILE(28, 24) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Scout Deployment"),
        .description = COMPOUND_STRING("AI knows the players party, moves and\nabilities."),
    },
    [RISK_HAS_PREDICTION_AI] =
    {
        .linkedRisks = sLinkedAi,
        .tiles = { COORD_TO_TILE(27, 25), COORD_TO_TILE(27, 26), COORD_TO_TILE(28, 25), COORD_TO_TILE(28, 26) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Strategize"),
        .description = COMPOUND_STRING("AI knows the players party, moves and\nabilities.\nAI predicts the player's action."),
    },
    [RISK_RANDOM_LEAD] =
    {
        .linkedRisks = sLinkedPool,
        .tiles = { COORD_TO_TILE(29, 21), COORD_TO_TILE(29, 22), COORD_TO_TILE(30, 21), COORD_TO_TILE(30, 22) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Target: Uncertain"),
        .description = COMPOUND_STRING("Opponent has random lead."),
    },
    [RISK_USES_POOLS] =
    {
        .linkedRisks = sLinkedPool,
        .tiles = { COORD_TO_TILE(29, 27), COORD_TO_TILE(29, 28), COORD_TO_TILE(30, 27), COORD_TO_TILE(30, 28) },
        .linkedCount = 2,
        .name = COMPOUND_STRING("Target: Unknown"),
        .description = COMPOUND_STRING("Opponent's party is picked from a pool of\nmons, creating a semi-random team."),
    },
    [RISK_PLAYER_JUST_BERRIES] =
    {
        .tiles = { COORD_TO_TILE(25, 6), COORD_TO_TILE(25, 7), COORD_TO_TILE(26, 6), COORD_TO_TILE(26, 7) },
        .name = COMPOUND_STRING("Natural"),
        .description = COMPOUND_STRING("Player can only use berries as held items."),
    },
    [RISK_NO_PP_RESTORE] =
    {
        .tiles = { COORD_TO_TILE(27, 6), COORD_TO_TILE(27, 7), COORD_TO_TILE(28, 6), COORD_TO_TILE(28, 7) },
        .name = COMPOUND_STRING("Resource Depletion"),
        .description = COMPOUND_STRING("Move PP doesn't restore between battles."),
    },
    [RISK_OPPONENT_HAS_ITEMS] =
    {
        .tiles = { COORD_TO_TILE(29, 29), COORD_TO_TILE(29, 30), COORD_TO_TILE(30, 29), COORD_TO_TILE(30, 30) },
        .name = COMPOUND_STRING("Target: Well Equipped"),
        .description = COMPOUND_STRING("Opponent mons have items."),
    },
    [RISK_NO_ORDER_CHANGE] =
    {
        .tiles = { COORD_TO_TILE(28, 4), COORD_TO_TILE(28, 5), COORD_TO_TILE(29, 4), COORD_TO_TILE(29, 5) },
        .name = COMPOUND_STRING("Locked in"),
        .description = COMPOUND_STRING("Player can't change party or move order\nbetween battles."),
    },
    [RISK_FLIP_TYPE_CHART] =
    {
        .tiles = { COORD_TO_TILE(12, 12), COORD_TO_TILE(12, 13), COORD_TO_TILE(13, 12), COORD_TO_TILE(13, 13) },
        .name = COMPOUND_STRING("Ambient: Inversion"),
        .description = COMPOUND_STRING("The type chart is flipped."),
    },
    [RISK_ATTACK_GETS_DROWSY] =
    {
        .tiles = { COORD_TO_TILE(29, 6), COORD_TO_TILE(29, 7), COORD_TO_TILE(30, 6), COORD_TO_TILE(30, 7) },
        .name = COMPOUND_STRING("Ambient Exhaustion"),
        .description = COMPOUND_STRING("Using and attack makes player mons drowsy."),
    },
    [RISK_HAS_GEN_1_CRIT_CHANCE] =
    {
        .tiles = { COORD_TO_TILE(12, 14), COORD_TO_TILE(12, 15), COORD_TO_TILE(13, 14), COORD_TO_TILE(13, 15) },
        .name = COMPOUND_STRING("Ambient: Ancient Crits"),
        .description = COMPOUND_STRING("Crit rate is calculated using Gen 1 formulas.\nFaster mons have a higher chance of\ncritting."),
    },
    [RISK_STATUS_GETS_PARA] =
    {
        .tiles = { COORD_TO_TILE(31, 6), COORD_TO_TILE(31, 7), COORD_TO_TILE(32, 6), COORD_TO_TILE(32, 7) },
        .name = COMPOUND_STRING("Ambient Inhibition"),
        .description = COMPOUND_STRING("Using a status move paralyzes player mons."),
    },
    [RISK_PLAYER_LOWER_DAMAGE_ROLLS] =
    {
        .tiles = { COORD_TO_TILE(16, 20), COORD_TO_TILE(16, 21), COORD_TO_TILE(17, 20), COORD_TO_TILE(17, 21) },
        .name = COMPOUND_STRING("Below Average"),
        .description = COMPOUND_STRING("Player attack rolls use only the lower\nhalf of results."),
    },
    [RISK_OPPONENT_HIGHER_DAMAGE_ROLLS] =
    {
        .tiles = { COORD_TO_TILE(18, 20), COORD_TO_TILE(18, 21), COORD_TO_TILE(19, 20), COORD_TO_TILE(19, 21) },
        .name = COMPOUND_STRING("Above Average"),
        .description = COMPOUND_STRING("Opponent attack rolls use only the upper\nhalf of the results."),
    },
    [RISK_PLAYER_HAS_NEGATIVE_METRONOME] =
    {
        .tiles = { COORD_TO_TILE(20, 20), COORD_TO_TILE(20, 21), COORD_TO_TILE(21, 20), COORD_TO_TILE(21, 21) },
        .name = COMPOUND_STRING("Damping"),
        .description = COMPOUND_STRING("Player has a negative Metronome item\neffect of them.\nConsecutive attacking moves does less\ndamage."),
    },
    [RISK_FOE_HAS_METRONOME] =
    {
        .tiles = { COORD_TO_TILE(22, 20), COORD_TO_TILE(22, 21), COORD_TO_TILE(23, 20), COORD_TO_TILE(23, 21) },
        .name = COMPOUND_STRING("Ramping"),
        .description = COMPOUND_STRING("Opponent has a positive Metronome item\neffect on them.\nConsecutive attacking moves does more\ndamage."),
    },
    [RISK_PLAYER_HAS_RECOIL] =
    {
        .tiles = { COORD_TO_TILE(19, 22), COORD_TO_TILE(19, 23), COORD_TO_TILE(20, 22), COORD_TO_TILE(20, 23) },
        .name = COMPOUND_STRING("Shattering Blade"),
        .description = COMPOUND_STRING("All player moves have recoil equal to 25%\nof damage dealt."),
    },
    [RISK_HAS_GUARANTEED_ACCURACY] =
    {
        .tiles = { COORD_TO_TILE(20, 14), COORD_TO_TILE(20, 15), COORD_TO_TILE(21, 14), COORD_TO_TILE(21, 15) },
        .name = COMPOUND_STRING("Deadeye Bolts"),
        .description = COMPOUND_STRING("Opponent can't miss moves."),
    },
    [RISK_OPPONENT_MOVES_FIRST] =
    {
        .tiles = { COORD_TO_TILE(18, 16), COORD_TO_TILE(18, 17), COORD_TO_TILE(19, 16), COORD_TO_TILE(19, 17) },
        .name = COMPOUND_STRING("Initiative Priority"),
        .description = COMPOUND_STRING("Opponents moves first in their priority\nbracket."),
    },
    [RISK_OPPONENT_ATTACKS_SWITCHES] =
    {
        .tiles = { COORD_TO_TILE(18, 14), COORD_TO_TILE(18, 15), COORD_TO_TILE(19, 14), COORD_TO_TILE(19, 15) },
        .name = COMPOUND_STRING("Formation Breaker"),
        .description = COMPOUND_STRING("Opponent attacks foribly switches the\nplayer at end of turn."),
    },
    [RISK_HAS_GUARANTEED_EFFECTS] =
    {
        .tiles = { COORD_TO_TILE(16, 14), COORD_TO_TILE(16, 15), COORD_TO_TILE(17, 14), COORD_TO_TILE(17, 15) },
        .name = COMPOUND_STRING("Special Execution"),
        .description = COMPOUND_STRING("Opponent's moves with secondary effects\nare guaranteed to proc those effects."),
    },
    [RISK_OPPONENT_ATTACKS_DISABLE] =
    {
        .tiles = { COORD_TO_TILE(16, 12), COORD_TO_TILE(16, 13), COORD_TO_TILE(17, 12), COORD_TO_TILE(17, 13) },
        .name = COMPOUND_STRING("Disabling"),
        .description = COMPOUND_STRING("Opponents attacks apply the Disable\neffect."),
    },
    [RISK_OPPONENT_INFLICTS_GASTRO_ACID] =
    {
        .tiles = { COORD_TO_TILE(20, 12), COORD_TO_TILE(20, 13), COORD_TO_TILE(21, 12), COORD_TO_TILE(21, 13) },
        .name = COMPOUND_STRING("Suppressing"),
        .description = COMPOUND_STRING("Opponents attacks apply Gastro Acid\nwhich suppresses abilities."),
    },
    [RISK_OPPONENT_ATTACKS_TORMENT] =
    {
        .tiles = { COORD_TO_TILE(18, 12), COORD_TO_TILE(18, 13), COORD_TO_TILE(19, 12), COORD_TO_TILE(19, 13) },
        .name = COMPOUND_STRING("Tormenting"),
        .description = COMPOUND_STRING("Opponents attacks apply the Torment\neffect, preventing mons from repeating\nattacks."),
    },
    [RISK_PLAYER_HAZARDS_NOT_REMOVABLE] =
    {
        .unlockedRisks = sLockedHazardRisks,
        .tiles = { COORD_TO_TILE(24, 12), COORD_TO_TILE(24, 13), COORD_TO_TILE(25, 12), COORD_TO_TILE(25, 13) },
        .unlockCount = 8,
        .name = COMPOUND_STRING("Environment: Area Lockdown"),
        .description = COMPOUND_STRING("Hazards can't be removed from the player's\nside of the field."),
        .lockTiles = {
            COORD_TO_TILE(23, 10), COORD_TO_TILE(24, 10), COORD_TO_TILE(25, 10),
            COORD_TO_TILE(23, 11), COORD_TO_TILE(24, 11), COORD_TO_TILE(25, 11)
        },
        .lockTilemap =
        {
            0xD4, 0xD5, 0xD5 | FLIP_H,
            0x0D, 0xE2, 0xE3,
        },
        .unlockTilemap =
        {
            771, 770, 0x0D,
            775, 773, 776,
        },
    },
    [RISK_PLAYER_SPIKES_1] =
    {
        .linkedRisks = sLinkedSpikes,
        .linkedCount = 3,
        .tiles = { COORD_TO_TILE(34, 12), COORD_TO_TILE(34, 13), COORD_TO_TILE(35, 12), COORD_TO_TILE(35, 13) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Debris I"),
        .description = COMPOUND_STRING("Player starts with 1 layer of spikes\non their side of the field."),
    },
    [RISK_PLAYER_SPIKES_2] =
    {
        .linkedRisks = sLinkedSpikes,
        .linkedCount = 3,
        .tiles = { COORD_TO_TILE(34, 14), COORD_TO_TILE(34, 15), COORD_TO_TILE(35, 14), COORD_TO_TILE(35, 15) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Debris II"),
        .description = COMPOUND_STRING("Player starts with 2 layer of spikes\non their side of the field."),
    },
    [RISK_PLAYER_SPIKES_3] =
    {
        .linkedRisks = sLinkedSpikes,
        .linkedCount = 3,
        .tiles = { COORD_TO_TILE(34, 16), COORD_TO_TILE(34, 17), COORD_TO_TILE(35, 16), COORD_TO_TILE(35, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Debris III"),
        .description = COMPOUND_STRING("Player starts with 3 layer of spikes\non their side of the field."),
    },
    [RISK_PLAYER_TOXIC_SPIKES_1] =
    {
        .linkedRisks = sLinkedTSpikes,
        .linkedCount = 2,
        .tiles = { COORD_TO_TILE(32, 14), COORD_TO_TILE(32, 15), COORD_TO_TILE(33, 14), COORD_TO_TILE(33, 15) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Noxious I"),
        .description = COMPOUND_STRING("Player starts with 1 layer of toxic spikes\non their side of the field."),
    },
    [RISK_PLAYER_TOXIC_SPIKES_2] =
    {
        .linkedRisks = sLinkedTSpikes,
        .linkedCount = 2,
        .tiles = { COORD_TO_TILE(32, 16), COORD_TO_TILE(32, 17), COORD_TO_TILE(33, 16), COORD_TO_TILE(33, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Noxious II"),
        .description = COMPOUND_STRING("Player starts with 2 layer of toxic spikes\non their side of the field."),
    },
    [RISK_PLAYER_STEALTH_ROCK] =
    {
        .tiles = { COORD_TO_TILE(26, 16), COORD_TO_TILE(26, 17), COORD_TO_TILE(27, 16), COORD_TO_TILE(27, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Shattered Earth"),
        .description = COMPOUND_STRING("Player starts with Stealth Rock\non their side of the field."),
    },
    [RISK_PLAYER_SHARP_STEEL] =
    {
        .tiles = { COORD_TO_TILE(28, 16), COORD_TO_TILE(28, 17), COORD_TO_TILE(29, 16), COORD_TO_TILE(29, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Shrapnell"),
        .description = COMPOUND_STRING("Player starts with Sharp Steel\non their side of the field."),
    },
    [RISK_PLAYER_STICKY_WEB] =
    {
        .tiles = { COORD_TO_TILE(30, 16), COORD_TO_TILE(30, 17), COORD_TO_TILE(31, 16), COORD_TO_TILE(31, 17) },
        .lockRisk = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
        .name = COMPOUND_STRING("Environment: Mire"),
        .description = COMPOUND_STRING("Player starts with Sticky Web\non their side of the field."),
    },
    [RISK_MINUS_1_MOVE] =
    {
        .unlockedRisks = sLockedAbilitiyRisks,
        .tiles = { COORD_TO_TILE(12, 26), COORD_TO_TILE(12, 27), COORD_TO_TILE(13, 26), COORD_TO_TILE(13, 27) },
        .unlockCount = 11,
        .name = COMPOUND_STRING("Ambient: Weakness"),
        .description = COMPOUND_STRING("Player mons can only use the first 3 moves."),
        .lockTiles = {
            COORD_TO_TILE(11, 24), COORD_TO_TILE(12, 24), COORD_TO_TILE(13, 24),
            COORD_TO_TILE(11, 25), COORD_TO_TILE(12, 25), COORD_TO_TILE(13, 25),
        },
        .lockTilemap =
        {
            256 + 0xD5, 0xD5, 0xD5 | FLIP_H,
            256 + 0xE1, 0xE2, 798,
        },
        .unlockTilemap = {
            769, 770, 0x0D,
            772, 773, 774,
        },
    },
    [RISK_PLAYER_HAS_PARENTAL_BOND] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(16, 34), COORD_TO_TILE(16, 35), COORD_TO_TILE(17, 34), COORD_TO_TILE(17, 35) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Dual Blades"),
        .description = COMPOUND_STRING("Player mons have Parental Bond.\nPlayer can only use the first 2 moves."),
    },
    [RISK_PLAYER_HAS_FILTER] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(18, 34), COORD_TO_TILE(18, 35), COORD_TO_TILE(19, 34), COORD_TO_TILE(19, 35) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Enhanced Armor"),
        .description = COMPOUND_STRING("Player mons have Filter.\nPlayer can only use the first 2 moves."),
    },
    [RISK_PLAYER_HAS_PERISH_BODY] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(20, 34), COORD_TO_TILE(20, 35), COORD_TO_TILE(21, 34), COORD_TO_TILE(21, 35) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Tolling Bells"),
        .description = COMPOUND_STRING("Player mons have Perish Body.\nPlayer can only use the first 2 moves."),
    },
    [RISK_PLAYER_HAS_BEAST_BOOST] =
    {
        .linkedRisks = sLinkedAbilities,
        .tiles = { COORD_TO_TILE(22, 34), COORD_TO_TILE(22, 35), COORD_TO_TILE(23, 34), COORD_TO_TILE(23, 35) },
        .linkedCount = 4,
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Primal Surge"),
        .description = COMPOUND_STRING("Player mons have Beast Boost.\nPlayer can only use the first 2 moves."),
    },
    [RISK_HAS_MOLD_BREAKER] =
    {
        .tiles = { COORD_TO_TILE(16, 26), COORD_TO_TILE(16, 27), COORD_TO_TILE(17, 26), COORD_TO_TILE(17, 27) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Piercing Ammo"),
        .description = COMPOUND_STRING("Opponent mons have Mold Breaker."),
    },
    [RISK_HAS_STURDY] =
    {
        .tiles = { COORD_TO_TILE(18, 28), COORD_TO_TILE(18, 29), COORD_TO_TILE(19, 28), COORD_TO_TILE(19, 29) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Last Stand"),
        .description = COMPOUND_STRING("Opponent mons have Sturdy."),
    },
    [RISK_HAS_REGENERATOR] =
    {
        .tiles = { COORD_TO_TILE(20, 26), COORD_TO_TILE(20, 27), COORD_TO_TILE(21, 26), COORD_TO_TILE(21, 27) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Second Wind"),
        .description = COMPOUND_STRING("Opponent mons have Regenerator."),
    },
    [RISK_HAS_BATTLE_ARMOR] =
    {
        .tiles = { COORD_TO_TILE(22, 26), COORD_TO_TILE(22, 27), COORD_TO_TILE(23, 26), COORD_TO_TILE(23, 27) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Impenetrable"),
        .description = COMPOUND_STRING("Opponent mons have Battle Armor."),
    },
    [RISK_HAS_WONDER_GUARD] =
    {
        .tiles = { COORD_TO_TILE(19, 30), COORD_TO_TILE(19, 31), COORD_TO_TILE(20, 30), COORD_TO_TILE(20, 31) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Camouflage"),
        .description = COMPOUND_STRING("Opponent mons have Wonder Guard.\nThere's a reason why only Shedinja\nhas this normally…"),
    },
    [RISK_HAS_FILTER] =
    {
        .tiles = { COORD_TO_TILE(18, 26), COORD_TO_TILE(18, 27), COORD_TO_TILE(19, 26), COORD_TO_TILE(19, 27) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Enhanced Armor"),
        .description = COMPOUND_STRING("Opponent mons have Filter."),
    },
    [RISK_HAS_ADAPTABILITY] =
    {
        .tiles = { COORD_TO_TILE(20, 28), COORD_TO_TILE(20, 29), COORD_TO_TILE(21, 28), COORD_TO_TILE(21, 29) },
        .lockRisk = RISK_MINUS_1_MOVE,
        .name = COMPOUND_STRING("Target: Honed Instinct"),
        .description = COMPOUND_STRING("Opponent mons have Adaptability."),
    },
};

const enum Risk sRiskMap[64][64] =
{
    [6][2] = RISK_RESET,
    [6][3] = RISK_RESET,
    [7][2] = RISK_RESET,
    [7][3] = RISK_RESET,

    [12][4] = RISK_OPPONENT_HP_1,
    [12][5] = RISK_OPPONENT_HP_1,
    [13][4] = RISK_OPPONENT_HP_1,
    [13][5] = RISK_OPPONENT_HP_1,

    [12][6] = RISK_OPPONENT_HP_2,
    [12][7] = RISK_OPPONENT_HP_2,
    [13][6] = RISK_OPPONENT_HP_2,
    [13][7] = RISK_OPPONENT_HP_2,

    [12][8] = RISK_OPPONENT_HP_3,
    [12][9] = RISK_OPPONENT_HP_3,
    [13][8] = RISK_OPPONENT_HP_3,
    [13][9] = RISK_OPPONENT_HP_3,

    [14][4] = RISK_TURN_LIMIT_1,
    [14][5] = RISK_TURN_LIMIT_1,
    [15][4] = RISK_TURN_LIMIT_1,
    [15][5] = RISK_TURN_LIMIT_1,

    [14][6] = RISK_TURN_LIMIT_2,
    [14][7] = RISK_TURN_LIMIT_2,
    [15][6] = RISK_TURN_LIMIT_2,
    [15][7] = RISK_TURN_LIMIT_2,

    [14][8] = RISK_TURN_LIMIT_3,
    [14][9] = RISK_TURN_LIMIT_3,
    [15][8] = RISK_TURN_LIMIT_3,
    [15][9] = RISK_TURN_LIMIT_3,

    [33][25] = RISK_PARTY_MINUS_1,
    [33][26] = RISK_PARTY_MINUS_1,
    [34][25] = RISK_PARTY_MINUS_1,
    [34][26] = RISK_PARTY_MINUS_1,

    [33][27] = RISK_PARTY_MINUS_2,
    [33][28] = RISK_PARTY_MINUS_2,
    [34][27] = RISK_PARTY_MINUS_2,
    [34][28] = RISK_PARTY_MINUS_2,

    [33][29] = RISK_PARTY_MINUS_3,
    [33][30] = RISK_PARTY_MINUS_3,
    [34][29] = RISK_PARTY_MINUS_3,
    [34][30] = RISK_PARTY_MINUS_3,

    [18][4] = RISK_MUST_SWITCH_1,
    [18][5] = RISK_MUST_SWITCH_1,
    [19][4] = RISK_MUST_SWITCH_1,
    [19][5] = RISK_MUST_SWITCH_1,

    [18][6] = RISK_MUST_SWITCH_2,
    [18][7] = RISK_MUST_SWITCH_2,
    [19][6] = RISK_MUST_SWITCH_2,
    [19][7] = RISK_MUST_SWITCH_2,

    [18][8] = RISK_MUST_SWITCH_3,
    [18][9] = RISK_MUST_SWITCH_3,
    [19][8] = RISK_MUST_SWITCH_3,
    [19][9] = RISK_MUST_SWITCH_3,

    [20][6] = RISK_CANT_SWITCH,
    [20][7] = RISK_CANT_SWITCH,
    [21][6] = RISK_CANT_SWITCH,
    [21][7] = RISK_CANT_SWITCH,

    [26][8] = RISK_PLAYER_STARTS_WITH_BURN,
    [26][9] = RISK_PLAYER_STARTS_WITH_BURN,
    [27][8] = RISK_PLAYER_STARTS_WITH_BURN,
    [27][9] = RISK_PLAYER_STARTS_WITH_BURN,

    [30][8] = RISK_PLAYER_STARTS_WITH_FROSTBITE,
    [30][9] = RISK_PLAYER_STARTS_WITH_FROSTBITE,
    [31][8] = RISK_PLAYER_STARTS_WITH_FROSTBITE,
    [31][9] = RISK_PLAYER_STARTS_WITH_FROSTBITE,

    [28][8] = RISK_PLAYER_STARTS_WITH_PARALYSIS,
    [28][9] = RISK_PLAYER_STARTS_WITH_PARALYSIS,
    [29][8] = RISK_PLAYER_STARTS_WITH_PARALYSIS,
    [29][9] = RISK_PLAYER_STARTS_WITH_PARALYSIS,

    [31][25] = RISK_OPPONENT_MORE_MONS_1,
    [31][26] = RISK_OPPONENT_MORE_MONS_1,
    [32][25] = RISK_OPPONENT_MORE_MONS_1,
    [32][26] = RISK_OPPONENT_MORE_MONS_1,

    [31][27] = RISK_OPPONENT_MORE_MONS_2,
    [31][28] = RISK_OPPONENT_MORE_MONS_2,
    [32][27] = RISK_OPPONENT_MORE_MONS_2,
    [32][28] = RISK_OPPONENT_MORE_MONS_2,

    [27][23] = RISK_HAS_OMNISCIENT_AI,
    [27][24] = RISK_HAS_OMNISCIENT_AI,
    [28][23] = RISK_HAS_OMNISCIENT_AI,
    [28][24] = RISK_HAS_OMNISCIENT_AI,

    [27][25] = RISK_HAS_PREDICTION_AI,
    [27][26] = RISK_HAS_PREDICTION_AI,
    [28][25] = RISK_HAS_PREDICTION_AI,
    [28][26] = RISK_HAS_PREDICTION_AI,

    [29][21] = RISK_RANDOM_LEAD,
    [29][22] = RISK_RANDOM_LEAD,
    [30][21] = RISK_RANDOM_LEAD,
    [30][22] = RISK_RANDOM_LEAD,

    [29][27] = RISK_USES_POOLS,
    [29][28] = RISK_USES_POOLS,
    [30][27] = RISK_USES_POOLS,
    [30][28] = RISK_USES_POOLS,

    [25][6] = RISK_PLAYER_JUST_BERRIES,
    [25][7] = RISK_PLAYER_JUST_BERRIES,
    [26][6] = RISK_PLAYER_JUST_BERRIES,
    [26][7] = RISK_PLAYER_JUST_BERRIES,

    [27][6] = RISK_NO_PP_RESTORE,
    [27][7] = RISK_NO_PP_RESTORE,
    [28][6] = RISK_NO_PP_RESTORE,
    [28][7] = RISK_NO_PP_RESTORE,

    [29][29] = RISK_OPPONENT_HAS_ITEMS,
    [29][30] = RISK_OPPONENT_HAS_ITEMS,
    [30][29] = RISK_OPPONENT_HAS_ITEMS,
    [30][30] = RISK_OPPONENT_HAS_ITEMS,

    [28][4] = RISK_NO_ORDER_CHANGE,
    [28][5] = RISK_NO_ORDER_CHANGE,
    [29][4] = RISK_NO_ORDER_CHANGE,
    [29][5] = RISK_NO_ORDER_CHANGE,

    [12][12] = RISK_FLIP_TYPE_CHART,
    [12][13] = RISK_FLIP_TYPE_CHART,
    [13][12] = RISK_FLIP_TYPE_CHART,
    [13][13] = RISK_FLIP_TYPE_CHART,

    [29][6] = RISK_ATTACK_GETS_DROWSY,
    [29][7] = RISK_ATTACK_GETS_DROWSY,
    [30][6] = RISK_ATTACK_GETS_DROWSY,
    [30][7] = RISK_ATTACK_GETS_DROWSY,

    [12][14] = RISK_HAS_GEN_1_CRIT_CHANCE,
    [12][15] = RISK_HAS_GEN_1_CRIT_CHANCE,
    [13][14] = RISK_HAS_GEN_1_CRIT_CHANCE,
    [13][15] = RISK_HAS_GEN_1_CRIT_CHANCE,

    [31][6] = RISK_STATUS_GETS_PARA,
    [31][7] = RISK_STATUS_GETS_PARA,
    [32][6] = RISK_STATUS_GETS_PARA,
    [32][7] = RISK_STATUS_GETS_PARA,

    [16][20] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    [16][21] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    [17][20] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,
    [17][21] = RISK_PLAYER_LOWER_DAMAGE_ROLLS,

    [18][20] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    [18][21] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    [19][20] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,
    [19][21] = RISK_OPPONENT_HIGHER_DAMAGE_ROLLS,

    [20][20] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    [20][21] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    [21][20] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,
    [21][21] = RISK_PLAYER_HAS_NEGATIVE_METRONOME,

    [22][20] = RISK_FOE_HAS_METRONOME,
    [22][21] = RISK_FOE_HAS_METRONOME,
    [23][20] = RISK_FOE_HAS_METRONOME,
    [23][21] = RISK_FOE_HAS_METRONOME,

    [19][22] = RISK_PLAYER_HAS_RECOIL,
    [19][23] = RISK_PLAYER_HAS_RECOIL,
    [20][22] = RISK_PLAYER_HAS_RECOIL,
    [20][23] = RISK_PLAYER_HAS_RECOIL,

    [20][14] = RISK_HAS_GUARANTEED_ACCURACY,
    [20][15] = RISK_HAS_GUARANTEED_ACCURACY,
    [21][14] = RISK_HAS_GUARANTEED_ACCURACY,
    [21][15] = RISK_HAS_GUARANTEED_ACCURACY,

    [18][16] = RISK_OPPONENT_MOVES_FIRST,
    [18][17] = RISK_OPPONENT_MOVES_FIRST,
    [19][16] = RISK_OPPONENT_MOVES_FIRST,
    [19][17] = RISK_OPPONENT_MOVES_FIRST,

    [18][14] = RISK_OPPONENT_ATTACKS_SWITCHES,
    [18][15] = RISK_OPPONENT_ATTACKS_SWITCHES,
    [19][14] = RISK_OPPONENT_ATTACKS_SWITCHES,
    [19][15] = RISK_OPPONENT_ATTACKS_SWITCHES,

    [16][14] = RISK_HAS_GUARANTEED_EFFECTS,
    [16][15] = RISK_HAS_GUARANTEED_EFFECTS,
    [17][14] = RISK_HAS_GUARANTEED_EFFECTS,
    [17][15] = RISK_HAS_GUARANTEED_EFFECTS,

    [16][12] = RISK_OPPONENT_ATTACKS_DISABLE,
    [16][13] = RISK_OPPONENT_ATTACKS_DISABLE,
    [17][12] = RISK_OPPONENT_ATTACKS_DISABLE,
    [17][13] = RISK_OPPONENT_ATTACKS_DISABLE,

    [20][12] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    [20][13] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    [21][12] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,
    [21][13] = RISK_OPPONENT_INFLICTS_GASTRO_ACID,

    [18][12] = RISK_OPPONENT_ATTACKS_TORMENT,
    [18][13] = RISK_OPPONENT_ATTACKS_TORMENT,
    [19][12] = RISK_OPPONENT_ATTACKS_TORMENT,
    [19][13] = RISK_OPPONENT_ATTACKS_TORMENT,

    [24][12] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    [24][13] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    [25][12] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,
    [25][13] = RISK_PLAYER_HAZARDS_NOT_REMOVABLE,

    [34][12] = RISK_PLAYER_SPIKES_1,
    [34][13] = RISK_PLAYER_SPIKES_1,
    [35][12] = RISK_PLAYER_SPIKES_1,
    [35][13] = RISK_PLAYER_SPIKES_1,

    [34][14] = RISK_PLAYER_SPIKES_2,
    [34][15] = RISK_PLAYER_SPIKES_2,
    [35][14] = RISK_PLAYER_SPIKES_2,
    [35][15] = RISK_PLAYER_SPIKES_2,

    [34][16] = RISK_PLAYER_SPIKES_3,
    [34][17] = RISK_PLAYER_SPIKES_3,
    [35][16] = RISK_PLAYER_SPIKES_3,
    [35][17] = RISK_PLAYER_SPIKES_3,

    [32][14] = RISK_PLAYER_TOXIC_SPIKES_1,
    [32][15] = RISK_PLAYER_TOXIC_SPIKES_1,
    [33][14] = RISK_PLAYER_TOXIC_SPIKES_1,
    [33][15] = RISK_PLAYER_TOXIC_SPIKES_1,

    [32][16] = RISK_PLAYER_TOXIC_SPIKES_2,
    [32][17] = RISK_PLAYER_TOXIC_SPIKES_2,
    [33][16] = RISK_PLAYER_TOXIC_SPIKES_2,
    [33][17] = RISK_PLAYER_TOXIC_SPIKES_2,

    [26][16] = RISK_PLAYER_STEALTH_ROCK,
    [26][17] = RISK_PLAYER_STEALTH_ROCK,
    [27][16] = RISK_PLAYER_STEALTH_ROCK,
    [27][17] = RISK_PLAYER_STEALTH_ROCK,

    [28][16] = RISK_PLAYER_SHARP_STEEL,
    [28][17] = RISK_PLAYER_SHARP_STEEL,
    [29][16] = RISK_PLAYER_SHARP_STEEL,
    [29][17] = RISK_PLAYER_SHARP_STEEL,

    [30][16] = RISK_PLAYER_STICKY_WEB,
    [30][17] = RISK_PLAYER_STICKY_WEB,
    [31][16] = RISK_PLAYER_STICKY_WEB,
    [31][17] = RISK_PLAYER_STICKY_WEB,

    [12][26] = RISK_MINUS_1_MOVE,
    [12][27] = RISK_MINUS_1_MOVE,
    [13][26] = RISK_MINUS_1_MOVE,
    [13][27] = RISK_MINUS_1_MOVE,

    [16][34] = RISK_PLAYER_HAS_PARENTAL_BOND,
    [16][35] = RISK_PLAYER_HAS_PARENTAL_BOND,
    [17][34] = RISK_PLAYER_HAS_PARENTAL_BOND,
    [17][35] = RISK_PLAYER_HAS_PARENTAL_BOND,

    [18][34] = RISK_PLAYER_HAS_FILTER,
    [18][35] = RISK_PLAYER_HAS_FILTER,
    [19][34] = RISK_PLAYER_HAS_FILTER,
    [19][35] = RISK_PLAYER_HAS_FILTER,

    [20][34] = RISK_PLAYER_HAS_PERISH_BODY,
    [20][35] = RISK_PLAYER_HAS_PERISH_BODY,
    [21][34] = RISK_PLAYER_HAS_PERISH_BODY,
    [21][35] = RISK_PLAYER_HAS_PERISH_BODY,

    [22][34] = RISK_PLAYER_HAS_BEAST_BOOST,
    [22][35] = RISK_PLAYER_HAS_BEAST_BOOST,
    [23][34] = RISK_PLAYER_HAS_BEAST_BOOST,
    [23][35] = RISK_PLAYER_HAS_BEAST_BOOST,

    [16][26] = RISK_HAS_MOLD_BREAKER,
    [16][27] = RISK_HAS_MOLD_BREAKER,
    [17][26] = RISK_HAS_MOLD_BREAKER,
    [17][27] = RISK_HAS_MOLD_BREAKER,

    [18][28] = RISK_HAS_STURDY,
    [18][29] = RISK_HAS_STURDY,
    [19][28] = RISK_HAS_STURDY,
    [19][29] = RISK_HAS_STURDY,

    [20][26] = RISK_HAS_REGENERATOR,
    [20][27] = RISK_HAS_REGENERATOR,
    [21][26] = RISK_HAS_REGENERATOR,
    [21][27] = RISK_HAS_REGENERATOR,

    [22][26] = RISK_HAS_BATTLE_ARMOR,
    [22][27] = RISK_HAS_BATTLE_ARMOR,
    [23][26] = RISK_HAS_BATTLE_ARMOR,
    [23][27] = RISK_HAS_BATTLE_ARMOR,

    [19][30] = RISK_HAS_WONDER_GUARD,
    [19][31] = RISK_HAS_WONDER_GUARD,
    [20][30] = RISK_HAS_WONDER_GUARD,
    [20][31] = RISK_HAS_WONDER_GUARD,

    [18][26] = RISK_HAS_FILTER,
    [18][27] = RISK_HAS_FILTER,
    [19][27] = RISK_HAS_FILTER,
    [19][26] = RISK_HAS_FILTER,

    [20][28] = RISK_HAS_ADAPTABILITY,
    [20][29] = RISK_HAS_ADAPTABILITY,
    [21][28] = RISK_HAS_ADAPTABILITY,
    [21][29] = RISK_HAS_ADAPTABILITY,
};

static EWRAM_DATA struct RiskUiState *sRiskUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;

static const u32 sBackgroundTiles[] = INCGFX_U32("graphics/risk_ui/background.png", ".4bpp.smol");
static const u32 sBackgroundTilemap[] = INCBIN_U32("graphics/risk_ui/background.bin.smolTM");
static const u16 sBackgroundPalette[] = INCGFX_U16("graphics/risk_ui/background.png", ".gbapal");

static const u32 sSelectorGfx[] = INCGFX_U32("graphics/risk_ui/selector.png", ".4bpp");
static const u16 sSelectorPal[] = INCGFX_U16("graphics/risk_ui/selector.png", ".gbapal");

//static const u32 sFrameGfx[] = INCGFX_U32("graphics/risk_ui/frame_new.png", ".4bpp.smol");
static const u32 sFrameTilemap[] = INCBIN_U32("graphics/risk_ui/frame_new.bin.smolTM");
static const u16 sFramePal[] = INCGFX_U16("graphics/risk_ui/frame_new.png", ".gbapal");

static const u32 sTotalIconGfx[] = INCGFX_U32("graphics/risk_ui/total_icon.png", ".4bpp");
static const u16 sTotalIconPal[] = INCGFX_U16("graphics/risk_ui/total_icon.png", ".gbapal");

static const struct BgTemplate sRiskUiBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 1,
        .screenSize = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 26,
        .priority = 2,
        .screenSize = 3,
    },
    {
        .bg = 2,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
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
static void TrySelectRiskUnderCursor(void);
static inline void SetRiskInactive(enum Risk risk);
static inline void SetRiskActive(enum Risk risk);
static void ChangeTilemapPalettesBeforeLoad(void);
static enum Risk GetRiskUnderCursor(void);
static void PrintRiskData(enum Risk risk);
static void ToggleLock(enum Risk risk, bool32 beforeLoad);
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
        //DecompressAndCopyTileDataToVram(2, sFrameGfx, 0, 0, 0);
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
        if (GetRiskUnderCursor() != RISK_NONE)
        {
            sRiskUiState->isShowingDescription = TRUE;
            gSprites[sRiskUiState->selectorId].invisible = TRUE;
            gTasks[taskId].func = Task_DisplayDescription;
        }
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
            if (sRiskUiState->xOffset < 114)
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
            if (sRiskUiState->yOffset < 198)
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

static enum Risk GetRiskUnderCursor(void)
{
    u32 xSel = (sRiskUiState->xSelector + sRiskUiState->xOffset + 4) / 8 - 1;
    u32 ySel = (sRiskUiState->ySelector + sRiskUiState->yOffset + 4) / 8 - 1;

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
            for (u32 i = 0; i < sRiskData[risk].linkedCount; i++)
            {
                SetRiskInactive(sRiskData[risk].linkedRisks[i]);
            }
        }
        else
        {
            for (u32 i = 0; i < sRiskData[risk].linkedCount; i++)
            {
                SetRiskInactive(sRiskData[risk].linkedRisks[i]);

                SetTilePalette(sRiskData[sRiskData[risk].linkedRisks[i]].tiles[0], PAL_INDEX_LOCKED);
                SetTilePalette(sRiskData[sRiskData[risk].linkedRisks[i]].tiles[1], PAL_INDEX_LOCKED);
                SetTilePalette(sRiskData[sRiskData[risk].linkedRisks[i]].tiles[2], PAL_INDEX_LOCKED);
                SetTilePalette(sRiskData[sRiskData[risk].linkedRisks[i]].tiles[3], PAL_INDEX_LOCKED);
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
        ToggleLock(risk, FALSE);
}

static inline void SetRiskActive(enum Risk risk)
{
    SetTilePalette(sRiskData[risk].tiles[0], PAL_INDEX_ACTIVE);
    SetTilePalette(sRiskData[risk].tiles[1], PAL_INDEX_ACTIVE);
    SetTilePalette(sRiskData[risk].tiles[2], PAL_INDEX_ACTIVE);
    SetTilePalette(sRiskData[risk].tiles[3], PAL_INDEX_ACTIVE);
    SetRisk(risk);
    if (sRiskData[risk].unlockCount > 0)
        ToggleLock(risk, FALSE);
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
            if (sRiskData[risk].linkedCount > 0)
            {
                for (u32 i = 0; i < sRiskData[risk].linkedCount; i++)
                {
                    enum Risk tempRisk = sRiskData[risk].linkedRisks[i];
                    if (risk != tempRisk)
                    {
                        for (u32 i = 0; i < 4; i++)
                        {
                            u32 tileNum = sRiskData[tempRisk].tiles[i];
                            u16 *tilemapPtr = (u16 *)sBg1TilemapBuffer;
                            u16 palMask = PAL_INDEX_LOCKED << 12;
                            u16 currVal = tilemapPtr[tileNum] & 0xFFF;
                            tilemapPtr[tileNum] = palMask | currVal;
                        }
                    }
                }
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
        if (sRiskData[risk].unlockCount > 0)
            ToggleLock(risk, TRUE);
    }
}

static void PrintRiskData(enum Risk risk)
{
    //  First clear out windows
    FillWindowPixelBuffer(WIN_RISK_NAME, PIXEL_FILL(2));
    FillWindowPixelBuffer(WIN_RISK_DESCRIPTION, PIXEL_FILL(2));
    FillWindowPixelBuffer(WIN_RISK_TOTAL, PIXEL_FILL(5));

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

static void ToggleLock(enum Risk risk, bool32 beforeLoad)
{
    assertf(sRiskData[risk].unlockCount > 0, "Attempting to unlock non-lock risk")
    {
        return;
    }
    u32 palette;
    u16 *tilemapPtr = (u16 *)(BG_VRAM + sRiskUiBgTemplates[1].mapBaseIndex * BG_SCREEN_SIZE);
    if (beforeLoad)
        tilemapPtr = (u16 *)sBg1TilemapBuffer;

    if (IsRiskActive(risk))
    {
        palette = PAL_INDEX_INACTIVE;
        //  Unlock the lock
        for (u32 i = 0; i < 6; i++)
        {
            u32 offset = sRiskData[risk].lockTiles[i];
            u16 newTileId = sRiskData[risk].unlockTilemap[i];
            tilemapPtr[offset] = newTileId | (PAL_INDEX_ACTIVE << 12);
        }

        //  Borders
        if (risk == RISK_MINUS_1_MOVE)
        {
            for (u32 x = 14; x <= 25; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 25)] |= (PAL_INDEX_ACTIVE << 12);
            }
            for (u32 y = 26; y <= 37; y++)
            {
                tilemapPtr[COORD_TO_TILE(24, y)] |= (PAL_INDEX_ACTIVE << 12);
                tilemapPtr[COORD_TO_TILE(25, y)] |= (PAL_INDEX_ACTIVE << 12);
            }
            for (u32 x = 12; x <= 23; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 36)] |= (PAL_INDEX_ACTIVE << 12);
                tilemapPtr[COORD_TO_TILE(x, 37)] |= (PAL_INDEX_ACTIVE << 12);
            }
            for (u32 y = 28; y <= 35; y++)
            {
                tilemapPtr[COORD_TO_TILE(12, y)] |= (PAL_INDEX_ACTIVE << 12);
                tilemapPtr[COORD_TO_TILE(13, y)] |= (PAL_INDEX_ACTIVE << 12);
            }
        }
        else
        {
            for (u32 x = 26; x <= 36; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 11)] |= (PAL_INDEX_ACTIVE << 12);
            }
            for (u32 y = 12; y <= 18; y++)
            {
                tilemapPtr[COORD_TO_TILE(36, y)] |= (PAL_INDEX_ACTIVE << 12);
            }
            for (u32 x = 24; x <= 35; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 18)] |= (PAL_INDEX_ACTIVE << 12);
            }
            for (u32 y = 14; y <= 17; y++)
            {
                tilemapPtr[COORD_TO_TILE(24, y)] |= (PAL_INDEX_ACTIVE << 12);
                tilemapPtr[COORD_TO_TILE(25, y)] |= (PAL_INDEX_ACTIVE << 12);
            }
        }
    }
    else
    {
        palette = PAL_INDEX_LOCKED;
        //  Lock the lock
        for (u32 i = 0; i < 6; i++)
        {
            u32 offset = sRiskData[risk].lockTiles[i];
            u16 newTileId = sRiskData[risk].lockTilemap[i];
            tilemapPtr[offset] = newTileId;
        }

        //  Borders
        if (risk == RISK_MINUS_1_MOVE)
        {
            for (u32 x = 14; x <= 25; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 25)] &= 0x0FFF;
            }
            for (u32 y = 26; y <= 37; y++)
            {
                tilemapPtr[COORD_TO_TILE(24, y)] &= 0x0FFF;
                tilemapPtr[COORD_TO_TILE(25, y)] &= 0x0FFF;
            }
            for (u32 x = 12; x <= 23; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 36)] &= 0x0FFF;
                tilemapPtr[COORD_TO_TILE(x, 37)] &= 0x0FFF;
            }
            for (u32 y = 28; y <= 35; y++)
            {
                tilemapPtr[COORD_TO_TILE(12, y)] &= 0x0FFF;
                tilemapPtr[COORD_TO_TILE(13, y)] &= 0x0FFF;
            }
        }
        else
        {
            for (u32 x = 26; x <= 36; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 11)] &= 0x0FFF;
            }
            for (u32 y = 12; y <= 18; y++)
            {
                tilemapPtr[COORD_TO_TILE(36, y)] &= 0x0FFF;
            }
            for (u32 x = 24; x <= 35; x++)
            {
                tilemapPtr[COORD_TO_TILE(x, 18)] &= 0x0FFF;
            }
            for (u32 y = 14; y <= 17; y++)
            {
                tilemapPtr[COORD_TO_TILE(24, y)] &= 0x0FFF;
                tilemapPtr[COORD_TO_TILE(25, y)] &= 0x0FFF;
            }
        }
    }

    for (u32 i = 0; i < sRiskData[risk].unlockCount; i++)
    {
        enum Risk lockRisk = sRiskData[risk].unlockedRisks[i];
        if (beforeLoad && IsRiskActive(risk))
            continue;

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
