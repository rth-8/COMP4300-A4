#include <iostream>
#include <fstream>
#include <cstdlib>

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
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Down), "PLAYERDOWN");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Left), "PLAYERLEFT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Right), "PLAYERRIGHT");

    registerAction(static_cast<int>(sf::Keyboard::Scancode::C), "TOGLEBB");

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
                    auto& a = e->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation(animName));
                    auto& bb = e->addComponent<CBoundingBox>(a.getAnimation()->getSize(), blockM, blockV);
                    e->addComponent<CTransform>(Vec2(
                        tileX + (roomX * this->windowW) + bb.halfSize.x, 
                        tileY + (roomY * this->windowH) + bb.halfSize.y)
                    );
                    // std::cout << "pos: " << e->getComponent<CTransform>().pos.x << "," << e->getComponent<CTransform>().pos.y << "\n";
                }
                else
                if (token == "Player")
                {
                    int bbX;
                    int bbY;
            
                    ss >> playerCfg.spawn_x >> playerCfg.spawn_y >> bbX >> bbY >> playerCfg.speed;
                    // create player entity
                    this->player = manager->addEntity("Player");
                    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingDown"));
                    this->player->addComponent<CTransform>(Vec2(playerCfg.spawn_x, playerCfg.spawn_y));
                    this->player->addComponent<CInput>();
                    this->player->addComponent<CBoundingBox>(Vec2(bbX, bbY), 1, 1);
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
        sMovement();
        sEnemySpawner();
        sCollision();
    }
    sRender();
    
    if (this->isPaused == false)
    {
        currentFrame++;
        this->engine->getWindow()->setTitle(
            std::format("({},{}), frame: {}", 
            this->player->getComponent<CTransform>().pos.x,
            this->player->getComponent<CTransform>().pos.y,
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

        e->getComponent<CAnimation>().getAnimation()->update(currentFrame);
    }
}

void ScenePlay::sMovement()
{
    for (auto e : this->manager->getEntities())
    {
        if (e->isAlive() == false)
            continue;

        auto& trans = e->getComponent<CTransform>();
        // auto& bb = e->getComponent<CBoundingBox>();
        
        // store previous position
        trans.prevPos = trans.pos;

        // apply speed
        trans.pos += trans.speed;
    }
    
    if (this->player->getComponent<CTransform>().pos.x < (this->view.getCenter().x - windowW2))
        this->view.setCenter(sf::Vector2f(this->view.getCenter().x - this->engine->getWindow()->getSize().x, this->view.getCenter().y));

    if (this->player->getComponent<CTransform>().pos.x > (this->view.getCenter().x + windowW2))
        this->view.setCenter(sf::Vector2f(this->view.getCenter().x + this->engine->getWindow()->getSize().x, this->view.getCenter().y));
    
    if (this->player->getComponent<CTransform>().pos.y < (this->view.getCenter().y - windowH2))
        this->view.setCenter(sf::Vector2f(this->view.getCenter().x, this->view.getCenter().y - this->engine->getWindow()->getSize().y));
    
    if (this->player->getComponent<CTransform>().pos.y > (this->view.getCenter().y + windowH2))
        this->view.setCenter(sf::Vector2f(this->view.getCenter().x, this->view.getCenter().y + this->engine->getWindow()->getSize().y));
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
    
    for (auto e : this->manager->getEntities("Tile"))
    {
        if (e->isAlive() == false)
            continue;

        if (e->getComponent<CBoundingBox>().blocksMovement == 1)
        {
            cVec.x = 0;
            cVec.y = 0;
            
            if (checkCollision(this->player, e, cVec))
            {
                // resolve collision for player entity
                this->player->getComponent<CTransform>().pos += cVec;
            }
        }
    }
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

        spr.setPosition({t.pos.x, t.pos.y});
        spr.setScale({t.scale.x, t.scale.y});
        w->draw(spr);
        
        if (this->isDrawingBB)
        {
            drawBB(*w, t, e->getComponent<CBoundingBox>());
            drawPoint(*w, t.pos.x, t.pos.y, 10);
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
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingUp"));
    this->player->getComponent<CTransform>().speed.y = -playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = 1;
}

void ScenePlay::startDown()
{
    this->player->getComponent<CInput>().down = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingDown"));
    this->player->getComponent<CTransform>().speed.y = playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = 1;
}

void ScenePlay::startLeft()
{
    this->player->getComponent<CInput>().left = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingHoriz"));
    this->player->getComponent<CTransform>().speed.x = -playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = -1;
}

void ScenePlay::startRight()
{
    this->player->getComponent<CInput>().right = true;
    this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerMovingHoriz"));
    this->player->getComponent<CTransform>().speed.x = playerCfg.speed;
    this->player->getComponent<CTransform>().scale.x = 1;
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
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startUp();
        }
        else
        if (action.name() == "PLAYERDOWN")
        {
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startDown();
        }
        else
        if (action.name() == "PLAYERLEFT")
        {
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startLeft();
        }
        else
        if (action.name() == "PLAYERRIGHT")
        {
            if (isPlayerMoving())
                waitingAction = action.name();
            else
                startRight();
        }
        else
        if (action.name() == "TOGLEBB")
        {
            this->isDrawingBB = !this->isDrawingBB;
        }
    }
    else
    if (action.type() == "END")
    {
        if (action.name() == "PLAYERUP")
        {
            if (waitingAction == action.name())
                waitingAction.clear();

            this->player->getComponent<CInput>().up = false;
            this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingUp"));
            this->player->getComponent<CTransform>().speed.y = 0;
            
            if (!waitingAction.empty())
            {
                startMoving(waitingAction);
                waitingAction.clear();
            }
        }
        else
        if (action.name() == "PLAYERDOWN")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            
            this->player->getComponent<CInput>().down = false;
            this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingDown"));
            this->player->getComponent<CTransform>().speed.y = 0;
            
            if (!waitingAction.empty())
            {
                startMoving(waitingAction);
                waitingAction.clear();
            }
        }
        else
        if (action.name() == "PLAYERLEFT")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            
            this->player->getComponent<CInput>().left = false;
            this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingHoriz"));
            this->player->getComponent<CTransform>().speed.x = 0;
            
            if (!waitingAction.empty())
            {
                startMoving(waitingAction);
                waitingAction.clear();
            }
        }
        else
        if (action.name() == "PLAYERRIGHT")
        {
            if (waitingAction == action.name())
                waitingAction.clear();
            
            this->player->getComponent<CInput>().right = false;
            this->player->addComponent<CAnimation>(&this->engine->getAssets()->getAnimation("PlayerStandingHoriz"));
            this->player->getComponent<CTransform>().speed.x = 0;
            
            if (!waitingAction.empty())
            {
                startMoving(waitingAction);
                waitingAction.clear();
            }
        }
    }
}

void ScenePlay::sDebug()
{
}
