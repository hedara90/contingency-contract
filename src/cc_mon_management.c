#include "cc_mon_management.h"
#include "pokedex.h"
#include "script.h"
#include "event_data.h"
#include "event_scripts.h"
#include "pokemon_storage_system.h"

#define ODDS_6_STAR 100
#define ODDS_5_STAR 4
#define PITY_6_STAR 80
#define PITY_5_STAR 3

struct GachaResult
{
    enum Species species;
    enum GiveResult result;
};

EWRAM_DATA struct GachaResult sGachaResults[10] = {0};

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
    .num4Stars = NELEMS(sIndomitability4Stars),
    .num5Stars = NELEMS(sIndomitability5Stars),
    .num6Stars = NELEMS(sIndomitability6Stars),
    .mons4Star = sIndomitability4Stars,
    .mons5Star = sIndomitability5Stars,
    .mons6Star = sIndomitability6Stars,
};

const struct GachaBanner sBannerMemories =
{
    .num4Stars = NELEMS(sIndomitability4Stars),
    .num5Stars = NELEMS(sIndomitability5Stars),
    .num6Stars = NELEMS(sIndomitability6Stars),
    .mons4Star = sIndomitability4Stars,
    .mons5Star = sIndomitability5Stars,
    .mons6Star = sIndomitability6Stars,
};

const struct GachaBanner sGachaBanners[BANNER_COUNT] =
{
    [BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT] = sBannerIndomitability,
    [BANNER_FURY_OF_THE_EARTHERN_CORE] = sBannerFury,
    [BANNER_MEMORIES_OF_MONTHS_PAST] = sBannerMemories,
};

enum GiveResult GiveGachaMon(enum Species species)
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
        return GIVE_RESULT_FIRST;
    }
}

void TestThing(void)
{
    DebugPrintf("Doing 10-pull");
    Do10Pull(BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT);
    //GiveGachaMon(SPECIES_MUDKIP);
    //GiveGachaMon(SPECIES_ZYGARDE);
}

enum Species RollGachaMon(enum Banner banner)
{
    enum Species species = SPECIES_NONE;
    const struct GachaBanner *bannerData = &sGachaBanners[banner];

    u32 rnd = 0;
    switch (banner)
    {
    case BANNER_COUNT:
    case BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT:
        rnd = LocalRandom32(&gSaveBlock1Ptr->bannerRng[BANNER_INDOMITABILITY_OF_THE_UNBREAKABLE_SPIRIT]);
        break;
    case BANNER_FURY_OF_THE_EARTHERN_CORE:
        rnd = LocalRandom32(&gSaveBlock1Ptr->bannerRng[BANNER_FURY_OF_THE_EARTHERN_CORE]);
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

    if (gSaveBlock1Ptr->pity6 == PITY_6_STAR)
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

void DoSinglePull(enum Banner banner)
{
    for (u32 i = 0; i < 10; i++)
    {
        enum Species species = SPECIES_NONE;
        enum GiveResult result = GIVE_RESULT_FIRST;
        sGachaResults[i].species = species;
        sGachaResults[i].result = result;
    }

    enum Species species = RollGachaMon(banner);
    enum GiveResult result = GiveGachaMon(species);
    sGachaResults[0].species = species;
    sGachaResults[0].result = result;
}

void Do10Pull(enum Banner banner)
{
    for (u32 i = 0; i < 10; i++)
    {
        enum Species species = RollGachaMon(banner);
        enum GiveResult result = GiveGachaMon(species);
        sGachaResults[i].species = species;
        sGachaResults[i].result = result;
    }
}
