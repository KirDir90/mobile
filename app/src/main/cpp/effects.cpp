#include "effects.h"

void OptimizedEngineEffect::Update(Vector2 shipPos, float shipAngle, float shipSpeed, float shipSize, float dt, bool engineOn)
{
    // Добавление новых частиц двигателя
    if (engineOn && count < MAX_PARTICLES) {
        int toAdd = GetRandomValue(2, 5);
        cpVect enginePos = cpvadd(Vector2TocpVect(shipPos), cpvmult(cpvforangle(shipAngle), -shipSize * 0.4f));
        for (int i = 0; i < toAdd && count < MAX_PARTICLES; i++) {
            AddParticle(cpVectToVector2(enginePos), shipAngle, shipSpeed, shipSize * 0.5);
        }
    }

    // Обновление существующих частиц
    for (int i = 0; i < count; ) {
        // Перемещение с учетом dt
        positions[i].x += velocities[i].x * dt;
        positions[i].y += velocities[i].y * dt;

        // Затухание с учетом dt
        float damping = 1.0f - (0.03f * 60.0f * dt);
        velocities[i] = Vector2Scale(velocities[i], damping);

        // Уменьшение жизни и размера с учетом dt
        lives[i] -= dt;
        sizes[i] *= (1 - 2.4f * dt);

        // Удаление мертвых частиц
        if (lives[i] <= 0 || sizes[i] < 0.5f) {
            RemoveParticle(i);
        }
        else {
            i++;
        }
    }
}

void OptimizedEngineEffect::Explode(Vector2 shipPos)
{
    // Создаем мощный взрыв с большим количеством частиц
    int particlesToCreate = 50 + GetRandomValue(0, 30);
    float explosionPower = 800.0f;

    for (int i = 0; i < count; i++) {
        RemoveParticle(i);
    }

    for (int i = 0; i < particlesToCreate && count < MAX_PARTICLES; i++) {
        AddExplosionParticle(shipPos, explosionPower);
    }
}

void OptimizedEngineEffect::UpdateExplosion(float dt)
{
    // Обновление только частиц взрыва (можно добавить флаг для идентификации типа частиц)
    for (int i = 0; i < count; ) {
        // Более интенсивная физика для взрыва
        positions[i].x += velocities[i].x * dt;
        positions[i].y += velocities[i].y * dt;

        // Добавляем гравитацию для эффекта падения
        velocities[i].y += 200.0f * dt; // гравитация

        // Меньшее затухание для более длительного полета
        float damping = 1.0f - (0.02f * 60.0f * dt);
        velocities[i] = Vector2Scale(velocities[i], damping);

        // Быстрое уменьшение размера для эффекта рассеивания
        lives[i] -= dt;
        sizes[i] *= (1 - 3.0f * dt);

        // Удаление мертвых частиц
        if (lives[i] <= 0 || sizes[i] < 0.3f) {
            RemoveParticle(i);
        }
        else {
            i++;
        }
    }
}

void OptimizedEngineEffect::Draw() {
    for (int i = 0; i < count; i++) {
        // Более плавное изменение альфа-канала
        float alpha = lives[i] * 2.0f;

        // Градиент цвета от желтого к красному в зависимости от времени жизни
        Color color;
        if (lives[i] > 0.3f) {
            // Ярко-желтый в начале жизни
            color = {
                255,
                255,
                (unsigned char)(100 * alpha),
                (unsigned char)(255 * alpha)
            };
        }
        else {
            // Переход к красному при затухании
            float transition = lives[i] / 0.3f;
            color = {
                255,
                (unsigned char)(255 * transition * alpha),
                (unsigned char)(50 * transition * alpha),
                (unsigned char)(255 * alpha)
            };
        }

        // Рисуем частицу с градиентом
        DrawCircleV(positions[i], sizes[i] / 2, color);

        // Для больших частиц добавляем свечение
        if (sizes[i] > 20.0f) {
            Color glowColor = { 255, 200, 100, (unsigned char)(100 * alpha) };
            DrawCircleV(positions[i], sizes[i], glowColor);
        }
    }
}

void OptimizedEngineEffect::AddParticle(Vector2 shipPos, float shipAngle, float shipSpeed, float shipSize)
{
    // Более разнообразные параметры для частиц двигателя
    float angleVariation = GetRandomValue(-25, 25) * DEG2RAD;
    float angle = shipAngle + angleVariation;

    // Разная скорость частиц
    float speed = 50.0f + (float)GetRandomValue(0, 150);

    // Разный размер частиц
    float size = 3.0f + (float)GetRandomValue(0, (int)(shipSize * 0.7f));

    // Разное время жизни
    float life = 0.2f + (float)GetRandomValue(0, 20) * 0.01f;

    // Случайное смещение от центра сопла
    float offsetX = -20.0f + (float)GetRandomValue(-10, 10);
    float offsetY = (float)GetRandomValue(-8, 8);
    Vector2 localOffset = { offsetX, offsetY };

    positions[count] = Vector2Add(shipPos, Vector2Rotate(localOffset, shipAngle));

    // Комбинируем скорость выброса и скорость корабля
    velocities[count] = Vector2Add(
        Vector2Rotate({ -speed, 0 }, angle),
        Vector2Rotate({ shipSpeed * 0.7f, 0 }, shipAngle)
    );

    lives[count] = life;
    sizes[count] = size;

    count++;
}

void OptimizedEngineEffect::AddExplosionParticle(Vector2 shipPos, float explosionPower)
{
    // Равномерное распределение по всем направлениям
    float angle = GetRandomValue(0, 360) * DEG2RAD;

    // Разная скорость для разнообразия
    float speedVariation = 0.3f + (float)GetRandomValue(0, 100) / 100.0f;
    float speed = explosionPower * speedVariation;

    // Разный размер частиц (больший разброс для взрыва)
    float size = 8.0f + (float)GetRandomValue(0, 600) / 10.0f;

    // Большее время жизни для эффектного взрыва
    float life = 0.8f + (float)GetRandomValue(0, 40) / 100.0f;

    // Случайное смещение от центра взрыва для объема
    float offsetDist = (float)GetRandomValue(0, 150) / 50.0f;
    Vector2 offset = Vector2Rotate({ offsetDist, 0 }, angle);

    positions[count] = Vector2Add(shipPos, offset);
    velocities[count] = Vector2Rotate({ speed, 0 }, angle);
    lives[count] = life;
    sizes[count] = size;

    count++;
}

void OptimizedEngineEffect::RemoveParticle(int index)
{
    // Заменяем удаляемую частицу последней в массиве и уменьшаем счетчик
    if (count > 0 && index < count) {
        positions[index] = positions[count - 1];
        velocities[index] = velocities[count - 1];
        lives[index] = lives[count - 1];
        sizes[index] = sizes[count - 1];
        count--;
    }
}