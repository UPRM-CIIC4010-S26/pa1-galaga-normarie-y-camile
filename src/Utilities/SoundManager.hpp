#pragma once
#include "raylib-cpp/raylib-cpp.hpp"

class SoundManager {
    public:
    static inline Sound attack;
    static inline Sound dead;
    static inline Sound gameOver;
    static inline Sound hit;
    static inline Sound shoot;
    static inline Sound start;
    static inline Music bg_music; // BONUS
    static inline Music unhealing_music; //BONUS
    static inline Sound skull_kid; //BONUS

    static void Load() {
        attack = LoadSound("audio/attack.mp3");
        dead = LoadSound("audio/dead.mp3");
        gameOver = LoadSound("audio/gameOver.mp3");
        hit = LoadSound("audio/hit.mp3");
        shoot = LoadSound("audio/shoot.mp3");
        start = LoadSound("audio/Galaga_Theme.mp3");
        bg_music = LoadMusicStream("audio/galaga_bg.mp3");
        unhealing_music = LoadMusicStream("audio/unhealing.mp3");
        skull_kid = LoadSound("audio/Skullkid_Laugh.mp3");
    }

    static void Unload() {
        UnloadSound(attack);
        UnloadSound(dead);
        UnloadSound(gameOver);
        UnloadSound(hit);
        UnloadSound(shoot);
        UnloadSound(start); 
        UnloadMusicStream(bg_music);  
        UnloadMusicStream(unhealing_music);
        UnloadSound(skull_kid);  
    }
};
