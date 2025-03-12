#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>
#include "vector2d.h"
#include "animation.h"

class CComponent
{
public:
    bool has;
};

class CAnimation : public CComponent
{
    Animation* animation;

public:
    int frame;
    bool repeat = false;
    
    CAnimation();
    CAnimation(Animation* a, bool rpt);
    Animation* getAnimation();
};

class CTransform : public CComponent
{
public:
    Vec2 pos        = { 0.0, 0.0 };
    Vec2 prevPos    = { 0.0, 0.0 };
    Vec2 scale      = { 1.0, 1.0 };
    Vec2 speed      = { 0.0, 0.0 };
    
    CTransform();
    CTransform(const Vec2& p);
};

class CBoundingBox : public CComponent
{
public:
    Vec2 size       = { 0.0, 0.0 };
    Vec2 halfSize   = { 0.0, 0.0 };
    int blocksMovement = 0;
    int blocksVision = 0;
    
    CBoundingBox();
    CBoundingBox(const Vec2& s, int bm, int bv);
};

class CLifeSpan : public CComponent
{
public:
    int lifespan = 0;
    int frameCreated = 0;
    
    CLifeSpan();
    CLifeSpan(int duration, int frame);
};

class CInput : public CComponent
{
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

class CGravity : public CComponent
{
public:
    float gravity;
};

class CState : public CComponent
{
public:
    std::string state = "none";
    std::string prev = "none";
    
    CState();
    CState(const std::string& s);
};

class CDamage : public CComponent
{
public:
    int damage = 1;
    
    CDamage();
    CDamage(int dmg);
};

class CInvincibility : public CComponent
{
public:
    int iFrames = 0;
    
    CInvincibility();
    CInvincibility(int ifrm);
};

class CHealth : public CComponent
{
public:
    int max = 0;
    int current = 0;
    
    CHealth();
    CHealth(int m, int c);
};

class CFollowPlayer : public CComponent
{
public:
    Vec2 home = {0, 0};
    float speed = 0;
    
    CFollowPlayer();
    CFollowPlayer(const Vec2& pos, float spd);
};

class CPatrol : public CComponent
{
public:
    std::vector<Vec2> positions;
    size_t currentPos = 0;
    float speed = 0;
    
    CPatrol();
    CPatrol(float spd);
    
    void addPosition(float x, float y);
};

#endif
