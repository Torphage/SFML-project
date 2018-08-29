
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "../headers/Player.h"
#include "../headers/Settings.h"
#include "../headers/Utils.h"
#include "../headers/Collider.h"

Player::Player(sf::RectangleShape body) {
    pos = sf::Vector2f(0, 0);
    speed = 1;
    switchedSpells = false;
    selectedSpell = 0;
    settings = Settings();
    std::vector<sf::Sprite> projectiles = std::vector<sf::Sprite> {};
    spellInventory = std::vector<Spell *> {};
    buffs = std::vector<Buff*> {};
    playeracc = 0.5;
    originalPlayerAcc = playeracc;
    fric = 1.2;
    this->body = body;
    this->body.setFillColor(sf::Color::Green);
    this->body.setOrigin(sf::Vector2f(body.getSize().x / 2, body.getSize().y / 2));
    castProgress = 0;
    casting = false;
    maxHp = 20;
    hitpoints = maxHp;
    timeSinceHurt = 0;
    level = 1;
}

Player::Player() {

}

Player::~Player() {

}

void Player::castSpell() {
   (*spellInventory[selectedSpell]).use();
   castProgress = 0;
}

void Player::addSpell(Spell *spell) {
    spellInventory.push_back(spell);
}

void Player::addProjectile(Projectile projectile) {
    projectiles.push_back(projectile);
}

void Player::clearProjectiles() {
    projectiles.clear();
}

std::vector<Projectile> &Player::getProjectiles() {
    return projectiles;
}

Spell *Player::getSpell(int index) {
    return spellInventory[index];
}

sf::Vector2f Player::getPos() {
    return body.getPosition();
}


void Player::setPos(sf::Vector2f newPos) {
    pos = newPos;
    body.setPosition(newPos);
}

void Player::setVel(sf::Vector2f newVel) {
    vel = newVel;
}

void Player::update(float dt) {
    acc = sf::Vector2f(0, 0);
    casting = false;
    switchedSpells = false;
    pos = body.getPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        acc.y = -playeracc;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        acc.y = playeracc;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        acc.x = -playeracc;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        acc.x = playeracc;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && selectedSpell != 0) {
        selectedSpell = 0;
        switchedSpells = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && selectedSpell != 1) {
        selectedSpell = 1;
        switchedSpells = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        casting = true;
        castProgress += 0.1 * (dt / Settings::TIMESCALE);
        if (castProgress > (*spellInventory[selectedSpell]).getCastTime() 
            && (*spellInventory[selectedSpell]).isReady) {
          castSpell();
        }
        if (!(*spellInventory[selectedSpell]).isReady) {
            casting = false;
        }

    }

    if (!casting || switchedSpells) {
      castProgress = 0;
    }

    if(!(acc.x == 0.0f || acc.y == 0.0f)) {
        acc = normalizedVec(acc) * playeracc;
    }

    acc += vel / fric;

    vel = acc;

    pos += vel * (dt / settings.TIMESCALE);

    body.setPosition(pos);

    for (int i = 0; i < projectiles.size(); i++) {
        if (projectiles[i].kill) {
            projectiles.erase(projectiles.begin() + i);
        }
    }
    
    uiCastBar.update(castProgress, (*spellInventory[selectedSpell]).getCastTime(), casting);
    if (switchedSpells) {
        (*spellBar).changeSelection(selectedSpell + 1);
    }

    if (timeSinceHurt > 0) {
        timeSinceHurt -= dt;
    }

}

void Player::draw(sf::RenderWindow &window) {
    window.draw(body);
}

Collider Player::getCollider() {
    return Collider(body, vel);
}

void Player::onCollision(sf::Vector2f direction) {
    if (direction.x != 0.0f) {
        vel.x = 0.0f;
        acc.x = 0.0f;
    } else if (direction.y != 0.0f) {
        vel.y = 0.0f;
        acc.y = 0.0f;
    }


    sf::Vector2f oldPos = body.getPosition();
    //body.setPosition(std::round(oldPos.x), std::round(oldPos.y));
    body.setPosition(oldPos.x, oldPos.y);
}

void Player::setMouseAngle(float angle) {
    mouseAngle = angle;
}

float Player::getMouseAngle() {
    return mouseAngle;
}

float Player::getMouseAngleRad() {
    return mouseAngle * (M_PI / 180);
}

sf::Vector2f Player::getMousePos() {
    return mousePos;
}
void Player::setMousePos(sf::Vector2f pos) {
    mousePos = pos;
}

void Player::hurt(float amount) {
    hitpoints -= amount;
    (*hpBar).update(hitpoints);
}

void Player::heal(float amount) {
    hitpoints += amount;
    (*hpBar).update(hitpoints);
}

float Player::getMaxHp() {
    return maxHp;
}

void Player::setHpBar(PlayerHpBar *bar) {
    hpBar = bar;
}

int Player::getLevel() {
    return level;
}

void Player::setLevel(int newLevel) {
    level = newLevel;
    (*levelIcon).update(level);
}

void Player::setLevelIcon(PlayerLevelIcon *icon) {
    levelIcon = icon;
}

void Player::setMoveSpeed(float newSpeed) {
    playeracc = newSpeed;
}

void Player::addBuff(Buff *buff) {
    buffs.push_back(buff);
    (*buff).begin(*this);
}

float Player::getSpeed() {
    return playeracc;
}

std::vector<Spell *> Player::getSpells() {
    return spellInventory;
}