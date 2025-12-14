#pragma once
#include "base.h"
#include "objects.h"
#include "spaceShips.h"

enum class Team {
	TEAM_RED,
	TEAM_BLUE,
	TEAM_GREEN
};

class Unit : public Object{
private:
	enum class Behaviour {
		SEARCH_FOR_TARGET,
		FOLLOW_TARGET,
		ATTACK_TARGET,
		AVOID_DANGER,
		PATROL_ZONE,
	};

    Behaviour current_behaviour = Behaviour::SEARCH_FOR_TARGET;
    std::shared_ptr<Unit> current_target = nullptr;
    Vector2 patrol_point = { 0, 0 };
    float patrol_timer = 0.0f;
	float following_timer = 5.0f;
	float decision_timer = .0f;
    Vector2 danger_position = { 0, 0 };

protected:
	std::shared_ptr<SpaceShip> m_ship;
	Team m_team;
	SpaceShipType m_type;
	bool player;
public:
	Unit(Team team, SpaceShipType type, Vector2 pos, cpSpace* space);
	bool IsPlayer() { return player; }
	void Step(float dt);
	void DrawShip();
	void UpdateShip(float dt);
	std::shared_ptr<SpaceShip> GetShip() { return m_ship; }
	Team GetTeam() { return m_team; };
	SpaceShipType GetSpaceShipType() { return m_type; }

	void UpdateBehaviour(float dt);

    void SearchForTarget(float dt);

    void FollowTarget(float dt);

    void AttackTarget(float dt);

    void AvoidDanger(float dt);

    void PatrolZone(float dt);

    // Вспомогательные методы
    void SetDangerPosition(Vector2 position);

    Vector2 GetPosition() {
        return cpVectToVector2(m_ship->GetPosition());
    }
};

class Player : public Unit {
public:
	void IdentificateAsPlayer() { player = true; }
	void TakeControl(float dt);
};
