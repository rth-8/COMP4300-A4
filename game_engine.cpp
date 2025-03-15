#include <iostream>
#include <fstream>

#include "game_engine.h"
#include "animation.h"
#include "assets.h"
#include "scene_menu.h"
#include "scene_play.h"
#include "action.h"

GameEngine::GameEngine(std::shared_ptr<sf::RenderWindow> win)
    : window(win)
{
    assets = std::make_shared<Assets>();
    init();
}

void GameEngine::init()
{
    std::fstream input;

    input.open("data/assets.dat", std::ios::in);
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
            
            std::string sval1;
            std::string sval2;
            int ival1;
            int ival2;
            
            if (std::getline(ss, token, ' '))
            {
                if (token == "Texture")
                {
                    ss >> sval1 >> sval2;
                    assets->addTexture(sval1, sval2);
                }
                else
                if (token == "Animation")
                {
                    ss >> sval1 >> sval2 >> ival1 >> ival2;
                    assets->addAnimation(sval1, sval2, ival1, ival2);
                }
                else
                if (token == "Font")
                {
                    ss >> sval1 >> sval2;
                    assets->addFont(sval1, sval2);
                }
                else
                if (token == "Sound")
                {
                    ss >> sval1 >> sval2;
                    assets->addSound(sval1, sval2);
                }
            }
        }
        
        input.close();
    }
    
    this->addScene("Menu", std::make_shared<SceneMenu>(this));
    this->changeScene("Menu");
}

std::shared_ptr<sf::RenderWindow> GameEngine::getWindow()
{
    return window;
}

std::shared_ptr<Assets> GameEngine::getAssets()
{
    return assets;
}

void GameEngine::addScene(const std::string& sceneId, std::shared_ptr<Scene> scene)
{
    this->scenes[sceneId] = scene;
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return scenes[currentSceneId];
}

void GameEngine::changeScene(const std::string& sceneId)
{
    // std::cout << "ENGINE: change scene: " << sceneId << "\n";
    currentSceneId = sceneId;
}

void GameEngine::run()
{
    running = true;
    
    while (running)
    {        
        if (running)
        {
            window->clear(sf::Color::Black);
            this->sUserInput();
            this->update();
            window->display();
        }
    }
}

void GameEngine::update()
{
    this->currentScene()->update();
}

void GameEngine::quit()
{
    // std::cout << "ENGINE: quit\n";
    running = false;
}

#define DEADZONE 50.0f
#define JU 6000
#define JD 6001
#define JL 6002
#define JR 6003

static bool joyUp = false;
static bool joyDown = false;
static bool joyLeft = false;
static bool joyRight = false;

void GameEngine::sUserInput()
{
    while (const std::optional event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            quit();
        }
        
        const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
        const auto* keyReleased = event->getIf<sf::Event::KeyReleased>();
        
        if (keyPressed)
        {
            int scancode = static_cast<int>(keyPressed->scancode);
            if (currentScene()->actionExists(scancode))
            {
                currentScene()->sDoAction(Action(currentScene()->getActionMap().at(scancode), "START"));
            }
        }
        
        if (keyReleased)
        {
            int scancode = static_cast<int>(keyReleased->scancode);
            if (currentScene()->actionExists(scancode))
            {
                currentScene()->sDoAction(Action(currentScene()->getActionMap().at(scancode), "END"));
            }
        }
        
        const auto* joystickButtonPressed = event->getIf<sf::Event::JoystickButtonPressed>();
        const auto* joystickButtonReleased = event->getIf<sf::Event::JoystickButtonReleased>();
        
        if (joystickButtonPressed)
        {
            int btn = static_cast<int>(joystickButtonPressed->button) + 5000;
            if (currentScene()->actionExists(btn))
            {
                currentScene()->sDoAction(Action(currentScene()->getActionMap().at(btn), "START"));
            }
        }
        
        if (joystickButtonReleased)
        {
            int btn = static_cast<int>(joystickButtonReleased->button) + 5000;
            if (currentScene()->actionExists(btn))
            {
                currentScene()->sDoAction(Action(currentScene()->getActionMap().at(btn), "END"));
            }
        }
        
        if (const auto* joystickMoved = event->getIf<sf::Event::JoystickMoved>())
        {
            if (joystickMoved->axis == sf::Joystick::Axis::X)
            {
                if (joystickMoved->position <= -DEADZONE || joystickMoved->position >= DEADZONE)
                {
                    if (joystickMoved->position <= -DEADZONE && !joyLeft)
                    {
                        std::cout << "Joy LEFT START\n";
                        joyLeft = true;
                        if (currentScene()->actionExists(JL))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JL), "START"));
                        }
                    }
                    else
                    if (joystickMoved->position >= DEADZONE && !joyRight)
                    {
                        std::cout << "Joy RIGHT START\n";
                        joyRight = true;
                        if (currentScene()->actionExists(JR))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JR), "START"));
                        }
                    }
                }
                else
                {
                    if (joyLeft)
                    {
                        std::cout << "Joy LEFT END\n";
                        joyLeft = false;
                        if (currentScene()->actionExists(JL))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JL), "END"));
                        }
                    }
                    else
                    if (joyRight)
                    {
                        std::cout << "Joy RIGHT END\n";
                        joyRight = false;
                        if (currentScene()->actionExists(JR))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JR), "END"));
                        }
                    }
                }
            }
            if (joystickMoved->axis == sf::Joystick::Axis::Y)
            {
                if (joystickMoved->position <= -DEADZONE || joystickMoved->position >= DEADZONE)
                {
                    if (joystickMoved->position <= -DEADZONE && !joyUp)
                    {
                        std::cout << "Joy UP START\n";
                        joyUp = true;
                        if (currentScene()->actionExists(JU))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JU), "START"));
                        }
                    }
                    else
                    if (joystickMoved->position >= DEADZONE && !joyDown)
                    {
                        std::cout << "Joy DOWN START\n";
                        joyDown = true;
                        if (currentScene()->actionExists(JD))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JD), "START"));
                        }
                    }
                }
                else
                {
                    if (joyUp)
                    {
                        std::cout << "Joy UP END\n";
                        joyUp = false;
                        if (currentScene()->actionExists(JU))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JU), "END"));
                        }
                    }
                    else
                    if (joyDown)
                    {
                        std::cout << "Joy DOWN END\n";
                        joyDown = false;
                        if (currentScene()->actionExists(JD))
                        {
                            currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JD), "END"));
                        }
                    }
                }
            }
        }
        else
        {
            if (joyUp)
            {
                std::cout << "Joy UP END (2)\n";
                joyUp = false;
                if (currentScene()->actionExists(JU))
                {
                    currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JU), "END"));
                }
            }
            if (joyDown)
            {
                std::cout << "Joy DOWN END (2)\n";
                joyDown = false;
                if (currentScene()->actionExists(JD))
                {
                    currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JD), "END"));
                }
            }
            if (joyLeft)
            {
                std::cout << "Joy LEFT END (2)\n";
                joyLeft = false;
                if (currentScene()->actionExists(JL))
                {
                    currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JL), "END"));
                }
            }
            if (joyRight)
            {
                std::cout << "Joy RIGHT END (2)\n";
                joyRight = false;
                if (currentScene()->actionExists(JR))
                {
                    currentScene()->sDoAction(Action(currentScene()->getActionMap().at(JR), "END"));
                }
            }
        }
    }
}
