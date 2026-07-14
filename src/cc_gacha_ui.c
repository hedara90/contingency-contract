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

struct GachaUiState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 numToPull;
    u16 pullState;
    u8 iconSpriteIds[10];
    u8 ballSpriteIds[10];
    u8 ballAnimState;
    u8 ballAnimIndex;
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

struct GachaGraphics
{
    const u32 *tiles;
    const u32 *tilemap;
    const u16 *palette;
};

static const struct GachaGraphics sGachaGraphics[] =
{
    [BANNER_ITEMS] =
    {
        .tiles = sItemsTiles,
        .tilemap = sItemsTilemap,
        .palette = sItemsPalette,
    },
    [BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT] =
    {
        .tiles = sIndomitabilityTiles,
        .tilemap = sIndomitabilityTilemap,
        .palette = sIndomitabilityPalette,
    },
    [BANNER_FURY_OF_THE_EARTHEN_CORE] =
    {
        .tiles = sFuryTiles,
        .tilemap = sFuryTilemap,
        .palette = sFuryPalette,
    },
    [BANNER_MEMORIES_OF_MONTHS_PAST] =
    {
        .tiles = sMemoriesTiles,
        .tilemap = sMemoriesTilemap,
        .palette = sMemoriesPalette,
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
        .charBaseIndex = 0,
        .mapBaseIndex = 24,
        .priority = 1,
        .screenSize = 2,
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 16,
        .priority = 2,
        .screenSize = 2,
    }
};

#define MONEY_WIDTH     6
#define MONEY_HEIGHT    2
#define PITY_WIDTH      12
#define PITY_HEIGHT     4
#define PULL_1_WIDTH    6
#define PULL_1_HEIGHT   4
#define PULL_10_WIDTH   7
#define PULL_10_HEIGHT  4

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
        .tilemapLeft = 0,
        .tilemapTop = 16,
        .width = PITY_WIDTH,
        .height = PITY_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PITY_BASEBLOCK
    },
    [WIN_PULL_1] =
    {
        .bg = 0,
        .tilemapLeft = 30 - PULL_10_WIDTH - PULL_1_WIDTH - 1,
        .tilemapTop = 16,
        .width = PULL_1_WIDTH,
        .height = PULL_1_HEIGHT,
        .paletteNum = 15,
        .baseBlock = PULL_1_BASEBLOCK
    },
    [WIN_PULL_10] =
    {
        .bg = 0,
        .tilemapLeft = 30 - PULL_10_WIDTH,
        .tilemapTop = 16,
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
    AdvanceComfyAnimations();
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
    else if (JOY_NEW(L_BUTTON))
    {
        u32 money = GetMoney(&gSaveBlock1Ptr->money);
        if (money >= PULL_1_COST)
        {
            sGachaUiState->pullState = 0;
            sGachaUiState->numToPull = 1;
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

    FillWindowPixelBuffer(WIN_MONEY, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    ConvertIntToDecimalStringN(&(str[2]), money, STR_CONV_MODE_LEFT_ALIGN, 8);
    AddTextPrinterParameterized4(WIN_MONEY,
                                 FONT_NORMAL,
                                 0, 0, 0, 0,
                                 sGachaUiWindowFontColors[FONT_WHITE],
                                 TEXT_SKIP_DRAW,
                                 str);
    CopyWindowToVram(WIN_MONEY, COPYWIN_GFX);
}

const u8 sPityStr[] = _(" pulls to\nguaranteed 6-star");

static void DrawPity(void)
{
    u32 toGuaranteed = PITY_6_STAR - gSaveBlock1Ptr->pity6;
    u8 str[32];

    u8 *strPtr = ConvertIntToDecimalStringN(str, toGuaranteed, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringCopy(strPtr, sPityStr);

    FillWindowPixelBuffer(WIN_PITY, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    AddTextPrinterParameterized4(WIN_PITY,
                                 FONT_NORMAL,
                                 0, 0, 0, 0,
                                 sGachaUiWindowFontColors[FONT_WHITE],
                                 TEXT_SKIP_DRAW,
                                 str);
    CopyWindowToVram(WIN_PITY, COPYWIN_GFX);
}

static void DrawPull(void)
{
    FillWindowPixelBuffer(WIN_PULL_1, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_PULL_10, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    enum FontColor color = FONT_WHITE;

    u32 money = GetMoney(&gSaveBlock1Ptr->money);

    if (money < PULL_1_COST)
        color = FONT_RED;

    AddTextPrinterParameterized4(WIN_PULL_1,
                                 FONT_NORMAL,
                                 0, 0, 0, 0,
                                 sGachaUiWindowFontColors[color],
                                 TEXT_SKIP_DRAW,
                                 COMPOUND_STRING("{L_BUTTON} 1 Pull\n{CLEAR_TO 20}500"));

    if (money < PULL_10_COST)
        color = FONT_RED;

    AddTextPrinterParameterized4(WIN_PULL_10,
                                 FONT_NORMAL,
                                 0, 0, 0, 0,
                                 sGachaUiWindowFontColors[color],
                                 TEXT_SKIP_DRAW,
                                 COMPOUND_STRING("{R_BUTTON} 10 Pull\n{CLEAR_TO 20}5000"));

    CopyWindowToVram(WIN_PULL_1, COPYWIN_GFX);
    CopyWindowToVram(WIN_PULL_10, COPYWIN_GFX);
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
        SetGpuReg(REG_OFFSET_BG0VOFS, 160);
        SetGpuReg(REG_OFFSET_BG1VOFS, 160);
        DrawText();
        sGachaUiState->pullState++;
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
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            for (u32 i = 0; i < sGachaUiState->numToPull; i++)
            {
                DestroySprite(&gSprites[sGachaUiState->ballSpriteIds[i]]);
                DestroySprite(&gSprites[sGachaUiState->iconSpriteIds[i]]);
                FreeSpriteTilesByTag(i);
                FreeSpritePaletteByTag(i);
                sGachaUiState->iconSpriteIds[i] = SPRITE_NONE;
                sGachaUiState->ballSpriteIds[i] = SPRITE_NONE;
            }
            FreeBallGfx(BALL_4_STAR);
            FreeBallGfx(BALL_5_STAR);
            FreeBallGfx(BALL_6_STAR);
            SetGpuReg(REG_OFFSET_BG0VOFS, 0);
            SetGpuReg(REG_OFFSET_BG1VOFS, 0);
            gTasks[taskId].func = Task_GachaUiMainInput;
        }
    }
}
