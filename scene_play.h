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
        int speed;
    };
    
    sf::View view;
    
    std::string levelPath;
    
    std::shared_ptr<Entity> player;
    PlayerConfig playerCfg;
    
    void init();
    void load_level();
    
    bool isPaused = false; 
    bool isDrawingBB = false; 
    
public:
    ScenePlay() = delete;
    ScenePlay(GameEngine* eng, const std::string & lvlP);
    ~ScenePlay();
    
    void update();
    
    void sAnimation();
    void sMovement();
    void sEnemySpawner();
    void sCollision();
    void sRender();
    void sDoAction(const Action& action);
    void sDebug();
};

#endif
