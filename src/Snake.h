#pragma once
#include "raylib.h"
#include <deque>
#include <vector>

enum class Dir { UP, DOWN, LEFT, RIGHT, NONE };

class Snake {
public:
    Snake();
    
    void SetDir(Dir d);
    Dir GetDir() const;
    Dir GetNextDir() const;
    
    void Move();
    void Grow();
    
    bool HitSelf() const;
    bool HitWall(int gw, int gh) const;
    
    Vector2 Head() const;
    const std::deque<Vector2>& Body() const;
    int Length() const;
    
    void Reset();
    
    void Draw(int ox, int oy, int cs) const;
    
private:
    std::deque<Vector2> body;
    Dir dir;
    Dir nextDir;
    bool growFlag;
};
