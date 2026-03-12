#include "Program.hpp"
#include <iostream>
Program::Program() {
    Background::sideWalls = std::pair<HitBox, HitBox>{ 
        HitBox(0, 0, 10, GetScreenHeight()), 
        HitBox(GetScreenWidth() - 10, 0, 10, GetScreenHeight())
    };
PlaySound(SoundManager::start);
}

void Program::Update() {
    UpdateMusicStream(SoundManager::bg_music);
    UpdateMusicStream(SoundManager::unhealing_music);
    for (Animation& a : Animation::animations) a.update();
    for (int i = 0; i < Animation::animations.size(); i++) {
        if (Animation::animations[i].done) Animation::animations.erase(Animation::animations.begin() + i);
    }
    pauseFrames = std::max(pauseFrames - 1, 0);

    if (!startup && !paused && !gameOver && pauseFrames <= 0) {
        //Enemy::ManageEnemies(player->hitBox);
        score += Enemy::ManageEnemies(player->hitBox); 
        StdEnemy::attackReset();
        ManageEnemyRespawns();
        player->update();

        for (std::pair<std::pair<float, float>, Enemy*> p : Enemy::enemies) {
            if (p.second && HitBox::Collision(player->hitBox, p.second->hitBox)) {
                Animation::animations.push_back(
                    Animation(player->position.first, player->position.second, 16, 0, 33, 34, 30 ,30, 3, ImageManager::SpriteSheet)
                );

                PlaySound(SoundManager::gameOver);
                Projectile::projectiles.clear();
                player->position.first = GetScreenWidth() / 2 - 15;
                p.second->health = 0;
                pauseFrames = 120;
                lives--;
            }
        }

        for (Projectile& p : Projectile::projectiles) { 
            p.update(); 
            if (p.ID != 0 && HitBox::Collision(player->hitBox, p.getHitBox())) {
                Animation::animations.push_back(
                    Animation(player->position.first, player->position.second, 16, 0, 33, 34, 30 ,30, 3, ImageManager::SpriteSheet)
                );

                PlaySound(SoundManager::gameOver);
                Projectile::projectiles.clear();
                player->position.first = GetScreenWidth() / 2 - 15;
                pauseFrames = 120;
                lives--;
            }

        }


        if (lives <= 0 && pauseFrames <= 0) {
            StopMusicStream(SoundManager::bg_music); 
            StopMusicStream(SoundManager::unhealing_music); 
            gameOver = true;}
        Projectile::CleanProjectiles();
        Projectile::ProjectileCollision();
    }
    //std::cout << score << std::endl; test
    // update lives (when it is not hell mode)
    if (!hell_mode) {
        int milestones = score / 1000;
        while (bonusLivesGiven < milestones) {
        if (lives < 5) {
            lives++;
        }
        bonusLivesGiven++;
        }
    }
    //BONUS: added majoras mask pop up at game over in Hell Mode
    if (gameOver && hell_mode) {
        // adding flicker
        static int framesCounter = 0;
        static int count = 0;
        framesCounter++;
        if (count < 45)
        {
            if ((framesCounter/ 5) % 2 == 0) {
                count++;
                DrawTexture(ImageManager::majoras, (GetScreenWidth()/2 - ImageManager::majoras.width/2), (GetScreenHeight()/2 - ImageManager::majoras.height/2) , WHITE);
        }
        }
        // play skull kids laugh
        if (!skull_kid_played && hell_mode) {
            PlaySound(SoundManager::skull_kid);
            skull_kid_played = true;
        }
    }
}

void Program::Draw() {
    DrawText(TextFormat("Score: %08i", score), 100, 50, 20, WHITE); 
    DrawText(TextFormat("Lives: %i", lives), 100, 75, 20, WHITE);
    background.Draw();
    if (pauseFrames <= 0 && !gameOver) player->draw();
    for (Animation& a : Animation::animations) a.draw();

    for (int i = 0; i < lives; i++) {
         DrawTexturePro(ImageManager::SpriteSheet, Rectangle{0, 0, 17, 18}, 
                   Rectangle{10.0f + i * 30, GetScreenHeight() - 30.0f, 20, 20}, 
                   Vector2{0, 0}, 0, WHITE);
    }


    for (Projectile p : Projectile::projectiles) p.draw();
    for (std::pair<std::pair<float, float>, Enemy*>& p : Enemy::enemies) if (p.second) p.second->draw();

    if (startup) {
        DrawStartup();
    } 
    if (paused) DrawPauseScreen();
    if (gameOver) DrawGameOver();
}

void Program::ManageEnemyRespawns() {
    delay = std::max(delay - 1, 0);

    respawnCooldown -= 1;
    if (respawnCooldown <= 0) {
        //update cooldown speed 
        if (!hell_mode) {
            respawnCooldown = 1080 - (score / 20);
            respawnCooldown = std::max(respawnCooldown, 200);
        }
        else {
            respawnCooldown = 1000 - (score / 20);
            respawnCooldown = std::max(respawnCooldown, 100);
        }
        for (std::pair<std::pair<float, float>, Enemy*>& p : Enemy::enemies) {
            if (!p.second && p.first.second != 150) {
                int eType = GetRandomValue(1, 3);

                if (eType == 1) {
                    p.second = new StEnemy(GetScreenWidth() / 2 - 15, 0, true);
                    respawnCooldown /= 2;
                } else {
                    p.second = new StdEnemy(GetScreenWidth() / 2 - 15, 0, true);
                }
                respawns++;
                break;
            } else if (!p.second && p.first.second == 150) {
                p.second = new SpEnemy(GetScreenWidth() / 2 - 15, 0, true);
                respawns++;
                break;
            }
        }
        // std::cout << "COOLDOWN: "<< respawnCooldown << std::endl;
    }

    if(respawns >= 4) {
        count = 4;
        respawns = 0;
    }

    if (count > 0 && delay <= 0) {
        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{0, 0}, 
            new DyEnemy(GetScreenWidth(), 300)
        });

        count--;
        delay = 20;
    }
}

void Program::DrawStartup() {
    DrawRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), Color{0, 0, 0, 125});
    DrawText("Galaga", (GetScreenWidth() / 2 - 237), 75, 144, WHITE);
    DrawText("Normal Mode (Press Enter)", (GetScreenWidth() / 2) - 165, GetScreenHeight() / 2, 24, GRAY);
    DrawText("Hell Mode (Press L)", (GetScreenWidth() / 2) - 125, (GetScreenHeight() / 2) - 70 , 24, GRAY);
}

void Program::DrawPauseScreen() {
    DrawRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), Color{0, 0, 0, 125});
    DrawText("Paused", (GetScreenWidth() / 2) - 85, GetScreenHeight() / 2 - 60, 48, WHITE);
    DrawText("Press Enter", (GetScreenWidth() / 2) - 75, GetScreenHeight() / 2, 24, GRAY);
}

void Program::DrawGameOver() {
    DrawRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), Color{0, 0, 0, 125});
    DrawText("Game Over", (GetScreenWidth() / 2) - 380, 50, 144, WHITE);
    DrawText("Press 'T' to try again", (GetScreenWidth() / 2) - 140, GetScreenHeight() / 2, 24, GRAY);
}

void Program::KeyInputs() {
    if ((!gameOver && !startup && IsKeyPressed('P')) || (paused && IsKeyPressed(KEY_ENTER))) paused = !paused;
    if (!paused && !startup && IsKeyPressed('O')) gameOver = !gameOver;
    if (!gameOver && !paused && IsKeyPressed('I')) startup = !startup;
    if (IsKeyPressed('H')) HitBox::drawHitbox = !HitBox::drawHitbox;
    if (IsKeyPressed('K')) {
        score += 500;
    }
    if (IsKeyPressed('L')) {
        StopMusicStream(SoundManager::bg_music);
        StopSound(SoundManager::start); 
        startup = false;
        hell_mode = true;
        Enemy::HellTexture = true;
        PlayMusicStream(SoundManager::unhealing_music);
        SetMusicVolume(SoundManager::unhealing_music, 0.6f);
        Reset();
    }
    if (gameOver && IsKeyPressed('T')) {
        gameOver = false;
        startup = true;
        Enemy::enemies.clear();
        Projectile::projectiles.clear();
        Animation::animations.clear();
        Reset();
    }

    if (startup && IsKeyPressed(KEY_ENTER)) {
        StopMusicStream(SoundManager::unhealing_music);
        StopSound(SoundManager::start); 
        PlayMusicStream(SoundManager::bg_music);
        SetMusicVolume(SoundManager::bg_music, 0.6f);
        startup = false;
        hell_mode = false;
        Enemy::HellTexture = false;

        Reset();
    }

    if (!startup && !paused && !gameOver && pauseFrames <= 0) player->keyInputs();
   
}

void Program::PlayerReset() {
    Animation::animations.push_back(
        Animation(player->position.first, player->position.second, 16, 0, 33, 34, 30 ,30, 3, ImageManager::SpriteSheet)
    );

    PlaySound(SoundManager::gameOver);
    Projectile::projectiles.clear();
    player->position.first = GetScreenWidth() / 2 - 15;
    pauseFrames = 120;
    lives--;
}

void Program::Reset() {
    Enemy::enemies.clear();
    Projectile::projectiles.clear();
    Animation::animations.clear();
    StdEnemy::attackInProgress = false;
    player = new Player((GetScreenWidth() / 2) - 15, GetScreenHeight() * 0.75f);
    respawnCooldown = 1080;
    respawns = 0;
    count = 0;
    delay = 0;
    score = 0;
    skull_kid_played = false;
    if(hell_mode)
        lives = 1;
    else
        lives = 3;
    if(hell_mode) {
        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{200, 150}, 
            new SpEnemy(200, 150)
        });

        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{600, 150}, 
            new SpEnemy(600, 150)
        });
        //amount of enemies on screen (rn its 60 normal)
        for (int i = 0; i <60; i++) {
            int col = i % 12;
            int row = i / 12;
            float x = 150 + 45 * col;
            float y = 250 + 45 * row;

            Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
                std::pair<float, float>{x, y}, 
                new StdEnemy(x, y)
            });
        }
        
        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{100, 400}, 
            new DyEnemy(100, 400)
        });
        
        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{GetScreenWidth()-150, 400}, 
            new DyEnemy(GetScreenWidth()-150, 400)
        });
        //Boss is the Moon
        StdEnemy* boss = new StdEnemy(GetScreenWidth()/2 - 100, 150);
        boss->isBoss = true;         
        boss->health = 50;             
        boss->hitBox.setSize(200, 200);
        boss->position.first = GetScreenWidth()/2 - 150;
        boss->position.second = 50;
        
        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{boss->position.first, boss->position.second}, 
            boss
        });
    } else {
        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{350, 150}, 
            new SpEnemy(350, 150)
        });

        Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
            std::pair<float, float>{600, 150}, 
            new SpEnemy(600, 150)
        });

        for (int i = 0; i < 30; i++) {
            int col = i % 10;
            int row = i / 10;
            float x = 250 + 50 * col;
            float y = 200 + 50 * row;

            Enemy::enemies.push_back(std::pair<std::pair<float, float>, Enemy*> {
                std::pair<float, float>{x, y}, 
                new StdEnemy(x, y)
            });
        }
    }
}