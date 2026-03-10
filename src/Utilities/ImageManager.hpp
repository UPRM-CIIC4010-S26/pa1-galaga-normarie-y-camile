#pragma once
#include "raylib-cpp/raylib-cpp.hpp"

class ImageManager {
    public:
        inline static Texture2D SpriteSheet;
        inline static Texture2D majoras;
        inline static Texture2D moon;

        static void Load() {
            SpriteSheet = LoadTextureFromImage(LoadImage("images/Arcade - Galaga - Miscellaneous - General Sprites.png"));  
            majoras = LoadTextureFromImage(LoadImage("images/majoras.png"));  
            moon = LoadTextureFromImage(LoadImage("images/moon.png"));
        }

        static void Unload() {
            UnloadTexture(SpriteSheet);
            UnloadTexture(majoras);
        }
};