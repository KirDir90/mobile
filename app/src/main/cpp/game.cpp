#include "game.h"

Game game;

InterfaceManager::InterfaceManager()
{
    /*current_interface = InterfaceState::MAIN_MENU;
    mainMenu = CreateMainMenu();
    optionMenu = CreateOptionMenu();
    gameMenu = CreateGameMenu();
    gameInterface = CreateGameRunningInterface();
    
    game.GetCamera().zoom = 0.5;
    game.GetCamera().offset = {1.0f * GetScreenWidth() / 2, 1.0f * GetScreenHeight() / 2};
    game.GetSecondCamera().zoom = 1;
    game.GetSecondCamera().offset = { 1.0f * GetScreenWidth() / 2, 1.0f * GetScreenHeight() / 2 };*/
    current_interface = InterfaceState::MAIN_MENU;

    PrepairInterface();
}

void InterfaceManager::PrepairInterface()
{
    mainMenu.reset();
    optionMenu.reset();
    gameMenu.reset();
    gameInterface.reset();

    //current_interface = InterfaceState::MAIN_MENU;
    mainMenu = CreateMainMenu();
    optionMenu = CreateOptionMenu();
    gameMenu = CreateGameMenu();
    gameInterface = CreateGameRunningInterface();

    game.GetCamera().zoom = 0.5;
    game.GetScene()->GetMainCamera()->SetOffset({ 1.0f * GetScreenWidth() / 2, 1.0f * GetScreenHeight() / 2 });
    game.GetSecondCamera().zoom = 1;
    game.GetSecondCamera().offset = { 1.0f * GetScreenWidth() / 2, 1.0f * GetScreenHeight() / 2 };
}

void InterfaceManager::ClearGameInterface()
{
    Color color;
    for (auto team : { Team::TEAM_BLUE, Team::TEAM_RED, Team::TEAM_GREEN }) {
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
        if (auto uiList = std::dynamic_pointer_cast<UIList>(
            GetGameRunningInterfaceItems()[0]))
        {
            if (auto uiList_2 = std::dynamic_pointer_cast<UIList>(
                uiList->GetElements()[1]))
            {
                if (auto uiList_3 = std::dynamic_pointer_cast<UIList>(
                    uiList_2->GetElements()[0]))
                {
                    for (auto item : uiList_3->GetElements())
                    {
                        if (auto uiLabel = std::dynamic_pointer_cast<UILabel>(item)) {
                            if (ColorToInt(item->GetTextColor()) == ColorToInt(color)) {
                                uiLabel->SetName("");
                            }
                        }
                    }
                }
            }
        }
    }
}

const std::vector<std::shared_ptr<UIElement>> InterfaceManager::GetOptionMenuItems()
{
    return optionMenu->GetElements();
}

const std::vector<std::shared_ptr<UIElement>> InterfaceManager::GetGameRunningInterfaceItems()
{
    return gameInterface->GetElements();
}

void InterfaceManager::SetInterface(InterfaceState interface) {
    current_interface = interface;

    if (interface == InterfaceState::GAME_RUNNING) {
        SceneState scene = game.GetSceneState();
        std::string name = scene == SceneState::TEAM_BATTLE ? "Team battle" :
            scene == SceneState::WAVE_MODE ? "Wave mode" :
            scene == SceneState::BOSS_FIGHT ? "Boss fight" :
            "Free mode";
        gameInterface->SetName(name);
        std::cout << name << "\n";
    }
}

void InterfaceManager::Update()
{
}

void InterfaceManager::Render()
{
    switch (game.GetInterfaceState()) {
    case InterfaceState::MAIN_MENU:
        mainMenu->Draw();
        return;
    case InterfaceState::OPTIONS_MENU:
        optionMenu->Draw();
        return;
    case InterfaceState::GAME_MENU:
        gameMenu->Draw();
        return;
    case InterfaceState::GAME_RUNNING:
        gameInterface->Draw();
        return;
    default:
        mainMenu->Draw();
        return;
    }
}



SettingsManager::SettingsManager() {
    LoadSettings("settings.json");
}

void SettingsManager::SetDefaults() {
    m_settings = {
        {"Max fps", 60},
        {"Team size", 30},
        {"Friendly fire", true},
        {"Windowed", false}
    };
    SaveSettings("settings.json");
}

void SettingsManager::LoadSettings(const std::string filename) {
    try {
        std::ifstream file(filename);
        if (file.good()) {
            m_settings = nlohmann::json::parse(file);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load settings: " << e.what() << std::endl;
        SetDefaults();
    }
}

void SettingsManager::SaveSettings(const std::string filename) {
    try {
        std::ofstream file(filename);
        file << m_settings.dump(4);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save settings: " << e.what() << std::endl;
    }
}

void SettingsManager::UpdateSettings() {

    const std::vector<std::shared_ptr<UIElement>> elements = game.GetOptionMenuItems();

    for (auto& item : elements) {

        if (&item) {
            if (UICheckbox* chck = dynamic_cast<UICheckbox*>(item.get())) {
                m_settings[chck->GetName()] = chck->GetValue();
            }
            else if (UISlider* sldr = dynamic_cast<UISlider*>(item.get())) {
                m_settings[sldr->GetName()] = (int)std::floor(sldr->GetValue());
            }
        }
    }
    std::cout << "saved settings\n";
    SaveSettings("settings.json");
}

float SettingsManager::GetValue(std::string name) {
    if (m_settings.find(name) != m_settings.end()) {
        return m_settings[name];
    }
    else {
        std::cout << "Invalid setting " + name + "\n";
    }
}

void SettingsManager::SetValue(std::string name, float value){
    try {
        m_settings[name] = value;
    }
    catch (std::exception e) {}
}



SceneManager::SceneManager()
{
    current_scene = SceneState::MENU_MODE;
    world = std::make_shared<World>();
    space_ship_interface = std::make_shared<SpaceShipInterface>();
    camera = std::make_shared<MainCamera>(Vector2{ 0.f, 0.f }, Vector2{ 0.f, 0.f }, 1.f);
    second_camera = Camera2D();
    second_camera.zoom = 1.f;
    
}

void SceneManager::PrepairBackground(int background_count)
{
    int index = background_count - 1;
    for (auto& back : background) {
        back.reset();
        if (index == background_count - 1) {
            back = std::make_shared<Background>(1.0f / background_count * (background_count - index), 4, 1024, BackgroundType::COLORED);
        }
        else {
            back = std::make_shared<Background>(1.0f / background_count * (background_count - index), 4, 2048, BackgroundType::COMMON);
        }
        index -= 1;
    }
    if (background.size() == 0) {
        for (int i = 1; i <= background_count; i++) {
            index = background_count - i;
            if (i == 1) {
                background.push_back(std::make_shared<Background>(1.0f / background_count * (i), 8, 512, BackgroundType::COLORED));
            }
            else {
                background.push_back(std::make_shared<Background>(1.0f / background_count * (i), 4, 1024, BackgroundType::COMMON));
            }
        }
    }
}

void SceneManager::SetScene(SceneState scene)
{
    current_scene = scene;
    world->Activate(scene);

    PrepairBackground(GameConstants::BACKGROUND_COUNT);
    space_ship_interface->PrepairSpaceShipInterface();
}

void SceneManager::Update(float dt) {

    if (IsKeyPressed(KEY_ESCAPE)) {
        std::cout << "escape\n";
        game.GoToPreviousInterfaceState();
    }

    if (current_scene != SceneState::MENU_MODE)
    {

        world->Step(dt);

        auto player = world->GetPlayer();
        bool alive = player->GetShip()->IsAlive();
        
        Vector2 pos = camera->GetPosition();

        if (alive) {
            pos = player->GetShip()->GetMiddle();
        }

        if (IsKeyDown(KEY_Q)) {

            Vector2 player_pos = camera->GetPosition();
            if (alive) {
                player_pos = player->GetShip()->GetMiddle();
            }
            Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), *camera->GetCamera());

            if (alive) {
                pos = { (player_pos.x + mouse_pos.x) * 0.5f, (player_pos.y + mouse_pos.y) * 0.5f };
            }
            else {
                pos = { (camera->GetPosition().x + mouse_pos.x) * 0.5f, (camera->GetPosition().y + mouse_pos.y) * 0.5f };
            }

        }
        
        

        if (world->GetAccumulator() == dt) {

            camera->CalculateVelocity(pos);
            camera->Update(world->GetLastAccumulator());

            Vector2 battlefield = world->GetBattlefieldSize();
            camera->SpanCamera(battlefield);


            second_camera.target = pos;
        }
        space_ship_interface->Update();


        for (auto& back : background) {
            back->Update();
        }

    }
}

void SceneManager::Render() {

    if (current_scene != SceneState::MENU_MODE)
    {
        Vector2 battlefield = world->GetBattlefieldSize();

        BeginMode2D(*camera->GetCamera());
        for (auto& back : background) {
            back->Draw();
        }


        DrawRectangleLines(-battlefield.x, -battlefield.y, 2 * battlefield.x, 2 * battlefield.y, RED);
        world->Render();
        if (world->GetPlayer()->GetShip()->IsAlive()) {
            space_ship_interface->Render();
        }

        EndMode2D();
        
    }
}



void SpaceShipInterface::PrepairSpaceShipInterface()
{
    if (!game.GetScene()->GetWorld()->IsActivated()) return;
    Team team = game.GetScene()->GetWorld()->GetPlayer()->GetTeam();

    switch (team) {
    case Team::TEAM_RED:
        player_color = RED;
        break;
    case Team::TEAM_BLUE:
        player_color = BLUE;
        break;
    case Team::TEAM_GREEN:
        player_color = GREEN;
        break;
    default:
        player_color = RED;
    }
    aim_size = 30.f;
}

void SpaceShipInterface::Update() {
    Vector2 player_pos = game.GetScene()->GetWorld()->GetPlayer()->GetPosition();
    Vector2 player_direction_norm = Vector2Normalize(cpVectToVector2(cpvforangle(game.GetScene()->GetWorld()->GetPlayer()->GetShip()->GetRotation())));
    float dist = Vector2Distance(player_pos, GetScreenToWorld2D(GetMousePosition(), game.GetScene()->GetCamera()));
    aim_position = { player_pos.x + player_direction_norm.x * dist, player_pos.y + player_direction_norm.y * dist };
}

void SpaceShipInterface::Render() {
    //std::cout << "render aim \n";
    DrawCircleLinesV(aim_position, aim_size * 0.5f, player_color);
}



Game::Game()
{
    settings = std::make_shared<SettingsManager>();
    scene = std::make_shared<SceneManager>();
    seed = GenerateSeed();
}

void Game::Step(float dt)
{
    if (should_update_window) UpdateWindow();
    scene->Update(dt);
}

void Game::Render() {
    scene->Render();
    interface->Render();
}

void Game::InitializeInterface()
{
    interface = std::make_shared<InterfaceManager>();
    resource = std::make_shared<RecourseManager>();
}

InterfaceState Game::GetInterfaceState()
{
    return interface->CurrentInterface();
}

SceneState Game::GetSceneState()
{
    return scene->CurrentScene();
}

const std::vector<std::shared_ptr<UIElement>> Game::GetOptionMenuItems() {
    return interface->GetOptionMenuItems();
}

void Game::UpdateWindow()
{

    if (GetSetting("Windowed")) {
        ClearWindowState(FLAG_FULLSCREEN_MODE);
        SetWindowState(FLAG_WINDOW_RESIZABLE);
        SetWindowState(FLAG_WINDOW_MAXIMIZED);
    }
    else {
        ClearWindowState(FLAG_WINDOW_RESIZABLE);
        ClearWindowState(FLAG_WINDOW_MAXIMIZED);

        // �������� ������� ��������
        int monitor = GetCurrentMonitor();
        int screenWidth = GetMonitorWidth(monitor);
        int screenHeight = GetMonitorHeight(monitor);

        // ������������� ������ ���� ��� ������ �����
        SetWindowSize(screenWidth, screenHeight);
        SetWindowState(FLAG_FULLSCREEN_MODE);

        // �������������: ������������� ���� � (0,0)
        SetWindowPosition(0, 0);
    }

    SetTargetFPS(GetSetting("Max fps") == 121 ? INT16_MAX : game.GetSetting("Max fps"));

    GuiLoadStyle("resources/style_cyber.rgs");

    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);          // ������� �������
    GuiSetStyle(DEFAULT, TEXT_PADDING, 10);         // ������� ������
    GuiSetStyle(DEFAULT, TEXT_SIZE, 1.0 / 32 * GetScreenWidth());

    //InitializeInterface();
    interface->PrepairInterface();
    ShouldUpdateWindow(false);
}

float Game::GetSetting(std::string name)
{
    return settings->GetValue(name);
}

void Game::SetInterface(InterfaceState newInterface)
{
    interface->SetInterface(newInterface);
}

void Game::GoToPreviousInterfaceState()
{
    SetScene(SceneState::MENU_MODE);
    InterfaceState state = game.GetInterfaceState();
    switch (state) {
    case InterfaceState::MAIN_MENU:
        SetInterface(InterfaceState::MAIN_MENU);
        break;
    case InterfaceState::GAME_MENU:
        SetInterface(InterfaceState::MAIN_MENU);
        break;
    case InterfaceState::OPTIONS_MENU:
        SetInterface(InterfaceState::MAIN_MENU);
        break;
    case InterfaceState::GAME_RUNNING:
        
        SetInterface(InterfaceState::GAME_MENU);
        break;
    }

}

void Game::SetScene(SceneState newScene)
{
    scene->SetScene(newScene);
}

std::shared_ptr<SceneManager> Game::GetScene()
{
    return scene;
}

std::shared_ptr<InterfaceManager> Game::GetInterfaceManager()
{
    return interface;
}

void Game::UpdateSettings() {
    settings->UpdateSettings();
    ShouldUpdateWindow(true);
}

void Game::UpdatePlayerHp()
{
    auto player = GetScene()->GetWorld()->GetPlayer();

    if (!player) return;

    if (auto uiList = std::dynamic_pointer_cast<UIList>(
        game.GetInterfaceManager()->GetGameRunningInterfaceItems()[0]))
    {
        if (auto uiList_2 = std::dynamic_pointer_cast<UIList>(
            uiList->GetElements()[0]))
        {
            for (auto item : uiList_2->GetElements())
            {
                if (auto uiLabel = std::dynamic_pointer_cast<UILabel>(item) ) {
                    if (item->GetName().find("Hp") != std::string::npos)
                        uiLabel->SetName("Hp " + std::to_string(int(std::ceil(GetScene()->GetWorld()->GetPlayer()->GetShip()->GetHp()))));
                }
            }
        }
    }
}

void Game::UpdatePlayerScore(float scale)
{

    auto player = GetScene()->GetWorld()->GetPlayer();

    if (!player) return;

    if (auto uiList = std::dynamic_pointer_cast<UIList>(
        game.GetInterfaceManager()->GetGameRunningInterfaceItems()[0]))
    {
        if (auto uiList_2 = std::dynamic_pointer_cast<UIList>(
            uiList->GetElements()[0]))
        {
            for (auto item : uiList_2->GetElements())
            {
                if (auto uiLabel = std::dynamic_pointer_cast<UILabel>(item)) {
                    if (item->GetName().find("Score") != std::string::npos) {
                        int last_score = extract_number(uiLabel->GetName());
                        uiLabel->SetName("Score " + std::to_string(int(scale + last_score)));
                    }
                }
            }
        }
    }
}

void Game::UpdateUnitsCount(Team team)
{
    Color color;

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
    if (auto uiList = std::dynamic_pointer_cast<UIList>(
        game.GetInterfaceManager()->GetGameRunningInterfaceItems()[0]))
    {
        if (auto uiList_2 = std::dynamic_pointer_cast<UIList>(
            uiList->GetElements()[1]))
        {
            if (auto uiList_3 = std::dynamic_pointer_cast<UIList>(
                uiList_2->GetElements()[0]))
            {
                for (auto item : uiList_3->GetElements())
                {
                    if (auto uiLabel = std::dynamic_pointer_cast<UILabel>(item)) {
                        if (ColorToInt(item->GetTextColor()) == ColorToInt(color)) {
                            int count = GetScene()->GetWorld()->GetUnitsCount(team);
                            uiLabel->SetName(std::to_string(count));
                            if (count == 0) SearchForWinner();
                        }
                    }
                }
            }
        }
    }
}

void Game::SetPlayerScore(float scale)
{
    if (auto uiList = std::dynamic_pointer_cast<UIList>(
        game.GetInterfaceManager()->GetGameRunningInterfaceItems()[0]))
    {
        if (auto uiList_2 = std::dynamic_pointer_cast<UIList>(
            uiList->GetElements()[0]))
        {
            for (auto item : uiList_2->GetElements())
            {
                if (auto uiLabel = std::dynamic_pointer_cast<UILabel>(item)) {
                    if (item->GetName().find("Score") != std::string::npos) {
                        
                        uiLabel->SetName("Score " + std::to_string(int(scale)));
                    }
                }
            }
        }
    }
}

void Game::SearchForWinner()
{
    int count_red = GetScene()->GetWorld()->GetUnitsCount(Team::TEAM_RED);
    int count_green = GetScene()->GetWorld()->GetUnitsCount(Team::TEAM_GREEN);
    int count_blue = GetScene()->GetWorld()->GetUnitsCount(Team::TEAM_BLUE);
    int count_total = count_red + count_blue + count_green;
    if (count_total == count_blue) {
        std::cout << "blue won!\n";
    }
    else if (count_total == count_red) {
        std::cout << "red won!\n";
    }
    else if (count_total == count_green) {
        std::cout << "green won!\n";
    }
    

}

int Game::GenerateSeed()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    SetRandomSeed((unsigned int)(millis));
    return GetRandomValue(0, INT_MAX);
}



RecourseManager::RecourseManager()
{
    std::string root = "";
    space_ship_textures.insert({ SpaceShipType::FIGHTER, std::make_unique<Texture2D>(LoadTexture((root + "space_ships/Fighter.png").c_str())) });
    if (space_ship_textures.contains(SpaceShipType::FIGHTER)) std::cout << "fighter loaded!\n";

    space_ship_textures.insert({ SpaceShipType::RIPPER, std::make_unique<Texture2D>(LoadTexture((root + "space_ships/Ripper.png").c_str())) });
    if (space_ship_textures.contains(SpaceShipType::RIPPER)) std::cout << "ripper loaded!\n";

    space_ship_textures.insert({ SpaceShipType::SCOUT, std::make_unique<Texture2D>(LoadTexture((root + "space_ships/Scout.png").c_str())) });
    if (space_ship_textures.contains(SpaceShipType::SCOUT)) std::cout << "scout loaded!\n";



    space_background_textures.insert({ BackgroundType::COMMON, std::make_unique<Texture2D>(LoadTexture((root + "space_background/space_background.png").c_str())) });
    if (space_background_textures.contains(BackgroundType::COMMON)) std::cout << "common background loaded!\n";

    space_background_textures.insert({ BackgroundType::COLORED, std::make_unique<Texture2D>(LoadTexture((root + "space_background/space_background_colored.png").c_str())) });
    if (space_background_textures.contains(BackgroundType::COLORED)) std::cout << "colored background loaded!\n";



    asteroid_textures.push_back(std::make_shared<Texture2D>(LoadTexture((root + "space_objects/tatooine_asteroid.png").c_str())));
    asteroid_textures.push_back(std::make_shared<Texture2D>(LoadTexture((root + "space_objects/hoth_asteroid.png").c_str())));

    null_texture = std::make_unique<Texture2D>(LoadTextureFromImage(GenImageColor(100, 100, YELLOW)));
}

std::shared_ptr<Texture2D> RecourseManager::GetSpaceShipTexture(SpaceShipType type)
{
    if (space_ship_textures.contains(type)) {
        return space_ship_textures.at(type);
    }
    return null_texture;
}

std::shared_ptr<Texture2D> RecourseManager::GetSpaceBackgroundTexture(BackgroundType type)
{
    if (space_background_textures.contains(type)) {
        return space_background_textures.at(type);
    }
    return null_texture;
}

std::shared_ptr<Texture2D> RecourseManager::GetAsteroidTexture(int id)
{
    if (asteroid_textures.empty()) {
        return null_texture;
    }
    return asteroid_textures[id % asteroid_textures.size()];
}

MainCamera::MainCamera(Vector2 pos, Vector2 offset, float zoom)
{
    main_camera = std::make_shared<Camera2D>();
    main_camera->target = pos;
    main_camera->offset = offset;
    main_camera->zoom = zoom;

    is_shaking = false;
    velocity = { 0, 0 };
    offset_velocity = { 0, 0 };
    target_offset = offset;

    // ������������� ������
    shake_offset = { 0, 0 };
    shake_duration = 0.0f;
    shake_timer = 0.0f;
    shake_intensity = 0.0f;
    shake_decay = 0.9f;    // ����������� ��������� (0-1)
    shake_trauma = 0.0f;   // ��������� ���� ������
}

void MainCamera::CalculateVelocity(Vector2 target_pos)
{
    Vector2 act_pos = main_camera->target;
    Vector2 new_velo = Vector2Normalize(target_pos - act_pos) *
            Span(float(cpvlengthsq(Vector2TocpVect(target_pos - act_pos))) * GameConstants::CAMERA_MOVE_COEFF,
                GameConstants::CAMERA_SPEED_MIN,
                GameConstants::CAMERA_SPEED_MAX);
    SetVelocity(new_velo);
}

void MainCamera::SpanCamera(Vector2 field)
{
    Vector2 pos = main_camera->target;
    main_camera->target = { Span(pos.x, -field.x, field.x), Span(pos.y, -field.y, field.y) };
    main_camera->zoom = Span(main_camera->zoom + GetMouseWheelMove() * 0.05f, GameConstants::CAMERA_ZOOM_MIN, GameConstants::CAMERA_ZOOM_MAX);
}

void MainCamera::Update(float dt)
{
    // 1. ���������� ������� ������ (���� �����)
        main_camera->target.x += velocity.x * dt;
        main_camera->target.y += velocity.y * dt;

    // 2. ������� �������� offset � target_offset (���� �����)
    /*if (offset_velocity.x != 0 || offset_velocity.y != 0) {
        main_camera->offset.x += offset_velocity.x * dt;
        main_camera->offset.y += offset_velocity.y * dt;
    }*/

    // ��������� ������ (������� ������)
    if (is_shaking && shake_timer > 0.0f)
    {
        shake_timer -= dt;

        // ��������� ���� ������ � ����������
        float trauma = shake_timer / shake_duration;

        // ��������� ������-���������� ���� � ������� ����������
        static Vector2 noise_seed = { 0.0f, 0.0f };
        float time = GetTime() * 10.0f;

        // ���������� ������ � ������� ��������� ��� ����� ����������� �������
        float x_noise =
            sinf(time * 23.0f + noise_seed.x) * 0.33f +
            sinf(time * 11.0f + noise_seed.x) * 0.33f +
            sinf(time * 5.0f + noise_seed.x) * 0.33f;

        float y_noise =
            sinf(time * 19.0f + noise_seed.y) * 0.33f +
            sinf(time * 13.0f + noise_seed.y) * 0.33f +
            sinf(time * 7.0f + noise_seed.y) * 0.33f;

        // ��������� ���������
        x_noise *= trauma * trauma * shake_intensity * 15.0f;
        y_noise *= trauma * trauma * shake_intensity * 15.0f;

        // ��������� ��������
        main_camera->offset.x = target_offset.x + x_noise;
        main_camera->offset.y = target_offset.y + y_noise;

        // ���� ������ �����������
        if (shake_timer <= 0.0f)
        {
            is_shaking = false;
            main_camera->offset = target_offset;
        }
    }
}

void MainCamera::SetVelocity(Vector2 velo)
{
    velocity = velo;
}

void MainCamera::SetZoom(float zoom)
{
    main_camera->zoom = zoom;
}

void MainCamera::SetOffset(Vector2 offset)
{
    main_camera->offset = offset;
    target_offset = offset;
}

void MainCamera::Shake(Vector2 impulse)
{
    is_shaking = true;

    // ��������� ������������� �� ��������
    float impulse_strength = sqrtf(impulse.x * impulse.x + impulse.y * impulse.y);

    // ��������� ��� �������� ��� ���� �����:
    shake_intensity = fminf(1.0f, impulse_strength / 50.0f); // 50 - ����. �������
    shake_duration = 0.3f + shake_intensity * 0.4f; // ������������ �� 0.3 �� 0.7 ���
    shake_timer = shake_duration;
}

// �������������� ������ � ���������� ����������
void MainCamera::Shake(float intensity = 0.5f, float duration = 0.5f)
{
    is_shaking = true;
    shake_intensity = intensity;
    shake_duration = duration;
    shake_timer = duration;
}
