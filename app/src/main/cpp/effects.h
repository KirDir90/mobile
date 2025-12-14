#pragma once
#include "base.h"

class OptimizedEngineEffect {
private:
    static const int MAX_PARTICLES = GameConstants::PARTICLE_COUNT;
    Vector2 positions[MAX_PARTICLES];
    Vector2 velocities[MAX_PARTICLES];
    float lives[MAX_PARTICLES];
    float sizes[MAX_PARTICLES];
    int count = 0;

    void AddParticle(Vector2 shipPos, float shipAngle, float shipSpeed, float shipSize);
    void AddExplosionParticle(Vector2 shipPos, float explosionPower);
    void RemoveParticle(int index);

public:
    OptimizedEngineEffect() = default;

    
    void Update(Vector2 shipPos, float shipAngle, float shipSpeed, float shipSize, float dt, bool engineOn);
    void Explode(Vector2 shipPos);
    void UpdateExplosion(float dt);

    void Draw();
};