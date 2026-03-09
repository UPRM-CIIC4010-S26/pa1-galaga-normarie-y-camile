#pragma once
#include "raylib-cpp/raylib-cpp.hpp"

class ImageManager {
    public:
        inline static Texture2D SpriteSheet;
        inline static Texture2D majoras;

        static void Load() {
            SpriteSheet = LoadTextureFromImage(LoadImage("images/Arcade - Galaga - Miscellaneous - General Sprites.png"));  
            majoras = LoadTextureFromImage(LoadImage("images/majoras.png"));  
        }

        static void Unload() {
            UnloadTexture(SpriteSheet);
            UnloadTexture(majoras);
        }
};