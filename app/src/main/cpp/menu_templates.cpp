#include "menu_templates.h"
#include "game.h"



void UIButton::Draw()
{
    if (GuiButton({ m_position.x, m_position.y, m_size.x, m_size.y }, m_name.c_str()) && m_onClick) {
        m_onClick();
    }
}

void UIButton::JustDraw()
{
    GuiButton({ m_position.x, m_position.y, m_size.x, m_size.y }, m_name.c_str());
}



void UILabel::Draw()
{
    // Сохраняем старый цвет текста
    int oldTextColor = GuiGetStyle(LABEL, TEXT_COLOR_NORMAL);

    // Устанавливаем новый цвет текста
    if (ColorToInt(m_textColor) != ColorToInt(BLACK)) GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(m_textColor));

    // Рисуем label
    GuiLabel({ m_position.x, m_position.y, m_size.x, m_size.y }, m_name.c_str());

    // Восстанавливаем старый цвет
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, oldTextColor);

    //DrawRectangleLines(m_position.x, m_position.y, m_size.x, m_size.y, BLUE);
}

void UILabel::JustDraw()
{
    Draw();
}



UIList::UIList(Vector2 position, Vector2 size, const std::string& name, Color color, Color textColor) 
    : UIElement(position, size, name, color, textColor),
    m_margin(20), m_margin_edge(0), m_horisontal(false), m_elements({}), m_defferd({}), m_usual({}), m_drop_edit(false) {}

void UIList::Draw()
{
    for (const auto& item : m_defferd) {
        if (auto ptr = std::dynamic_pointer_cast<UIDropbox>(item)) {
            m_drop_edit = ptr->IsEditing();
        }
    }
    if (m_drop_edit) {
        for (const auto& item : m_usual) {
            item->JustDraw();
        }
    }
    else {
        for (const auto& item : m_usual) {
            item->Draw();
        }
    }
    for (const auto& item : m_defferd) {
        item->Draw();
    }
}

void UIList::JustDraw()
{
    for (const auto& item : m_usual) {
        item->JustDraw();
    }
    for (const auto& item : m_defferd) {
        item->JustDraw();
    }
}

void UIList::AddUIElement(std::shared_ptr<UIElement> item)
{
    m_elements.push_back(std::move(item));

    if (std::dynamic_pointer_cast<UIDropbox>(m_elements[m_elements.size()-1])) {
        m_defferd.push_back(m_elements[m_elements.size() - 1]);
    }
    else {
        m_usual.push_back(m_elements[m_elements.size() - 1]);
    }
    ElementsChanged();
}

UIElement& UIList::GetUIElement(int index)
{
    int max = static_cast<int>(m_elements.size()) - 1;
    index = Span(index, 0, max);
    return *m_elements[index];
}

const std::vector<std::shared_ptr<UIElement>> UIList::GetElements() const
{
    return m_elements;
}

template<typename T>

T* UIList::GetElementAs(int index)
{
    UIElement& baseElement = GetUIElement(index);
    return dynamic_cast<T*>(&baseElement);
}

void UIList::ElementsChanged()
{
    m_margin = m_size.x * 0.01;
    int rows = 0;

    for (const auto& element : m_elements) {
        rows += element->GetSizeCoeff();
    }

    if (!m_horisontal) {
        float re_rows = 1.0f;
        if (rows != 0) re_rows = 1.0f / rows;
        float last_y = m_position.y;

        for (const auto& item : m_elements) {
            item->SetPosition({ m_position.x + m_margin_edge, last_y + m_margin_edge });
            item->SetSize({ m_size.x - 2 * m_margin_edge, (m_size.y - 2 * m_margin_edge + m_margin) * re_rows * item->GetSizeCoeff() - m_margin });
            last_y += item->GetSize().y + m_margin;
        }
    }
    else {
        float re_columns = 1.0f;
        if (rows != 0) re_columns = 1.0f / rows;
        float last_x = m_position.x;

        for (const auto& item : m_elements) {
            item->SetPosition({ last_x + m_margin_edge, m_position.y + m_margin_edge });
            item->SetSize({ (m_size.x - 2 * m_margin_edge + m_margin) * re_columns * item->GetSizeCoeff() - m_margin, m_size.y - 2 * m_margin_edge });
            last_x += item->GetSize().x + m_margin;
        }
    }
}



Menu::Menu(Vector2 position, Vector2 size, const std::string& name, Color color, Color textColor)
    : UIList(position, size, name, color, textColor) {
    m_margin_edge = 40;
    SetHorisontal(false);
}

void Menu::Draw()
{
    GuiGroupBox({ m_position.x, m_position.y, m_size.x, m_size.y }, m_name.c_str());
    UIList::Draw();
}



void UICheckbox::Draw()
{
    GuiLabel({ m_position.x, m_position.y, m_size.x, m_size.y }, m_name.c_str());
    GuiCheckBox({ m_position.x + m_size.x - m_size.y * 0.5f, m_position.y + m_size.y * 0.25f, m_size.y * 0.5f, m_size.y * 0.5f }, "", &m_value);
}

void UICheckbox::JustDraw()
{
    Draw();
}



void UISlider::Draw()
{
    std::string value = std::floor(m_value) == 121 ? "Unlimited" : std::to_string((int)std::floor(m_value));
    GuiLabel({ m_position.x, m_position.y, m_size.x * 0.5f, m_size.y },
        (m_name + " " + value).c_str());
    GuiSlider({ m_position.x + m_size.x * 0.5f, m_position.y + m_size.y * 0.25f, m_size.x * 0.5f, m_size.y * 0.5f },
        "", "",
        &m_value, min_value, max_value);
}

void UISlider::JustDraw()
{
    Draw();
}



void UIDropbox::Draw() {
    if (GuiDropdownBox({ m_position.x, m_position.y, m_size.x, m_size.y }, items.c_str(), &m_selected, editMode)) {
        editMode = !editMode;
        std::cout << GetValue() << "\n";
    }
}

void UIDropbox::JustDraw()
{
    GuiDropdownBox({ m_position.x, m_position.y, m_size.x, m_size.y }, items.c_str(), &m_selected, editMode);
}
