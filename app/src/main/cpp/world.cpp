#include "world.h"
#include "game.h"

World::World()
{
    bullet_container = std::make_shared<BulletContainer>();
    unit_container = std::make_shared<UnitContainer>();
    asteroid_container = std::make_shared<AsteroidContainer>();
    space = cpSpaceNew();
}

void World::CreateWorld(int teamSize, std::vector<cpVect> spawnPoints, int fault, Vector2 battlefield)
{
    battlefield_size = battlefield;
    
    
    this->fault = fault;
    int un = 0;
    if (spawnPoints.size() > 0) pos_red = (spawnPoints[0]);
    if (spawnPoints.size() > 1) pos_blue = (spawnPoints[1]);
    if (spawnPoints.size() > 2) pos_green = (spawnPoints[2]);

    for (int i = 0; i < GameConstants::ASTEROID_COUNT; i++) {
        asteroid_container->AddItem(std::make_shared<Asteroid>(GetRandomValue(-battlefield_size.x*0.5f, battlefield_size.x * 0.5f),
            GetRandomValue(-battlefield_size.y * 0.5f, battlefield_size.y * 0.5f),
            0.05f, GetRandomValue(100, 1000) * 0.01f, space));
    }

    for (auto& point : spawnPoints) {
        Team team;

        switch (un) {
        case 0:
            team = Team::TEAM_RED;
            count_red = teamSize;
            has_red = true;
            break;
        case 1:
            team = Team::TEAM_BLUE;
            count_blue = teamSize;
            has_blue = true;
            break;
        case 2:
            team = Team::TEAM_GREEN;
            count_green = teamSize;
            has_green = true;
            break;
        default:
            team = Team::TEAM_GREEN;
            count_green = teamSize;
            has_green = true;
            break;
        }

        for (int i = 0; i < std::min(teamSize, GameConstants::MAX_UNIT_ON_FIELD); i++) {
            SpaceShipType type;


            int rnd = GetRandomValue(0, 2);
            switch (rnd) {
            case 0:
                type = SpaceShipType::FIGHTER;
                //std::cout << "fighter\n";
                break;
            case 1:
                type = SpaceShipType::RIPPER;
                //std::cout << "ripper\n";
                break;
            case 2:
                type = SpaceShipType::SCOUT;
                //std::cout << "scout\n";
                break;
            default:
                type = SpaceShipType::FIGHTER;
                break;
            }
            Vector2 spawn_point = {
                    static_cast<float>(point.x + GetRandomValue(-fault, fault)),
                    static_cast<float>(point.y + GetRandomValue(-fault, fault))
            };
            auto unit = std::make_shared<Unit>(team, type, spawn_point, space);
            unit->GetShip()->SetPilot(unit);
            unit_container->AddItem(unit);
        }
        un = (un + 1) % 3;
        game.UpdateUnitsCount(team);
    }
    int rnd = GetRandomValue(0, spawnPoints.size() - 1);
    Team team;
    switch (rnd) {
    case 0:
        team = Team::TEAM_BLUE;
        break;
    case 1:
        team = Team::TEAM_RED;
        break;
    case 2:
        team = Team::TEAM_GREEN;
        break;
    default:
        team = Team::TEAM_BLUE;
        break;
    }
    SearchForNewPlayer(team);

    game.SetPlayerScore(0);
    
}

void World::Activate(SceneState state)
{
    if (activated) {
        game.UpdatePlayerHp();
        game.UpdatePlayerScore(0);
        if (HasTeam(Team::TEAM_RED)) game.UpdateUnitsCount(Team::TEAM_RED);
        if (HasTeam(Team::TEAM_BLUE))game.UpdateUnitsCount(Team::TEAM_BLUE);
        if (HasTeam(Team::TEAM_GREEN))game.UpdateUnitsCount(Team::TEAM_GREEN);
        return;
    }
    float width, height;
    switch (state) {
    case SceneState::TEAM_BATTLE:
        width = GameConstants::TEAM_MODE_WORLD_WIDTH;
        height = GameConstants::TEAM_MODE_WORLD_HEIGHT;

        CreateWorld(game.GetSetting("Team size"),
            { {-width * 0.5f, -height * 0.5f}, {width * 0.7f, -height * 0.7f}, {0, height * 0.5f} }, width * 0.3f,
            { width, height });
        activated = true;
        break;
    case SceneState::BOSS_FIGHT:
        break;
    case SceneState::FREE_MODE:
        CreateWorld(game.GetSetting("Team size"), { {GameConstants::TEAM_MODE_WORLD_WIDTH * 0.5, 0},
            {-GameConstants::TEAM_MODE_WORLD_WIDTH * 0.5, 0} },
            GameConstants::TEAM_MODE_WORLD_HEIGHT * 0.5,
            { GameConstants::TEAM_MODE_WORLD_WIDTH, GameConstants::TEAM_MODE_WORLD_HEIGHT });
        activated = true;
        break;
    case SceneState::WAVE_MODE:
        break;
    default:
        activated = false;
        break;
    }
}

void World::AddElement(std::shared_ptr<Object> object)
{
    if (std::shared_ptr<Unit> unit = std::dynamic_pointer_cast<Unit>(object)) {
        unit_container->AddItem(unit);
    }
    else if (std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(object)) {
        bullet_container->AddItem(bullet);
    }
}

void World::AddMissingUnits()
{
    while (miss_blue > 0) {
        miss_blue--;
        unit_container->AddItem(CreateUnit(Team::TEAM_BLUE));
    }
    while (miss_green > 0) {
        miss_green--;
        unit_container->AddItem(CreateUnit(Team::TEAM_GREEN));
    }
    while (miss_red > 0) {
        miss_red--;
        unit_container->AddItem(CreateUnit(Team::TEAM_RED));
    }
}

std::shared_ptr<Unit> World::CreateUnit(Team team)
{
    SpaceShipType type;
    cpVect point;
    int rnd = GetRandomValue(0, 2);
    switch (rnd) {
    case 0:
        type = SpaceShipType::FIGHTER;
        //std::cout << "fighter\n";
        break;
    case 1:
        type = SpaceShipType::RIPPER;
        //std::cout << "ripper\n";
        break;
    case 2:
        type = SpaceShipType::SCOUT;
        //std::cout << "scout\n";
        break;
    default:
        type = SpaceShipType::FIGHTER;
        break;
    }

    switch (team) {
    case Team::TEAM_RED:
        point = pos_red;
        break;
    case Team::TEAM_GREEN:
        point = pos_green;
        break;
    case Team::TEAM_BLUE:
        point = pos_blue;
        break;
    }

    Vector2 spawn_point = {
            static_cast<float>(point.x + GetRandomValue(-fault, fault)),
            static_cast<float>(point.y + GetRandomValue(-fault, fault))
    };
    auto unit = std::make_shared<Unit>(team, type, spawn_point, space);
    unit->GetShip()->SetPilot(unit);

    return std::move(unit);
}

void World::SearchForNewPlayer(Team team)
{
    for (auto p : unit_container->GetElements()) {
        if (p->GetTeam() == team) {
            player = std::static_pointer_cast<Player>(p);
            break;
        }
    }
    player->IdentificateAsPlayer();

    game.UpdatePlayerHp();
}

bool World::HasTeam(Team team)
{
    bool flag = false;
    switch (team) {
    case Team::TEAM_BLUE:
        flag = has_blue;
        break;
    case Team::TEAM_RED:
        flag = has_red;
        break;
    case Team::TEAM_GREEN:
        flag = has_green;
        break;
    }
    return flag;
}

void World::RemoveElement(Object* object)
{
}

int World::GetUnitsCount(Team team)
{
    int count = 0;
    switch (team) {
    case Team::TEAM_RED:
        count = count_red;
        break;
    case Team::TEAM_BLUE:
        count = count_blue;
        break;
    case Team::TEAM_GREEN:
        count = count_green;
        break;
    default:
        count = 0;
    }
    return count;
}

void World::MissUnit(Team team)
{
    switch (team) {
    case Team::TEAM_RED:
        if (count_red > GameConstants::MAX_UNIT_ON_FIELD) {
            miss_red++;
        }
        count_red--;
        break;
    case Team::TEAM_GREEN:
        if (count_green > GameConstants::MAX_UNIT_ON_FIELD) {
            miss_green++;
        }
        count_green--;
        break;
    case Team::TEAM_BLUE:
        if (count_blue > GameConstants::MAX_UNIT_ON_FIELD) {
            miss_blue++;
        }
        count_blue--;
        break;
    }
}

void World::Clear()
{
    std::cout << "begin clear\n";
    has_blue = false;
    has_green = false;
    has_red = false;

    count_blue = 0;
    count_green = 0;
    count_red = 0;

    game.GetInterfaceManager()->ClearGameInterface();

    unit_container->Clear();
    bullet_container->Clear();
    asteroid_container->Clear();
    player.reset();
    std::cout << "clear\n";
}

void World::Step(float dt) {

    if (accumulator >= update_time) {
        
        //if (!player->GetShip()->IsAlive()) {
        //    //Deactivate();
        //    game.GoToPreviousInterfaceState();
        //    return;
        //}
        AddMissingUnits();

        if (player->GetShip()->GetHp() <= 0) {
            SearchForNewPlayer(player->GetTeam());
        }
        player->TakeControl(accumulator);
        unit_container->Update(accumulator);
        bullet_container->Update(accumulator);
        asteroid_container->Update(accumulator);

         /*2. ������*/
        cpSpaceStep(space, accumulator);
        last_accumulator = accumulator;
        accumulator = 0.f;
        
    }
    accumulator += dt;
    
    /*bullet_container->Update(dt);
    unit_container->Update(dt);
    player->TakeControl(dt);
    cpSpaceStep(space, dt);*/

}

void World::Render() {
    unit_container->Draw();
    bullet_container->Draw();
    asteroid_container->Draw();
}



Background::Background(float move_coeff, float tiles_count, float real_tile_size, BackgroundType type) 
    : move_coeff(move_coeff), tiles_count(tiles_count), real_tile_size(real_tile_size)
{
    tile_id = std::vector<std::vector<int> >(200, std::vector<int>(200, -1));
    stars_background = game.GetResource()->GetSpaceBackgroundTexture(std::move(type));

    tile_size = 1.0f * stars_background->width / tiles_count;

    offset = { real_tile_size * tile_id.size() * 0.5f, real_tile_size * tile_id[0].size() * 0.5f };
}

void Background::Update() {
    Rectangle target = GetCameraRectangle2D(game.GetCamera());
    target.x += offset.x;
    target.y += offset.y;


    target.x = (target.x + target.width * 0.5) * move_coeff - target.width * 0.5;
    target.y = (target.y + target.height * 0.5) * move_coeff - target.height * 0.5;

    /*target.x *= move_coeff;
    target.y *= move_coeff;*/
    int tar_x = floor(target.x);
    int tar_y = floor(target.y);
    int tar_wid = ceil(target.width);
    int tar_hei = ceil(target.height);

    float x_start = tar_x - Mod(tar_x, real_tile_size);
    float x_end = (tar_x + tar_wid) - Mod((tar_x + tar_wid), real_tile_size);
    float y_start = tar_y - Mod(tar_y, real_tile_size);
    float y_end = (tar_y + tar_hei) - Mod(tar_y + tar_hei, real_tile_size);
    int x_zero = (int)tar_x / real_tile_size;
    int y_zero = (int)tar_y / real_tile_size;

    int seed = game.GetSeed();

    for (int xi = 0; x_start + real_tile_size * xi <= x_end + real_tile_size; xi += 1) {
        for (int yi = 0; y_start + real_tile_size * yi <= y_end + real_tile_size; yi += 1) {

            int x_index = x_zero + xi;
            int y_index = y_zero + yi;

            //if (x_index >= tile_id.size() || y_index >= tile_id[0].size() || x_index < 0 || y_index < 0) continue;
            x_index = Mod(x_index, tile_id.size());
            y_index = Mod(y_index, tile_id.size());

            if (tile_id[x_index][y_index] == -1) tile_id[x_index][y_index] = GetTileIndex(x_index, y_index, seed);
        }
    }
}

void Background::Draw() {

    Rectangle target = GetCameraRectangle2D(game.GetCamera());

    target.x += offset.x;
    target.y += offset.y;

    Rectangle final_target = target;

    target.x = (target.x + target.width * 0.5) * move_coeff - target.width * 0.5;
    target.y = (target.y + target.height * 0.5) * move_coeff - target.height * 0.5;

    /*target.x *= move_coeff;
    target.y *= move_coeff;*/

    Vector2 transform = { final_target.x - target.x, final_target.y - target.y };


    int tar_x = floor(target.x);
    int tar_y = floor(target.y);
    int tar_wid = ceil(target.width);
    int tar_hei = ceil(target.height);

    float x_start = tar_x - Mod(tar_x, real_tile_size);
    float x_end = (tar_x + tar_wid) - Mod((tar_x + tar_wid), real_tile_size);
    float y_start = tar_y - Mod(tar_y, real_tile_size);
    float y_end = (tar_y + tar_hei) - Mod(tar_y + tar_hei, real_tile_size);
    int x_zero = (int)tar_x / real_tile_size;
    int y_zero = (int)tar_y / real_tile_size;


    for (int xi = 0; x_start + real_tile_size * xi <= x_end + real_tile_size; xi += 1) {
        for (int yi = 0; y_start + real_tile_size * yi <= y_end + real_tile_size; yi += 1) {

            int x_index = x_zero + xi;
            int y_index = y_zero + yi;

            //if (x_index >= tile_id.size() || y_index >= tile_id[0].size() || x_index < 0 || y_index < 0) continue;
            x_index = Mod(x_index, tile_id.size());
            y_index = Mod(y_index, tile_id.size());

            int tile_x = (tile_id[x_index][y_index] % tiles_count) * tile_size;
            int tile_y = (tile_id[x_index][y_index] / tiles_count) * tile_size;

            float pos_x = x_start + xi * real_tile_size + transform.x - offset.x;
            float pos_y = y_start + yi * real_tile_size + transform.y - offset.y;

            //DrawCircle(pos_x, pos_y, 100, WHITE);

            DrawTexturePro(*stars_background,
                            Rectangle(tile_x, tile_y, tile_size, tile_size),
                            Rectangle(pos_x, pos_y, real_tile_size, real_tile_size),
                            {0, 0},
                            0,
                            WHITE);
            //DrawRectangleLinesEx(Rectangle(pos_x, pos_y, real_tile_size, real_tile_size), 2, RED);
            //DrawText(TextFormat("%d", tile_id[x_index][y_index]), pos_x + 10, pos_y + 10, 30, RED);
            //std::cout << x << "\t" << y << " id : " << tile_id[x][y] << "\n";
        }
    }
    
}

char Background::GetTileIndex(int x, int y, int seed)
{
    // �������� ���������� � seed
    uint32_t hash = (x * 374761393u + y * 668265263u + seed * 2246822519u) & 0xFFFFFFFFu;

    // �������������� ������������� (PCG-�������� ��������)
    hash = ((hash >> 16) ^ hash) * 0x45d9f3bu;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3bu;
    hash = (hash >> 16) ^ hash;

    // �������� � ��������� 0-15
    return hash & 15;
}



Texture2D GeneratedBackground::GenerateSpaceTexture()
{
    RenderTexture2D target = LoadRenderTexture(width, height);

    BeginTextureMode(target);
    ClearBackground(BLACK);

    // 1. ����������� ���
    DrawSpaceGradient();

    // 2. ���������� (������� ����)
    std::vector<Vector2> nebulaPositions;
    std::vector<float> nebulaSizes;
    std::vector<Color> nebulaColors;
    std::vector<int> nebulaTypes;

    int nebulaCount = GetRandomValue(500, 1000);
    GenerateNebula(nebulaPositions, nebulaSizes, nebulaColors, nebulaTypes, nebulaCount);

    for (size_t i = 0; i < nebulaPositions.size(); i++) {
        DrawNebula(nebulaPositions[i], nebulaSizes[i], nebulaColors[i], nebulaTypes[i]);
    }

    EndTextureMode();

    return target.texture;
}

void GeneratedBackground::DrawSpaceGradient()
{
    // �����-����� �������� �� ������ � �����
    for (int y = 0; y < height; y += 2) {
        float progress = (float)y / height;
        Color color = {
            (unsigned char)(8 + progress * 4),
            (unsigned char)(12 + progress * 8),
            (unsigned char)(20 + progress * 12),
            255
        };
        DrawRectangle(0, y, width, 2, color);
    }
}

void GeneratedBackground::DrawNebula(Vector2 center, float size, Color baseColor, int type)
{
    int layers = 20;

    for (int i = 0; i < layers; i++) {
        float layerProgress = (float)i / layers;
        float layerSize = size * (0.1f + 0.9f * layerProgress);

        float noiseX = 0, noiseY = 0;
        switch (type) {
        case 0: // ���������
            noiseX = sin(center.x * 0.01f + layerProgress * 10) * size * 0.1f;
            noiseY = cos(center.y * 0.01f + layerProgress * 8) * size * 0.1f;
            break;
        case 1: // ����������
            noiseX = GetRandomValue(-size * 0.15f, size * 0.15f);
            noiseY = GetRandomValue(-size * 0.15f, size * 0.15f);
            break;
        case 2: // ����������
            noiseX = cos(layerProgress * 8) * size * 0.2f;
            noiseY = sin(layerProgress * 8) * size * 0.2f;
            break;
        default: // ���������
            noiseX = GetRandomValue(-size * 0.1f, size * 0.1f);
            noiseY = GetRandomValue(-size * 0.1f, size * 0.1f);
            break;
        }

        Vector2 layerCenter = {
            center.x + noiseX,
            center.y + noiseY
        };

        Color layerColor = baseColor;
        layerColor.a = (unsigned char)(baseColor.a * (1.0f - layerProgress * 0.8f));

        DrawCircleGradient(
            (int)layerCenter.x, (int)layerCenter.y,
            layerSize,
            layerColor,
            { 0, 0, 0, 0 }
        );
    }
}

void GeneratedBackground::GenerateNebula(std::vector<Vector2>& positions, std::vector<float>& sizes,
    std::vector<Color>& colors, std::vector<int>& types, int count)
{
    for (int i = 0; i < count; i++) {
        positions.push_back({
            1.f * GetRandomValue(-width * 0.1f, width * 1.1f),
            1.f * GetRandomValue(-height * 0.1f, height * 1.1f)
            });

        float size = GetRandomValue(max_size * 0.4f, max_size * 1.2f);
        sizes.push_back(size);

        int nebulaType = GetRandomValue(0, 4);
        types.push_back(nebulaType);

        switch (nebulaType) {
        case 0: // ����� ����������
            colors.push_back(Color{ 80, 120, 255, 15 });
            break;
        case 1: // ������� ����������
            colors.push_back(Color{ 255, 100, 80, 12 });
            break;
        case 2: // ��������� ����������
            colors.push_back(Color{ 180, 80, 220, 18 });
            break;
        case 3: // ������-�������
            colors.push_back(Color{ 80, 220, 200, 10 });
            break;
        default:
            colors.push_back(Color{ 120, 120, 200, 15 });
            break;
        }
    }
}


