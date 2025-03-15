#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#include "scene_play.h"
#include "game_engine.h"
#include "assets.h"
#include "entity.h"
#include "entity_manager.h"

ScenePlay::ScenePlay(GameEngine* eng, const std::string & lvlP)
    : Scene(eng)
    , levelPath(lvlP)
{
    // std::cout << "SCENE PLAY: c'tor\n";
    
    this->windowW = this->engine->getWindow()->getSize().x;
    this->windowW2 = this->engine->getWindow()->getSize().x / 2;
    this->windowH = this->engine->getWindow()->getSize().y;
    this->windowH2 = this->engine->getWindow()->getSize().y / 2;

    this->view = this->engine->getWindow()->getDefaultView();
    
    this->entrances.reserve(10);
    
    init();
}

ScenePlay::~ScenePlay()
{
    // std::cout << "SCENE PLAY: d'tor\n";
}

void ScenePlay::init()
{
    // register actions
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Escape), "RETURNTOMENU");

    registerAction(static_cast<int>(sf::Keyboard::Scancode::Up), "PLAYERUP");
    registerAction(6000, "PLAYERUP");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Down), "PLAYERDOWN");
    registerAction(6001, "PLAYERDOWN");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Left), "PLAYERLEFT");
    registerAction(6002, "PLAYERLEFT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Right), "PLAYERRIGHT");
    registerAction(6003, "PLAYERRIGHT");
    
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Space), "PLAYERATTACK");
    registerAction(5002, "PLAYERATTACK");

    registerAction(static_cast<int>(sf::Keyboard::Scancode::C), "TOGLEBB");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::T), "TOGLETEX");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Y), "TOGLEFOLLOW");

    load_level();
}

void ScenePlay::load_level()
{
    std::cout << "Loading level: \"" << levelPath << "\"\n";

    std::fstream input;
    input.open(this->levelPath, std::ios::in);
    if (input.is_open())
    {
        std::string line;
        while (std::getline(input, line))
        {
            // std::cout << "LINE: " << line << '\n';

            if (line[0] == '#')
                continue;

            std::stringstream ss(line);
            std::string token;
            
            if (std::getline(ss, token, ' '))
            {
                if (token == "Tile")
                {
                    std::string animName;
                    int roomX, roomY;
                    int tileX, tileY;
                    int blockM, blockV;
                    
                    ss >> animName >> roomX >> roomY >> tileX >> tileY >> blockM >> blockV;
                    // std::cout << "TILE: " << animName << ", [" << roomX << "," << roomY << "], [" << tileX << "," << tileY << "], " << blockM << "," << blockV << "\n";
                    
                    auto e = manager->addEntity("Tile");
                    auto& a = e->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation(animName), true);
                    auto& bb = e->addComponent<CBoundingBox>(a.getAnimation()->getSize(), blockM, blockV);
                    e->addComponent<CTransform>(Vec2(
                        tileX + (roomX * this->windowW) + bb.halfSize.x, 
                        tileY + (roomY * this->windowH) + bb.halfSize.y)
                    );
                    // std::cout << "pos: " << e->getComponent<CTransform>().pos.x << "," << e->getComponent<CTransform>().pos.y << "\n";
                    
                    if (animName == "Entrance")
                    {
                        this->entrances.emplace_back(Vec2(e->getComponent<CTransform>().pos));
                    }
                }
                else
                if (token == "Player")
                {
                    int bbX;
                    int bbY;
                    int maxHealth;
            
                    ss >> playerCfg.spawn_x >> playerCfg.spawn_y >> bbX >> bbY >> playerCfg.speed >> maxHealth;
                    
                    // create player entity
                    this->player = manager->addEntity("Player");
                    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingDown"), true);
                    this->player->addComponent<CTransform>(Vec2(playerCfg.spawn_x, playerCfg.spawn_y));
                    this->player->addComponent<CInput>();
                    this->player->addComponent<CBoundingBox>(Vec2(bbX, bbY), 1, 1);
                    this->player->addComponent<CHealth>(maxHealth, maxHealth);
                    this->player->addComponent<CState>("facingdown");
                }
                else
                if (token == "NPC")
                {
                    std::string animName;
                    int roomX, roomY;
                    int tileX, tileY;
                    int blockM, blockV;
                    int maxHealth, damage;
                    std::string aiType;
                    float speed;
            
                    ss >> animName >> roomX >> roomY >> tileX >> tileY >> blockM >> blockV >> maxHealth >> damage >> aiType;
                    // std::cout << "NPC: " << animName 
                              // << ", [" << roomX << "," << roomY << "], [" << tileX << "," << tileY << "], " 
                              // << blockM << ", " << blockV << ", "
                              // << maxHealth << ", " << damage << ", " << aiType << "\n";
                    
                    // create npc entity
                    auto e = manager->addEntity("NPC");
                    auto& a = e->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation(animName), true);
                    auto& bb = e->addComponent<CBoundingBox>(a.getAnimation()->getSize(), blockM, blockV);
                    auto& t = e->addComponent<CTransform>(Vec2(
                        tileX + (roomX * this->windowW) + bb.halfSize.x, 
                        tileY + (roomY * this->windowH) + bb.halfSize.y)
                    );
                    e->addComponent<CHealth>(maxHealth, maxHealth);
                    e->addComponent<CDamage>(damage);
                    
                    if (aiType == "Follow")
                    {
                        ss >> speed;
                        // std::cout << "- add CFollowPlayer: speeed = " << speed << "\n";
                        e->addComponent<CFollowPlayer>(t.pos, speed);
                    }
                    else
                    if (aiType == "Patrol")
                    {
                        int cnt;
                        float x, y;
                        
                        ss >> speed >> cnt;
                        
                        auto& patrol = e->addComponent<CPatrol>(speed);
                        for (int i=0; i<cnt; ++i)
                        {
                            ss >> x >> y;
                            patrol.addPosition(x + bb.halfSize.x, y + bb.halfSize.y);
                        }
                    }
                }
            }
        }

        input.close();
    }
}

void ScenePlay::update()
{
    if (this->isPaused == false) 
    {
        this->manager->update();
        sAnimation();
        sAi();
        sMovement();
        sEnemySpawner();
        sCollision();
    }
    sRender();
    
    if (this->isPaused == false)
    {
        currentFrame++;
        
        auto& t = this->player->getComponent<CTransform>();
        int roomX, roomY;
        if (t.pos.x < 0)
            roomX = (int)((t.pos.x / windowW) - 1);
        else
            roomX = (int)(t.pos.x / windowW);
        if (t.pos.y < 0)
            roomY = (int)((t.pos.y / windowH) - 1);
        else
            roomY = (int)(t.pos.y / windowH);
        
        this->engine->getWindow()->setTitle(
            std::format("({},{}) ~ [{},{}] ~ frame: {}", 
            this->player->getComponent<CTransform>().pos.x,
            this->player->getComponent<CTransform>().pos.y,
            roomX,
            roomY,
            currentFrame));
    }
}

void ScenePlay::sAnimation()
{
    // update animations
    for (auto e : this->manager->getEntities())
    {
        if (e->isAlive() == false)
            continue;

        auto& ac = e->getComponent<CAnimation>();
        auto* anim = ac.getAnimation();
        
        if (anim->getName() == "Explosion")
            anim->update(ac.frame++);
        else
            anim->update(currentFrame);
    }
}

// static Vec2 checkIntersection(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
// {
    // Vec2 v = b - a;
    // Vec2 u = d - c;
    
    // Vec2 c_a = c - a;
    // Vec2 a_c = a - c;
    
    // float r = c_a.crossProd(u) / v.crossProd(u);
    // float s = a_c.crossProd(v) / u.crossProd(v);
    
    // if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
        // return Vec2(a.x + r*v.x, a.y + r*v.y);
    // else
        // return Vec2(0, 0);
// }

// static bool checkIntersection(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
// {
    // Vec2 v = b - a;
    // Vec2 u = d - c;
    
    // Vec2 c_a = c - a;
    // Vec2 a_c = a - c;
    
    // float r = c_a.crossProd(u) / v.crossProd(u);
    // float s = a_c.crossProd(v) / u.crossProd(v);
    
    // if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
        // return true;
    // else
        // return false;
// }

static bool checkIntersection(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
    Vec2 v = b - a;
    Vec2 u = d - c;
    
    Vec2 c_a = c - a;
    float vxu = v.crossProd(u);
    
    float r = c_a.crossProd(u) / vxu;
    float s = c_a.crossProd(v) / vxu;
    
    if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
        return true;
    else
        return false;
}

#define RANGE 2.0
bool static reachedPosition(const Vec2& p1, const Vec2& p2)
{
    if (p1.x >= p2.x - RANGE && p1.x <= p2.x + RANGE &&
        p1.y >= p2.y - RANGE && p1.y <= p2.y + RANGE)
        return true;
    else
        return false;
}

void ScenePlay::sAi()
{
    auto& playerTrans = this->player->getComponent<CTransform>();
    
    for (auto e : this->manager->getEntities("NPC"))
    {
        if (e->isAlive() == false)
            continue;

        auto& npcTrans = e->getComponent<CTransform>();
        
        if (e->hasComponent<CFollowPlayer>())
        {
            bool hasVision = true;
            
            for (auto e2 : this->manager->getEntities())
            {
                if (e == e2)
                    continue;
                
                if (e2->getTag() == "Player")
                    continue;
                
                auto& e2Trans = e2->getComponent<CTransform>();
                auto& e2BB = e2->getComponent<CBoundingBox>();
                
                // if entity does not block vision, it is not needed to calculate intersections
                if (e2BB.blocksVision == 0)
                    continue;
                
                // check intersections of line from player to npc with edges of another entity (tile, another npc)
                
                bool top = checkIntersection(playerTrans.pos, npcTrans.pos, 
                            Vec2(e2Trans.pos.x - e2BB.halfSize.x, e2Trans.pos.y - e2BB.halfSize.y),
                            Vec2(e2Trans.pos.x + e2BB.halfSize.x, e2Trans.pos.y - e2BB.halfSize.y));
                
                bool bottom = checkIntersection(playerTrans.pos, npcTrans.pos, 
                            Vec2(e2Trans.pos.x - e2BB.halfSize.x, e2Trans.pos.y + e2BB.halfSize.y),
                            Vec2(e2Trans.pos.x + e2BB.halfSize.x, e2Trans.pos.y + e2BB.halfSize.y));
                            
                bool left = checkIntersection(playerTrans.pos, npcTrans.pos, 
                            Vec2(e2Trans.pos.x - e2BB.halfSize.x, e2Trans.pos.y - e2BB.halfSize.y),
                            Vec2(e2Trans.pos.x - e2BB.halfSize.x, e2Trans.pos.y + e2BB.halfSize.y));
                            
                bool right = checkIntersection(playerTrans.pos, npcTrans.pos, 
                            Vec2(e2Trans.pos.x + e2BB.halfSize.x, e2Trans.pos.y - e2BB.halfSize.y),
                            Vec2(e2Trans.pos.x + e2BB.halfSize.x, e2Trans.pos.y + e2BB.halfSize.y));
                            
                // if there is intersection with at least one edge, then line of sight is broken 
                if (top || bottom || left || right)
                {
                    hasVision = false;
                    break;
                }
            } // end for
            
            if (hasVision)
            {
                // if there is line of sight, then follow player
                npcTrans.speed = playerTrans.pos - npcTrans.pos;
                npcTrans.speed.normalize().mul(e->getComponent<CFollowPlayer>().speed);
            }
            else
            {
                // if line of sight is broken, then try to return to home position 
                npcTrans.speed = e->getComponent<CFollowPlayer>().home - npcTrans.pos;
                npcTrans.speed.normalize().mul(e->getComponent<CFollowPlayer>().speed);
                
                // if home position is reached (+-), then stop movement
                if (reachedPosition(npcTrans.pos, e->getComponent<CFollowPlayer>().home))
                {
                    npcTrans.pos.x = e->getComponent<CFollowPlayer>().home.x;
                    npcTrans.pos.y = e->getComponent<CFollowPlayer>().home.y;
                    npcTrans.speed.x = 0;
                    npcTrans.speed.y = 0;
                }
            }
        }
        
        if (e->hasComponent<CPatrol>())
        {
            auto& patrol = e->getComponent<CPatrol>();
            
            if (reachedPosition(npcTrans.pos, patrol.positions[patrol.currentPos]))
            {
                npcTrans.pos.x = patrol.positions[patrol.currentPos].x;
                npcTrans.pos.y = patrol.positions[patrol.currentPos].y;
                
                patrol.currentPos = (patrol.currentPos + 1) % patrol.positions.size();
            
                npcTrans.speed = patrol.positions[patrol.currentPos] - npcTrans.pos;
                npcTrans.speed.normalize().mul(patrol.speed);
            }
        }
    }
}

void ScenePlay::sMovement()
{
    for (auto e : this->manager->getEntities())
    {
        if (e->isAlive() == false)
            continue;
        
        if (e->hasComponent<CLifeSpan>())
        {
            if (e->getComponent<CLifeSpan>().frameCreated + e->getComponent<CLifeSpan>().lifespan == currentFrame)
            {
                e->kill();
                
                if (e == this->sword)
                {
                    this->sword = nullptr;
                    if (this->player->getComponent<CState>().state == "attacking")
                    {
                        this->player->getComponent<CState>().state = this->player->getComponent<CState>().prev;
                        this->player->getComponent<CState>().prev = "none";
                        
                        if (!waitingAction.empty())
                        {
                            startMoving(waitingAction);
                            waitingAction.clear();
                        }
                    }
                }
            }
        }
        
        if (e->hasComponent<CInvincibility>())
        {
            e->getComponent<CInvincibility>().iFrames--;
            if (e->getComponent<CInvincibility>().iFrames == 0)
                e->removeComponent<CInvincibility>();
        }
        
        if (e == this->player)
        {
            if (this->player->getComponent<CState>().state == "attacking")
                continue;
        }
        
        auto& trans = e->getComponent<CTransform>();
        
        // store previous position
        trans.prevPos = trans.pos;

        // apply speed
        trans.pos += trans.speed;
    }
    
    if (this->followMode)
    {
        this->view.setCenter(sf::Vector2f(this->player->getComponent<CTransform>().pos.x, this->player->getComponent<CTransform>().pos.y));
    }
    else
    {
        // check, if player goes out of current view
        // if so, then move view to new position
        
        if (this->player->getComponent<CTransform>().pos.x < (this->view.getCenter().x - windowW2))
            this->view.setCenter(sf::Vector2f(this->view.getCenter().x - this->engine->getWindow()->getSize().x, this->view.getCenter().y));

        if (this->player->getComponent<CTransform>().pos.x > (this->view.getCenter().x + windowW2))
            this->view.setCenter(sf::Vector2f(this->view.getCenter().x + this->engine->getWindow()->getSize().x, this->view.getCenter().y));
        
        if (this->player->getComponent<CTransform>().pos.y < (this->view.getCenter().y - windowH2))
            this->view.setCenter(sf::Vector2f(this->view.getCenter().x, this->view.getCenter().y - this->engine->getWindow()->getSize().y));
        
        if (this->player->getComponent<CTransform>().pos.y > (this->view.getCenter().y + windowH2))
            this->view.setCenter(sf::Vector2f(this->view.getCenter().x, this->view.getCenter().y + this->engine->getWindow()->getSize().y));
    }
}

void ScenePlay::sEnemySpawner()
{
}

static bool checkCollision(std::shared_ptr<Entity> moving, std::shared_ptr<Entity> still, Vec2& vec)
{
    auto& movingTrans = moving->getComponent<CTransform>();
    auto& movingBB = moving->getComponent<CBoundingBox>();
    
    auto& stillTrans = still->getComponent<CTransform>();
    auto& stillBB = still->getComponent<CBoundingBox>();
    
    float pox = movingBB.halfSize.x + stillBB.halfSize.x - abs(movingTrans.prevPos.x - stillTrans.pos.x);
    float poy = movingBB.halfSize.y + stillBB.halfSize.y - abs(movingTrans.prevPos.y - stillTrans.pos.y);

    float ox = movingBB.halfSize.x + stillBB.halfSize.x - abs(movingTrans.pos.x - stillTrans.pos.x);
    float oy = movingBB.halfSize.y + stillBB.halfSize.y - abs(movingTrans.pos.y - stillTrans.pos.y);

    short state = 0;
    if (pox > 0 && poy <= 0) state = 1;
    else
    if (pox <= 0 && poy > 0) state = 2;
    else
    if (pox > 0 && poy > 0) state = 3;

    if (ox > 0 && oy > 0)
    {        
        if (state == 1)
        {
            // vertical            
            if (movingTrans.pos.y < stillTrans.pos.y) vec.y = -oy;
            if (movingTrans.pos.y > stillTrans.pos.y) vec.y = oy;
        }
        else
        if (state == 2 || state == 3)
        {
            // horizontal
            if (movingTrans.pos.x < stillTrans.pos.x) vec.x = -ox;
            if (movingTrans.pos.x > stillTrans.pos.x) vec.x = ox;
        }
        
        return true;
    }
    
    return false;
}

void ScenePlay::sCollision()
{
    Vec2 cVec;
    
    for (auto tile : this->manager->getEntities("Tile"))
    {
        if (tile->isAlive() == false)
            continue;

        auto& animName = tile->getComponent<CAnimation>().getAnimation()->getName();
        
        if (tile->getComponent<CBoundingBox>().blocksMovement == 1)
        {
            cVec.x = 0;
            cVec.y = 0;
            
            // collisions with player entity
            if (checkCollision(this->player, tile, cVec))
            {
                // std::cout << "Player colliding with " << animName << "\n";
                
                if (animName == "Entrance")
                {
                    while (true)
                    {
                        auto& newPos = this->entrances[ rand() % this->entrances.size() ];
                        if (newPos == tile->getComponent<CTransform>().pos)
                            continue;
                        
                        this->player->getComponent<CTransform>().pos = newPos;
                        this->player->getComponent<CTransform>().pos.y += tile->getComponent<CBoundingBox>().size.y;
                        this->player->getComponent<CTransform>().speed.x = 0;
                        this->player->getComponent<CTransform>().speed.y = 0;
                        break;
                    }
                }
                else
                // player pick heart
                if (animName == "Heart")
                {
                    this->engine->getAssets()->getSound("PickHeartSound").play();
                    
                    tile->kill();
                    if (this->player->getComponent<CHealth>().current < this->player->getComponent<CHealth>().max)
                        this->player->getComponent<CHealth>().current += 1;
                }
                // resolve collision for player entity
                else
                    this->player->getComponent<CTransform>().pos += cVec;
            }
            
            // collisions with npc entities
            for (auto npc : this->manager->getEntities("NPC"))
            {
                cVec.x = 0;
                cVec.y = 0;
                
                if (checkCollision(npc, tile, cVec))
                {
                    // npc picks heart
                    if (animName == "Heart")
                    {
                        this->engine->getAssets()->getSound("PickHeartSound").play();
                        
                        tile->kill();
                        if (npc->getComponent<CHealth>().current < npc->getComponent<CHealth>().max)
                            npc->getComponent<CHealth>().current += 1; 
                    }
                    // resolve collision for npc entity
                    else
                        npc->getComponent<CTransform>().pos += cVec;
                }
            }
        }
    }

    for (auto npc : this->manager->getEntities("NPC"))
    {
        if (this->player->hasComponent<CInvincibility>() == false)
        {
            if (checkCollision(npc, this->player, cVec))
            {
                // npc hurts player
                // std::cout << "DMG: " << npc->getComponent<CDamage>().damage << "\n";
                this->player->getComponent<CHealth>().current -= npc->getComponent<CDamage>().damage;
                
                if (this->player->getComponent<CHealth>().current == 0)
                {
                    // player is dead
                    // TODO
                }
                else
                {
                    this->player->addComponent<CInvincibility>(90);
                }
            }
        }
        
        if (this->sword != nullptr && this->sword->hasComponent<CDamage>())
        {
            if (checkCollision(npc, this->sword, cVec))
            {
                // sword hits npc
                this->engine->getAssets()->getSound("HurtSound").play();
                npc->getComponent<CHealth>().current -= this->sword->getComponent<CDamage>().damage;
                this->sword->removeComponent<CDamage>();
                
                if (npc->getComponent<CHealth>().current == 0)
                {
                    // if npc has no health left, then kill it
                    npc->kill();
                    
                    // spawn explosion at same position
                    auto explosion = manager->addEntity("Explosion");
                    explosion->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("Explosion"), false);
                    explosion->addComponent<CTransform>(Vec2(npc->getComponent<CTransform>().pos.x, npc->getComponent<CTransform>().pos.y));
                    explosion->addComponent<CLifeSpan>(explosion->getComponent<CAnimation>().getAnimation()->getDuration(), currentFrame);
                }
            }
        }
    }
}

static void drawLine(sf::RenderWindow & window, float x1, float y1, float x2, float y2)
{
    std::array line =
    {
        sf::Vertex{sf::Vector2f(x1, y1), sf::Color::Red},
        sf::Vertex{sf::Vector2f(x2, y2), sf::Color::Red}
    };

    window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
}

static void drawPoint(sf::RenderWindow & window, float x, float y, int size)
{
    std::array line1 =
    {
        sf::Vertex{sf::Vector2f(x-size, y), sf::Color::Red},
        sf::Vertex{sf::Vector2f(x+size, y), sf::Color::Red}
    };

    std::array line2 =
    {
        sf::Vertex{sf::Vector2f(x, y-size), sf::Color::Red},
        sf::Vertex{sf::Vector2f(x, y+size), sf::Color::Red}
    };

    window.draw(line1.data(), line1.size(), sf::PrimitiveType::Lines);
    window.draw(line2.data(), line2.size(), sf::PrimitiveType::Lines);
}

static void drawDot(sf::RenderWindow & window, float x, float y, int size)
{
    sf::CircleShape circle(size, 4);
    circle.setPosition(sf::Vector2f(x - size, y - size));
    circle.setFillColor(sf::Color(0, 0, 0, 255));
    window.draw(circle);
}

static void drawBB(sf::RenderWindow & window, const CTransform& trans, const CBoundingBox& bb)
{
    sf::RectangleShape rectangle({bb.size.x, bb.size.y});
    rectangle.setPosition(sf::Vector2f(trans.pos.x - bb.halfSize.x, trans.pos.y - bb.halfSize.y));
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));
    rectangle.setOutlineThickness(-1);
    if (bb.blocksMovement == 1 && bb.blocksVision == 1)
        rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));
    else
    if (bb.blocksMovement == 1 && bb.blocksVision == 0)
        rectangle.setOutlineColor(sf::Color(0, 0, 255, 255));
    else
    if (bb.blocksMovement == 0 && bb.blocksVision == 1)
        rectangle.setOutlineColor(sf::Color(255, 0, 0, 255));
    window.draw(rectangle);
}

static void drawHealth(sf::RenderWindow & window, const CTransform& trans, const Vec2& size, const CHealth& health)
{
    float w = 66;
    float x = trans.pos.x - 32;
    float y = trans.pos.y - size.y;
    
    sf::RectangleShape rectangle({w, 8});
    rectangle.setPosition(sf::Vector2f(x, y));
    rectangle.setFillColor(sf::Color(20, 20, 20, 255));
    window.draw(rectangle);
    
    int margin = 3;
    float hbar = (w - (margin * (health.max + 1))) / health.max;
    x += margin;
    for (int i=0; i<health.current; ++i)
    {
        sf::RectangleShape rectangle({hbar, 4});
        rectangle.setPosition(sf::Vector2f(x, y + 2));
        rectangle.setFillColor(sf::Color(255, 0, 0, 255));
        window.draw(rectangle);
        
        x += hbar + margin;
    }
}

void ScenePlay::sRender()
{
    auto w = this->engine->getWindow();
    w->clear(sf::Color(252,216,168));
    
    w->setView(this->view);

    for (auto e : this->manager->getEntities())
    {
        if (e->isAlive() == false)
            continue;
        
        auto& spr = e->getComponent<CAnimation>().getAnimation()->getSprite();
        auto& t = e->getComponent<CTransform>();
        
        if (this->isDrawingTex)
        {
            if (e->hasComponent<CInvincibility>())
            {
                if (e->getComponent<CInvincibility>().iFrames % 6 == 0)
                {
                    spr.setPosition({t.pos.x, t.pos.y});
                    spr.setScale({t.scale.x, t.scale.y});
                    w->draw(spr);
                }
            }
            else
            {
                spr.setPosition({t.pos.x, t.pos.y});
                spr.setScale({t.scale.x, t.scale.y});
                w->draw(spr);
            }
        }
        
        if (e->hasComponent<CHealth>())
        {
            drawHealth(*w, t, e->getComponent<CAnimation>().getAnimation()->getSize(), e->getComponent<CHealth>());
        }
        
        if (this->isDrawingBB)
        {
            drawBB(*w, t, e->getComponent<CBoundingBox>());
            drawPoint(*w, t.pos.x, t.pos.y, 12);
            
            if (e->hasComponent<CFollowPlayer>())
            {
                drawLine(*w,
                         e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y,
                         this->player->getComponent<CTransform>().pos.x, this->player->getComponent<CTransform>().pos.y
                );
            }
            
            if (e->hasComponent<CPatrol>())
            {
                for (auto& p : e->getComponent<CPatrol>().positions)
                    drawDot(*w, p.x, p.y, 5);
            }
        }
    }
}

void ScenePlay::startMoving(std::string name)
{
    if (name == "PLAYERUP") startUp();
    else if (name == "PLAYERDOWN") startDown();
    else if (name == "PLAYERLEFT") startLeft();
    else if (name == "PLAYERRIGHT") startRight();
}

void ScenePlay::startUp()
{
    this->player->getComponent<CInput>().up = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingUp"), true);
    this->player->getComponent<CTransform>().speed.y = -playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = 1;
    this->player->getComponent<CState>().state = "facingup";
}

void ScenePlay::startDown()
{
    this->player->getComponent<CInput>().down = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingDown"), true);
    this->player->getComponent<CTransform>().speed.y = playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = 1;
    this->player->getComponent<CState>().state = "facingdown";
}

void ScenePlay::startLeft()
{
    this->player->getComponent<CInput>().left = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingHoriz"), true);
    this->player->getComponent<CTransform>().speed.x = -playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = -1;
    this->player->getComponent<CState>().state = "facingleft";
}

void ScenePlay::startRight()
{
    this->player->getComponent<CInput>().right = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingHoriz"), true);
    this->player->getComponent<CTransform>().speed.x = playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = 1;
    this->player->getComponent<CState>().state = "facingright";
}

bool ScenePlay::isPlayerMoving()
{
    if (this->player->getComponent<CInput>().up == true ||
        this->player->getComponent<CInput>().down == true ||
        this->player->getComponent<CInput>().left == true ||
        this->player->getComponent<CInput>().right == true)
        return true;
    else
        return false;
}

void ScenePlay::sDoAction(const Action& action)
{
    std::cout << "SCENE PLAY: do action: " << action.name() << " (" << action.type() << ")\n";
    std::cout << "waiting: " << waitingAction << "\n";

    if (action.type() == "START")
    {
        if (action.name() == "RETURNTOMENU")
        {
            this->engine->changeScene("Menu");
        }
        else
        if (action.name() == "PLAYERUP")
        {
            if (this->player->getComponent<CState>().state == "attacking")
                waitingAction = action.name();
            else
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startUp();
        }
        else
        if (action.name() == "PLAYERDOWN")
        {
            if (this->player->getComponent<CState>().state == "attacking")
                waitingAction = action.name();
            else
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startDown();
        }
        else
        if (action.name() == "PLAYERLEFT")
        {
            if (this->player->getComponent<CState>().state == "attacking")
                waitingAction = action.name();
            else
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startLeft();
        }
        else
        if (action.name() == "PLAYERRIGHT")
        {
            if (this->player->getComponent<CState>().state == "attacking")
                waitingAction = action.name();
            else
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startRight();
        }
        else
        if (action.name() == "PLAYERATTACK")
        {
            if (this->player->getComponent<CState>().state != "attacking" && this->sword == nullptr)
            {
                auto& ps = this->player->getComponent<CState>();
                auto& pt = this->player->getComponent<CTransform>();
                auto& pb = this->player->getComponent<CBoundingBox>();
                
                ps.prev = ps.state;
                ps.state = "attacking";
                
                this->sword = manager->addEntity("Sword");
                if (ps.prev == "facingup")
                {
                    // attacking to up
                    this->sword->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("SwordDown"), true);
                    auto& bb = this->sword->addComponent<CBoundingBox>(this->sword->getComponent<CAnimation>().getAnimation()->getSize(), 0, 0);
                    auto& t = this->sword->addComponent<CTransform>(Vec2(pt.pos.x, pt.pos.y - pb.halfSize.y - bb.halfSize.y));
                    t.scale.x = 1;
                    t.scale.y = -1;
                }
                else
                if (ps.prev == "facingdown")
                {
                    // attacking to up
                    this->sword->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("SwordDown"), true);
                    auto& bb = this->sword->addComponent<CBoundingBox>(this->sword->getComponent<CAnimation>().getAnimation()->getSize(), 0, 0);
                    auto& t = this->sword->addComponent<CTransform>(Vec2(pt.pos.x, pt.pos.y + pb.halfSize.y + bb.halfSize.y));
                    t.scale.x = 1;
                    t.scale.y = 1;
                }
                else
                if (ps.prev == "facingleft")
                {
                    // attacking to left
                    this->sword->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("SwordRight"), true);
                    auto& bb = this->sword->addComponent<CBoundingBox>(this->sword->getComponent<CAnimation>().getAnimation()->getSize(), 0, 0);
                    auto& t = this->sword->addComponent<CTransform>(Vec2(pt.pos.x - pb.halfSize.x - bb.halfSize.x, pt.pos.y));
                    t.scale.x = -1;
                    t.scale.y = 1;
                }
                else
                if (ps.prev == "facingright")
                {
                    // attacking to right
                    this->sword->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("SwordRight"), true);
                    auto& bb = this->sword->addComponent<CBoundingBox>(this->sword->getComponent<CAnimation>().getAnimation()->getSize(), 0, 0);
                    auto& t = this->sword->addComponent<CTransform>(Vec2(pt.pos.x + pb.halfSize.x + bb.halfSize.x, pt.pos.y));
                    t.scale.x = 1;
                    t.scale.y = 1;
                }
                
                this->sword->addComponent<CDamage>(1);
                this->sword->addComponent<CLifeSpan>(10, currentFrame);
                
                this->engine->getAssets()->getSound("SwordSlashSound").play();
            }
        }
        else
        if (action.name() == "TOGLEBB")
        {
            this->isDrawingBB = !this->isDrawingBB;
        }
        else
        if (action.name() == "TOGLETEX")
        {
            this->isDrawingTex = !this->isDrawingTex;
        }
        else
        if (action.name() == "TOGLEFOLLOW")
        {
            this->followMode = !this->followMode;
            if (this->followMode == false)
            {
                auto& t = this->player->getComponent<CTransform>();
                int roomX, roomY;
                if (t.pos.x < 0)
                    roomX = (int)((t.pos.x / windowW) - 1);
                else
                    roomX = (int)(t.pos.x / windowW);
                if (t.pos.y < 0)
                    roomY = (int)((t.pos.y / windowH) - 1);
                else
                    roomY = (int)(t.pos.y / windowH);
                
                this->view.setCenter(sf::Vector2f(windowW2 + (windowW * roomX), windowH2 + (windowH * roomY)));
            }
        }
    }
    else
    if (action.type() == "END")
    {
        if (action.name() == "PLAYERUP")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            else
            {
                this->player->getComponent<CInput>().up = false;
                this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingUp"), true);
                this->player->getComponent<CTransform>().speed.y = 0;
            }
            
            if (this->player->getComponent<CState>().state != "attacking")
            {
                if (!waitingAction.empty())
                {
                    startMoving(waitingAction);
                    waitingAction.clear();
                }
            }
        }
        else
        if (action.name() == "PLAYERDOWN")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            else
            {
                this->player->getComponent<CInput>().down = false;
                this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingDown"), true);
                this->player->getComponent<CTransform>().speed.y = 0;
            }
            
            if (this->player->getComponent<CState>().state != "attacking")
            {
                if (!waitingAction.empty())
                {
                    startMoving(waitingAction);
                    waitingAction.clear();
                }
            }
        }
        else
        if (action.name() == "PLAYERLEFT")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            else
            {
                this->player->getComponent<CInput>().left = false;
                this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingHoriz"), true);
                this->player->getComponent<CTransform>().speed.x = 0;
            }
            
            if (this->player->getComponent<CState>().state != "attacking")
            {
                if (!waitingAction.empty())
                {
                    startMoving(waitingAction);
                    waitingAction.clear();
                }
            }
        }
        else
        if (action.name() == "PLAYERRIGHT")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            else
            {
                this->player->getComponent<CInput>().right = false;
                this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingHoriz"), true);
                this->player->getComponent<CTransform>().speed.x = 0;
            }
            
            if (this->player->getComponent<CState>().state != "attacking")
            {
                if (!waitingAction.empty())
                {
                    startMoving(waitingAction);
                    waitingAction.clear();
                }
            }
        }
    }
}

void ScenePlay::sDebug()
{
}
