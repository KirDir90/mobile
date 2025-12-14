#pragma once
#include "base.h"
#include "containers.h"



class World {
private:
	std::shared_ptr<BulletContainer> bullet_container;
	std::shared_ptr<UnitContainer> unit_container;
	std::shared_ptr<AsteroidContainer> asteroid_container;
	std::shared_ptr<Player> player;
	cpVect pos_red = { 0, 0 };
	cpVect pos_green = { 0, 0 };
	cpVect pos_blue = { 0, 0 };
	float fault = 0.f;
	int miss_red = 0, miss_green = 0, miss_blue = 0;
	int count_red = 0, count_green = 0, count_blue = 0;
	bool has_red = false, has_green = false, has_blue = false;
	float update_time = 1.f / 90.f;
	float accumulator = 0.0f;
	float last_accumulator = 0.0f;
	Vector2 battlefield_size = { 20000.f, 20000.f };
	cpSpace* space;
	bool activated = false;
	void CreateWorld(int teamSize, std::vector<cpVect> spawnPoints, int fault, Vector2 battlefield);
public:
	World();
	~World() = default;
	bool IsActivated() { return activated; }
	void Activate(SceneState state);
	void Deactivate() { Clear(); activated = false; }
	void AddElement(std::shared_ptr<Object> object);
	void AddMissingUnits();
	std::shared_ptr<Unit> CreateUnit(Team team);
	std::vector<std::shared_ptr<Unit>> GetUnits() { return unit_container->GetElements(); }
	std::vector<std::shared_ptr<Bullet>> GetBullets() { return bullet_container->GetElements(); }
	std::vector<std::shared_ptr<Asteroid>> GetAsteroids() { return asteroid_container->GetElements(); }
	std::shared_ptr<Player> GetPlayer() { return player; };
	void SearchForNewPlayer(Team team);
	Vector2 GetBattlefieldSize() { return battlefield_size; }
	bool HasTeam(Team team);
	void RemoveElement(Object* object);
	float GetAccumulator() { return accumulator; }
	float GetLastAccumulator() { return last_accumulator; }
	float GetUpdateTime() { return update_time; }
	int GetUnitsCount(Team team);

	void MissUnit(Team team);

	void Clear();
	void Step(float dt);
	void Render();
};

class Background {
protected:
	std::vector<std::vector<int> > tile_id;
	std::shared_ptr<Texture2D> stars_background;
	Vector2 offset;
	int tiles_count;
	float tile_size;
	float real_tile_size;
	float move_coeff;
	float height = 1024.f;
	float width = 1024.f;
	float max_size = 300.f;

public:
	Background(float move_coeff, float tiles_count, float real_tile_size, BackgroundType type);
	void Draw();
	void Update();
	char GetTileIndex(int x, int y, int seed);

};

class GeneratedBackground : public Background {
protected:
	
public:
	Texture2D GenerateSpaceTexture();
	void DrawSpaceGradient();
	void DrawNebula(Vector2 center, float size, Color baseColor, int type);
	void GenerateNebula(std::vector<Vector2>& positions, std::vector<float>& sizes,
		std::vector<Color>& colors, std::vector<int>& types, int count);
};

