#pragma once
#include <SFML/Graphics.hpp>

#include "../libs/include/TMXParser.h"
#include "../libs/include/TSXParser.h"
#include "./Obstacle.h"

class TileMap {
public:
    TileMap(std::string path, std::vector<Obstacle> &obstacles);
    ~TileMap();

    void printData();
    // Size in tiles, size in pixels can be aquired
    // Through mapTexture.getSize()
    sf::Vector2i getSize();
    // Texture for sprite that will be used as background
    sf::Texture mapTexture;

private:
    TMX::Parser tmx;
    
    sf::Texture generateMap();
    
    sf::Image tileSetTexture;
    
    std::string path;
    std::string tileSetPath;

    int tileSize;
    int tileMargin;

    int tileSetWidth;
    int tileSetHeight;

    int width;
    int height;

};

void printVec(std::vector< std::vector<int> > vec);