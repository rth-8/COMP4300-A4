#ifndef SCENE_PLAY_H
#define SCENE_PLAY_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Text.hpp>

#include "scene.h"

class Entity;

class ScenePlay : public Scene
{
    struct PlayerConfig
    {
        int spawn_x;
        int spawn_y;
        float speed;
    };
    
    sf::View view;
    int windowW;
    int windowW2;
    int windowH;
    int windowH2;
    
    std::string levelPath;
    
    std::shared_ptr<Entity> player;
    PlayerConfig playerCfg;
    
    std::shared_ptr<Entity> sword = nullptr;
    
    void init();
    void load_level();
    
    bool isPaused = false; 
    bool isDrawingBB = false; 
    
    std::string waitingAction;
    void startMoving(std::string name);
    void startUp();
    void startDown();
    void startLeft();
    void startRight();
    bool isPlayerMoving();
    
public:
    ScenePlay() = delete;
    ScenePlay(GameEngine* eng, const std::string & lvlP);
    ~ScenePlay();
    
    void update();
    
    void sAnimation();
    void sAi();
    void sMovement();
    void sEnemySpawner();
    void sCollision();
    void sRender();
    void sDoAction(const Action& action);
    void sDebug();
};

#endif
