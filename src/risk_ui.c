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

struct RiskUiState
{
    MainCallback savedCallback;
    u8 loadState;
    s16 xSelector;
    s16 ySelector;
    s16 xOffset;
    s16 yOffset;
    u8 selectorId;
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
};

static EWRAM_DATA struct RiskUiState *sRiskUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;

static const u32 sBackgroundTiles[] = INCGFX_U32("graphics/risk_ui/tiles.png", ".4bpp.smol");
static const u32 sBackgroundTilemap[] = INCBIN_U32("graphics/risk_ui/tiles.bin.smolTM");
static const u16 sBackgroundPalette[] = INCGFX_U16("graphics/risk_ui/tiles.png", ".gbapal");

static const u32 sSelectorGfx[] = INCGFX_U32("graphics/risk_ui/selector.png", ".4bpp");
static const u16 sSelectorPal[] = INCGFX_U16("graphics/risk_ui/selector.png", ".gbapal");

static const u32 sFrameGfx[] = INCGFX_U32("graphics/risk_ui/frame_tiles.png", ".4bpp.smol");
static const u32 sFrameTilemap[] = INCBIN_U32("graphics/risk_ui/frame_tiles.bin.smolTM");
static const u16 sFramePal[] = INCGFX_U16("graphics/risk_ui/frame_tiles.png", ".gbapal");

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

#define NAME_WIDTH 16
#define NAME_HEIGHT 2
#define DESCRIPTION_WIDTH 28
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
        .paletteNum = 15,
        .baseBlock = NAME_BASEBLOCK,
    },
    [WIN_RISK_DESCRIPTION] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 20,
        .width = DESCRIPTION_WIDTH,
        .height = DESCRIPTION_HEIGHT,
        .paletteNum = 15,
        .baseBlock = DESCRIPTION_BASEBLOCK
    },
    [WIN_RISK_TOTAL] =
    {
        .bg = 0,
        .tilemapLeft = 25,
        .tilemapTop = 18,
        .width = TOTAL_WIDTH,
        .height = TOTAL_HEIGHT,
        .paletteNum = 15,
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
    [FONT_BLACK]  = {2, 3,  4},
    [FONT_WHITE]  = {2, 1,  2},
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
static void MoveSelectorX(s32 distance);
static void MoveSelectorY(s32 distance);
static void GetSelectedTiles(u16 *tiles);
static void TrySelectRiskUnderCursor(void);
static inline void SetRiskInactive(enum Risk risk);
static inline void SetRiskActive(enum Risk risk);
static void ChangeTilemapPalettesBeforeLoad(void);
static enum Risk GetRiskUnderCursor(void);
static void PrintRiskData(enum Risk risk);

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
    }
    else if (risk != RISK_NONE)
    {
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
    }
}

static inline void SetRiskInactive(enum Risk risk)
{
    SetTilePalette(sRiskData[risk].tiles[0], 0);
    SetTilePalette(sRiskData[risk].tiles[1], 0);
    SetTilePalette(sRiskData[risk].tiles[2], 0);
    SetTilePalette(sRiskData[risk].tiles[3], 0);
    ClearRisk(risk);
}

static inline void SetRiskActive(enum Risk risk)
{
    SetTilePalette(sRiskData[risk].tiles[0], 1);
    SetTilePalette(sRiskData[risk].tiles[1], 1);
    SetTilePalette(sRiskData[risk].tiles[2], 1);
    SetTilePalette(sRiskData[risk].tiles[3], 1);
    SetRisk(risk);
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
                u16 palMask = 1 << 12;
                u16 currVal = tilemapPtr[tileNum] & 0xFFF;
                tilemapPtr[tileNum] = palMask | currVal;
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
                                     sRiskUiWindowFontColors[FONT_BLACK],
                                     TEXT_SKIP_DRAW,
                                     str);
    }
    CopyWindowToVram(WIN_RISK_NAME, COPYWIN_GFX);
    CopyWindowToVram(WIN_RISK_DESCRIPTION, COPYWIN_GFX);
    CopyWindowToVram(WIN_RISK_TOTAL, COPYWIN_GFX);
}
