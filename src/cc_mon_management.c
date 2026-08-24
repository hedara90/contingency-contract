#include "cc_mon_management.h"
#include "item.h"
#include "pokedex.h"
#include "script.h"
#include "script_menu.h"
#include "even_sprite.h"
#include "event_data.h"
#include "event_scripts.h"
#include "list_menu.h"
#include "malloc.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "text.h"
#include "constants/pokeball.h"
#include "constants/items.h"
#include "constants/characters.h"

EWRAM_DATA struct GachaResult gGachaResults[10] = {0};
EWRAM_DATA u8 sPointSprite;

const u32 sPointsDisplayGfx[] = INCGFX_U32("graphics/gacha/points_indicator.png", ".4bpp");
const u16 sPointsDisplayPal[] = INCGFX_U16("graphics/gacha/points_indicator.png", ".gbapal");

const enum Item sItems4Stars[] =
{
//  Status Berries
    ITEM_CHERI_BERRY,
    ITEM_CHESTO_BERRY,
    ITEM_PECHA_BERRY,
    ITEM_RAWST_BERRY,
    ITEM_ASPEAR_BERRY,
    ITEM_PERSIM_BERRY,
//  Resist Berries
    ITEM_OCCA_BERRY,
    ITEM_PASSHO_BERRY,
    ITEM_WACAN_BERRY,
    ITEM_RINDO_BERRY,
    ITEM_YACHE_BERRY,
    ITEM_CHOPLE_BERRY,
    ITEM_KEBIA_BERRY,
    ITEM_SHUCA_BERRY,
    ITEM_COBA_BERRY,
    ITEM_PAYAPA_BERRY,
    ITEM_TANGA_BERRY,
    ITEM_CHARTI_BERRY,
    ITEM_KASIB_BERRY,
    ITEM_HABAN_BERRY,
    ITEM_COLBUR_BERRY,
    ITEM_BABIRI_BERRY,
    ITEM_CHILAN_BERRY,
    ITEM_ROSELI_BERRY,
//  Typal damage boosters
    ITEM_BLACK_BELT,
    ITEM_BLACK_GLASSES,
    ITEM_CHARCOAL,
    ITEM_DRAGON_FANG,
    ITEM_FAIRY_FEATHER,
    ITEM_HARD_STONE,
    ITEM_MAGNET,
    ITEM_METAL_COAT,
    ITEM_MIRACLE_SEED,
    ITEM_MYSTIC_WATER,
    ITEM_NEVER_MELT_ICE,
    ITEM_POISON_BARB,
    ITEM_SHARP_BEAK,
    ITEM_SILK_SCARF,
    ITEM_SILVER_POWDER,
    ITEM_SOFT_SAND,
    ITEM_SPELL_TAG,
    ITEM_TWISTED_SPOON,
};

const enum Item sItems5Stars[] =
{
    ITEM_RED_CARD,
    ITEM_LUM_BERRY,
    ITEM_SITRUS_BERRY,
    ITEM_EXPERT_BELT,
    ITEM_CUSTAP_BERRY,
    ITEM_AIR_BALLOON,
    ITEM_BLACK_SLUDGE,
    ITEM_LEFTOVERS,
    ITEM_EVIOLITE,
    ITEM_FOCUS_SASH,
    ITEM_HEAVY_DUTY_BOOTS,
    ITEM_IRON_BALL,
    ITEM_LOADED_DICE,
    ITEM_POWER_HERB,
    ITEM_PROTECTIVE_PADS,
    ITEM_ROCKY_HELMET,
    ITEM_THROAT_SPRAY,
    ITEM_WHITE_HERB,
    ITEM_WISE_GLASSES,
    ITEM_MUSCLE_BAND,
    ITEM_TOXIC_ORB,
    ITEM_FLAME_ORB,
    ITEM_SCOPE_LENS,
};

const enum Item sItems6Stars[] =
{
    ITEM_CHOICE_BAND,
    ITEM_CHOICE_SPECS,
    ITEM_CHOICE_SCARF,
    ITEM_EJECT_BUTTON,
    ITEM_EJECT_PACK,
    ITEM_ASSAULT_VEST,
};

const enum Species sIndomitability4Stars[] =
{
    SPECIES_STARAPTOR,
    SPECIES_RAMPARDOS,
    SPECIES_VAPOREON,
    SPECIES_UMBREON,
    SPECIES_PORYGON2,
    SPECIES_CURSOLA,
    SPECIES_ORICORIO_POM_POM,
    SPECIES_DRAGALGE,
    SPECIES_CERULEDGE,
    SPECIES_SANDSLASH_ALOLA,
    SPECIES_TOXICROAK,
    SPECIES_FROSLASS,
    SPECIES_CROBAT,
};

const enum Species sIndomitability5Stars[] =
{
    SPECIES_ARAQUANID,
    SPECIES_WHIMSICOTT,
    SPECIES_MAGMORTAR,
    SPECIES_GLISCOR,
    SPECIES_GARDEVOIR,
    SPECIES_MEDICHAM,
    SPECIES_SINISTCHA,
    SPECIES_PORYGON_Z,
};

const enum Species sIndomitability6Stars[] =
{
    SPECIES_SLOWKING_GALAR,
    SPECIES_ALOMOMOLA,
};

const enum Species sFury4Stars[] =
{
    SPECIES_DRUDDIGON,
    SPECIES_FLAREON,
    SPECIES_LEAFEON,
    SPECIES_GLACEON,
    SPECIES_MASQUERAIN,
    SPECIES_DUBWOOL,
    SPECIES_LYCANROC,
    SPECIES_SEISMITOAD,
    SPECIES_PAWMOT,
    SPECIES_STEELIX,
    SPECIES_COMFEY,
    SPECIES_CINCCINO,
    SPECIES_GRIMMSNARL,
};

const enum Species sFury5Stars[] =
{
    SPECIES_CORVIKNIGHT,
    SPECIES_GARGANACL,
    SPECIES_FERROTHORN,
    SPECIES_MAMOSWINE,
    SPECIES_PALOSSAND,
    SPECIES_INCINEROAR,
    SPECIES_WEAVILE,
    SPECIES_NIDOKING,
};

const enum Species sFury6Stars[] =
{
    SPECIES_GARCHOMP,
    SPECIES_HEATRAN,
};

const enum Species sMemories4Stars[] =
{
    SPECIES_MEOWSTIC,
    SPECIES_TOGEDEMARU,
    SPECIES_SYLVEON,
    SPECIES_JOLTEON,
    SPECIES_OCTILLERY,
    SPECIES_ELDEGOSS,
    SPECIES_VICTREEBEL,
    SPECIES_ARMAROUGE,
    SPECIES_LOKIX,
    SPECIES_CRUSTLE,
    SPECIES_BRELOOM,
    SPECIES_DRAMPA,
    SPECIES_ESPEON,
};

const enum Species sMemories5Stars[] =
{
    SPECIES_ROTOM,
    SPECIES_SCIZOR,
    SPECIES_SAMUROTT_HISUI,
    SPECIES_HERACROSS,
    SPECIES_TYRANTRUM,
    SPECIES_NINETALES_ALOLA,
    SPECIES_GLIMMORA,
    SPECIES_GALVANTULA,
};

const enum Species sMemories6Stars[] =
{
    SPECIES_IRON_VALIANT,
    SPECIES_DRAGAPULT,
};

const struct GachaBanner sBannerItems =
{
    .num4Stars = NELEMS(sItems4Stars),
    .num5Stars = NELEMS(sItems5Stars),
    .num6Stars = NELEMS(sItems6Stars),
    .items4Star = sItems4Stars,
    .items5Star = sItems5Stars,
    .items6Star = sItems6Stars,
};

const struct GachaBanner sBannerIndomitability =
{
    .num4Stars = NELEMS(sIndomitability4Stars),
    .num5Stars = NELEMS(sIndomitability5Stars),
    .num6Stars = NELEMS(sIndomitability6Stars),
    .mons4Star = sIndomitability4Stars,
    .mons5Star = sIndomitability5Stars,
    .mons6Star = sIndomitability6Stars,
};

const struct GachaBanner sBannerFury =
{
    .num4Stars = NELEMS(sFury4Stars),
    .num5Stars = NELEMS(sFury5Stars),
    .num6Stars = NELEMS(sFury6Stars),
    .mons4Star = sFury4Stars,
    .mons5Star = sFury5Stars,
    .mons6Star = sFury6Stars,
};

const struct GachaBanner sBannerMemories =
{
    .num4Stars = NELEMS(sMemories4Stars),
    .num5Stars = NELEMS(sMemories5Stars),
    .num6Stars = NELEMS(sMemories6Stars),
    .mons4Star = sMemories4Stars,
    .mons5Star = sMemories5Stars,
    .mons6Star = sMemories6Stars,
};

const struct GachaBanner sGachaBanners[BANNER_COUNT] =
{
    [BANNER_ITEMS] = sBannerItems,
    [BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT] = sBannerIndomitability,
    [BANNER_FURY_OF_THE_EARTHEN_CORE] = sBannerFury,
    [BANNER_MEMORIES_OF_MONTHS_PAST] = sBannerMemories,
};

enum GiveResult GiveGachaMon(enum Species species, u32 star)
{
    //  First see if the player has the mon at all, done with dex flags
    bool32 hasMon = GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT);
    if (hasMon)
    {

        //  Iterate over all mons that the player has in party
        for (u32 i = 0; i < 6; i++)
        {
            if (GetMonData(&gParties[0][i], MON_DATA_SPECIES) == species)
            {
                u8 marking = GetMonData(&gParties[0][i], MON_DATA_MARKINGS);
                if (marking == 0xF)
                {
                    bool32 isShiny = GetMonData(&gParties[0][i], MON_DATA_IS_SHINY);
                    if (isShiny)
                        return GIVE_RESULT_CAP;

                    isShiny = TRUE;
                    SetMonData(&gParties[0][i], MON_DATA_IS_SHINY, &isShiny);
                    return GIVE_RESULT_DUPE;
                }
                marking = 1 | (marking << 1);
                SetMonData(&gParties[0][i], MON_DATA_MARKINGS, &marking);
                return GIVE_RESULT_DUPE;
            }
        }

        for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        {
            for (u32 index = 0; index < 30; index++)
            {
                struct BoxPokemon *mon = GetBoxedMonPtr(box, index);
                if (GetBoxMonData(mon, MON_DATA_SPECIES) == species)
                {
                    u8 marking = GetBoxMonData(mon, MON_DATA_MARKINGS);
                    if (marking == 0xF)
                    {
                        bool32 isShiny = GetBoxMonData(mon, MON_DATA_IS_SHINY);
                        if (isShiny)
                            return GIVE_RESULT_CAP;

                        isShiny = TRUE;
                        SetBoxMonData(mon, MON_DATA_MARKINGS, &isShiny);
                        return GIVE_RESULT_DUPE;
                    }
                    marking = 1 | (marking << 1);
                    SetBoxMonData(mon, MON_DATA_MARKINGS, &marking);
                    return GIVE_RESULT_DUPE;
                }
            }
        }

        //  Iterate over all mons in the boxes
        return GIVE_RESULT_FIRST;
    }
    else
    {
        //  Give the mon to the player
        VarSet(VAR_TEMP_1, species);
        RunScriptImmediately(EventScript_GiveGachaMon);
        //  Find the newly given mon and set its ball properly
        bool32 found = FALSE;
        for (u32 i = 0; i < 6; i++)
        {
            if (GetMonData(&gParties[0][i], MON_DATA_SPECIES) == species)
            {
                found = TRUE;
                enum PokeBall ball = BALL_POKE;
                switch (star)
                {
                case 4:
                    ball = BALL_4_STAR;
                    break;
                case 5:
                    ball = BALL_6_STAR;
                    break;
                case 6:
                    ball = BALL_5_STAR;
                    break;
                }
                SetMonData(&gParties[0][i], MON_DATA_POKEBALL, &ball);
            }
        }

        if (!found)
        {
            for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
            {
                for (u32 index = 0; index < 30; index++)
                {
                    struct BoxPokemon *mon = GetBoxedMonPtr(box, index);
                    if (GetBoxMonData(mon, MON_DATA_SPECIES) == species)
                    {
                        found = TRUE;
                        enum PokeBall ball = BALL_POKE;
                        switch (star)
                        {
                        case 4:
                            ball = BALL_4_STAR;
                            break;
                        case 5:
                            ball = BALL_6_STAR;
                            break;
                        case 6:
                            ball = BALL_5_STAR;
                            break;
                        }
                        SetBoxMonData(mon, MON_DATA_POKEBALL, &ball);
                    }
                }
            }
        }

        return GIVE_RESULT_FIRST;
    }
}

void TestThing(void)
{
    DebugPrintf("Doing 10-pull");
    Do10Pull(BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT);
}

enum Species RollGachaMon(enum Banner banner, u32 *star)
{
    enum Species species = SPECIES_NONE;
    const struct GachaBanner *bannerData = &sGachaBanners[banner];

    u32 rnd = 0;
    switch (banner)
    {
    case BANNER_COUNT:
    case BANNER_ITEMS:
    case BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT:
        rnd = LocalRandom32(&gSaveBlock1Ptr->bannerRng[BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT]);
        break;
    case BANNER_FURY_OF_THE_EARTHEN_CORE:
        rnd = LocalRandom32(&gSaveBlock1Ptr->bannerRng[BANNER_FURY_OF_THE_EARTHEN_CORE]);
        break;
    case BANNER_MEMORIES_OF_MONTHS_PAST:
        rnd = LocalRandom32(&gSaveBlock1Ptr->bannerRng[BANNER_MEMORIES_OF_MONTHS_PAST]);
        break;
    }
    u32 rndHigh = rnd >> 16;
    u32 rndLow = rnd & 0xFFFF;

    u32 starToUse = 4;
    if (rndHigh % ODDS_6_STAR == 0)
    {
        gSaveBlock1Ptr->pity5 = 0;
        gSaveBlock1Ptr->pity6 = 0;
        starToUse = 6;
    }
    else if (rndHigh % ODDS_5_STAR == 0)
    {
        gSaveBlock1Ptr->pity5 = 0;
        starToUse = 5;
    }

    if (gSaveBlock1Ptr->pity6 == PITY_6_STAR - 1)
    {
        gSaveBlock1Ptr->pity5 = 0;
        gSaveBlock1Ptr->pity6 = 0;
        starToUse = 6;
    }
    else if (gSaveBlock1Ptr->pity5 == PITY_5_STAR)
    {
        gSaveBlock1Ptr->pity6++;
        gSaveBlock1Ptr->pity5 = 0;
        starToUse = 5;
    }
    else
    {
        gSaveBlock1Ptr->pity5++;
        gSaveBlock1Ptr->pity6++;
    }

    *star = starToUse;
    switch (starToUse)
    {
    case 6:
        species = bannerData->mons6Star[rndLow % bannerData->num6Stars];
        break;
    case 5:
        species = bannerData->mons5Star[rndLow % bannerData->num5Stars];
        break;
    case 4:
        species = bannerData->mons4Star[rndLow % bannerData->num4Stars];
        break;
    }

    return species;
}

static enum Item RollGachaItem(enum Banner banner, u32 *star)
{
    enum Item item = ITEM_NONE;
    const struct GachaBanner *bannerData = &sGachaBanners[banner];

    u32 rnd = 0;
    switch (banner)
    {
    case BANNER_COUNT:
    case BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT:
    case BANNER_FURY_OF_THE_EARTHEN_CORE:
    case BANNER_MEMORIES_OF_MONTHS_PAST:
    case BANNER_ITEMS:
        rnd = LocalRandom32(&gSaveBlock1Ptr->bannerRng[BANNER_ITEMS]);
    }
    u32 rndHigh = rnd >> 16;
    u32 rndLow = rnd & 0xFFFF;

    u32 starToUse = 4;
    if (rndHigh % ODDS_ITEM_6_STAR == 0)
    {
        gSaveBlock1Ptr->pityItem5 = 0;
        gSaveBlock1Ptr->pityItem6 = 0;
        starToUse = 6;
    }
    else if (rndHigh % ODDS_ITEM_5_STAR == 0)
    {
        gSaveBlock1Ptr->pityItem5 = 0;
        starToUse = 5;
    }

    if (gSaveBlock1Ptr->pityItem6 == PITY_ITEM_6_STAR - 1)
    {
        gSaveBlock1Ptr->pity5 = 0;
        gSaveBlock1Ptr->pity6 = 0;
        starToUse = 6;
    }
    else if (gSaveBlock1Ptr->pityItem5 == PITY_ITEM_5_STAR)
    {
        gSaveBlock1Ptr->pityItem6++;
        gSaveBlock1Ptr->pityItem5 = 0;
        starToUse = 5;
    }
    else
    {
        gSaveBlock1Ptr->pityItem5++;
        gSaveBlock1Ptr->pityItem6++;
    }

    *star = starToUse;
    switch (starToUse)
    {
    case 6:
        item = bannerData->items6Star[rndLow % bannerData->num6Stars];
        break;
    case 5:
        item = bannerData->items5Star[rndLow % bannerData->num5Stars];
        break;
    case 4:
        item = bannerData->items4Star[rndLow % bannerData->num4Stars];
        break;
    }

    return item;
}

static bool32 HasItemAlready(enum Item item)
{
    //  Look in bag for item
    bool32 hasItem = CheckBagHasItem(item, 1);
    if (!hasItem)
    {
        //  Look in party for item
        for (u32 i = 0; i < 6; i++)
        {
            if (GetMonData(&gParties[0][i], MON_DATA_SPECIES) != SPECIES_NONE)
            {
                if (GetMonData(&gParties[0][i], MON_DATA_HELD_ITEM) == item)
                {
                    hasItem = TRUE;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (!hasItem)
    {
        //  Look in boxes for item
        for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        {
            for (u32 index = 0; index < 30; index++)
            {
                struct BoxPokemon *mon = GetBoxedMonPtr(box, index);
                if (GetBoxMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE)
                {
                    if (GetBoxMonData(mon, MON_DATA_HELD_ITEM) == item)
                    {
                        hasItem = TRUE;
                        break;
                    }
                }
            }

            if (hasItem)
                break;
        }
    }

    return hasItem;
}

enum GiveResult GiveGachaItem(enum Item item, u32 star)
{
    bool32 hasItem = HasItemAlready(item);

    if (!hasItem)
    {
        AddBagItem(item, 1);
        return GIVE_RESULT_FIRST;
    }
    else
    {
        return GIVE_RESULT_DUPE;
    }
}

void DoSinglePull(enum Banner banner)
{
    for (u32 i = 0; i < 10; i++)
    {
        enum Species species = SPECIES_NONE;
        enum GiveResult result = GIVE_RESULT_FIRST;
        gGachaResults[i].species = species;
        gGachaResults[i].result = result;
    }

    enum GiveResult result;
    u32 star = 4;;
    if (banner == BANNER_ITEMS)
    {
        enum Item item = RollGachaItem(banner, &star);
        result = GiveGachaItem(item, star);
        gGachaResults[0].item = item;
    }
    else
    {
        enum Species species = RollGachaMon(banner, &star);
        result = GiveGachaMon(species, star);
        gGachaResults[0].species = species;
    }
    gGachaResults[0].result = result;
    gGachaResults[0].stars = star;
}

void Do10Pull(enum Banner banner)
{
    for (u32 i = 0; i < 10; i++)
    {
        enum GiveResult result;
        u32 star = 4;
        if (banner == BANNER_ITEMS)
        {
            enum Item item = RollGachaItem(banner, &star);
            result = GiveGachaItem(item, star);
            gGachaResults[i].item = item;
        }
        else
        {
            enum Species species = RollGachaMon(banner, &star);
            result = GiveGachaMon(species, star);
            gGachaResults[i].species = species;
        }
        gGachaResults[i].result = result;
        gGachaResults[i].stars = star;
    }
}

bool32 CheckRarities(void)
{
    bool32 passed = TRUE;
    for (u32 i = 0; i < NELEMS(sIndomitability4Stars); i++)
    {
        if (gSpeciesInfo[sIndomitability4Stars[i]].rarity != 4)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (4-star)", GetSpeciesName(sIndomitability4Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sFury4Stars); i++)
    {
        if (gSpeciesInfo[sFury4Stars[i]].rarity != 4)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (4-star)", GetSpeciesName(sFury4Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sMemories4Stars); i++)
    {
        if (gSpeciesInfo[sMemories4Stars[i]].rarity != 4)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (4-star)", GetSpeciesName(sMemories4Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sIndomitability5Stars); i++)
    {
        if (gSpeciesInfo[sIndomitability5Stars[i]].rarity != 5)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (5-star)", GetSpeciesName(sIndomitability5Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sFury5Stars); i++)
    {
        if (gSpeciesInfo[sFury5Stars[i]].rarity != 5)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (5-star)", GetSpeciesName(sFury5Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sMemories5Stars); i++)
    {
        if (gSpeciesInfo[sMemories5Stars[i]].rarity != 5)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (5-star)", GetSpeciesName(sMemories5Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sIndomitability6Stars); i++)
    {
        if (gSpeciesInfo[sIndomitability6Stars[i]].rarity != 6)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (6-star)", GetSpeciesName(sIndomitability6Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sFury6Stars); i++)
    {
        if (gSpeciesInfo[sFury6Stars[i]].rarity != 6)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (6-star)", GetSpeciesName(sFury6Stars[i]));
        }
    }

    for (u32 i = 0; i < NELEMS(sMemories6Stars); i++)
    {
        if (gSpeciesInfo[sMemories6Stars[i]].rarity != 6)
        {
            passed = FALSE;
            DebugPrintf("Missing %S (6-star)", GetSpeciesName(sMemories6Stars[i]));
        }
    }
    return passed;
}

const u8 sNoItemString[] = _("Nevermind…");
void BuildListOf4StarItems(void)
{
    u8 *buffer = Alloc(20);
    u32 index = 0;
    while (sNoItemString[index] != EOS)
    {
        buffer[index] = sNoItemString[index];
        index++;
    }
    buffer[index] = EOS;
    struct ListMenuItem basic;
    basic.name = buffer;
    basic.id = ITEM_NONE;
    MultichoiceDynamic_PushElement(basic);

    for (u32 i = 0; i < NELEMS(sItems4Stars); i++)
    {
        if (!HasItemAlready(sItems4Stars[i]))
        {
            u32 size = 0;
            const u8 *itemName = GetItemName(sItems4Stars[i]);
            while (itemName[size] != EOS)
                size++;
            size++;
            u8 *nameBuffer = Alloc(size);
            for (u32 i = 0; i < size; i++)
            {
                nameBuffer[i] = itemName[i];
            }
            struct ListMenuItem item;
            item.name = nameBuffer;
            item.id = sItems4Stars[i];
            MultichoiceDynamic_PushElement(item);
        }
    }
}

void BuildListOf5StarItems(void)
{
    u8 *buffer = Alloc(20);
    u32 index = 0;
    while (sNoItemString[index] != EOS)
    {
        buffer[index] = sNoItemString[index];
        index++;
    }
    buffer[index] = EOS;
    struct ListMenuItem basic;
    basic.name = buffer;
    basic.id = ITEM_NONE;
    MultichoiceDynamic_PushElement(basic);

    for (u32 i = 0; i < NELEMS(sItems5Stars); i++)
    {
        if (!HasItemAlready(sItems5Stars[i]))
        {
            u32 size = 0;
            const u8 *itemName = GetItemName(sItems5Stars[i]);
            while (itemName[size] != EOS)
                size++;
            size++;
            u8 *nameBuffer = Alloc(size);
            for (u32 i = 0; i < size; i++)
            {
                nameBuffer[i] = itemName[i];
            }
            struct ListMenuItem item;
            item.name = nameBuffer;
            item.id = sItems5Stars[i];
            MultichoiceDynamic_PushElement(item);
        }
    }
}

void BuildListOf6StarItems(void)
{
    u8 *buffer = Alloc(20);
    u32 index = 0;
    while (sNoItemString[index] != EOS)
    {
        buffer[index] = sNoItemString[index];
        index++;
    }
    buffer[index] = EOS;
    struct ListMenuItem basic;
    basic.name = buffer;
    basic.id = ITEM_NONE;
    MultichoiceDynamic_PushElement(basic);

    for (u32 i = 0; i < NELEMS(sItems6Stars); i++)
    {
        if (!HasItemAlready(sItems6Stars[i]))
        {
            u32 size = 0;
            const u8 *itemName = GetItemName(sItems6Stars[i]);
            while (itemName[size] != EOS)
                size++;
            size++;
            u8 *nameBuffer = Alloc(size);
            for (u32 i = 0; i < size; i++)
            {
                nameBuffer[i] = itemName[i];
            }
            struct ListMenuItem item;
            item.name = nameBuffer;
            item.id = sItems6Stars[i];
            MultichoiceDynamic_PushElement(item);
        }
    }
}

const union TextColor sTextColor =
{
    .background = 0,
    .foreground = 1,
    .accent = 0,
    .shadow = 4,
};

void DisplayItemPoints(void)
{
    u32 currBP = gSaveBlock2Ptr->frontier.battlePoints;

    DebugPrintf("showing sprite");

    struct Even_CreateSpriteStruct cs = {0};
    cs.sprite = sPointsDisplayGfx;
    cs.tileTag = 0xDEDE;
    cs.palette = sPointsDisplayPal;
    cs.palTag = 0xDEDE;
    cs.posX = 240 - 32;
    cs.posY = 16;
    cs.spriteSize = SPRITE_SIZE(64x32);
    cs.spriteShape = SPRITE_SHAPE(64x32);
    sPointSprite = Even_CreateSprite(&cs);

    u8 text[8];
    ConvertIntToDecimalStringN(text, currBP, STR_CONV_MODE_LEFT_ALIGN, 4);
    u32 stringWidth = GetStringWidth(FONT_NORMAL, text, 0);

    AddSpriteTextPrinterParameterized6(sPointSprite, FONT_NORMAL, 32 - stringWidth / 2, 8, 0, 0, sTextColor, 0, text);
}

void HideItemPoints(void)
{
    DestroySprite(&gSprites[sPointSprite]);
    FreeSpriteTilesByTag(0xDEDE);
    FreeSpritePaletteByTag(0xDEDE);
}

void GiveItemFromPointShop(void)
{
    enum Item item = VarGet(VAR_RESULT);

    u32 rarity = 0;
    for (u32 i = 0; i < NELEMS(sItems6Stars); i++)
    {
        if (sItems6Stars[i] == item)
        {
            rarity = 6;
            break;
        }
    }

    if (rarity == 0)
    {
        for (u32 i = 0; i < NELEMS(sItems5Stars); i++)
        {
            if (sItems5Stars[i] == item)
            {
                rarity = 5;
                break;
            }
        }
    }

    if (rarity == 0)
    {
        for (u32 i = 0; i < NELEMS(sItems4Stars); i++)
        {
            if (sItems4Stars[i] == item)
            {
                rarity = 4;
                break;
            }
        }
    }

    bool32 canGiveItem = FALSE;
    u32 currBP = gSaveBlock2Ptr->frontier.battlePoints;
    switch (rarity)
    {
    case 4:
        if (currBP >= 16)
        {
            canGiveItem = TRUE;
            gSaveBlock2Ptr->frontier.battlePoints -= 16;
        }
        break;
    case 5:
        if (currBP >= 64)
        {
            canGiveItem = TRUE;
            gSaveBlock2Ptr->frontier.battlePoints -= 64;
        }
        break;
    case 6:
        if (currBP >= 256)
        {
            canGiveItem = TRUE;
            gSaveBlock2Ptr->frontier.battlePoints -= 256;
        }
        break;
    }

    if (canGiveItem)
    {
        AddBagItem(item, 1);
        VarSet(VAR_RESULT, 1);
    }
    else
    {
        VarSet(VAR_RESULT, 0);
    }
}
