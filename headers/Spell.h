#pragma once
#include <SFML/Graphics.hpp>
#include "./Player.h"
#include "./Npc.h"
#include "./Collider.h"
#include "./Animation.h"
#include "./Explosion.h"

class Player;
class Enemy;
class Buff;
class Spell {
public:
    Spell();
    ~Spell();
    virtual void use();
    void setAnimation(Animation anim);
    std::string name;
    std::string spellType;
    virtual int getCastTime();
    virtual float getCooldown();
    virtual float getCooldownTimer();
    virtual void update(float dt);
    bool isReady;

    static  std::vector<Explosion> *explosions;
    float getManaCost();
private:

protected:
    int castTime;
    float cooldown;
    float cooldownTimer;
    float manaCost;
    Animation anim;
    bool isAnimated = false;
};

class Buff {
public:
    Buff();
    ~Buff();
    virtual void update(Player &player, float dt);
    virtual void begin(Player &player);
    virtual void end(Player &player);
    bool kill;

protected:
    float counter;
    float duration;

};

class Projectile: public sf::Sprite{
public:
    Projectile(sf::Texture &texture, sf::Vector2f vel,
               float speed, sf::Vector2f pos,
               float rotation, float scale,
               void (*callback)(Projectile &projectile,
               float dt, sf::Vector2f mousePos));
    Projectile(sf::Texture &texture, sf::Vector2f vel,
               float speed, sf::Vector2f pos,
               float rotation, float scale,
               void (*callback)(Projectile &projectile,
               float dt, sf::Vector2f mousePos),
               bool (*onCollide)(Enemy &enemy));
    Projectile(sf::Texture &texture, sf::IntRect textSize, sf::Vector2f vel,
               float speed, sf::Vector2f pos,
               float rotation, float scale,
               void (*callback)(Projectile &projectile,
               float dt, sf::Vector2f mousePos),
               bool (*onCollide)(Enemy &enemy));
    ~Projectile();
    Projectile();
    void update(float dt, sf::Vector2f mousePos);
    void draw(sf::RenderWindow &window);
    void onCollision(Enemy &enemy);
    void (*func)(Projectile &projectile, float dt, sf::Vector2f mousePos);
    // Will be overloaded for different types of collisions
    bool (*onCollide)(Enemy &enemy);
    bool kill;

    float getSpeed();
    float counter;
    

    sf::Vector2f vel;
    SpriteCollider getCollider();

    void setAnimation(Animation anim);
    void setTextureSize(sf::IntRect newSize);
private:
    Animation anim;
    bool isAnimated = false;
    float speed;
    float rotation;
};

class Fireball: public Spell {
public:
    Fireball(Player &player);
    ~Fireball();
    void use() override;
    int getCastTime() override;
    void update(float dt) override;
    float getCooldownTimer() override;
    float getCooldown() override;

private:
    int castTime;
    Player &player;
    sf::Texture texture;

};

class MagicMissile: public Spell {
public:
    MagicMissile(Player &player);
    ~MagicMissile();
    void use() override;
    int getCastTime() override;
    void update(float dt) override;
    float getCooldownTimer() override;
    float getCooldown() override;

private:
    int castTime;
    Player &player;
    sf::Texture texture;
};

class SprintBuff: public Buff {
public:
    SprintBuff();
    SprintBuff(Player &player);
    ~SprintBuff();
    void update(Player &player, float dt) override;
    void begin(Player &player) override;
    void end(Player &player) override;



private:
    float speedBuff;
    float playerStartSpeed;

};

class SprintSpell: public Spell {
public:

    SprintSpell(Player &player);
    ~SprintSpell();
    void use() override;
    int getCastTime();
    void update(float dt) override;
    float getCooldownTimer() override;
    float getCooldown() override;

private:
    int castTime;
    Player &player;
    SprintBuff temp;
    Buff *buff;
    
};

