#pragma once
#include "base.h"


class UIElement {
protected:
    Vector2 m_position = { 0.0f, 0.0f };
    Vector2 m_size = { 0.0f, 0.0f };
    Color m_color = RAYWHITE;
    Color m_textColor = BLACK;
    std::string m_name;
    int m_size_coeff = 1;

public:
    UIElement(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK)
        : m_position(position), m_size(size), m_name(name),
        m_color(color), m_textColor(textColor), m_size_coeff(1) {
    }
    UIElement(int size_coeff) : UIElement() { m_size_coeff = size_coeff; };

    // Геттеры и сеттеры
    Vector2 GetPosition() const { return m_position; }
    virtual void SetPosition(Vector2 position) { m_position = position; }

    Vector2 GetSize() const { return m_size; }
    virtual void SetSize(Vector2 size) { m_size = size; }

    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

    Color GetColor() const { return m_color; }
    void SetColor(Color color) { m_color = color; }

    Color GetTextColor() const { return m_textColor; }
    void SetTextColor(Color textColor) { m_textColor = textColor; }

    int GetSizeCoeff() const { return m_size_coeff; }
    void SetSizeCoeff(int size_coeff) { m_size_coeff = size_coeff; }

    // Виртуальные методы
    virtual void Draw() {};
    virtual void JustDraw() {};
    //virtual void Update() {}
    virtual ~UIElement() = default;
};

class UIButton : public UIElement {
protected:
    std::function<void()> m_onClick;
public:
    UIButton(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK) : UIElement() { }
    UIButton(int size_coeff) { m_size_coeff = size_coeff; }
    void Draw() override;
    void JustDraw() override;
    void SetOnClick(std::function<void()> func) { m_onClick = std::move(func); }
};

class UICheckbox : public UIElement {
protected:
    bool m_value;
public:
    UICheckbox(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK) : UIElement(), m_value(false) { }
    UICheckbox(int size_coeff) : m_value(false) { m_size_coeff = size_coeff; }
    void Draw() override;
    void JustDraw() override;
    void SetValue(bool value) { m_value = std::move(value); }
    bool GetValue() { return m_value; }
};

class UISlider : public UIElement {
protected:
    std::string text = m_name.c_str() + std::string(" = ") + std::to_string(std::floor(m_value));
    float m_value;
    float min_value;
    float max_value;
public:
    UISlider(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK) : UIElement(), m_value(0), min_value(0), max_value(100) {
    }
    UISlider(int size_coeff) : m_value(0), min_value(0), max_value(100) { m_size_coeff = size_coeff; }
    void Draw() override;
    void JustDraw() override;
    void SetValue(float value) { m_value = std::move(value); }
    void SetMax(float new_m) { max_value = new_m; }
    void SetMin(float new_m) { min_value = new_m; }
    float GetValue() { return m_value; }
};

class UIDropbox : public UIElement{
protected:
    std::string items = "";
    int m_selected;
    bool editMode = false;
    /*std::function<void()> myDraw = []() {DefferDraw(); };
    std::function<void()> defferDraw = []() {DefferDraw(); };*/
public:
    UIDropbox(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK) : UIElement(), m_selected(0) {
    }
    UIDropbox(int size_coeff) : m_selected(0) { m_size_coeff = size_coeff; }
    void Draw() override;
    void JustDraw() override;
    /*void DefferDraw();*/
    void SetValue(int value) { m_selected = std::move(value); }
    int GetValue() { return m_selected; }
    bool IsEditing() { return editMode; }
    void AddItem(std::string item) { items != "" ? items += ";" + item : items += item; }
};

class UILabel : public UIElement {
protected:

public:
    UILabel(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK) : UIElement() {
    }
    UILabel(int size_coeff) { m_size_coeff = size_coeff; }
    void Draw() override;
    void JustDraw() override;
};

class UIList : public UIElement{
protected:
    std::vector<std::shared_ptr<UIElement>> m_elements;
    std::vector<std::shared_ptr<UIElement>> m_usual;
    std::vector<std::shared_ptr<UIElement>> m_defferd;
    float m_margin;
    float m_margin_edge;
    bool m_horisontal;
    bool m_drop_edit;
public:
    UIList(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK);

    UIList(int size_coeff) : m_margin(20), m_margin_edge(0), m_horisontal(false), m_drop_edit(false) {m_size_coeff = size_coeff;}
    void Draw() override;
    void JustDraw() override;
    void SetPosition(Vector2 position) override { UIElement::SetPosition(position); ElementsChanged(); };
    void SetSize(Vector2 size) override { UIElement::SetSize(size); ElementsChanged(); };
    void AddUIElement(std::shared_ptr<UIElement> item);
    template<typename T>
    T* GetElementAs(int index);
    UIElement& GetUIElement(int index);
    const std::vector<std::shared_ptr<UIElement>> GetElements() const;
    void ElementsChanged();
    void SetHorisontal(bool horisontal) { m_horisontal = horisontal; ElementsChanged(); }
    bool GetHorisontal() { return m_horisontal; }

};

class Menu : public UIList {
protected:

public:
    Menu(Vector2 position = { 0.0f, 0.0f },
        Vector2 size = { 0.0f, 0.0f },
        const std::string& name = "",
        Color color = RAYWHITE,
        Color textColor = BLACK);
    void Draw() override;
    void JustDraw() override {};
};