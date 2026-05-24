#include "Game.h"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

Game::Game()
    : state(GState::MENU)
    , score(0)
    , highScore(0)
    , foodEaten(0)
    , moveTimer(0)
    , moveInterval(0.18f)
    , echoSpawnTimer(0)
    , timeSlowLeft(0)
    , ghostLeft(0)
    , difficultyTimer(0)
    , screenShake(0)
    , spcCount(0)
    , menuOpt(0)
    , menuGlow(0)
{
    LoadHighScore();
    srand((unsigned int)time(nullptr));
}

void Game::SaveHighScore()
{
    FILE* f = fopen("timeecho_highscore.dat", "w");
    if (f) {
        fprintf(f, "%d", highScore);
        fclose(f);
    }
}

void Game::LoadHighScore()
{
    FILE* f = fopen("timeecho_highscore.dat", "r");
    if (f) {
        fscanf(f, "%d", &highScore);
        fclose(f);
    }
}

void Game::StartGame()
{
    snake.Reset();
    foods.clear();
    echoes.clear();
    score = 0;
    foodEaten = 0;
    moveTimer = 0;
    moveInterval = 0.18f;
    echoSpawnTimer = 0;
    timeSlowLeft = 0;
    ghostLeft = 0;
    difficultyTimer = 0;
    screenShake = 0;
    spcCount = 0;
    
    SpawnFood();
    SpawnFood();
    state = GState::PLAYING;
}

void Game::SpawnFood()
{
    FoodItem fi;
    int tries = 0;
    bool ok;
    
    do {
        ok = true;
        fi.pos.x = (float)(rand() % GW);
        fi.pos.y = (float)(rand() % GH);
        
        for (auto& s : snake.Body()) {
            if (s.x == fi.pos.x && s.y == fi.pos.y) {
                ok = false;
                break;
            }
        }
        for (auto& e : echoes) {
            if (e.pos.x == fi.pos.x && e.pos.y == fi.pos.y) {
                ok = false;
                break;
            }
        }
        for (auto& f : foods) {
            if (f.pos.x == fi.pos.x && f.pos.y == fi.pos.y) {
                ok = false;
                break;
            }
        }
        tries++;
    } while (!ok && tries < 100);
    
    // Random type: 60% normal, 20% time crystal, 20% ghost orb
    int r = rand() % 100;
    if (r < 60)
        fi.type = FoodType::NORMAL;
    else if (r < 80)
        fi.type = FoodType::TIME_CRYSTAL;
    else
        fi.type = FoodType::GHOST_ORB;
    
    fi.bobPhase = (float)(rand() % 360);
    foods.push_back(fi);
}

void Game::SpawnFoodOfType(FoodType t)
{
    FoodItem fi;
    int tries = 0;
    bool ok;
    
    do {
        ok = true;
        fi.pos.x = (float)(rand() % GW);
        fi.pos.y = (float)(rand() % GH);
        
        for (auto& s : snake.Body()) {
            if (s.x == fi.pos.x && s.y == fi.pos.y) {
                ok = false;
                break;
            }
        }
        for (auto& e : echoes) {
            if (e.pos.x == fi.pos.x && e.pos.y == fi.pos.y) {
                ok = false;
                break;
            }
        }
        for (auto& f : foods) {
            if (f.pos.x == fi.pos.x && f.pos.y == fi.pos.y) {
                ok = false;
                break;
            }
        }
        tries++;
    } while (!ok && tries < 100);
    
    fi.type = t;
    fi.bobPhase = (float)(rand() % 360);
    foods.push_back(fi);
}

void Game::DoEchoes(float dt)
{
    // Remove expired echoes after they fade
    echoes.erase(
        std::remove_if(echoes.begin(), echoes.end(),
            [](const EchoObstacle& e) { return e.alpha <= 0; }),
        echoes.end()
    );
    
    // Pulse effect
    for (auto& e : echoes) {
        e.pulse += dt * 3;
        if (e.pulse > 6.28f) e.pulse -= 6.28f;
    }
}

void Game::DoParticles(float dt)
{
    for (auto& p : particles) {
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.vel.y += 120 * dt; // gravity
        p.life -= dt;
    }
    
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0; }),
        particles.end()
    );
}

void Game::BurstParticles(Vector2 pos, Color col, int n)
{
    for (int i = 0; i < n; i++) {
        Particle p;
        p.pos = pos;
        float ang = (float)(rand() % 360) * 3.14159f / 180.0f;
        float spd = 40 + (float)(rand() % 80);
        p.vel.x = cosf(ang) * spd;
        p.vel.y = sinf(ang) * spd;
        p.col = col;
        p.maxLife = 0.5f + (float)(rand() % 50) / 100.0f;
        p.life = p.maxLife;
        particles.push_back(p);
    }
}

void Game::HandleInput()
{
    if (state == GState::MENU) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            menuOpt = (menuOpt - 1 + 3) % 3;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            menuOpt = (menuOpt + 1) % 3;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            switch (menuOpt) {
                case 0: StartGame(); break;
                case 1: state = GState::CONTROLS; break;
                case 2: break; // quit
            }
        }
    }
    else if (state == GState::PLAYING) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            snake.SetDir(Dir::UP);
        else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            snake.SetDir(Dir::DOWN);
        else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
            snake.SetDir(Dir::LEFT);
        else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
            snake.SetDir(Dir::RIGHT);
        else if (IsKeyPressed(KEY_ESCAPE))
            state = GState::MENU;
    }
    else if (state == GState::GAME_OVER) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            StartGame();
        else if (IsKeyPressed(KEY_ESCAPE))
            state = GState::MENU;
    }
    else if (state == GState::CONTROLS) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            state = GState::MENU;
    }
}

void Game::Update()
{
    menuGlow += GetFrameTime() * 2;
    
    if (state != GState::PLAYING) return;
    
    float dt = GetFrameTime();
    
    // Slow motion effect
    float timeScale = 1.0f;
    if (timeSlowLeft > 0) {
        timeScale = 0.4f;
        timeSlowLeft -= dt;
        if (timeSlowLeft < 0) timeSlowLeft = 0;
    }
    if (ghostLeft > 0) {
        ghostLeft -= dt * timeScale;
        if (ghostLeft < 0) ghostLeft = 0;
    }
    
    dt *= timeScale;
    
    // Difficulty increase
    difficultyTimer += dt;
    if (difficultyTimer > 8.0f) {
        difficultyTimer = 0;
        moveInterval *= 0.92f;
        if (moveInterval < 0.06f) moveInterval = 0.06f;
    }
    
    // Movement
    moveTimer += dt;
    if (moveTimer >= moveInterval) {
        moveTimer -= moveInterval;
        
        snake.Move();
        
        // Wall collision
        if (snake.HitWall(GW, GH)) {
            state = GState::GAME_OVER;
            if (score > highScore) {
                highScore = score;
                SaveHighScore();
            }
            screenShake = 0.3f;
            return;
        }
        
        // Self collision
        if (snake.HitSelf()) {
            state = GState::GAME_OVER;
            if (score > highScore) {
                highScore = score;
                SaveHighScore();
            }
            screenShake = 0.3f;
            return;
        }
        
        // Echo collision
        Vector2 h = snake.Head();
        for (auto& e : echoes) {
            if (e.pos.x == h.x && e.pos.y == h.y && e.alpha > 0.3f) {
                if (ghostLeft > 0) {
                    // pass through, destroy echo
                    e.alpha = 0;
                } else {
                    state = GState::GAME_OVER;
                    if (score > highScore) {
                        highScore = score;
                        SaveHighScore();
                    }
                    screenShake = 0.3f;
                    return;
                }
            }
        }
        
        // Food collision
        for (auto it = foods.begin(); it != foods.end(); ) {
            if (it->pos.x == h.x && it->pos.y == h.y) {
                snake.Grow();
                foodEaten++;
                
                int bx = GetOX() + (int)it->pos.x * CELL_SZ + CELL_SZ / 2;
                int by = GetOY() + (int)it->pos.y * CELL_SZ + CELL_SZ / 2;
                
                switch (it->type) {
                    case FoodType::NORMAL:
                        score += 10;
                        BurstParticles({ (float)bx, (float)by }, (Color){ 50, 220, 90, 255 }, 8);
                        break;
                    case FoodType::TIME_CRYSTAL:
                        score += 25;
                        timeSlowLeft = 5.0f;
                        BurstParticles({ (float)bx, (float)by }, (Color){ 50, 150, 255, 255 }, 12);
                        break;
                    case FoodType::GHOST_ORB:
                        score += 20;
                        ghostLeft = 5.0f;
                        BurstParticles({ (float)bx, (float)by }, (Color){ 200, 80, 255, 255 }, 12);
                        break;
                }
                
                it = foods.erase(it);
                
                // Spawn new food
                SpawnFood();
                
                // Every 5 food, spawn a special one
                if (foodEaten % 5 == 0) {
                    SpawnFoodOfType(FoodType::TIME_CRYSTAL);
                }
                if (foodEaten % 7 == 0) {
                    SpawnFoodOfType(FoodType::GHOST_ORB);
                }
                
                // Keep a reasonable number
                while ((int)foods.size() > 4) {
                    foods.erase(foods.begin());
                }
            } else {
                ++it;
            }
        }
        
        // Spawn echo obstacles at the snake's previous positions
        // We track the tail position
        echoSpawnTimer += moveInterval; // accumulate based on moves
        if (echoSpawnTimer >= 0.6f) {
            echoSpawnTimer = 0;
            
            // Place echo at random past body position
            if (snake.Length() > 3) {
                int idx = 1 + (rand() % (snake.Length() - 1));
                auto it2 = snake.Body().begin();
                std::advance(it2, idx);
                
                EchoObstacle eo;
                eo.pos = *it2;
                eo.alpha = 0.7f;
                eo.pulse = 0;
                
                // Don't place on food or existing echoes
                bool blocked = false;
                for (auto& f : foods) {
                    if (f.pos.x == eo.pos.x && f.pos.y == eo.pos.y)
                        blocked = true;
                }
                for (auto& e : echoes) {
                    if (e.pos.x == eo.pos.x && e.pos.y == eo.pos.y)
                        blocked = true;
                }
                if (eo.pos.x == snake.Head().x && eo.pos.y == snake.Head().y)
                    blocked = true;
                
                if (!blocked) {
                    echoes.push_back(eo);
                }
            }
        }
        
        // Update echoes (fade oldest first when too many)
        while ((int)echoes.size() > 40) {
            echoes.erase(echoes.begin());
        }
        
        // Fade echoes slowly
        for (auto& e : echoes) {
            e.alpha -= 0.001f;
        }
    }
    
    // Screen shake decay
    if (screenShake > 0) {
        screenShake -= dt;
        if (screenShake < 0) screenShake = 0;
    }
    
    // Echos update
    DoEchoes(dt);
    DoParticles(dt);
}

void Game::Draw()
{
    int ox = GetOX();
    int oy = GetOY();
    
    switch (state) {
        case GState::MENU: DrawMenu(); break;
        case GState::PLAYING: DrawPlaying(); break;
        case GState::GAME_OVER: DrawGameOver(); break;
        case GState::CONTROLS: DrawControls(); break;
    }
}

void Game::DrawMenu()
{
    // Background
    DrawRectangle(0, 0, SW, SH, (Color){ 15, 15, 25, 255 });
    
    // Stars
    for (int i = 0; i < 80; i++) {
        int sx = (i * 137 + 42) % SW;
        int sy = (i * 97 + 13) % SH;
        int bright = (int)(100 + 155 * sinf(menuGlow + i * 0.7f));
        unsigned char sb = (unsigned char)bright;
        DrawCircle(sx, sy, 1.5f, (Color){ sb, sb, sb, 255 });
    }
    
    // Title
    const char* title = "TIMEE SNAKE";
    int titleW = MeasureText(title, 60);
    DrawText(title, (SW - titleW) / 2, 140, 60, (Color){ 50, 220, 90, 255 });
    
    // Subtitle
    const char* sub = "ECHO";
    int subW = MeasureText(sub, 80);
    int subX = (SW - subW) / 2;
    Color echoCol = (Color){
        100, 60, 255,
        (unsigned char)(180 + 75 * sinf(menuGlow))
    };
    DrawText(sub, subX, 200, 80, echoCol);
    
    const char* desc = "Fight your own past";
    int descW = MeasureText(desc, 18);
    DrawText(desc, (SW - descW) / 2, 280, 18, (Color){ 180, 180, 200, 255 });
    
    // Menu options
    const char* opts[3] = { "PLAY", "CONTROLS", "QUIT" };
    for (int i = 0; i < 3; i++) {
        int fs = (i == menuOpt) ? 32 : 24;
        Color c;
        if (i == menuOpt) {
            float g = 0.5f + 0.5f * sinf(menuGlow * 1.5f);
            c = (Color){ (unsigned char)(80 + 175 * g), (unsigned char)(220 * g), (unsigned char)(80 + 175 * g), 255 };
        } else {
            c = (Color){ 140, 140, 160, 255 };
        }
        
        int w = MeasureText(opts[i], fs);
        int x = (SW - w) / 2;
        int y = 340 + i * 55;
        DrawText(opts[i], x, y, fs, c);
        
        if (i == menuOpt) {
            DrawLine(x - 20, y + fs + 2, x + w + 20, y + fs + 2,
                (Color){ 50, 220, 90, 150 });
        }
    }
    
    // Footer
    const char* foot = "Arrow keys to navigate, Enter to select";
    int footW = MeasureText(foot, 14);
    DrawText(foot, (SW - footW) / 2, SH - 40, 14, (Color){ 100, 100, 120, 255 });
}

void Game::DrawPlaying()
{
    int ox = GetOX();
    int oy = GetOY();
    
    // Screen shake
    float shakeX = 0, shakeY = 0;
    if (screenShake > 0) {
        shakeX = (float)((rand() % 20) - 10) * screenShake * 2;
        shakeY = (float)((rand() % 20) - 10) * screenShake * 2;
    }
    
    BeginScissorMode(0, 0, SW, SH);
    
    // Background
    DrawRectangle(0, 0, SW, SH, (Color){ 10, 10, 18, 255 });
    
    // Game area background
    DrawRectangle(ox + (int)shakeX, oy + (int)shakeY, GW * CELL_SZ, GH * CELL_SZ, (Color){ 18, 18, 30, 255 });
    
    // Grid lines
    for (int x = 0; x <= GW; x++) {
        int lx = ox + (int)shakeX + x * CELL_SZ;
        DrawLine(lx, oy + (int)shakeY, lx, oy + (int)shakeY + GH * CELL_SZ, (Color){ 25, 25, 40, 255 });
    }
    for (int y = 0; y <= GH; y++) {
        int ly = oy + (int)shakeY + y * CELL_SZ;
        DrawLine(ox + (int)shakeX, ly, ox + (int)shakeX + GW * CELL_SZ, ly, (Color){ 25, 25, 40, 255 });
    }
    
    // Draw echoes
    for (auto& e : echoes) {
        if (e.alpha <= 0) continue;
        int ex = ox + (int)shakeX + (int)e.pos.x * CELL_SZ;
        int ey = oy + (int)shakeY + (int)e.pos.y * CELL_SZ;
        
        float pulse = 0.5f + 0.5f * sinf(e.pulse);
        unsigned char a = (unsigned char)(e.alpha * 255 * (0.5f + 0.5f * pulse));
        
        Color ec = { 100, 60, 255, a };
        DrawRectangle(ex + 2, ey + 2, CELL_SZ - 4, CELL_SZ - 4, ec);
        
        // Glow border
        Color gl = { 120, 80, 255, (unsigned char)(a / 3) };
        DrawRectangleLinesEx((Rectangle){ (float)ex, (float)ey, (float)CELL_SZ, (float)CELL_SZ }, 1, gl);
    }
    
    // Draw foods
    float time = (float)GetTime();
    for (auto& f : foods) {
        int fx = ox + (int)shakeX + (int)f.pos.x * CELL_SZ;
        int fy = oy + (int)shakeY + (int)f.pos.y * CELL_SZ;
        
        float bob = sinf(time * 3 + f.bobPhase) * 2;
        fy += (int)bob;
        
        int half = CELL_SZ / 2;
        int cx = fx + half;
        int cy = fy + half;
        int rad = CELL_SZ / 2 - 2;
        
        switch (f.type) {
            case FoodType::NORMAL:
                DrawCircle(cx, cy, rad, (Color){ 50, 220, 90, 255 });
                DrawCircle(cx, cy, rad - 4, (Color){ 80, 255, 120, 200 });
                break;
            case FoodType::TIME_CRYSTAL: {
                float gl = 0.7f + 0.3f * sinf(time * 4 + f.bobPhase);
                unsigned char ga = (unsigned char)(gl * 255);
                unsigned char ga3 = (unsigned char)((int)ga / 3);
                DrawCircle(cx, cy, rad + 3, (Color){ 50, 150, 255, ga3 });
                DrawCircle(cx, cy, rad, (Color){ 50, 150, 255, 255 });
                DrawCircle(cx, cy, rad - 3, (Color){ 120, 200, 255, 200 });
                DrawTriangle(
                    { (float)cx, (float)(cy - rad + 2) },
                    { (float)(cx - rad + 2), (float)cy },
                    { (float)(cx + rad - 2), (float)cy },
                    (Color){ 180, 230, 255, 150 }
                );
                DrawTriangle(
                    { (float)cx, (float)(cy + rad - 2) },
                    { (float)(cx - rad + 2), (float)cy },
                    { (float)(cx + rad - 2), (float)cy },
                    (Color){ 80, 180, 255, 100 }
                );
                break;
            }
            case FoodType::GHOST_ORB: {
                float gl = 0.7f + 0.3f * sinf(time * 3 + f.bobPhase * 1.3f);
                unsigned char ga = (unsigned char)(gl * 255);
                unsigned char ga3g = (unsigned char)((int)ga / 3);
                unsigned char ga2g = (unsigned char)((int)ga / 2);
                DrawCircle(cx, cy, rad + 3, (Color){ 200, 80, 255, ga3g });
                DrawCircle(cx, cy, rad, (Color){ 200, 80, 255, 255 });
                DrawCircle(cx, cy, rad - 2, (Color){ 230, 180, 255, 200 });
                DrawCircle(cx + 2, cy - 3, 3, (Color){ 255, 200, 255, ga2g });
                break;
            }
        }
    }
    
    // Draw snake
    snake.Draw(ox + (int)shakeX, oy + (int)shakeY, CELL_SZ);
    
    // Particles
    for (auto& p : particles) {
        float a = p.life / p.maxLife;
        Color c = p.col;
        c.a = (unsigned char)(a * 255);
        DrawCircle((int)p.pos.x + (int)shakeX, (int)p.pos.y + (int)shakeY, 3 * a, c);
    }
    
    EndScissorMode();
    
    // HUD
    // Top bar
    DrawRectangle(0, 0, SW, oy, (Color){ 10, 10, 18, 220 });
    DrawLine(0, oy, SW, oy, (Color){ 30, 30, 50, 255 });
    
    char scoreTxt[32];
    sprintf(scoreTxt, "SCORE: %d", score);
    DrawText(scoreTxt, 20, 20, 22, (Color){ 220, 220, 240, 255 });
    
    char hiTxt[32];
    sprintf(hiTxt, "BEST: %d", highScore);
    DrawText(hiTxt, 200, 20, 18, (Color){ 140, 140, 160, 255 });
    
    // Power-up indicators
    int pux = SW - 250;
    if (timeSlowLeft > 0) {
        int secs = (int)(timeSlowLeft + 0.5f);
        char buf[32];
        sprintf(buf, "SLOW %ds", secs);
        DrawText(buf, pux, 20, 18, (Color){ 50, 150, 255, 255 });
        pux += 120;
    }
    if (ghostLeft > 0) {
        int secs = (int)(ghostLeft + 0.5f);
        char buf[32];
        sprintf(buf, "GHOST %ds", secs);
        DrawText(buf, pux, 20, 18, (Color){ 200, 80, 255, 255 });
    }
    
    // Speed indicator
    float spdPct = (0.18f - moveInterval) / (0.18f - 0.06f);
    int barW = 80;
    int barH = 6;
    int barX = SW / 2 - barW / 2;
    int barY = oy / 2 - barH / 2;
    DrawRectangle(barX, barY, barW, barH, (Color){ 30, 30, 50, 255 });
    DrawRectangle(barX, barY, (int)(barW * spdPct), barH, (Color){ 50, 220, 90, 200 });
    
    // Pause hint
    const char* escHint = "ESC - Menu";
    DrawText(escHint, SW - 110, SH - 25, 14, (Color){ 80, 80, 100, 255 });
}

void Game::DrawGameOver()
{
    // Dim the game behind
    DrawRectangle(0, 0, SW, SH, (Color){ 0, 0, 0, 180 });
    
    // Overlay
    int boxW = 400;
    int boxH = 280;
    int bx = (SW - boxW) / 2;
    int by = (SH - boxH) / 2;
    
    DrawRectangle(bx, by, boxW, boxH, (Color){ 15, 15, 30, 230 });
    DrawRectangleLines(bx, by, boxW, boxH, (Color){ 50, 220, 90, 200 });
    
    const char* go = "GAME OVER";
    int goW = MeasureText(go, 44);
    DrawText(go, (SW - goW) / 2, by + 30, 44, (Color){ 255, 80, 80, 255 });
    
    char sc[32];
    sprintf(sc, "Score: %d", score);
    int scW = MeasureText(sc, 28);
    DrawText(sc, (SW - scW) / 2, by + 100, 28, (Color){ 220, 220, 240, 255 });
    
    char hi[32];
    sprintf(hi, "Best: %d", highScore);
    int hiW = MeasureText(hi, 22);
    
    Color hiCol;
    if (score >= highScore && score > 0) {
        hiCol = (Color){ 50, 220, 90, 255 };
        const char* best = "NEW BEST!";
        int bW = MeasureText(best, 22);
        DrawText(best, (SW - bW) / 2, by + 135, 22, hiCol);
    } else {
        hiCol = (Color){ 180, 180, 200, 255 };
    }
    DrawText(hi, (SW - hiW) / 2, by + 160, 22, hiCol);
    
    const char* retry = "SPACE / ENTER - Retry";
    int rW = MeasureText(retry, 18);
    DrawText(retry, (SW - rW) / 2, by + 210, 18, (Color){ 140, 140, 160, 255 });
    
    const char* back = "ESC - Menu";
    int bkW = MeasureText(back, 18);
    DrawText(back, (SW - bkW) / 2, by + 240, 18, (Color){ 140, 140, 160, 255 });
}

void Game::DrawControls()
{
    DrawRectangle(0, 0, SW, SH, (Color){ 15, 15, 25, 255 });
    
    const char* title = "CONTROLS";
    int tw = MeasureText(title, 44);
    DrawText(title, (SW - tw) / 2, 80, 44, (Color){ 50, 220, 90, 255 });
    
    int y = 160;
    int ls = 28;
    
    DrawText("WASD / Arrow Keys - Move", 200, y, ls, (Color){ 200, 200, 220, 255 }); y += 45;
    DrawText("ESC - Pause / Menu", 200, y, ls, (Color){ 200, 200, 220, 255 }); y += 45;
    DrawText("SPACE/ENTER - Select / Retry", 200, y, ls, (Color){ 200, 200, 220, 255 }); y += 60;
    
    DrawLine(200, y, SW - 200, y, (Color){ 50, 220, 90, 100 }); y += 30;
    
    DrawText("POWER-UPS:", 200, y, 24, (Color){ 140, 140, 180, 255 }); y += 40;
    
    DrawCircle(220, y - 5, 8, (Color){ 50, 150, 255, 255 });
    DrawText("Time Crystal - Slow down time", 240, y - 14, 22, (Color){ 180, 180, 200, 255 }); y += 40;
    
    DrawCircle(220, y - 5, 8, (Color){ 200, 80, 255, 255 });
    DrawText("Ghost Orb - Pass through echoes", 240, y - 14, 22, (Color){ 180, 180, 200, 255 }); y += 60;
    
    DrawLine(200, y, SW - 200, y, (Color){ 50, 220, 90, 100 }); y += 30;
    
    DrawText("TIME ECHO MECHANIC:", 200, y, 20, (Color){ 140, 140, 180, 255 }); y += 35;
    
    const char* desc = "As you move, ghostly echoes of your past";
    DrawText(desc, 200, y, 18, (Color){ 160, 160, 180, 255 }); y += 25;
    const char* desc2 = "positions become obstacles. Avoid them!";
    DrawText(desc2, 200, y, 18, (Color){ 160, 160, 180, 255 }); y += 25;
    const char* desc3 = "Ghost Orb lets you phase through them.";
    DrawText(desc3, 200, y, 18, (Color){ 160, 160, 180, 255 }); y += 50;
    
    const char* back = "Press ESC / SPACE / ENTER to return";
    int bk = MeasureText(back, 16);
    DrawText(back, (SW - bk) / 2, SH - 50, 16, (Color){ 100, 100, 120, 255 });
}
