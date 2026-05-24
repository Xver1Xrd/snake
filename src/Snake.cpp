#include "Snake.h"
#include <cmath>

Snake::Snake()
    : dir(Dir::RIGHT)
    , nextDir(Dir::RIGHT)
    , growFlag(false)
{
    Reset();
}

void Snake::SetDir(Dir d)
{
    if ((dir == Dir::UP && d == Dir::DOWN) ||
        (dir == Dir::DOWN && d == Dir::UP) ||
        (dir == Dir::LEFT && d == Dir::RIGHT) ||
        (dir == Dir::RIGHT && d == Dir::LEFT))
        return;
    nextDir = d;
}

Dir Snake::GetDir() const { return dir; }
Dir Snake::GetNextDir() const { return nextDir; }

void Snake::Move()
{
    dir = nextDir;
    
    Vector2 newHead = body.front();
    
    switch (dir) {
        case Dir::UP:    newHead.y -= 1; break;
        case Dir::DOWN:  newHead.y += 1; break;
        case Dir::LEFT:  newHead.x -= 1; break;
        case Dir::RIGHT: newHead.x += 1; break;
        default: break;
    }
    
    body.push_front(newHead);
    
    if (growFlag) {
        growFlag = false;
    } else {
        body.pop_back();
    }
}

void Snake::Grow()
{
    growFlag = true;
}

bool Snake::HitSelf() const
{
    Vector2 head = body.front();
    for (size_t i = 1; i < body.size(); i++) {
        if (body[i].x == head.x && body[i].y == head.y)
            return true;
    }
    return false;
}

bool Snake::HitWall(int gw, int gh) const
{
    Vector2 head = body.front();
    return head.x < 0 || head.x >= gw || head.y < 0 || head.y >= gh;
}

Vector2 Snake::Head() const { return body.front(); }

const std::deque<Vector2>& Snake::Body() const { return body; }

int Snake::Length() const { return (int)body.size(); }

void Snake::Reset()
{
    body.clear();
    body.push_back({ 5, 10 });
    body.push_back({ 4, 10 });
    body.push_back({ 3, 10 });
    dir = Dir::RIGHT;
    nextDir = Dir::RIGHT;
    growFlag = false;
}

void Snake::Draw(int ox, int oy, int cs) const
{
    int idx = 0;
    for (auto it = body.begin(); it != body.end(); ++it, idx++) {
        float t = (float)idx / (float)body.size();
        Color c;
        
        if (idx == 0) {
            c = (Color){ 50, 220, 90, 255 };
        } else {
            int r = (int)(50 + 80 * (1 - t));
            int g = (int)(220 - 80 * (1 - t));
            int b = (int)(90 - 40 * (1 - t));
            c = (Color){ (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
        }
        
        int x = ox + (int)it->x * cs;
        int y = oy + (int)it->y * cs;
        
        DrawRectangle(x + 1, y + 1, cs - 2, cs - 2, c);
        
        if (idx == 0) {
            DrawRectangleLinesEx(
                (Rectangle){ (float)x, (float)y, (float)cs, (float)cs },
                2, (Color){ 30, 180, 70, 255 }
            );
            
            int cx = x + cs / 2;
            int cy = y + cs / 2;
            int eyeOff = cs / 5;
            int eyeSize = cs / 8;
            
            Color eyeC = { 255, 255, 255, 255 };
            Color pupilC = { 20, 20, 20, 255 };
            
            switch (dir) {
                case Dir::UP:
                    DrawCircle(cx - eyeOff, cy - 2, eyeSize, eyeC);
                    DrawCircle(cx + eyeOff, cy - 2, eyeSize, eyeC);
                    DrawCircle(cx - eyeOff, cy - 2, eyeSize / 2, pupilC);
                    DrawCircle(cx + eyeOff, cy - 2, eyeSize / 2, pupilC);
                    break;
                case Dir::DOWN:
                    DrawCircle(cx - eyeOff, cy + 2, eyeSize, eyeC);
                    DrawCircle(cx + eyeOff, cy + 2, eyeSize, eyeC);
                    DrawCircle(cx - eyeOff, cy + 2, eyeSize / 2, pupilC);
                    DrawCircle(cx + eyeOff, cy + 2, eyeSize / 2, pupilC);
                    break;
                case Dir::LEFT:
                    DrawCircle(cx - 2, cy - eyeOff, eyeSize, eyeC);
                    DrawCircle(cx - 2, cy + eyeOff, eyeSize, eyeC);
                    DrawCircle(cx - 2, cy - eyeOff, eyeSize / 2, pupilC);
                    DrawCircle(cx - 2, cy + eyeOff, eyeSize / 2, pupilC);
                    break;
                case Dir::RIGHT:
                    DrawCircle(cx + 2, cy - eyeOff, eyeSize, eyeC);
                    DrawCircle(cx + 2, cy + eyeOff, eyeSize, eyeC);
                    DrawCircle(cx + 2, cy - eyeOff, eyeSize / 2, pupilC);
                    DrawCircle(cx + 2, cy + eyeOff, eyeSize / 2, pupilC);
                    break;
                default: break;
            }
        }
    }
}
