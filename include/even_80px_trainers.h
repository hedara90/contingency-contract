#ifndef GUARD_EVEN_80PX_TRAINERS
#define GUARD_EVEN_80PX_TRAINERS

#include "gba/types.h"
#include "sprite.h"

struct Even_BigSpriteSheet
{
    const u32 *sprite;
    const u16 *palette;
    u16 tileTag;
    u16 palTag;
    s16 posX;
    s16 posY;
    bool8 compressed;
    u8 subpriority;
};

struct Even_BigSprite
{
    s16 posX;
    s16 posY;
    u16 tileStarts[6];
    s16 data[8];
    u8 spriteIds[6];
    u16 tileTags[6];
    u16 palTag;
    bool8 isInitialized;
};

struct BigSpriteData
{
    s8 xOffset;
    s8 yOffset;
    u8 spriteSize;
    u8 spriteShape;
    u32 spriteOffset;
    u32 vramWords;
};

void Even_CreateBigSprite(struct Even_BigSpriteSheet *bisSpriteSheet, struct Even_BigSprite *bigSprite);
void Even_FreeBigSprite(struct Even_BigSprite *bigSprite);
void Even_MoveBigSprite(struct Even_BigSprite *bigSprite, s32 deltaX, s32 deltaY);
void Even_SetBigSpritePos(struct Even_BigSprite *bigSprite, s32 posX, s32 posY);
void Even_ChangeBigSpriteGraphics(struct Even_BigSprite *bigSprite, const u32 *newGraphics, bool32 compressed);
s32 Even_GetBigSubspriteOffsetX(u32 index);
s32 Even_GetBigSubspriteOffsetY(u32 index);

#endif
