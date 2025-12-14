#pragma once
#include <iostream>
#include "raymob.h"
#include "chipmunk.h"
#include <vector>
#include <cstring>
#include <raygui.h>
#include <memory>
#include <functional>
#include <algorithm>
#include "json.hpp"
#include <fstream>
#include <filesystem>
#include <execution>
#include "raymath.h"
#include "constants.h"
#include <random>

using json = nlohmann::json;

extern bool gameRunning;
extern Image icon;

class Object {
public:
    virtual ~Object() = default;
};

enum class InterfaceState {
    MAIN_MENU,
    OPTIONS_MENU,
    GAME_MENU,
    GAME_RUNNING
};

enum class SceneState {
    MENU_MODE,
    TEAM_BATTLE,
    WAVE_MODE,
    BOSS_FIGHT,
    FREE_MODE
};

enum class BackgroundType {
    COMMON,
    COLORED
};

template<typename T>
T Span(T element, T min, T max) {
    if (element > max) element = max;
    if (element < min) element = min;
    return element;
}

inline float PolygonArea(const std::vector<cpVect>& vertices)
{
    int count = (int)vertices.size();
    return std::abs(cpAreaForPoly(count, vertices.data(), 0.0f));
}

inline int Mod(int a, int b)
{
    return (a % b + b) % b;
}

inline Vector2 cpVectToVector2(cpVect vect) {
    return  { (float)vect.x, (float)vect.y };
}

inline cpVect Vector2TocpVect(Vector2 vect) {
    return  { vect.x, vect.y };
}

inline cpVect cpvdiv(cpVect v, cpFloat s) {
    return cpv(v.x / s, v.y / s);
}

inline Rectangle GetCameraRectangle2D(Camera2D camera) {
    // �������� ����� ������
    Vector2 center = camera.target;

    // ��������� ������ ������� �������
    float azoom = 1.0f / camera.zoom;
    float width = (float)GetScreenWidth() * azoom;
    float height = (float)GetScreenHeight() * azoom;

    // ��������� �������� (offset)
    Vector2 offset = camera.offset;
    center.x -= offset.x * azoom;
    center.y -= offset.y * azoom;

    // ���������� �������������
    return Rectangle{
            center.x,
            center.y,
            width,
            height
    };
}

inline Rectangle GetCameraCollider2D(Camera2D camera, float obj) {
    Rectangle cam = GetCameraRectangle2D(camera);
    return Rectangle{
        cam.x - obj * 0.5f - 10,
        cam.y - obj * 0.5f - 10,
        cam.width + obj + 20,
        cam.height + obj + 20
    };
}

inline Color BlendOverlay(Color a, Color b) {
    Color result;
    result.r = (a.r < 128) ? (2 * a.r * b.r / 255) : (255 - 2 * (255 - a.r) * (255 - b.r) / 255);
    result.g = (a.g < 128) ? (2 * a.g * b.g / 255) : (255 - 2 * (255 - a.g) * (255 - b.g) / 255);
    result.b = (a.b < 128) ? (2 * a.b * b.b / 255) : (255 - 2 * (255 - a.b) * (255 - b.b) / 255);
    result.a = (unsigned char)std::min(255, a.a + b.a);
    return result;
}

inline bool CheckLineLineCollision(Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2, Vector2* intersection = nullptr) {
    float denominator = ((a2.x - a1.x) * (b2.y - b1.y) - (a2.y - a1.y) * (b2.x - b1.x));

    // ������� �����������
    if (fabs(denominator) < 1e-10f) {
        return false;
    }

    float ua = ((b2.x - b1.x) * (a1.y - b1.y) - (b2.y - b1.y) * (a1.x - b1.x)) / denominator;
    float ub = ((a2.x - a1.x) * (a1.y - b1.y) - (a2.y - a1.y) * (a1.x - b1.x)) / denominator;

    // �������� ��� ����������� ������ ����� ��������
    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
        if (intersection) {
            intersection->x = a1.x + ua * (a2.x - a1.x);
            intersection->y = a1.y + ua * (a2.y - a1.y);
        }
        return true;
    }

    return false;
}

inline bool CheckPointsInSameCells(Vector2 p1, Vector2 p2, int cell_size) {
    // �������� �������� ������� - ���������� ���������
    int inv_cell_size = 1.0f / cell_size; // �������������� ���������

    int cell_x1 = static_cast<int>(p1.x * inv_cell_size);
    int cell_y1 = static_cast<int>(p1.y * inv_cell_size);
    int cell_x2 = static_cast<int>(p2.x * inv_cell_size);
    int cell_y2 = static_cast<int>(p2.y * inv_cell_size);

    return (cell_x1 == cell_x2) && (cell_y1 == cell_y2);
}

inline bool CheckPointsDist(Vector2 p1, Vector2 p2, int target_dist) {
    return Vector2DistanceSqr(p1, p2) <= target_dist * target_dist;
}

inline bool CheckLinePolygonCollision(Vector2 lineStart, Vector2 lineEnd,
    const std::vector<Vector2>& polygonVertices,
    std::vector<Vector2>* intersections = nullptr) {

    if (polygonVertices.size() < 3) {
        return false; // �� �������
    }

    bool collision = false;

    // ��������� ����������� � ������ �������� ��������
    for (size_t i = 0; i < polygonVertices.size(); i++) {
        size_t next = (i + 1) % polygonVertices.size();

        Vector2 intersection;
        if (CheckLineLineCollision(lineStart, lineEnd,
            polygonVertices[i], polygonVertices[next],
            &intersection)) {
            collision = true;

            if (intersections) {
                intersections->push_back(intersection);
            }
            else {
                // ���� �� ����� ����� �����������, ����� ����� ��� ������ ���������
                return true;
            }
        }
    }

    return collision;
}

inline std::optional<Vector2> GetLineRectIntersectionSimple(Rectangle rect, Vector2 lineStart, Vector2 lineEnd) {
    // ��������� ����������� � ������ �������� ��������������
    Vector2 intersections[4];
    bool found = false;

    // ������� �������
    if (CheckCollisionLines(lineStart, lineEnd,
        { rect.x, rect.y }, { rect.x + rect.width, rect.y },
        &intersections[0])) {
        found = true;
        return intersections[0];
    }
    // ������ �������
    else if (CheckCollisionLines(lineStart, lineEnd,
        { rect.x + rect.width, rect.y }, { rect.x + rect.width, rect.y + rect.height },
        &intersections[1])) {
        found = true;
        return intersections[1];
    }
    // ������ �������
    else if (CheckCollisionLines(lineStart, lineEnd,
        { rect.x, rect.y + rect.height }, { rect.x + rect.width, rect.y + rect.height },
        &intersections[2])) {
        found = true;
        return intersections[2];
    }
    // ����� �������
    else if (CheckCollisionLines(lineStart, lineEnd,
        { rect.x, rect.y }, { rect.x, rect.y + rect.height },
        &intersections[3])) {
        found = true;
        return intersections[3];
    }

    // ���������� ������ ��������� �����
    for (int i = 0; i < 4; i++) {
        if (intersections[i].x != 0 || intersections[i].y != 0) {
            return intersections[i];
        }
    }

    return std::nullopt;
}

inline int extract_number(const std::string& str) {
    int result = 0;
    bool negative = false;
    bool found_digit = false;

    for (size_t i = 0; i < str.size(); i++) {
        char c = str[i];

        // ��������� ����� ������ ����� �������
        if (!found_digit) {
            if (c == '-') {
                negative = true;
                continue;
            }
            else if (c == '+') {
                continue;
            }
        }

        // ���� ��� �����
        if (c >= '0' && c <= '9') {
            found_digit = true;

            // �������� �� ������������
            if (result > (std::numeric_limits<int>::max() - (c - '0')) / 10) {
                return negative ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
            }

            result = result * 10 + (c - '0');
        }
        // ���� ����� ����� � ��������� ��-����� - ���������������
        else if (found_digit) {
            break;
        }
    }

    return negative ? -result : result;
}



