#pragma once
#include "base.h"
#include "effects.h"
#include "forward.h"



class DynamicObject : public Object{
protected:
	cpBody* body = nullptr;
	cpShape* circle_shape = nullptr;
	cpShape* poly_shape = nullptr;
	float size = 100;
	void SafeDestroyPhysics();

public:
	DynamicObject() {};
	virtual ~DynamicObject() { SafeDestroyPhysics(); }
	bool IsActive() { return !(body == nullptr || poly_shape == nullptr); }
	void ApplyForce(float x, float y, float dt);
	void ApplyImpulse(float x, float y, float dt);
	void RotateAt(float diretion, float dt);
	void LookAt(float x, float y, float dt);
	void SetSize(float size) { this->size = size; }
	
	cpShape* GetShape();
	cpShape* GetPolyShape();
	cpShape* GetCircleShape();
	cpSpace* GetSpace();
	cpVect GetPosition();
	float GetRotation();

	void Draw() {};
};

class DynamicPolygon : public DynamicObject {
protected:
	OptimizedEngineEffect engineParticles;
	std::vector<cpVect> vertices = {};
	std::vector<Vector2> points = {};
	cpVect middle;
	Color color = RED;
	Texture2D* texture;
public:
	DynamicPolygon(float x, float y, std::vector<cpVect> vertices, float dencity, cpSpace* space);
	virtual ~DynamicPolygon() override = default;
	const std::vector<Vector2>& GetVertices();
	float GetSize() { return size; }
	Vector2 GetMiddle();
	void RotateToPointWithTorque(cpVect targetPoint, float torque_strength, float dt);
	void UpdateVelocity(float dt);
	virtual void VelocityResist(float dt);
	void SetTexture(Texture2D* tex);
	void SetColor(Color new_color) { color = new_color; }
	virtual void Update(float dt);
	void DrawMyTexture(Texture2D* texture, cpVect position, float angleRadians, Color tint = WHITE);
	void Draw();
};

class Asteroid : public DynamicPolygon {
protected:
	float size_coeff;
public:
	Asteroid(float x, float y, float dencity, float size_c, cpSpace* space);
};

class SpaceShip : public DynamicPolygon, public std::enable_shared_from_this<SpaceShip>{
protected:
	float health;
	float power;
	float torque_strength;
	float explosion_timer = 1.f;
	float damage;
	float reload = 0.3f;
	float last_shot = 0.0f;
	bool alive = true;
	bool engineOn = false;
	Vector2 death_point = { 0, 0 };
	std::shared_ptr<Unit> my_pilot;

public:
	SpaceShip(float x, float y, std::vector<cpVect> vertices, float dencity,
		cpSpace* space, float hp = 100, float dmg = 20, float rel = 0.3f, float pwr = 100000, float torqueStrength = 300000);
	~SpaceShip() override = default;
	bool IsAlive() { return health > 0; }
	float GetHp() { return health; }
	std::shared_ptr<Unit> GetPilot();
	void TakeDamage(float damage);
	void SetPilot(std::shared_ptr<Unit> pilot) { my_pilot = pilot; }
	void VelocityResist(float dt) override;
	float GetExplosionTime() { return explosion_timer; }
	void Update(float dt) override;
	void Accelerate(float dt);
	void SlowDown(float dt);
	void RotateTo(float x, float y, float dt);
	void Fire();
	void Draw();
	
};

class Bullet : public Object {
private:
	float size = 5;
	float damage;
	bool hit = false;
	float live_time = 1.5f;
	float check_time = 0.013333f;
	float last_check = 0;
	Vector2 position;
	Vector2 last_check_pos;
	Vector2 velocity;
	Color color = RED;
	std::shared_ptr<SpaceShip> parent;
public:
	Bullet(float damage, Vector2 position, Vector2 velocity, std::shared_ptr<SpaceShip> ptr, Color col);
	std::shared_ptr<SpaceShip> GetParent() { return parent; }
	void Update(float dt);
	bool IsActive();
	void Draw();
};
