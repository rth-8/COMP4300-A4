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
    
    CAnimation();
    CAnimation(Animation* a);
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
};

#endif
