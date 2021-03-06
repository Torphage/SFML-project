
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <vector>

#include "../headers/Npc.h"
#include "../headers/Utils.h"
#include "../headers/Settings.h"
#include "../headers/Obstacle.h"
#include "../headers/Player.h"

Enemy::Enemy(sf::Texture &texture, sf::Vector2f pos, 
          sf::Vector2f vel, float moveSpeed, 
          float maxHp, float hitpoints, 
          float attackStrength, 
          float attackSpeed, unsigned int level,
          Player &player):
    player(player) {
    this->vel = vel;
    this->moveSpeed = moveSpeed;
    this->maxHp = maxHp;
    this->hitpoints = hitpoints;
    this->attackStrength = attackStrength;
    this->attackSpeed = attackSpeed;
    this->level = level;

    settings = Settings();
    fric = 0.9;
    acc = sf::Vector2f(0.0f, 0.0f);

    setTexture(texture);
    setOrigin(sf::Vector2f(texture.getSize().x, texture.getSize().y) / 2.0f);
    setPosition(pos);
    hurtTime = 0;
}

Enemy::~Enemy() {

}

float Enemy::getHitpoints() {
    return hitpoints;
}

float Enemy::getMaxHitpoints() {
    return maxHp;
}

float Enemy::hurt(float amount) {
    hitpoints -= amount;
    hurtTime = 100.0f;
}

float Enemy::getAttackStr() {
    return attackStrength;
}

bool Enemy::isAlive() {
    return hitpoints > 0;
}

bool Enemy::canAttack() {
    return timeToAttack <= 0;
}

void Enemy::resetAttackTimer() {
    timeToAttack = attackSpeed;
}

void Enemy::setPathfinder(EnemyPathfinder pathfinder, TileMap &map) {
    this->pathfinder = pathfinder;
    this->pathfinder.generateGraph(map.getNavData(), 0.5);
    this->pathfinder.setCurrTime(500);
}

EnemyPathfinder *Enemy::getPathfinder() {
    return &pathfinder;
}

sf::Vector2f Enemy::getVel() {
    return vel;
}

SpriteCollider Enemy::getCollider() {
    return SpriteCollider(*this, getVel());
}

Slime::Slime(sf::Texture &texture, sf::Vector2f pos, sf::Vector2f vel, Player &player) :
    Enemy(texture, pos, vel, 1.0f, 10.0f, 10.0f, 2.0f, 100.0f, 1, player) {
}

Slime::Slime(sf::Texture &texture, sf::Vector2f pos, Player &player) :
    Enemy(texture, pos, sf::Vector2f(0.0f, 0.0f), 1.0f, 10.0f, 10.0f, 2.0f, 500.0f, 1, player){

}

Slime::~Slime() {

}

// Updates the internal state of the slime. This method handles all the 
// "thinking" a slime has to do. If it attacks, which way to go, etc.
// Args:
// dt - time since last update
void Slime::update(float dt) {
    pathfinder.update(dt, getPosition(), player.getPos());
    if (timeToAttack > 0) {
        timeToAttack -= dt;
    }
    acc = sf::Vector2f(0.0f, 0.0f);

    // Asking the pathfinder for directions
    sf::Vector2f direction;
    std::vector<sf::Vector2f> path = pathfinder.getPath();
    if (path.size() == 0) {
        direction = player.getPos() - getPosition();
    } else {
        direction = path[1] - getPosition();
    }

    acc = direction;
    if(!(acc.x == 0.0f && acc.y == 0.0f)) {
        acc = normalizedVec(acc) * moveSpeed;
    }

    vel = acc * (float)(dt * 0.1);
    sf::Vector2f step = vel;
    sf::Vector2f pos = getPosition();
    if (path.size() > 0) {
        if(path[1].x - pos.x == 0 && path[1].y - pos.y == 0) {
            pathfinder.increaseStep();
        }
        if (std::abs(path[1].x - pos.x) < std::abs(step.x)) {
            step.x = path[1].x - pos.x;
        }
        if (std::abs(path[1].y - pos.y) < std::abs(step.y)) {
            step.y = path[1].y - pos.y;
        }
    }
    pos += step;

    // Flash red if hurt
    setPosition(pos);
    if (hurtTime > 0) {
        setColor(sf::Color(255, 0, 0));
        hurtTime -= dt;
    } else {
        setColor(sf::Color(255, 255, 255));
    }

}

void Slime::draw(sf::RenderWindow &window) {
    window.draw(*this);
}

EnemyFactory::EnemyFactory(Player &player, TileMap &map):
    player(player), map(map) {
    sf::Texture slimeTexture;
    slimeTexture.loadFromFile("./resources/enemy_textures/slime.png");
    slimeTexture.setSmooth(true);
    enemyTextures.push_back(slimeTexture);

    enemyPathfinders.push_back(EnemyPathfinder(1000));
    enemyPathfinders[0].generateGraph(map.getNavData(), 1);
    enemyPathfinders[0].setAggroRange(500);
}

EnemyFactory::~EnemyFactory() {
}

// Spawns a given enemy at a location
// Args:
// enemyType - the name of the enemy to be spawned
// pos       - the position where the enemy will be spawned
void EnemyFactory::spawnEnemy(std::string enemyType, sf::Vector2f pos) {
    if (enemyType == "slime") {
        enemies.push_back(std::unique_ptr<Enemy>(new Slime(enemyTextures[0], pos, player)));
        enemies.back()->setPathfinder(enemyPathfinders[0], map);
    }
}

// Updates itself and all belonging enemies
// Args:
// dt - time since last update
void EnemyFactory::update(float dt) {
    std::vector<int> deadEnemies;
    int i = 0;
    for (auto itr = enemies.begin(); itr != enemies.end(); ++itr, i++) {
        (*itr)->update(dt);
        if (!(*itr)->isAlive()) {
            deadEnemies.push_back(i);
        }
    }
    // Remove dead enemies
    for (i = deadEnemies.size() - 1; i >= 0; i--) {
        enemies.erase(enemies.begin() + deadEnemies[i]);
    }
}

// Draws all enemies
// Args:
// window - reference to the game window
void EnemyFactory::draw(sf::RenderWindow &window) {
    for (auto itr = enemies.begin(); itr != enemies.end(); ++itr) {
        (*itr)->draw(window);
    }
}

// Checks all containing enemies for collision with walls
// Args:
// obstacles - all walls/obstacles on the map
void EnemyFactory::wallCollide(std::vector<Obstacle> obstacles) {
    sf::Vector2f direction;
    for (Obstacle &obstacle : obstacles) {
        Collider obstacleColl = obstacle.getCollider();
        for (auto itr = enemies.begin(); itr != enemies.end(); ++itr) {
            (*itr)->getCollider().checkCollision(&obstacleColl, direction, 0.0f);
        }
    }
}

void EnemyFactory::hurtEnemy(int i, int amount) {
    (*enemies[i]).hurt(amount);
}

// Checks all belonging enemies for collisions with spells
// and calls the apropriate functions
// Args:
// projs - reference to all projectiles that can be collided with
void EnemyFactory::spellCollide(std::vector<Projectile> &projs) {
    for (auto itr = enemies.begin(); itr != enemies.end(); ++itr) {
        SpriteCollider currentSprite = (*itr)->getCollider();
        for (Projectile &projectile : projs) {
            SpriteCollider currentSpell = projectile.getCollider();
            if (currentSprite.isColliding(&currentSpell)) {
                projectile.onCollision(*(itr->get()));
            }
        }
    }
}

// Check all enemies for collision with player
// Args:
// player - reference to the player object
void EnemyFactory::playerCollide(Player &player) {
    sf::Vector2f direction;
    for (auto itr = enemies.begin(); itr != enemies.end(); ++itr) {
        SpriteCollider currentSprite = (*itr)->getCollider();
        Collider playerCol = player.getCollider();
        if (currentSprite.checkCollision(&playerCol, direction, 0.0f)) {
            if ((*itr)->canAttack()) {
                player.hurt((*itr)->getAttackStr());
                (*itr)->resetAttackTimer();
            }
        }
    }
}

// Check all enemies for collisions with active explosions
// Args:
// explosions - references to all active explosions
void EnemyFactory::explosionCollide(std::vector<Explosion> &explosions) {
    for (auto itr = enemies.begin(); itr != enemies.end(); ++itr) {
        SpriteCollider currentSprite = (*itr)->getCollider();
        for (auto iitr = explosions.begin(); iitr != explosions.end(); ++iitr) {
            CollisionCircle currentExplosion = iitr->getCollider();
            if (currentExplosion.isColliding(&currentSprite) && iitr->active) {
                iitr->onCollision(*(itr->get()));
            }
        }
    }
}

sf::Vector2f EnemyFactory::getEnemy(int index) {
    return enemies[index]->getPosition();
}

void EnemyFactory::generatePathTexture(int index, sf::RenderTexture &text, sf::Sprite &sprite) {
    enemies[index]->getPathfinder()->generatePathTexture(text, sprite);
}

void EnemyFactory::generateGraphTexture(int index, sf::RenderTexture &text, sf::Sprite &sprite) {
    enemies[index]->getPathfinder()->generateGraphTexture(text, sprite);
}

