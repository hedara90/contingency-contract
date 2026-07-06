static const u32 sPartyMenuBg_Gfx_SwSh[]            = INCBIN_U32("graphics/party_menu/swsh/tiles.4bpp.smol");
static const u16 sPartyMenuBg_Pal_SwSh[]            = INCBIN_U16("graphics/party_menu/swsh/tiles.gbapal");
static const u32 sPartyMenuBg_Main_Tilemap_SwSh[]   = INCBIN_U32("graphics/party_menu/swsh/bg_main.bin.smolTM");
static const u32 sPartyMenuBg_Scroll_Tilemap_SwSh[] = INCBIN_U32("graphics/party_menu/swsh/bg_scroll.bin.smolTM");

enum {
    BUTTON_START,
    BUTTON_SELECT,
    BUTTON_L,
    BUTTON_R,
    BUTTON_NONE = 0xFF,
};

static const u8 sButtons_Gfx[][4 * TILE_SIZE_4BPP] = {
    [BUTTON_START]  = INCBIN_U8("graphics/party_menu/swsh/button_start.4bpp"),
    [BUTTON_SELECT] = INCBIN_U8("graphics/party_menu/swsh/button_select.4bpp"),
    [BUTTON_L]      = INCBIN_U8("graphics/party_menu/swsh/button_l.4bpp"),
    [BUTTON_R]      = INCBIN_U8("graphics/party_menu/swsh/button_r.4bpp"),
};

static const struct OamData sOamData_Button = {
    .size = SPRITE_SIZE(32x8),
    .shape = SPRITE_SHAPE(32x8),
    .priority = 0,
};

#if SWSH_PARTY_MENU == TRUE
static const u32 sStatusGfx_Icons_SwSh[] = INCBIN_U32("graphics/party_menu/swsh/status_icons.4bpp.smol");
// Palette loaded to keep with vanilla structure, but not actually used
static const u16 sStatusPal_Icons_SwSh[] = INCBIN_U16("graphics/party_menu/swsh/status_icons.gbapal");

static const u32 sHeldItemGfx[]          = INCBIN_U32("graphics/party_menu/swsh/hold_icons.4bpp");
const u16 gHeldItemPalette[]             = INCBIN_U16("graphics/party_menu/swsh/hold_icons.gbapal");
#else
static const u32 sHeldItemGfx[]          = INCBIN_U32("graphics/party_menu/hold_icons.4bpp");
const u16 gHeldItemPalette[]             = INCBIN_U16("graphics/party_menu/hold_icons.gbapal");
#endif

static const u32 sHoverCursorGfx[]        = INCBIN_U32("graphics/party_menu/swsh/hover_cursor.4bpp.smol");
static const u32 sSelectFrameGfx[]        = INCBIN_U32("graphics/party_menu/swsh/select_frame.4bpp.smol");
static const u32 sMessageWindowGfx[]      = INCBIN_U32("graphics/party_menu/swsh/message_window.4bpp.smol");
static const u32 sMultiuseWindowGfx[]     = INCBIN_U32("graphics/party_menu/swsh/multiuse_window.4bpp.smol");
static const u16 sMonShadowPalette[]      = INCBIN_U16("graphics/party_menu/swsh/shadow.gbapal");
static const u32 sMoveTypes_Gfx[]         = INCBIN_U32("graphics/party_menu/swsh/move_types.4bpp.smol");

static const u8 sText_EggNickname[POKEMON_NAME_LENGTH + 1]  = _("Egg");
static const u8 sMenuText_Confirm[]                         = _("Confirm");
static const u8 sMenuText_Switch[]                          = _("Switch");
static const u8 sMenuText_Boxes[]                           = _("Boxes");
static const u8 sMenuText_ViewPartnerParty[]                = _("{STR_VAR_1}'s party");
static const u8 sMenuText_ViewPlayerParty[]                 = _("Your party");
static const u8 sText_SendThisMonToPC[]                     = _("Send {STR_VAR_1} to the PC?");

static const struct BgTemplate sPartyMenuBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 0,
        .mapBaseIndex = 27,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    },
};

static const struct PartyMenuBoxInfoRects sPartyBoxInfoRects[] =
{
    [PARTY_BOX_SWSH_COLUMN] =
    {
        BlitBitmapToPartyWindow_SwSh,
        {
            //The below are the x, y, width, and height for each of the following info
            32,  0, 48, 13, // Nickname
            80, 11, 32,  8, // Level
            91,  0,  8,  8, // Gender
            32, 11, 24,  8, // HP
            47, 11, 24,  8, // Max HP
            32, 12, 64,  2  // HP bar
        },
        32, 11, 64, 12        // Description text
    }
};


// Each layout array has an array for each of the 6 party slots
// The array for each slot has the sprite coords of its various sprites in the following order
// Pokémon icon (x, y), held item (x, y), status condition (x, y), menu Poké Ball (x, y)
static const u8 sPartyMenuSpriteCoords[PARTY_LAYOUT_COUNT][PARTY_SIZE][4 * 2] =
{
    [PARTY_LAYOUT_SINGLE] =
    {
        { 34,  18,  40,  28, 108,  27,  16,  34},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 34,  90,  40, 100, 108,  99, 102,  73},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    [PARTY_LAYOUT_DOUBLE] =
    {
        { 26,  18,  32,  28, 100,  27,  16,  18},
        { 26,  42,  32,  52, 100,  51,  16,  74},
        { 34,  66,  40,  76, 108,  75, 102,  25},
        { 34,  90,  40, 100, 108,  99, 102,  57},
        { 34, 114,  40, 124, 108, 123, 102,  89},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    [PARTY_LAYOUT_MULTI] =
    {
        { 26,  18,  32,  28, 100,  27,  16,  18},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 26,  90,  32, 100, 100,  99,  16,  90},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    [PARTY_LAYOUT_MULTI_SHOWCASE] =
    {
        { 26,  18,  32,  28, 100,  27,  16,  18},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 26,  90,  32, 100, 100,  99,  16,  90},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    // MULTI_FULL* layouts use the same positions as SINGLE
    [PARTY_LAYOUT_MULTI_FULL] =
    {
        { 34,  18,  40,  28, 108,  27,  16,  34},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 34,  90,  40, 100, 108,  99, 102,  73},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    [PARTY_LAYOUT_MULTI_FULL_PARTNER] =
    {
        { 34,  18,  40,  28, 108,  27,  16,  34},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 34,  90,  40, 100, 108,  99, 102,  73},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    [PARTY_LAYOUT_MULTI_FULL_SHOWCASE] =
    {
        { 34,  18,  40,  28, 108,  27,  16,  34},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 34,  90,  40, 100, 108,  99, 102,  73},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
    [PARTY_LAYOUT_MULTI_FULL_SHOWCASE_PARTNER] =
    {
        { 34,  18,  40,  28, 108,  27,  16,  34},
        { 34,  42,  40,  52, 108,  51, 102,  25},
        { 34,  66,  40,  76, 108,  75, 102,  49},
        { 34,  90,  40, 100, 108,  99, 102,  73},
        { 34, 114,  40, 124, 108, 123, 102,  97},
        { 34, 138,  40, 148, 108, 147, 102, 121},
    },
};

static const struct PartyMenuMoveBoxInfoRects sPartyMoveBoxInfoRects[] =
{
    { BlitBitmapToPartyMoveWindow_SwSh,
        {
            10,  1, 62, 12, // Move name
            86,  1, 10, 12, // PP
        }
    },
};

// Used only when both Cancel and Confirm are present
static const u32 sConfirmButton_Tilemap[] = INCBIN_U32("graphics/party_menu/confirm_button.bin");
static const u32 sCancelButton_Tilemap[] = INCBIN_U32("graphics/party_menu/cancel_button.bin");

// Text colors for BG, FG, and Shadow in that order
static const u8 sFontColorTable[][3] =
{
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_GRAY, TEXT_COLOR_DARK_GRAY},  // Default
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_GREEN},      // Unused
    {TEXT_COLOR_TRANSPARENT, TEXT_DYNAMIC_COLOR_2,  TEXT_DYNAMIC_COLOR_3},  // Gender symbol
    {TEXT_COLOR_WHITE,       TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY}, // Selection actions
    {TEXT_COLOR_WHITE,       TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_BLUE}, // Field moves
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},  // Unused
    {TEXT_COLOR_WHITE,       TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_RED},  // Move relearner
    {0,  5,  6},  // PP state 0 (yellow)
    {0,  7,  8},  // PP state 1 (orange)
    {0,  9, 10},  // PP state 2 (red)
    {0,  3,  4},  // PP state 3 (white)
    {0,  1,  2},  // Ability name, move names
    {0,  3,  6},  // Item multiuse
};

static const struct WindowTemplate sSinglePartyMenuWindowTemplate[] =
{
    { // Party mon 1
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 3,
        .width = 10,
        .height = 7,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 1,
        .width = 18,
        .height = 3,
        .paletteNum = 4,
        .baseBlock = 0xB5,
    },
    { // Party mon 3
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 4,
        .width = 18,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xEB,
    },
    { // Party mon 4
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 7,
        .width = 18,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0x121,
    },
    { // Party mon 5
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 10,
        .width = 18,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x157,
    },
    { // Party mon 6
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 13,
        .width = 18,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x18D,
    },
    [WIN_MSG] = {
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x1EB,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sDoublePartyMenuWindowTemplate[] =
{
    { // Party mon 1
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 10,
        .height = 7,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 8,
        .width = 10,
        .height = 7,
        .paletteNum = 4,
        .baseBlock = 0xB5,
    },
    { // Party mon 3
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 1,
        .width = 18,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xFB,
    },
    { // Party mon 4
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 5,
        .width = 18,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0x125,
    },
    { // Party mon 5
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 9,
        .width = 18,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x167,
    },
    { // Party mon 6
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 13,
        .width = 18,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x19D,
    },
    [WIN_MSG] = {
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x1EB,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sMultiPartyMenuWindowTemplate[] =
{
    { // Party mon 1
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 10,
        .height = 7,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 8,
        .width = 10,
        .height = 7,
        .paletteNum = 4,
        .baseBlock = 0xB5,
    },
    { // Party mon 3
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 2,
        .width = 18,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xFB,
    },
    { // Party mon 4
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 5,
        .width = 18,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0x131,
    },
    { // Party mon 5
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 9,
        .width = 18,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x167,
    },
    { // Party mon 6
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 12,
        .width = 18,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x19D,
    },
    [WIN_MSG] = {
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x1EB,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sShowcaseMultiPartyMenuWindowTemplate[] =
{
    { // Party mon 1
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 2,
        .width = 10,
        .height = 7,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 3,
        .width = 18,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xB5,
    },
    { // Party mon 3
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 6,
        .width = 18,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0xEB,
    },
    { // Party mon 4
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 11,
        .width = 10,
        .height = 7,
        .paletteNum = 4,
        .baseBlock = 0x121,
    },
    { // Party mon 5
        .bg = 2,
        .tilemapLeft = 12,
        .tilemapTop = 12,
        .width = 18,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x177,
    },
    { // Party mon 6
        .bg = 2,
        .tilemapLeft = 12,
        .tilemapTop = 15,
        .width = 18,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x1AD,
    },
    DUMMY_WIN_TEMPLATE
};

#define PARTY_LABEL_WINDOW_PROMPT 7

static const struct WindowTemplate sSinglePartyMenuWindowTemplate_SwSh[] =
{
    { // Party mon 1
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 14,
        .height = 3,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 4,
        .width = 14,
        .height = 3,
        .paletteNum = 4,
        .baseBlock = 0x99,
    },
    { // Party mon 3
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 14,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xC3,
    },
    { // Party mon 4
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 10,
        .width = 14,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0xED,
    },
    { // Party mon 5
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 13,
        .width = 14,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x117,
    },
    { // Party mon 6
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 16,
        .width = 14,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x141,
    },
    [WIN_MSG] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x16B,
    },
    [PARTY_LABEL_WINDOW_PROMPT] = {
        .bg = 1,
        .tilemapLeft = 16,
        .tilemapTop = 18,
        .width = 14,
        .height = 2,
        .paletteNum = 0,
        .baseBlock = 0x1F0,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sDoublePartyMenuWindowTemplate_SwSh[] =
{
    { // Party mon 1
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 14,
        .height = 3,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 4,
        .width = 14,
        .height = 3,
        .paletteNum = 4,
        .baseBlock = 0x99,
    },
    { // Party mon 3
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 14,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xC3,
    },
    { // Party mon 4
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 10,
        .width = 14,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0xED,
    },
    { // Party mon 5
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 13,
        .width = 14,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x117,
    },
    { // Party mon 6
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 16,
        .width = 14,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x141,
    },
    [WIN_MSG] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x16B,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sMultiPartyMenuWindowTemplate_SwSh[] =
{
    { // Party mon 1 (player lead)
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 14,
        .height = 3,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 4,
        .width = 14,
        .height = 3,
        .paletteNum = 4,
        .baseBlock = 0x99,
    },
    { // Party mon 3
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 14,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xC3,
    },
    { // Party mon 4 (partner lead)
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 10,
        .width = 14,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0xED,
    },
    { // Party mon 5
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 13,
        .width = 14,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x117,
    },
    { // Party mon 6
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 16,
        .width = 14,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x141,
    },
    [WIN_MSG] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x16B,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sShowcaseMultiPartyMenuWindowTemplate_SwSh[] =
{
    { // Party mon 1 (player lead)
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 14,
        .height = 3,
        .paletteNum = 3,
        .baseBlock = 0x6F,
    },
    { // Party mon 2
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 4,
        .width = 14,
        .height = 3,
        .paletteNum = 4,
        .baseBlock = 0x99,
    },
    { // Party mon 3
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 14,
        .height = 3,
        .paletteNum = 5,
        .baseBlock = 0xC3,
    },
    { // Party mon 4 (partner lead)
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 10,
        .width = 14,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 0xED,
    },
    { // Party mon 5
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 13,
        .width = 14,
        .height = 3,
        .paletteNum = 7,
        .baseBlock = 0x117,
    },
    { // Party mon 6
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 16,
        .width = 14,
        .height = 3,
        .paletteNum = 8,
        .baseBlock = 0x141,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sDefaultPartyMsgWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 17,
    .width = 21,
    .height = 2,
    .paletteNum = 15,
    .baseBlock = 0x1FD,
};

static const struct WindowTemplate sWindowTemplate_FirstBattleOakVoiceover =
{
    .bg = 0,
    .tilemapLeft = 2,
    .tilemapTop = 15,
    .width = 27,
    .height = 4,
    .paletteNum = 14,
    .baseBlock = 0x1DF,
};

// static const struct WindowTemplate sDoWhatWithMonMsgWindowTemplate =
// {
//     .bg = 2,
//     .tilemapLeft = 1,
//     .tilemapTop = 17,
//     .width = 16,
//     .height = 2,
//     .paletteNum = 15,
//     .baseBlock = 0x285,
// };

// static const struct WindowTemplate sDoWhatWithItemMsgWindowTemplate =
// {
//     .bg = 2,
//     .tilemapLeft = 1,
//     .tilemapTop = 17,
//     .width = 20,
//     .height = 2,
//     .paletteNum = 15,
//     .baseBlock = 0x299,
// };

// static const struct WindowTemplate sDoWhatWithMailMsgWindowTemplate =
// {
//     .bg = 2,
//     .tilemapLeft = 1,
//     .tilemapTop = 17,
//     .width = 18,
//     .height = 2,
//     .paletteNum = 15,
//     .baseBlock = 0x299,
// };

// static const struct WindowTemplate sWhichMoveMsgWindowTemplate =
// {
//     .bg = 2,
//     .tilemapLeft = 1,
//     .tilemapTop = 17,
//     .width = 16,
//     .height = 2,
//     .paletteNum = 15,
//     .baseBlock = 0x299,
// };

static const struct WindowTemplate sAlreadyHoldingOneMsgWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 15,
    .width = 20,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x22D,
};

static const struct WindowTemplate sOrderWhichApplianceMsgWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 15,
    .width = 14,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x22D,
};

static const struct WindowTemplate sItemGiveTakeWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 23,
    .tilemapTop = 11,
    .width = 6,
    .height = 8,
    .paletteNum = 14,
    .baseBlock = 0x39D,
};

static const struct WindowTemplate sMailReadTakeWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 21,
    .tilemapTop = 13,
    .width = 8,
    .height = 6,
    .paletteNum = 14,
    .baseBlock = 0x39D,
};

static const struct WindowTemplate sMoveSelectWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 19,
    .tilemapTop = 11,
    .width = 10,
    .height = 8,
    .paletteNum = 14,
    .baseBlock = 0x283,
};

static const struct WindowTemplate sCatalogSelectWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 17,
    .tilemapTop = 5,
    .width = 12,
    .height = 14,
    .paletteNum = 14,
    .baseBlock = 0x283,
};

static const struct WindowTemplate sZygardeCubeSelectWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 18,
    .tilemapTop = 13,
    .width = 11,
    .height = 6,
    .paletteNum = 14,
    .baseBlock = 0x283,
};

static const struct WindowTemplate sPartyMenuYesNoWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 21,
    .tilemapTop = 9,
    .width = 5,
    .height = 4,
    .paletteNum = 14,
    .baseBlock = 0x283,
};

static const struct WindowTemplate sLevelUpStatsWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 19,
    .tilemapTop = 1,
    .width = 10,
    .height = 11,
    .paletteNum = 14,
    .baseBlock = 0x283,
};

static const struct WindowTemplate sGiveHowManyItemsWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 20,
    .tilemapTop = 11,
    .width = 4,
    .height = 2,
    .paletteNum = 1,
    .baseBlock = 0x283,
};

static const struct WindowTemplate sMoveInfoWindowTemplate_SwSh[] =
{
    { // Move slot 1
        .bg = 1,
        .tilemapLeft = 16,
        .tilemapTop = 2,
        .width = 14,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 0x331,
    },
    { // Move slot 2
        .bg = 1,
        .tilemapLeft = 16,
        .tilemapTop = 4,
        .width = 14,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 0x34D,
    },
    { // Move slot 3
        .bg = 1,
        .tilemapLeft = 16,
        .tilemapTop = 6,
        .width = 14,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 0x369,
    },
    { // Move slot 4
        .bg = 1,
        .tilemapLeft = 16,
        .tilemapTop = 8,
        .width = 14,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 0x385,
    },
};

static const struct WindowTemplate sAbilityInfoWindowTemplate =
{
    .bg = 1,
    .tilemapLeft = 17,
    .tilemapTop = 11,
    .width = 13,
    .height = 4,
    .paletteNum = 1,
    .baseBlock = 0x3A7,
};

static const struct WindowTemplate sUnusedWindowTemplate1 =
{
    .bg = 0,
    .tilemapLeft = 2,
    .tilemapTop = 15,
    .width = 27,
    .height = 4,
    .paletteNum = 14,
    .baseBlock = 0x1DF,
};

static const struct WindowTemplate sUnusedWindowTemplate2 =
{
    .bg = 0,
    .tilemapLeft = 0,
    .tilemapTop = 13,
    .width = 18,
    .height = 3,
    .paletteNum = 12,
    .baseBlock = 0x39D,
};

// Plain tilemaps for party menu slots.
// The versions with no HP bar are used by eggs, and in certain displays like registering at a battle facility.
// There is no empty version of the main slot because it shouldn't ever be empty.
static const u8 sSlotTilemap_Main[]      = INCBIN_U8("graphics/party_menu/slot_main.bin");
static const u8 sSlotTilemap_MainNoHP[]  = INCBIN_U8("graphics/party_menu/slot_main_no_hp.bin");
static const u8 sSlotTilemap_Wide[]      = INCBIN_U8("graphics/party_menu/slot_wide.bin");
static const u8 sSlotTilemap_WideNoHP[]  = INCBIN_U8("graphics/party_menu/slot_wide_no_hp.bin");
static const u8 sSlotTilemap_WideEmpty[] = INCBIN_U8("graphics/party_menu/slot_wide_empty.bin");
static const u8 sSlotTilemap_Main_SwSh[]  = INCBIN_U8("graphics/party_menu/swsh/slot.bin");
static const u8 sSlotTilemap_Empty_SwSh[] = INCBIN_U8("graphics/party_menu/swsh/slot_empty.bin");

// Plain tilemaps for move slots
static const u8 sMoveTilemap_Main_SwSh[]  = INCBIN_U8("graphics/party_menu/swsh/move.bin");
static const u8 sMoveTilemap_Empty_SwSh[] = INCBIN_U8("graphics/party_menu/swsh/move_empty.bin");
static const u8 sAbilityTilemap_SwSh[]    = INCBIN_U8("graphics/party_menu/swsh/ability_box.bin");

// Palette offsets
static const u8 sGenderPalOffsets[]     = {11, 12};
static const u8 sHPBarPalOffset         = 9;
static const u8 sPartyBoxPalOffset1     = 1;
static const u8 sPartyBoxPalOffsets3[]  = {2, 3};

// Palette ids
static const u8 sGenderMalePalIds[]               = {11, 12};
static const u8 sGenderFemalePalIds[]             = {13, 14};
static const u8 sHPBarPalIds[]                    = {7, 8, 9};
static const u8 sHPBarEmptyPalId                  = 10;
static const u8 sPartyBoxNoMonPalId               = 17;
static const u8 sPartyBoxEmptySlotPalId1          = 33;
static const u8 sPartyBoxMultiPalId1              = 49;
static const u8 sPartyBoxSelectedForActionPalId1  = 65;
static const u8 sPartyBoxCurrSelectionPalId1      = 81;
static const u8 sPartyBoxCurrSelectionMultiPalId  = 97;

// Text palettes
static const u8 sPartyBoxEmptySlotPalIds3[]          = {34, 35};
static const u8 sPartyBoxMultiPalIds3[]              = {50, 51};
static const u8 sPartyBoxSelectedForActionPalIds3[]  = {66, 67};
static const u8 sPartyBoxCurrSelectionPalIds3[]      = {82, 83};
static const u8 sPartyBoxCurrSelectionMultiPalIds3[] = {98, 99};

static const u8 *const sActionStringTable[] =
{
    [PARTY_MSG_CHOOSE_MON]             = gText_ChoosePokemon,
    [PARTY_MSG_CHOOSE_MON_OR_CANCEL]   = gText_ChoosePokemonCancel,
    [PARTY_MSG_CHOOSE_MON_AND_CONFIRM] = gText_ChoosePokemonConfirm,
    [PARTY_MSG_MOVE_TO_WHERE]          = gText_MoveToWhere,
    [PARTY_MSG_TEACH_WHICH_MON]        = gText_TeachWhichPokemon,
    [PARTY_MSG_USE_ON_WHICH_MON]       = gText_UseOnWhichPokemon,
    [PARTY_MSG_GIVE_TO_WHICH_MON]      = gText_GiveToWhichPokemon,
    [PARTY_MSG_NOTHING_TO_CUT]         = gText_NothingToCut,
    [PARTY_MSG_CANT_SURF_HERE]         = gText_CantSurfHere,
    [PARTY_MSG_ALREADY_SURFING]        = gText_AlreadySurfing,
    [PARTY_MSG_CURRENT_TOO_FAST]       = gText_CurrentIsTooFast,
    [PARTY_MSG_ENJOY_CYCLING]          = gText_EnjoyCycling,
    [PARTY_MSG_ALREADY_IN_USE]         = gText_InUseAlready_PM,
    [PARTY_MSG_CANT_USE_HERE]          = gText_CantUseHere,
    [PARTY_MSG_NO_MON_FOR_BATTLE]      = gText_NoPokemonForBattle,
    [PARTY_MSG_CHOOSE_MON_2]           = gText_ChoosePokemon2,
    [PARTY_MSG_NOT_ENOUGH_HP]          = gText_NotEnoughHp,
    [PARTY_MSG_X_MONS_ARE_NEEDED]      = gText_PokemonAreNeeded,
    [PARTY_MSG_MONS_CANT_BE_SAME]      = gText_PokemonCantBeSame, //TODO: regular dialog
    [PARTY_MSG_NO_SAME_HOLD_ITEMS]     = gText_NoIdenticalHoldItems, //TODO: regular dialog
    [PARTY_MSG_UNUSED]                 = gText_EmptyString2,
    [PARTY_MSG_DO_WHAT_WITH_MON]       = gText_DoWhatWithPokemon,
    [PARTY_MSG_RESTORE_WHICH_MOVE]     = gText_RestoreWhichMove, //TODO: set cursor on move slots
    [PARTY_MSG_BOOST_PP_WHICH_MOVE]    = gText_BoostPp,
    [PARTY_MSG_DO_WHAT_WITH_ITEM]      = gText_DoWhatWithItem,
    [PARTY_MSG_DO_WHAT_WITH_MAIL]      = gText_DoWhatWithMail,
    [PARTY_MSG_ALREADY_HOLDING_ONE]    = gText_AlreadyHoldingOne,
    [PARTY_MSG_WHICH_APPLIANCE]        = gText_WhichAppliance,
    [PARTY_MSG_CHOOSE_SECOND_FUSION]   = gText_NextFusionMon,
    [PARTY_MSG_NO_POKEMON]             = COMPOUND_STRING("You have no POKéMON."),
    [PARTY_MSG_CHOOSE_MON_FOR_BOX]     = gText_SendWhichMonToPC,
    [PARTY_MSG_SEND_MON_TO_BOX]        = sText_SendThisMonToPC,
    [PARTY_MSG_MOVE_ITEM_WHERE]        = gText_MoveItemWhere,
};

static const u8 sText_NoUse[] = _("NO USE");

static const u8 *const sDescriptionStringTable[] =
{
    [PARTYBOX_DESC_NO_USE]     = sText_NoUse,
    [PARTYBOX_DESC_ABLE_3]     = gText_Able,
    [PARTYBOX_DESC_FIRST]      = gText_First_PM,
    [PARTYBOX_DESC_SECOND]     = gText_Second_PM,
    [PARTYBOX_DESC_THIRD]      = gText_Third_PM,
    [PARTYBOX_DESC_FOURTH]     = gText_Fourth,
    [PARTYBOX_DESC_ABLE]       = gText_Able2,
    [PARTYBOX_DESC_NOT_ABLE]   = gText_NotAble,
    [PARTYBOX_DESC_ABLE_2]     = gText_Able3,
    [PARTYBOX_DESC_NOT_ABLE_2] = gText_NotAble2,
    [PARTYBOX_DESC_LEARNED]    = gText_Learned,
    [PARTYBOX_DESC_HAVE]       = gText_Have,
    [PARTYBOX_DESC_DONT_HAVE]  = gText_DontHave,
};

static const u16 sUnusedData[] =
{
    0x0108, 0x0151, 0x0160, 0x015b, 0x002e, 0x005c, 0x0102, 0x0153, 0x014b, 0x00ed, 0x00f1, 0x010d, 0x003a, 0x003b, 0x003f, 0x0071,
    0x00b6, 0x00f0, 0x00ca, 0x00db, 0x00da, 0x004c, 0x00e7, 0x0055, 0x0057, 0x0059, 0x00d8, 0x005b, 0x005e, 0x00f7, 0x0118, 0x0068,
    0x0073, 0x015f, 0x0035, 0x00bc, 0x00c9, 0x007e, 0x013d, 0x014c, 0x0103, 0x0107, 0x0122, 0x009c, 0x00d5, 0x00a8, 0x00d3, 0x011d,
    0x0121, 0x013b, 0x000f, 0x0013, 0x0039, 0x0046, 0x0094, 0x00f9, 0x007f, 0x0123,
};

static const u8 sText_Trade4[] = _("TRADE");

struct
{
    const u8 *text;
    TaskFunc func;
} static const sCursorOptions[MENU_FIELD_MOVES] =
{
    [MENU_SUMMARY]         = {COMPOUND_STRING("Summary"),         CursorCb_Summary},
    [MENU_SWITCH]          = {COMPOUND_STRING("Switch"),          CursorCb_Switch},
    [MENU_CANCEL1]         = {COMPOUND_STRING("Cancel"),          CursorCb_Cancel1},
    [MENU_ITEM]            = {COMPOUND_STRING("Item"),            CursorCb_Item},
    [MENU_GIVE]            = {COMPOUND_STRING("Give"),            CursorCb_Give},
    [MENU_TAKE_ITEM]       = {COMPOUND_STRING("Take"),            CursorCb_TakeItem},
    [MENU_MOVE_ITEM]       = {COMPOUND_STRING("Move"),            CursorCb_MoveItem},
    [MENU_MAIL]            = {COMPOUND_STRING("Mail"),            CursorCb_Mail},
    [MENU_TAKE_MAIL]       = {COMPOUND_STRING("Take"),            CursorCb_TakeMail},
    [MENU_READ]            = {COMPOUND_STRING("Read"),            CursorCb_Read},
    [MENU_CANCEL2]         = {COMPOUND_STRING("Cancel"),          CursorCb_Cancel2},
    [MENU_SHIFT]           = {COMPOUND_STRING("Shift"),           CursorCb_SendMon},
    [MENU_SEND_OUT]        = {COMPOUND_STRING("Send Out"),        CursorCb_SendMon},
    [MENU_ENTER]           = {COMPOUND_STRING("Enter"),           CursorCb_Enter},
    [MENU_NO_ENTRY]        = {COMPOUND_STRING("No Entry"),        CursorCb_NoEntry},
    [MENU_STORE]           = {COMPOUND_STRING("Store"),           CursorCb_Store},
    [MENU_REGISTER]        = {COMPOUND_STRING("Register"),        CursorCb_Register},
    [MENU_TRADE1]          = {COMPOUND_STRING("Trade"),           CursorCb_Trade1},
    [MENU_TRADE2]          = {COMPOUND_STRING("Trade"),           CursorCb_Trade2},
    [MENU_TOSS]            = {COMPOUND_STRING("Toss"),            CursorCb_Toss},
    [MENU_CATALOG_BULB]    = {COMPOUND_STRING("Light bulb"),      CursorCb_CatalogBulb},
    [MENU_CATALOG_OVEN]    = {COMPOUND_STRING("Microwave oven"),  CursorCb_CatalogOven},
    [MENU_CATALOG_WASHING] = {COMPOUND_STRING("Washing machine"), CursorCb_CatalogWashing},
    [MENU_CATALOG_FRIDGE]  = {COMPOUND_STRING("Refrigerator"),    CursorCb_CatalogFridge},
    [MENU_CATALOG_FAN]     = {COMPOUND_STRING("Electric fan"),    CursorCb_CatalogFan},
    [MENU_CATALOG_MOWER]   = {COMPOUND_STRING("Lawn mower"),      CursorCb_CatalogMower},
    [MENU_CHANGE_FORM]     = {COMPOUND_STRING("Change form"),     CursorCb_ChangeForm},
    [MENU_CHANGE_ABILITY]  = {COMPOUND_STRING("Change Ability"),  CursorCb_ChangeAbility},
};

static const u8 sPartyMenuAction_SummarySwitchCancel[] = {MENU_SUMMARY, MENU_SWITCH, MENU_CANCEL1};
static const u8 sPartyMenuAction_ShiftSummaryCancel[] = {MENU_SHIFT, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_SendOutSummaryCancel[] = {MENU_SEND_OUT, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_SummaryCancel[] = {MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_EnterSummaryCancel[] = {MENU_ENTER, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_NoEntrySummaryCancel[] = {MENU_NO_ENTRY, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_StoreSummaryCancel[] = {MENU_STORE, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_GiveTakeItemCancel[] = {MENU_GIVE, MENU_TAKE_ITEM, MENU_MOVE_ITEM, MENU_CANCEL2};
static const u8 sPartyMenuAction_ReadTakeMailCancel[] = {MENU_READ, MENU_TAKE_MAIL, MENU_CANCEL2};
static const u8 sPartyMenuAction_RegisterSummaryCancel[] = {MENU_REGISTER, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_TradeSummaryCancel1[] = {MENU_TRADE1, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_TradeSummaryCancel2[] = {MENU_TRADE2, MENU_SUMMARY, MENU_CANCEL1};
static const u8 sPartyMenuAction_TakeItemTossCancel[] = {MENU_TAKE_ITEM, MENU_TOSS, MENU_CANCEL1};
static const u8 sPartyMenuAction_RotomCatalog[] = {MENU_CATALOG_BULB, MENU_CATALOG_OVEN, MENU_CATALOG_WASHING, MENU_CATALOG_FRIDGE, MENU_CATALOG_FAN, MENU_CATALOG_MOWER, MENU_CANCEL1};
static const u8 sPartyMenuAction_ZygardeCube[] = {MENU_CHANGE_FORM, MENU_CHANGE_ABILITY, MENU_CANCEL1};



static const u8 *const sPartyMenuActions[] =
{
    [ACTIONS_NONE]          = NULL,
    [ACTIONS_SWITCH]        = sPartyMenuAction_SummarySwitchCancel,
    [ACTIONS_SHIFT]         = sPartyMenuAction_ShiftSummaryCancel,
    [ACTIONS_SEND_OUT]      = sPartyMenuAction_SendOutSummaryCancel,
    [ACTIONS_ENTER]         = sPartyMenuAction_EnterSummaryCancel,
    [ACTIONS_NO_ENTRY]      = sPartyMenuAction_NoEntrySummaryCancel,
    [ACTIONS_STORE]         = sPartyMenuAction_StoreSummaryCancel,
    [ACTIONS_SUMMARY_ONLY]  = sPartyMenuAction_SummaryCancel,
    [ACTIONS_ITEM]          = sPartyMenuAction_GiveTakeItemCancel,
    [ACTIONS_MAIL]          = sPartyMenuAction_ReadTakeMailCancel,
    [ACTIONS_REGISTER]      = sPartyMenuAction_RegisterSummaryCancel,
    [ACTIONS_TRADE]         = sPartyMenuAction_TradeSummaryCancel1,
    [ACTIONS_SPIN_TRADE]    = sPartyMenuAction_TradeSummaryCancel2,
    [ACTIONS_TAKEITEM_TOSS] = sPartyMenuAction_TakeItemTossCancel,
    [ACTIONS_ROTOM_CATALOG] = sPartyMenuAction_RotomCatalog,
    [ACTIONS_ZYGARDE_CUBE]  = sPartyMenuAction_ZygardeCube,
};

static const u8 sPartyMenuActionCounts[] =
{
    [ACTIONS_NONE]          = 0,
    [ACTIONS_SWITCH]        = ARRAY_COUNT(sPartyMenuAction_SummarySwitchCancel),
    [ACTIONS_SHIFT]         = ARRAY_COUNT(sPartyMenuAction_ShiftSummaryCancel),
    [ACTIONS_SEND_OUT]      = ARRAY_COUNT(sPartyMenuAction_SendOutSummaryCancel),
    [ACTIONS_ENTER]         = ARRAY_COUNT(sPartyMenuAction_EnterSummaryCancel),
    [ACTIONS_NO_ENTRY]      = ARRAY_COUNT(sPartyMenuAction_NoEntrySummaryCancel),
    [ACTIONS_STORE]         = ARRAY_COUNT(sPartyMenuAction_StoreSummaryCancel),
    [ACTIONS_SUMMARY_ONLY]  = ARRAY_COUNT(sPartyMenuAction_SummaryCancel),
    [ACTIONS_ITEM]          = ARRAY_COUNT(sPartyMenuAction_GiveTakeItemCancel),
    [ACTIONS_MAIL]          = ARRAY_COUNT(sPartyMenuAction_ReadTakeMailCancel),
    [ACTIONS_REGISTER]      = ARRAY_COUNT(sPartyMenuAction_RegisterSummaryCancel),
    [ACTIONS_TRADE]         = ARRAY_COUNT(sPartyMenuAction_TradeSummaryCancel1),
    [ACTIONS_SPIN_TRADE]    = ARRAY_COUNT(sPartyMenuAction_TradeSummaryCancel2),
    [ACTIONS_TAKEITEM_TOSS] = ARRAY_COUNT(sPartyMenuAction_TakeItemTossCancel),
    [ACTIONS_ROTOM_CATALOG] = ARRAY_COUNT(sPartyMenuAction_RotomCatalog),
    [ACTIONS_ZYGARDE_CUBE]  = ARRAY_COUNT(sPartyMenuAction_ZygardeCube),
};

static const u8 *const sUnionRoomTradeMessages[] =
{
    [UR_TRADE_MSG_NOT_MON_PARTNER_WANTS - 1]       = gText_NotPkmnOtherTrainerWants,
    [UR_TRADE_MSG_NOT_EGG - 1]                     = gText_ThatIsntAnEgg,
    [UR_TRADE_MSG_MON_CANT_BE_TRADED_NOW - 1]      = gText_PkmnCantBeTradedNow,
    [UR_TRADE_MSG_MON_CANT_BE_TRADED - 1]          = gText_PkmnCantBeTraded,
    [UR_TRADE_MSG_PARTNERS_MON_CANT_BE_TRADED - 1] = gText_OtherTrainersPkmnCantBeTraded,
    [UR_TRADE_MSG_EGG_CANT_BE_TRADED -1]           = gText_EggCantBeTradedNow,
    [UR_TRADE_MSG_PARTNER_CANT_ACCEPT_MON - 1]     = gText_OtherTrainerCantAcceptPkmn,
    [UR_TRADE_MSG_CANT_TRADE_WITH_PARTNER_1 - 1]   = gText_CantTradeWithTrainer,
    [UR_TRADE_MSG_CANT_TRADE_WITH_PARTNER_2 - 1]   = gText_CantTradeWithTrainer,
};

static const struct OamData sOamData_HeldItem =
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

static const union AnimCmd sSpriteAnim_HeldItem[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_HeldMail[] =
{
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_HeldItem[] =
{
    sSpriteAnim_HeldItem,
    sSpriteAnim_HeldMail,
};

const struct SpriteSheet gSpriteSheet_HeldItem =
{
    .data = sHeldItemGfx, .size = sizeof(sHeldItemGfx), .tag = TAG_HELD_ITEM
};

static const struct SpritePalette sSpritePalette_HeldItem =
{
    .data = gHeldItemPalette, .tag = TAG_HELD_ITEM
};

static const struct SpriteTemplate sSpriteTemplate_HeldItem =
{
    .tileTag = TAG_HELD_ITEM,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_HeldItem,
    .anims = sSpriteAnimTable_HeldItem,
};

static const struct OamData sOamData_HoverCursor =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_HoverCursor =
{
    .data = sHoverCursorGfx,
    .size = (16 * 16 * 3) / 2,
    .tag = TAG_HOVER_CURSOR
};

static const union AnimCmd sAnim_HoverCursor[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_HoverCursor[] =
{
    sAnim_HoverCursor,
};

static const struct SpriteTemplate sSpriteTemplate_HoverCursor =
{
    .tileTag = TAG_HOVER_CURSOR,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_HoverCursor,
    .anims = sAnims_HoverCursor,
};

static const struct OamData sOamData_SelectFrame =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(16x32),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(16x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_SelectFrameLeft[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SelectFrameRight[] = {
    ANIMCMD_FRAME(0, 0, TRUE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SelectFrameMiddle[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_SelectFrame[] = {
    sSpriteAnim_SelectFrameLeft,
    sSpriteAnim_SelectFrameRight,
    sSpriteAnim_SelectFrameMiddle,
};

static const struct CompressedSpriteSheet sSpriteSheet_SelectFrame =
{
    .data = sSelectFrameGfx,
    .size = (16 * 32 * 2) / 2,
    .tag = TAG_SELECT_FRAME,
};

static const struct SpritePalette sSpritePal_SelectFrame =
{
    .data = gHeldItemPalette,
    .tag = TAG_HELD_ITEM,
};

static const struct SpriteTemplate sSpriteTemplate_SelectFrame =
{
    .tileTag = TAG_SELECT_FRAME ,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_SelectFrame,
    .anims = sSpriteAnimTable_SelectFrame,
};

static const struct OamData sOamData_MessageWindow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(32x32),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(32x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_MessageWindow_TopLeft[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_TopMiddle[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_TopRight[] = {
    ANIMCMD_FRAME(8, 0, TRUE, TRUE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_BottomLeft[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_BottomMiddle[] = {
    ANIMCMD_FRAME(24, 0, FALSE, TRUE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MessageWindow_BottomRight[] = {
    ANIMCMD_FRAME(0, 0, TRUE, TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MessageWindow[] = {
    sSpriteAnim_MessageWindow_TopLeft,
    sSpriteAnim_MessageWindow_TopMiddle,
    sSpriteAnim_MessageWindow_TopRight,
    sSpriteAnim_MessageWindow_BottomLeft,
    sSpriteAnim_MessageWindow_BottomMiddle,
    sSpriteAnim_MessageWindow_BottomRight,
};

static const struct CompressedSpriteSheet sSpriteSheet_MessageWindow =
{
    .data = sMessageWindowGfx,
    .size = (32 * 16 * 5) / 2,
    .tag = TAG_MESSAGE_WINDOW,
};

static const struct SpritePalette sSpritePal_MessageWindow =
{
    .data = gHeldItemPalette,
    .tag = TAG_HELD_ITEM,
};

static const struct SpriteTemplate sSpriteTemplate_MessageWindow =
{
    .tileTag = TAG_MESSAGE_WINDOW,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_MessageWindow,
    .anims = sSpriteAnimTable_MessageWindow,
};

static const struct OamData sOamData_MultiuseWindow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .size = SPRITE_SIZE(32x16),
    .x = 0,
    .matrixNum = 0,
    .shape = SPRITE_SHAPE(32x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_MultiuseWindow_TopLeft[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MultiuseWindow_TopRight[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MultiuseWindow_BottomLeft[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MultiuseWindow_BottomRight[] = {
    ANIMCMD_FRAME(8, 0, TRUE, TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MultiuseWindow[] = {
    sSpriteAnim_MultiuseWindow_TopLeft,
    sSpriteAnim_MultiuseWindow_TopRight,
    sSpriteAnim_MultiuseWindow_BottomLeft,
    sSpriteAnim_MultiuseWindow_BottomRight,
};

static const struct CompressedSpriteSheet sSpriteSheet_MultiuseWindow =
{
    .data = sMultiuseWindowGfx,
    .size = (32 * 16 * 3) / 2,
    .tag = TAG_MULTIUSE_WINDOW,
};

static const struct SpritePalette sSpritePal_MultiuseWindow =
{
    .data = gHeldItemPalette,
    .tag = TAG_HELD_ITEM,
};

static const struct SpriteTemplate sSpriteTemplate_MultiuseWindow =
{
    .tileTag = TAG_MULTIUSE_WINDOW,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_MultiuseWindow,
    .anims = sSpriteAnimTable_MultiuseWindow,
};

static const struct SpritePalette sSpritePal_PartyMonShadow =
{
    .data = sMonShadowPalette,
    .tag = TAG_MON_SHADOW
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
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_StatusPoison[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusParalyzed[] =
{
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusSleep[] =
{
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusFrozen[] =
{
    ANIMCMD_FRAME(12, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusBurn[] =
{
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusPokerus[] =
{
    ANIMCMD_FRAME(20, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusFaint[] =
{
    ANIMCMD_FRAME(24, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_StatusFrostbite[] =
{
    ANIMCMD_FRAME(28, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteTemplate_StatusCondition[] =
{
    sSpriteAnim_StatusPoison,
    sSpriteAnim_StatusParalyzed,
    sSpriteAnim_StatusSleep,
    sSpriteAnim_StatusFrozen,
    sSpriteAnim_StatusBurn,
    sSpriteAnim_StatusPokerus,
    sSpriteAnim_StatusFaint,
    sSpriteAnim_StatusFrostbite
};

static const struct CompressedSpriteSheet sSpriteSheet_StatusIcons =
{
#if SWSH_PARTY_MENU == TRUE
    .data = sStatusGfx_Icons_SwSh,
#else
    .data = gStatusGfx_Icons,
#endif
    .size = 0x400,
    .tag = TAG_STATUS_ICONS
};

static const struct SpritePalette sSpritePalette_StatusIcons =
{
    .data = sStatusPal_Icons_SwSh,
    .tag = TAG_HELD_ITEM
};

const struct SpriteTemplate gSpriteTemplate_StatusIcons =
{
    .tileTag = TAG_STATUS_ICONS,
    .paletteTag = TAG_HELD_ITEM,
    .oam = &sOamData_StatusCondition,
    .anims = sSpriteTemplate_StatusCondition,
};

static const struct OamData sOamData_MoveTypes =
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

static const union AnimCmd sSpriteAnim_TypeNone[] = {
    ANIMCMD_FRAME(TYPE_NONE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeStellar[] = {
    ANIMCMD_FRAME(TYPE_STELLAR * 4, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MoveTypes[NUMBER_OF_MON_TYPES] = {
    [TYPE_NONE]     = sSpriteAnim_TypeNone,
    [TYPE_NORMAL]   = sSpriteAnim_TypeNormal,
    [TYPE_FIGHTING] = sSpriteAnim_TypeFighting,
    [TYPE_FLYING]   = sSpriteAnim_TypeFlying,
    [TYPE_POISON]   = sSpriteAnim_TypePoison,
    [TYPE_GROUND]   = sSpriteAnim_TypeGround,
    [TYPE_ROCK]     = sSpriteAnim_TypeRock,
    [TYPE_BUG]      = sSpriteAnim_TypeBug,
    [TYPE_GHOST]    = sSpriteAnim_TypeGhost,
    [TYPE_STEEL]    = sSpriteAnim_TypeSteel,
    [TYPE_MYSTERY]  = sSpriteAnim_TypeMystery,
    [TYPE_FIRE]     = sSpriteAnim_TypeFire,
    [TYPE_WATER]    = sSpriteAnim_TypeWater,
    [TYPE_GRASS]    = sSpriteAnim_TypeGrass,
    [TYPE_ELECTRIC] = sSpriteAnim_TypeElectric,
    [TYPE_PSYCHIC]  = sSpriteAnim_TypePsychic,
    [TYPE_ICE]      = sSpriteAnim_TypeIce,
    [TYPE_DRAGON]   = sSpriteAnim_TypeDragon,
    [TYPE_DARK]     = sSpriteAnim_TypeDark,
    [TYPE_FAIRY]    = sSpriteAnim_TypeFairy,
    [TYPE_STELLAR]  = sSpriteAnim_TypeStellar,
};

static const u8 sMoveTypeToPalOffset[NUMBER_OF_MON_TYPES] =
{
    [TYPE_ELECTRIC] = 1,
    [TYPE_FAIRY]    = 1,
    [TYPE_BUG]      = 1,
    [TYPE_GRASS]    = 1,
    [TYPE_GROUND]   = 2,
    [TYPE_DARK]     = 2,
    [TYPE_FLYING]   = 2,
    [TYPE_GHOST]    = 2,
    [TYPE_WATER]    = 3,
    [TYPE_DRAGON]   = 3,
    [TYPE_STEEL]    = 4,
};

static const struct CompressedSpriteSheet sSpriteSheet_MoveTypes =
{
    .data = sMoveTypes_Gfx,
    .size = (NUMBER_OF_MON_TYPES) * 0x80,
    .tag = TAG_MOVE_TYPES
};

static const struct SpriteTemplate sSpriteTemplate_MoveTypes =
{
    .tileTag = TAG_MOVE_TYPES,
    .paletteTag = POKE_ICON_BASE_PAL_TAG,
    .oam = &sOamData_MoveTypes,
    .anims = sSpriteAnimTable_MoveTypes,
};

static const u8 *const sUnused_StatStrings[] =
{
    gText_HP4,
    gText_Attack3,
    gText_Defense3,
    gText_SpAtk4,
    gText_SpDef4,
    gText_Speed2
};

#define ROTOM_BASE_MOVE  MOVE_THUNDER_SHOCK
#define ROTOM_HEAT_MOVE  MOVE_OVERHEAT
#define ROTOM_WASH_MOVE  MOVE_HYDRO_PUMP
#define ROTOM_FROST_MOVE MOVE_BLIZZARD
#define ROTOM_FAN_MOVE   MOVE_AIR_SLASH
#define ROTOM_MOW_MOVE   MOVE_LEAF_STORM

static const u16 sRotomFormChangeMoves[5] =
{
    ROTOM_HEAT_MOVE,
    ROTOM_WASH_MOVE,
    ROTOM_FROST_MOVE,
    ROTOM_FAN_MOVE,
    ROTOM_MOW_MOVE,
};
