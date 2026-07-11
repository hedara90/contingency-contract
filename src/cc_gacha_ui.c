#include "cc_gacha_ui.h"
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


struct GachaUiState
{
    MainCallback savedCallback;
    u8 loadState;
    enum Banner banner;
};

enum WindowIds
{
    WIN_MONEY,
    WIN_PITY,
    WIN_PULL_1,
    WIN_PULL_10,
    WIN_COUNT
};

static EWRAM_DATA struct GachaUiState *sGachaUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static const u32 sIndomitabilityTiles[] = INCGFX_U32("graphics/gacha/default_tiles.png", ".4bpp.smol");
static const u32 sIndomitabilityTilemap[] = INCBIN_U32("graphics/gacha/default_tiles.bin.smolTM");
static const u16 sIndomitabilityPalette[] = INCGFX_U16("graphics/gacha/default_tiles.png", ".gbapal");

struct GachaGraphics
{
    const u32 *tiles;
    const u32 *tilemap;
    const u16 *palette;
};

static const struct GachaGraphics sGachaGraphics[] =
{
    [BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT] =
    {
        .tiles = sIndomitabilityTiles,
        .tilemap = sIndomitabilityTilemap,
        .palette = sIndomitabilityPalette,
    },
    [BANNER_FURY_OF_THE_EARTHERN_CORE] =
    {
        .tiles = sIndomitabilityTiles,
        .tilemap = sIndomitabilityTilemap,
        .palette = sIndomitabilityPalette,
    },
    [BANNER_MEMORIES_OF_MONTHS_PAST] =
    {
        .tiles = sIndomitabilityTiles,
        .tilemap = sIndomitabilityTilemap,
        .palette = sIndomitabilityPalette,
    },
};

static const struct BgTemplate sGachaUiBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 22,
        .priority = 1
    },
    {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 20,
        .priority = 2
    }
};

#define MONEY_WIDTH     8
#define MONEY_HEIGHT    2
#define PITY_WIDTH      8
#define PITY_HEIGHT     4
#define PULL_1_WIDTH    8
#define PULL_1_HEIGHT   2
#define PULL_10_WIDTH   8
#define PULL_10_HEIGHT  2

#define MONEY_SIZE      MONEY_WIDTH * MONEY_HEIGHT
#define PITY_SIZE       PITY_WIDTH * PITY_HEIGHT
#define PULL_1_SIZE     PULL_1_WIDTH * PULL_1_HEIGHT
#define PULL_10_SIZE     PULL_10_WIDTH * PULL_10_HEIGHT

#define MONEY_BASEBLOCK     1
#define PITY_BASEBLOCK      MONEY_BASEBLOCK + MONEY_SIZE
#define PULL_1_BASEBLOCK    PITY_BASEBLOCK + PITY_SIZE
#define PULL_10_BASEBLOCK   PULL_1_BASEBLOCK + PULL_1_SIZE

static const struct WindowTemplate sGachaUiWindowTemplates[] =
{
    [WIN_MONEY] =
    {
        .bg = 0,
        .tilemapLeft = 30 - MONEY_WIDTH,
        .tilemapTop = 0,
        .width = MONEY_WIDTH,
        .height = MONEY_HEIGHT,
        .paletteNum = 15,
        .baseBlock = MONEY_BASEBLOCK,
    },
    [WIN_PITY] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = PITY_WIDTH,
        .height = PITY_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PITY_BASEBLOCK
    },
    [WIN_PULL_1] =
    {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 1,
        .width = PULL_1_WIDTH,
        .height = PULL_1_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PULL_1_BASEBLOCK
    },
    [WIN_PULL_10] =
    {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 1,
        .width = PULL_10_WIDTH,
        .height = PULL_10_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PULL_10_BASEBLOCK
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

static const u8 sGachaUiWindowFontColors[][3] =
{
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT, 3,  4},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, 1,  2},
    [FONT_FADED]  = {TEXT_COLOR_TRANSPARENT, 5,  6},
    [FONT_BLUE]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
};

static void GachaUi_SetupCB(void);
static void GachaUi_ResetGpuRegsAndBgs(void);
static bool8 GachaUi_InitBgs(void);
static void GachaUi_FadeAndBail(void);
static void Task_GachaUiWaitFadeAndBail(u8 taskId);
static void GachaUi_VBlankCB(void);
static void GachaUi_FreeResources(void);
static void GachaUi_MainCB(void);
static bool8 GachaUi_LoadGraphics(void);
static void GachaUi_InitWindows(void);
static void Task_GachaUiWaitFadeIn(u8 taskId);
static void Task_GachaUiMainInput(u8 taskId);

static void Task_GachaUiWaitFadeAndExitGracefully(u8 taskId);

void Gacha_Init(MainCallback callback, enum Banner banner)
{
    sGachaUiState = AllocZeroed(sizeof(struct GachaUiState));
    if (sGachaUiState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sGachaUiState->savedCallback = callback;
    sGachaUiState->loadState = 0;

    SetMainCallback2(GachaUi_SetupCB);
}

void Gacha_InitFromScript(struct ScriptContext *ctx)
{
    enum Banner banner = ScriptReadWord(ctx);
    Gacha_Init(CB2_ReturnToFieldContinueScriptPlayMapMusic, banner);
}

static void GachaUi_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        GachaUi_ResetGpuRegsAndBgs();
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
        if (GachaUi_InitBgs())
        {
            sGachaUiState->loadState = 0;
            gMain.state++;
        }
        else
        {
            GachaUi_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (GachaUi_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        GachaUi_InitWindows();
        gMain.state++;
        break;
    case 5:
        CreateTask(Task_GachaUiWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(GachaUi_VBlankCB);
        SetMainCallback2(GachaUi_MainCB);
        break;
    }
}

static void GachaUi_ResetGpuRegsAndBgs(void)
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

static bool8 GachaUi_InitBgs(void)
{
    const u32 TILEMAP_BUFFER_SIZE = (1024 * 2);

    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    ResetBgsAndClearDma3BusyFlags(0);

    InitBgsFromTemplates(0, sGachaUiBgTemplates, NELEMS(sGachaUiBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);

    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}

static void GachaUi_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_GachaUiWaitFadeAndBail, 0);

    SetVBlankCallback(GachaUi_VBlankCB);
    SetMainCallback2(GachaUi_MainCB);
}

static void Task_GachaUiWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sGachaUiState->savedCallback);
        GachaUi_FreeResources();
        DestroyTask(taskId);
    }
}

static void GachaUi_VBlankCB(void)
{

    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void GachaUi_FreeResources(void)
{
    if (sGachaUiState != NULL)
    {
        Free(sGachaUiState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}

static void GachaUi_MainCB(void)
{
    AnimateSprites();
    RunTasks();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static bool8 GachaUi_LoadGraphics(void)
{
    switch (sGachaUiState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sGachaGraphics[sGachaUiState->banner].tiles, 0, 0, 0);
        sGachaUiState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sGachaGraphics[sGachaUiState->banner].tilemap, sBg1TilemapBuffer);
            sGachaUiState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sGachaGraphics[sGachaUiState->banner].palette, BG_PLTT_ID(0), PLTT_SIZE_4BPP * 4);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sGachaUiState->loadState++;
    default:
        sGachaUiState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void GachaUi_InitWindows(void)
{
    InitWindows(sGachaUiWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);

    for (u32 i = 0; i < WIN_COUNT; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        PutWindowTilemap(i);
        CopyWindowToVram(i, COPYWIN_FULL);
    }
}

static void Task_GachaUiWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_GachaUiMainInput;
}

static void Task_GachaUiMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_GachaUiWaitFadeAndExitGracefully;
    }
}

static void Task_GachaUiWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sGachaUiState->savedCallback);
        GachaUi_FreeResources();
        DestroyTask(taskId);
    }
}
