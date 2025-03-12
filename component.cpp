#include "component.h"
#include "animation.h"

/*-----------------------------------------------------------------------------------------------*/

CAnimation::CAnimation() : frame(0)
{
}

CAnimation::CAnimation(Animation* a, bool rpt) : animation(a), frame(0), repeat(rpt)
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

/*-----------------------------------------------------------------------------------------------*/

CLifeSpan::CLifeSpan()
{
}

CLifeSpan::CLifeSpan(int duration, int frame) : lifespan(duration), frameCreated(frame)
{
}

/*-----------------------------------------------------------------------------------------------*/

CState::CState()
{
}

CState::CState(const std::string& s) : state(s)
{
}

/*-----------------------------------------------------------------------------------------------*/

CDamage::CDamage()
{
}

CDamage::CDamage(int dmg) : damage(dmg)
{
}

/*-----------------------------------------------------------------------------------------------*/

CInvincibility::CInvincibility()
{
}

CInvincibility::CInvincibility(int ifrm) : iFrames(ifrm)
{
}

/*-----------------------------------------------------------------------------------------------*/

CHealth::CHealth()
{
}

CHealth::CHealth(int m, int c) : max(m), current(c)
{
}

/*-----------------------------------------------------------------------------------------------*/

CFollowPlayer::CFollowPlayer()
{
    
}

CFollowPlayer::CFollowPlayer(const Vec2& pos, float spd) : home(pos), speed(spd)
{
}

/*-----------------------------------------------------------------------------------------------*/

CPatrol::CPatrol()
{
}

CPatrol::CPatrol(float spd) : speed(spd)
{
    positions.reserve(10);
}

void CPatrol::addPosition(float x, float y)
{
    positions.emplace_back(x, y);
}
