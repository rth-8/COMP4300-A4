#include <iostream>
#include <memory>

#include "entity.h"

Entity::Entity(const std::string & tag, int id)
{
    this->id = id;
    this->tag = tag;
    
    this->removeComponent<CAnimation>();
    this->removeComponent<CTransform>();
    this->removeComponent<CBoundingBox>();
    this->removeComponent<CLifeSpan>();
    this->removeComponent<CInput>();
    this->removeComponent<CGravity>();
    this->removeComponent<CState>();
    this->removeComponent<CDamage>();
    this->removeComponent<CInvincibility>();
    this->removeComponent<CHealth>();
    this->removeComponent<CFollowPlayer>();
    this->removeComponent<CPatrol>();
    
    // std::cout << "ENTITY " << this->tag << " (" << this->id << ") created\n";
}

Entity::Entity(const Entity & src)
{
    this->id = src.id;
    this->tag = src.tag;
}

Entity::~Entity()
{
    // std::cout << "ENTITY " << this->tag << " (" << this->id << ") destroyed\n";
}

const std::string & Entity::getTag() const
{
    return this->tag;
}

void Entity::kill()
{
    this->alive = false;
}

const bool Entity::isAlive() const
{
    return this->alive;
}
