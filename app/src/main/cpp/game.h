#pragma once
#include "base.h"
#include "menu_templates.h"
#include "menu_creator.h"
#include "world.h"





class InterfaceManager {
private:
	InterfaceState current_interface;
	std::shared_ptr<Menu> mainMenu;
	std::shared_ptr<Menu> gameMenu;
	std::shared_ptr<Menu> optionMenu;
	std::shared_ptr<UIList> gameInterface;

public:
	InterfaceManager();
	void PrepairInterface();
	void ClearGameInterface();
	const std::vector<std::shared_ptr<UIElement>> GetOptionMenuItems();
	const std::vector<std::shared_ptr<UIElement>> GetGameRunningInterfaceItems();
	InterfaceState CurrentInterface() { return current_interface; }
	void SetInterface(InterfaceState interface);

	void Update();
	void Render();
};

class SettingsManager {

private:
    void SetDefaults();
	
    json m_settings;

public:
    SettingsManager();

    void LoadSettings(const std::string filename = "settings.json");
    void SaveSettings(const std::string filename = "settings.json");
	void UpdateSettings();

    // Геттеры
	float GetValue(std::string name);

    // Сеттеры
    void SetValue(std::string name, float value);
};

class SpaceShipInterface {
private:
	Vector2 aim_position;
	Color player_color;
	float aim_size;


public:
	SpaceShipInterface() = default;
	void PrepairSpaceShipInterface();
	void Update();
	void Render();

};

class MainCamera {
private:
	std::shared_ptr<Camera2D> main_camera;
	bool is_shaking;
	Vector2 velocity;
	Vector2 offset_velocity;
	Vector2 target_offset;
	float shake_duration = 0.0f;
	float shake_timer = 0.0f;
	float shake_intensity = 0.0f;

	// Для тряски
	Vector2 shake_offset;      // Текущее смещение от тряски
	//Vector2 shake_velocity;    // Скорость тряски
	float shake_decay;         // Коэффициент затухания
	float shake_trauma;        // Сила тряски (0-1)

public:
	MainCamera(Vector2 pos, Vector2 offset, float zoom);
	std::shared_ptr<Camera2D> GetCamera() { return main_camera; }
	void CalculateVelocity(Vector2 target_pos);
	Vector2 GetPosition() { return main_camera->target; }

	void SpanCamera(Vector2 field);
	void Update(float dt);
	void SetVelocity(Vector2 velo);
	void SetZoom(float zoom);
	void SetOffset(Vector2 offset);
	void Shake(Vector2 impulse);
	void Shake(float intensity, float duration);
};

class SceneManager {
private:
	SceneState current_scene;
	std::shared_ptr<World> world;
	std::shared_ptr<SpaceShipInterface> space_ship_interface;
	std::vector<std::shared_ptr<Background> > background;
	std::shared_ptr<MainCamera> camera;
	//Camera2D camera;

	Camera2D second_camera;
public:
	SceneManager();
	SceneState CurrentScene() { return current_scene; }
	void PrepairBackground(int background_count);
	void SetScene(SceneState scene);
	void AddElementToWorld(std::shared_ptr<Object> object) { world->AddElement(std::move(object)); }
	Camera2D& GetCamera() { return *camera->GetCamera(); }
	std::shared_ptr<MainCamera> GetMainCamera() { return camera; }

	Camera2D& GetSecondCamera() { return second_camera; }
	std::shared_ptr<World> GetWorld() { return world; }

	void Update(float dt);
	void Render();
};

class RecourseManager {
private:
	std::unordered_map<SpaceShipType, std::shared_ptr<Texture2D> > space_ship_textures;
	std::unordered_map<BackgroundType, std::shared_ptr<Texture2D> > space_background_textures;
	std::vector < std::shared_ptr<Texture2D> > asteroid_textures;
	std::shared_ptr<Texture2D> null_texture;

public:
	RecourseManager();
	std::shared_ptr<Texture2D> GetSpaceShipTexture(SpaceShipType type);
	std::shared_ptr<Texture2D> GetSpaceBackgroundTexture(BackgroundType type);
	std::shared_ptr<Texture2D> GetAsteroidTexture(int id);
};

class Game {
private:

	int seed = 1;
	bool should_update_window = false;

	std::shared_ptr<InterfaceManager> interface;
	std::shared_ptr<SceneManager> scene;
	std::shared_ptr<SettingsManager> settings;
	std::shared_ptr<RecourseManager> resource;
	

public:
	Game();
	void Step(float dt);
	void Render();
	void InitializeInterface();
	void UpdateSettings();
	void UpdatePlayerHp();
	void UpdatePlayerScore(float scale);
	void UpdateUnitsCount(Team team);
	void SetPlayerScore(float scale);
	void SearchForWinner();
	int GenerateSeed();
	int GetSeed() { return seed; }
	void ShouldUpdateWindow(bool f) { should_update_window = f; }
	

	InterfaceState GetInterfaceState();
	SceneState GetSceneState();
	Camera2D& GetCamera() { return scene->GetCamera(); }
	Camera2D& GetSecondCamera() { return scene->GetSecondCamera(); }
	float GetSetting(std::string name);
	bool IsWorldActivated() { return scene->GetWorld()->IsActivated(); }
	const std::vector<std::shared_ptr<UIElement>> GetOptionMenuItems();
	void UpdateWindow();


	void SetInterface(InterfaceState newInterface);
	void GoToPreviousInterfaceState();
	void SetScene(SceneState newScene);
	std::shared_ptr<SceneManager> GetScene();
	std::shared_ptr<InterfaceManager> GetInterfaceManager();
	std::shared_ptr<RecourseManager> GetResource() { return resource; }
	void AddElementToWorld(std::shared_ptr<Object> object) { scene->AddElementToWorld(std::move(object)); }
};

extern Game game;
