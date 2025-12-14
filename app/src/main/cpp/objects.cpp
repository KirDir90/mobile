#include "objects.h"
#include "game.h"

void DynamicObject::ApplyForce(float x, float y, float dt)
{
	if (!IsActive()) return;
	cpBodyApplyForceAtLocalPoint(body, { x * dt, y * dt }, cpvzero);
}

void DynamicObject::ApplyImpulse(float x, float y, float dt)
{
	if (!IsActive()) return;
	cpBodyApplyImpulseAtLocalPoint(body, { x * dt, y * dt }, cpvzero);
}

void DynamicObject::RotateAt(float diretion, float dt)
{
	if (!IsActive()) return;
	cpBodySetAngle(body, diretion);
}

void DynamicObject::LookAt(float x, float y, float dt)
{
	if (!IsActive()) return;
	float direction = cpvtoangle(cpvsub({ x, y }, cpBodyGetPosition(body)));
	RotateAt(direction, dt);
}

void DynamicObject::SafeDestroyPhysics()
{
    if (!body) return;

    cpSpace* space = cpBodyGetSpace(body);
    if (space) {
        // ������� �����
        if (circle_shape && cpSpaceContainsShape(space, circle_shape)) {
            cpSpaceRemoveShape(space, circle_shape);
        }
        if (poly_shape && cpSpaceContainsShape(space, poly_shape)) {
            cpSpaceRemoveShape(space, poly_shape);
        }

        // ������� ����
        if (cpSpaceContainsBody(space, body)) {
            cpSpaceRemoveBody(space, body);
        }
    }

    // ����������� ������ (���� �� �������)
    if (circle_shape) {
        cpShapeFree(circle_shape);
        circle_shape = nullptr;
    }
    if (poly_shape) {
        cpShapeFree(poly_shape);
        poly_shape = nullptr;
    }
    if (body) {
        cpBodyFree(body);
        body = nullptr;
    }
    //std::cout << "safe destroy \n";
}

cpShape* DynamicObject::GetShape()
{
	if (!IsActive()) return nullptr;
	if (cpSpaceContainsShape(cpBodyGetSpace(body), poly_shape)) return poly_shape;
	else return circle_shape;
}

cpShape* DynamicObject::GetPolyShape()
{
	if (!IsActive()) return nullptr;
	return poly_shape;
}

cpShape* DynamicObject::GetCircleShape()
{
	if (!IsActive()) return nullptr;
	return circle_shape;
}

cpSpace* DynamicObject::GetSpace()
{
	if (!IsActive()) return nullptr;
	return cpBodyGetSpace(body);
}

cpVect DynamicObject::GetPosition() {
	if (!IsActive()) return cpvzero;
	return cpBodyGetPosition(body);
}

float DynamicObject::GetRotation() {
	if (!IsActive()) return 0;
	return cpvtoangle(cpBodyGetRotation(body));
}



DynamicPolygon::DynamicPolygon(float x, float y, std::vector<cpVect> vert, float density, cpSpace* space)
    : DynamicObject(), texture(nullptr), engineParticles()
{
    vertices = std::move(vert);
    points.resize(vertices.size());

    // ���������� ���������� ������ ���� (������� �������������� ���� ������)
    cpVect sum = cpvzero;
    for (const auto& vec : vertices) {
        sum = cpvadd(sum, vec);
    }
    middle = cpvmult(sum, 1.0f / vertices.size());

    //std::cout << "Middle: " << middle.x << ", " << middle.y << "\n";

    // ���������� ������� (bounding box)
    cpVect min_bound = vertices[0], max_bound = vertices[0];
    for (const auto& vec : vertices) {
        min_bound.x = std::min(min_bound.x, vec.x);
        min_bound.y = std::min(min_bound.y, vec.y);
        max_bound.x = std::max(max_bound.x, vec.x);
        max_bound.y = std::max(max_bound.y, vec.y);
    }
    size = std::max(max_bound.x - min_bound.x, max_bound.y - min_bound.y);

    float area = PolygonArea(vertices);
    float mass = area * density;

    // ������� ��� ������� ���, ����� middle ���� (0,0)
    std::vector<cpVect> local_vertices;
    for (const auto& vec : vertices) {
        local_vertices.push_back(cpvsub(vec, middle));
    }
    vertices = local_vertices;

    // ������ ������� ������ ���� �������� ��� ��������� ������
    float moment = cpMomentForPoly(mass, local_vertices.size(), local_vertices.data(), cpvzero, 0.f);

    body = cpBodyNew(mass, moment);

    // ������������� ������� ���� � �������� ����� + �������� middle
    cpBodySetPosition(body, cpv(x + middle.x, y + middle.y));

    // ������� ����� �� ���������� ���������
    poly_shape = cpPolyShapeNew(body, local_vertices.size(), local_vertices.data(), cpTransformIdentity, 0.1);

    // �������� ����� ���� ������ ���� �������
    circle_shape = cpCircleShapeNew(body, size * 0.5f, cpvzero);

    cpSpaceAddBody(space, body);
    cpSpaceAddShape(space, poly_shape);

    //std::cout << "Mass: " << mass << ", Moment: " << moment << "\n";
}

const std::vector<Vector2>& DynamicPolygon::GetVertices()
{
    return points;
}

Vector2 DynamicPolygon::GetMiddle()
{
    return cpVectToVector2(GetPosition());
}

void DynamicPolygon::RotateToPointWithTorque(cpVect targetPoint, float torque_strength, float dt) {

    if (!IsActive()) return;

    cpVect currentPos = cpBodyGetPosition(body);
    float currentAngle = cpBodyGetAngle(body);

    cpVect direction = cpvsub(targetPoint, currentPos);
    float targetAngle = cpvtoangle(direction);

    float angleDiff = (targetAngle - currentAngle);

    while (angleDiff > CP_PI) angleDiff -= 2.0f * CP_PI;
    while (angleDiff < -CP_PI) angleDiff += 2.0f * CP_PI;

    float angleCoeff = (angleDiff > 0 && angleDiff < CP_PI) ? 1 : -1;

    // ��������� ������������ �������
    float torque = angleCoeff * torque_strength;
    cpBodySetTorque(body, torque);

    if (std::abs(cpBodyGetAngularVelocity(body) * dt) >= std::abs(angleDiff)) {
        cpBodySetTorque(body, 0);
        cpBodySetAngle(body, targetAngle);
        cpBodySetAngularVelocity(body, 0);
    }
    if (angleDiff * cpBodyGetAngularVelocity(body) < 0) {
        cpBodySetTorque(body, 0);
        cpBodySetAngularVelocity(body, 0);
    }
}

void DynamicPolygon::UpdateVelocity(float dt)
{
    VelocityResist(dt);
}

void DynamicPolygon::VelocityResist(float dt)
{
    if (!IsActive()) return;

    float res_coeff = GameConstants::VELOCITY_RESIST_COEFF_STATIC;
    cpVect velocity = cpBodyGetVelocity(body);
    float speed = cpvlengthsq(velocity);

    cpBodyApplyForceAtWorldPoint(body, -cpvnormalize(velocity) * speed * res_coeff, GetPosition());
}

void DynamicPolygon::SetTexture(Texture2D* tex)
{
    texture = std::move(tex);
}

void DynamicPolygon::Update(float dt) {
    if (!IsActive()) return;
    if (cpSpaceContainsShape(cpBodyGetSpace(body), poly_shape) &&
        !cpSpaceContainsShape(cpBodyGetSpace(body), circle_shape) &&
        cpvdistsq(GetPosition(), Vector2TocpVect(game.GetCamera().target)) > 1000000) {

        cpSpaceRemoveShape(cpBodyGetSpace(body), poly_shape);
        cpSpaceAddShape(cpBodyGetSpace(body), circle_shape);
    }
    if (cpSpaceContainsShape(cpBodyGetSpace(body), circle_shape) && 
        !cpSpaceContainsShape(cpBodyGetSpace(body), poly_shape) && 
        cpvdistsq(GetPosition(), Vector2TocpVect(game.GetCamera().target)) < 900000) {

        cpSpaceRemoveShape(cpBodyGetSpace(body), circle_shape);
        cpSpaceAddShape(cpBodyGetSpace(body), poly_shape);
    }
    if (cpSpaceContainsShape(cpBodyGetSpace(body), circle_shape) && cpvdistsq(GetPosition(),
        Vector2TocpVect(game.GetCamera().target)) > 10000000) {
        cpSpaceRemoveShape(cpBodyGetSpace(body), circle_shape);
        //cpSpaceAddShape(cpBodyGetSpace(body), poly_shape);
    }
    if (!cpSpaceContainsShape(cpBodyGetSpace(body), circle_shape) && cpvdistsq(GetPosition(),
        Vector2TocpVect(game.GetCamera().target)) < 9000000) {
        cpSpaceAddShape(cpBodyGetSpace(body), circle_shape);
        //cpSpaceAddShape(cpBodyGetSpace(body), poly_shape);
    }
    UpdateVelocity(dt);
}

void DynamicPolygon::DrawMyTexture(Texture2D* texture, cpVect position, float angleRadians, Color tint) {
    if (texture == nullptr || texture->id == 0) return;

    // ��������� ������� (����� ������� � ��������� ��� ���������)
    //const Vector2 pivotOffset = { 0, -20 };// �������� ������ �������� (����� �������� ������ ������)
    Vector2 pos = GetMiddle();

    // ������������� �������� ��������
    Rectangle sourceRec = {
        0, 0,
        (float)texture->width,
        (float)texture->height
    };

    // ������������� ���������� � ������ ��������
    Rectangle destRec = {
        pos.x,    // X ��������
        pos.y,   // Y ��������  
        size,                         // ������
        size                         // ������
    };

    // ����� �������� (����� �������)
    Vector2 origin = {
        size / 2, //- pivotOffset.x,
        size / 2 //- pivotOffset.y
    };

    // ���� �������� � �������� (Raylib ���������� �������)
    float rotationDegrees = RAD2DEG * angleRadians + 90;

    DrawTexturePro(
        *texture,
        sourceRec,
        destRec,
        origin,
        rotationDegrees,
        tint
    );
}

void DynamicPolygon::Draw()
{

    cpVect position = cpBodyGetPosition(body);
    float angle = cpBodyGetAngle(body);

    
    if (texture != nullptr) {
        DrawMyTexture(texture, position, angle, color);
    }
    else
    {
        if (cpSpaceContainsShape(cpBodyGetSpace(body), poly_shape)) {

            if (points.size() != vertices.size()) {
                points.resize(vertices.size());
            }

            // ������������ ��������������
            std::transform(vertices.begin(), vertices.end(),
                points.begin(),
                [position, angle](cpVect vertex) {
                    cpVect rotated = cpvrotate(vertex, cpvforangle(angle));
                    return Vector2{ float(position.x + rotated.x), float(position.y + rotated.y) };
                });

            points.push_back(points[0]);
            DrawLineStrip(points.data(), points.size(), color);
        }
        else {
            Vector2 center = cpVectToVector2(
                cpvadd(position, cpvrotate(cpCircleShapeGetOffset(circle_shape), cpvforangle(angle))));
            DrawCircleLines(center.x, center.y, cpCircleShapeGetRadius(circle_shape), color);
        }
        DrawCircleV(GetMiddle(), 2, WHITE);
        DrawCircleLinesV(GetMiddle(), GetSize() * 0.5f, WHITE);
    }
}



SpaceShip::SpaceShip(float x, float y, std::vector<cpVect> vertices, float dencity, cpSpace* space,
    float hp, float dmg, float rel, float pwr, float torqueStrength)
    : DynamicPolygon(x, y, vertices, dencity, space)
{
    damage = dmg;
    health = hp;
    power = pwr;
    reload = rel;
    torque_strength = torqueStrength;
}

std::shared_ptr<Unit> SpaceShip::GetPilot()
{
    return my_pilot;
}

void SpaceShip::TakeDamage(float damage)
{
    health = std::fmaxf(0, health - damage); //std::cout << "health: " << health << "\n";

    if (GetPilot()->IsPlayer()) {
        game.UpdatePlayerHp();
        game.GetScene()->GetMainCamera()->Shake(damage * 0.02f, 0.5f);
    }
    if (!IsAlive()) {
        death_point = GetMiddle();
        engineParticles.Explode(GetMiddle());
        game.GetScene()->GetWorld()->MissUnit(GetPilot()->GetTeam());
        game.UpdateUnitsCount(GetPilot()->GetTeam());
        if (CheckCollisionPointRec(death_point, GetCameraRectangle2D(game.GetScene()->GetCamera()))) {
            float dist = Vector2DistanceSqr(death_point, game.GetScene()->GetMainCamera()->GetPosition());
            game.GetScene()->GetMainCamera()->Shake(Span(10000.f / dist, 0.5f, 2.f), 0.5f);
        }
        SafeDestroyPhysics();
    }
}

void SpaceShip::VelocityResist(float dt)
{
    float res_coeff = GameConstants::VELOCITY_RESIST_COEFF_STATIC;
    if (engineOn) res_coeff = GameConstants::VELOCITY_RESIST_COEFF;

    cpVect velocity = cpBodyGetVelocity(body);
    float speed = cpvlengthsq(velocity);

    cpBodyApplyForceAtWorldPoint(body, -cpvnormalize(velocity) * speed * res_coeff, GetPosition());
}

void SpaceShip::Accelerate(float dt) {
    
    cpBodyApplyForceAtLocalPoint(body, { power, 0 }, cpvzero);

    engineOn = true;
}

void SpaceShip::SlowDown(float dt)
{
    cpVect velocity = cpBodyGetVelocity(body);
    float current_speed = cpvlength(velocity);
    if (current_speed > 0) {
        // �������������� ������������ ���� ������
        cpVect friction_direction = cpvmult(velocity, -1.0f / current_speed); // ��������������� ������ ������ ��������
        cpVect friction_force = cpvmult(friction_direction, power); // ����� ������������ ����������� ������ ��������
        cpBodyApplyForceAtWorldPoint(body, friction_force, cpBodyGetPosition(body));
    }
}

void SpaceShip::RotateTo(float x, float y, float dt)
{
    RotateToPointWithTorque({ x, y }, torque_strength, dt);
}

void SpaceShip::Update(float dt) {

    DynamicPolygon::Update(dt);
    if (!IsAlive()) {
        explosion_timer -= dt;
        engineParticles.UpdateExplosion(dt);
        return;
    }

    if (!CheckCollisionPointRec(cpVectToVector2(GetPosition()), GetCameraCollider2D(game.GetCamera(), vertices[0].x))) {
        return;
    }
    engineParticles.Update(GetMiddle(), cpBodyGetAngle(body), cpvlength(cpBodyGetVelocity(body)), GetSize(), dt, engineOn);

}

void SpaceShip::Fire()
{
    if (reload <= GetTime() - last_shot) {
        auto this_ship = shared_from_this();
        game.AddElementToWorld(std::make_shared<Bullet>(damage, GetMiddle(),
            cpVectToVector2(cpvadd(cpBodyGetVelocity(body),
                { GameConstants::BULLET_SPEED * std::cosf(this->GetRotation()),
                GameConstants::BULLET_SPEED * std::sinf(this->GetRotation()) })), this_ship, color));
        last_shot = GetTime();
    }
}

void SpaceShip::Draw() {

    if (!IsAlive()) {
        if (CheckCollisionPointRec(death_point, GetCameraCollider2D(game.GetCamera(), vertices[0].x))) {
            //std::cout << "draw explosion\n";
            engineParticles.Draw();
        }
        return;
    }

    if (!CheckCollisionPointRec(cpVectToVector2(GetPosition()), GetCameraCollider2D(game.GetCamera(), vertices[0].x))) {

        Rectangle rec = {   GetCameraRectangle2D(game.GetCamera()).x+5,
                            GetCameraRectangle2D(game.GetCamera()).y+5,
                            GetCameraRectangle2D(game.GetCamera()).width-10,
                            GetCameraRectangle2D(game.GetCamera()).height-15};

        auto inter =    GetLineRectIntersectionSimple(rec, cpVectToVector2(GetPosition()),
                        game.GetCamera().target);

        if (!inter.has_value()) return;

        float dist = Vector2DistanceSqr(inter.value(), cpVectToVector2(GetPosition()));
        DrawCircleV(inter.value(), Span(100000000.0f / dist, 5.0f, 30.0f), color);
        return;
    }
    
    
    if (!IsAlive()) {
        //std::cout << "draw explosion\n";
        engineParticles.Draw();
        return;
    }

    engineParticles.Draw();
    DynamicPolygon::Draw();
    engineOn = false;
}




Bullet::Bullet(float damage, Vector2 position, Vector2 velocity, std::shared_ptr<SpaceShip> parent_ship, Color col)
    : damage(damage), position(position), velocity(velocity), parent(parent_ship)
{
    color = col;
    size = GetScreenWidth() * 0.0025f;  // �������� ������� f ��� float
    last_check_pos = position;
    last_check = GetTime();
}

void Bullet::Update(float dt)
{
    if (!IsActive()) {
        return;
    }

    Vector2 old_pos = last_check_pos;
    Vector2 new_pos = {
        position.x + velocity.x * dt,
        position.y + velocity.y * dt
    };

    // ��������� ������������ ������ ���� ������ �����
    if (GetTime() - last_check >= check_time) {
        last_check = GetTime();
        last_check_pos = new_pos;

        auto units = game.GetScene()->GetWorld()->GetUnits();
        auto objects = game.GetScene()->GetWorld()->GetAsteroids();
        auto hit_iterator = std::find_if(units.begin(), units.end(),
            [&](const std::shared_ptr<Unit> unit) {
                // �������� �� nullptr ������ unit
                if (!unit) {
                    return false;
                }

                // �������� parent � ��� pilot
                if (!parent) {
                    return false;
                }

                auto parent_pilot = parent->GetPilot();
                if (!parent_pilot) {
                    return false;
                }

                // �������� ��� ��� �� ������������ ����
                if (unit.get() == parent_pilot.get()) {
                    return false;
                }

                // �������� ������� unit'�
                auto ship = unit->GetShip();
                if (!ship || !ship->IsAlive() || !ship->IsActive()) {;
                    return false;
                }

                // �������� ��� ��� �� ������������ �������
                if (ship.get() == parent.get()) {
                    return false;
                }

                // �������� friendly fire � ��������������� ����������
                auto parent_team = parent_pilot->GetTeam();
                auto unit_team = unit->GetTeam();

                if (parent_team == unit_team && !game.GetSetting("Friendly fire")) {
                    return false;
                }

                // �������� ������������
                return CheckCollisionCircleLine(
                    cpVectToVector2(ship->GetPosition()),
                    ship->GetSize() * 0.5f,
                    old_pos,
                    new_pos
                );
            });

        if (hit_iterator != units.end()) {
            auto unit_hit = *hit_iterator;
            auto ship_hit = unit_hit->GetShip();

            if (ship_hit && ship_hit->IsAlive()) {

                if (GetParent()->GetPilot()->IsPlayer()) game.UpdatePlayerScore(std::min(damage, ship_hit->GetHp()));
                else if (ship_hit->GetPilot()->IsPlayer()) game.UpdatePlayerScore(-1 * std::min(damage, ship_hit->GetHp()));
                ship_hit->TakeDamage(damage);
                
                hit = true;
                live_time = 0;  // ���������� ����
                return;
            }
        }
        else {
            auto hit_iterator = std::find_if(objects.begin(), objects.end(),
                [&](const std::shared_ptr<Asteroid> object) {
                    // �������� �� nullptr ������ unit
                    if (!object) {
                        return false;
                    }

                    if (!object->IsActive()) {
                        return false;
                    }

                    // �������� ������������
                    return CheckCollisionCircleLine(
                        cpVectToVector2(object->GetPosition()),
                        object->GetSize() * 0.5f,
                        old_pos,
                        new_pos
                    );
                });
            if (hit_iterator != objects.end()) {

                hit = true;
                live_time = 0;  // ���������� ����
                return;
            }
        }
    }
    // ��������� �������
    position = new_pos;
    live_time -= dt;
}

bool Bullet::IsActive() {
    if (hit || live_time <= 0) return false;
    return true;
}

void Bullet::Draw() {
    if (IsActive() && CheckCollisionPointRec(position, GetCameraCollider2D(game.GetCamera(), 0))) {
        //DrawCircle(position.x, position.y, size, RED);
        DrawLineEx({ position.x - velocity.x * 0.01f, position.y - velocity.y * 0.01f },
            position,
            parent->GetSize() * 0.05f,
            color);
    }
}




Asteroid::Asteroid(float x, float y, float dencity, float size_c, cpSpace* space)
    : DynamicPolygon(x, y, asteroidVertices, dencity, space), size_coeff(size_c)
{
    cpSpaceRemoveShape(space, poly_shape);
    size *= size_coeff;

    circle_shape = cpCircleShapeNew(body, size * 0.5f, cpvzero);

    cpSpaceAddShape(space, circle_shape);

    SetTexture(game.GetResource()->GetAsteroidTexture(GetRandomValue(0, 100)).get());
}
