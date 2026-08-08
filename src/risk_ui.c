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

struct RiskUiState
{
    MainCallback savedCallback;
    u8 loadState;
    s16 xSelector;
    s16 ySelector;
    s16 xOffset;
    s16 yOffset;
    u8 selectorId;
};

enum WindowIds
{
    WIN_MONEY,
    WIN_PITY,
    WIN_PULL_1,
    WIN_PULL_10,
    WIN_COUNT
};

struct RiskIcon
{
    enum Risk *linkedRisks;
    enum Risk *unlockedRisks;
    u16 tiles[4];
    u8 linkedCount;
    u8 unlockCount;
    const u8 *name;
    const u8 *description;
};

const enum Risk sRiskMap[64][64] =
{
    [0][0] = RISK_RESET,
    [0][1] = RISK_RESET,
    [1][0] = RISK_RESET,
    [1][1] = RISK_RESET,
};

static EWRAM_DATA struct RiskUiState *sRiskUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static const u32 sBackgroundTiles[] = INCGFX_U32("graphics/risk_ui/tiles.png", ".4bpp.smol");
static const u32 sBackgroundTilemap[] = INCBIN_U32("graphics/risk_ui/tiles.bin.smolTM");
static const u16 sBackgroundPalette[] = INCGFX_U16("graphics/risk_ui/tiles.png", ".gbapal");

static const u32 sSelectorGfx[] = INCGFX_U32("graphics/risk_ui/selector.png", ".4bpp");
static const u16 sSelectorPal[] = INCGFX_U16("graphics/risk_ui/selector.png", ".gbapal");

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

static const struct WindowTemplate sRiskUiWindowTemplates[] =
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

static const u8 sRiskUiWindowFontColors[][3] =
{
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT, 3,  4},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, 1,  2},
    [FONT_FADED]  = {TEXT_COLOR_TRANSPARENT, 5,  6},
    [FONT_BLUE]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
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
static void FlipSelectedTiles(void);

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

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    ResetBgsAndClearDma3BusyFlags(0);

    InitBgsFromTemplates(0, sRiskUiBgTemplates, NELEMS(sRiskUiBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);

    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

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
        sRiskUiState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sBackgroundTilemap, sBg1TilemapBuffer);
            sRiskUiState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sBackgroundPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP * 4);
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
        FillWindowPixelBuffer(i, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        PutWindowTilemap(i);
        CopyWindowToVram(i, COPYWIN_FULL);
    }
}

static void Task_RiskUiWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_RiskUiMainInput;
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
        FlipSelectedTiles();
    }
    else if (JOY_NEW(DPAD_ANY) || JOY_HELD(DPAD_ANY))
    {
        if (JOY_NEW(DPAD_UP) || JOY_HELD(DPAD_UP))
        {
            MoveSelectorY(-4);
        }
        else if (JOY_NEW(DPAD_DOWN) || JOY_HELD(DPAD_DOWN))
        {
            MoveSelectorY(4);
        }

        if (JOY_NEW(DPAD_LEFT) || JOY_HELD(DPAD_LEFT))
        {
            MoveSelectorX(-4);
        }
        else if (JOY_NEW(DPAD_RIGHT) || JOY_HELD(DPAD_RIGHT))
        {
            MoveSelectorX(4);
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
        if (sRiskUiState->ySelector == 160 - 16)
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
            u32 tileArea = 0;

            if (currX > 31)
            {
                currX -= 32;
                tileArea += 1;
            }

            if (currY > 31)
            {
                currY -= 32;
                tileArea += 2;
            }

            u32 tileBase = 0;

            switch (tileArea)
            {
            case 0:
                break;
            case 1:
                tileBase = 1024;
                break;
            case 2:
                tileBase = 2 *1024;
                break;
            case 3:
                tileBase = 3 *1024;
                break;
            }

            tiles[y * 2 + x] = tileBase + currY * 32 + currX;
        }
    }
}

static void FlipSelectedTiles(void)
{
    u16 tiles[4];
    GetSelectedTiles(tiles);
    for (u32 i = 0; i < 4; i++)
    {
        DebugPrintf("%u", tiles[i]);
        SetTilePalette(tiles[i], 1);
    }
}
