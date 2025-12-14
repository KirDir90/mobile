// containers.h
#pragma once
#include "base.h"
#include "objects.h"
#include "units.h"

// Базовый класс-шаблон для контейнеров
template<typename T>
class ObjectContainer {
protected:
    std::vector<std::shared_ptr<T>> objects;
    float check_time = 1.0f;

public:
    ObjectContainer() = default;
    virtual ~ObjectContainer() = default;

    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;

    virtual void AddItem(std::shared_ptr<T> item) {
        objects.push_back(std::move(item));
    }

    void Clear() {
        objects.clear();
    }

    std::vector<std::shared_ptr<T>> GetElements() const {
        return objects;
    }

    int GetElementsCount() const {
        return static_cast<int>(objects.size());
    }

    virtual void RemoveNonActive() = 0;
};

// Класс-наследник для пуль
class BulletContainer : public ObjectContainer<Bullet> {
private:
    std::vector<std::shared_ptr<Bullet>> bullets_to_remove;

public:
    BulletContainer();
    void Update(float dt) override;
    void Draw() override;
    void RemoveNonActive() override;

    // Для совместимости со старым кодом
    std::vector<std::shared_ptr<Bullet>> GetElements() { return objects; }
};

// Класс-наследник для юнитов
class UnitContainer : public ObjectContainer<Unit> {
public:
    UnitContainer();
    void Update(float dt) override;
    void Draw() override;
    void RemoveNonActive() override;

    // Для совместимости со старым кодом
    std::vector<std::shared_ptr<Unit>> GetElements() { return objects; }
};

// Новый класс для астероидов
class AsteroidContainer : public ObjectContainer<Asteroid> {
    Color GenerateRandomAsteroidColor();
public:
    AsteroidContainer();
    void AddItem(std::shared_ptr<Asteroid> item) override;
    void Update(float dt) override;
    void Draw() override;
    void RemoveNonActive() override;

    // Дополнительные методы для астероидов, если нужны
    std::vector<std::shared_ptr<Asteroid>> GetAsteroidsInRadius(Vector2 center, float radius);

    // Для совместимости со старым кодом
    std::vector<std::shared_ptr<Asteroid>> GetElements() { return objects; }
};