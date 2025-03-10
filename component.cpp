#include "component.h"
#include "animation.h"

/*-----------------------------------------------------------------------------------------------*/

CAnimation::CAnimation() : frame(0)
{
}

CAnimation::CAnimation(Animation* a) : animation(a), frame(0)
{
}

Animation* CAnimation::getAnimation()
{
    return animation;
}

/*-----------------------------------------------------------------------------------------------*/

CTransform::CTransform()
{
}

CTransform::CTransform(const Vec2& p) : pos(p)
{
}

/*-----------------------------------------------------------------------------------------------*/

CBoundingBox::CBoundingBox()
{
}

CBoundingBox::CBoundingBox(const Vec2& s, int bm, int bv) : size(s), halfSize(s.x/2, s.y/2), blocksMovement(bm), blocksVision(bv)
{
}
