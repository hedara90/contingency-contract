#include "gba/types.h"
#include "global.h"
#include "even_80px_trainers.h"
#include "even_sprite.h"
#include "malloc.h"
#include "menu.h"

const struct BigSpriteData sBigSpriteDatas[6] =
{
    {-8,  -8,  SPRITE_SIZE(64x64), SPRITE_SHAPE(64x64), 0,         512},
    { 32, -24, SPRITE_SIZE(16x32), SPRITE_SHAPE(16x32), 512,       64},
    { 32,  8,  SPRITE_SIZE(16x32), SPRITE_SHAPE(16x32), 512 + 64,  64},
    {-24,  32, SPRITE_SIZE(32x16), SPRITE_SHAPE(32x16), 512 + 128, 64},
    { 8,   32, SPRITE_SIZE(32x16), SPRITE_SHAPE(32x16), 512 + 192, 64},
    { 32,  32, SPRITE_SIZE(16x16), SPRITE_SHAPE(16x16), 512 + 256, 32}
};

void Even_CreateBigSprite(struct Even_BigSpriteSheet *bigSpriteSheet, struct Even_BigSprite *bigSprite)
{
    u32 *tempSprite = NULL;
    const u32 *sprite;
    if (bigSpriteSheet->compressed)
    {
        tempSprite = malloc_and_decompress(bigSpriteSheet->sprite, NULL);
        sprite = tempSprite;
    }
    else
    {
        sprite = bigSpriteSheet->sprite;
    }

    bigSprite->posX = bigSpriteSheet->posX;
    bigSprite->posY = bigSpriteSheet->posY;
    for (u32 i = 0; i < 6; i++)
    {
        struct Even_CreateSpriteStruct spriteStruct = {0};
        spriteStruct.sprite = &sprite[sBigSpriteDatas[i].spriteOffset];
        spriteStruct.spriteCompressed = FALSE;
        spriteStruct.tileTag = bigSpriteSheet->tileTag + i;
        if (i == 0)
            spriteStruct.palette = bigSpriteSheet->palette;
        spriteStruct.palTag = bigSpriteSheet->palTag;
        spriteStruct.spriteSize = sBigSpriteDatas[i].spriteSize;
        spriteStruct.spriteShape = sBigSpriteDatas[i].spriteShape;
        spriteStruct.posX = bigSpriteSheet->posX + sBigSpriteDatas[i].xOffset;
        spriteStruct.posY = bigSpriteSheet->posY + sBigSpriteDatas[i].yOffset;
        spriteStruct.subpriority = bigSpriteSheet->subpriority;
        bigSprite->spriteIds[i] = Even_CreateSprite(&spriteStruct);
        bigSprite->tileStarts[i] = gSprites[bigSprite->spriteIds[i]].sheetTileStart;
        bigSprite->tileTags[i] = bigSpriteSheet->tileTag + i;
    }
    bigSprite->palTag = bigSpriteSheet->palTag;
    if (bigSpriteSheet->compressed)
    {
        Free(tempSprite);
    }
    bigSprite->isInitialized = TRUE;
}

void Even_FreeBigSprite(struct Even_BigSprite *bigSprite)
{
    if (!bigSprite->isInitialized)
        return;

    for (u32 i = 0; i < 6; i++)
    {
        FreeSpriteOamMatrix(&gSprites[bigSprite->spriteIds[i]]);
        DestroySprite(&gSprites[bigSprite->spriteIds[i]]);
        FreeSpriteTilesByTag(bigSprite->tileTags[i]);
    }
    FreeSpritePaletteByTag(bigSprite->palTag);
    bigSprite->isInitialized = FALSE;
}

void Even_MoveBigSprite(struct Even_BigSprite *bigSprite, s32 deltaX, s32 deltaY)
{
    bigSprite->posX += deltaX;
    bigSprite->posY += deltaY;
    for (u32 i = 0; i < 6; i++)
    {
        gSprites[bigSprite->spriteIds[i]].x += deltaX;
        gSprites[bigSprite->spriteIds[i]].y += deltaY;
    }
}

void Even_SetBigSpritePos(struct Even_BigSprite *bigSprite, s32 posX, s32 posY)
{
    bigSprite->posX = posX;
    bigSprite->posY = posY;
    for (u32 i = 0; i < 6; i++)
    {
        gSprites[bigSprite->spriteIds[i]].x = posX + sBigSpriteDatas[i].xOffset;
        gSprites[bigSprite->spriteIds[i]].y = posY + sBigSpriteDatas[i].yOffset;
    }
}

void Even_ChangeBigSpriteGraphics(struct Even_BigSprite *bigSprite, const u32 *newGraphics, bool32 compressed)
{
    u32 *sprite = NULL;
    if (compressed)
        sprite = malloc_and_decompress(newGraphics, NULL);
    else
        sprite = (u32 *)newGraphics;

    for (u32 i = 0; i < 6; i++)
    {
        u32 tileStart = bigSprite->tileStarts[i];
        u32 *dst = (u32 *)(OBJ_VRAM0 + tileStart * TILE_SIZE_4BPP);
        u32 *src = &sprite[sBigSpriteDatas[i].spriteOffset];
        for (u32 j = 0; j < sBigSpriteDatas[i].vramWords; j++)
            dst[j] = src[j];
    }

    if (compressed)
        Free(sprite);
}

s32 Even_GetBigSubspriteOffsetX(u32 index)
{
    return sBigSpriteDatas[index].xOffset;
}

s32 Even_GetBigSubspriteOffsetY(u32 index)
{
    return sBigSpriteDatas[index].yOffset;
}
