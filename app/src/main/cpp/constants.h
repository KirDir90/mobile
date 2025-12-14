#pragma once
class GameConstants {
public:
    GameConstants() = delete;

    // Common physics
    static constexpr float SHIP_MAX_SPEED = 500.0f;
    static constexpr float SHIP_ACCELERATION = 8000.0f;
    static constexpr float SHIP_BRAKE_FORCE = 4000.0f;
    static constexpr float VELOCITY_RESIST_COEFF = .05f;
    static constexpr float VELOCITY_RESIST_COEFF_STATIC = .001f;


    //SpaceShip power
    static constexpr float FIGHTER_POWER = 60000.0f;
    static constexpr float RIPPER_POWER = 40000.0f;
    static constexpr float SCOUT_POWER = 30000.0f;
    static constexpr float FIGHTER_TORQUE = 75000.0f;
    static constexpr float RIPPER_TORQUE = 125000.0f;
    static constexpr float SCOUT_TORQUE = 250000.0f;

    // SpaceShip health
    static constexpr float FIGHTER_HEALTH = 100.0f;
    static constexpr float RIPPER_HEALTH = 150.0f;
    static constexpr float SCOUT_HEALTH = 500.0f;

    //SpaceShip weapon
    static constexpr float FIGHTER_RELOAD = .3f;
    static constexpr float RIPPER_RELOAD = .3f;
    static constexpr float SCOUT_RELOAD = .5f;
    static constexpr float FIGHTER_DAMAGE = 20.f;
    static constexpr float RIPPER_DAMAGE = 30.f;
    static constexpr float SCOUT_DAMAGE = 100.f;

    // Common weapon
    static constexpr float BULLET_SIZE = 2.f;
    static constexpr float BULLET_SPEED = 3000.0f;
    static constexpr float BULLET_LIFETIME = 1.5f;

    // Team
    static constexpr int MAX_UNIT_ON_FIELD = 15;

    // Other
    static constexpr float PARTICLE_SCALE = 1.0f;
    static constexpr float PARTICLE_COUNT = 30.0f;
    static constexpr float BACKGROUND_COUNT = 4;
    static constexpr float MIN_WORLD_UPDATE_TIME = 1.f / 120.f;
    static constexpr float FREE_MODE_WORLD_WIDTH = 20000.f;
    static constexpr float FREE_MODE_WORLD_HEIGHT = 20000.f;
    static constexpr float TEAM_MODE_WORLD_WIDTH = 10000.f;
    static constexpr float TEAM_MODE_WORLD_HEIGHT = 10000.f;
    static constexpr float ASTEROID_COUNT = 15;

    // Camera
    static constexpr float CAMERA_ZOOM_MAX = 3.0f;
    static constexpr float CAMERA_ZOOM_MIN = .4f;
    static constexpr float CAMERA_SPEED_MAX = 50000.f;
    static constexpr float CAMERA_SPEED_MIN = 10.f;
    static constexpr float CAMERA_MOVE_COEFF = .01f;
    

};