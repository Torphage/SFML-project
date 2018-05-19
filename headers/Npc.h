#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "./Settings.h"

// Abstract class serving as superclass for all enemies.
// Different types of enemies will have their own classes.
// Args:
// texture - The texture
// pos - Initial position of the enemy
// vel - Initial velocity of the enemy
// moveSpeed - Scalar for the speed of the enemy
// maxHp - maximum hitpoints for the enemy
// hitpoints - Initial hitpoints of the enemy
class Enemy: public sf::Sprite {
public:
    Enemy(sf::Texture &texture, sf::Vector2f pos, 
          sf::Vector2f vel, float moveSpeed, 
          float maxHp, float hitpoints, 
          float attackStrength,
          float attackSpeed, unsigned int level);
    ~Enemy();

    virtual void update(float dt){};
    virtual void draw(sf::RenderWindow &window){};

    sf::Vector2f getVel();

    float getHitpoints();
    float getMaxHitpoints();

    unsigned int getLevel();

protected:
    float maxHp;
    float hitpoints;

    float moveSpeed;
    float fric;

    float attackStrength;
    float attackSpeed;

    Settings settings;

    unsigned int level;

    sf::Vector2f vel;
    sf::Vector2f acc;
};

class Slime: public Enemy {
public:
    Slime(sf::Texture &texture, sf::Vector2f pos, sf::Vector2f vel);
    ~Slime();

    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;
};
