// containers.cpp
#include "containers.h"

// ==================== BulletContainer ====================
BulletContainer::BulletContainer() : ObjectContainer<Bullet>() {}

void BulletContainer::Update(float dt) {
    std::for_each(objects.begin(), objects.end(), [&dt](auto& bullet) {
        bullet->Update(dt);
        });

    // Помечаем неактивные пули для удаления
    if (check_time <= 0) {
        std::erase_if(objects, [](const auto& bullet) {
            return bullet && !bullet->IsActive();
            });
        check_time = GameConstants::BULLET_LIFETIME;
    }
    check_time -= dt;
}

void BulletContainer::Draw() {
    for (auto& bullet : objects) {
        bullet->Draw();
    }
}

void BulletContainer::RemoveNonActive() {
    std::erase_if(objects, [](const auto& bullet) {
        return !bullet->IsActive();
        });
}

// ==================== UnitContainer ====================
UnitContainer::UnitContainer() : ObjectContainer<Unit>() {}

void UnitContainer::Update(float dt) {
    std::for_each(objects.begin(), objects.end(), [&dt](std::shared_ptr<Unit> unit) {
        if (!unit) return;

        if (!unit->IsPlayer() && unit->GetShip()->GetExplosionTime() >= 0) {
            unit->Step(dt);
        }
        });

    if (check_time <= 0) {
        std::erase_if(objects, [](const auto& unit) {
            return !unit->GetShip()->IsAlive() && unit->GetShip()->GetExplosionTime() <= 0;
            });
        check_time = 1.0f;
    }
    check_time -= dt;
}

void UnitContainer::Draw() {
    for (auto& unit : objects) {
        unit->DrawShip();
        if (unit->IsPlayer()) {
            Vector2 target = unit->GetPosition();
            float size = unit->GetShip()->GetSize();

            // Простой треугольник над кораблем
            Vector2 top = { target.x, target.y - size * 1.0f };
            Vector2 left = { target.x - size * 0.5f, target.y - size * 2.0f };
            Vector2 right = { target.x + size * 0.5f, target.y - size * 2.0f };

            DrawTriangle(right, left, top, { 0, 0, 255, 32 });

            // Контур для лучшей видимости
            DrawTriangleLines(top, left, right, BLACK);
        }
    }
}

void UnitContainer::RemoveNonActive() {
    if (objects.empty()) return;

    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
            [](std::shared_ptr<Unit> obj) {
                return !obj->GetShip()->IsAlive() && obj->GetShip()->GetExplosionTime() <= 0;
            }),
        objects.end()
    );
}

Color AsteroidContainer::GenerateRandomAsteroidColor()
{
    // Генерация естественного цвета астероида
    int color_choice = GetRandomValue(0, 100);

    if (color_choice < 40) {
        // Коричневые оттенки (40%)
        int brown_shade = GetRandomValue(80, 160);
        return {
            static_cast<unsigned char>(brown_shade + GetRandomValue(0, 20)),
            static_cast<unsigned char>(brown_shade * 0.6f),
            static_cast<unsigned char>(brown_shade * 0.3f),
            255
            };
    }
    else if (color_choice < 70) {
        // Серые оттенки (30%)
        int gray_shade = GetRandomValue(90, 170);
        return{
            static_cast<unsigned char>(gray_shade),
            static_cast<unsigned char>(gray_shade),
            static_cast<unsigned char>(gray_shade),
            255
            };
    }
    else if (color_choice < 85) {
        // Красноватые оттенки (15%)
        return{
            static_cast<unsigned char>(GetRandomValue(120, 180)),
            static_cast<unsigned char>(GetRandomValue(50, 100)),
            static_cast<unsigned char>(GetRandomValue(20, 60)),
            255
            };
    }
    else {
        // Темные оттенки (15%)
        return{
            static_cast<unsigned char>(GetRandomValue(50, 100)),
            static_cast<unsigned char>(GetRandomValue(40, 80)),
            static_cast<unsigned char>(GetRandomValue(30, 70)),
            255
            };
    }
}

// ==================== AsteroidContainer ====================
AsteroidContainer::AsteroidContainer() : ObjectContainer<Asteroid>() {}

void AsteroidContainer::AddItem(std::shared_ptr<Asteroid> item)
{
    item->ApplyImpulse(GetRandomValue(-100000, 100000), GetRandomValue(-100000, 100000), 0.1f);
    item->SetColor(GenerateRandomAsteroidColor());
    

    ObjectContainer::AddItem(item);
}


void AsteroidContainer::Update(float dt) {
    std::for_each(objects.begin(), objects.end(), [&dt](auto& asteroid) {
        if (asteroid) {
            asteroid->Update(dt);
        }
        });

    // Периодическая проверка на удаление разрушенных астероидов
    if (check_time <= 0) {
        std::erase_if(objects, [](const auto& asteroid) {
            return asteroid && !asteroid->IsActive();
            });
        check_time = 1.0f;
    }
    check_time -= dt;
}

void AsteroidContainer::Draw() {
    for (auto& asteroid : objects) {
        if (asteroid) {
            asteroid->Draw();
        }
    }
}

void AsteroidContainer::RemoveNonActive() {
    std::erase_if(objects, [](const auto& asteroid) {
        return !asteroid->IsActive();
        });
}

// Дополнительный метод для поиска астероидов в радиусе
std::vector<std::shared_ptr<Asteroid>> AsteroidContainer::GetAsteroidsInRadius(Vector2 center, float radius) {
    std::vector<std::shared_ptr<Asteroid>> result;

    for (const auto& asteroid : objects) {
        if (asteroid) {
            Vector2 pos = cpVectToVector2(asteroid->GetPosition());
            float distance = Vector2Distance(pos, center);
            float asteroid_radius = asteroid->GetSize() / 2.0f;

            if (distance <= (radius + asteroid_radius)) {
                result.push_back(asteroid);
            }
        }
    }

    return result;
}