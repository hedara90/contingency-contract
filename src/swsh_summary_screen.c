#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_anim.h"
#include "frontier_util.h"
#include "battle_message.h"
#include "battle_tent.h"
#include "battle_factory.h"
#include "bg.h"
#include "swsh_summary_screen.h"
#include "comfy_anim.h"
#include "contest.h"
#include "contest_effect.h"
#include "data.h"
#include "daycare.h"
#include "decompress.h"
#include "dynamic_placeholder_text_util.h"
#include "egg_hatch.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "link.h"
#include "m4a.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "menu_specialized.h"
#include "trig.h"
#include "mon_markings.h"
#include "move_relearner.h"
#include "naming_screen.h"
#include "party_menu.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "pokemon_sprite_visualizer.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "pokerus.h"
#include "region_map.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "tv.h"
#include "window.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "constants/characters.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#if SWSH_SUMMARY_SCREEN == TRUE
enum SWSHPSSEffect
{
    PSS_EFFECT_BATTLE,
    PSS_EFFECT_CONTEST,
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    PSS_EFFECT_CONDITIONS,
#endif
    PSS_EFFECT_COUNT
};

enum SWSHSkillsPageState
{
    SKILL_STATE_STATS,
    SKILL_STATE_IVS,
    SKILL_STATE_EVS,
};

#define PSS_BUFFER_SIZE 0x400

#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
#define SUMMARY_GRAPH_CENTER_X  80
#define SUMMARY_GRAPH_CENTER_Y  81
#define SUMMARY_GRAPH_TOP_Y     (SUMMARY_GRAPH_CENTER_Y - 35)
#define SUMMARY_GRAPH_BOTTOM_Y  (SUMMARY_GRAPH_CENTER_Y + 30)
#define SUMMARY_GRAPH_HEIGHT    (SUMMARY_GRAPH_BOTTOM_Y - SUMMARY_GRAPH_TOP_Y + 1)
// First free tile in charblock 2 after sSummaryScreen_Gfx (384 tiles used)
#define SUMMARY_GRAPH_FILL_TILE 384
#define SHIFT_RIGHT_ADJUSTED(n, s) (((n) >> (s)) + (((n) >> ((s) - 1)) & 1))

static const u8 sSummaryConditionToLineLength[MAX_CONDITION + 1] =
{
     4,  5,  6,  7,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 13,
    13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17,
    17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19,
    19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21,
    21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
    31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33,
    33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34,
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35
};
#endif

// Button prompts (bottom right)
#define PSS_LABEL_WINDOW_PROMPT_CANCEL 0 // Also handles the "rename" prompt if P_SUMMARY_SCREEN_RENAME is true
#define PSS_LABEL_WINDOW_PROMPT_SWITCH 1

// Moves screen
#define PSS_LABEL_WINDOW_MOVES_POWER_ACC 2 // Also contains the power and accuracy values

// Above/below the pokemon's portrait (right)
#define PSS_LABEL_WINDOW_PORTRAIT_INFO 3

// button prompt for cycling IV/EV/stats views
#define PSS_LABEL_WINDOW_PROMPT_IV_EV_STATS 4
#define PSS_LABEL_WINDOW_PROMPT_MOVES 5

#define PSS_LABEL_WINDOW_END 6

// Dynamic fields for the Pokémon Info page
#define PSS_DATA_WINDOW_INFO_ITEM 0
#define PSS_DATA_WINDOW_INFO_SPECIES 1
// #define PSS_DATA_WINDOW_INFO_OT_OTID 3

// Dynamic fields for the Pokémon Skills page
#define PSS_DATA_WINDOW_SKILLS_STATS 0
#define PSS_DATA_WINDOW_SKILLS_ABILITY 1

// Dynamic fields for the Battle Moves page
#define PSS_DATA_WINDOW_MOVE_DESCRIPTION 0

// Dynamic fields for the Pokémon Memo page
#define PSS_DATA_WINDOW_MEMO_NOTE 0
#define PSS_DATA_WINDOW_MEMO_EXP 1

#define MOVE_SLOT_COUNT 5
#define MOVE_SLOT_SPRITES_COUNT 5
#define MOVE_FRAME_SPRITES_COUNT 10
#define DYNAMAX_LEVEL_SPRITES_COUNT 10
#define TYPE_ICON_SPRITE_COUNT (MAX_MON_MOVES + 1)
#define MON_CHARACTERISTIC_COUNT (6 * 5)

// Default font, see PrintTextOnWindow
#define PSS_DEFAULT_FONT FONT_SHORT_NARROW
// for the spriteIds field in PokemonSummaryScreenData
enum SwShSummarySprites
{
    SPRITE_ARR_ID_MON,
    SPRITE_ARR_ID_SHADOW,
    SPRITE_ARR_ID_BALL,
    SPRITE_ARR_ID_STATUS,
    SPRITE_ARR_ID_GENDER,
    SPRITE_ARR_ID_GIGANTAMAX,
    // all sprites below are considered "page-specific" and will be hidden when switching pages
    SPRITE_ARR_ID_POKERUS_CURED,
    SPRITE_ARR_ID_ITEM,
    SPRITE_ARR_ID_SHINY,
    SPRITE_ARR_ID_FRIENDSHIP,
    SPRITE_ARR_ID_CATEGORY,
    SPRITE_ARR_ID_HP_GRADE,
    SPRITE_ARR_ID_ATK_GRADE,
    SPRITE_ARR_ID_DEF_GRADE,
    SPRITE_ARR_ID_SPA_GRADE,
    SPRITE_ARR_ID_SPD_GRADE,
    SPRITE_ARR_ID_SPE_GRADE,
    SPRITE_ARR_ID_LR_BUTTON,
    SPRITE_ARR_ID_INFO_PROMPT,
    SPRITE_ARR_ID_TERA_TYPE,
    SPRITE_ARR_ID_TYPE, // 2 for mon types, 5 for move types(4 moves and 1 to learn), used interchangeably, because mon types and move types aren't shown on the same screen
    SPRITE_ARR_ID_MOVE_SLOT = SPRITE_ARR_ID_TYPE + TYPE_ICON_SPRITE_COUNT,
    SPRITE_ARR_ID_MOVE_CURSOR = SPRITE_ARR_ID_MOVE_SLOT + (MOVE_SLOT_SPRITES_COUNT * MOVE_SLOT_COUNT),
    SPRITE_ARR_ID_MOVE_FRAME = SPRITE_ARR_ID_MOVE_CURSOR + 1,
    SPRITE_ARR_ID_DYNAMAX_LEVEL = SPRITE_ARR_ID_MOVE_FRAME + MOVE_FRAME_SPRITES_COUNT,
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    SPRITE_ARR_ID_CONTEST_CATEGORY = SPRITE_ARR_ID_DYNAMAX_LEVEL + DYNAMAX_LEVEL_SPRITES_COUNT,
    SPRITE_ARR_ID_MAX_COND_SPARKLE = SPRITE_ARR_ID_CONTEST_CATEGORY + CONTEST_CATEGORIES_COUNT,
    SPRITE_ARR_ID_COUNT = SPRITE_ARR_ID_MAX_COND_SPARKLE + CONTEST_CATEGORIES_COUNT,
#else
    SPRITE_ARR_ID_COUNT = SPRITE_ARR_ID_DYNAMAX_LEVEL + DYNAMAX_LEVEL_SPRITES_COUNT,
#endif
};

static EWRAM_DATA struct PokemonSummaryScreenData
{
    /*0x00*/ union {
        struct Pokemon *mons;
        struct BoxPokemon *boxMons;
    } monList;
    /*0x04*/ MainCallback callback;
    /*0x08*/ struct Sprite *markingsSprite;
    /*0x0C*/ void *markingComboTilesPtr;
    /*0x10*/ struct Pokemon currentMon;
    /*0x70*/ struct PokeSummary
    {
        enum Species species; // 0x0
        enum Species species2; // 0x2
        u8 isEgg:1; // 0x4
        u8 isShiny:1;
        u8 padding:6;
        u8 level; // 0x5
        u8 ribbonCount; // 0x6
        u8 ailment; // 0x7
        u8 abilityNum; // 0x8
        u8 metLocation; // 0x9
        u8 metLevel; // 0xA
        u8 metGame; // 0xB
        u32 pid; // 0xC
        u32 exp; // 0x10
        enum Move moves[MAX_MON_MOVES]; // 0x14
        u8 pp[MAX_MON_MOVES]; // 0x1C
        u16 currentHP; // 0x20
        u16 maxHP; // 0x22
        u16 atk; // 0x24
        u16 def; // 0x26
        u16 spatk; // 0x28
        u16 spdef; // 0x2A
        u16 speed; // 0x2C
        enum Item item; // 0x2E
        u16 friendship; // 0x30
        u8 OTGender; // 0x32
        u8 nature; // 0x33
        u8 ppBonuses; // 0x34
        u8 sanity; // 0x35
        u8 OTName[17]; // 0x36
        u32 OTID; // 0x48
        enum Type teraType;
        u8 mintNature;
        u8 ivHp;
        u8 ivAtk;
        u8 ivDef;
        u8 ivSpatk;
        u8 ivSpdef;
        u8 ivSpeed;
        u8 evHp;
        u8 evAtk;
        u8 evDef;
        u8 evSpatk;
        u8 evSpdef;
        u8 evSpeed; // 0x56
    } summary;
    u16 bg3TilemapBuffers[PSS_BUFFER_SIZE];
    u16 bg2TilemapBuffers[PSS_PAGE_COUNT][PSS_BUFFER_SIZE];
    u16 bg1TilemapBuffers[PSS_EFFECT_COUNT][PSS_BUFFER_SIZE];
    u8 mode;
    bool8 isBoxMon;
    u8 curMonIndex;
    u8 maxMonIndex;
    u8 currPageIndex;
    u8 minPageIndex;
    u8 maxPageIndex;
    bool8 lockMonFlag; // This is used to prevent the player from changing Pokémon in the move deleter select, etc, but it is not needed because the input is handled differently there
    u16 newMove;
    u8 firstMoveIndex;
    u8 secondMoveIndex;
    bool8 lockMovesFlag; // This is used to prevent the player from changing position of moves in a battle or when trading.
    u8 hasRelearnableMoves;
    u8 windowIds[2];
    u8 spriteIds[SPRITE_ARR_ID_COUNT];
    s16 switchCounter; // Used for various switch statement cases that decompress/load graphics or Pokémon data
    u16 monAnimTimer; // tracks time between re-playing mon anims
    u8 monAnimPlayed; // tracks if anim has been played at least once
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    struct ConditionGraph conditionGraph;
    struct Sprite *conditionSparkles[MAX_CONDITION_SPARKLES];
#endif
} *sMonSummaryScreen = NULL;

static EWRAM_DATA u8 sMoveSlotToReplace = 0;
static EWRAM_DATA u8 sHeldSlot = 0;
static EWRAM_DATA u32 sHeldAnimId = 0;
ALIGNED(4) static EWRAM_DATA u8 sAnimDelayTaskId = 0;
static EWRAM_DATA u8 sStringVar5[8] = {0};

// forward declarations
static bool8 LoadGraphics(void);
static void CB2_InitSummaryScreen(void);
static void InitBGs(void);
static bool8 DecompressGraphics(void);
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
static void PatchPageIndicatorIcons(void);
#endif
static void CopyMonToSummaryStruct(struct Pokemon *);
static bool8 ExtractMonDataToSummaryStruct(struct Pokemon *);
static void SetSelectMoveTilemaps(void);
static void CloseSummaryScreen(u8);
static void Task_HandleInput(u8);
static void ChangeSummaryPokemon(u8, s8);
static void Task_ChangeSummaryMon(u8);
static s8 AdvanceMonIndex(s8);
static s8 AdvanceMultiBattleMonIndex(s8);
static bool8 IsValidToViewInMulti(struct Pokemon *);
static void ChangePage(u8, s8);
static void PssScroll(u8);
static void PssScrollEnd(u8);
static void TryDrawExperienceProgressBar(void);
static void SwitchToMoveSelection(u8);
static void Task_HandleInput_MoveSelect(u8);
static bool8 HasMoreThanOneMove(void);
static void ChangeSelectedMove(s16 *, s8, u8 *);
static void CloseMoveSelectMode(u8);
static void SwitchToMovePositionSwitchMode(u8);
static void Task_HandleInput_MoveSwitch(u8);
static void ExitMovePositionSwitchMode(u8, bool8);
static void LiftMoveSlot(u8);
static void DropMoveSlot(void);
static void AnimateLiftedSlotToTarget(void);
static void SwapMonMoves(struct Pokemon *, u8, u8);
static void SwapBoxMonMoves(struct BoxPokemon *, u8, u8);
static void Task_SetHandleReplaceMoveInput(u8);
static void Task_HandleReplaceMoveInput(u8);
static bool8 CanReplaceMove(void);
static void ShowCantForgetHMsWindow(u8);
static void Task_HandleInputCantForgetHMsMoves(u8);
static void DrawExperienceProgressBar(struct Pokemon *);
static void ResetWindows(void);
static void PrintMonPortraitInfo(void);
static void PrintNotEggInfo(void);
static void PrintEggInfo(void);
static void PrintEggStepsRemaining(void);
static void LoadGenderGfx(void);
static void CreateGenderSprite(struct Pokemon *, enum Species);
static void CreateGigantamaxSprite(void);
static void PrintButtonIcon(u8, u8, u32, u32);
static u8 GetButtonTextOffset(u8 buttonType);
static void PrintTextOnWindowWithFont(u8, const u8 *, u8, u8, u8, u8, u32);
static void PrintMovesPagePrompt(void);
static void RefreshRelearnModePrompt(void);
static void ClearMovesPagePrompt(void);
static void PrintPagePrompts(void);
static void PutPageWindowTilemaps(u8);
static void ClearPageWindowTilemaps(u8);
static void RemoveWindowByIndex(u8);
static void PrintPageSpecificText(u8);
static void CreateTextPrinterTask(u8);
static void PrintInfoPageText(void);
static void Task_PrintInfoPage(u8);
static void PrintMonOTName(void);
static void PrintMonOTID(void);
static void PrintMonNature(void);
static void PrintMonDexNumberSpecies(void);
static void PrintMonAbilityName(void);
static void PrintMonAbilityDescription(void);
static void BufferMonTrainerMemo(void);
static void BufferNatureString(u8);
static void GetMetLevelString(u8 *);
static bool8 DoesMonOTMatchOwner(void);
static bool8 DidMonComeFromGBAGames(void);
static bool8 IsInGamePartnerMon(void);
static void PrintEggState(void);
static void Task_PrintSkillsPage(u8);
static void PrintHeldItemInfo(void);
static void PrintSkillsPageText(void);
static void PrintStatLabels(void);
static void PrintExpPointsNextLevel(void);
static void PrintBattleMoves(void);
static void Task_PrintBattleMoves(u8);
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
static void PrintConditionsPageText(void);
static void Task_PrintConditionsPage(u8);
static void CreateConditionsPageCategoryIcons(void);
static void DestroyContestCategoryIcons(void);
static void CreateSheenSparkleSprites(void);
static void DestroySheenSparkleSprites(void);
static void SpriteCB_SheenSparkle(struct Sprite *);
static void SpriteCB_MaxCondSparkle(struct Sprite *);
static void CreateMaxConditionSparkles(void);
static void DestroyMaxConditionSparkles(void);
#endif
static void PrintContestMovesPageText(void);
static void Task_PrintContestMoves(u8);
static void PrintMemoPageText(void);
static void Task_PrintMemoPage(u8);
static void PrintMoveNameAndPP(u8);
static void HandleAppealJamTilemap(enum Move);
static void PrintMoveDescription(enum Move);
static void PrintNewMoveDetailsOrCancelText(void);
static void SwapMovesNamesPP(u8, u8);
static void PrintHMMovesCantBeForgotten(void);
static void ResetSpriteIds(void);
static void SetSpriteInvisibility(u8, bool8);
static void HidePageSpecificSprites(void);
static void SetTypeIcons(void);
static void SetTypeSpritePosAndPal(u8, u8, u8, u8);
static void CreateMoveTypeIcons(void);
static void SetMonTypeIcons(void);
static void SetMoveTypeIcons(void);
static void SetContestMoveTypeIcons(void);
static void SetNewMoveTypeIcon(void);
static void SwapMovesTypeSprites(u8, u8);
static u8 LoadMonGfxAndSprite(struct Pokemon *, s16 *, bool32);
static u8 CreateMonSprite(struct Pokemon *, bool32);
static void SpriteCB_Pokemon(struct Sprite *);
static void StopPokemonAnimations(void);
static void CreateMonMarkingsSprite(struct Pokemon *);
static void UpdateMonMarkingsSprite(struct Pokemon *);
static void CreateCaughtBallSprite(struct Pokemon *);
static void RefreshInfoPageItemTilemap(void);
static void CreateDynamaxLevelSprites(void);
static void DestroyDynamaxLevelSprites(void);
static void CreateHeldItemSprite(void);
static void DestroyHeldItemIconSprite(void);
static void CreateStatusSprite(void);
static void CreateMoveCursorSprite(void);
static void DestroyMoveCursorSprite(void);
static void SpriteCB_MoveCursor(struct Sprite *);
static void CreateMoveSlotSprites(void);
static void DestroyMoveSlotSprites(void);
static void UpdateMoveSlotVisibility(void);
static void UpdateMoveSlotPalette(void);
static void SummaryScreen_DestroyAnimDelayTask(void);
static void SetShinySprite(void);
static void SetPokerusCuredSprite(void);
static void HandleStatusSprite(struct Pokemon *);
static u8 AddWindowFromTemplateList(const struct WindowTemplate*, u8);
static void ClearCancelText(void);
static bool32 ShouldRemoveHyphen(const u8*, const u8*, const u8*);
static u8 FormatTextByWidth(u8*, s32, u8, const u8*, s16);
static void Task_ShowEffectTilemap(u8);
static void Task_HideEffectTilemap(u8);
static void ShowCategoryIcon(enum Move);
static void DestroyCategoryIcon(void);
static void ChangeSummaryState(s16*, u8);
static void DrawNextSkillsButtonPrompt(u8);
static void PrintStats(u8 mode);
static void SetFriendshipSprite(void);
static void TrySetInfoPageIcons(void);
static void RunMonAnimTimer(void);
static bool32 ShouldShowMoveRelearner(void);
static bool32 ShouldShowRename(void);
static void ShowCancelOrRenamePrompt(void);
static void CB2_ReturnToSummaryScreenFromNamingScreen(void);
static void CB2_PssChangePokemonNickname(void);
static void UpdateMoveRelearnerState(bool32 goingDown);
static void PrintRightAlignedPrompt(u8, u8, const u8*, int, u8);

// const rom data

static const u8 sMemoNatureTextColor[]          = _("{COLOR LIGHT_BLUE}{SHADOW DYNAMIC_COLOR1}");
static const u8 sMemoMiscTextColor[]            = _("{COLOR WHITE}{SHADOW DARK_GRAY}");
static const u8 sStatsHPLayout[]                = _("{DYNAMIC 0}/{DYNAMIC 1}");
static const u8 sStatsHPIVEVLayout[]            = _("{DYNAMIC 0}");
static const u8 sMovesPPLayout[]                = _("{DYNAMIC 0}/{DYNAMIC 1}");
static const u8 sEggStepsLayout[]               = _("{DYNAMIC 0} steps");

static const u8 sText_Empty[]                   = _("");
static const u8 sText_Cancel[]                  = _("Cancel");
static const u8 sText_Switch[]                  = _("Switch");
static const u8 sText_Rename[]                  = _("Rename");
static const u8 sText_Lv[]                      = _("Lv.");
static const u8 sText_HP_Title[]                = _("HP");
static const u8 sText_Attack_Title[]            = _("Attack");
static const u8 sText_Defense_Title[]           = _("Defense");
static const u8 sText_SpAtk_Title[]             = _("Sp. Atk");
static const u8 sText_SpDef_Title[]             = _("Sp. Def");
static const u8 sText_Speed_Title[]             = _("Speed");
static const u8 sText_ViewIVs[]                 = _("View IV");
static const u8 sText_ViewEVs[]                 = _("View EV");
static const u8 sText_ViewStats[]               = _("View Stats");
static const u8 sText_Exp[]                     = _("Exp.");
static const u8 sText_NextLv[]                  = _("Next Lv.");
static const u8 sText_RentalPkmn[]              = _("Rental Pokémon");
static const u8 sText_None[]                    = _("None");
static const u8 sText_Egg[]                     = _("Egg");
static const u8 sText_Nature[]                  = _("{DYNAMIC 0}{DYNAMIC 2}{DYNAMIC 1}{DYNAMIC 5}");
static const u8 sText_MintNature[]              = _("{DYNAMIC 0}{DYNAMIC 2} {EMOJI_LEAF}{DYNAMIC 1}");

// Relearn prompt texts
static const u8 sText_RelearnModeLevel[]         = _("Level");
static const u8 sText_RelearnModeTM[]            = _("TM");
static const u8 sText_RelearnModeTutor[]         = _("Tutor");
static const u8 sText_Relearn[]                  = _("Relearn ");
static const u8 sText_Info[]                     = _("Info");

static const u8 *const sRelearnModeNames[] = {
    [MOVE_RELEARNER_LEVEL_UP_MOVES] = sText_RelearnModeLevel,
    [MOVE_RELEARNER_EGG_MOVES]      = sText_Egg,
    [MOVE_RELEARNER_TM_MOVES]       = sText_RelearnModeTM,
    [MOVE_RELEARNER_TUTOR_MOVES]    = sText_RelearnModeTutor,
};

// Trainer Memo page texts
static const u8 sText_MemoNature[]              = _("{DYNAMIC 0}{DYNAMIC 2}{DYNAMIC 1}{DYNAMIC 5} by nature");
static const u8 sText_MemoMet[]                 = _("Met when it was {LV_2}. {DYNAMIC 1}{DYNAMIC 3}{DYNAMIC 1}\nLocation: {DYNAMIC 0}{DYNAMIC 4}{DYNAMIC 1}");
static const u8 sText_MemoHatched[]             = _("Hatched from an egg\nLocation: {DYNAMIC 0}{DYNAMIC 4}{DYNAMIC 1}");
static const u8 sText_MemoTraded[]              = _("Met in a trade");
static const u8 sText_MemoFateful[]             = _("Met in a fateful encounter\nat {LV_2}. {DYNAMIC 1}{DYNAMIC 3}{DYNAMIC 1}");
static const u8 sText_MemoProbablyMet[]         = _("Seems to have met at {LV_2}. {DYNAMIC 1}{DYNAMIC 3}{DYNAMIC 1}\nLocation: {DYNAMIC 0}{DYNAMIC 4}{DYNAMIC 1}");
static const u8 sText_MemoMetSomewhere[]        = _("Met somewhere at {LV_2}. {DYNAMIC 1}{DYNAMIC 3}{DYNAMIC 1}");
static const u8 sText_MemoHatchedSomewhere[]    = _("Hatched from an egg\nsomewhere.");

// Characteristics
static const u8 sCharacteristic_HP_0[]          = _("Loves to eat");
static const u8 sCharacteristic_HP_1[]          = _("Takes plenty of siestas");
static const u8 sCharacteristic_HP_2[]          = _("Nods off a lot");
static const u8 sCharacteristic_HP_3[]          = _("Scatters things often");
static const u8 sCharacteristic_HP_4[]          = _("Likes to relax");

static const u8 sCharacteristic_Atk_0[]         = _("Proud of its power");
static const u8 sCharacteristic_Atk_1[]         = _("Likes to thrash about");
static const u8 sCharacteristic_Atk_2[]         = _("A little quick tempered");
static const u8 sCharacteristic_Atk_3[]         = _("Likes to fight");
static const u8 sCharacteristic_Atk_4[]         = _("Quick tempered");

static const u8 sCharacteristic_Def_0[]         = _("Sturdy body");
static const u8 sCharacteristic_Def_1[]         = _("Capable of taking hits");
static const u8 sCharacteristic_Def_2[]         = _("Highly persistent");
static const u8 sCharacteristic_Def_3[]         = _("Good endurance");
static const u8 sCharacteristic_Def_4[]         = _("Good perseverance");

static const u8 sCharacteristic_Speed_0[]       = _("Likes to run");
static const u8 sCharacteristic_Speed_1[]       = _("Alert to sounds");
static const u8 sCharacteristic_Speed_2[]       = _("Impetuous and silly");
static const u8 sCharacteristic_Speed_3[]       = _("Somewhat of a clown");
static const u8 sCharacteristic_Speed_4[]       = _("Quick to flee");

static const u8 sCharacteristic_SpAtk_0[]       = _("Highly curious");
static const u8 sCharacteristic_SpAtk_1[]       = _("Mischievous");
static const u8 sCharacteristic_SpAtk_2[]       = _("Thoroughly cunning");
static const u8 sCharacteristic_SpAtk_3[]       = _("Often lost in thought");
static const u8 sCharacteristic_SpAtk_4[]       = _("Very finicky");

static const u8 sCharacteristic_SpDef_0[]       = _("Strong willed");
static const u8 sCharacteristic_SpDef_1[]       = _("Somewhat vain");
static const u8 sCharacteristic_SpDef_2[]       = _("Strongly defiant");
static const u8 sCharacteristic_SpDef_3[]       = _("Hates to lose");
static const u8 sCharacteristic_SpDef_4[]       = _("Somewhat stubborn");

static const u8 *const sMonCharacteristicTable[MON_CHARACTERISTIC_COUNT] = {
    // HP
    sCharacteristic_HP_0,
    sCharacteristic_HP_1,
    sCharacteristic_HP_2,
    sCharacteristic_HP_3,
    sCharacteristic_HP_4,
    // Attack
    sCharacteristic_Atk_0,
    sCharacteristic_Atk_1,
    sCharacteristic_Atk_2,
    sCharacteristic_Atk_3,
    sCharacteristic_Atk_4,
    // Defense
    sCharacteristic_Def_0,
    sCharacteristic_Def_1,
    sCharacteristic_Def_2,
    sCharacteristic_Def_3,
    sCharacteristic_Def_4,
    // Speed
    sCharacteristic_Speed_0,
    sCharacteristic_Speed_1,
    sCharacteristic_Speed_2,
    sCharacteristic_Speed_3,
    sCharacteristic_Speed_4,
    // Sp. Atk
    sCharacteristic_SpAtk_0,
    sCharacteristic_SpAtk_1,
    sCharacteristic_SpAtk_2,
    sCharacteristic_SpAtk_3,
    sCharacteristic_SpAtk_4,
    // Sp. Def
    sCharacteristic_SpDef_0,
    sCharacteristic_SpDef_1,
    sCharacteristic_SpDef_2,
    sCharacteristic_SpDef_3,
    sCharacteristic_SpDef_4,
};

// bg gfx
const u32 sSummaryScreen_Gfx[]                      = INCGFX_U32("graphics/summary_screen/swsh/tiles.png", ".4bpp.smol");
const u16 sSummaryScreen_Pal[]                      = INCGFX_U16("graphics/summary_screen/swsh/tiles.png", ".gbapal");
const u32 sSummaryPage_ScrollBG_Tilemap[]           = INCGFX_U32("graphics/summary_screen/swsh/scroll_bg.bin", ".smolTM");
const u32 sSummaryPage_Info_Tilemap[]               = INCGFX_U32("graphics/summary_screen/swsh/page_info.bin", ".smolTM");
const u32 sSummaryPage_Info_Item_Tilemap[]          = INCGFX_U32("graphics/summary_screen/swsh/page_info_item.bin", ".smolTM");
const u32 sSummaryPage_Skills_Tilemap[]             = INCGFX_U32("graphics/summary_screen/swsh/page_skills.bin", ".smolTM");
const u32 sSummaryPage_Skills_Dynamax_Tilemap[]     = INCGFX_U32("graphics/summary_screen/swsh/page_skills_dynamax.bin", ".smolTM");
const u32 sSummaryPage_BattleMoves_Tilemap[]        = INCGFX_U32("graphics/summary_screen/swsh/page_battle_moves.bin", ".smolTM");
const u32 sSummaryEffect_Battle_Tilemap[]           = INCGFX_U32("graphics/summary_screen/swsh/effect_battle.bin", ".smolTM");
const u32 sSummaryPage_ContestMoves_Tilemap[]       = INCGFX_U32("graphics/summary_screen/swsh/page_contest_moves.bin", ".smolTM");
const u32 sSummaryEffect_Contest_Tilemap[]          = INCGFX_U32("graphics/summary_screen/swsh/effect_contest.bin", ".smolTM");
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
const u32 sSummaryPage_Conditions_Tilemap[]         = INCGFX_U32("graphics/summary_screen/swsh/page_conditions.bin", ".smolTM");
#endif
const u32 sSummaryPage_Memo_Tilemap[]               = INCGFX_U32("graphics/summary_screen/swsh/page_memo.bin", ".smolTM");

// sprite gfx
static const u8 sButtons_Gfx[][4 * TILE_SIZE_4BPP] = {
    INCGFX_U8("graphics/summary_screen/swsh/button_a.png", ".4bpp"),
    INCGFX_U8("graphics/summary_screen/swsh/button_b.png", ".4bpp"),
    INCGFX_U8("graphics/summary_screen/swsh/button_start.png", ".4bpp"),
    INCGFX_U8("graphics/summary_screen/swsh/button_lr.png", ".4bpp"),
};
static const u32 sTeraTypes_Gfx[]                   = INCGFX_U32("graphics/summary_screen/swsh/tera_types.png", ".4bpp.smol");
// Share sDynamaxLevels_Pal
static const u32 sDynamaxLevels_Gfx[]               = INCGFX_U32("graphics/summary_screen/swsh/dynamax_levels.png", ".4bpp.smol");
static const u32 sMoveCursor_Gfx[]                  = INCGFX_U32("graphics/summary_screen/swsh/move_cursor.png", ".4bpp.smol");
static const u32 sMoveSlot_Gfx[]                    = INCGFX_U32("graphics/summary_screen/swsh/move_slot.png", ".4bpp.smol");
// Share sCategoryIcons_Pal
static const u32 sCategoryIcons_Gfx[]               = INCGFX_U32("graphics/summary_screen/swsh/category_icons.png", ".4bpp.smol");
static const u32 sPokerusCuredIcon_Gfx[]            = INCGFX_U32("graphics/summary_screen/swsh/pokerus_cured_icon.png", ".4bpp.smol");
static const u32 sShinyIcon_Gfx[]                   = INCGFX_U32("graphics/summary_screen/swsh/shiny_icon.png", ".4bpp.smol");
// Share sGenderIcons_Pal
static const u32 sGenderIcons_Gfx[]                 = INCGFX_U32("graphics/summary_screen/swsh/gender_icons.png", ".4bpp.smol");
static const u32 sFriendshipIcon_Gfx[]              = INCGFX_U32("graphics/summary_screen/swsh/heart.png", ".4bpp.smol");
static const u32 sGigantamaxIcon_Gfx[]              = INCGFX_U32("graphics/summary_screen/swsh/gigantamax.png", ".4bpp.smol");
#if SWSH_SUMMARY_SWSH_STATUS_ICONS == TRUE
static const u32 sStatusGfx_Icons[]                 = INCGFX_U32("graphics/summary_screen/swsh/status_icons.png", ".4bpp.smol");
static const u16 sStatusPal_Icons[]                 = INCGFX_U16("graphics/summary_screen/swsh/status_icons.png", ".gbapal");
#endif

static const u16 sCategoryIcons_Pal[]               = INCGFX_U16("graphics/summary_screen/swsh/category_icons.png", ".gbapal");
static const u16 sGenderIcons_Pal[]                 = INCGFX_U16("graphics/summary_screen/swsh/gender_icons.png", ".gbapal");
static const u16 sDynamaxLevels_Pal[]               = INCGFX_U16("graphics/summary_screen/swsh/dynamax_levels.png", ".gbapal");
static const u16 sMoveSlot_Pals[]                   = INCGFX_U16("graphics/summary_screen/swsh/move_slot.png", ".gbapal");
static const u16 sMonShadow_Pal[]                   = INCGFX_U16("graphics/summary_screen/swsh/shadow.pal", ".gbapal");
#if SWSH_SUMMARY_SWSH_TYPE_ICONS == TRUE
    static const u32 sMoveTypes_Gfx[] = INCGFX_U32("graphics/summary_screen/swsh/move_types.png", ".4bpp.smol");
    #if SWSH_SUMMARY_SWSH_TYPE_ICONS_SV_PAL == TRUE
        static const u16 sMoveTypes_Pal[] = INCGFX_U16("graphics/summary_screen/swsh/tera_types.png", ".gbapal");
    #else
        static const u16 sMoveTypes_Pal[] = INCGFX_U16("graphics/summary_screen/swsh/move_types.png", ".gbapal");
    #endif
#endif


static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 2,
        .mapBaseIndex = 29,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 30,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0,
    },
    {
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0,
    },
};

static const s8 sMultiBattleOrder[] = {0, 2, 3, 1, 4, 5};
static const struct WindowTemplate sSummaryTemplate[] =
{
    [PSS_LABEL_WINDOW_PROMPT_CANCEL] = {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 18,
        .width = 9,
        .height = 2,
        .paletteNum = 2,
        .baseBlock = 1,
    },
    [PSS_LABEL_WINDOW_PROMPT_SWITCH] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 13,
        .width = 8,
        .height = 2,
        .paletteNum = 2,
        .baseBlock = 19,
    },
    [PSS_LABEL_WINDOW_MOVES_POWER_ACC] = {
        .bg = 0,
        .tilemapLeft = 4,
        .tilemapTop = 15,
        .width = 6,
        .height = 5,
        .paletteNum = 2,
        .baseBlock = 35,
    },
    [PSS_LABEL_WINDOW_PORTRAIT_INFO] = {
        .bg = 0,
        .tilemapLeft = 15,
        .tilemapTop = 1,
        .width = 13,
        .height = 2,
        .paletteNum = 2,
        .baseBlock = 65,
    },
    [PSS_LABEL_WINDOW_PROMPT_IV_EV_STATS] = {
        .bg = 0,
        .tilemapLeft = 20,
        .tilemapTop = 18,
        .width = 10,
        .height = 2,
        .paletteNum = 2,
        .baseBlock = 91,
    },
    [PSS_LABEL_WINDOW_PROMPT_MOVES] = {
        .bg = 0,
        .tilemapLeft = 10,
        .tilemapTop = 18,
        .width = 20,
        .height = 2,
        .paletteNum = 2,
        .baseBlock = 111,
    },
    [PSS_LABEL_WINDOW_END] = DUMMY_WIN_TEMPLATE
};
static const struct WindowTemplate sPageInfoTemplate[] =
{
    [PSS_DATA_WINDOW_INFO_ITEM] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 12,
        .width = 18,
        .height = 7,
        .paletteNum = 2,
        .baseBlock = 151,
    },
    [PSS_DATA_WINDOW_INFO_SPECIES] = {
        .bg = 0,
        .tilemapLeft = 7,
        .tilemapTop = 3,
        .width = 12,
        .height = 9,
        .paletteNum = 2,
        .baseBlock = 277,
    },
};
static const struct WindowTemplate sPageSkillsTemplate[] =
{
    [PSS_DATA_WINDOW_SKILLS_STATS] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 4,
        .width = 18,
        .height = 7,
        .paletteNum = 2,
        .baseBlock = 151,
    },
    [PSS_DATA_WINDOW_SKILLS_ABILITY] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = SWSH_SUMMARY_SHOW_DYNAMAX_LEVEL ? 13 : 11,
        .width = 18,
        .height = 5,
        .paletteNum = 2,
        .baseBlock = 277,
    },
};
static const struct WindowTemplate sPageMovesTemplate[] = // This is used for both battle moves
{
    [PSS_DATA_WINDOW_MOVE_DESCRIPTION] = {
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 15,
        .width = 18,
        .height = 5,
        .paletteNum = 2,
        .baseBlock = 151,
    },
};
static const struct WindowTemplate sPageMemoTemplate[] =
{
    [PSS_DATA_WINDOW_MEMO_NOTE] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 4,
        .width = 18,
        .height = 10,
        .paletteNum = 2,
        .baseBlock = 151,
    },
    [PSS_DATA_WINDOW_MEMO_EXP] = {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 13,
        .width = 14,
        .height = 4,
        .paletteNum = 2,
        .baseBlock = 331,
    },
};
// {bg, fg, shadow} entries indexing into BG palette 2
static const u8 sTextColors[][3] =
{
    {0, 1, 2},   // [0] main: most page texts (species, OT, nature, ability, stats, etc.)
    {0, 3, 4},   // [1] portrait text (nickname/level) + prompt texts
    {0, 5, 6},   // [2] shiny dex number + "HM can't be forgotten" warning
};

static void (*const sTextPrinterFunctions[])(void) =
{
    [PSS_PAGE_INFO] = PrintInfoPageText,
    [PSS_PAGE_SKILLS] = PrintSkillsPageText,
    [PSS_PAGE_BATTLE_MOVES] = PrintBattleMoves,
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    [PSS_PAGE_CONDITIONS] = PrintConditionsPageText,
#endif
    [PSS_PAGE_CONTEST_MOVES] = PrintContestMovesPageText,
    [PSS_PAGE_MEMO] = PrintMemoPageText,
};

static void (*const sTextPrinterTasks[])(u8 taskId) =
{
    [PSS_PAGE_INFO] = Task_PrintInfoPage,
    [PSS_PAGE_SKILLS] = Task_PrintSkillsPage,
    [PSS_PAGE_BATTLE_MOVES] = Task_PrintBattleMoves,
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    [PSS_PAGE_CONDITIONS] = Task_PrintConditionsPage,
#endif
    [PSS_PAGE_CONTEST_MOVES] = Task_PrintContestMoves,
    [PSS_PAGE_MEMO] = Task_PrintMemoPage,
};

#define TAG_MON_STATUS          30001
#define TAG_MOVE_TYPES          30002
#define TAG_MON_MARKINGS        30003
#define TAG_SHINY_ICON          30004
#define TAG_POKERUS_CURED_ICON  30005
#define TAG_CATEGORY_ICONS      30006
#define TAG_STAT_GRADES         30007
#define TAG_FRIENDSHIP_ICON     30008
#define TAG_GIGANTAMAX_ICON     30009
#define TAG_TERA_TYPE           30010
#define TAG_MON_SHADOW          30011
#define TAG_INFO_PROMPT         30014
#define TAG_GENDER_ICON         30015
#define TAG_HELD_ITEM_ICON      30017
#define TAG_LR_BUTTON           30018
#define TAG_DYNAMAX_LEVELS      30021
#define TAG_MOVE_SLOT_MAIN      30022
#define TAG_MOVE_SLOT_FOCUS     30023
#define TAG_MOVE_SLOT_0         30024
#define TAG_MOVE_SLOT_1         30025
#define TAG_MOVE_SLOT_2         30026
#define TAG_MOVE_SLOT_3         30027
#define TAG_MOVE_SLOT_4         30028
#define TAG_MOVE_CURSOR         30029


enum SwShCategoryIcon
{
    CATEGORY_ICON_PHYSICAL,
    CATEGORY_ICON_SPECIAL,
    CATEGORY_ICON_STATUS,
};

// ===============================================
// SWSH Custom Category Icons (local to summary screen only)
// ===============================================

static const struct OamData sOamData_CategoryIcons_SwSh =
{
    .size = SPRITE_SIZE(32x16),
    .shape = SPRITE_SHAPE(32x16),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_CategoryIcons =
{
    .data = sCategoryIcons_Gfx,
    .size = 32*16*3/2,
    .tag = TAG_CATEGORY_ICONS,
};

static const struct SpritePalette sSpritePal_CategoryIcons =
{
    .data = sCategoryIcons_Pal,
    .tag = TAG_CATEGORY_ICONS
};

static const union AnimCmd sSpriteAnim_CategoryPhysical_SwSh[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_CategorySpecial_SwSh[] =
{
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_CategoryStatus_SwSh[] =
{
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_CategoryIcons_SwSh[] =
{
    [CATEGORY_ICON_PHYSICAL] = sSpriteAnim_CategoryPhysical_SwSh,
    [CATEGORY_ICON_SPECIAL]  = sSpriteAnim_CategorySpecial_SwSh,
    [CATEGORY_ICON_STATUS]   = sSpriteAnim_CategoryStatus_SwSh,
};

static const struct SpriteTemplate sSpriteTemplate_CategoryIcons =
{
    .tileTag = TAG_CATEGORY_ICONS,
    .paletteTag = TAG_CATEGORY_ICONS,
    .oam = &sOamData_CategoryIcons_SwSh,
    .anims = sSpriteAnimTable_CategoryIcons_SwSh,
};

enum FriendshipLevels
{
    FRIENDSHIP_LEVEL_0,
    FRIENDSHIP_LEVEL_1,
    FRIENDSHIP_LEVEL_2,
    FRIENDSHIP_LEVEL_3,
    FRIENDSHIP_LEVEL_4,
    FRIENDSHIP_LEVEL_5,
    FRIENDSHIP_LEVEL_MAX,
    FRIENDSHIP_LEVEL_COUNT,
};

// edit these to change what friendship value the heart icon changes at
static const u8 sFriendshipLevelToThreshold[FRIENDSHIP_LEVEL_COUNT] =
{
    [FRIENDSHIP_LEVEL_0]    = 0,
    [FRIENDSHIP_LEVEL_1]    = 44,
    [FRIENDSHIP_LEVEL_2]    = 88,
    [FRIENDSHIP_LEVEL_3]    = 132,
    [FRIENDSHIP_LEVEL_4]    = 176,
    [FRIENDSHIP_LEVEL_5]    = 220,
    [FRIENDSHIP_LEVEL_MAX]  = MAX_FRIENDSHIP,
};

static const struct OamData sOamData_FriendshipIcon =
{
    .size = SPRITE_SIZE(8x8),
    .shape = SPRITE_SHAPE(8x8),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_FriendshipIcon =
{
    .data = sFriendshipIcon_Gfx,
    .size = FRIENDSHIP_LEVEL_COUNT * (8 * 8),
    .tag = TAG_FRIENDSHIP_ICON,
};

static const union AnimCmd sSpriteAnim_FriendshipIcon0[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_FriendshipIcon1[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_1, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_FriendshipIcon2[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_2, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_FriendshipIcon3[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_3, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_FriendshipIcon4[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_FriendshipIcon5[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_5, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_FriendshipIconMax[] = {
    ANIMCMD_FRAME(FRIENDSHIP_LEVEL_MAX, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_FriendshipIcon[FRIENDSHIP_LEVEL_COUNT] = {
    [FRIENDSHIP_LEVEL_0]    = sSpriteAnim_FriendshipIcon0,
    [FRIENDSHIP_LEVEL_1]    = sSpriteAnim_FriendshipIcon1,
    [FRIENDSHIP_LEVEL_2]    = sSpriteAnim_FriendshipIcon2,
    [FRIENDSHIP_LEVEL_3]    = sSpriteAnim_FriendshipIcon3,
    [FRIENDSHIP_LEVEL_4]    = sSpriteAnim_FriendshipIcon4,
    [FRIENDSHIP_LEVEL_5]    = sSpriteAnim_FriendshipIcon5,
    [FRIENDSHIP_LEVEL_MAX]  = sSpriteAnim_FriendshipIconMax,
};

static const struct SpriteTemplate sSpriteTemplate_FriendshipIcon =
{
    .tileTag = TAG_FRIENDSHIP_ICON,
    .paletteTag = TAG_GENDER_ICON,
    .oam = &sOamData_FriendshipIcon,
    .anims = sSpriteAnimTable_FriendshipIcon,
};

static const struct OamData sOamData_MoveTypes =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_TypeNone[] = {
    ANIMCMD_FRAME(TYPE_NONE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeStellar[] = {
    ANIMCMD_FRAME(TYPE_STELLAR * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCool[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_COOL + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryBeauty[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_BEAUTY + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCute[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_CUTE + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategorySmart[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_SMART + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryTough[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_TOUGH + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MoveTypes[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT] = {
    [TYPE_NONE] = sSpriteAnim_TypeNone,
    [TYPE_NORMAL] = sSpriteAnim_TypeNormal,
    [TYPE_FIGHTING] = sSpriteAnim_TypeFighting,
    [TYPE_FLYING] = sSpriteAnim_TypeFlying,
    [TYPE_POISON] = sSpriteAnim_TypePoison,
    [TYPE_GROUND] = sSpriteAnim_TypeGround,
    [TYPE_ROCK] = sSpriteAnim_TypeRock,
    [TYPE_BUG] = sSpriteAnim_TypeBug,
    [TYPE_GHOST] = sSpriteAnim_TypeGhost,
    [TYPE_STEEL] = sSpriteAnim_TypeSteel,
    [TYPE_MYSTERY] = sSpriteAnim_TypeMystery,
    [TYPE_FIRE] = sSpriteAnim_TypeFire,
    [TYPE_WATER] = sSpriteAnim_TypeWater,
    [TYPE_GRASS] = sSpriteAnim_TypeGrass,
    [TYPE_ELECTRIC] = sSpriteAnim_TypeElectric,
    [TYPE_PSYCHIC] = sSpriteAnim_TypePsychic,
    [TYPE_ICE] = sSpriteAnim_TypeIce,
    [TYPE_DRAGON] = sSpriteAnim_TypeDragon,
    [TYPE_DARK] = sSpriteAnim_TypeDark,
    [TYPE_FAIRY] = sSpriteAnim_TypeFairy,
    [TYPE_STELLAR] = sSpriteAnim_TypeStellar,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_COOL] = sSpriteAnim_CategoryCool,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_BEAUTY] = sSpriteAnim_CategoryBeauty,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_CUTE] = sSpriteAnim_CategoryCute,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_SMART] = sSpriteAnim_CategorySmart,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_TOUGH] = sSpriteAnim_CategoryTough,
};

static const struct CompressedSpriteSheet sSpriteSheet_MoveTypes =
{
#if SWSH_SUMMARY_SWSH_TYPE_ICONS == TRUE
    .data = sMoveTypes_Gfx,
#else
    .data = gMoveTypes_Gfx,
#endif
    .size = (NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT) * 0x100,
    .tag = TAG_MOVE_TYPES
};

static const struct SpriteTemplate sSpriteTemplate_MoveTypes =
{
    .tileTag = TAG_MOVE_TYPES,
    .paletteTag = TAG_MOVE_TYPES,
    .oam = &sOamData_MoveTypes,
    .anims = sSpriteAnimTable_MoveTypes,
};

static const union AnimCmd sSpriteAnim_TeraTypeNone[] = {
    ANIMCMD_FRAME(TYPE_NONE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeStellar[] = {
    ANIMCMD_FRAME(TYPE_STELLAR * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_TeraType[NUMBER_OF_MON_TYPES] = {
    [TYPE_NONE] = sSpriteAnim_TeraTypeNone,
    [TYPE_NORMAL] = sSpriteAnim_TeraTypeNormal,
    [TYPE_FIGHTING] = sSpriteAnim_TeraTypeFighting,
    [TYPE_FLYING] = sSpriteAnim_TeraTypeFlying,
    [TYPE_POISON] = sSpriteAnim_TeraTypePoison,
    [TYPE_GROUND] = sSpriteAnim_TeraTypeGround,
    [TYPE_ROCK] = sSpriteAnim_TeraTypeRock,
    [TYPE_BUG] = sSpriteAnim_TeraTypeBug,
    [TYPE_GHOST] = sSpriteAnim_TeraTypeGhost,
    [TYPE_STEEL] = sSpriteAnim_TeraTypeSteel,
    [TYPE_MYSTERY] = sSpriteAnim_TeraTypeMystery,
    [TYPE_FIRE] = sSpriteAnim_TeraTypeFire,
    [TYPE_WATER] = sSpriteAnim_TeraTypeWater,
    [TYPE_GRASS] = sSpriteAnim_TeraTypeGrass,
    [TYPE_ELECTRIC] = sSpriteAnim_TeraTypeElectric,
    [TYPE_PSYCHIC] = sSpriteAnim_TeraTypePsychic,
    [TYPE_ICE] = sSpriteAnim_TeraTypeIce,
    [TYPE_DRAGON] = sSpriteAnim_TeraTypeDragon,
    [TYPE_DARK] = sSpriteAnim_TeraTypeDark,
    [TYPE_FAIRY] = sSpriteAnim_TeraTypeFairy,
    [TYPE_STELLAR] = sSpriteAnim_TeraTypeStellar
};

static const struct OamData sOamData_TeraType =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_TeraType =
{
    .data = sTeraTypes_Gfx,
    .size = NUMBER_OF_MON_TYPES * (16 * 16),
    .tag = TAG_TERA_TYPE
};

static const struct SpriteTemplate sSpriteTemplate_TeraType =
{
    .tileTag = TAG_TERA_TYPE,
    .paletteTag = TAG_TERA_TYPE,
    .oam = &sOamData_TeraType,
    .anims = sSpriteAnimTable_TeraType,
};

static const struct OamData sOamData_MoveSlot =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_MoveSlot_0[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MoveSlot_1[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MoveSlot_2[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MoveSlot_3[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MoveSlot_4[] = {
    ANIMCMD_FRAME(32, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MoveSlot[] = {
    sSpriteAnim_MoveSlot_0,
    sSpriteAnim_MoveSlot_1,
    sSpriteAnim_MoveSlot_2,
    sSpriteAnim_MoveSlot_3,
    sSpriteAnim_MoveSlot_4,
};

static const u8 sMoveSlotAnims[MOVE_SLOT_SPRITES_COUNT] = {0, 1, 2, 3, 4};

#define MOVE_SLOT_GFX_SIZE ((32 * 16 * 5) / 2)

static const struct CompressedSpriteSheet sSpriteSheets_MoveSlot[MOVE_SLOT_COUNT] =
{
    { .data = sMoveSlot_Gfx, .size = MOVE_SLOT_GFX_SIZE, .tag = TAG_MOVE_SLOT_0 },
    { .data = sMoveSlot_Gfx, .size = MOVE_SLOT_GFX_SIZE, .tag = TAG_MOVE_SLOT_1 },
    { .data = sMoveSlot_Gfx, .size = MOVE_SLOT_GFX_SIZE, .tag = TAG_MOVE_SLOT_2 },
    { .data = sMoveSlot_Gfx, .size = MOVE_SLOT_GFX_SIZE, .tag = TAG_MOVE_SLOT_3 },
    { .data = sMoveSlot_Gfx, .size = MOVE_SLOT_GFX_SIZE, .tag = TAG_MOVE_SLOT_4 },
};

static const struct SpritePalette sSpritePal_MoveSlotMain  = {
    .data = sMoveSlot_Pals,
    .tag = TAG_MOVE_SLOT_MAIN
};

static const struct SpritePalette sSpritePal_MoveSlotFocus = {
    .data = &sMoveSlot_Pals[16],
    .tag = TAG_MOVE_SLOT_FOCUS
};

static const u16 sMoveSlotTileTags[MOVE_SLOT_COUNT] =
{
    TAG_MOVE_SLOT_0, TAG_MOVE_SLOT_1, TAG_MOVE_SLOT_2, TAG_MOVE_SLOT_3, TAG_MOVE_SLOT_4
};

static const struct SpriteTemplate sMoveSlotSpriteTemplates[MOVE_SLOT_COUNT] =
{
    { .tileTag = TAG_MOVE_SLOT_0, .paletteTag = TAG_MOVE_SLOT_MAIN, .oam = &sOamData_MoveSlot, .anims = sSpriteAnimTable_MoveSlot },
    { .tileTag = TAG_MOVE_SLOT_1, .paletteTag = TAG_MOVE_SLOT_MAIN, .oam = &sOamData_MoveSlot, .anims = sSpriteAnimTable_MoveSlot },
    { .tileTag = TAG_MOVE_SLOT_2, .paletteTag = TAG_MOVE_SLOT_MAIN, .oam = &sOamData_MoveSlot, .anims = sSpriteAnimTable_MoveSlot },
    { .tileTag = TAG_MOVE_SLOT_3, .paletteTag = TAG_MOVE_SLOT_MAIN, .oam = &sOamData_MoveSlot, .anims = sSpriteAnimTable_MoveSlot },
    { .tileTag = TAG_MOVE_SLOT_4, .paletteTag = TAG_MOVE_SLOT_MAIN, .oam = &sOamData_MoveSlot, .anims = sSpriteAnimTable_MoveSlot },
};

static const union TextColor sMoveSlotNameColor = { .background = 0, .foreground = 9, .shadow = 10 };
static const union TextColor sMoveSlotPPColors[4] =
{
    { .background = 0, .foreground = 1, .shadow = 2 }, // ppState 0: half (25–50%)
    { .background = 0, .foreground = 3, .shadow = 4 }, // ppState 1: low (≤25%)
    { .background = 0, .foreground = 5, .shadow = 6 }, // ppState 2: critical (0 PP)
    { .background = 0, .foreground = 7, .shadow = 8 }, // ppState 3: full (>50%)
};
#define MOVE_SLOT_NAME_FILL_COLOR 14
#define MOVE_SLOT_PP_FILL_COLOR   15

static const struct OamData sOamData_MoveCursor =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_MoveCursor =
{
    .data = sMoveCursor_Gfx,
    .size = (16 * 16 * 3) / 2,
    .tag = TAG_MOVE_CURSOR
};

static const union AnimCmd sAnim_MoveCursor[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_MoveCursor[] =
{
    sAnim_MoveCursor,
};

static const struct SpriteTemplate sSpriteTemplate_MoveCursor =
{
    .tileTag = TAG_MOVE_CURSOR,
    .paletteTag = TAG_DYNAMAX_LEVELS,
    .oam = &sOamData_MoveCursor,
    .anims = sAnims_MoveCursor,
};

// Shared palette for the move cursor/frame, ability box, and Dynamax box/level sprites
static const struct SpritePalette sSpritePal_DynamaxLevels =
{
    .data = sDynamaxLevels_Pal,
    .tag = TAG_DYNAMAX_LEVELS,
};


enum DynamaxLevels
{
    DYNAMAX_LEVEL_0,
    DYNAMAX_LEVEL_1,
    DYNAMAX_LEVEL_2,
    DYNAMAX_LEVEL_3,
    DYNAMAX_LEVEL_4,
    DYNAMAX_LEVEL_5,
    DYNAMAX_LEVEL_6,
    DYNAMAX_LEVEL_7,
    DYNAMAX_LEVEL_8,
    DYNAMAX_LEVEL_9,
    DYNAMAX_LEVEL_10,
    DYNAMAX_LEVEL_COUNT
};

static const struct OamData sOamData_DynamaxLevel =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(16x16),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(16x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_DynamaxLevels =
{
    .data = sDynamaxLevels_Gfx,
    .size = 11 * (16 * 16) / 2,
    .tag = TAG_DYNAMAX_LEVELS,
};

// Animation commands for each Dynamax level (0-10)
static const union AnimCmd sSpriteAnim_DynamaxLevel0[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel1[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel2[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel3[] = {
    ANIMCMD_FRAME(12, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel4[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel5[] = {
    ANIMCMD_FRAME(20, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel6[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel7[] = {
    ANIMCMD_FRAME(28, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel8[] = {
    ANIMCMD_FRAME(32, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel9[] = {
    ANIMCMD_FRAME(36, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_DynamaxLevel10[] = {
    ANIMCMD_FRAME(40, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_DynamaxLevels[] = {
    sSpriteAnim_DynamaxLevel0,
    sSpriteAnim_DynamaxLevel1,
    sSpriteAnim_DynamaxLevel2,
    sSpriteAnim_DynamaxLevel3,
    sSpriteAnim_DynamaxLevel4,
    sSpriteAnim_DynamaxLevel5,
    sSpriteAnim_DynamaxLevel6,
    sSpriteAnim_DynamaxLevel7,
    sSpriteAnim_DynamaxLevel8,
    sSpriteAnim_DynamaxLevel9,
    sSpriteAnim_DynamaxLevel10,
};

static const struct SpriteTemplate sSpriteTemplate_DynamaxLevel =
{
    .tileTag = TAG_DYNAMAX_LEVELS,
    .paletteTag = TAG_DYNAMAX_LEVELS,
    .oam = &sOamData_DynamaxLevel,
    .anims = sSpriteAnimTable_DynamaxLevels,
};

static const struct OamData sOamData_StatusCondition =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_StatusPoison[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusParalyzed[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusSleep[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFrozen[] = {
    ANIMCMD_FRAME(12, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusBurn[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusPokerus[] = {
    ANIMCMD_FRAME(20, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFaint[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFrostbite[] = {
    ANIMCMD_FRAME(28, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_StatusCondition[] = {
    sSpriteAnim_StatusPoison,
    sSpriteAnim_StatusParalyzed,
    sSpriteAnim_StatusSleep,
    sSpriteAnim_StatusFrozen,
    sSpriteAnim_StatusBurn,
    sSpriteAnim_StatusPokerus,
    sSpriteAnim_StatusFaint,
    sSpriteAnim_StatusFrostbite,
};

static const struct CompressedSpriteSheet sSpriteSheet_StatusIcons =
{
#if SWSH_SUMMARY_SWSH_STATUS_ICONS == TRUE
    .data = sStatusGfx_Icons,
#else
    .data = gStatusGfx_Icons,
#endif
    .size = 0x400,
    .tag = TAG_MON_STATUS
};

static const struct SpritePalette sSpritePal_StatusIcons =
{
#if SWSH_SUMMARY_SWSH_STATUS_ICONS == TRUE
    .data = sStatusPal_Icons,
#else
    .data = gStatusPal_Icons,
#endif
    .tag = TAG_MON_STATUS
};

static const struct SpriteTemplate sSpriteTemplate_StatusCondition =
{
    .tileTag = TAG_MON_STATUS,
    .paletteTag = TAG_MON_STATUS,
    .oam = &sOamData_StatusCondition,
    .anims = sSpriteAnimTable_StatusCondition,
};

static const struct OamData sOamData_GenderIcons =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_Female[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Male[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_GenderIcons[] = {
    sSpriteAnim_Female,
    sSpriteAnim_Male,
};

static const struct CompressedSpriteSheet sSpriteSheet_GenderIcons =
{
    .data = sGenderIcons_Gfx,
    .size = 0x200,
    .tag = TAG_GENDER_ICON
};

static const struct SpritePalette sSpritePal_GenderIcons =
{
    .data = sGenderIcons_Pal,
    .tag = TAG_GENDER_ICON
};

static const struct SpriteTemplate sSpriteTemplate_GenderIcons =
{
    .tileTag = TAG_GENDER_ICON,
    .paletteTag = TAG_GENDER_ICON,
    .oam = &sOamData_GenderIcons,
    .anims = sSpriteAnimTable_GenderIcons,
};

static const struct OamData sOamData_ShinyIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_ShinyIcon =
{
    .data = sShinyIcon_Gfx,
    .size = 8*8/2,
    .tag = TAG_SHINY_ICON
};

static const struct SpriteTemplate sSpriteTemplate_ShinyIcon =
{
    .tileTag = TAG_SHINY_ICON,
    .paletteTag = TAG_CATEGORY_ICONS,
    .oam = &sOamData_ShinyIcon,
};

static const struct OamData sOamData_GigantamaxIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_GigantamaxIcon =
{
    .data = sGigantamaxIcon_Gfx,
    .size = (16*16)/2,
    .tag = TAG_GIGANTAMAX_ICON
};

static const struct SpriteTemplate sSpriteTemplate_GigantamaxIcon =
{
    .tileTag = TAG_GIGANTAMAX_ICON,
    .paletteTag = TAG_GENDER_ICON,
    .oam = &sOamData_GigantamaxIcon,
};

static const struct OamData sOamData_PokerusCuredIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_PokerusCuredIcon =
{
    .data = sPokerusCuredIcon_Gfx,
    .size = 8*8,
    .tag = TAG_POKERUS_CURED_ICON
};

static const struct SpriteTemplate sSpriteTemplate_PokerusCuredIcon =
{
    .tileTag = TAG_POKERUS_CURED_ICON,
    .paletteTag = TAG_CATEGORY_ICONS,
    .oam = &sOamData_PokerusCuredIcon,
};

#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
static const struct OamData sOam_SheenSparkle =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 0,
};

static const union AnimCmd sAnim_SheenSparkle[] =
{
    ANIMCMD_FRAME( 0, 5),
    ANIMCMD_FRAME( 4, 5),
    ANIMCMD_FRAME( 8, 5),
    ANIMCMD_FRAME(12, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(20, 5),
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_SheenSparkle[] =
{
    sAnim_SheenSparkle,
};

static const struct SpriteTemplate sSpriteTemplate_SheenSparkle =
{
    .tileTag    = TAG_CONDITION_SPARKLE,
    .paletteTag = TAG_GENDER_ICON,
    .oam        = &sOam_SheenSparkle,
    .anims      = sAnims_SheenSparkle,
    .callback   = SpriteCB_SheenSparkle,
};

static const struct SpriteTemplate sSpriteTemplate_MaxCondSparkle =
{
    .tileTag    = TAG_CONDITION_SPARKLE,
    .paletteTag = TAG_GENDER_ICON,
    .oam        = &sOam_SheenSparkle,
    .anims      = sAnims_SheenSparkle,
    .callback   = SpriteCB_MaxCondSparkle,
};
#endif

static const struct SpritePalette sSpritePal_MonShadow =
{
    sMonShadow_Pal, TAG_MON_SHADOW
};

// code
void ShowPokemonSummaryScreen_SwSh(u8 mode, void *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void))
{
    u8 pageCount = PSS_PAGE_COUNT;

    sMonSummaryScreen = AllocZeroed(sizeof(*sMonSummaryScreen));
    sMonSummaryScreen->mode = mode;
    sHeldSlot = MOVE_SLOT_COUNT;
    sHeldAnimId = INVALID_COMFY_ANIM;
    if (monIndex == PC_MON_CHOSEN)
    {
        sMonSummaryScreen->monList.boxMons = GetBoxedMonPtr(gSpecialVar_MonBoxId, 0);
        sMonSummaryScreen->curMonIndex = gSpecialVar_MonBoxPos;
        sMonSummaryScreen->maxMonIndex = IN_BOX_COUNT - 1;
    }
    else
    {
        sMonSummaryScreen->monList.mons = mons;
        sMonSummaryScreen->curMonIndex = monIndex;
        sMonSummaryScreen->maxMonIndex = maxMonIndex;
    }
    sMonSummaryScreen->callback = callback;
    if (gInitialSummaryScreenCallback == NULL)
        gInitialSummaryScreenCallback = callback;

    if (mode == SUMMARY_MODE_BOX || monIndex == PC_MON_CHOSEN)
        sMonSummaryScreen->isBoxMon = TRUE;
    else
        sMonSummaryScreen->isBoxMon = FALSE;

    switch (mode)
    {
    case SUMMARY_MODE_NORMAL:
    case SUMMARY_MODE_BOX:
    case SUMMARY_MODE_BOX_CURSOR:
    case SUMMARY_MODE_RELEARNER_BATTLE:
    case SUMMARY_MODE_RELEARNER_CONTEST:
        sMonSummaryScreen->minPageIndex = 0;
        sMonSummaryScreen->maxPageIndex = pageCount - 1;
        break;
    case SUMMARY_MODE_LOCK_MOVES:
        sMonSummaryScreen->minPageIndex = 0;
        sMonSummaryScreen->maxPageIndex = pageCount - 1;
        sMonSummaryScreen->lockMovesFlag = TRUE;
        break;
    case SUMMARY_MODE_SELECT_MOVE:
        sMonSummaryScreen->minPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->maxPageIndex = pageCount - 1;
        sMonSummaryScreen->lockMonFlag = TRUE;
        break;
    }

    if (mode == SUMMARY_MODE_RELEARNER_BATTLE)
        sMonSummaryScreen->currPageIndex = PSS_PAGE_BATTLE_MOVES;
    else if (mode == SUMMARY_MODE_RELEARNER_CONTEST)
        sMonSummaryScreen->currPageIndex = PSS_PAGE_CONTEST_MOVES;
    else if (mode == SUMMARY_MODE_SELECT_MOVE
            && gRelearnMode == RELEARN_MODE_PSS_PAGE_CONTEST_MOVES)
        sMonSummaryScreen->currPageIndex = PSS_PAGE_CONTEST_MOVES;
    else
        sMonSummaryScreen->currPageIndex = sMonSummaryScreen->minPageIndex;

    SummaryScreen_SetAnimDelayTaskId_SwSh(TASK_NONE);
    StopShadowAnimDelayTask();

    if (gMonSpritesGfxPtr == NULL)
        CreateMonSpritesGfxManager(MON_SPR_GFX_MANAGER_A, MON_SPR_GFX_MODE_NORMAL);

    if (mode != SUMMARY_MODE_SELECT_MOVE && mode != SUMMARY_MODE_RELEARNER_BATTLE && mode != SUMMARY_MODE_RELEARNER_CONTEST)
        gMoveRelearnerState = MOVE_RELEARNER_LEVEL_UP_MOVES;

    SetMainCallback2(CB2_InitSummaryScreen);
}

void ShowSelectMovePokemonSummaryScreen_SwSh(struct Pokemon *mons, u8 monIndex, void (*callback)(void), u16 newMove)
{
    ShowPokemonSummaryScreen_SwSh(SUMMARY_MODE_SELECT_MOVE, mons, monIndex, gPartiesCount[B_TRAINER_PLAYER] - 1, callback);
    sMonSummaryScreen->newMove = newMove;
}

static void MainCB2(void)
{
    AdvanceComfyAnimations();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlank(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
    if (SWSH_SUMMARY_SCROLLING_BG)
    {
        ChangeBgX(3, 64, BG_COORD_ADD);
        ChangeBgY(3, 64, BG_COORD_ADD);
    }
    if (SWSH_SUMMARY_MON_IDLE_ANIMS && !sMonSummaryScreen->summary.isEgg)
    {
        RunMonAnimTimer();
    }
}

// Mon sprite data fields
#define sSpecies data[0]
#define sDontFlip data[1]
#define sDelayAnim data[2]
#define sIsShadow data[3]

static void RunMonAnimTimer(void)
{
    u32 i;

    if (gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].callback == SpriteCallbackDummy) // mon anim is finished
    {
        sMonSummaryScreen->monAnimTimer++;
    }

    if (sMonSummaryScreen->monAnimTimer > SWSH_SUMMARY_MON_IDLE_ANIMS_FRAMES
        && sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] != SPRITE_NONE) // time to re-run the anim
    {
        for (i = 1; i < 8; i++)
        {
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].data[i] = 0; // sprite data isn't always cleared after the anim finishes
            if (SWSH_SUMMARY_MON_SHADOWS)
                gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].data[i] = 0;
        }

        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].sSpecies = sMonSummaryScreen->summary.species2;
        if (SWSH_SUMMARY_MON_SHADOWS)
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].sSpecies = sMonSummaryScreen->summary.species2;

        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].sIsShadow = FALSE;
        if (SWSH_SUMMARY_MON_SHADOWS)
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].sIsShadow = TRUE;

        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].callback = SpriteCB_Pokemon;
        if (SWSH_SUMMARY_MON_SHADOWS)
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].callback = SpriteCB_Pokemon;

        sMonSummaryScreen->monAnimTimer = 0;
    }
}

static void CB2_InitSummaryScreen(void)
{
    while (MenuHelpers_ShouldWaitForLinkRecv() != TRUE && LoadGraphics() != TRUE && MenuHelpers_IsLinkActive() != TRUE);
}

static bool8 LoadGraphics(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ResetVramOamAndBgCntRegs();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = 1;
        gMain.state++;
        break;
    case 3:
        ResetSpriteData();
        gMain.state++;
        break;
    case 4:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 5:
        InitBGs();
        sMonSummaryScreen->switchCounter = 0;
        gMain.state++;
        break;
    case 6:
        if (DecompressGraphics() != FALSE)
            gMain.state++;
        break;
    case 7:
        ResetWindows();
        gMain.state++;
        break;
    case 8:
        CopyMonToSummaryStruct(&sMonSummaryScreen->currentMon);
        sMonSummaryScreen->switchCounter = 0;
        gMain.state++;
        break;
    case 9:
        if (ExtractMonDataToSummaryStruct(&sMonSummaryScreen->currentMon) != 0)
            gMain.state++;
        break;
    case 10:
        PrintMonPortraitInfo();
        gMain.state++;
        break;
    case 11:
        PrintPagePrompts();
        gMain.state++;
        break;
    case 12:
        ResetSpriteIds();
        PrintPageSpecificText(sMonSummaryScreen->currPageIndex);
        gMain.state++;
        break;
    case 13:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
            SetSelectMoveTilemaps();
        else if (sMonSummaryScreen->mode == SUMMARY_MODE_RELEARNER_BATTLE) // load the appropriate moves page when returning from move relearner
            SetBgTilemapBuffer(2, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES]);
        else if (sMonSummaryScreen->mode == SUMMARY_MODE_RELEARNER_CONTEST)
            SetBgTilemapBuffer(2, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_CONTEST_MOVES]);
        gMain.state++;
        break;
    case 14:
        PutPageWindowTilemaps(sMonSummaryScreen->currPageIndex);
        gMain.state++;
        break;
    case 15:
        CreateMoveTypeIcons();
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE && SWSH_SUMMARY_CATEGORY_ICONS
            && sMonSummaryScreen->currPageIndex != PSS_PAGE_CONTEST_MOVES
        )
            ShowCategoryIcon(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]);
        sMonSummaryScreen->switchCounter = 0;
        gMain.state++;
        break;
    case 16:
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] = LoadMonGfxAndSprite(&sMonSummaryScreen->currentMon, &sMonSummaryScreen->switchCounter, FALSE);
        if (SWSH_SUMMARY_MON_SHADOWS)
            sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW] = LoadMonGfxAndSprite(&sMonSummaryScreen->currentMon, &sMonSummaryScreen->switchCounter, TRUE);

        if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] != SPRITE_NONE)
        {
            sMonSummaryScreen->monAnimTimer = 0;
            sMonSummaryScreen->monAnimPlayed = FALSE;
            sMonSummaryScreen->switchCounter = 0;
            gMain.state++;
        }
        break;
    case 17:
        CreateMonMarkingsSprite(&sMonSummaryScreen->currentMon);
        gMain.state++;
        break;
    case 18:
        CreateCaughtBallSprite(&sMonSummaryScreen->currentMon);
        gMain.state++;
        break;
    case 19:
        CreateStatusSprite();
        gMain.state++;
        break;
    case 20:
        CreateGenderSprite(&sMonSummaryScreen->currentMon, sMonSummaryScreen->summary.species);
        gMain.state++;
        break;
    case 21:
        if(SWSH_SUMMARY_SHOW_GIGANTAMAX)
            CreateGigantamaxSprite();
        gMain.state++;
        break;
    case 22:
        if (sMonSummaryScreen->summary.isEgg)
            ScheduleBgCopyTilemapToVram(2);
        gMain.state++;
        break;
    case 23:
        SetTypeIcons();
        TrySetInfoPageIcons();
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
            || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
            CreateMoveSlotSprites();
        else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_SKILLS)
            CreateDynamaxLevelSprites();
        gMain.state++;
        break;
    case 24:
        if (sMonSummaryScreen->mode != SUMMARY_MODE_SELECT_MOVE)
            CreateTask(Task_HandleInput, 0);
        else
            CreateTask(Task_SetHandleReplaceMoveInput, 0);
        gMain.state++;
        break;
    case 25:
        BlendPalettes(PALETTES_ALL, 16, 0);
        gMain.state++;
        break;
    case 26:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gPaletteFade.bufferTransferDisabled = 0;
        gMain.state++;
        break;
    case 27:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_RELEARNER_BATTLE
            || sMonSummaryScreen->mode == SUMMARY_MODE_RELEARNER_CONTEST)
        {
            PrintMovesPagePrompt();
        }
        gMain.state++;
        break;
    default:
        SetVBlankCallback(VBlank);
        SetMainCallback2(MainCB2);
        return TRUE;
    }
    return FALSE;
}

static void InitBGs(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    SetBgTilemapBuffer(3, sMonSummaryScreen->bg3TilemapBuffers);
    SetBgTilemapBuffer(2, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_INFO]);
    SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_BATTLE]);
    ResetAllBgsCoordinates();
    ScheduleBgCopyTilemapToVram(3);
    ScheduleBgCopyTilemapToVram(2);
    ScheduleBgCopyTilemapToVram(1);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    if (SWSH_SUMMARY_BG_BLEND || SWSH_SUMMARY_MON_SHADOWS)
    {
        if (SWSH_SUMMARY_BG_BLEND)
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG3 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG1);
        else
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG3 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND);

        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(14, 6));
    }
    ShowBg(3);
    ShowBg(2);
    ShowBg(0);
}

static bool8 DecompressGraphics(void)
{
    switch (sMonSummaryScreen->switchCounter)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, &sSummaryScreen_Gfx, 0, 0, 0);
        sMonSummaryScreen->switchCounter++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != 1)
        {
            DecompressDataWithHeaderWram(sSummaryPage_Info_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_INFO]);
            sMonSummaryScreen->switchCounter++;
        }
        break;
    case 2:
        if (SWSH_SUMMARY_SHOW_DYNAMAX_LEVEL)
            DecompressDataWithHeaderWram(sSummaryPage_Skills_Dynamax_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_SKILLS]);
        else
            DecompressDataWithHeaderWram(sSummaryPage_Skills_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_SKILLS]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 3:
        DecompressDataWithHeaderWram(sSummaryPage_BattleMoves_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 4:
        DecompressDataWithHeaderWram(sSummaryPage_Memo_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_MEMO]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 5:
        DecompressDataWithHeaderWram(sSummaryEffect_Battle_Tilemap, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_BATTLE]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 6:
        DecompressDataWithHeaderWram(sSummaryPage_ScrollBG_Tilemap, sMonSummaryScreen->bg3TilemapBuffers);
        sMonSummaryScreen->switchCounter++;
        break;
    case 7:
        LoadPalette(sSummaryScreen_Pal, BG_PLTT_ID(0), 4 * PLTT_SIZE_4BPP);
        sMonSummaryScreen->switchCounter++;
        break;
    case 8:
        LoadCompressedSpriteSheet(&sSpriteSheet_MoveTypes);
        sMonSummaryScreen->switchCounter++;
        break;
    case 9:
        LoadCompressedSpriteSheet(&sSpriteSheet_MoveCursor);
        sMonSummaryScreen->switchCounter++;
        break;
    case 10:
        sMonSummaryScreen->switchCounter++;
        break;
    case 11:
        LoadCompressedSpriteSheet(&sSpriteSheet_StatusIcons);
        sMonSummaryScreen->switchCounter++;
        break;
    case 12:
        LoadSpritePalette(&sSpritePal_StatusIcons);
        sMonSummaryScreen->switchCounter++;
        break;
    case 13:
        LoadCompressedSpriteSheet(&sSpriteSheet_ShinyIcon);
        sMonSummaryScreen->switchCounter++;
        break;
    case 14:
        LoadSpritePalette(&sSpritePal_CategoryIcons);
        sMonSummaryScreen->switchCounter++;
        break;
    case 15:
        LoadCompressedSpriteSheet(&sSpriteSheet_PokerusCuredIcon);
        sMonSummaryScreen->switchCounter++;
        break;
    case 16:
        if (SWSH_SUMMARY_CATEGORY_ICONS)
            LoadCompressedSpriteSheet(&sSpriteSheet_CategoryIcons);
        sMonSummaryScreen->switchCounter++;
        break;
    case 17:
        if (SWSH_SUMMARY_SHOW_FRIENDSHIP)
            LoadCompressedSpriteSheet(&sSpriteSheet_FriendshipIcon);
        sMonSummaryScreen->switchCounter++;
        break;
    case 18:
        LoadSpritePalette(&sSpritePal_GenderIcons);
        sMonSummaryScreen->switchCounter++;
        break;
    case 19:
    #if SWSH_SUMMARY_SWSH_TYPE_ICONS == TRUE
        LoadPalette(sMoveTypes_Pal, OBJ_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
    #else
        LoadPalette(gMoveTypes_Pal, OBJ_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
    #endif
        sMonSummaryScreen->switchCounter++;
        break;
    case 20:
        if (SWSH_SUMMARY_SHOW_GIGANTAMAX)
            LoadCompressedSpriteSheet(&sSpriteSheet_GigantamaxIcon);
        sMonSummaryScreen->switchCounter++;
        break;
    case 21:
        if (SWSH_SUMMARY_SHOW_TERA_TYPE)
            LoadCompressedSpriteSheet(&sSpriteSheet_TeraType);
        sMonSummaryScreen->switchCounter++;
        break;
    case 22:
        sMonSummaryScreen->switchCounter++;
        break;
    case 23:
        sMonSummaryScreen->switchCounter++;
        break;
    case 24:
        sMonSummaryScreen->switchCounter++;
        break;
    case 25:
        if (SWSH_SUMMARY_SHOW_DYNAMAX_LEVEL)
            LoadCompressedSpriteSheet(&sSpriteSheet_DynamaxLevels);
        sMonSummaryScreen->switchCounter++;
        break;
    case 26:
        sMonSummaryScreen->switchCounter++;
        break;
    case 27:
        LoadSpritePalette(&sSpritePal_DynamaxLevels);
        sMonSummaryScreen->switchCounter++;
        break;
    case 28:
        LoadCompressedSpriteSheet(&sSpriteSheets_MoveSlot[0]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 29:
        LoadCompressedSpriteSheet(&sSpriteSheets_MoveSlot[1]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 30:
        LoadCompressedSpriteSheet(&sSpriteSheets_MoveSlot[2]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 31:
        LoadCompressedSpriteSheet(&sSpriteSheets_MoveSlot[3]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 32:
        LoadCompressedSpriteSheet(&sSpriteSheets_MoveSlot[4]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 33:
        LoadSpritePalette(&sSpritePal_MoveSlotMain);
        sMonSummaryScreen->switchCounter++;
        break;
    case 34:
        LoadSpritePalette(&sSpritePal_MoveSlotFocus);
        sMonSummaryScreen->switchCounter++;
        break;
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    case 35:
        DecompressDataWithHeaderWram(sSummaryPage_ContestMoves_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_CONTEST_MOVES]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 36:
        DecompressDataWithHeaderWram(sSummaryEffect_Contest_Tilemap, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONTEST]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 37:
        DecompressDataWithHeaderWram(sSummaryPage_Conditions_Tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_CONDITIONS]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 38:
        PatchPageIndicatorIcons();
        sMonSummaryScreen->switchCounter++;
        break;
    case 39:
        ConditionGraph_Init(&sMonSummaryScreen->conditionGraph);
        CpuFill32(0xEEEEEEEE, (void *)(BG_CHAR_ADDR(2) + SUMMARY_GRAPH_FILL_TILE * TILE_SIZE_4BPP), TILE_SIZE_4BPP);
        {
            u16 i;
            for (i = 0; i < PSS_BUFFER_SIZE; i++)
                sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONDITIONS][i] = (1 << 12) | SUMMARY_GRAPH_FILL_TILE;
        }
        sMonSummaryScreen->switchCounter = 0;
        return TRUE;
#else
    case 35:
        sMonSummaryScreen->switchCounter = 0;
        return TRUE;
#endif
    }
    return FALSE;
}

static void CopyMonToSummaryStruct(struct Pokemon *mon)
{
    if (!sMonSummaryScreen->isBoxMon)
    {
        struct Pokemon *partyMon = sMonSummaryScreen->monList.mons;
        *mon = partyMon[sMonSummaryScreen->curMonIndex];
    }
    else
    {
        struct BoxPokemon *boxMon = sMonSummaryScreen->monList.boxMons;
        BoxMonToMon(&boxMon[sMonSummaryScreen->curMonIndex], mon);
    }
}

static bool8 ExtractMonDataToSummaryStruct(struct Pokemon *mon)
{
    u32 i;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    // Spread the data extraction over multiple frames.
    switch (sMonSummaryScreen->switchCounter)
    {
    case 0:
        sum->species = GetMonData(mon, MON_DATA_SPECIES);
        sum->species2 = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        sum->exp = GetMonData(mon, MON_DATA_EXP);
        sum->level = GetMonData(mon, MON_DATA_LEVEL);
        sum->abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
        sum->item = GetMonData(mon, MON_DATA_HELD_ITEM);
        sum->pid = GetMonData(mon, MON_DATA_PERSONALITY);
        sum->sanity = GetMonData(mon, MON_DATA_SANITY_IS_BAD_EGG);

        if (sum->sanity)
            sum->isEgg = TRUE;
        else
            sum->isEgg = GetMonData(mon, MON_DATA_IS_EGG);

        break;
    case 1:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            sum->moves[i] = GetMonData(mon, MON_DATA_MOVE1+i);
            sum->pp[i] = GetMonData(mon, MON_DATA_PP1+i);
        }
        sum->ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
        break;
    case 2:
        sum->nature = GetNature(mon);
        sum->mintNature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
        sum->currentHP = GetMonData(mon, MON_DATA_HP);
        sum->maxHP = GetMonData(mon, MON_DATA_MAX_HP);
        sum->atk = GetMonData(mon, MON_DATA_ATK);
        sum->def = GetMonData(mon, MON_DATA_DEF);
        sum->spatk = GetMonData(mon, MON_DATA_SPATK);
        sum->spdef = GetMonData(mon, MON_DATA_SPDEF);
        sum->speed = GetMonData(mon, MON_DATA_SPEED);
        break;
    case 3:
        GetMonData(mon, MON_DATA_OT_NAME, sum->OTName);
        ConvertInternationalString(sum->OTName, GetMonData(mon, MON_DATA_LANGUAGE));
        sum->ailment = GetMonAilment(mon);
        sum->OTGender = GetMonData(mon, MON_DATA_OT_GENDER);
        sum->OTID = GetMonData(mon, MON_DATA_OT_ID);
        sum->metLocation = GetMonData(mon, MON_DATA_MET_LOCATION);
        sum->metLevel = GetMonData(mon, MON_DATA_MET_LEVEL);
        sum->metGame = GetMonData(mon, MON_DATA_MET_GAME);
        sum->friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
        break;
    case 4:
        sum->ivHp = GetMonData(mon, MON_DATA_HP_IV);
        sum->ivAtk = GetMonData(mon, MON_DATA_ATK_IV);
        sum->ivDef = GetMonData(mon, MON_DATA_DEF_IV);
        sum->ivSpatk = GetMonData(mon, MON_DATA_SPATK_IV);
        sum->ivSpdef = GetMonData(mon, MON_DATA_SPDEF_IV);
        sum->ivSpeed = GetMonData(mon, MON_DATA_SPEED_IV);
        break;
    case 5:
        sum->evHp = GetMonData(mon, MON_DATA_HP_EV);
        sum->evAtk = GetMonData(mon, MON_DATA_ATK_EV);
        sum->evDef = GetMonData(mon, MON_DATA_DEF_EV);
        sum->evSpatk = GetMonData(mon, MON_DATA_SPATK_EV);
        sum->evSpdef = GetMonData(mon, MON_DATA_SPDEF_EV);
        sum->evSpeed = GetMonData(mon, MON_DATA_SPEED_EV);
        break;
    default:
        sum->ribbonCount = GetMonData(mon, MON_DATA_RIBBON_COUNT);
        sum->teraType = GetMonData(mon, MON_DATA_TERA_TYPE);
        sum->isShiny = GetMonData(mon, MON_DATA_IS_SHINY);
        return TRUE;
    }
    sMonSummaryScreen->switchCounter++;
    return FALSE;
}

static void SetSelectMoveTilemaps(void)
{
    if (gRelearnMode == RELEARN_MODE_PSS_PAGE_CONTEST_MOVES)
    {
        SetBgTilemapBuffer(2, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_CONTEST_MOVES]);
        SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONTEST]);
    }
    else
    {
        SetBgTilemapBuffer(2, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES]);
        SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_BATTLE]);
    }
    ShowBg(1);
}

// Page indicator icon tile bases (2×2 each; base+0..+3 are TL, TR, BL, BR)
#define PAGE_ICON_TILE_HL_INFO          1   // highlighted (active page)
#define PAGE_ICON_TILE_HL_SKILLS        5
#define PAGE_ICON_TILE_HL_BATTLE_MOVES  9
#define PAGE_ICON_TILE_HL_CONDITIONS    13
#define PAGE_ICON_TILE_HL_CONTEST_MOVES 17
#define PAGE_ICON_TILE_HL_MEMO          21
#define PAGE_ICON_TILE_INFO             25  // non-highlighted
#define PAGE_ICON_TILE_SKILLS           29
#define PAGE_ICON_TILE_BATTLE_MOVES     33
#define PAGE_ICON_TILE_CONDITIONS       37
#define PAGE_ICON_TILE_CONTEST_MOVES    41
#define PAGE_ICON_TILE_MEMO             45


#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
static void ApplyPageIcon(u16 *buf, u8 slot, u16 baseTile)
{
    u8 col = 2 * slot - 1; // slot 1→col 1, slot 2→col 3, slot 3→col 5, slot 4→col 7
    buf[32 + col]     = baseTile;
    buf[32 + col + 1] = baseTile + 1;
    buf[64 + col]     = baseTile + 2;
    buf[64 + col + 1] = baseTile + 3;
}

static void PatchPageIndicatorIcons(void)
{
    // Info: slot 4 Memo → Conditions (inactive)
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_INFO],         4, PAGE_ICON_TILE_CONTEST_MOVES);

    // Skills: slot 4 Memo → Conditions (inactive)
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_SKILLS],       4, PAGE_ICON_TILE_CONTEST_MOVES);

    // Battle moves: all 4 slots shift to accommodate contest pages
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES], 1, PAGE_ICON_TILE_SKILLS);
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES], 2, PAGE_ICON_TILE_HL_BATTLE_MOVES);
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES], 3, PAGE_ICON_TILE_CONTEST_MOVES);
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_BATTLE_MOVES], 4, PAGE_ICON_TILE_CONDITIONS);

    // Memo: slots 1–3 shift to accommodate contest pages
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_MEMO],         1, PAGE_ICON_TILE_BATTLE_MOVES);
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_MEMO],         2, PAGE_ICON_TILE_CONTEST_MOVES);
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_MEMO],         3, PAGE_ICON_TILE_CONDITIONS);
}
#endif

static void FreeSummaryScreen(void)
{
    FreeAllWindowBuffers();
    Free(sMonSummaryScreen);
}

static void BeginCloseSummaryScreen(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = CloseSummaryScreen;
}

static void CloseSummaryScreen(u8 taskId)
{
    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE && !gPaletteFade.active)
    {
        if (SWSH_SUMMARY_BG_BLEND || SWSH_SUMMARY_MON_SHADOWS)
        {
            SetGpuReg(REG_OFFSET_BLDCNT, 0);
            SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        }
        if (sMonSummaryScreen->callback == gInitialSummaryScreenCallback)
            gInitialSummaryScreenCallback = NULL;
        SetMainCallback2(sMonSummaryScreen->callback);
        gLastViewedMonIndex = sMonSummaryScreen->curMonIndex;
        SummaryScreen_DestroyAnimDelayTask();
        ResetSpriteData();
        FreeAllSpritePalettes();
        StopCryAndClearCrySongs();
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 0x100);
        if (gMonSpritesGfxPtr == NULL)
            DestroyMonSpritesGfxManager(MON_SPR_GFX_MANAGER_A);
        FreeSummaryScreen();
        DestroyTask(taskId);
    }
}

#define tSkillsState data[3]

// Cycle summary page between stats, IVs and EVs
static void ChangeSummaryState(s16 *data, u8 taskId)
{
    switch (tSkillsState)
    {
    case SKILL_STATE_STATS:
        tSkillsState = SKILL_STATE_IVS;
        break;
    case SKILL_STATE_IVS:
        tSkillsState = SKILL_STATE_EVS;
        break;
    case SKILL_STATE_EVS:
        tSkillsState = SKILL_STATE_STATS;
        break;
    }

    gTasks[taskId].func = Task_HandleInput;
}

enum {
    BUTTON_A,
    BUTTON_B,
    BUTTON_START,
    BUTTON_LR,
};

static void PrintRightAlignedPrompt(u8 windowId, u8 button, const u8 *text, int maxPx, u8 colorId)
{
    int stringXPos = GetStringRightAlignXOffset(FONT_SMALL, text, maxPx);
    int iconXPos = stringXPos - GetButtonTextOffset(button);
    if (iconXPos < 0)
        iconXPos = 0;
    PrintButtonIcon(windowId, button, iconXPos, 4);
    PrintTextOnWindowWithFont(windowId, text, stringXPos, 0, 0, colorId, FONT_SMALL);
}

// draw button prompts when cycling between stats, IVs and EVs
static void DrawNextSkillsButtonPrompt(u8 mode)
{
    const u8 *text;

    switch (mode)
    {
    case SKILL_STATE_STATS: text = sText_ViewIVs;   break;
    case SKILL_STATE_IVS:   text = sText_ViewEVs;   break;
    default:                text = sText_ViewStats; break;
    }
    FillWindowPixelBuffer(PSS_LABEL_WINDOW_PROMPT_IV_EV_STATS, PIXEL_FILL(0));
    PrintRightAlignedPrompt(PSS_LABEL_WINDOW_PROMPT_IV_EV_STATS, BUTTON_A, text, 76, 1);
    ScheduleBgCopyTilemapToVram(0);
}

#define IS_MOVE_PAGE(page) (page == PSS_PAGE_BATTLE_MOVES || page == PSS_PAGE_CONTEST_MOVES)

static void Task_HandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 defaultSkillsState = SKILL_STATE_STATS;

    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE && !gPaletteFade.active)
    {
        if (JOY_NEW(DPAD_UP))
        {
            tSkillsState = defaultSkillsState;
            ChangeSummaryPokemon(taskId, -1);
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            tSkillsState = defaultSkillsState;
            ChangeSummaryPokemon(taskId, 1);
        }
        else if ((JOY_NEW(DPAD_LEFT)))
        {
            tSkillsState = defaultSkillsState;
            ChangePage(taskId, -1);
        }
        else if ((JOY_NEW(DPAD_RIGHT)))
        {
            tSkillsState = defaultSkillsState;
            ChangePage(taskId, 1);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
                || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
            {
                PlaySE(SE_SELECT);
                SwitchToMoveSelection(taskId);
            }
            else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_SKILLS)
            {
                if (SWSH_SUMMARY_SHOW_IV_EV)
                {
                    // Cycle through IVs/EVs/stats on pressing A
                    ChangeSummaryState(data, taskId);
                    DrawNextSkillsButtonPrompt(tSkillsState);
                    PlaySE(SE_SELECT);
                    PrintStats(tSkillsState);
                }
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            StopPokemonAnimations();
            PlaySE(SE_SELECT);
            BeginCloseSummaryScreen(taskId);
        }
        else if (JOY_NEW(START_BUTTON))
        {
            if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO && ShouldShowRename())
            {
                if (sMonSummaryScreen->isBoxMon)
                {
                    gSpecialVar_0x8004 = PC_MON_CHOSEN;
                    gSpecialVar_MonBoxPos = sMonSummaryScreen->curMonIndex;
                    gSpecialVar_MonBoxId = StorageGetCurrentBox();
                }
                else
                {
                    gSpecialVar_0x8004 = sMonSummaryScreen->curMonIndex;
                }
                sMonSummaryScreen->callback = CB2_PssChangePokemonNickname;
                StopPokemonAnimations();
                PlaySE(SE_SELECT);
                BeginCloseSummaryScreen(taskId);
            }
            if (ShouldShowMoveRelearner()
                && (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
                    || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES))
            {
                sMonSummaryScreen->callback = CB2_InitLearnMove;
                gRelearnMode = sMonSummaryScreen->currPageIndex;
                gSpecialVar_MonBoxPos = sMonSummaryScreen->curMonIndex;
                if (sMonSummaryScreen->isBoxMon)
                {
                    gSpecialVar_0x8004 = PC_MON_CHOSEN;
                    gSpecialVar_MonBoxPos = sMonSummaryScreen->curMonIndex;
                    gSpecialVar_MonBoxId = StorageGetCurrentBox();
                }
                else
                {
                    gSpecialVar_0x8004 = sMonSummaryScreen->curMonIndex;
                }
                StopPokemonAnimations();
                PlaySE(SE_SELECT);
                BeginCloseSummaryScreen(taskId);
            }
        }
        else if (DEBUG_POKEMON_SPRITE_VISUALIZER && JOY_NEW(SELECT_BUTTON) && !gMain.inBattle)
        {
            sMonSummaryScreen->callback = CB2_Pokemon_Sprite_Visualizer;
            StopPokemonAnimations();
            PlaySE(SE_SELECT);
            CloseSummaryScreen(taskId);
        }
        else if (JOY_NEW(R_BUTTON)) // R means increase. Level -> Egg -> TM -> Tutor
        {
            if (P_SUMMARY_SCREEN_MOVE_RELEARNER && IS_MOVE_PAGE(sMonSummaryScreen->currPageIndex) && !gMain.inBattle)
            {
                gMoveRelearnerState++;
                UpdateMoveRelearnerState(FALSE);
                RefreshRelearnModePrompt();
                PlaySE(SE_SELECT);
            }
        }
        else if (JOY_NEW(L_BUTTON)) // L means decrease. Level <- Egg <- TM <- Tutor
        {
            if (P_SUMMARY_SCREEN_MOVE_RELEARNER && IS_MOVE_PAGE(sMonSummaryScreen->currPageIndex) && !gMain.inBattle)
            {
                gMoveRelearnerState--;
                UpdateMoveRelearnerState(TRUE);
                RefreshRelearnModePrompt();
                PlaySE(SE_SELECT);
            }
        }
    }
}

#undef tSkillsState

#define tSummaryState data[0]

static struct BoxPokemon *GetCurrentBoxmon(void)
{
    if (sMonSummaryScreen->isBoxMon)
        return &sMonSummaryScreen->monList.boxMons[sMonSummaryScreen->curMonIndex];
    return &sMonSummaryScreen->monList.mons[sMonSummaryScreen->curMonIndex].box;
}

static bool32 HasAnyRelearnableMoves(enum MoveRelearnerStates state)
{
    return CanBoxMonRelearnMoves(GetCurrentBoxmon(), state);
}

static void UpdateMoveRelearnerState(bool32 goingDown)
{
    s32 state;

    sMonSummaryScreen->hasRelearnableMoves = FALSE;
    for (u32 i = 0; i < MOVE_RELEARNER_COUNT; i++)
    {
        state = (gMoveRelearnerState + i * (goingDown ? -1 : 1)) % MOVE_RELEARNER_COUNT;
        if (HasAnyRelearnableMoves(state))
        {
            sMonSummaryScreen->hasRelearnableMoves = TRUE;
            gMoveRelearnerState = state;
            break;
        }
    }
}

static void ChangeSummaryPokemon(u8 taskId, s8 delta)
{
    s8 monId;

    if (!sMonSummaryScreen->lockMonFlag)
    {
        if (sMonSummaryScreen->isBoxMon == TRUE)
        {
            if (sMonSummaryScreen->currPageIndex != PSS_PAGE_INFO)
            {
                if (delta == 1)
                    delta = 0;
                else
                    delta = 2;
            }
            else
            {
                if (delta == 1)
                    delta = 1;
                else
                    delta = 3;
            }
            monId = AdvanceStorageMonIndex(sMonSummaryScreen->monList.boxMons, sMonSummaryScreen->curMonIndex, sMonSummaryScreen->maxMonIndex, delta);
        }
        else if (IsMultiBattle() == TRUE && !AreMultiPartiesFullTeams())
        {
            monId = AdvanceMultiBattleMonIndex(delta);
        }
        else
        {
            monId = AdvanceMonIndex(delta);
        }

        if (monId != -1)
        {
            PlaySE(SE_SELECT);
            sMonSummaryScreen->curMonIndex = monId;
            sMonSummaryScreen->monAnimTimer = 0;
            sMonSummaryScreen->monAnimPlayed = FALSE;
            gTasks[taskId].tSummaryState = 0;
            gTasks[taskId].func = Task_ChangeSummaryMon;
        }
    }
}

static void Task_ChangeSummaryMon(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tSummaryState)
    {
    case 0:
        StopCryAndClearCrySongs();
        break;
    case 1:
        SummaryScreen_DestroyAnimDelayTask();
        DestroySpriteAndFreeResources(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]]);
        if (SWSH_SUMMARY_MON_SHADOWS)
            DestroySpriteAndFreeResources(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]]);
        DestroyHeldItemIconSprite();
        break;
    case 2:
        DestroySpriteAndFreeResources(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]]);
        break;
    case 3:
        CopyMonToSummaryStruct(&sMonSummaryScreen->currentMon);
        sMonSummaryScreen->switchCounter = 0;
        break;
    case 4:
        if (ExtractMonDataToSummaryStruct(&sMonSummaryScreen->currentMon) == FALSE)
        {
            return;
        }
        else
        {
            if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
                || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
            {
                if (P_SUMMARY_SCREEN_MOVE_RELEARNER)
                {
                    gMoveRelearnerState = MOVE_RELEARNER_LEVEL_UP_MOVES;
                    UpdateMoveRelearnerState(FALSE);
                }
                PrintMovesPagePrompt();
            }
        }
        break;
    case 5:
        UpdateMonMarkingsSprite(&sMonSummaryScreen->currentMon);
        break;
    case 6:
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] = LoadMonGfxAndSprite(&sMonSummaryScreen->currentMon, &data[1], FALSE);

        if (SWSH_SUMMARY_MON_SHADOWS)
            sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW] = LoadMonGfxAndSprite(&sMonSummaryScreen->currentMon, &data[1], TRUE);

        if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] == SPRITE_NONE)
            return;

        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].sDelayAnim = 1;
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].sIsShadow = FALSE;

        if (SWSH_SUMMARY_MON_SHADOWS)
        {
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].sDelayAnim = 1;
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].sIsShadow = TRUE;
        }

        TryDrawExperienceProgressBar();
        data[1] = 0;
        break;
    case 7:
        CreateCaughtBallSprite(&sMonSummaryScreen->currentMon);
        break;
    case 8:
        CreateGenderSprite(&sMonSummaryScreen->currentMon, sMonSummaryScreen->summary.species);
        break;
    case 9:
        if (SWSH_SUMMARY_SHOW_GIGANTAMAX)
            CreateGigantamaxSprite();
        break;
    case 10:
        SetTypeIcons();
        TrySetInfoPageIcons();
        break;
    case 11:
        PrintMonPortraitInfo();
        HandleStatusSprite(&sMonSummaryScreen->currentMon); // Moved here from case 7
        break;
    case 12:
        PrintPageSpecificText(sMonSummaryScreen->currPageIndex);
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO)
        {
            ScheduleBgCopyTilemapToVram(2);

            if (P_SUMMARY_SCREEN_RENAME)
            {
                FillWindowPixelBuffer(PSS_LABEL_WINDOW_PROMPT_CANCEL, PIXEL_FILL(0));
                ShowCancelOrRenamePrompt();
                PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_CANCEL);
            }
            RefreshInfoPageItemTilemap();
            CreateHeldItemSprite();
        }
        else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_SKILLS)
        {
            DrawNextSkillsButtonPrompt(SKILL_STATE_STATS);
            CreateDynamaxLevelSprites();
        }
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
        else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONDITIONS
              || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
        {
            DestroyMaxConditionSparkles();
            DestroySheenSparkleSprites();
            DestroyContestCategoryIcons();
            CreateTextPrinterTask(sMonSummaryScreen->currPageIndex);
        }
#endif
        TryDrawExperienceProgressBar();
        break;
    case 13:
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].sDelayAnim = 0;
        if (SWSH_SUMMARY_MON_SHADOWS)
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].sDelayAnim = 0;
        break;
    default:
        if (!MenuHelpers_ShouldWaitForLinkRecv())
        {
            data[0] = 0; // tSkillsState
            gTasks[taskId].func = Task_HandleInput;
        }
        return;
    }
    tSummaryState++;
}

static s8 AdvanceMonIndex(s8 delta)
{
    struct Pokemon *mon = sMonSummaryScreen->monList.mons;
    u8 index = sMonSummaryScreen->curMonIndex;
    u8 numMons = sMonSummaryScreen->maxMonIndex + 1;
    delta += numMons;

    index = (index + delta) % numMons;

    // skip over any Eggs unless on the Info Page
    if (sMonSummaryScreen->currPageIndex != PSS_PAGE_INFO)
        while (GetMonData(&mon[index], MON_DATA_IS_EGG))
            index = (index + delta) % numMons;

    // to avoid "scrolling" to the same Pokemon
    if (index == sMonSummaryScreen->curMonIndex)
        return -1;
    else
        return index;
}

static s8 AdvanceMultiBattleMonIndex(s8 delta)
{
    struct Pokemon *mons = sMonSummaryScreen->monList.mons;
    s8 index, arrId = 0;
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sMultiBattleOrder[i] == sMonSummaryScreen->curMonIndex)
        {
            arrId = i;
            break;
        }
    }

    while (TRUE)
    {
        const s8 *order = sMultiBattleOrder;

        arrId += delta;
        if (arrId < 0 || arrId >= PARTY_SIZE)
            return -1;
        index = order[arrId];
        if (IsValidToViewInMulti(&mons[index]) == TRUE)
            return index;
    }
}

static bool8 IsValidToViewInMulti(struct Pokemon *mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return FALSE;
    else if (sMonSummaryScreen->curMonIndex != 0 || !GetMonData(mon, MON_DATA_IS_EGG))
        return TRUE;
    else
        return FALSE;
}

#define tScrollState data[0]
#define tMosaicStrength data[1]

static void ChangePage(u8 taskId, s8 delta)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    s16 *data = gTasks[taskId].data;
    u32 currPageIndex;

    if (summary->isEgg)
        return;

    PlaySE(SE_SELECT);
    ClearPageWindowTilemaps(sMonSummaryScreen->currPageIndex);
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_SKILLS)
    {
        DestroyDynamaxLevelSprites();
    }
    else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO)
    {
        DestroyHeldItemIconSprite();
    }
    else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
    {
        DestroyMoveSlotSprites();
    }
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONDITIONS)
    {
        DestroyMaxConditionSparkles();
        DestroySheenSparkleSprites();
        DestroyContestCategoryIcons();
    }
#endif
    else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
    {
        DestroyMoveSlotSprites();
    }

    // Wrap around pages (after clearing the old page's tilemaps)
    if (delta == -1 && sMonSummaryScreen->currPageIndex == sMonSummaryScreen->minPageIndex)
        sMonSummaryScreen->currPageIndex = sMonSummaryScreen->maxPageIndex + 1;
    else if (delta == 1 && sMonSummaryScreen->currPageIndex == sMonSummaryScreen->maxPageIndex)
        sMonSummaryScreen->currPageIndex = sMonSummaryScreen->minPageIndex - 1;

    currPageIndex = sMonSummaryScreen->currPageIndex += delta;

#if !SWSH_SUMMARY_SHOW_CONTEST_PAGES
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
        currPageIndex = sMonSummaryScreen->currPageIndex += delta;
#endif

    tScrollState = 0;
    SetTaskFuncWithFollowupFunc(taskId, PssScroll, gTasks[taskId].func);
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    if (sMonSummaryScreen->currPageIndex != PSS_PAGE_CONDITIONS)
#endif
        CreateTextPrinterTask(sMonSummaryScreen->currPageIndex);
    HidePageSpecificSprites();

    // acts like a quick reset
    if (currPageIndex == PSS_PAGE_SKILLS)
    {
        gMoveRelearnerState = MOVE_RELEARNER_LEVEL_UP_MOVES;
        UpdateMoveRelearnerState(FALSE);
    }
}

static void PssScroll(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (tScrollState == 0)
    {
        tMosaicStrength = 0;
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        SetBgTilemapBuffer(2, sMonSummaryScreen->bg2TilemapBuffers[sMonSummaryScreen->currPageIndex]);
        ShowBg(2);
        SetGpuRegBits(REG_OFFSET_BG2CNT, BGCNT_MOSAIC);
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
            SetGpuRegBits(REG_OFFSET_BG1CNT, BGCNT_MOSAIC);
    }

    // build up mosaic effect
    if (tScrollState <= 3)
    {
        tMosaicStrength += 1;
        SetGpuReg(REG_OFFSET_MOSAIC, (tMosaicStrength & 15) * 17);
    }
    // build down mosaic effect
    if (tScrollState > 3)
    {
        tMosaicStrength -= 1;
        SetGpuReg(REG_OFFSET_MOSAIC, (tMosaicStrength & 15) * 17);
    }

    tScrollState++;

    if (tScrollState >= 8)
        gTasks[taskId].func = PssScrollEnd;
}

static void PssScrollEnd(u8 taskId)
{
    if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
    {
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
        {
            enum Move move = sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES
                ? sMonSummaryScreen->newMove
                : sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
            SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONTEST]);
            HandleAppealJamTilemap(move);
        }
        else
        {
            SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_BATTLE]);
            ScheduleBgCopyTilemapToVram(1);
        }
    }

    ClearGpuRegBits(REG_OFFSET_BG2CNT, BGCNT_MOSAIC);
    if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        ClearGpuRegBits(REG_OFFSET_BG1CNT, BGCNT_MOSAIC);

    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    ScheduleBgCopyTilemapToVram(2);
    PutPageWindowTilemaps(sMonSummaryScreen->currPageIndex);

    SetTypeIcons();
    TrySetInfoPageIcons();
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_SKILLS)
    {
        CreateDynamaxLevelSprites();
    }
    else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
            || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
    {
        CreateMoveSlotSprites();
        if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_CURSOR] != SPRITE_NONE)
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_MOVE_CURSOR, FALSE);
            UpdateMoveSlotPalette();
        }
    }
    TryDrawExperienceProgressBar();

    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO)
    {
        if (sMonSummaryScreen->markingsSprite != NULL)
           sMonSummaryScreen->markingsSprite->invisible = FALSE;
    }
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONDITIONS)
        CreateTextPrinterTask(sMonSummaryScreen->currPageIndex);
#endif

    SwitchTaskToFollowupFunc(taskId);
}

#undef tScrollState
#undef tMosaicStrength

static void TryDrawExperienceProgressBar(void)
{
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_MEMO)
        DrawExperienceProgressBar(&sMonSummaryScreen->currentMon);
}


static void SwitchToMoveSelection(u8 taskId)
{
    sMonSummaryScreen->firstMoveIndex = 0;

    ClearMovesPagePrompt();
    if (!sMonSummaryScreen->lockMovesFlag)
    {
        PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_SWITCH);
    }

    CreateTask(Task_ShowEffectTilemap, 1);

    CreateMoveCursorSprite();
    UpdateMoveSlotPalette();
    gTasks[taskId].func = Task_HandleInput_MoveSelect;
}

static void Task_HandleInput_MoveSelect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
    {
        if (JOY_REPEAT(DPAD_UP))
        {
            data[0] = MAX_MON_MOVES - 1;
            ChangeSelectedMove(data, -1, &sMonSummaryScreen->firstMoveIndex);
        }
        else if (JOY_REPEAT(DPAD_DOWN))
        {
            data[0] = MAX_MON_MOVES - 1;
            ChangeSelectedMove(data, 1, &sMonSummaryScreen->firstMoveIndex);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            if (sMonSummaryScreen->lockMovesFlag == TRUE)
            {
                PlaySE(SE_SELECT);
                CloseMoveSelectMode(taskId);
            }
            else if (HasMoreThanOneMove() == TRUE)
            {
                PlaySE(SE_SELECT);
                SwitchToMovePositionSwitchMode(taskId);
            }
            else
            {
                PlaySE(SE_FAILURE);
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            CloseMoveSelectMode(taskId);
        }
    }
}

static bool8 HasMoreThanOneMove(void)
{
    u8 i;
    for (i = 1; i < MAX_MON_MOVES; i++)
    {
        if (sMonSummaryScreen->summary.moves[i] != 0)
            return TRUE;
    }
    return FALSE;
}

static void ChangeSelectedMove(s16 *taskData, s8 direction, u8 *moveIndexPtr)
{
    s8 i, newMoveIndex;
    enum Move move;

    PlaySE(SE_SELECT);
    newMoveIndex = *moveIndexPtr;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        newMoveIndex += direction;
        if (newMoveIndex > taskData[0])
            newMoveIndex = 0;
        else if (newMoveIndex < 0)
            newMoveIndex = taskData[0];

        if (newMoveIndex == MAX_MON_MOVES)
        {
            move = sMonSummaryScreen->newMove;
            break;
        }
        move = sMonSummaryScreen->summary.moves[newMoveIndex];
        if (move != 0)
            break;
    }

    PrintMoveDescription(move);

    *moveIndexPtr = newMoveIndex;
    UpdateMoveSlotPalette();
}

static void CloseMoveSelectMode(u8 taskId)
{
    DestroyMoveCursorSprite();
    UpdateMoveSlotPalette();
    ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_SWITCH);
    PrintMoveDescription(MOVE_NONE);
    sMonSummaryScreen->mode = SUMMARY_MODE_NORMAL;

    CreateTask(Task_HideEffectTilemap, 1);
    gTasks[taskId].func = Task_HandleInput;
}

static void SwitchToMovePositionSwitchMode(u8 taskId)
{
    sHeldSlot = sMonSummaryScreen->firstMoveIndex;
    sMonSummaryScreen->secondMoveIndex = sHeldSlot;
    LiftMoveSlot(sHeldSlot);
    UpdateMoveSlotPalette();
    gTasks[taskId].func = Task_HandleInput_MoveSwitch;
}

static void Task_HandleInput_MoveSwitch(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 slot = sHeldSlot;
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slot * MOVE_SLOT_SPRITES_COUNT)];
    u8 typeIconId = sMonSummaryScreen->spriteIds[slot + SPRITE_ARR_ID_TYPE];
    u8 i;

    if (sHeldAnimId != INVALID_COMFY_ANIM)
    {
        s16 animY = ReadComfyAnimValueSmooth(&gComfyAnims[sHeldAnimId]);
        for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
        {
            if (spriteIds[i] != SPRITE_NONE)
                gSprites[spriteIds[i]].y = animY;
        }
        if (typeIconId != SPRITE_NONE)
            gSprites[typeIconId].y = animY;
    }

    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
    {
        if (JOY_REPEAT(DPAD_UP))
        {
            data[0] = MAX_MON_MOVES - 1;
            ChangeSelectedMove(data, -1, &sMonSummaryScreen->secondMoveIndex);
            AnimateLiftedSlotToTarget();
        }
        else if (JOY_REPEAT(DPAD_DOWN))
        {
            data[0] = MAX_MON_MOVES - 1;
            ChangeSelectedMove(data, 1, &sMonSummaryScreen->secondMoveIndex);
            AnimateLiftedSlotToTarget();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            if (sMonSummaryScreen->firstMoveIndex == sMonSummaryScreen->secondMoveIndex)
                ExitMovePositionSwitchMode(taskId, FALSE);
            else
                ExitMovePositionSwitchMode(taskId, TRUE);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            ExitMovePositionSwitchMode(taskId, FALSE);
        }
    }
}

static void ExitMovePositionSwitchMode(u8 taskId, bool8 swapMoves)
{
    enum Move move;

    PlaySE(SE_SELECT);
    DropMoveSlot();

    if (swapMoves == TRUE)
    {
        if (!sMonSummaryScreen->isBoxMon)
        {
            struct Pokemon *mon = sMonSummaryScreen->monList.mons;
            SwapMonMoves(&mon[sMonSummaryScreen->curMonIndex], sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        }
        else
        {
            struct BoxPokemon *boxMon = sMonSummaryScreen->monList.boxMons;
            SwapBoxMonMoves(&boxMon[sMonSummaryScreen->curMonIndex], sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        }
        CopyMonToSummaryStruct(&sMonSummaryScreen->currentMon);
        SwapMovesNamesPP(sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        SwapMovesTypeSprites(sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        sMonSummaryScreen->firstMoveIndex = sMonSummaryScreen->secondMoveIndex;
    }
    UpdateMoveSlotPalette();

    move = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
    PrintMoveDescription(move);
    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);
    gTasks[taskId].func = Task_HandleInput_MoveSelect;
}

static void SwapMonMoves(struct Pokemon *mon, u8 moveIndex1, u8 moveIndex2)
{
    struct PokeSummary* summary = &sMonSummaryScreen->summary;

    enum Move move1 = summary->moves[moveIndex1];
    enum Move move2 = summary->moves[moveIndex2];
    u8 move1pp = summary->pp[moveIndex1];
    u8 move2pp = summary->pp[moveIndex2];
    u8 ppBonuses = summary->ppBonuses;

    // Calculate PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetMonData(mon, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetMonData(mon, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetMonData(mon, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetMonData(mon, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);

    summary->moves[moveIndex1] = move2;
    summary->moves[moveIndex2] = move1;

    summary->pp[moveIndex1] = move2pp;
    summary->pp[moveIndex2] = move1pp;

    summary->ppBonuses = ppBonuses;
}

static void SwapBoxMonMoves(struct BoxPokemon *mon, u8 moveIndex1, u8 moveIndex2)
{
    struct PokeSummary* summary = &sMonSummaryScreen->summary;

    enum Move move1 = summary->moves[moveIndex1];
    enum Move move2 = summary->moves[moveIndex2];
    u8 move1pp = summary->pp[moveIndex1];
    u8 move2pp = summary->pp[moveIndex2];
    u8 ppBonuses = summary->ppBonuses;

    // Calculate PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetBoxMonData(mon, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetBoxMonData(mon, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetBoxMonData(mon, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetBoxMonData(mon, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetBoxMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);

    summary->moves[moveIndex1] = move2;
    summary->moves[moveIndex2] = move1;

    summary->pp[moveIndex1] = move2pp;
    summary->pp[moveIndex2] = move1pp;

    summary->ppBonuses = ppBonuses;
}

static void Task_SetHandleReplaceMoveInput(u8 taskId)
{
    SetNewMoveTypeIcon();
    CreateMoveCursorSprite();
    UpdateMoveSlotPalette();
    gTasks[taskId].func = Task_HandleReplaceMoveInput;
}

static void Task_HandleReplaceMoveInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE)
    {
        if (gPaletteFade.active != TRUE)
        {
            if (JOY_NEW(DPAD_UP))
            {
                data[0] = 4;
                ChangeSelectedMove(data, -1, &sMonSummaryScreen->firstMoveIndex);
            }
            else if (JOY_NEW(DPAD_DOWN))
            {
                data[0] = 4;
                ChangeSelectedMove(data, 1, &sMonSummaryScreen->firstMoveIndex);
            }
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
            else if (JOY_NEW(DPAD_LEFT))
            {
                if (sMonSummaryScreen->currPageIndex != PSS_PAGE_BATTLE_MOVES)
                    ChangePage(taskId, -1);
            }
            else if (JOY_NEW(DPAD_RIGHT))
            {
                if (sMonSummaryScreen->currPageIndex != PSS_PAGE_CONTEST_MOVES)
                    ChangePage(taskId, 1);
            }
#endif
            else if (JOY_NEW(A_BUTTON))
            {
                if (CanReplaceMove() == TRUE)
                {
                    StopPokemonAnimations();
                    PlaySE(SE_SELECT);
                    sMoveSlotToReplace = sMonSummaryScreen->firstMoveIndex;
                    gSpecialVar_0x8005 = sMoveSlotToReplace;
                    gSpecialVar_Result = TRUE;
                    BeginCloseSummaryScreen(taskId);
                }
                else
                {
                    PlaySE(SE_FAILURE);
                    ShowCantForgetHMsWindow(taskId);
                }
            }
            else if (JOY_NEW(B_BUTTON))
            {
                StopPokemonAnimations();
                PlaySE(SE_SELECT);
                sMoveSlotToReplace = MAX_MON_MOVES;
                gSpecialVar_0x8005 = MAX_MON_MOVES;
                gSpecialVar_Result = FALSE;
                BeginCloseSummaryScreen(taskId);
            }
        }
    }
}

static bool8 CanReplaceMove(void)
{
    if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES
        || sMonSummaryScreen->newMove == MOVE_NONE
        || IsMoveHM(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]) != TRUE)
        return TRUE;
    else
        return FALSE;
}

static void ShowCantForgetHMsWindow(u8 taskId)
{
    PrintHMMovesCantBeForgotten();
    gTasks[taskId].func = Task_HandleInputCantForgetHMsMoves;
}

// This redraws the power/accuracy window when the player scrolls out of the "HM Moves can't be forgotten" message
static void Task_HandleInputCantForgetHMsMoves(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    enum Move move;
    if (FuncIsActiveTask(Task_ShowEffectTilemap) != 1)
    {
        if (JOY_NEW(DPAD_UP))
        {
            data[1] = 1;
            data[0] = 4;
            ChangeSelectedMove(&data[0], -1, &sMonSummaryScreen->firstMoveIndex);
            data[1] = 0;
            gTasks[taskId].func = Task_HandleReplaceMoveInput;
        }
        else if (JOY_NEW(DPAD_DOWN))
        {
            data[1] = 1;
            data[0] = 4;
            ChangeSelectedMove(&data[0], 1, &sMonSummaryScreen->firstMoveIndex);
            data[1] = 0;
            gTasks[taskId].func = Task_HandleReplaceMoveInput;
        }
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
        else if (JOY_NEW(DPAD_LEFT))
        {
            if (sMonSummaryScreen->currPageIndex != PSS_PAGE_BATTLE_MOVES)
            {
                move = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
                gTasks[taskId].func = Task_HandleReplaceMoveInput;
                ChangePage(taskId, -1);
            }
        }
        else if (JOY_NEW(DPAD_RIGHT))
        {
            if (sMonSummaryScreen->currPageIndex != PSS_PAGE_CONTEST_MOVES)
            {
                move = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
                gTasks[taskId].func = Task_HandleReplaceMoveInput;
                ChangePage(taskId, 1);
            }
        }
#endif
        else if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            move = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
            PrintMoveDescription(move);
            ScheduleBgCopyTilemapToVram(0);
            gTasks[taskId].func = Task_HandleReplaceMoveInput;
        }
    }
}

u8 GetMoveSlotToReplace_SwSh(void)
{
    return sMoveSlotToReplace;
}

#define tMoveTaskState data[2]
#define tMosaicStrength data[3]

static void Task_ShowEffectTilemap(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    // set mosaic effect and appropriate tilemap buffer
    if (tMoveTaskState == 0)
    {
        tMosaicStrength = 6;
        SetGpuReg(REG_OFFSET_MOSAIC, (tMosaicStrength & 15) * 17);
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
            SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONTEST]);
        else
            SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_BATTLE]);
        ScheduleBgCopyTilemapToVram(1);
        ShowBg(1);
        SetGpuRegBits(REG_OFFSET_BG1CNT, BGCNT_MOSAIC);
        tMoveTaskState++;
    }
    // attenuate mosaic effect
    else if (tMoveTaskState <= 4)
    {
        tMosaicStrength -= 1;
        SetGpuReg(REG_OFFSET_MOSAIC, (tMosaicStrength & 15) * 17);
        tMoveTaskState++;
    }
    // finish mosaic effect and print text
    else
    {
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        ClearGpuRegBits(REG_OFFSET_BG1CNT, BGCNT_MOSAIC);
        PrintMoveDescription(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]);
        SetNewMoveTypeIcon();

        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            PrintNewMoveDetailsOrCancelText();
            u8 i;
            u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (4 * MOVE_SLOT_SPRITES_COUNT)];
            for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
            {
                if (spriteIds[i] != MAX_SPRITES)
                    gSprites[spriteIds[i]].invisible = FALSE;
            }
        }
        UpdateMoveSlotPalette();

        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
            PutWindowTilemap(PSS_LABEL_WINDOW_MOVES_POWER_ACC);

        DestroyTask(taskId);
    }
}

static void Task_HideEffectTilemap(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 windowId;

    // wait for showing task to finish
    if (FuncIsActiveTask(Task_ShowEffectTilemap))
        return;

    // clean up the windows and text on the first pass
    // set up mosaic effect
    if (tMoveTaskState == 0)
    {
        tMosaicStrength = 0;
        SetGpuReg(REG_OFFSET_MOSAIC, (tMosaicStrength & 15) * 17);
        SetGpuRegBits(REG_OFFSET_BG1CNT, BGCNT_MOSAIC);

        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
            ClearWindowTilemap(PSS_LABEL_WINDOW_MOVES_POWER_ACC);

        windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_DESCRIPTION);
        ClearWindowTilemap(windowId);
        ClearCancelText();

        if (SWSH_SUMMARY_CATEGORY_ICONS)
            DestroyCategoryIcon();

        u8 i;
        u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (4 * MOVE_SLOT_SPRITES_COUNT)];
        for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
        {
            if (spriteIds[i] != MAX_SPRITES)
                gSprites[spriteIds[i]].invisible = TRUE;
        }
        UpdateMoveSlotPalette();

        tMoveTaskState++;
    }
    // dial up mosaic effect over time
    else if (tMoveTaskState < 5)
    {
        tMosaicStrength++;
        SetGpuReg(REG_OFFSET_MOSAIC, (tMosaicStrength & 15) * 17);
        tMoveTaskState++;
    }
    // hide BG and clear mosaic effect
    else
    {
        HideBg(1);
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        ClearGpuRegBits(REG_OFFSET_BG1CNT, BGCNT_MOSAIC);
        PrintMovesPagePrompt();
        DestroyTask(taskId);
    }
}

#undef tMoveTaskState
#undef tMosaicStrength

#define EXP_BAR_TILEMAP_START 519
#define EXP_BAR_TILE_EMPTY    368
#define EXP_BAR_TILE_FULL     376
#define EXP_BAR_TILE_COUNT    12

static void DrawExperienceProgressBar(struct Pokemon *unused)
{
    s64 numExpProgressBarTicks;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u16 *dst;
    u8 i;

    if (summary->level < MAX_LEVEL)
    {
        u32 expBetweenLevels = gExperienceTables[gSpeciesInfo[summary->species].growthRate][summary->level + 1] - gExperienceTables[gSpeciesInfo[summary->species].growthRate][summary->level];
        u32 expSinceLastLevel = summary->exp - gExperienceTables[gSpeciesInfo[summary->species].growthRate][summary->level];

        numExpProgressBarTicks = expSinceLastLevel * (EXP_BAR_TILE_COUNT * 8) / expBetweenLevels;
        if (numExpProgressBarTicks == 0 && expSinceLastLevel != 0)
            numExpProgressBarTicks = 1;
    }
    else
    {
        numExpProgressBarTicks = 0;
    }

    dst = &sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_MEMO][EXP_BAR_TILEMAP_START];
    for (i = 0; i < EXP_BAR_TILE_COUNT; i++)
    {
        if (numExpProgressBarTicks > (8 - 1))
            dst[i] = EXP_BAR_TILE_FULL;
        else
            dst[i] = EXP_BAR_TILE_EMPTY + (numExpProgressBarTicks % 8);
        numExpProgressBarTicks -= 8;
        if (numExpProgressBarTicks < 0)
            numExpProgressBarTicks = 0;
    }

    ScheduleBgCopyTilemapToVram(2);
}

static void ResetWindows(void)
{
    u8 i;

    InitWindows(sSummaryTemplate);
    DeactivateAllTextPrinters();
    for (i = 0; i < PSS_LABEL_WINDOW_END; i++)
        FillWindowPixelBuffer(i, PIXEL_FILL(0));
    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
        sMonSummaryScreen->windowIds[i] = WINDOW_NONE;
}

static void PrintTextOnWindowWithFont(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId, u32 fontId)
{
    AddTextPrinterParameterized4(windowId, fontId, x, y, 0, lineSpacing, sTextColors[colorId], 0, string);
}

static void PrintTextOnWindow(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId)
{
    PrintTextOnWindowWithFont(windowId, string, x, y, lineSpacing, colorId, PSS_DEFAULT_FONT);
}

static UNUSED void PrintTextOnWindowToFitPx_WithFont(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId, u32 fontId, u32 width)
{
    u32 font = GetFontIdToFit(string, fontId, 0, width);
    PrintTextOnWindowWithFont(windowId, string, x, y, lineSpacing, colorId, font);
}

static void PrintMonPortraitInfo(void)
{
    FillWindowPixelBuffer(PSS_LABEL_WINDOW_PORTRAIT_INFO, PIXEL_FILL(0));
    if (!sMonSummaryScreen->summary.isEgg)
        PrintNotEggInfo();
    else
        PrintEggInfo();
    ScheduleBgCopyTilemapToVram(0);
}

static void PrintNotEggInfo(void)
{
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u8 statusAnim = GetMonAilment(&sMonSummaryScreen->currentMon);

    // print nickname
    GetMonNickname(mon, gStringVar1);
    PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_INFO, gStringVar1, 6, 1, 0, 1);

    // See CreateGenderSprite function, not using gender symbols (text)
    // PrintGenderSymbol(mon, summary->species2);

    // print level only if no status condition
    if (statusAnim == 0)
    {
        // Convert level number to string
        ConvertIntToDecimalStringN(gStringVar2, summary->level, STR_CONV_MODE_LEFT_ALIGN, 3);

        // Print "Lv." with FONT_NORMAL
        PrintTextOnWindowWithFont(PSS_LABEL_WINDOW_PORTRAIT_INFO, sText_Lv, 74, 1, 0, 1, FONT_SMALL_NARROWER);
        // Print the level number with FONT_SHORT_NARROW, positioned after "Lv." (10) + space (1)
        PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_INFO, gStringVar2, 74 + 10 + 1, 1, 0, 1);
    }

    PutWindowTilemap(PSS_LABEL_WINDOW_PORTRAIT_INFO);
}

static void PrintEggStepsRemaining(void)
{
    u32 eggCycles = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_FRIENDSHIP);
    u32 stepsInCurrentCycle = gSaveBlock1Ptr->daycare.stepCounter;

    #if P_EGG_CYCLE_LENGTH >= GEN_8
        u32 stepsPerCycle = 128;
    #elif P_EGG_CYCLE_LENGTH == GEN_5 || P_EGG_CYCLE_LENGTH == GEN_6
        u32 stepsPerCycle = 257;
    #elif P_EGG_CYCLE_LENGTH == GEN_4
        u32 stepsPerCycle = 255;
    #else // GEN_3 or GEN_7
        u32 stepsPerCycle = 256;
    #endif

    // Handle fast-hatching abilities like Flame Body, etc.
    u8 cyclesToSubtract = GetEggCyclesToSubtract();
    u32 stepsRemaining;

    // Calculate how many actual step cycles are needed
    // 1. Round up division
    // 2. Add 1 for the last cycle (GF Jank?)
    u32 cyclesNeeded = ((eggCycles + cyclesToSubtract - 1) / cyclesToSubtract) + 1;
    stepsRemaining = ((cyclesNeeded) * stepsPerCycle) - stepsInCurrentCycle;

    ConvertIntToDecimalStringN(gStringVar1, stepsRemaining, STR_CONV_MODE_LEFT_ALIGN, 5);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, gStringVar1);
    DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar1, sEggStepsLayout);

    u8 stringXPos = GetStringRightAlignXOffset(PSS_DEFAULT_FONT, gStringVar1, 80) + 24;
    PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_INFO, gStringVar1, stringXPos, 1, 0, 1);
}

static void PrintEggInfo(void)
{
    // GetMonNickname(&sMonSummaryScreen->currentMon, gStringVar1);
    StringCopy(gStringVar1, sText_Egg);
    PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_INFO, gStringVar1, 4, 1, 0, 1);
    PrintEggStepsRemaining();
    PutWindowTilemap(PSS_LABEL_WINDOW_PORTRAIT_INFO);
}

// Functions
static void LoadGenderGfx(void)
{
    if (GetSpriteTileStartByTag(TAG_GENDER_ICON) == 0xFFFF)
    {
        LoadCompressedSpriteSheet(&sSpriteSheet_GenderIcons);
    }
}

static void CreateGenderSprite(struct Pokemon *mon, enum Species species)
{
    u8 *spriteId = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_GENDER];
    u8 gender;

    if (*spriteId == SPRITE_NONE)
    {
        LoadGenderGfx();
        *spriteId = CreateSprite(&sSpriteTemplate_GenderIcons, 231, 17, 6);
    }

    if (species != SPECIES_NIDORAN_M && species != SPECIES_NIDORAN_F && !sMonSummaryScreen->summary.isEgg)
    {
        gender = GetMonGender(mon);
        if (gender == MON_MALE || gender == MON_FEMALE)
        {
            u8 animIndex = (gender == MON_MALE) ? 1 : 0;
            StartSpriteAnim(&gSprites[*spriteId], animIndex);
            SetSpriteInvisibility(SPRITE_ARR_ID_GENDER, FALSE);
        }
        else
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_GENDER, TRUE);
        }
    }
    else
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_GENDER, TRUE);
    }
}

static u8 GetButtonTextOffset(u8 buttonType)
{
    static const u8 sButtonTextOffset[] = {
        [BUTTON_A]     = 11,   // 8px + 3px
        [BUTTON_B]     = 11,   // 8px + 3px
        [BUTTON_START] = 26,   // (32 - 9)px + 3px
        [BUTTON_LR]    = 19,   // 16px + 3px
    };
    return sButtonTextOffset[buttonType];
}

static void PrintButtonIcon(u8 windowId, u8 buttonType, u32 x, u32 y)
{
    static const struct {
        u8 width;
        u8 height;
    } sButtonDimensions[] = {
        [BUTTON_A]      = {8, 8},
        [BUTTON_B]      = {8, 8},
        [BUTTON_START]  = {32, 8},
        [BUTTON_LR]     = {16, 8},
    };

    const u8 *button = sButtons_Gfx[buttonType];
    u8 width = sButtonDimensions[buttonType].width;
    u8 height = sButtonDimensions[buttonType].height;

    BlitBitmapToWindow(windowId, button, x, y, width, height);
}

static void PrintPagePrompts(void)
{
    ShowCancelOrRenamePrompt();
    PrintRightAlignedPrompt(PSS_LABEL_WINDOW_PROMPT_SWITCH, BUTTON_A, sText_Switch, 60, 0);

    if (SWSH_SUMMARY_SHOW_IV_EV)
        DrawNextSkillsButtonPrompt(SKILL_STATE_STATS);

    if (IS_MOVE_PAGE(sMonSummaryScreen->currPageIndex)
        && sMonSummaryScreen->mode != SUMMARY_MODE_SELECT_MOVE)
    {
        UpdateMoveRelearnerState(FALSE);
        PrintMovesPagePrompt();
    }
}

static void PutPageWindowTilemaps(u8 page)
{
    u8 i;

    switch (page)
    {
    case PSS_PAGE_INFO:
        PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_CANCEL);
        break;
    case PSS_PAGE_SKILLS:
        PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_IV_EV_STATS);
        break;
    case PSS_PAGE_BATTLE_MOVES:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            if (sMonSummaryScreen->newMove != MOVE_NONE || sMonSummaryScreen->firstMoveIndex != MAX_MON_MOVES)
                PutWindowTilemap(PSS_LABEL_WINDOW_MOVES_POWER_ACC);
        }
        else
        {
            UpdateMoveRelearnerState(FALSE);
            PrintMovesPagePrompt();
        }
        break;
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    case PSS_PAGE_CONDITIONS:
        break;
#endif
    case PSS_PAGE_CONTEST_MOVES:
        if (sMonSummaryScreen->mode != SUMMARY_MODE_SELECT_MOVE)
        {
            UpdateMoveRelearnerState(FALSE);
            PrintMovesPagePrompt();
        }
        break;
    case PSS_PAGE_MEMO:
        break;
    }

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
        PutWindowTilemap(sMonSummaryScreen->windowIds[i]);

    ScheduleBgCopyTilemapToVram(0);
}

static void ClearPageWindowTilemaps(u8 page)
{
    u8 i;

    switch (page)
    {
    case PSS_PAGE_INFO:
        ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_CANCEL);
        break;
    case PSS_PAGE_SKILLS:
        if (SWSH_SUMMARY_SHOW_IV_EV)
            ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_IV_EV_STATS);
        break;
    case PSS_PAGE_BATTLE_MOVES:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            if (sMonSummaryScreen->newMove != MOVE_NONE || sMonSummaryScreen->firstMoveIndex != MAX_MON_MOVES)
                ClearWindowTilemap(PSS_LABEL_WINDOW_MOVES_POWER_ACC);
        }
        else
        {
            ClearMovesPagePrompt();
        }
        break;
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    case PSS_PAGE_CONDITIONS:
        ScanlineEffect_Clear();
        HideBg(1);
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON);
        if (SWSH_SUMMARY_MON_SHADOWS)
        {
            u8 shadowId = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW];
            LoadPalette(sMonShadow_Pal, OBJ_PLTT_ID(gSprites[shadowId].oam.paletteNum), PLTT_SIZE_4BPP);
        }
        if (SWSH_SUMMARY_BG_BLEND || SWSH_SUMMARY_MON_SHADOWS)
        {
            if (SWSH_SUMMARY_BG_BLEND)
                SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG3 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG1);
            else
                SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG3 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND);
            SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(14, 6));
        }
        else
        {
            SetGpuReg(REG_OFFSET_BLDCNT, 0);
            SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        }
        break;
#endif
    case PSS_PAGE_CONTEST_MOVES:
        if (sMonSummaryScreen->mode != SUMMARY_MODE_SELECT_MOVE)
        {
            ClearMovesPagePrompt();
        }
        break;
    case PSS_PAGE_MEMO:
        break;
    }

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
        RemoveWindowByIndex(i);

    ScheduleBgCopyTilemapToVram(0);
}

static u8 AddWindowFromTemplateList(const struct WindowTemplate *template, u8 templateId)
{
    u8 *windowIdPtr = &sMonSummaryScreen->windowIds[templateId];
    if (*windowIdPtr == WINDOW_NONE)
    {
        *windowIdPtr = AddWindow(&template[templateId]);
        FillWindowPixelBuffer(*windowIdPtr, PIXEL_FILL(0));
    }
    return *windowIdPtr;
}

static void RemoveWindowByIndex(u8 windowIndex)
{
    u8 *windowIdPtr = &sMonSummaryScreen->windowIds[windowIndex];
    if (*windowIdPtr != WINDOW_NONE)
    {
        ClearWindowTilemap(*windowIdPtr);
        RemoveWindow(*windowIdPtr);
        *windowIdPtr = WINDOW_NONE;
    }
}

static void PrintPageSpecificText(u8 pageIndex)
{
    u16 i;
    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
    {
        if (sMonSummaryScreen->windowIds[i] != WINDOW_NONE)
            FillWindowPixelBuffer(sMonSummaryScreen->windowIds[i], PIXEL_FILL(0));
    }
    sTextPrinterFunctions[pageIndex]();
}

static void CreateTextPrinterTask(u8 pageIndex)
{
    CreateTask(sTextPrinterTasks[pageIndex], 16);
}

static void PrintInfoPageText(void)
{
    if (sMonSummaryScreen->summary.isEgg)
    {
        PrintMonDexNumberSpecies();
        PrintEggState();
    }
    else
    {
        PrintMonOTName();
        PrintMonOTID();
        PrintMonDexNumberSpecies();
        PrintHeldItemInfo();
        PrintMonNature();
    }
}

static void Task_PrintInfoPage(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    switch (data[0])
    {
    case 1:
        PrintMonOTName();
        break;
    case 2:
        PrintMonOTID();
        break;
    case 3:
        PrintMonDexNumberSpecies();
        break;
    case 4:
        PrintHeldItemInfo();
        break;
    case 5:
        PrintMonNature();
        break;
    case 6:
        DestroyTask(taskId);
        return;
    }
    data[0]++;
}

static void PrintMonDexNumberSpecies(void)
{
    int windowId;
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u16 dexNum = SpeciesToPokedexNum(summary->species); // not printing pokedex number

    windowId = AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_SPECIES);


    if (sMonSummaryScreen->summary.isEgg)
    {
        PrintTextOnWindow(windowId, gText_FiveMarks, 0, 5, 0, 0);
    }
    else
    {
        PrintTextOnWindow(windowId, GetSpeciesName(summary->species2), 0, 5, 0, 0);

        if (dexNum != 0xFFFF)
        {
            u8 digitCount = (NATIONAL_DEX_COUNT > 999 && IsNationalPokedexEnabled()) ? 4 : 3;

            StringCopy(gStringVar1, &gText_NumberClear01[0]);
            ConvertIntToDecimalStringN(gStringVar2, dexNum, STR_CONV_MODE_LEADING_ZEROS, digitCount);
            StringAppend(gStringVar1, gStringVar2);
            ConvertIntToDecimalStringN(gStringVar1, dexNum, STR_CONV_MODE_LEADING_ZEROS, digitCount);

            if (!IsMonShiny(mon))
            {
                PrintTextOnWindow(windowId, gStringVar1, 76, 5, 0, 0);
            }
            else
            {
                PrintTextOnWindow(windowId, gStringVar1, 76, 5, 0, 2);
            }
        }
    }

}

static void PrintMonOTName(void)
{
    int windowId = AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_SPECIES);
    if (InBattleFactory() != TRUE && InSlateportBattleTent() != TRUE)
    {
        PrintTextOnWindow(windowId, sMonSummaryScreen->summary.OTName, 0, 37, 0, 0);
    }
    else
    {
        StringCopy(gStringVar1, sText_RentalPkmn);
        PrintTextOnWindow(windowId, gStringVar1, 0, 37, 0, 0);
    }
}

static void PrintMonOTID(void)
{
    if (InBattleFactory() != TRUE && InSlateportBattleTent() != TRUE)
    {
        ConvertIntToDecimalStringN(gStringVar1, (u16)sMonSummaryScreen->summary.OTID, STR_CONV_MODE_LEADING_ZEROS, 5);
        PrintTextOnWindow(AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_SPECIES), gStringVar1, 71, 37, 0, 0);
    }
}

static void PrintMonNature(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    u8 windowId = AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_SPECIES);

    DynamicPlaceholderTextUtil_Reset();

    if (sum->mintNature != sum->nature)
    {
        BufferNatureString(1);
        DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sText_MintNature);
    }
    else
    {
        BufferNatureString(0);
        DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sText_Nature);
    }

    PrintTextOnWindow(windowId, gStringVar4, 0, 53, 0, 0);
}

static void PrintMonAbilityName(void)
{
    enum Ability ability = GetAbilityBySpecies(sMonSummaryScreen->summary.species, sMonSummaryScreen->summary.abilityNum);
    u8 y = SWSH_SUMMARY_SHOW_DYNAMAX_LEVEL ? 3 : 1;

    PrintTextOnWindow(AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_ABILITY), gAbilitiesInfo[ability].name, 48, y, 0, 0);
}

static void PrintMonAbilityDescription(void)
{
    enum Ability ability = GetAbilityBySpecies(sMonSummaryScreen->summary.species, sMonSummaryScreen->summary.abilityNum);
    u8 y = SWSH_SUMMARY_SHOW_DYNAMAX_LEVEL ? 22 : 20;

    PrintTextOnWindow(AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_ABILITY), gAbilitiesInfo[ability].description, 0, y, 0, 0);
}

static const u8 *GetCharacteristicString(void)
{
    u8 ivs[6];
    u8 i;
    u8 max = 0;
    u8 tieMask = 0;
    u8 startIdx;
    u8 chosenIdx = 0xFF;

    ivs[0] = sMonSummaryScreen->summary.ivHp;
    ivs[1] = sMonSummaryScreen->summary.ivAtk;
    ivs[2] = sMonSummaryScreen->summary.ivDef;
    ivs[3] = sMonSummaryScreen->summary.ivSpeed;
    ivs[4] = sMonSummaryScreen->summary.ivSpatk;
    ivs[5] = sMonSummaryScreen->summary.ivSpdef;

    for (i = 0; i < 6; i++)
    {
        if (ivs[i] > max)
        {
            max = ivs[i];
            tieMask = (1 << i);
        }
        else if (ivs[i] == max)
        {
            tieMask |= (1 << i);
        }
    }

    if ((tieMask & (tieMask - 1)) == 0)
    {
        for (i = 0; i < 6; i++)
            if (tieMask & (1 << i))
            {
                chosenIdx = i;
                break;
            }
    }
    else
    {
        startIdx = (u8)(sMonSummaryScreen->summary.pid % 6);
        for (i = 0; i < 6; i++)
        {
            u8 idx = (startIdx + i) % 6;
            if (tieMask & (1 << idx))
            {
                chosenIdx = idx;
                break;
            }
        }
    }

    if (chosenIdx == 0xFF)
        return sText_Empty;

    u8 ivMod5 = ivs[chosenIdx] % 5;
    u16 index = (u16)chosenIdx * 5 + ivMod5;
    if (index >= MON_CHARACTERISTIC_COUNT)
        return sText_Empty;
    return sMonCharacteristicTable[index];
}

static void BufferNatureString(u8 useMint)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;

    DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, sMemoNatureTextColor);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(1, sMemoMiscTextColor);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(2, useMint ? gNaturesInfo[sum->mintNature].name : gNaturesInfo[sum->nature].name);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(5, gText_EmptyString5);
}

static void BufferMonNatureMemo(void)
{
    BufferNatureString(0);
    DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sText_MemoNature);
}

static void BufferMonEncounter(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    const u8 *text;
    bool32 locationFound = sum->metLocation < MAPSEC_NONE;
    u8 *metLevelString = Alloc(32);
    u8 *metLocationString = Alloc(32);

    GetMetLevelString(metLevelString);
    if (DoesMonOTMatchOwner() == TRUE && sum->metLevel == 0)
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(3, gText_EmptyString5);

    if (locationFound)
    {
        GetMapNameHandleAquaHideout(metLocationString, sum->metLocation);
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(4, metLocationString);
    }

    text = sText_MemoNature;

    if (DoesMonOTMatchOwner() == TRUE)
    {
        if (sum->metLevel == 0)
            text = (!locationFound) ? sText_MemoHatchedSomewhere : sText_MemoHatched;
        else
            text = (!locationFound) ? sText_MemoMetSomewhere : sText_MemoMet;
    }
    else if (sum->metLocation == METLOC_FATEFUL_ENCOUNTER)
    {
        text = sText_MemoFateful;
    }
    else if (sum->metLocation != METLOC_IN_GAME_TRADE && DidMonComeFromGBAGames())
    {
        text = (!locationFound) ? sText_MemoTraded : sText_MemoProbablyMet;
    }
    else
    {
        text = sText_MemoTraded;
    }

    DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar3, text);

    Free(metLevelString);
    Free(metLocationString);
}




static void BufferMonTrainerMemo(void)
{
    DynamicPlaceholderTextUtil_Reset();
    BufferMonNatureMemo();

    if (InBattleFactory() == TRUE || InSlateportBattleTent() == TRUE || IsInGamePartnerMon() == TRUE)
    {
        gStringVar3[0] = 0;
        return;
    }

    BufferMonEncounter();
}

static void PrintMonNatureMemo(void)
{
    u8 windowId = AddWindowFromTemplateList(sPageMemoTemplate, PSS_DATA_WINDOW_MEMO_NOTE);
    PrintTextOnWindow(windowId, gStringVar4, 8, 0, 0, 0);
}

static void PrintMonEncounter(void)
{
    u8 windowId = AddWindowFromTemplateList(sPageMemoTemplate, PSS_DATA_WINDOW_MEMO_NOTE);
    const u8 *buf = (gStringVar3[0] != 0) ? gStringVar3 : sText_Empty;
    PrintTextOnWindow(windowId, buf, 8, 19, 0, 0);
}

static void PrintMonCharacteristic(void)
{
    u8 windowId = AddWindowFromTemplateList(sPageMemoTemplate, PSS_DATA_WINDOW_MEMO_NOTE);
    const u8 *text = GetCharacteristicString();
    u8 y = (gStringVar3[0] != 0) ? 53 : 19; // if no encounter, move the characteristic up
    PrintTextOnWindow(windowId, text, 8, y, 0, 0);
}

static void GetMetLevelString(u8 *output)
{
    u8 level = sMonSummaryScreen->summary.metLevel;
    if (level == 0)
        level = EGG_HATCH_LEVEL;
    ConvertIntToDecimalStringN(output, level, STR_CONV_MODE_LEFT_ALIGN, 3);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(3, output);
}

static bool8 DoesMonOTMatchOwner(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    u32 trainerId;
    u8 gender;

    if (sMonSummaryScreen->monList.mons == gParties[B_TRAINER_OPPONENT_A])
    {
        u8 multiID = GetMultiplayerId() ^ 1;
        trainerId = gLinkPlayers[multiID].trainerId & 0xFFFF;
        gender = gLinkPlayers[multiID].gender;
        StringCopy(gStringVar1, gLinkPlayers[multiID].name);
    }
    else
    {
        trainerId = GetPlayerIDAsU32() & 0xFFFF;
        gender = gSaveBlock2Ptr->playerGender;
        StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
    }

    if (gender != sum->OTGender || trainerId != (sum->OTID & 0xFFFF) || StringCompareWithoutExtCtrlCodes(gStringVar1, sum->OTName))
        return FALSE;
    else
        return TRUE;
}

static bool8 DidMonComeFromGBAGames(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    if (sum->metGame > 0 && sum->metGame <= VERSION_LEAF_GREEN)
        return TRUE;
    return FALSE;
}

static bool8 IsInGamePartnerMon(void)
{
    if (gPartyMenu.layout == PARTY_LAYOUT_MULTI_FULL)
    {
        return FALSE;
    }
    else if (gPartyMenu.layout == PARTY_LAYOUT_MULTI_FULL_PARTNER)
    {
        return TRUE;
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) && gMain.inBattle)
    {
        if (sMonSummaryScreen->curMonIndex == 1 || sMonSummaryScreen->curMonIndex == 4 || sMonSummaryScreen->curMonIndex == 5)
            return TRUE;
    }
    return FALSE;
}

static void PrintEggState(void)
{
    const u8 *text;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;

    // FillWindowPixelBuffer(PSS_DATA_WINDOW_INFO_ITEM, PIXEL_FILL(1));

    if (sMonSummaryScreen->summary.sanity == TRUE)
        text = gText_EggWillTakeALongTime;
    else if (sum->friendship <= 5)
        text = gText_EggAboutToHatch;
    else if (sum->friendship <= 10)
        text = gText_EggWillHatchSoon;
    else if (sum->friendship <= 40)
        text = gText_EggWillTakeSomeTime;
    else
        text = gText_EggWillTakeALongTime;
    text = gText_EmptyString5;

    PrintTextOnWindow(AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_ITEM), text, 16, 4, 0, 0);
}

static void PrintSkillsPageText(void)
{
    PrintMonAbilityName();
    PrintMonAbilityDescription();
    PrintStats(SKILL_STATE_STATS);
}

// Update the task function to include stat label printing as a separate step
static void Task_PrintSkillsPage(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 1:
        PrintMonAbilityName();
        break;
    case 2:
        PrintMonAbilityDescription();
        break;
    case 3:
        PrintStats(SKILL_STATE_STATS);
        break;
    case 4:
        DestroyTask(taskId);
        return;
    }
    data[0]++;
}

static void PrintHeldItemInfo(void)
{
    const u8 *text;
    const u8 *description;
    u8 desc[200];
    u32 fontId;
    u8 windowId = AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_ITEM);

    if (sMonSummaryScreen->summary.item == ITEM_ENIGMA_BERRY_E_READER
        && IsMultiBattle() == TRUE
        && (sMonSummaryScreen->curMonIndex == 1 || sMonSummaryScreen->curMonIndex == 4 || sMonSummaryScreen->curMonIndex == 5))
    {
        text = GetItemName(ITEM_ENIGMA_BERRY_E_READER);
        description = GetItemDescription(ITEM_ENIGMA_BERRY_E_READER);
    }
    else if (sMonSummaryScreen->summary.item == ITEM_NONE)
    {
        text = sText_Empty;
        description = sText_Empty;
    }
    else
    {
        CopyItemName(sMonSummaryScreen->summary.item, gStringVar1);
        text = gStringVar1;
        description = GetItemDescription(sMonSummaryScreen->summary.item);
    }

    fontId = GetFontIdToFit(text, PSS_DEFAULT_FONT, 0, 72);
    PrintTextOnWindowWithFont(windowId, text, 74, 5, 0, 0, fontId);

    fontId = FormatTextByWidth(desc, 144, PSS_DEFAULT_FONT, description, GetFontAttribute(PSS_DEFAULT_FONT, FONTATTR_LETTER_SPACING));
    PrintTextOnWindowWithFont(windowId, desc, 0, 23, 1, 0, fontId);
}

static void BufferStat(u8 *dst, u32 stat, u32 strId, u32 align)
{
    // Simplified BufferStat that only converts to string, no nature coloring
    ConvertIntToDecimalStringN(dst, stat, STR_CONV_MODE_RIGHT_ALIGN, align);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(strId, dst);
}


static void PrintStats(u8 mode)
{
    u16 hp, hp2, atk, def, spA, spD, spe;
    u8 windowId;

    FillWindowPixelBuffer(sMonSummaryScreen->windowIds[PSS_DATA_WINDOW_SKILLS_STATS], 0);

    switch (mode)
    {
    case SKILL_STATE_STATS:
    default:
        hp = sMonSummaryScreen->summary.currentHP;
        hp2 = sMonSummaryScreen->summary.maxHP;
        atk = sMonSummaryScreen->summary.atk;
        def = sMonSummaryScreen->summary.def;
        spA = sMonSummaryScreen->summary.spatk;
        spD = sMonSummaryScreen->summary.spdef;
        spe = sMonSummaryScreen->summary.speed;
        break;
    case SKILL_STATE_IVS:
        hp = sMonSummaryScreen->summary.ivHp;
        atk = sMonSummaryScreen->summary.ivAtk;
        def = sMonSummaryScreen->summary.ivDef;
        spA = sMonSummaryScreen->summary.ivSpatk;
        spD = sMonSummaryScreen->summary.ivSpdef;
        spe = sMonSummaryScreen->summary.ivSpeed;
        break;
    case SKILL_STATE_EVS:
        hp = sMonSummaryScreen->summary.evHp;
        atk = sMonSummaryScreen->summary.evAtk;
        def = sMonSummaryScreen->summary.evDef;
        spA = sMonSummaryScreen->summary.evSpatk;
        spD = sMonSummaryScreen->summary.evSpdef;
        spe = sMonSummaryScreen->summary.evSpeed;
        break;
    }

    PrintStatLabels();
    windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_STATS);

    if (mode == SKILL_STATE_STATS)
    {
        u8 currentHPString[8];
        u8 maxHPString[8];
        ConvertIntToDecimalStringN(currentHPString, hp, STR_CONV_MODE_RIGHT_ALIGN, 3);
        ConvertIntToDecimalStringN(maxHPString, hp2, STR_CONV_MODE_RIGHT_ALIGN, 3);
        DynamicPlaceholderTextUtil_Reset();
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, currentHPString);
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(1, maxHPString);
        DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sStatsHPLayout);
        AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 72 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar4, 0), 2, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, gStringVar4);
    }
    else
    {
        u8 hpString[8];
        BufferStat(hpString, hp, 0, 7);
        DynamicPlaceholderTextUtil_Reset();
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, hpString);
        DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sStatsHPIVEVLayout);
        AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 72 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar4, 0), 2, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, gStringVar4);
    }

    // print other stats
    BufferStat(gStringVar1, atk, 0, 3);
    AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 144 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar1, 0), 2, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, gStringVar1);

    BufferStat(gStringVar2, def, 1, 3);
    AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 72 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar2, 0), 19, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, gStringVar2);

    BufferStat(gStringVar3, spA, 2, 3);
    AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 144 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar3, 0), 19, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, gStringVar3);

    BufferStat(gStringVar4, spD, 3, 3);
    AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 72 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar4, 0), 36, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, gStringVar4);

    BufferStat(sStringVar5, spe, 4, 3);
    AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, 144 - GetStringWidth(PSS_DEFAULT_FONT, sStringVar5, 0), 36, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, sStringVar5);

    // Now copy everything to VRAM in one operation
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

// Add a function to just print the stat labels
static void PrintStatLabels(void)
{
    static const u8 sTextNatureDown[] = _("{COLOR}{07}{SHADOW}{08}");
    static const u8 sTextNatureUp[] = _("{COLOR}{05}{SHADOW}{06}");
    static const u8 sTextNatureNeutral[] = _("{COLOR}{01}");

    struct StatLabelInfo {
        u8 statIndex;
        const u8 *text;
        u8 x;
        u8 y;
    };

    static const struct StatLabelInfo sStatLabels[] = {
        {STAT_HP,     sText_HP_Title,      8,  2},
        {STAT_ATK,    sText_Attack_Title,  80, 2},
        {STAT_DEF,    sText_Defense_Title, 8,  19},
        {STAT_SPATK,  sText_SpAtk_Title,   80, 19},
        {STAT_SPDEF,  sText_SpDef_Title,   8,  36},
        {STAT_SPEED,  sText_Speed_Title,   80, 36},
    };

    u8 windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_STATS);
    u8 coloredLabel[64];
    u8 natureUpStat = gNaturesInfo[sMonSummaryScreen->summary.mintNature].statUp;
    u8 natureDownStat = gNaturesInfo[sMonSummaryScreen->summary.mintNature].statDown;
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sStatLabels); i++)
    {
        const u8 *color;

        if (!SWSH_SUMMARY_NATURE_COLORS || natureUpStat == natureDownStat)
            color = sTextNatureNeutral;
        else if (sStatLabels[i].statIndex == natureUpStat)
            color = sTextNatureUp;
        else if (sStatLabels[i].statIndex == natureDownStat)
            color = sTextNatureDown;
        else
            color = sTextNatureNeutral;

        StringCopy(coloredLabel, color);
        StringAppend(coloredLabel, sStatLabels[i].text);
        AddTextPrinterParameterized4(windowId, PSS_DEFAULT_FONT, sStatLabels[i].x, sStatLabels[i].y, 0, 0, sTextColors[0], TEXT_SKIP_DRAW, coloredLabel);
    }
}

static void PrintExpPointsNextLevel(void)
{
    u32 expToNextLevel;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    u8 windowId = AddWindowFromTemplateList(sPageMemoTemplate, PSS_DATA_WINDOW_MEMO_EXP);

    // print current exp
    ConvertIntToDecimalStringN(gStringVar1, sum->exp, STR_CONV_MODE_LEFT_ALIGN, 7);
    PrintTextOnWindow(windowId, gStringVar1, 8, 4, 0, 0);

    // print exp to next level
    if (sum->level < MAX_LEVEL)
        expToNextLevel = gExperienceTables[gSpeciesInfo[sum->species].growthRate][sum->level + 1] - sum->exp;
    else
        expToNextLevel = 0;

    ConvertIntToDecimalStringN(gStringVar1, expToNextLevel, STR_CONV_MODE_RIGHT_ALIGN, 5);
    PrintTextOnWindow(windowId, gStringVar1, 104 - GetStringWidth(PSS_DEFAULT_FONT, gStringVar1, 0), 15, 0, 0);
}

static void PrintBattleMoves(void)
{
    PrintMoveNameAndPP(0);
    PrintMoveNameAndPP(1);
    PrintMoveNameAndPP(2);
    PrintMoveNameAndPP(3);

    if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
    {
        PrintNewMoveDetailsOrCancelText();
        if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES)
        {
            if (sMonSummaryScreen->newMove != MOVE_NONE)
                PrintMoveDescription(sMonSummaryScreen->newMove);
        }
        else
        {
            PrintMoveDescription(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]);
        }
    }
}

static void Task_PrintBattleMoves(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 1:
        PrintMoveNameAndPP(0);
        break;
    case 2:
        PrintMoveNameAndPP(1);
        break;
    case 3:
        PrintMoveNameAndPP(2);
        break;
    case 4:
        PrintMoveNameAndPP(3);
        break;
    case 5:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
            PrintNewMoveDetailsOrCancelText();
        break;
    case 6:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES)
                data[1] = sMonSummaryScreen->newMove;
            else
                data[1] = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
        }
        break;
    case 7:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            if (sMonSummaryScreen->newMove != MOVE_NONE || sMonSummaryScreen->firstMoveIndex != MAX_MON_MOVES)
                PrintMoveDescription(data[1]);
        }
        break;
    case 8:
        DestroyTask(taskId);
        return;
    }
    data[0]++;
}

static void PrintMemoPageText(void)
{
    BufferMonTrainerMemo();
    PrintMonNatureMemo();
    PrintMonEncounter();
    PrintMonCharacteristic();
    PrintExpPointsNextLevel();
}

static void Task_PrintMemoPage(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 1:
        BufferMonTrainerMemo();
        break;
    case 2:
        PrintMonNatureMemo();
        break;
    case 3:
        PrintMonEncounter();
        break;
    case 4:
        PrintMonCharacteristic();
        break;
    case 5:
        PrintExpPointsNextLevel();
        break;
    case 6:
        ScheduleBgCopyTilemapToVram(0);
        break;
    case 7:
        DestroyTask(taskId);
        return;
    }
    data[0]++;
}

#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
static const struct ScanlineEffectParams sSummaryConditionGraphScanline =
{
    .dmaDest = &REG_WIN0H,
    .dmaControl = SCANLINE_EFFECT_DMACNT_32BIT,
    .initState = 1,
};
static void SummaryConditionGraph_CalcPositions(u8 *conditions, struct UCoords16 *positions)
{
    u8 lineLength, sinIdx;
    s8 posIdx;
    u16 i;

    // Cool is straight up-and-down (not angled), so no need for Sin
    lineLength = sSummaryConditionToLineLength[*(conditions++)];
    positions[GRAPH_COOL].x = SUMMARY_GRAPH_CENTER_X;
    positions[GRAPH_COOL].y = SUMMARY_GRAPH_CENTER_Y - lineLength;

    sinIdx = 64;
    posIdx = GRAPH_COOL;
    for (i = 1; i < CONDITION_COUNT; i++)
    {
        sinIdx += 51;
        if (--posIdx < 0)
            posIdx = CONDITION_COUNT - 1;

        if (posIdx == GRAPH_CUTE)
            sinIdx++;

        lineLength = sSummaryConditionToLineLength[*(conditions++)];
        positions[posIdx].x = SUMMARY_GRAPH_CENTER_X + ((lineLength * gSineTable[64 + sinIdx]) >> 8);
        positions[posIdx].y = SUMMARY_GRAPH_CENTER_Y - ((lineLength * gSineTable[sinIdx]) >> 8);

        if (posIdx <= GRAPH_CUTE && (lineLength != 32 || posIdx != GRAPH_CUTE))
            positions[posIdx].x++;
    }
}

static void SummaryConditionGraph_InitWindow(void)
{
    // BG1 is the graph fill layer; exclude it from WINOUT (always BG1, so bg param dropped vs ConditionGraph_InitWindow)
    u32 flags = (WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ) & ~(1 << 1);

    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
    SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(0, SUMMARY_GRAPH_CENTER_X));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(SUMMARY_GRAPH_TOP_Y, SUMMARY_GRAPH_BOTTOM_Y));
    SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(SUMMARY_GRAPH_TOP_Y, SUMMARY_GRAPH_BOTTOM_Y));
    SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR);
    SetGpuReg(REG_OFFSET_WINOUT, flags);
}

static void SummaryConditionGraph_CalcLine(struct ConditionGraph *graph, u16 *scanline, struct UCoords16 *pos1, struct UCoords16 *pos2, bool8 dir, u16 *overflowScanline)
{
    u16 i, height, top, bottom, x2;
    u16 *ptr;
    s32 x, xIncrement = 0;

    if (pos1->y < pos2->y)
    {
        top = pos1->y;
        bottom = pos2->y;
        x = pos1->x << 10;
        x2 = pos2->x;
        height = bottom - top;
        if (height != 0)
            xIncrement = ((x2 - pos1->x) << 10) / height;
    }
    else
    {
        bottom = pos1->y;
        top = pos2->y;
        x = pos2->x << 10;
        x2 = pos1->x;
        height = bottom - top;
        if (height != 0)
            xIncrement = ((x2 - pos2->x) << 10) / height;
    }

    height++;
    if (overflowScanline == NULL)
    {
        scanline += (top - SUMMARY_GRAPH_TOP_Y) * 2;
        for (i = 0; i < height; i++)
        {
            scanline[dir] = SHIFT_RIGHT_ADJUSTED(x, 10) + dir;
            x += xIncrement;
            scanline += 2;
        }

        ptr = scanline - 2;
    }
    else if (xIncrement > 0)
    {
        overflowScanline += (top - SUMMARY_GRAPH_TOP_Y) * 2;
        // Less readable than the other loops, but it has to be written this way to match.
        for (i = 0; i < height; overflowScanline[dir] = SHIFT_RIGHT_ADJUSTED(x, 10) + dir, x += xIncrement, overflowScanline += 2, i++)
        {
            if (x >= (SUMMARY_GRAPH_CENTER_X << 10))
                break;
        }

        graph->bottom = top + i;
        scanline += (graph->bottom - SUMMARY_GRAPH_TOP_Y) * 2;
        for (; i < height; i++)
        {
            scanline[dir] = SHIFT_RIGHT_ADJUSTED(x, 10) + dir;
            x += xIncrement;
            scanline += 2;
        }

        ptr = scanline - 2;
    }
    else if (xIncrement < 0)
    {
        scanline += (top - SUMMARY_GRAPH_TOP_Y) * 2;
        for (i = 0; i < height; i++)
        {
            scanline[dir] = SHIFT_RIGHT_ADJUSTED(x, 10) + dir;
            if (x < (SUMMARY_GRAPH_CENTER_X << 10))
            {
                scanline[dir] = SUMMARY_GRAPH_CENTER_X;
                break;
            }
            x += xIncrement;
            scanline += 2;
        }

        graph->bottom = top + i;
        overflowScanline += (graph->bottom - SUMMARY_GRAPH_TOP_Y) * 2;
        for (; i < height; i++)
        {
            overflowScanline[dir] = SHIFT_RIGHT_ADJUSTED(x, 10) + dir;
            x += xIncrement;
            overflowScanline += 2;
        }

        ptr = overflowScanline - 2;
    }
    else
    {
        graph->bottom = top;
        scanline += (top - SUMMARY_GRAPH_TOP_Y) * 2;
        overflowScanline += (top - SUMMARY_GRAPH_TOP_Y) * 2;
        scanline[1] = pos1->x + 1;
        overflowScanline[0] = pos2->x;
        overflowScanline[1] = SUMMARY_GRAPH_CENTER_X;
        return;
    }

    ptr[dir] = dir + x2;
}

static void SummaryConditionGraph_CalcRightHalf(struct ConditionGraph *graph)
{
    u16 i, y, bottom;

    // Calculate Cool -> Beauty line
    if (graph->curPositions[GRAPH_COOL].y < graph->curPositions[GRAPH_BEAUTY].y)
    {
        y = graph->curPositions[GRAPH_COOL].y;
        SummaryConditionGraph_CalcLine(graph, graph->scanlineRight[0], &graph->curPositions[GRAPH_COOL], &graph->curPositions[GRAPH_BEAUTY], TRUE, NULL);
    }
    else
    {
        y = graph->curPositions[GRAPH_BEAUTY].y;
        SummaryConditionGraph_CalcLine(graph, graph->scanlineRight[0], &graph->curPositions[GRAPH_BEAUTY], &graph->curPositions[GRAPH_COOL], FALSE, NULL);
    }

    // Calculate Beauty -> Cute line
    // No need for conditional, positions on the Beauty line are always above the Cute line
    SummaryConditionGraph_CalcLine(graph, graph->scanlineRight[0], &graph->curPositions[GRAPH_BEAUTY], &graph->curPositions[GRAPH_CUTE], TRUE, NULL);

    // Calculate Cute -> Smart line (includes left scanline because this crosses the halfway point)
    i = (graph->curPositions[GRAPH_CUTE].y <= graph->curPositions[GRAPH_SMART].y);
    SummaryConditionGraph_CalcLine(graph, graph->scanlineRight[0], &graph->curPositions[GRAPH_CUTE], &graph->curPositions[GRAPH_SMART], i, graph->scanlineLeft[0]);

    // Clear down to new top
    for (i = SUMMARY_GRAPH_TOP_Y; i < y; i++)
    {
        graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][0] = 0;
        graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][1] = 0;
    }

    for (i = graph->curPositions[GRAPH_COOL].y; i <= graph->bottom; i++)
        graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][0] = SUMMARY_GRAPH_CENTER_X;

    // Clear after new bottom
    bottom = max(graph->bottom, graph->curPositions[GRAPH_CUTE].y);
    for (i = bottom + 1; i <= SUMMARY_GRAPH_BOTTOM_Y; i++)
    {
        graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][0] = 0;
        graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][1] = 0;
    }

    for (i = SUMMARY_GRAPH_TOP_Y; i <= SUMMARY_GRAPH_BOTTOM_Y; i++)
    {
        if (graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][0] == 0
         && graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][1] != 0)
            graph->scanlineRight[i - SUMMARY_GRAPH_TOP_Y][0] = SUMMARY_GRAPH_CENTER_X;
    }
}

static void SummaryConditionGraph_CalcLeftHalf(struct ConditionGraph *graph)
{
    s32 i, y, bottom;

    // Calculate Cool -> Tough line
    if (graph->curPositions[GRAPH_COOL].y < graph->curPositions[GRAPH_TOUGH].y)
    {
        y = graph->curPositions[GRAPH_COOL].y;
        SummaryConditionGraph_CalcLine(graph, graph->scanlineLeft[0], &graph->curPositions[GRAPH_COOL], &graph->curPositions[GRAPH_TOUGH], FALSE, NULL);
    }
    else
    {
        y = graph->curPositions[GRAPH_TOUGH].y;
        SummaryConditionGraph_CalcLine(graph, graph->scanlineLeft[0], &graph->curPositions[GRAPH_TOUGH], &graph->curPositions[GRAPH_COOL], TRUE, NULL);
    }

    // Calculate Tough -> Smart line
    // No need for conditional, positions on the Tough line are always above the Smart line
    SummaryConditionGraph_CalcLine(graph, graph->scanlineLeft[0], &graph->curPositions[GRAPH_TOUGH], &graph->curPositions[GRAPH_SMART], FALSE, NULL);

    // Clear down to new top
    for (i = SUMMARY_GRAPH_TOP_Y; i < y; i++)
    {
        graph->scanlineLeft[i - SUMMARY_GRAPH_TOP_Y][0] = 0;
        graph->scanlineLeft[i - SUMMARY_GRAPH_TOP_Y][1] = 0;
    }

    for (i = graph->curPositions[GRAPH_COOL].y; i <= graph->bottom; i++)
        graph->scanlineLeft[i - SUMMARY_GRAPH_TOP_Y][1] = SUMMARY_GRAPH_CENTER_X;

    // Clear after new bottom
    bottom = max(graph->bottom, graph->curPositions[GRAPH_SMART].y + 1);
    for (i = bottom; i <= SUMMARY_GRAPH_BOTTOM_Y; i++)
    {
        graph->scanlineLeft[i - SUMMARY_GRAPH_TOP_Y][0] = 0;
        graph->scanlineLeft[i - SUMMARY_GRAPH_TOP_Y][1] = 0;
    }

    for (i = 0; i < SUMMARY_GRAPH_HEIGHT; i++)
    {
        if (graph->scanlineLeft[i][0] >= graph->scanlineLeft[i][1])
        {
            graph->scanlineLeft[i][1] = 0;
            graph->scanlineLeft[i][0] = 0;
        }
    }
}

static void SummaryConditionGraph_Draw(struct ConditionGraph *graph)
{
    u16 i;

    if (!graph->needsDraw)
        return;

    SummaryConditionGraph_CalcRightHalf(graph);
    SummaryConditionGraph_CalcLeftHalf(graph);

    for (i = 0; i < SUMMARY_GRAPH_HEIGHT; i++)
    {
        // Draw right half
        gScanlineEffectRegBuffers[1][(i + SUMMARY_GRAPH_TOP_Y - 1) * 2 + 0] = // double assignment
        gScanlineEffectRegBuffers[0][(i + SUMMARY_GRAPH_TOP_Y - 1) * 2 + 0] = (graph->scanlineRight[i][0] << 8) | (graph->scanlineRight[i][1]);
        // Draw left half
        gScanlineEffectRegBuffers[1][(i + SUMMARY_GRAPH_TOP_Y - 1) * 2 + 1] = // double assignment
        gScanlineEffectRegBuffers[0][(i + SUMMARY_GRAPH_TOP_Y - 1) * 2 + 1] = (graph->scanlineLeft[i][0] << 8) | (graph->scanlineLeft[i][1]);
    }

    graph->needsDraw = FALSE;
}

static bool8 SummaryConditionGraph_ResetScanline(struct ConditionGraph *graph)
{
    struct ScanlineEffectParams params;

    switch (graph->scanlineResetState)
    {
    case 0:
        ScanlineEffect_Clear();
        graph->scanlineResetState++;
        return TRUE;
    case 1:
        params = sSummaryConditionGraphScanline;
        ScanlineEffect_SetParams(params);
        graph->scanlineResetState++;
        return FALSE;
    default:
        return FALSE;
    }
}

#define sSparkleId           data[0]
#define sDelayTimer          data[1]
#define sNumExtraSparkles    data[2]
#define sCurSparkleId        data[3]
#define sNextSparkleSpriteId data[5]

static void ShowAllSheenSparkles(struct Sprite *sprite)
{
    u16 i;
    u8 id = sprite->sNextSparkleSpriteId;
    for (i = 0; i < (u16)(sprite->sNumExtraSparkles + 1); i++)
    {
        SeekSpriteAnim(&gSprites[id], 0);
        gSprites[id].invisible = FALSE;
        id = gSprites[id].sNextSparkleSpriteId;
    }
}

static void SpriteCB_SheenSparkle_DoNextAfterDelay(struct Sprite *sprite)
{
    if (++sprite->sDelayTimer > 60)
    {
        u16 i;
        u8 id = sprite->sNextSparkleSpriteId;
        sprite->sDelayTimer = 0;
        for (i = 0; i < (u16)(sprite->sNumExtraSparkles + 1); i++)
        {
            gSprites[id].sDelayTimer = (gSprites[id].sSparkleId * 16) + 1;
            gSprites[id].callback = SpriteCB_SheenSparkle;
            id = gSprites[id].sNextSparkleSpriteId;
        }
    }
}

static void SpriteCB_SheenSparkle_WaitForAllAnim(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        sprite->sDelayTimer = 0;
        sprite->callback = SpriteCB_SheenSparkle_DoNextAfterDelay;
    }
}

static void SpriteCB_SheenSparkle(struct Sprite *sprite)
{
    if (sprite->sDelayTimer != 0)
    {
        if (--sprite->sDelayTimer != 0)
            return;
        SeekSpriteAnim(sprite, 0);
        sprite->invisible = FALSE;
    }

    sprite->x = 54 + 12 * sprite->sSparkleId;
    sprite->y = 140;

    if (sprite->animEnded)
    {
        sprite->invisible = TRUE;
        if (sprite->sCurSparkleId == sprite->sNumExtraSparkles)
        {
            if (sprite->sCurSparkleId == MAX_CONDITION_SPARKLES - 1)
            {
                ShowAllSheenSparkles(sprite);
                sprite->callback = SpriteCB_SheenSparkle_WaitForAllAnim;
            }
            else
            {
                sprite->callback = SpriteCB_SheenSparkle_DoNextAfterDelay;
            }
        }
        else
        {
            sprite->callback = SpriteCallbackDummy;
        }
    }
}

static void InitSummarySparkles(u8 count, struct Sprite **sprites)
{
    u16 i;
    for (i = 0; i < MAX_CONDITION_SPARKLES; i++)
    {
        if (sprites[i] == NULL)
            break;
        sprites[i]->sSparkleId = i;
        sprites[i]->sDelayTimer = (i * 16) + 1;
        sprites[i]->sNumExtraSparkles = count;
        sprites[i]->sCurSparkleId = i;
        if (count != MAX_CONDITION_SPARKLES - 1)
        {
            sprites[i]->callback = SpriteCB_SheenSparkle;
        }
        else
        {
            sprites[i]->x = 54 + 12 * i;
            sprites[i]->y = 140;
            ShowAllSheenSparkles(sprites[i]);
            sprites[i]->callback = SpriteCB_SheenSparkle_WaitForAllAnim;
            sprites[i]->invisible = FALSE;
        }
    }
}

static void CreateSheenSparkleSprites(void)
{
    struct SpriteSheet sheet;
    struct SpritePalette pal;
    u8 sheen = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_SHEEN);
    u8 count = GET_NUM_CONDITION_SPARKLES(sheen);
    u16 i, firstSpriteId = 0;
    struct Sprite **sparkles = sMonSummaryScreen->conditionSparkles;

    LoadConditionSparkle(&sheet, &pal);
    LoadSpriteSheet(&sheet);

    for (i = 0; i < (u16)(count + 1); i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_SheenSparkle, 54 + 12 * i, 140, 0);
        if (spriteId != MAX_SPRITES)
        {
            sparkles[i] = &gSprites[spriteId];
            sparkles[i]->invisible = TRUE;
            if (i != 0)
                sparkles[i - 1]->sNextSparkleSpriteId = spriteId;
            else
                firstSpriteId = spriteId;
        }
        else
        {
            break;
        }
    }

    sparkles[count]->sNextSparkleSpriteId = firstSpriteId;
    InitSummarySparkles(count, sparkles);
}

static void DestroySheenSparkleSprites(void)
{
    u16 i;
    struct Sprite **sparkles = sMonSummaryScreen->conditionSparkles;
    for (i = 0; i < MAX_CONDITION_SPARKLES; i++)
    {
        if (sparkles[i] != NULL)
        {
            DestroySprite(sparkles[i]);
            sparkles[i] = NULL;
        }
        else
        {
            break;
        }
    }
    FreeSpriteTilesByTag(TAG_CONDITION_SPARKLE);
}

#undef sSparkleId
#undef sDelayTimer
#undef sNumExtraSparkles
#undef sCurSparkleId
#undef sNextSparkleSpriteId

#define MAX_COND_SPARKLE_DELAY 60

static void SpriteCB_MaxCondSparkle(struct Sprite *sprite)
{
    if (sprite->data[0] != 0)
    {
        if (--sprite->data[0] == 0)
        {
            sprite->invisible = FALSE;
            SeekSpriteAnim(sprite, 0);
        }
        return;
    }
    if (sprite->animEnded)
    {
        sprite->invisible = TRUE;
        sprite->data[0] = MAX_COND_SPARKLE_DELAY;
    }
}

#undef MAX_COND_SPARKLE_DELAY

static const u8 sConditionMonData[CONTEST_CATEGORIES_COUNT] =
{
    [CONTEST_CATEGORY_COOL]   = MON_DATA_COOL,
    [CONTEST_CATEGORY_BEAUTY] = MON_DATA_BEAUTY,
    [CONTEST_CATEGORY_CUTE]   = MON_DATA_CUTE,
    [CONTEST_CATEGORY_SMART]  = MON_DATA_SMART,
    [CONTEST_CATEGORY_TOUGH]  = MON_DATA_TOUGH,
};

static void CreateMaxConditionSparkles(void)
{
    u8 i;
    for (i = 0; i < CONTEST_CATEGORIES_COUNT; i++)
    {
        u8 arrId = SPRITE_ARR_ID_MAX_COND_SPARKLE + i;
        sMonSummaryScreen->spriteIds[arrId] = SPRITE_NONE;
        if (GetMonData(&sMonSummaryScreen->currentMon, sConditionMonData[i]) == MAX_CONDITION)
        {
            struct Sprite *icon = &gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CONTEST_CATEGORY + i]];
            u8 spriteId = CreateSprite(&sSpriteTemplate_MaxCondSparkle,
                                       icon->x + 16, icon->y - 4,
                                       icon->subpriority - 1);
            if (spriteId != MAX_SPRITES)
                sMonSummaryScreen->spriteIds[arrId] = spriteId;
        }
    }
}

static void DestroyMaxConditionSparkles(void)
{
    u8 i;
    for (i = 0; i < CONTEST_CATEGORIES_COUNT; i++)
    {
        u8 arrId = SPRITE_ARR_ID_MAX_COND_SPARKLE + i;
        if (sMonSummaryScreen->spriteIds[arrId] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sMonSummaryScreen->spriteIds[arrId]]);
            sMonSummaryScreen->spriteIds[arrId] = SPRITE_NONE;
        }
    }
}

static void CreateConditionsPageCategoryIcons(void)
{
    static const u8 xCoords[CONTEST_CATEGORIES_COUNT] = {81, 123, 109, 53, 38};
    static const u8 yCoords[CONTEST_CATEGORIES_COUNT] = {40,  61, 117, 117, 61};
    u8 i;
    for (i = 0; i < CONTEST_CATEGORIES_COUNT; i++)
    {
        u8 arrId = SPRITE_ARR_ID_CONTEST_CATEGORY + i;
        sMonSummaryScreen->spriteIds[arrId] = CreateSprite(&sSpriteTemplate_MoveTypes, xCoords[i], yCoords[i], 2);
        StartSpriteAnim(&gSprites[sMonSummaryScreen->spriteIds[arrId]], NUMBER_OF_MON_TYPES + i);
        gSprites[sMonSummaryScreen->spriteIds[arrId]].oam.paletteNum = gContestCategoryInfo[i].palette;
    }
}

static void DestroyContestCategoryIcons(void)
{
    u8 i;
    for (i = 0; i < CONTEST_CATEGORIES_COUNT; i++)
    {
        u8 arrId = SPRITE_ARR_ID_CONTEST_CATEGORY + i;
        if (sMonSummaryScreen->spriteIds[arrId] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sMonSummaryScreen->spriteIds[arrId]]);
            sMonSummaryScreen->spriteIds[arrId] = SPRITE_NONE;
        }
    }
}

static void PrintConditionsPageText(void)
{
}

// shadow palette patched on conditions page entry: value 5→6 so (6×EVA=11)>>4 == (5×EVA=14)>>4 == 4
static const u16 sShadowPalConditions[16] = {[0 ... 15] = RGB(6, 6, 6)};

static void Task_PrintConditionsPage(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    struct ConditionGraph *graph = &sMonSummaryScreen->conditionGraph;
    struct UCoords16 center[CONDITION_COUNT];
    u8 i;

    switch (data[0])
    {
    case 0:
        graph->conditions[0][CONDITION_COOL]   = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_COOL);
        graph->conditions[0][CONDITION_TOUGH]  = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_TOUGH);
        graph->conditions[0][CONDITION_SMART]  = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_SMART);
        graph->conditions[0][CONDITION_CUTE]   = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_CUTE);
        graph->conditions[0][CONDITION_BEAUTY] = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_BEAUTY);
        SummaryConditionGraph_CalcPositions(graph->conditions[0], graph->savedPositions[0]);
        for (i = 0; i < CONDITION_COUNT; i++)
        {
            center[i].x = SUMMARY_GRAPH_CENTER_X;
            center[i].y = SUMMARY_GRAPH_CENTER_Y;
        }
        ConditionGraph_SetNewPositions(graph, center, graph->savedPositions[0]);
        ConditionGraph_InitResetScanline(graph);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3);
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(11, 6));
        if (SWSH_SUMMARY_MON_SHADOWS)
        {
            u8 shadowId = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW];
            LoadPalette(sShadowPalConditions, OBJ_PLTT_ID(gSprites[shadowId].oam.paletteNum), PLTT_SIZE_4BPP);
        }
        SetBgTilemapBuffer(1, sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONDITIONS]);
        ScheduleBgCopyTilemapToVram(1);
        SummaryConditionGraph_InitWindow();
        CreateConditionsPageCategoryIcons();
        CreateSheenSparkleSprites();
        CreateMaxConditionSparkles();
        break;
    case 1:
        if (SummaryConditionGraph_ResetScanline(graph))
            data[0]--;
        else
        {
            ScanlineEffect_InitHBlankDmaTransfer();
            SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON);
            ShowBg(1);
        }
        break;
    case 2:
    {
        bool8 animating = ConditionGraph_TryUpdate(graph);
        SummaryConditionGraph_Draw(graph);
        if (animating)
            data[0]--;
        break;
    }
    case 3:
        ScheduleBgCopyTilemapToVram(0);
        break;
    case 4:
        DestroyTask(taskId);
        return;
    }
    data[0]++;
}
#endif

static void PrintContestMovesPageText(void)
{
    PrintMoveNameAndPP(0);
    PrintMoveNameAndPP(1);
    PrintMoveNameAndPP(2);
    PrintMoveNameAndPP(3);

    if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
    {
        PrintNewMoveDetailsOrCancelText();
        if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES)
        {
            if (sMonSummaryScreen->newMove != MOVE_NONE)
                PrintMoveDescription(sMonSummaryScreen->newMove);
        }
        else
        {
            PrintMoveDescription(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]);
        }
    }
}

static void Task_PrintContestMoves(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 1:
        PrintMoveNameAndPP(0);
        break;
    case 2:
        PrintMoveNameAndPP(1);
        break;
    case 3:
        PrintMoveNameAndPP(2);
        break;
    case 4:
        PrintMoveNameAndPP(3);
        break;
    case 5:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
            PrintNewMoveDetailsOrCancelText();
        break;
    case 6:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES)
                data[1] = sMonSummaryScreen->newMove;
            else
                data[1] = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
        }
        break;
    case 7:
        if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        {
            if (sMonSummaryScreen->newMove != MOVE_NONE || sMonSummaryScreen->firstMoveIndex != MAX_MON_MOVES)
                PrintMoveDescription(data[1]);
        }
        break;
    case 8:
        DestroyTask(taskId);
        return;
    }
    data[0]++;
}

static void PrintMoveNameAndPP(u8 slotIndex)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slotIndex * MOVE_SLOT_SPRITES_COUNT)];
    if (spriteIds[0] == SPRITE_NONE)
        return;

    u8 sp0 = spriteIds[0], sp4 = spriteIds[4];
    enum Move move = summary->moves[slotIndex];

    FillSpriteRectColor(sp0, 8, 0, 80, 16, MOVE_SLOT_NAME_FILL_COLOR);
    if (move != MOVE_NONE)
        AddSpriteTextPrinterParameterized6(sp0, FONT_SMALL, 12, 0, 0, 0, sMoveSlotNameColor, 0, GetMoveName(move));

    FillSpriteRectColor(sp4, 0, 0, 26, 16, MOVE_SLOT_PP_FILL_COLOR);
    if (move != MOVE_NONE)
    {
        u8 pp = CalculatePPWithBonus(move, summary->ppBonuses, slotIndex);
        u8 ppState = GetCurrentPpToMaxPpState(summary->pp[slotIndex], pp);
        ConvertIntToDecimalStringN(gStringVar1, summary->pp[slotIndex], STR_CONV_MODE_RIGHT_ALIGN, 2);
        ConvertIntToDecimalStringN(gStringVar2, pp, STR_CONV_MODE_RIGHT_ALIGN, 2);
        DynamicPlaceholderTextUtil_Reset();
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, gStringVar1);
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(1, gStringVar2);
        DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sMovesPPLayout);
        AddSpriteTextPrinterParameterized6(sp4, FONT_SMALL, 1, 0, 0, 0, sMoveSlotPPColors[ppState], 0, gStringVar4);
    }
}

static void PrintMovePowerAndAccuracy(enum Move moveIndex)
{
    const u8 *text;
    u8 xPos;
    FillWindowPixelRect(PSS_LABEL_WINDOW_MOVES_POWER_ACC, PIXEL_FILL(0), 0, 0, 24, 40);

    if (moveIndex != MOVE_NONE)
    {
        if (gMovesInfo[moveIndex].power < 2)
        {
            text = gText_ThreeDashes;
        }
        else
        {
            ConvertIntToDecimalStringN(gStringVar1, gMovesInfo[moveIndex].power, STR_CONV_MODE_LEFT_ALIGN, 3);
            text = gStringVar1;
        }
        xPos = 22 - GetStringWidth(PSS_DEFAULT_FONT, text, 0);
        PrintTextOnWindow(PSS_LABEL_WINDOW_MOVES_POWER_ACC, text, xPos, 2, 0, 0);

        if (gMovesInfo[moveIndex].accuracy == 0)
        {
            text = gText_ThreeDashes;
        }
        else
        {
            ConvertIntToDecimalStringN(gStringVar1, gMovesInfo[moveIndex].accuracy, STR_CONV_MODE_LEFT_ALIGN, 3);
            text = gStringVar1;
        }
        xPos = 22 - GetStringWidth(PSS_DEFAULT_FONT, text, 0);
        PrintTextOnWindow(PSS_LABEL_WINDOW_MOVES_POWER_ACC, text, xPos, 23, 0, 0);
    } else {
        text = gText_ThreeDashes;
        xPos = 22 - GetStringWidth(PSS_DEFAULT_FONT, text, 0);
        PrintTextOnWindow(PSS_LABEL_WINDOW_MOVES_POWER_ACC, text, xPos, 2, 0, 0);
        PrintTextOnWindow(PSS_LABEL_WINDOW_MOVES_POWER_ACC, text, xPos, 23, 0, 0);
    }
}

#define TILEMAP_APPEAL_START  (15 * 32 + 5)
#define TILEMAP_JAM_START     (18 * 32 + 5)
#define TILE_APPEAL_FILLED    ((1 << 12) | 49)
#define TILE_APPEAL_EMPTY     ((1 << 12) | 234)
#define TILE_JAM_FILLED       ((1 << 12) | 50)
#define TILE_JAM_EMPTY        ((1 << 12) | 239)

static void HandleAppealJamTilemap(enum Move move)
{
    u16 *buf = sMonSummaryScreen->bg1TilemapBuffers[PSS_EFFECT_CONTEST];
    u8 i, appeal = 0, jam = 0;

    if (move != MOVE_NONE)
    {
        u32 effect = GetMoveContestEffect(move);
        appeal = gContestEffects[effect].appeal;
        if (appeal != 0xFF)
            appeal /= 10;
        jam = gContestEffects[effect].jam;
        if (jam != 0xFF)
            jam /= 10;
    }

    for (i = 0; i < MAX_CONTEST_MOVE_HEARTS; i++)
        buf[TILEMAP_APPEAL_START + (i / 4 * 32) + (i & 3)] = (appeal == 0xFF || i < appeal) ? TILE_APPEAL_FILLED : TILE_APPEAL_EMPTY;

    for (i = 0; i < MAX_CONTEST_MOVE_HEARTS; i++)
        buf[TILEMAP_JAM_START + (i / 4 * 32) + (i & 3)] = (jam == 0xFF || i < jam) ? TILE_JAM_FILLED : TILE_JAM_EMPTY;

    ScheduleBgCopyTilemapToVram(1);
}

#undef TILEMAP_APPEAL_START
#undef TILEMAP_JAM_START
#undef TILE_APPEAL_FILLED
#undef TILE_APPEAL_EMPTY
#undef TILE_JAM_FILLED
#undef TILE_JAM_EMPTY

static void PrintMoveDescription(enum Move move)
{
    u8 windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_DESCRIPTION);
    u8 desc[SWSH_MAX_MOVE_DESCRIPTION_LENGTH];
    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    if (move != MOVE_NONE)
    {
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
        {
            if (SWSH_SUMMARY_CATEGORY_ICONS)
                ShowCategoryIcon(move);

            PrintMovePowerAndAccuracy(move);

            if (SWSH_SUMMARY_AUTO_FORMAT_MOVE_DESCRIPTIONS)
            {
                u8 descFontId;
                if (gMovesInfo[move].effect != EFFECT_PLACEHOLDER)
                    descFontId = FormatTextByWidth(desc, 136, PSS_DEFAULT_FONT, gMovesInfo[move].description, GetFontAttribute(PSS_DEFAULT_FONT, FONTATTR_LETTER_SPACING));
                else
                    descFontId = FormatTextByWidth(desc, 136, PSS_DEFAULT_FONT, gNotDoneYetDescription, GetFontAttribute(PSS_DEFAULT_FONT, FONTATTR_LETTER_SPACING));

                PrintTextOnWindowWithFont(windowId, desc, 0, 4, 1, 0, descFontId);
            }
            else
            {
                if (gMovesInfo[move].effect != EFFECT_PLACEHOLDER)
                    PrintTextOnWindow(windowId, gMovesInfo[move].description, 0, 4, 1, 0);
                else
                    PrintTextOnWindow(windowId, gNotDoneYetDescription, 0, 4, 1, 0);
            }
        }
        else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
        {
            HandleAppealJamTilemap(move);
            if (SWSH_SUMMARY_AUTO_FORMAT_MOVE_DESCRIPTIONS)
            {
                u8 descFontId = FormatTextByWidth(desc, 136, PSS_DEFAULT_FONT, gContestEffects[GetMoveContestEffect(move)].description, GetFontAttribute(PSS_DEFAULT_FONT, FONTATTR_LETTER_SPACING));
                PrintTextOnWindowWithFont(windowId, desc, 0, 4, 1, 0, descFontId);
            }
            else
            {
                PrintTextOnWindow(windowId, gContestEffects[GetMoveContestEffect(move)].description, 0, 4, 1, 0);
            }
        }
        PutWindowTilemap(windowId);
    }
    else
    {
        if (SWSH_SUMMARY_CATEGORY_ICONS)
            DestroyCategoryIcon();

        ClearWindowTilemap(windowId);
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
            PrintMovePowerAndAccuracy(MOVE_NONE);
        else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
            HandleAppealJamTilemap(MOVE_NONE);
    }

    ScheduleBgCopyTilemapToVram(0);
}

static void PrintNewMoveDetailsOrCancelText(void)
{
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (4 * MOVE_SLOT_SPRITES_COUNT)];
    if (spriteIds[0] == SPRITE_NONE)
        return;

    u8 sp0 = spriteIds[0], sp4 = spriteIds[4];

    FillSpriteRectColor(sp0, 8, 0, 80, 16, MOVE_SLOT_NAME_FILL_COLOR);

    if (sMonSummaryScreen->newMove == MOVE_NONE)
    {
        AddSpriteTextPrinterParameterized6(sp0, FONT_SMALL, 12, 0, 0, 0, sMoveSlotNameColor, 0, sText_Cancel);
    }
    else
    {
        enum Move move = sMonSummaryScreen->newMove;
        AddSpriteTextPrinterParameterized6(sp0, FONT_SMALL, 12, 0, 0, 0, sMoveSlotNameColor, 0, GetMoveName(move));

        FillSpriteRectColor(sp4, 0, 0, 24, 16, MOVE_SLOT_PP_FILL_COLOR);
        u8 maxPP = gMovesInfo[move].pp;
        ConvertIntToDecimalStringN(gStringVar1, maxPP, STR_CONV_MODE_RIGHT_ALIGN, 2);
        DynamicPlaceholderTextUtil_Reset();
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, gStringVar1);
        DynamicPlaceholderTextUtil_SetPlaceholderPtr(1, gStringVar1);
        DynamicPlaceholderTextUtil_ExpandPlaceholders(gStringVar4, sMovesPPLayout);
        AddSpriteTextPrinterParameterized6(sp4, FONT_SMALL, 1, 0, 0, 0, sMoveSlotPPColors[3], 0, gStringVar4);
    }
}

static void ClearCancelText(void)
{
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (4 * MOVE_SLOT_SPRITES_COUNT)];
    if (spriteIds[0] == SPRITE_NONE)
        return;

    u8 sp0 = spriteIds[0], sp4 = spriteIds[4];
    FillSpriteRectColor(sp0, 8, 0, 80, 12, MOVE_SLOT_NAME_FILL_COLOR);
    FillSpriteRectColor(sp4, 0, 0, 24, 12, MOVE_SLOT_PP_FILL_COLOR);
}

static void SwapMovesNamesPP(u8 moveIndex1, u8 moveIndex2)
{
    PrintMoveNameAndPP(moveIndex1);
    PrintMoveNameAndPP(moveIndex2);
}

static void PrintHMMovesCantBeForgotten(void)
{
    u8 message[200];
    u8 windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_DESCRIPTION);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));

    u8 msgFontId = FormatTextByWidth(message, 136, PSS_DEFAULT_FONT, gText_HMMovesCantBeForgotten2, GetFontAttribute(PSS_DEFAULT_FONT, FONTATTR_LETTER_SPACING));
    PrintTextOnWindowWithFont(windowId, message, 0, 4, 0, 2, msgFontId);
}

static void ShowCategoryIcon(enum Move move)
{
    if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY] == SPRITE_NONE)
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY] = CreateSprite(&sSpriteTemplate_CategoryIcons, 70, 129, 0);

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY]].invisible = FALSE;

    StartSpriteAnim(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY]], GetBattleMoveCategory(move));
}

static void DestroyCategoryIcon(void)
{
    if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY] != SPRITE_NONE)
        DestroySprite(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY]]);
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_CATEGORY] = SPRITE_NONE;
}

static void ResetSpriteIds(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->spriteIds); i++)
        sMonSummaryScreen->spriteIds[i] = SPRITE_NONE;
}

static void DestroySpriteInArray(u8 spriteArrayId)
{
    if (sMonSummaryScreen->spriteIds[spriteArrayId] != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]]);
        sMonSummaryScreen->spriteIds[spriteArrayId] = SPRITE_NONE;
    }
}

static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible)
{
    gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]].invisible = invisible;
}

static void HidePageSpecificSprites(void)
{
    // Keeps Pokemon, caught ball, and status sprites visible.
    u8 i;

    for (i = SPRITE_ARR_ID_POKERUS_CURED; i < ARRAY_COUNT(sMonSummaryScreen->spriteIds); i++)
    {
        if (sMonSummaryScreen->spriteIds[i] != SPRITE_NONE)
            SetSpriteInvisibility(i, TRUE);
    }
    sMonSummaryScreen->markingsSprite->invisible = TRUE;
}

static void SetContestMoveTypeIcons(void)
{
    u8 i;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (summary->moves[i] != MOVE_NONE)
        {
            u8 category = gMovesInfo[summary->moves[i]].contestCategory;
            SetTypeSpritePosAndPal(NUMBER_OF_MON_TYPES + category, 94, 29 + (i * 18), i + SPRITE_ARR_ID_TYPE);
            gSprites[sMonSummaryScreen->spriteIds[i + SPRITE_ARR_ID_TYPE]].oam.paletteNum = gContestCategoryInfo[category].palette;
        }
        else
        {
            SetSpriteInvisibility(i + SPRITE_ARR_ID_TYPE, TRUE);
        }
    }
}

static void SetTypeIcons(void)
{
    switch (sMonSummaryScreen->currPageIndex)
    {
    case PSS_PAGE_INFO:
        SetMonTypeIcons();
        break;
    case PSS_PAGE_BATTLE_MOVES:
        SetMoveTypeIcons();
        SetNewMoveTypeIcon();
        UpdateMoveSlotVisibility();
        UpdateMoveSlotPalette();
        break;
    case PSS_PAGE_CONTEST_MOVES:
        SetContestMoveTypeIcons();
        SetNewMoveTypeIcon();
        UpdateMoveSlotVisibility();
        UpdateMoveSlotPalette();
        break;
    }
}

static void TrySetInfoPageIcons(void)
{
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO)
    {
        SetPokerusCuredSprite();
        SetShinySprite();
        RefreshInfoPageItemTilemap();
        CreateHeldItemSprite();
        // // Hold off on printing friendship
        if (SWSH_SUMMARY_SHOW_FRIENDSHIP)
            SetFriendshipSprite();
    }
}

static void CreateMoveTypeIcons(void)
{
    u8 i;

    for (i = SPRITE_ARR_ID_TERA_TYPE; i < SPRITE_ARR_ID_TYPE + TYPE_ICON_SPRITE_COUNT; i++)
    {
        if (sMonSummaryScreen->spriteIds[i] == SPRITE_NONE)
        {
            if (i == SPRITE_ARR_ID_TERA_TYPE)
            {
                if (SWSH_SUMMARY_SHOW_TERA_TYPE)
                    sMonSummaryScreen->spriteIds[i] = CreateSprite(&sSpriteTemplate_TeraType, 0, 0, 2);
            }
            else
            {
                sMonSummaryScreen->spriteIds[i] = CreateSprite(&sSpriteTemplate_MoveTypes, 0, 0, 2);
            }
        }

        SetSpriteInvisibility(i, TRUE);
    }
}

static void SetTypeSpritePosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId)
{
    struct Sprite *sprite = &gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]];
    StartSpriteAnim(sprite, typeId);
    if (typeId < NUMBER_OF_MON_TYPES)
        sprite->oam.paletteNum = gTypesInfo[typeId].palette;
    sprite->x = x + 16;
    sprite->y = y + 7; // shift up by 1 to accomodate gen 8 type badges
    SetSpriteInvisibility(spriteArrayId, FALSE);
}

static void SetMonTypeIcons(void)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    if (summary->isEgg)
    {
        SetTypeSpritePosAndPal(TYPE_MYSTERY, 56, 46, SPRITE_ARR_ID_TYPE);
        SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 1, TRUE);
        if (SWSH_SUMMARY_SHOW_TERA_TYPE)
            SetSpriteInvisibility(SPRITE_ARR_ID_TERA_TYPE, TRUE);
    }
    else
    {
        SetTypeSpritePosAndPal(gSpeciesInfo[summary->species].types[0], 56, 46, SPRITE_ARR_ID_TYPE);
        if (gSpeciesInfo[summary->species].types[0] != gSpeciesInfo[summary->species].types[1])
        {
            SetTypeSpritePosAndPal(gSpeciesInfo[summary->species].types[1], 92, 46, SPRITE_ARR_ID_TYPE + 1);
            SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 1, FALSE);
        }
        else
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 1, TRUE);
        }
        if (SWSH_SUMMARY_SHOW_TERA_TYPE)
        {
            SetTypeSpritePosAndPal(summary->teraType, 124, 45, SPRITE_ARR_ID_TERA_TYPE);
        }
    }
}

static enum BattlerId GetCurrentBattlerFromSumIndex(u32 sumIndex)
{
    for (u32 battler = B_BATTLER_0; battler < gBattlersCount; battler++)
    {
        if (!IsOnPlayerSide(battler))
            continue;

        if (gBattlerPartyIndexes[battler] == sumIndex)
            return battler;
    }

    return B_BATTLER_0;
}

static enum Type SummaryScreen_GetDynamicMoveType(struct Pokemon *mon, enum Move move, enum Type type)
{
    if (!P_SHOW_DYNAMIC_TYPES)
        return type;

    if (gBattleStruct == NULL)
        return CheckDynamicMoveType(mon, move, 0, MON_OUTSIDE_BATTLE);

    u32 partyIndex = sMonSummaryScreen->curMonIndex;
    bool32 isDouble = IsDoubleBattle();

    if ((isDouble && partyIndex > 1) || (!isDouble && partyIndex > 0))
        return CheckDynamicMoveType(mon, move, 0, MON_OUTSIDE_BATTLE);

    return CheckDynamicMoveType(mon, move, GetCurrentBattlerFromSumIndex(sMonSummaryScreen->curMonIndex), MON_IN_BATTLE);
}

static void SetMoveTypeIcons(void)
{
    u8 i;
    enum Move move;
    u32 type;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = summary->moves[i];
        if (move != MOVE_NONE)
        {
            type = GetMoveType(move);
            type = SummaryScreen_GetDynamicMoveType(&sMonSummaryScreen->currentMon, move, type);
            SetTypeSpritePosAndPal(type, 94, 29 + (i * 18), i + SPRITE_ARR_ID_TYPE);
        }
        else
        {
            SetSpriteInvisibility(i + SPRITE_ARR_ID_TYPE, TRUE);
        }

    }
}


static void SetNewMoveTypeIcon(void)
{
    u32 move = sMonSummaryScreen->newMove;
    u8 i = 4;

    if (move == MOVE_NONE)
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_TYPE + 4, TRUE);
    }
    else if (sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
    {
        u8 category = gMovesInfo[move].contestCategory;
        SetTypeSpritePosAndPal(NUMBER_OF_MON_TYPES + category, 94 + 8, 29 + (i * 18), SPRITE_ARR_ID_TYPE + 4);
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_TYPE + 4]].oam.paletteNum = gContestCategoryInfo[category].palette;
    }
    else
    {
        enum Type type = GetMoveType(move);
        type = SummaryScreen_GetDynamicMoveType(&sMonSummaryScreen->currentMon, move, type);
        SetTypeSpritePosAndPal(type, 94 + 8, 29 + (i * 18), SPRITE_ARR_ID_TYPE + 4);
    }
}

static void SwapMovesTypeSprites(u8 moveIndex1, u8 moveIndex2)
{
    struct Sprite *sprite1 = &gSprites[sMonSummaryScreen->spriteIds[moveIndex1 + SPRITE_ARR_ID_TYPE]];
    struct Sprite *sprite2 = &gSprites[sMonSummaryScreen->spriteIds[moveIndex2 + SPRITE_ARR_ID_TYPE]];

    u8 temp = sprite1->animNum;
    sprite1->animNum = sprite2->animNum;
    sprite2->animNum = temp;

    temp = sprite1->oam.paletteNum;
    sprite1->oam.paletteNum = sprite2->oam.paletteNum;
    sprite2->oam.paletteNum = temp;

    sprite1->animBeginning = TRUE;
    sprite1->animEnded = FALSE;
    sprite2->animBeginning = TRUE;
    sprite2->animEnded = FALSE;
}

static u8 LoadMonGfxAndSprite(struct Pokemon *mon, s16 *state, bool32 isShadow)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    switch (*state)
    {
    default:
        return CreateMonSprite(mon, isShadow);
    case 0:
        if (gMain.inBattle)
        {
            HandleLoadSpecialPokePicIsEgg(TRUE,
                                     gMonSpritesGfxPtr->spritesGfx[B_POSITION_OPPONENT_LEFT],
                                     summary->species,
                                     summary->pid,
                                     summary->isEgg);
        }
        else
        {
            if (gMonSpritesGfxPtr != NULL)
            {
                HandleLoadSpecialPokePicIsEgg(TRUE,
                                         gMonSpritesGfxPtr->spritesGfx[B_POSITION_OPPONENT_LEFT],
                                         summary->species,
                                         summary->pid,
                                         summary->isEgg);
            }
            else
            {
                HandleLoadSpecialPokePicIsEgg(TRUE,
                                         MonSpritesGfxManager_GetSpritePtr(MON_SPR_GFX_MANAGER_A, B_POSITION_OPPONENT_LEFT),
                                         summary->species,
                                         summary->pid,
                                         summary->isEgg);
            }
        }
        (*state)++;
        return 0xFF;
    case 1:
        LoadSpritePaletteWithTag(GetMonSpritePalFromSpeciesAndPersonalityIsEgg(summary->species, summary->isShiny, summary->pid, summary->isEgg), summary->species2);
        SetMultiuseSpriteTemplateToPokemon(summary->species2, B_POSITION_OPPONENT_LEFT);
        (*state)++;
        return 0xFF;
    }
}

static void PlayMonCry(void)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    if (!summary->isEgg)
    {
        if (ShouldPlayNormalMonCry(&sMonSummaryScreen->currentMon) == TRUE)
            PlayCry_ByMode(summary->species2, 0, CRY_MODE_NORMAL);
        else
            PlayCry_ByMode(summary->species2, 0, CRY_MODE_WEAK);
    }
}

static u8 CreateMonSprite(struct Pokemon *unused, bool32 isShadow)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u8 shadowPalette = 0;
    u8 spriteId = CreateSprite(&gMultiuseSpriteTemplate, 202, 76, 5);

    FreeSpriteOamMatrix(&gSprites[spriteId]);
    gSprites[spriteId].sSpecies = summary->species2;
    gSprites[spriteId].sDelayAnim = 0;
    gSprites[spriteId].sIsShadow = isShadow;
    gSprites[spriteId].callback = SpriteCB_Pokemon;
    gSprites[spriteId].oam.priority = 2;

    if (isShadow)
    {
        FreeSpritePaletteByTag(TAG_MON_SHADOW); // reload the palette entirely because some sprite anims modify it
        shadowPalette = LoadSpritePalette(&sSpritePal_MonShadow);
        gSprites[spriteId].oam.paletteNum = shadowPalette;
        gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;
        gSprites[spriteId].subpriority = 6;
        gSprites[spriteId].x += 4;
        gSprites[spriteId].y += 2;
    }

    return spriteId;
}

static void SpriteCB_Pokemon(struct Sprite *sprite)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    if (!gPaletteFade.active && sprite->sDelayAnim != 1)
    {
        sprite->sDontFlip = TRUE;

        if (!sMonSummaryScreen->monAnimPlayed) // only play cry on the first time mon is animated
            PlayMonCry();

        PokemonSummaryDoMonAnimation(sprite, sprite->sSpecies, summary->isEgg, sprite->sIsShadow);
        sMonSummaryScreen->monAnimPlayed = TRUE;
    }
}

#undef sSpecies
#undef sDontFlip
#undef sDelayAnim
#undef sIsShadow

// Track and then destroy Task_PokemonSummaryAnimateAfterDelay
// Normally destroys itself but it can be interrupted before the animation starts
void SummaryScreen_SetAnimDelayTaskId_SwSh(u8 taskId)
{
    sAnimDelayTaskId = taskId;
}

static void SummaryScreen_DestroyAnimDelayTask(void)
{
    if (sAnimDelayTaskId != TASK_NONE)
    {
        DestroyTask(sAnimDelayTaskId);
        sAnimDelayTaskId = TASK_NONE;
    }
    StopShadowAnimDelayTask();
}

static void StopPokemonAnimations(void)  // A subtle effect, this function stops Pokémon animations when leaving the PSS
{
    u16 i;
    u16 paletteIndex;

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].animPaused = TRUE;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].callback = SpriteCallbackDummy;
    if (SWSH_SUMMARY_MON_SHADOWS)
    {
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].animPaused = TRUE;
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHADOW]].callback = SpriteCallbackDummy;
    }
    StopPokemonAnimationDelayTask();

    paletteIndex = OBJ_PLTT_ID(gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].oam.paletteNum);

    for (i = 0; i < 16; i++)
    {
        u16 id = i + paletteIndex;
        gPlttBufferUnfaded[id] = gPlttBufferFaded[id];
    }
}


static void CreateMonMarkingsSprite(struct Pokemon *mon)
{
    struct Sprite *sprite = CreateMonMarkingComboSprite(TAG_MON_MARKINGS, TAG_CATEGORY_ICONS, sCategoryIcons_Pal);

    sMonSummaryScreen->markingsSprite = sprite;
    if (sprite != NULL)
    {
        sMonSummaryScreen->markingComboTilesPtr = (void *) OBJ_VRAM0 + 32 * GetSpriteTileStartByTag(TAG_MON_MARKINGS);
        UpdateMonMarkingTiles(GetMonData(mon, MON_DATA_MARKINGS), sMonSummaryScreen->markingComboTilesPtr);

        sMonSummaryScreen->markingsSprite->x = 137;
        sMonSummaryScreen->markingsSprite->y = 85;
        sMonSummaryScreen->markingsSprite->oam.priority = 2;
        if (sMonSummaryScreen->currPageIndex != PSS_PAGE_INFO)
            sMonSummaryScreen->markingsSprite->invisible = TRUE;
    }
}

static void SetPokerusCuredSprite(void)
{
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_POKERUS_CURED] == SPRITE_NONE)
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_POKERUS_CURED] = CreateSprite(&sSpriteTemplate_PokerusCuredIcon, 117, 85, 0);

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_POKERUS_CURED]].invisible = !ShouldPokemonShowCuredPokerus(mon);
}

static void SetShinySprite(void)
{
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHINY] == SPRITE_NONE)
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHINY] = CreateSprite(&sSpriteTemplate_ShinyIcon, 126, 36, 6);

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SHINY]].invisible = !IsMonShiny(mon);
}

bool8 MonHasGigantamaxFactor(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_GIGANTAMAX_FACTOR, NULL);
}

static void CreateGigantamaxSprite(void)
{
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    u8 *spriteId = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_GIGANTAMAX];

    if (*spriteId == SPRITE_NONE)
        *spriteId = CreateSprite(&sSpriteTemplate_GigantamaxIcon, 114, 17, 6);

    gSprites[*spriteId].invisible = !MonHasGigantamaxFactor(mon);
}

static void SetFriendshipSprite(void)
{
    u8 level = FRIENDSHIP_LEVEL_0;

    if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_FRIENDSHIP] == SPRITE_NONE)
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_FRIENDSHIP] = CreateSprite(&sSpriteTemplate_FriendshipIcon, 233, 32, 0);

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_FRIENDSHIP]].invisible = FALSE;

    // don't even think about swapping the order of the conditions here or the compiler will smite you for your arrogance
    // I have no idea why it works that way
    while (level < FRIENDSHIP_LEVEL_MAX && sMonSummaryScreen->summary.friendship >= sFriendshipLevelToThreshold[level + 1])
        level++;

    StartSpriteAnim(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_FRIENDSHIP]], level);
}

static void UpdateMonMarkingsSprite(struct Pokemon *mon)
{
    if (sMonSummaryScreen->markingsSprite != NULL && sMonSummaryScreen->markingComboTilesPtr != NULL)
    {
        UpdateMonMarkingTiles(GetMonData(mon, MON_DATA_MARKINGS), sMonSummaryScreen->markingComboTilesPtr);
    }
    else
    {
        CreateMonMarkingsSprite(mon);
    }
}

static void CreateCaughtBallSprite(struct Pokemon *mon)
{
    enum PokeBall ball = GetMonData(mon, MON_DATA_POKEBALL);

    LoadBallGfx(ball);
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL] = CreateSprite(&gPokeBalls[ball].spriteTemplate, 95, 16, 6);
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].callback = SpriteCallbackDummy;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].oam.priority = 1;
}

static void RefreshInfoPageItemTilemap(void)
{
    const u32 *tilemap = (sMonSummaryScreen->summary.item != ITEM_NONE)
        ? sSummaryPage_Info_Item_Tilemap
        : sSummaryPage_Info_Tilemap;

    DecompressDataWithHeaderWram(tilemap, sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_INFO]);
#if SWSH_SUMMARY_SHOW_CONTEST_PAGES
    ApplyPageIcon(sMonSummaryScreen->bg2TilemapBuffers[PSS_PAGE_INFO], 4, PAGE_ICON_TILE_CONTEST_MOVES);
#endif
    ScheduleBgCopyTilemapToVram(2);
}

static void CreateDynamaxLevelSprites(void)
{
    u8 i;
    u8 level = GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_DYNAMAX_LEVEL);
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_DYNAMAX_LEVEL];
    u8 animFrame;
    s16 xPos;

    DestroyDynamaxLevelSprites();

    if (!SWSH_SUMMARY_SHOW_DYNAMAX_LEVEL)
        return;

    if (level >= DYNAMAX_LEVEL_COUNT)
        level = DYNAMAX_LEVEL_COUNT - 1;

    if (level == 0)
        return;

    for (i = 0; i < DYNAMAX_LEVEL_SPRITES_COUNT; i++)
    {
        if (i < level)
            animFrame = i + 1;
        else
            animFrame = 0;

        xPos = 62 + (i * 8) + 8;

        spriteIds[i] = CreateSprite(&sSpriteTemplate_DynamaxLevel, xPos, 98, 1);

        if (spriteIds[i] != MAX_SPRITES)
        {
            StartSpriteAnim(&gSprites[spriteIds[i]], animFrame);
        }
    }
}

static void DestroyDynamaxLevelSprites(void)
{
    u8 i;
    for (i = 0; i < DYNAMAX_LEVEL_SPRITES_COUNT; i++)
        DestroySpriteInArray(SPRITE_ARR_ID_DYNAMAX_LEVEL + i);
}

static void CreateHeldItemSprite(void)
{
    enum Item itemId = sMonSummaryScreen->summary.item;
    u8 spriteId;

    // Always destroy old sprite first
    DestroyHeldItemIconSprite();

    // Only create new sprite if mon has an item
    if (itemId == ITEM_NONE)
        return;

    spriteId = AddItemIconSprite(TAG_HELD_ITEM_ICON, TAG_HELD_ITEM_ICON, itemId);

    if (spriteId != MAX_SPRITES)
    {
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_ITEM] = spriteId;
        gSprites[spriteId].x = 70;  // Set your desired x position
        gSprites[spriteId].y = 112;   // Set your desired y position
    }
}

static void DestroyHeldItemIconSprite(void)
{
    u8 spriteId = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_ITEM];

    if (spriteId != SPRITE_NONE)
    {
        FreeSpriteTilesByTag(TAG_HELD_ITEM_ICON);
        FreeSpritePaletteByTag(TAG_HELD_ITEM_ICON);
        DestroySprite(&gSprites[spriteId]);
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_ITEM] = SPRITE_NONE;
    }
}

static void CreateStatusSprite(void)
{
    u8 *spriteId = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_STATUS];
    u8 statusAnim;

    if (*spriteId == SPRITE_NONE)
        *spriteId = CreateSprite(&sSpriteTemplate_StatusCondition, 205, 16, 6); // moved from 213 to 179 to print status where level is.

    statusAnim = GetMonAilment(&sMonSummaryScreen->currentMon);
    if (statusAnim != 0)
    {
        StartSpriteAnim(&gSprites[*spriteId], statusAnim - 1);
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, FALSE);
    }
    else
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, TRUE);
    }
}

static void HandleStatusSprite(struct Pokemon *mon)
{
    u8 statusAnim = GetMonAilment(&sMonSummaryScreen->currentMon);
    if (statusAnim != 0)
    {
        StartSpriteAnim(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_STATUS]], statusAnim - 1);
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, FALSE);
    }
    else
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, TRUE);
    }
}

static void CreateMoveCursorSprite(void)
{
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
        || sMonSummaryScreen->currPageIndex == PSS_PAGE_CONTEST_MOVES)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_MoveCursor, 10, 35, 0);
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_CURSOR] = spriteId;
        gSprites[spriteId].y2 = sMonSummaryScreen->firstMoveIndex * 18;
        gSprites[spriteId].callback = SpriteCB_MoveCursor;
    }
}

static void DestroyMoveCursorSprite(void)
{
    DestroySpriteInArray(SPRITE_ARR_ID_MOVE_CURSOR);
}

static void SpriteCB_MoveCursor(struct Sprite *sprite)
{
    s16 targetY = sMonSummaryScreen->firstMoveIndex * 18;
    s16 diff = targetY - sprite->y2;

    if (diff != 0)
    {
        s16 step = diff / 2;
        if (step == 0)
            step = (diff > 0) ? 1 : -1;
        sprite->y2 += step;
    }
}

static void LiftMoveSlot(u8 slot)
{
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slot * MOVE_SLOT_SPRITES_COUNT)];
    s16 liftedY = 36 + slot * 18 - 8;
    u8 typeIconId = sMonSummaryScreen->spriteIds[slot + SPRITE_ARR_ID_TYPE];
    u8 cursorId = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_CURSOR];
    struct ComfyAnimEasingConfig config;
    u8 i;

    for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
    {
        if (spriteIds[i] != SPRITE_NONE)
        {
            struct Sprite *sprite = &gSprites[spriteIds[i]];
            sprite->x += 4;
            sprite->y = liftedY;
            sprite->oam.priority = 0;
        }
    }

    if (typeIconId != SPRITE_NONE)
    {
        struct Sprite *typeSprite = &gSprites[typeIconId];
        typeSprite->x += 4;
        typeSprite->y = liftedY;
        typeSprite->oam.priority = 0;
    }

    if (cursorId != SPRITE_NONE)
        gSprites[cursorId].invisible = TRUE;

    InitComfyAnimConfig_Easing(&config);
    config.from = Q_24_8(liftedY);
    config.to = Q_24_8(liftedY);
    config.durationFrames = 1;
    config.easingFunc = ComfyAnimEasing_EaseOutCubic;
    sHeldAnimId = CreateComfyAnim_Easing(&config);
}

static void DropMoveSlot(void)
{
    u8 slot = sHeldSlot;
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slot * MOVE_SLOT_SPRITES_COUNT)];
    s16 homeY = 36 + slot * 18;
    u8 typeIconId = sMonSummaryScreen->spriteIds[slot + SPRITE_ARR_ID_TYPE];
    u8 cursorId = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_CURSOR];
    u8 i;

    for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
    {
        if (spriteIds[i] != SPRITE_NONE)
        {
            struct Sprite *sprite = &gSprites[spriteIds[i]];
            sprite->x -= 4;
            sprite->y = homeY;
            sprite->oam.priority = 1;
        }
    }

    if (typeIconId != SPRITE_NONE)
    {
        struct Sprite *typeSprite = &gSprites[typeIconId];
        typeSprite->x -= 4;
        typeSprite->y = homeY;
        typeSprite->oam.priority = 1;
    }

    if (cursorId != SPRITE_NONE)
        gSprites[cursorId].invisible = FALSE;

    if (sHeldAnimId != INVALID_COMFY_ANIM)
    {
        ReleaseComfyAnim(sHeldAnimId);
        sHeldAnimId = INVALID_COMFY_ANIM;
    }

    sHeldSlot = MOVE_SLOT_COUNT;
}

static void AnimateLiftedSlotToTarget(void)
{
    struct ComfyAnimEasingConfig config;
    s16 targetY = 36 + sMonSummaryScreen->secondMoveIndex * 18 - 8;

    if (sHeldAnimId == INVALID_COMFY_ANIM)
        return;

    InitComfyAnimConfig_Easing(&config);
    config.from = gComfyAnims[sHeldAnimId].position;
    config.to = Q_24_8(targetY);
    config.durationFrames = 8;
    config.easingFunc = ComfyAnimEasing_EaseOutCubic;
    InitComfyAnim_Easing(&config, &gComfyAnims[sHeldAnimId]);
}

static void CreateMoveSlotSprites(void)
{
    u8 slot, i;
    u8 spriteId;

    for (slot = 0; slot < MOVE_SLOT_COUNT; slot++)
    {
        u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slot * MOVE_SLOT_SPRITES_COUNT)];
        s16 y = 36 + (slot * 18);
        s16 xOffset = (slot == 4) ? 8 : 0;

        for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
        {
            s16 x = 24 + (i * 32) + xOffset;
            spriteId = CreateSprite(&sMoveSlotSpriteTemplates[slot], x, y, 3);
            spriteIds[i] = spriteId;
            StartSpriteAnim(&gSprites[spriteId], sMoveSlotAnims[i]);
            SetSpriteSheetFrameTileNum(&gSprites[spriteId]);
        }
        u8 nameIds[3] = { spriteIds[0], spriteIds[1], spriteIds[2] };
        SetupSpritesForTextPrinting(nameIds, NULL, 3, 1);
        u8 ppIds[1] = { spriteIds[4] };
        SetupSpritesForTextPrinting(ppIds, NULL, 1, 1);
    }
    UpdateMoveSlotVisibility();
    UpdateMoveSlotPalette();

    for (slot = 0; slot < MAX_MON_MOVES; slot++)
        PrintMoveNameAndPP(slot);
    if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        PrintNewMoveDetailsOrCancelText();
}

static void DestroyMoveSlotSprites(void)
{
    u8 i;
    for (i = 0; i < MOVE_SLOT_SPRITES_COUNT * MOVE_SLOT_COUNT; i++)
        DestroySpriteInArray(SPRITE_ARR_ID_MOVE_SLOT + i);
}

static void UpdateMoveSlotVisibility(void)
{
    u8 slot, i;

    for (slot = 0; slot < MOVE_SLOT_COUNT; slot++)
    {
        u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slot * MOVE_SLOT_SPRITES_COUNT)];
        bool8 visible = (slot < MAX_MON_MOVES)
            ? (sMonSummaryScreen->summary.moves[slot] != MOVE_NONE)
            : (sMonSummaryScreen->newMove != MOVE_NONE);

        for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
        {
            if (spriteIds[i] != MAX_SPRITES)
                gSprites[spriteIds[i]].invisible = !visible;
        }
    }
}

static void UpdateMoveSlotPalette(void)
{
    u8 slot, i;
    u8 mainPalSlot = IndexOfSpritePaletteTag(TAG_MOVE_SLOT_MAIN);
    u8 focusPalSlot = IndexOfSpritePaletteTag(TAG_MOVE_SLOT_FOCUS);
    bool8 cursorActive = (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_CURSOR] != SPRITE_NONE);
    bool8 switchModeActive = (sHeldSlot != MOVE_SLOT_COUNT);

    for (slot = 0; slot < MOVE_SLOT_COUNT; slot++)
    {
        u8 *spriteIds = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SLOT + (slot * MOVE_SLOT_SPRITES_COUNT)];
        bool8 isHighlighted;

        if (switchModeActive)
            isHighlighted = (slot == sMonSummaryScreen->secondMoveIndex && slot != sHeldSlot);
        else
            isHighlighted = (cursorActive && slot == sMonSummaryScreen->firstMoveIndex);

        u8 paletteNum = isHighlighted ? focusPalSlot : mainPalSlot;

        for (i = 0; i < MOVE_SLOT_SPRITES_COUNT; i++)
        {
            if (spriteIds[i] != MAX_SPRITES)
                gSprites[spriteIds[i]].oam.paletteNum = paletteNum;
        }
    }
}


// New helper function that performs the formatting logic
static u8 PerformTextFormatting(u8 *result, s32 maxWidth, u8 fontId, const u8 *str, s16 letterSpacing, u32 *outLineCount)
{
    u8 *end, *ptr, *curLine, *lastSpace;

    end = result;
    // copy string, replacing spaces and line breaks with EOS
    // EXCEPT: if newline follows a hyphen, skip the newline without adding EOS
    while (*str != EOS)
    {
        if (*str == CHAR_SPACE || *str == CHAR_NEWLINE)
        {
            // Skip newline after hyphen, otherwise mark as break point
            if (!(*str == CHAR_NEWLINE && end > result && *(end - 1) == CHAR_HYPHEN))
            {
                *end = EOS;
                end++;
            }
        }
        else
        {
            // Regular character - copy it
            *end = *str;
            end++;
        }

        str++;
    }
    *end = EOS; // now end points to the true end of the string

    // Step 2: Remove hyphens for specific words only
    u8 *p = result;
    while (p < end)
    {
        if (*p == CHAR_HYPHEN && ShouldRemoveHyphen(p, result, end))
        {
            u8 *dst = p;
            u8 *src = p + 1;
            while (src <= end)
                *dst++ = *src++;
            end--;
        }
        else
        {
            p++;
        }
    }

    ptr = result;
    curLine = ptr;
    *outLineCount = 1;

    while (*ptr != EOS)
        ptr++;
    // now ptr is the first EOS char

    while (ptr != end)
    {
        // all the EOS chars (except *end) must be replaced by either ' ' or '\n'
        lastSpace = ptr++; // this points at the EOS

        // check that adding the next word this line still fits
        *lastSpace = CHAR_SPACE;
        if (GetStringWidth(fontId, curLine, letterSpacing) > maxWidth)
        {
            *lastSpace = CHAR_NEWLINE;
            (*outLineCount)++;
            curLine = ptr;
        }

        while (*ptr != EOS)
            ptr++;
        // now ptr is the next EOS char
    }

    // Check if the last line also fits within maxWidth
    return (GetStringWidth(fontId, curLine, letterSpacing) <= maxWidth);
}

// Original FormatTextByWidth function by Vexx on Ravepossum's branch
// Modified here to use PerformTextFormatting and try 1-font narrower if needed
static u8 FormatTextByWidth(u8 *result, s32 maxWidth, u8 fontId, const u8 *str, s16 letterSpacing)
{
    u32 lineCount;
    bool32 lastLineFits;

    // Try formatting with progressively narrower fonts until it fits in 2 lines or fewer
    while (TRUE)
    {
        lastLineFits = PerformTextFormatting(result, maxWidth, fontId, str, letterSpacing, &lineCount);

        // If we have 2 or fewer lines AND the last line fits, we're done
        if (lineCount < 3 && lastLineFits)
            break;

        // Try to get a narrower font
        u8 narrowerFontId = fontId;
        if (fontId == PSS_DEFAULT_FONT)
            narrowerFontId = FONT_SHORT_NARROWER;

        // If no narrower font available, use what we have
        if (narrowerFontId == fontId)
            break;

        fontId = narrowerFontId;
        letterSpacing = GetFontAttribute(fontId, FONTATTR_LETTER_SPACING);
    }

    return fontId;
}

static inline bool32 ShouldShowMoveRelearner(void)
{
    return (P_SUMMARY_SCREEN_MOVE_RELEARNER
         && !sMonSummaryScreen->lockMovesFlag
         && sMonSummaryScreen->mode != SUMMARY_MODE_BOX_CURSOR
         && sMonSummaryScreen->hasRelearnableMoves
         && !InBattleFactory()
         && !InSlateportBattleTent());
}

static void RefreshRelearnModePrompt(void)
{
    FillWindowPixelRect(PSS_LABEL_WINDOW_PROMPT_MOVES, PIXEL_FILL(0), 0, 0, 120, 16);
    if (ShouldShowMoveRelearner())
    {
        PrintButtonIcon(PSS_LABEL_WINDOW_PROMPT_MOVES, BUTTON_LR, 8, 4);
        PrintButtonIcon(PSS_LABEL_WINDOW_PROMPT_MOVES, BUTTON_START, 27, 4);
        PrintTextOnWindowWithFont(PSS_LABEL_WINDOW_PROMPT_MOVES, sText_Relearn, 53, 0, 0, 1, FONT_SMALL);
        PrintTextOnWindowWithFont(PSS_LABEL_WINDOW_PROMPT_MOVES, sRelearnModeNames[gMoveRelearnerState],
                                  53 + GetStringWidth(FONT_SMALL, sText_Relearn, 0), 0, 0, 1, FONT_SMALL);
    }
    ScheduleBgCopyTilemapToVram(0);
}

static void PrintMovesPagePrompt(void)
{
    FillWindowPixelBuffer(PSS_LABEL_WINDOW_PROMPT_MOVES, PIXEL_FILL(0));
    if (ShouldShowMoveRelearner())
    {
        PrintButtonIcon(PSS_LABEL_WINDOW_PROMPT_MOVES, BUTTON_LR, 8, 4);
        PrintButtonIcon(PSS_LABEL_WINDOW_PROMPT_MOVES, BUTTON_START, 27, 4);
        PrintTextOnWindowWithFont(PSS_LABEL_WINDOW_PROMPT_MOVES, sText_Relearn, 53, 0, 0, 1, FONT_SMALL);
        PrintTextOnWindowWithFont(PSS_LABEL_WINDOW_PROMPT_MOVES, sRelearnModeNames[gMoveRelearnerState],
                                  53 + GetStringWidth(FONT_SMALL, sText_Relearn, 0), 0, 0, 1, FONT_SMALL);
    }
    PrintRightAlignedPrompt(PSS_LABEL_WINDOW_PROMPT_MOVES, BUTTON_A, sText_Info, 156, 1);
    PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_MOVES);
    ScheduleBgCopyTilemapToVram(0);
}

static void ClearMovesPagePrompt(void)
{
    ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_MOVES);
    ScheduleBgCopyTilemapToVram(0);
}

static inline bool32 ShouldShowRename(void)
{
    return (P_SUMMARY_SCREEN_RENAME
         && !sMonSummaryScreen->lockMovesFlag
         && !sMonSummaryScreen->summary.isEgg
         && sMonSummaryScreen->mode != SUMMARY_MODE_BOX_CURSOR
         && !InBattleFactory()
         && !InSlateportBattleTent()
         && GetPlayerIDAsU32() == sMonSummaryScreen->summary.OTID);
}

static void ShowCancelOrRenamePrompt(void)
{
    if (ShouldShowRename())
        PrintRightAlignedPrompt(PSS_LABEL_WINDOW_PROMPT_CANCEL, BUTTON_START, sText_Rename, 68, 1);
    else
        PrintRightAlignedPrompt(PSS_LABEL_WINDOW_PROMPT_CANCEL, BUTTON_B, sText_Cancel, 68, 1);
}

static void CB2_ReturnToSummaryScreenFromNamingScreen(void)
{
    SetBoxMonData(GetSelectedBoxMonFromPcOrParty(), MON_DATA_NICKNAME, gStringVar2);
    ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL, gParties[B_TRAINER_PLAYER], gSpecialVar_0x8004, gPartiesCount[B_TRAINER_PLAYER] - 1, gInitialSummaryScreenCallback);
}

static void CB2_PssChangePokemonNickname(void)
{
    ChangePokemonNicknameWithCallback(CB2_ReturnToSummaryScreenFromNamingScreen);
}

/*
Montblanc note:
- The functions below are used to remove hyphens from specific words when they are split across lines.
- For example, Incineum Z has "Incine-\n" and "roar", which the previous formatter would have rendered: "Incine-roar"
- Is this 100% overkill for just a few words? Yes.
- Is it worth it to read item descriptions and not see random jank? My sanity says yes.
*/
// Lookup table for hyphen removal - stores both parts of hyphenated words
static const struct HyphenPattern {
    const char *before;
    const char *after;
} sHyphenRemovalPatterns[] = {
    {"Incine", "roar"},
    {"La", "riat"},
    {"Marsha", "dow"},
    {"Thi", "ef"},
    {"Elec", "tric"},
    {"Fight", "ing"},
    {"pro", "motes"},
    {"Decidu", "eye"},
    {"Sha", "ckle"},
    {"invigor", "ating"},
    {"Thunder", "bolt"},
    {"inde", "scribable"},
};

// Convert ASCII char to GBA charset equivalent
static u8 AsciiToGbaChar(char c)
{
    if (c >= 'A' && c <= 'Z')
        return CHAR_A + (c - 'A');
    if (c >= 'a' && c <= 'z')
        return CHAR_a + (c - 'a');
    if (c >= '0' && c <= '9')
        return CHAR_0 + (c - '0');
    return c;
}

// Check if hyphen at position should be removed for specific words
static bool32 ShouldRemoveHyphen(const u8 *p, const u8 *start, const u8 *end)
{
    // Check all patterns in the table
    for (u32 i = 0; i < ARRAY_COUNT(sHyphenRemovalPatterns); i++)
    {
        const char *before = sHyphenRemovalPatterns[i].before;
        const char *after = sHyphenRemovalPatterns[i].after;

        // Calculate lengths
        u32 beforeLen = 0, afterLen = 0;
        while (before[beforeLen]) beforeLen++;
        while (after[afterLen]) afterLen++;

        // Check bounds
        if (p < start + beforeLen)
            continue;

        // Check "before" pattern
        bool32 matches = TRUE;
        for (u32 j = 0; j < beforeLen; j++)
        {
            if (p[-(s32)beforeLen + j] != AsciiToGbaChar(before[j]))
            {
                matches = FALSE;
                break;
            }
        }

        if (!matches)
            continue;

        // Check "after" pattern
        for (u32 j = 0; j < afterLen; j++)
        {
            if (p[1 + j] != AsciiToGbaChar(after[j]))
            {
                matches = FALSE;
                break;
            }
        }

        if (matches)
            return TRUE;
    }

    // Special case: Poké-mon (with é)
    if (p >= start + 4 &&
        p[-4] == CHAR_P && p[-3] == CHAR_o && p[-2] == CHAR_k && p[-1] == CHAR_e_ACUTE &&
        p[1] == CHAR_m && p[2] == CHAR_o && p[3] == CHAR_n)
        return TRUE;

    return FALSE;
}

#endif