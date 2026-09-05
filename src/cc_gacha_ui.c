#include "cc_gacha_ui.h"
#include "gba/types.h"
#include "bg.h"
#include "cc_mon_management.h"
#include "comfy_anim.h"
#include "decompress.h"
#include "even_sprite.h"
#include "global.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "line_break.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "money.h"
#include "move.h"
#include "overworld.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "pokemon_icon.h"
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

#include "constants/cc_version.h"

#define MON_OFFSET_MOVEMENT 6
#define COLOR_TO_FILL TEXT_COLOR_TRANSPARENT

struct GachaUiState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 numToPull;
    u16 pullState;
    u8 iconSpriteIds[10];
    u8 ballSpriteIds[10];
    u8 indicatorIds[10];
    u16 ballAnimState;
    u16 ballAnimIndex;
    enum Banner banner;
    u16 offset;
    u16 infoState;
    u8 infoIconIds[23];
    u16 monOffset;
};

enum WindowIds
{
    WIN_MONEY,
    WIN_PITY,
    WIN_PULLS,
    WIN_ITEMS,
    WIN_COUNT
};

static EWRAM_DATA struct GachaUiState *sGachaUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA u8 *sBg2TilemapBuffer = NULL;

static const u32 sItemsTiles[] = INCGFX_U32("graphics/gacha/items_tiles.png", ".4bpp.smol");
static const u32 sItemsTilemap[] = INCBIN_U32("graphics/gacha/items_tiles.bin.smolTM");
static const u16 sItemsPalette[] = INCGFX_U16("graphics/gacha/items_tiles.png", ".gbapal");

static const u32 sIndomitabilityTiles[] = INCGFX_U32("graphics/gacha/indomitability_tiles.png", ".4bpp.smol");
static const u32 sIndomitabilityTilemap[] = INCBIN_U32("graphics/gacha/indomitability_tiles.bin.smolTM");
static const u16 sIndomitabilityPalette[] = INCGFX_U16("graphics/gacha/indomitability_tiles.png", ".gbapal");

static const u32 sFuryTiles[] = INCGFX_U32("graphics/gacha/fury_tiles.png", ".4bpp.smol");
static const u32 sFuryTilemap[] = INCBIN_U32("graphics/gacha/fury_tiles.bin.smolTM");
static const u16 sFuryPalette[] = INCGFX_U16("graphics/gacha/fury_tiles.png", ".gbapal");

static const u32 sMemoriesTiles[] = INCGFX_U32("graphics/gacha/memories_tiles.png", ".4bpp.smol");
static const u32 sMemoriesTilemap[] = INCBIN_U32("graphics/gacha/memories_tiles.bin.smolTM");
static const u16 sMemoriesPalette[] = INCGFX_U16("graphics/gacha/memories_tiles.png", ".gbapal");

static const u32 sNewGfx[] = INCGFX_U32("graphics/gacha/new.png", ".4bpp");
static const u16 sNewPal[] = INCGFX_U16("graphics/gacha/new.png", ".gbapal");

static const u32 sIndomitabilityMonTiles[] = INCGFX_U32("graphics/gacha/indomitability_Mon_tiles.png", ".4bpp.smol");
static const u32 sIndomitabilityMonTilemap[] = INCBIN_U32("graphics/gacha/indomitability_Mon_tiles.bin.smolTM");
static const u16 sIndomitabilityMonPalette[] = INCGFX_U16("graphics/gacha/indomitability_Mon_tiles.png", ".gbapal");

static const u32 sFuryMonTiles[] = INCGFX_U32("graphics/gacha/fury_Mon_tiles.png", ".4bpp.smol");
static const u32 sFuryMonTilemap[] = INCBIN_U32("graphics/gacha/fury_Mon_tiles.bin.smolTM");
static const u16 sFuryMonPalette[] = INCGFX_U16("graphics/gacha/fury_Mon_tiles.png", ".gbapal");

static const u32 sMemoriesMonTiles[] = INCGFX_U32("graphics/gacha/memories_Mon_tiles.png", ".4bpp.smol");
static const u32 sMemoriesMonTilemap[] = INCBIN_U32("graphics/gacha/memories_Mon_tiles.bin.smolTM");
static const u16 sMemoriesMonPalette[] = INCGFX_U16("graphics/gacha/memories_Mon_tiles.png", ".gbapal");

static const u32 sItemsItemsTiles[] = INCGFX_U32("graphics/gacha/items_items_tiles.png", ".4bpp.smol");
static const u32 sItemsItemsTilemap[] = INCBIN_U32("graphics/gacha/items_items_tiles.bin.smolTM");
static const u16 sItemsItemsPalette[] = INCGFX_U16("graphics/gacha/items_items_tiles.png", ".gbapal");

struct GachaGraphics
{
    const u32 *tiles;
    const u32 *tilemap;
    const u16 *palette;

    const u32 *tilesMon;
    const u32 *tilemapMon;
    const u16 *paletteMon;
};

static const struct GachaGraphics sGachaGraphics[] =
{
    [BANNER_ITEMS] =
    {
        .tiles = sItemsTiles,
        .tilemap = sItemsTilemap,
        .palette = sItemsPalette,

        .tilesMon = sItemsItemsTiles,
        .tilemapMon = sItemsItemsTilemap,
        .paletteMon = sItemsItemsPalette,
    },
    [BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT] =
    {
        .tiles = sIndomitabilityTiles,
        .tilemap = sIndomitabilityTilemap,
        .palette = sIndomitabilityPalette,

        .tilesMon = sIndomitabilityMonTiles,
        .tilemapMon = sIndomitabilityMonTilemap,
        .paletteMon = sIndomitabilityMonPalette,
    },
    [BANNER_FURY_OF_THE_EARTHEN_CORE] =
    {
        .tiles = sFuryTiles,
        .tilemap = sFuryTilemap,
        .palette = sFuryPalette,

        .tilesMon = sFuryMonTiles,
        .tilemapMon = sFuryMonTilemap,
        .paletteMon = sFuryMonPalette,
    },
    [BANNER_MEMORIES_OF_MONTHS_PAST] =
    {
        .tiles = sMemoriesTiles,
        .tilemap = sMemoriesTilemap,
        .palette = sMemoriesPalette,

        .tilesMon = sMemoriesMonTiles,
        .tilemapMon = sMemoriesMonTilemap,
        .paletteMon = sMemoriesMonPalette,
    },
};

static const u16 sStarToBall[] =
{
    [4] = BALL_4_STAR,
    [5] = BALL_5_STAR,
    [6] = BALL_6_STAR,
};

static const struct BgTemplate sGachaUiBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 3,
        .mapBaseIndex = 28,
        .priority = 0,
        .screenSize = 2,
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .priority = 2,
        .screenSize = 2,
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 26,
        .priority = 1,
        .screenSize = 2,
    },
};

#define MONEY_WIDTH     6
#define MONEY_HEIGHT    2
#define PITY_WIDTH      12
#define PITY_HEIGHT     4
#define PULLS_WIDTH     11
#define PULLS_HEIGHT    4
#define ITEM_WIDTH      10
#define ITEM_HEIGHT     2

#define MONEY_SIZE      MONEY_WIDTH * MONEY_HEIGHT
#define PITY_SIZE       PITY_WIDTH * PITY_HEIGHT
#define PULLS_SIZE      PULLS_WIDTH * PULLS_HEIGHT
#define ITEM_SIZE       ITEM_WIDTH * ITEM_HEIGHT

#define MONEY_BASEBLOCK     1
#define PITY_BASEBLOCK      MONEY_BASEBLOCK + MONEY_SIZE
#define PULLS_BASEBLOCK     PITY_BASEBLOCK + PITY_SIZE
#define ITEM_BASEBLOCK      PULLS_BASEBLOCK + PULLS_SIZE

static const struct WindowTemplate sGachaUiWindowTemplates[] =
{
    [WIN_MONEY] =
    {
        .bg = 0,
        .tilemapLeft = 30 - MONEY_WIDTH,
        .tilemapTop = 0 + 20,
        .width = MONEY_WIDTH,
        .height = MONEY_HEIGHT,
        .paletteNum = 15,
        .baseBlock = MONEY_BASEBLOCK,
    },
    [WIN_PITY] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 16 + 20,
        .width = PITY_WIDTH,
        .height = PITY_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PITY_BASEBLOCK
    },
    [WIN_PULLS] =
    {
        .bg = 0,
        .tilemapLeft = 30 - PULLS_WIDTH,
        .tilemapTop = 16 + 20,
        .width = PULLS_WIDTH,
        .height = PULLS_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PULLS_BASEBLOCK
    },
    [WIN_ITEMS] =
    {
        .bg = 0,
        .tilemapLeft = 10,
        .tilemapTop = 16 + 20 + 20,
        .width = ITEM_WIDTH,
        .height = ITEM_HEIGHT,
        .paletteNum = 15,
        .baseBlock = ITEM_BASEBLOCK,
    },
    DUMMY_WIN_TEMPLATE
};

enum FontColor
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_FADED,
    FONT_BLUE,
    FONT_RED,
};

static const u8 sGachaUiWindowFontColors[][3] =
{
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT, 3,  4},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, 1,  2},
    [FONT_FADED]  = {TEXT_COLOR_TRANSPARENT, 5,  6},
    [FONT_BLUE]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
    [FONT_RED]    = {TEXT_COLOR_TRANSPARENT, 1,  4},
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
static void DrawText(void);
static void DrawMoney(void);
static void DrawPity(void);
static void DrawPull(void);
static void Task_PullAnim(u8 taskId);
static void Task_PullAnimItem(u8 taskId);
static void Task_InfoTask(u8 taskId);
static void Task_InfoTaskItems(u8 taskId);

static void Task_GachaUiWaitFadeAndExitGracefully(u8 taskId);

void Gacha_Init(MainCallback callback, enum Banner banner)
{
    sGachaUiState = AllocZeroed(sizeof(struct GachaUiState));
    if (sGachaUiState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sGachaUiState->banner = banner;
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
        DrawText();
        gMain.state++;
        break;
    case 6:
        //  Set base offset
        SetGpuReg(REG_OFFSET_BG0VOFS, 160);
        SetGpuReg(REG_OFFSET_BG1VOFS, 160);
        SetGpuReg(REG_OFFSET_BG2VOFS, 160);
        sGachaUiState->offset = 160;
        sGachaUiState->monOffset = 160;
        CreateTask(Task_GachaUiWaitFadeIn, 0);
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 8:
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
    const u32 TILEMAP_BUFFER_SIZE = (1024 * 4);

    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    sBg2TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg2TilemapBuffer == NULL)
        return FALSE;

    ResetBgsAndClearDma3BusyFlags(0);

    InitBgsFromTemplates(0, sGachaUiBgTemplates, NELEMS(sGachaUiBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    SetBgTilemapBuffer(2, sBg2TilemapBuffer);

    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);

    ShowBg(0);
    ShowBg(1);
    ShowBg(2);

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
    if (sBg2TilemapBuffer != NULL)
    {
        Free(sBg2TilemapBuffer);
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
    AdvanceComfyAnimations();
}

static bool8 GachaUi_LoadGraphics(void)
{
    switch (sGachaUiState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sGachaGraphics[sGachaUiState->banner].tiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(2, sGachaGraphics[sGachaUiState->banner].tilesMon, 0, 0, 0);
        sGachaUiState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sGachaGraphics[sGachaUiState->banner].tilemap, sBg1TilemapBuffer);
            DecompressDataWithHeaderWram(sGachaGraphics[sGachaUiState->banner].tilemapMon, sBg2TilemapBuffer);
            sGachaUiState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sGachaGraphics[sGachaUiState->banner].palette, BG_PLTT_ID(0), PLTT_SIZE_4BPP * 2);
        LoadPalette(&sGachaGraphics[sGachaUiState->banner].paletteMon[32], BG_PLTT_ID(2), PLTT_SIZE_4BPP * 13);
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
        FillWindowPixelBuffer(i, PIXEL_FILL(COLOR_TO_FILL));
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
    else if (JOY_NEW(A_BUTTON))
    {
        sGachaUiState->infoState = 0;
        if (sGachaUiState->banner == BANNER_ITEMS)
            gTasks[taskId].func = Task_InfoTaskItems;
        else
            gTasks[taskId].func = Task_InfoTask;
    }
    else if (JOY_NEW(L_BUTTON))
    {
        u32 money = GetMoney(&gSaveBlock1Ptr->money);
        if (money >= PULL_1_COST)
        {
            sGachaUiState->pullState = 0;
            sGachaUiState->numToPull = 1;
            if (sGachaUiState->banner == BANNER_ITEMS)
                gTasks[taskId].func = Task_PullAnimItem;
            else
                gTasks[taskId].func = Task_PullAnim;
            SetMoney(&gSaveBlock1Ptr->money, money - PULL_1_COST);
        }
        else
        {
            PlaySE(SE_WALL_HIT);
        }
    }
    else if (JOY_NEW(R_BUTTON))
    {
        u32 money = GetMoney(&gSaveBlock1Ptr->money);
        if (money >= PULL_10_COST)
        {
            sGachaUiState->pullState = 0;
            sGachaUiState->numToPull = 10;
            if (sGachaUiState->banner == BANNER_ITEMS)
                gTasks[taskId].func = Task_PullAnimItem;
            else
                gTasks[taskId].func = Task_PullAnim;
            SetMoney(&gSaveBlock1Ptr->money, money - PULL_10_COST);
        }
        else
        {
            PlaySE(SE_WALL_HIT);
        }
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

static void DrawText(void)
{
    DrawMoney();
    DrawPity();
    DrawPull();
}

static void DrawMoney(void)
{
    u8 str[32];
    str[0] = CHAR_CURRENCY;
    str[1] = CHAR_SPACE;
    u32 money = GetMoney(&gSaveBlock1Ptr->money);

    FillWindowPixelBuffer(WIN_MONEY, PIXEL_FILL(COLOR_TO_FILL));
    ConvertIntToDecimalStringN(&(str[2]), money, STR_CONV_MODE_LEFT_ALIGN, 8);
    AddTextPrinterParameterized4(WIN_MONEY,
                                 FONT_NORMAL,
                                 0, 0, 0, 0,
                                 sGachaUiWindowFontColors[FONT_WHITE],
                                 TEXT_SKIP_DRAW,
                                 str);
    CopyWindowToVram(WIN_MONEY, COPYWIN_GFX);
}

const u8 sPityStr1[] = _(" pulls to");
const u8 sPityStr2[] = _("guaranteed 6-star");

static void DrawPity(void)
{
    u32 toGuaranteed;
    if (sGachaUiState->banner == BANNER_ITEMS)
        toGuaranteed = PITY_ITEM_6_STAR - gSaveBlock1Ptr->pityItem6;
    else
        toGuaranteed = PITY_6_STAR - gSaveBlock1Ptr->pity6;

    u8 str[32];

    u8 *strPtr = ConvertIntToDecimalStringN(str, toGuaranteed, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringCopy(strPtr, sPityStr1);

    FillWindowPixelBuffer(WIN_PITY, PIXEL_FILL(COLOR_TO_FILL));
    AddTextPrinterParameterized4(WIN_PITY,
                                 FONT_NORMAL,
                                 0, 7, 0, 0,
                                 sGachaUiWindowFontColors[FONT_WHITE],
                                 TEXT_SKIP_DRAW,
                                 str);
    AddTextPrinterParameterized4(WIN_PITY,
                                 FONT_NORMAL,
                                 0, 17, 0, 0,
                                 sGachaUiWindowFontColors[FONT_WHITE],
                                 TEXT_SKIP_DRAW,
                                 sPityStr2);
    CopyWindowToVram(WIN_PITY, COPYWIN_GFX);
}

static void DrawPull(void)
{
    FillWindowPixelBuffer(WIN_PULLS, PIXEL_FILL(COLOR_TO_FILL));

    enum FontColor color = FONT_WHITE;

    u32 money = GetMoney(&gSaveBlock1Ptr->money);

    if (money < PULL_1_COST)
        color = FONT_RED;

    AddTextPrinterParameterized4(WIN_PULLS,
                                 FONT_NORMAL,
                                 0, 6, 0, 0,
                                 sGachaUiWindowFontColors[color],
                                 TEXT_SKIP_DRAW,
                                 COMPOUND_STRING("{L_BUTTON} 1 Pull"));

    AddTextPrinterParameterized4(WIN_PULLS,
                                 FONT_NORMAL,
                                 65, 6, 0, 0,
                                 sGachaUiWindowFontColors[color],
                                 TEXT_SKIP_DRAW,
                                 COMPOUND_STRING("50"));

    if (money < PULL_10_COST)
        color = FONT_RED;

    AddTextPrinterParameterized4(WIN_PULLS,
                                 FONT_NORMAL,
                                 0, 18, 0, 0,
                                 sGachaUiWindowFontColors[color],
                                 TEXT_SKIP_DRAW,
                                 COMPOUND_STRING("{R_BUTTON} 10 Pull"));

    AddTextPrinterParameterized4(WIN_PULLS,
                                 FONT_NORMAL,
                                 65, 18, 0, 0,
                                 sGachaUiWindowFontColors[color],
                                 TEXT_SKIP_DRAW,
                                 COMPOUND_STRING("500"));

    CopyWindowToVram(WIN_PULLS, COPYWIN_GFX);
}

static void UpdateBall(struct Sprite *sprite)
{
    sprite->x = ReadComfyAnimValueSmooth(&gComfyAnims[sprite->data[0]]);
    if (gComfyAnims[sprite->data[0]].completed)
    {
        ReleaseComfyAnim(sprite->data[0]);
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SkipPullAnim(void)
{
    for (u32 i = 0; i < sGachaUiState->numToPull; i++)
    {
        if (gSprites[sGachaUiState->ballSpriteIds[i]].callback == UpdateBall)
        {
            gSprites[sGachaUiState->ballSpriteIds[i]].callback = SpriteCallbackDummy;
            ReleaseComfyAnim(gSprites[sGachaUiState->ballSpriteIds[i]].data[0]);
        }
        gSprites[sGachaUiState->ballSpriteIds[i]].invisible = TRUE;
        gSprites[sGachaUiState->iconSpriteIds[i]].invisible = FALSE;
    }
    sGachaUiState->pullState = 6;
    sGachaUiState->ballAnimState = 0;
    sGachaUiState->ballAnimIndex = 0;
}

static void Task_PullAnim(u8 taskId)
{
    switch (sGachaUiState->pullState)
    {
    case 0:
        if (sGachaUiState->offset > 0)
        {
            sGachaUiState->offset -= 8;
            sGachaUiState->monOffset -= MON_OFFSET_MOVEMENT;;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
        }
        else
        {
            sGachaUiState->pullState++;
        }
        break;
    case 1:
        switch (sGachaUiState->numToPull)
        {
        case 1:
            DoSinglePull(sGachaUiState->banner);
            break;
        case 10:
            Do10Pull(sGachaUiState->banner);
            break;
        }
        sGachaUiState->pullState++;
        break;
    case 2:
        //  Load ball spritesheets
        LoadBallGfx(BALL_4_STAR);
        LoadBallGfx(BALL_5_STAR);
        LoadBallGfx(BALL_6_STAR);
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            sGachaUiState->iconSpriteIds[i] = SPRITE_NONE;
            struct Even_CreateSpriteStruct cs = {0};
            cs.sprite = NULL;
            cs.tileTag = gPokeBalls[sStarToBall[gGachaResults[i].stars]].spriteTemplate.tileTag;
            cs.palette = NULL;
            cs.palTag = gPokeBalls[sStarToBall[gGachaResults[i].stars]].spriteTemplate.paletteTag;
            cs.spriteSize = SPRITE_SIZE(16x16);
            cs.spriteShape =  SPRITE_SHAPE(16x16);
            cs.posX = 260;
            cs.posY = (i / 5) * 50 + 50;
            sGachaUiState->ballSpriteIds[i] = Even_CreateSprite(&cs);

            struct ComfyAnimEasingConfig config;
            InitComfyAnimConfig_Easing(&config);
            config.durationFrames = 60;
            config.from = Q_24_8(260);
            config.to = Q_24_8((i % 5) * 48 + 24);
            config.easingFunc = ComfyAnimEasing_EaseInOutCubic;
            gSprites[sGachaUiState->ballSpriteIds[i]].data[0] = CreateComfyAnim_Easing(&config);
            gSprites[sGachaUiState->ballSpriteIds[i]].callback = UpdateBall;
        }
        sGachaUiState->pullState++;
        break;
    case 3:
        //  Set up the animations
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            sGachaUiState->iconSpriteIds[i] = SPRITE_NONE;
            struct Even_CreateSpriteStruct cs = {0};
            cs.sprite = (u32 *)gSpeciesInfo[gGachaResults[i].species].iconSprite;
            cs.tileTag = i;
            cs.palette = gMonIconPaletteTable[gSpeciesInfo[gGachaResults[i].species].iconPalIndex].data;
            cs.palTag = i;
            cs.spriteSize = SPRITE_SIZE(32x32);
            cs.spriteShape =  SPRITE_SHAPE(32x32);
            cs.posX = (i % 5) * 48 + 24;
            cs.posY = (i / 5) * 50 + 50;
            sGachaUiState->iconSpriteIds[i] = Even_CreateSprite(&cs);
            gSprites[sGachaUiState->iconSpriteIds[i]].invisible = TRUE;
        }
        sGachaUiState->pullState++;
        break;
    case 4:
        //  Wait for balls to finish animating
    {
        if (JOY_NEW(A_BUTTON))
        {
            SkipPullAnim();
            break;
        }
        //  Add handling for canceling animation with a button press
        //  and jump to final display
        bool32 animsDone = TRUE;
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            if (gSprites[sGachaUiState->ballSpriteIds[i]].callback == UpdateBall)
            {
                animsDone = FALSE;
                break;
            }
        }
        if (animsDone)
        {
            sGachaUiState->ballAnimState = 0;
            sGachaUiState->ballAnimIndex = 0;
            sGachaUiState->pullState++;
        }
        break;
    }
    case 5:
        if (JOY_NEW(A_BUTTON))
        {
            SkipPullAnim();
            break;
        }
        //  Open balls 1-by-1
        switch (sGachaUiState->ballAnimState)
        {
        case 0:
            PlaySE(SE_BALL_OPEN);
            gSprites[sGachaUiState->ballSpriteIds[sGachaUiState->ballAnimIndex]].oam.tileNum += 4;
            sGachaUiState->ballAnimState++;
            break;
        case 1 ... 10:
            sGachaUiState->ballAnimState++;
            break;
        case 11:
            gSprites[sGachaUiState->ballSpriteIds[sGachaUiState->ballAnimIndex]].oam.tileNum += 4;
            sGachaUiState->ballAnimState++;
            break;
        case 12 ... 22:
            sGachaUiState->ballAnimState++;
            break;
        case 23:
            gSprites[sGachaUiState->ballSpriteIds[sGachaUiState->ballAnimIndex]].invisible = TRUE;
            gSprites[sGachaUiState->iconSpriteIds[sGachaUiState->ballAnimIndex]].invisible = FALSE;
            sGachaUiState->ballAnimIndex++;
            sGachaUiState->ballAnimState = 0;
            break;
        }

        if (sGachaUiState->ballAnimIndex == sGachaUiState->numToPull)
            sGachaUiState->pullState++;
        break;
    case 6:
        //  Display the result indicators
        //  and give max dupe resources
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            if (gGachaResults[i].result == GIVE_RESULT_FIRST)
            {
                struct Even_CreateSpriteStruct cs = {0};
                cs.sprite = sNewGfx;
                cs.tileTag = 10;
                cs.palette = sNewPal;
                cs.palTag = 10;
                cs.spriteSize = SPRITE_SIZE(32x16);
                cs.spriteShape = SPRITE_SHAPE(32x16);
                cs.posX = (i % 5) * 48 + 24;
                cs.posY = (i / 5) * 50 + 50 - 24;
                sGachaUiState->indicatorIds[i] = Even_CreateSprite(&cs);
            }
            else
            {
                if (gGachaResults[i].result == GIVE_RESULT_CAP)
                {
                    u32 multiplier = 1;
                    switch (gGachaResults[i].stars)
                    {
                    case 5:
                        multiplier = 2;
                        break;
                    case 6:
                        multiplier = 3;
                        break;
                    }
                    //  Refund some money
                    u32 newMoney = GetMoney(&gSaveBlock1Ptr->money) + (PULL_1_COST / 4) * multiplier;
                    if (newMoney > 999999)
                        newMoney = 999999;
                    SetMoney(&gSaveBlock1Ptr->money, newMoney);
                }
                sGachaUiState->indicatorIds[i] = SPRITE_NONE;
            }
        }
        sGachaUiState->pullState++;
        break;
    case 7:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            DrawText();
            sGachaUiState->pullState = 8;
        }
        break;
    case 8:
        if (sGachaUiState->offset < 160)
        {
            sGachaUiState->offset += 8;
            sGachaUiState->monOffset += MON_OFFSET_MOVEMENT;
            for (u32 i = 0; i < sGachaUiState->numToPull; i++)
            {
                if (sGachaUiState->indicatorIds[i] != SPRITE_NONE)
                {
                    gSprites[sGachaUiState->indicatorIds[i]].y -= 8;
                    if (gSprites[sGachaUiState->indicatorIds[i]].y < -32)
                    {
                        gSprites[sGachaUiState->indicatorIds[i]].invisible = TRUE;
                    }
                }

                gSprites[sGachaUiState->ballSpriteIds[i]].y -= 8;
                if (gSprites[sGachaUiState->ballSpriteIds[i]].y < -32)
                {
                    gSprites[sGachaUiState->ballSpriteIds[i]].invisible = TRUE;
                }
                gSprites[sGachaUiState->iconSpriteIds[i]].y -= 8;
                if (gSprites[sGachaUiState->iconSpriteIds[i]].y < -32)
                {
                    gSprites[sGachaUiState->iconSpriteIds[i]].invisible = TRUE;
                }
            }
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
        }
        else
        {
            ReleaseComfyAnims();
            for (u32 i = 0; i < sGachaUiState->numToPull; i++)
            {
                if (sGachaUiState->indicatorIds[i] != SPRITE_NONE)
                    DestroySprite(&gSprites[sGachaUiState->indicatorIds[i]]);

                DestroySprite(&gSprites[sGachaUiState->ballSpriteIds[i]]);
                DestroySprite(&gSprites[sGachaUiState->iconSpriteIds[i]]);
                FreeSpriteTilesByTag(i);
                FreeSpritePaletteByTag(i);
                sGachaUiState->iconSpriteIds[i] = SPRITE_NONE;
                sGachaUiState->ballSpriteIds[i] = SPRITE_NONE;
            }
            FreeSpriteTilesByTag(10);
            FreeSpritePaletteByTag(10);
            FreeBallGfx(BALL_4_STAR);
            FreeBallGfx(BALL_5_STAR);
            FreeBallGfx(BALL_6_STAR);
            gTasks[taskId].func = Task_GachaUiMainInput;
        }
        break;
    }
}

static void SkipItemPullAnim(void)
{
    for (u32 i = 0; i < sGachaUiState->numToPull; i++)
    {
        if (gSprites[sGachaUiState->ballSpriteIds[i]].callback == UpdateBall)
        {
            gSprites[sGachaUiState->ballSpriteIds[i]].callback = SpriteCallbackDummy;
            ReleaseComfyAnim(gSprites[sGachaUiState->ballSpriteIds[i]].data[0]);
        }
        gSprites[sGachaUiState->ballSpriteIds[i]].x = (i  % 5) * 48 + 24 + 4;
    }
    sGachaUiState->pullState = 4;
    sGachaUiState->ballAnimState = 0;
    sGachaUiState->ballAnimIndex = 0;
}

static void Task_PullAnimItem(u8 taskId)
{
    switch (sGachaUiState->pullState)
    {
    case 0:
        if (sGachaUiState->offset > 0)
        {
            sGachaUiState->offset -= 8;
            sGachaUiState->monOffset -= MON_OFFSET_MOVEMENT;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
        }
        else
        {
            sGachaUiState->pullState++;
        }
        break;
    case 1:
        switch (sGachaUiState->numToPull)
        {
        case 1:
            DoSinglePull(sGachaUiState->banner);
            break;
        case 10:
            Do10Pull(sGachaUiState->banner);
            break;
        }
        sGachaUiState->pullState++;
        sGachaUiState->ballAnimState = 0;
        sGachaUiState->ballAnimIndex = 0;
        break;
    case 2:
        //  Set up all item sprites
        u32 *fuckingItemGfxBuffer = Alloc(32 * 32 / 2);
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            DecompressDataWithHeaderWram(gItemsInfo[gGachaResults[i].item].iconPic, fuckingItemGfxBuffer);
            for (u32 i = 0; i < 8; i++)
            {
                fuckingItemGfxBuffer[8 * 10 + i] = fuckingItemGfxBuffer[8 * 8 + i];
                fuckingItemGfxBuffer[8 * 9 + i] = fuckingItemGfxBuffer[8 * 7 + i];
                fuckingItemGfxBuffer[8 * 8 + i] = fuckingItemGfxBuffer[8 * 6 + i];

                fuckingItemGfxBuffer[8 * 6 + i] = fuckingItemGfxBuffer[8 * 5 + i];
                fuckingItemGfxBuffer[8 * 5 + i] = fuckingItemGfxBuffer[8 * 4 + i];
                fuckingItemGfxBuffer[8 * 4 + i] = fuckingItemGfxBuffer[8 * 3 + i];

                fuckingItemGfxBuffer[8 * 3 + i] = 0;
                fuckingItemGfxBuffer[8 * 7 + i] = 0;
                fuckingItemGfxBuffer[8 * 11 + i] = 0;
                fuckingItemGfxBuffer[8 * 12 + i] = 0;
                fuckingItemGfxBuffer[8 * 13 + i] = 0;
                fuckingItemGfxBuffer[8 * 14 + i] = 0;
                fuckingItemGfxBuffer[8 * 15 + i] = 0;
            }
            struct Even_CreateSpriteStruct cs = {0};
            cs.sprite = fuckingItemGfxBuffer;
            cs.tileTag = i;
            cs.palette = gItemsInfo[gGachaResults[i].item].iconPalette;
            cs.palTag = i;
            cs.spriteSize = SPRITE_SIZE(32x32);
            cs.spriteShape =  SPRITE_SHAPE(32x32);
            cs.posX = 260;
            cs.posY = (i / 5) * 50 + 50 + 4;
            sGachaUiState->ballSpriteIds[i] = Even_CreateSprite(&cs);
        }
        Free(fuckingItemGfxBuffer);
        sGachaUiState->ballAnimIndex = 0;
        sGachaUiState->ballAnimState = 0;
        sGachaUiState->pullState++;
        break;
    case 3:
        //  Slide in the items one by one
        if (JOY_NEW(A_BUTTON))
        {
            SkipItemPullAnim();
            break;
        }

        if (sGachaUiState->ballAnimIndex < sGachaUiState->numToPull && sGachaUiState->ballAnimState % 10 == 0)
        {
            struct ComfyAnimEasingConfig config;
            InitComfyAnimConfig_Easing(&config);
            config.durationFrames = 60;
            config.from = Q_24_8(260);
            config.to = Q_24_8((sGachaUiState->ballAnimIndex % 5) * 48 + 24 + 4);
            config.easingFunc = ComfyAnimEasing_EaseInOutCubic;
            gSprites[sGachaUiState->ballSpriteIds[sGachaUiState->ballAnimIndex]].data[0] = CreateComfyAnim_Easing(&config);
            gSprites[sGachaUiState->ballSpriteIds[sGachaUiState->ballAnimIndex]].callback = UpdateBall;
            sGachaUiState->ballAnimIndex++;
        }

        sGachaUiState->ballAnimState++;

        bool32 animsDone = TRUE;
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            if (gSprites[sGachaUiState->ballSpriteIds[i]].callback == UpdateBall)
            {
                animsDone = FALSE;
                break;
            }
        }

        if (animsDone)
        {
            sGachaUiState->ballAnimState = 0;
            sGachaUiState->ballAnimIndex = 0;
            sGachaUiState->pullState++;
        }
        break;
    case 4:
        //  Display the result indicators
        //  and give dupe resources
        for (u32 i = 0; i < sGachaUiState->numToPull; i++)
        {
            if (gGachaResults[i].result == GIVE_RESULT_FIRST)
            {
                struct Even_CreateSpriteStruct cs = {0};
                cs.sprite = sNewGfx;
                cs.tileTag = 10;
                cs.palette = sNewPal;
                cs.palTag = 10;
                cs.spriteSize = SPRITE_SIZE(32x16);
                cs.spriteShape = SPRITE_SHAPE(32x16);
                cs.posX = (i % 5) * 48 + 24;
                cs.posY = (i / 5) * 50 + 50 - 24;
                sGachaUiState->indicatorIds[i] = Even_CreateSprite(&cs);
            }
            else
            {
                //  Give some BP
                u32 newBp = gSaveBlock2Ptr->frontier.battlePoints;
                switch (gGachaResults[i].stars)
                {
                case 4:
                    newBp += 1;
                    break;
                case 5:
                    newBp += 4;
                    break;
                case 6:
                    newBp += 16;
                    break;
                }
                if (newBp > 1999)
                    newBp = 1999;
                gSaveBlock2Ptr->frontier.battlePoints = newBp;
                sGachaUiState->indicatorIds[i] = SPRITE_NONE;
            }
        }
        sGachaUiState->pullState++;
        break;
    case 5:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            DrawText();
            sGachaUiState->pullState = 6;
        }
        break;
    case 6:
        if (sGachaUiState->offset < 160)
        {
            sGachaUiState->offset += 8;
            sGachaUiState->monOffset += MON_OFFSET_MOVEMENT;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
            for (u32 i = 0; i < sGachaUiState->numToPull; i++)
            {
                if (sGachaUiState->indicatorIds[i] != SPRITE_NONE)
                {
                    gSprites[sGachaUiState->indicatorIds[i]].y -= 8;
                    if (gSprites[sGachaUiState->indicatorIds[i]].y < -32)
                    {
                        gSprites[sGachaUiState->indicatorIds[i]].invisible = TRUE;
                    }
                }
                gSprites[sGachaUiState->ballSpriteIds[i]].y -= 8;
                if (gSprites[sGachaUiState->ballSpriteIds[i]].y < -32)
                {
                    gSprites[sGachaUiState->ballSpriteIds[i]].invisible = TRUE;
                }
            }
        }
        else
        {
            ReleaseComfyAnims();
            for (u32 i = 0; i < sGachaUiState->numToPull; i++)
            {
                if (sGachaUiState->indicatorIds[i] != SPRITE_NONE)
                    DestroySprite(&gSprites[sGachaUiState->indicatorIds[i]]);
                DestroySprite(&gSprites[sGachaUiState->ballSpriteIds[i]]);
                FreeSpriteTilesByTag(i);
                FreeSpritePaletteByTag(i);
                sGachaUiState->ballSpriteIds[i] = SPRITE_NONE;
            }
            FreeSpriteTilesByTag(10);
            FreeSpritePaletteByTag(10);
            gTasks[taskId].func = Task_GachaUiMainInput;
        }
        break;
    }
}

static void SpriteCB_Dummy(struct Sprite *sprite)
{
}

static void Task_InfoTask(u8 taskId)
{
    switch (sGachaUiState->infoState)
    {
    case 0:
    {
        struct BannerInfo info = GetBannerInfo(sGachaUiState->banner, 6);
        LoadMonIconPalettes();
        for (u32 i = 0; i < info.count; i++)
        {
            sGachaUiState->infoIconIds[i] = CreateMonIcon(info.species[i], SpriteCB_Dummy, 70 + i * 32, 24 + 160, 0 , 0);
            gSprites[sGachaUiState->infoIconIds[i]].invisible = TRUE;
        }

        info = GetBannerInfo(sGachaUiState->banner, 5);
        for (u32 i = 0; i < info.count; i++)
        {
            sGachaUiState->infoIconIds[2 + i] = CreateMonIcon(info.species[i], SpriteCB_Dummy, 40 + i * 27, 63 + 160, 0 , 0);
            gSprites[sGachaUiState->infoIconIds[2 + i]].invisible = TRUE;
        }
        info = GetBannerInfo(sGachaUiState->banner, 4);
        for (u32 i = 0; i < info.count - 7; i++)
        {
            sGachaUiState->infoIconIds[10 + i] = CreateMonIcon(info.species[i], SpriteCB_Dummy, 32 + i * 34, 113 + 160, 0 , 0);
            gSprites[sGachaUiState->infoIconIds[10 + i]].invisible = TRUE;
        }
        for (u32 i = 0; i < 7; i++)
        {
            sGachaUiState->infoIconIds[16 + i] = CreateMonIcon(info.species[6 + i], SpriteCB_Dummy, 16 + i * 34, 136 + 160, 0 , 0);
            gSprites[sGachaUiState->infoIconIds[16 + i]].invisible = TRUE;
        }

        sGachaUiState->infoState++;
        break;
    }
        break;
    case 1:
        if (sGachaUiState->offset < 328)
        {
            sGachaUiState->offset += 8;
            sGachaUiState->monOffset += MON_OFFSET_MOVEMENT;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
            if (sGachaUiState->offset < 328)
            {
                for (u32 i = 0; i < 23; i++)
                {
                    gSprites[sGachaUiState->infoIconIds[i]].y -= 8;
                    if (gSprites[sGachaUiState->infoIconIds[i]].y < 184)
                    {
                        gSprites[sGachaUiState->infoIconIds[i]].invisible = FALSE;
                    }
                }
            }
        }
        else
        {
            sGachaUiState->infoState++;
        }
        break;
    case 2:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 3:
        if (sGachaUiState->offset > 160)
        {
            sGachaUiState->offset -= 8;
            sGachaUiState->monOffset -= MON_OFFSET_MOVEMENT;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
            for (u32 i = 0; i < 23; i++)
            {
                gSprites[sGachaUiState->infoIconIds[i]].y += 8;
                if (gSprites[sGachaUiState->infoIconIds[i]].y > 184)
                {
                    gSprites[sGachaUiState->infoIconIds[i]].invisible = TRUE;
                }
            }
        }
        else
        {
            FreeMonIconPalettes();
            for (u32 i = 0; i < 23; i++)
            {
                FreeAndDestroyMonIconSprite(&gSprites[sGachaUiState->infoIconIds[i]]);
            }
            gTasks[taskId].func = Task_GachaUiMainInput;
        }
        break;
    }
}

static u32 ShowItem(enum Item item, s32 x, s32 y, u32 index)
{
    u32 fuckingItemGfxBuffer[32 * 16];
    DecompressDataWithHeaderWram(gItemsInfo[item].iconPic, fuckingItemGfxBuffer);
    for (u32 i = 0; i < 8; i++)
    {
        fuckingItemGfxBuffer[8 * 10 + i] = fuckingItemGfxBuffer[8 * 8 + i];
        fuckingItemGfxBuffer[8 * 9 + i] = fuckingItemGfxBuffer[8 * 7 + i];
        fuckingItemGfxBuffer[8 * 8 + i] = fuckingItemGfxBuffer[8 * 6 + i];

        fuckingItemGfxBuffer[8 * 6 + i] = fuckingItemGfxBuffer[8 * 5 + i];
        fuckingItemGfxBuffer[8 * 5 + i] = fuckingItemGfxBuffer[8 * 4 + i];
        fuckingItemGfxBuffer[8 * 4 + i] = fuckingItemGfxBuffer[8 * 3 + i];

        fuckingItemGfxBuffer[8 * 3 + i] = 0;
        fuckingItemGfxBuffer[8 * 7 + i] = 0;
        fuckingItemGfxBuffer[8 * 11 + i] = 0;
        fuckingItemGfxBuffer[8 * 12 + i] = 0;
        fuckingItemGfxBuffer[8 * 13 + i] = 0;
        fuckingItemGfxBuffer[8 * 14 + i] = 0;
        fuckingItemGfxBuffer[8 * 15 + i] = 0;
    }

    struct Even_CreateSpriteStruct cs = {0};
    cs.sprite = fuckingItemGfxBuffer;
    cs.tileTag = index;
    cs.palette = gItemsInfo[item].iconPalette;
    cs.palTag = index;
    cs.spriteSize = SPRITE_SIZE(32x32);
    cs.spriteShape =  SPRITE_SHAPE(32x32);
    cs.posX = x;
    cs.posY = y;
    return Even_CreateSprite(&cs);
}

static void Task_InfoTaskItems(u8 taskId)
{
    struct BannerInfo info;
    switch (sGachaUiState->infoState)
    {
    case 0:
        //  Create 6-star item icons
        info = GetBannerInfo(BANNER_ITEMS, 6);
        FillWindowPixelBuffer(WIN_ITEMS, PIXEL_FILL(COLOR_TO_FILL));
        AddTextPrinterParameterized4(WIN_ITEMS,
                                     FONT_NORMAL,
                                     3, 0, 0, 0,
                                     sGachaUiWindowFontColors[FONT_WHITE],
                                     TEXT_SKIP_DRAW,
                                     COMPOUND_STRING("6-star items"));
        CopyWindowToVram(WIN_ITEMS, COPYWIN_GFX);

        for (u32 i = 0; i < 6; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[i], 40 + 32 * i, 80 + 160, i);
            gSprites[sGachaUiState->infoIconIds[i]].invisible = TRUE;
        }
        sGachaUiState->infoState++;
        break;
    case 1:
        //  Slide down
        if (sGachaUiState->offset < 328)
        {
            sGachaUiState->offset += 8;
            sGachaUiState->monOffset += MON_OFFSET_MOVEMENT;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
            if (sGachaUiState->offset < 328)
            {
                for (u32 i = 0; i < 6; i++)
                {
                    gSprites[sGachaUiState->infoIconIds[i]].y -= 8;
                    if (gSprites[sGachaUiState->infoIconIds[i]].y < 184)
                    {
                        gSprites[sGachaUiState->infoIconIds[i]].invisible = FALSE;
                    }
                }
            }
        }
        else
        {
            sGachaUiState->infoState++;
        }
        break;
    case 2:
        //  Wait for input
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON) || JOY_NEW(START_BUTTON) || JOY_NEW(SELECT_BUTTON)
         || JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON)
         || JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 3:
        for (u32 i = 0; i < 6; i++)
        {
            DestroySprite(&gSprites[sGachaUiState->infoIconIds[i]]);
            FreeSpriteTilesByTag(i);
            FreeSpritePaletteByTag(i);
        }

        info = GetBannerInfo(BANNER_ITEMS, 5);
        FillWindowPixelBuffer(WIN_ITEMS, PIXEL_FILL(COLOR_TO_FILL));
        AddTextPrinterParameterized4(WIN_ITEMS,
                                     FONT_NORMAL,
                                     3, 0, 0, 0,
                                     sGachaUiWindowFontColors[FONT_WHITE],
                                     TEXT_SKIP_DRAW,
                                     COMPOUND_STRING("5-star items"));
        CopyWindowToVram(WIN_ITEMS, COPYWIN_GFX);
        for (u32 i = 0; i < 12; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[i], 40 + 32 * (i % 6), 60 + 32 * (i / 6), i);
        }
        sGachaUiState->infoState++;
        break;
    case 4:
        //  Wait for input
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON) || JOY_NEW(START_BUTTON) || JOY_NEW(SELECT_BUTTON)
         || JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON)
         || JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 5:
        for (u32 i = 0; i < 12; i++)
        {
            DestroySprite(&gSprites[sGachaUiState->infoIconIds[i]]);
            FreeSpriteTilesByTag(i);
            FreeSpritePaletteByTag(i);
        }

        info = GetBannerInfo(BANNER_ITEMS, 5);
        for (u32 i = 0; i < 6; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[12 + i], 40 + 32 * i, 60, i);
        }
        for (u32 i = 6; i < 11; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[12 + i], 40 + 16 + 32 * (i - 6), 60 + 32, i);
        }
        sGachaUiState->infoState++;
        break;
    case 6:
        //  Wait for input
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON) || JOY_NEW(START_BUTTON) || JOY_NEW(SELECT_BUTTON)
         || JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON)
         || JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 7:
        for (u32 i = 0; i < 11; i++)
        {
            DestroySprite(&gSprites[sGachaUiState->infoIconIds[i]]);
            FreeSpriteTilesByTag(i);
            FreeSpritePaletteByTag(i);
        }

        info = GetBannerInfo(BANNER_ITEMS, 4);
        FillWindowPixelBuffer(WIN_ITEMS, PIXEL_FILL(COLOR_TO_FILL));
        AddTextPrinterParameterized4(WIN_ITEMS,
                                     FONT_NORMAL,
                                     3, 0, 0, 0,
                                     sGachaUiWindowFontColors[FONT_WHITE],
                                     TEXT_SKIP_DRAW,
                                     COMPOUND_STRING("4-star items"));
        CopyWindowToVram(WIN_ITEMS, COPYWIN_GFX);
        for (u32 i = 0; i < 14; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[i], 24 + 32 * (i % 7), 60 + 32 * (i / 7), i);
        }
        sGachaUiState->infoState++;
        break;
    case 8:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON) || JOY_NEW(START_BUTTON) || JOY_NEW(SELECT_BUTTON)
         || JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON)
         || JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 9:
        for (u32 i = 0; i < 14; i++)
        {
            DestroySprite(&gSprites[sGachaUiState->infoIconIds[i]]);
            FreeSpriteTilesByTag(i);
            FreeSpritePaletteByTag(i);
        }

        info = GetBannerInfo(BANNER_ITEMS, 4);
        for (u32 i = 0; i < 14; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[14 + i], 24 + 32 * (i % 7), 60 + 32 * (i / 7), i);
        }
        sGachaUiState->infoState++;
        break;
    case 10:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON) || JOY_NEW(START_BUTTON) || JOY_NEW(SELECT_BUTTON)
         || JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON)
         || JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 11:
        for (u32 i = 0; i < 14; i++)
        {
            DestroySprite(&gSprites[sGachaUiState->infoIconIds[i]]);
            FreeSpriteTilesByTag(i);
            FreeSpritePaletteByTag(i);
        }

        info = GetBannerInfo(BANNER_ITEMS, 4);
        for (u32 i = 0; i < 14; i++)
        {
            sGachaUiState->infoIconIds[i] = ShowItem(info.items[28 + i], 24 + 32 * (i % 7), 60 + 32 * (i / 7), i);
        }
        sGachaUiState->infoState++;
        break;
    case 12:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON) || JOY_NEW(START_BUTTON) || JOY_NEW(SELECT_BUTTON)
         || JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON)
         || JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
        {
            sGachaUiState->infoState++;
        }
        break;
    case 13:
        //  slide away
        if (sGachaUiState->offset > 160)
        {
            sGachaUiState->offset -= 8;
            sGachaUiState->monOffset -= MON_OFFSET_MOVEMENT;
            SetGpuReg(REG_OFFSET_BG0VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG1VOFS, sGachaUiState->offset);
            SetGpuReg(REG_OFFSET_BG2VOFS, sGachaUiState->monOffset);
            for (u32 i = 0; i < 14; i++)
            {
                gSprites[sGachaUiState->infoIconIds[i]].y += 8;
                if (gSprites[sGachaUiState->infoIconIds[i]].y > 184)
                {
                    gSprites[sGachaUiState->infoIconIds[i]].invisible = TRUE;
                }
            }
        }
        else
        {
            for (u32 i = 0; i < 14; i++)
            {
                DestroySprite(&gSprites[sGachaUiState->infoIconIds[i]]);
                FreeSpriteTilesByTag(i);
                FreeSpritePaletteByTag(i);
            }
            gTasks[taskId].func = Task_GachaUiMainInput;
        }
        break;
    }
}
