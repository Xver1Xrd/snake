#pragma once
#include "raylib.h"
#include "Snake.h"
#include <vector>
#include <cstdlib>

enum class GState { MENU, PLAYING, GAME_OVER, CONTROLS };

enum class FoodType { NORMAL, TIME_CRYSTAL, GHOST_ORB };

struct FoodItem {
    Vector2 pos;
    FoodType type;
    float bobPhase;
};

struct EchoObstacle {
    Vector2 pos;
    float alpha;
    float pulse;
};

struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color col;
    float life;
    float maxLife;
};

class Game {
public:
    Game();
    void HandleInput();
    void Update();
    void Draw();
    
    void SaveHighScore();
    void LoadHighScore();
    
    static const int CELL_SZ = 28;
    static const int GW = 22;
    static const int GH = 20;
    static const int SW = 900;
    static const int SH = 700;
    
    int GetOX() const { return (SW - GW * CELL_SZ) / 2; }
    int GetOY() const { return 60; }
    
private:
    void StartGame();
    void SpawnFood();
    void SpawnFoodOfType(FoodType t);
    void DoEchoes(float dt);
    void DoParticles(float dt);
    void BurstParticles(Vector2 pos, Color col, int n);
    void DrawMenu();
    void DrawPlaying();
    void DrawGameOver();
    void DrawControls();
    
    GState state;
    Snake snake;
    
    std::vector<FoodItem> foods;
    std::vector<EchoObstacle> echoes;
    std::vector<Particle> particles;
    
    int score;
    int highScore;
    int foodEaten;
    int spcCount;
    
    float moveTimer;
    float moveInterval;
    float echoSpawnTimer;
    float timeSlowLeft;
    float ghostLeft;
    float difficultyTimer;
    float screenShake;
    
    int menuOpt;
    float menuGlow;
};
