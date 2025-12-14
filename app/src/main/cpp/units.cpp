#include "units.h"
#include "game.h"

Unit::Unit(Team team, SpaceShipType type, Vector2 pos, cpSpace* space) : m_team(team), m_type(type)
{
	player = false;
	float hp = 100;
	float damage = 20;
	float pwr = 100000;
	float trqStr = 300000;
	float rel = 0.3;

	std::vector<cpVect> verts;

	switch (m_type) {
	case SpaceShipType::FIGHTER:
		verts = fighterVertices;
		hp = GameConstants::FIGHTER_HEALTH;
		damage = GameConstants::FIGHTER_DAMAGE;
		pwr = GameConstants::FIGHTER_POWER;
		trqStr = GameConstants::FIGHTER_TORQUE;
		rel = GameConstants::FIGHTER_RELOAD;

		break;
	case SpaceShipType::RIPPER:
		verts = ripperVertices;
		hp = GameConstants::RIPPER_HEALTH;
		damage = GameConstants::RIPPER_DAMAGE;
		pwr = GameConstants::RIPPER_POWER;
		trqStr = GameConstants::RIPPER_TORQUE;
		rel = GameConstants::RIPPER_RELOAD;
		break;
	case SpaceShipType::SCOUT:
		verts = scoutVertices;
		hp = GameConstants::SCOUT_HEALTH;
		damage = GameConstants::SCOUT_DAMAGE;
		pwr = GameConstants::SCOUT_POWER;
		trqStr = GameConstants::SCOUT_TORQUE;
		rel = GameConstants::SCOUT_RELOAD;
		break;
	}

	Color color = RED;
	switch (team) {
	case Team::TEAM_RED:
		color = RED;
		break;
	case Team::TEAM_BLUE:
		color = BLUE;
		break;
	case Team::TEAM_GREEN:
		color = GREEN;
		break;
	default:
		color = RED;
	}

	m_ship = std::make_shared<SpaceShip>(pos.x, pos.y, verts, 0.01f, space, hp, damage, rel, pwr, trqStr);
	m_ship->RotateAt(GetRandomValue(-180, 180), 1);
	
	m_ship->SetColor(color);
	
	m_ship->SetTexture(game.GetResource()->GetSpaceShipTexture(m_type).get());
}

void Unit::Step(float dt)
{
	if (m_ship->IsAlive()) {
		Vector2 battlefield = game.GetScene()->GetWorld()->GetBattlefieldSize();

		if (m_ship->GetPosition().x > battlefield.x ||
			m_ship->GetPosition().x < -battlefield.x ||
			m_ship->GetPosition().y > battlefield.y ||
			m_ship->GetPosition().y < -battlefield.y) {
			m_ship->RotateTo(0, 0, dt);
			m_ship->Accelerate(dt);
		}
		else {
			UpdateBehaviour(dt);
		}

	}
	UpdateShip(dt);
}

void Unit::DrawShip()
{
	m_ship->Draw();
}

void Unit::UpdateShip(float dt)
{
	m_ship->Update(dt);
}

void Unit::UpdateBehaviour(float dt)
{
	decision_timer -= dt;
	switch (current_behaviour) {
	case Behaviour::SEARCH_FOR_TARGET:
		SearchForTarget(dt);
		break;
	case Behaviour::FOLLOW_TARGET:
		FollowTarget(dt);
		break;
	case Behaviour::ATTACK_TARGET:
		AttackTarget(dt);
		break;
	case Behaviour::AVOID_DANGER:
		AvoidDanger(dt);
		break;
	case Behaviour::PATROL_ZONE:
		PatrolZone(dt);
		break;
	}
	if (decision_timer <= 0) decision_timer = 0.001f * GetRandomValue(4000, 7000);
}

void Unit::SearchForTarget(float dt)
{
	if (decision_timer > 0 && current_target) return;
	// Поиск ближайшего врага
	auto enemies = game.GetScene()->GetWorld()->GetUnits();
	std::shared_ptr<Unit> nearest_enemy = nullptr;
	float min_distance = std::numeric_limits<float>::max();

	for (auto& unit : enemies) {
		if (!unit || unit->GetTeam() == GetTeam() || !unit->GetShip()->IsAlive())
			continue;

		float dist = Vector2Distance(GetPosition(), cpVectToVector2(unit->GetShip()->GetPosition()));
		if (dist < min_distance) {
			min_distance = dist;
			nearest_enemy = unit;
		}
	}

	if (nearest_enemy && min_distance < 4000.0f) {
		current_target = nearest_enemy;
		current_behaviour = Behaviour::FOLLOW_TARGET;
	}
	else {
		// Если врагов нет, переходим к патрулированию
		following_timer -= dt;
		if (following_timer <= 0) {
			following_timer = 5.0f;
			current_behaviour = Behaviour::PATROL_ZONE;
		}
		else {
			current_target = nearest_enemy;
			current_behaviour = Behaviour::FOLLOW_TARGET;
		}
	}
}

void Unit::FollowTarget(float dt)
{
	if (!current_target || !current_target->GetShip()->IsAlive()) {
		current_behaviour = Behaviour::SEARCH_FOR_TARGET;
		return;
	}

	Vector2 target_pos = cpVectToVector2(current_target->GetShip()->GetPosition());
	m_ship->RotateTo(target_pos.x, target_pos.y, dt);
	m_ship->Accelerate(dt);

	float distance = Vector2Distance(GetPosition(), target_pos);


	// Если достаточно близко - атакуем
	if (distance < (GameConstants::BULLET_SPEED)*GameConstants::BULLET_SPEED) {
		current_behaviour = Behaviour::ATTACK_TARGET;
	}

	// Если цель слишком далеко - теряем её
	if (distance > 2000.0f) {
		current_target = nullptr;
		current_behaviour = Behaviour::SEARCH_FOR_TARGET;
	}
}

void Unit::AttackTarget(float dt)
{
	if (!current_target || !current_target->GetShip()->IsAlive()) {
		current_behaviour = Behaviour::SEARCH_FOR_TARGET;
		return;
	}

	Vector2 target_pos = cpVectToVector2(current_target->GetShip()->GetPosition());

	// Маневрирование вокруг цели
	float circle_distance = 500.0f;
	
	float distance = Vector2Distance(GetPosition(), target_pos);
	m_ship->RotateTo(target_pos.x, target_pos.y, dt);

	if (distance < circle_distance)
	{
		float angle = GetTime() * 2.0f; // Движение по кругу
		Vector2 maneuver_point = {
			target_pos.x + cos(angle) * circle_distance,
			target_pos.y + sin(angle) * circle_distance
		};
		m_ship->RotateTo(maneuver_point.x, maneuver_point.y, dt);
	}

	
	m_ship->Accelerate(dt);

	float target_angle = cpvtoangle(Vector2TocpVect(target_pos) - m_ship->GetPosition());
	if (Span(m_ship->GetRotation(), target_angle - .1f, target_angle + .1f) == m_ship->GetRotation()) {
		m_ship->Fire();
	}

	if (distance > (GameConstants::BULLET_SPEED) * GameConstants::BULLET_SPEED) {
		current_behaviour = Behaviour::FOLLOW_TARGET;
	}
}

void Unit::AvoidDanger(float dt)
{
	// Движение в противоположную сторону от опасности
	Vector2 current_pos = GetPosition();
	Vector2 escape_direction = Vector2Normalize(Vector2Subtract(current_pos, danger_position));
	Vector2 escape_point = Vector2Add(current_pos, Vector2Scale(escape_direction, 400.0f));

	m_ship->RotateTo(escape_point.x, escape_point.y, dt);
	m_ship->Accelerate(dt);

	// Через некоторое время возвращаемся к поиску
	patrol_timer -= dt;
	if (patrol_timer <= 0) {
		current_behaviour = Behaviour::SEARCH_FOR_TARGET;
		patrol_timer = 3.0f; // 3 секунды уклонения
	}
}

void Unit::PatrolZone(float dt)
{
	// Обновление точки патрулирования
	patrol_timer -= dt;
	if (patrol_timer <= 0 || Vector2Distance(GetPosition(), patrol_point) < 50.0f) {
		//Vector2 battlefield = game.GetScene()->GetWorld()->GetBattlefieldSize();
		patrol_point = {
			float(GetRandomValue(-400, 400)) + GetPosition().x,
			float(GetRandomValue(-400, 400)) + GetPosition().y
		};
		patrol_timer = 5.0f; // Менять точку каждые 5 секунд
	}

	m_ship->RotateTo(patrol_point.x, patrol_point.y, dt);

	// Движение с перерывами
	if (GetRandomValue(0, 100) < 70) {
		m_ship->Accelerate(dt);
	}

	// Периодическая проверка на врагов
	if (GetRandomValue(0, 200) == 0) {
		current_behaviour = Behaviour::SEARCH_FOR_TARGET;
	}
}

void Unit::SetDangerPosition(Vector2 position)
{
	danger_position = position;
	current_behaviour = Behaviour::AVOID_DANGER;
	patrol_timer = 3.0f;
}



void Player::TakeControl(float dt)
{
	if (!m_ship->IsAlive()) {
		UpdateShip(dt);
		return;
	}
	Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), game.GetCamera());
	m_ship->RotateTo(mousePos.x, mousePos.y, dt);

	if (IsKeyDown(KEY_W)) {
		m_ship->Accelerate(dt);
	}
	if (IsKeyDown(KEY_S)) {
		m_ship->SlowDown(dt);
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		m_ship->Fire();
	}
	UpdateShip(dt);
	//std::cout << m_ship->GetPosition().x << "\t" << m_ship->GetPosition().y << "\n";
}
