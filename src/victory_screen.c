#include "victory_screen.h"
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
#include "risk.h"
#include "even_sprite.h"
#include "pokemon_icon.h"


struct VictoryScreenState
{
    MainCallback savedCallback;
    u8 loadState;
    bool8 fromSave;
    enum Gauntlet gauntlet;
    u8 monSpriteIds[6];
    u8 potSpriteIds[6];
    u8 riskSpriteIds[44];
    u8 numDupes[6];
    enum Species species[6];
    struct Risks savedRisks;
};

enum WindowIds
{
    WIN_TITLE,
    WIN_RISK,
    WIN_TRAINER,
    WIN_COUNT
};

static EWRAM_DATA struct VictoryScreenState *sVictoryScreenState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static const u32 sTiles[] = INCGFX_U32("graphics/victory_screen/tiles.png", ".4bpp.smol");
static const u32 sTilemap[] = INCGFX_U32("graphics/victory_screen/tiles.bin", ".smolTM");
static const u16 sPalette[] = INCGFX_U16("graphics/victory_screen/tiles.png", ".gbapal");

static const u32 sRiskGfx[] = INCGFX_U32("graphics/victory_screen/risks.png", ".4bpp", "-mwidth 2 -mheight 2");
static const u16 sRiskPal[] = INCGFX_U16("graphics/victory_screen/risks.png", ".gbapal");

static const struct BgTemplate sVictoryScreenBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .priority = 1
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 28,
        .priority = 2
    }
};

#define TITLE_WIDTH 32
#define TITLE_HEIGHT 2
#define RISK_WIDTH 4
#define RISK_HEIGHT 2
#define TRAINER_WIDTH 32
#define TRAINER_HEIGHT 2

#define TITLE_SIZE TITLE_WIDTH * TITLE_HEIGHT
#define RISK_SIZE RISK_WIDTH * RISK_HEIGHT
#define TRAINER_SIZE TRAINER_WIDTH * TRAINER_HEIGHT

#define TITLE_BASEBLOCK 1
#define RISK_BASEBLOCK TITLE_BASEBLOCK + TITLE_SIZE
#define TRAINER_BASEBLOCK RISK_BASEBLOCK + RISK_SIZE

static const struct WindowTemplate sVictoryScreenWindowTemplates[] =
{
    [WIN_TITLE] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = TITLE_WIDTH,
        .height = TITLE_HEIGHT,
        .paletteNum = 15,
        .baseBlock = TITLE_BASEBLOCK,
    },
    [WIN_RISK] =
    {
        .bg = 0,
        .tilemapLeft = 30 - 4,
        .tilemapTop = 18,
        .width = RISK_WIDTH,
        .height = RISK_HEIGHT,
        .paletteNum = 15,
        .baseBlock = RISK_BASEBLOCK,
    },
    [WIN_TRAINER] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 2,
        .width = TRAINER_WIDTH,
        .height = TRAINER_HEIGHT,
        .paletteNum = 15,
        .baseBlock = TRAINER_BASEBLOCK,
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

static const u8 sVictoryScreenWindowFontColors[][3] =
{
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT, 3,  4},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, 1,  2},
    [FONT_FADED]  = {TEXT_COLOR_TRANSPARENT, 5,  6},
    [FONT_BLUE]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
};

static void VictoryScreen_SetupCB(void);
static void VictoryScreen_ResetGpuRegsAndBgs(void);
static bool8 VictoryScreen_InitBgs(void);
static void VictoryScreen_FadeAndBail(void);
static void Task_VictoryScreenWaitFadeAndBail(u8 taskId);
static void VictoryScreen_VBlankCB(void);
static void VictoryScreen_FreeResources(void);
static void VictoryScreen_MainCB(void);
static bool8 VictoryScreen_LoadGraphics(void);
static void VictoryScreen_InitWindows(void);
static void Task_VictoryScreenWaitFadeIn(u8 taskId);
static void Task_VictoryScreenMainInput(u8 taskId);

static void VictoryScreen_PrintText(void);
static void VictoryScreen_ShowMons(void);
static void VictoryScreen_LoadRisks(void);

static void Task_VictoryScreenWaitFadeAndExitGracefully(u8 taskId);

void VictoryScreen_Init(MainCallback callback, enum Gauntlet gauntlet, bool32 fromSave)
{
    sVictoryScreenState = AllocZeroed(sizeof(struct VictoryScreenState));
    if (sVictoryScreenState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    for (u32 i = 0; i < 6; i++)
    {
        sVictoryScreenState->monSpriteIds[i] = SPRITE_NONE;
        sVictoryScreenState->potSpriteIds[i] = SPRITE_NONE;
    }

    for (u32 i = 0; i < 44; i++)
    {
        sVictoryScreenState->riskSpriteIds[i] = SPRITE_NONE;
    }

    sVictoryScreenState->savedCallback = callback;
    sVictoryScreenState->loadState = 0;
    sVictoryScreenState->gauntlet = gauntlet;
    sVictoryScreenState->fromSave = fromSave;

    if (fromSave)
    {
        sVictoryScreenState->savedRisks = gSaveBlock1Ptr->risks;
        gSaveBlock1Ptr->risks = gSaveBlock1Ptr->wins[gauntlet].risks;
        for (u32 i = 0; i < 6; i++)
        {
            sVictoryScreenState->species[i] = gSaveBlock1Ptr->wins[gauntlet].species[i];
            sVictoryScreenState->numDupes[i] = gSaveBlock1Ptr->wins[gauntlet].numDupes[i];
        }
    }
    else
    {
        for (u32 i = 0; i < 6; i++)
        {
            struct Pokemon *mon = &gParties[0][i];
            sVictoryScreenState->species[i] = GetMonData(mon, MON_DATA_SPECIES);
            if (GetMonData(mon, MON_DATA_IS_SHINY))
            {
                sVictoryScreenState->numDupes[i] = 5;
            }
            else
            {
                switch (GetMonData(mon, MON_DATA_MARKINGS))
                {
                default:
                    sVictoryScreenState->numDupes[i] = 0;
                    break;
                case 1:
                    sVictoryScreenState->numDupes[i] = 1;
                    break;
                case 3:
                    sVictoryScreenState->numDupes[i] = 2;
                    break;
                case 7:
                    sVictoryScreenState->numDupes[i] = 3;
                    break;
                case 15:
                    sVictoryScreenState->numDupes[i] = 4;
                    break;
                }
            }
        }
    }

    SetMainCallback2(VictoryScreen_SetupCB);
}

void VictoryScreen_InitFromScript(struct ScriptContext *ctx)
{
    enum Gauntlet gauntlet = ScriptReadByte(ctx);
    bool32 fromSave = ScriptReadByte(ctx);
    VictoryScreen_Init(CB2_ReturnToFieldContinueScriptPlayMapMusic, gauntlet, fromSave);
}

static void VictoryScreen_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        VictoryScreen_ResetGpuRegsAndBgs();
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
        if (VictoryScreen_InitBgs())
        {
            sVictoryScreenState->loadState = 0;
            gMain.state++;
        }
        else
        {
            VictoryScreen_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (VictoryScreen_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        VictoryScreen_InitWindows();
        gMain.state++;
        break;
    case 5:
        //  Print text
        VictoryScreen_PrintText();
        gMain.state++;
        break;
    case 6:
        //  Load mon sprites and potentials
        VictoryScreen_ShowMons();
        gMain.state++;
        break;
    case 7:
        //  Load risk sprites
        VictoryScreen_LoadRisks();
        gMain.state++;
        break;
    case 8:
        CreateTask(Task_VictoryScreenWaitFadeIn, 0);
        gMain.state++;
        break;
    case 9:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 10:
        SetVBlankCallback(VictoryScreen_VBlankCB);
        SetMainCallback2(VictoryScreen_MainCB);
        break;
    }
}

static void VictoryScreen_ResetGpuRegsAndBgs(void)
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

static bool8 VictoryScreen_InitBgs(void)
{
    const u32 TILEMAP_BUFFER_SIZE = (1024 * 2);

    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    ResetBgsAndClearDma3BusyFlags(0);

    InitBgsFromTemplates(0, sVictoryScreenBgTemplates, NELEMS(sVictoryScreenBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);

    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}

static void VictoryScreen_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_VictoryScreenWaitFadeAndBail, 0);

    SetVBlankCallback(VictoryScreen_VBlankCB);
    SetMainCallback2(VictoryScreen_MainCB);
}

static void Task_VictoryScreenWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sVictoryScreenState->savedCallback);
        VictoryScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static void VictoryScreen_VBlankCB(void)
{

    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void VictoryScreen_FreeResources(void)
{
    if (sVictoryScreenState != NULL)
    {
        if (sVictoryScreenState->fromSave)
        {
            gSaveBlock1Ptr->risks = sVictoryScreenState->savedRisks;
        }
        else
        {
            //  Not viewing from the save, which means that it's from a newly won gauntlet
            //  so it should be saved to the saveblock if it's better for the gauntlet
            u32 currentRiskTotal = GetTotalTiskValue();
            struct Risks savedRisks = gSaveBlock1Ptr->risks;
            gSaveBlock1Ptr->risks = gSaveBlock1Ptr->wins[sVictoryScreenState->gauntlet].risks;
            u32 oldRiskTotal = GetTotalTiskValue();
            gSaveBlock1Ptr->risks = savedRisks;
            if (currentRiskTotal > oldRiskTotal)
            {
                struct SavedGauntletWin *win = &gSaveBlock1Ptr->wins[sVictoryScreenState->gauntlet];
                win->risks = savedRisks;
                for (u32 i = 0; i < 6; i++)
                {
                    win->species[i] = sVictoryScreenState->species[i];
                    win->numDupes[i] = sVictoryScreenState->numDupes[i];
                }
            }
        }
        Free(sVictoryScreenState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }

    for (u32 i = 0; i < 6; i++)
    {
        if (sVictoryScreenState->monSpriteIds[i] != SPRITE_NONE)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sVictoryScreenState->monSpriteIds[i]]);
        }
        if (sVictoryScreenState->potSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sVictoryScreenState->potSpriteIds[i]]);
            FreeSpriteTilesByTag(6 + i);
        }
        FreeMonIconPalettes();
    }

    FreeSpritePaletteByTag(6);
    FreeSpritePaletteByTag(12);
    for (u32 i = 0; i < 44; i++)
    {
        if (sVictoryScreenState->riskSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sVictoryScreenState->riskSpriteIds[i]]);
            FreeSpriteTilesByTag(12 + i);
        }
    }

    FreeAllWindowBuffers();
    ResetSpriteData();
}

static void VictoryScreen_MainCB(void)
{
    AnimateSprites();
    RunTasks();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static bool8 VictoryScreen_LoadGraphics(void)
{
    switch (sVictoryScreenState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sTiles, 0, 0, 0);
        sVictoryScreenState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sTilemap, sBg1TilemapBuffer);
            sVictoryScreenState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP * 4);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sVictoryScreenState->loadState++;
    default:
        sVictoryScreenState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void VictoryScreen_InitWindows(void)
{
    InitWindows(sVictoryScreenWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);

    for (u32 i = 0; i < WIN_COUNT; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        PutWindowTilemap(i);
        CopyWindowToVram(i, COPYWIN_FULL);
    }
}

static void Task_VictoryScreenWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_VictoryScreenMainInput;
}

static void Task_VictoryScreenMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_VictoryScreenWaitFadeAndExitGracefully;
    }
}

static void Task_VictoryScreenWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sVictoryScreenState->savedCallback);
        VictoryScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static void VictoryScreen_PrintText(void)
{
    const u8 *str;
    switch (sVictoryScreenState->gauntlet)
    {
    default:
    case GAUNTLET_CRISIS_TEAM:
        str = COMPOUND_STRING("Endfield Crisis Team");
        break;
    case GAUNTLET_WULING:
        str = COMPOUND_STRING("Wuling Science Station");
        break;
    case GAUNTLET_RECONVENERS:
        str = COMPOUND_STRING("Rhodes Island Reconveners");
        break;
    case GAUNTLET_ABYSSAL_HUNTERS:
        str = COMPOUND_STRING("Abyssal Hunters");
        break;
    case GAUNTLET_AK_GREEN:
        str = COMPOUND_STRING("Band of Misfits");
        break;
    case GAUNTLET_AK_YELLOW:
        str = COMPOUND_STRING("Confluence of East and West");
        break;
    }
    u32 width = GetStringWidth(FONT_NORMAL, str, 0);
    AddTextPrinterParameterized4(WIN_TITLE, FONT_NORMAL, 120 - width / 2, 0, 0, 0, sVictoryScreenWindowFontColors[1], 0, str);
    CopyWindowToVram(WIN_TITLE, COPYWIN_GFX);

    u32 totalRisk = GetTotalTiskValue();
    u8 numStr[4];
    ConvertIntToDecimalStringN(numStr, totalRisk, STR_CONV_MODE_LEFT_ALIGN, 3);
    width = GetStringWidth(FONT_NORMAL, numStr, 0);
    AddTextPrinterParameterized4(WIN_RISK, FONT_NORMAL, 16 - width / 2, 0, 0, 0, sVictoryScreenWindowFontColors[1], 0, numStr);
    CopyWindowToVram(WIN_TITLE, COPYWIN_GFX);

    u32 trainerId = gSaveBlock2Ptr->playerTrainerId[0] + (gSaveBlock2Ptr->playerTrainerId[1] << 8) + (gSaveBlock2Ptr->playerTrainerId[2] << 16) + (gSaveBlock2Ptr->playerTrainerId[3] << 24);
    u8 trainerStr[12];
    trainerStr[0] = CHAR_0;
    trainerStr[1] = CHAR_x;
    for (u32 i = 0; i < 8; i++)
    {
        u8 currChar = CHAR_0;
        switch ((trainerId >> (4 * i)) & 0xF)
        {
        case 0:
            currChar = CHAR_0;
            break;
        case 1:
            currChar = CHAR_1;
            break;
        case 2:
            currChar = CHAR_2;
            break;
        case 3:
            currChar = CHAR_3;
            break;
        case 4:
            currChar = CHAR_4;
            break;
        case 5:
            currChar = CHAR_5;
            break;
        case 6:
            currChar = CHAR_6;
            break;
        case 7:
            currChar = CHAR_7;
            break;
        case 8:
            currChar = CHAR_8;
            break;
        case 9:
            currChar = CHAR_9;
            break;
        case 10:
            currChar = CHAR_A;
            break;
        case 11:
            currChar = CHAR_B;
            break;
        case 12:
            currChar = CHAR_C;
            break;
        case 13:
            currChar = CHAR_D;
            break;
        case 14:
            currChar = CHAR_E;
            break;
        case 15:
            currChar = CHAR_F;
            break;
        }
        trainerStr[9 - i] = currChar;
    }
    trainerStr[10] = EOS;

    width = GetStringWidth(FONT_NORMAL, trainerStr, 0);
    AddTextPrinterParameterized4(WIN_TRAINER, FONT_NORMAL, 120 - width / 2, 0, 0, 0, sVictoryScreenWindowFontColors[1], 0, trainerStr);
    CopyWindowToVram(WIN_TRAINER, COPYWIN_GFX);
}

static void SpriteCB_Dummy(struct Sprite *sprite)
{
}

static const u16 sMarkingsPal[] = INCGFX_U16("graphics/interface/mon_markings.png", ".gbapal");
static const u32 sMarkingsGfx[] = INCGFX_U32("graphics/interface/mon_markings.png", ".4bpp");

static void VictoryScreen_ShowMons(void)
{
    LoadMonIconPalettes();
    for (u32 i = 0; i < 6; i++)
    {
        if (sVictoryScreenState->species[i] == SPECIES_NONE)
            break;
        sVictoryScreenState->monSpriteIds[i] = CreateMonIcon(sVictoryScreenState->species[i], SpriteCB_Dummy, 20 + 40 * i, 80, 0, 0);

        struct Even_CreateSpriteStruct cs = {0};
        cs.sprite = &sMarkingsGfx[8 + 32 * sVictoryScreenState->numDupes[i]];
        cs.tileTag = 6 + i;
        cs.palette = sMarkingsPal;
        cs.palTag = 6;
        cs.spriteSize = SPRITE_SIZE(16x8);
        cs.spriteShape = SPRITE_SHAPE(16x8);
        cs.posX = 20 + 40 * i;
        cs.posY = 100;
        sVictoryScreenState->potSpriteIds[i] = Even_CreateSprite(&cs);
    }
}

static void VictoryScreen_LoadRisks(void)
{
    u32 numRisks = 0;
    for (enum Risk risk = RISK_NONE + 2; risk < RISK_COUNT; risk++)
    {
        if (IsRiskActive(risk))
        {
            struct Even_CreateSpriteStruct cs = {0};
            cs.sprite = &sRiskGfx[(risk - 2) * 16 * 16 / 8];
            cs.tileTag = 12 + numRisks;
            cs.palette = sRiskPal;
            cs.palTag = 12;
            cs.spriteSize = SPRITE_SIZE(16x16);
            cs.spriteShape = SPRITE_SHAPE(16x16);
            cs.posX = 8 + (numRisks % 15) * 16;
            cs.posY = 120 + (numRisks / 15) * 16;
            sVictoryScreenState->riskSpriteIds[numRisks] = Even_CreateSprite(&cs);
            numRisks++;
        }
    }
}
