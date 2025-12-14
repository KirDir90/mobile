#include "menu_creator.h"

std::unique_ptr<Menu> CreateMainMenu() {

    float width = static_cast<float>(GetScreenWidth()) * 0.5f;
    float height = static_cast<float>(GetScreenHeight()) * 0.45f;
    auto menu = std::make_unique<Menu>(Vector2{ (static_cast<float>(GetScreenWidth()) - width) * 0.5f,
                                                (static_cast<float>(GetScreenHeight()) - height) * 0.5f },
        Vector2{ width, height },
        "Main menu",
        RAYWHITE,
        BLACK);
    // Creating elements

    auto btn = std::make_unique<UIButton>(3);
    auto list = std::make_unique<UIList>(2);
    auto quitBtn = std::make_unique<UIButton>(1);


    btn->SetName("Start");
    btn->SetOnClick([]() { game.SetInterface(InterfaceState::GAME_MENU); });
    menu->AddUIElement(std::move(btn));


    btn = std::make_unique<UIButton>(3);
    btn->SetName("Options");
    btn->SetOnClick([]() { game.SetInterface(InterfaceState::OPTIONS_MENU); });
    menu->AddUIElement(std::move(btn));

    
    list->SetHorisontal(true);
    quitBtn->SetName("Quit");
    quitBtn->SetOnClick([]() { std::cout << "Window should close\n"; gameRunning = false; });
    list->AddUIElement(std::move(quitBtn));
    list->AddUIElement(std::make_unique<UILabel>(2));
    menu->AddUIElement(std::move(list));



    return menu;
}

std::unique_ptr<Menu> CreateOptionMenu()
{
    float width = static_cast<float>(GetScreenWidth()) * 0.5f;
    float height = static_cast<float>(GetScreenHeight()) * 0.45f;
    auto menu = std::make_unique<Menu>(Vector2{ (static_cast<float>(GetScreenWidth()) - width) * 0.5f,
                                                (static_cast<float>(GetScreenHeight()) - height) * 0.5f },
        Vector2{ width, height },
        "Option menu",
        RAYWHITE,
        BLACK);

    
    // Creating and adding elements
    auto chck = std::make_unique<UICheckbox>(1);
    auto sldr = std::make_unique<UISlider>(1);
    auto btnApply = std::make_unique<UIButton>(1);
    auto quitBtn = std::make_unique<UIButton>(1);

    chck->SetName("Friendly fire");
    chck->SetValue(game.GetSetting("Friendly fire")? true : false);
    menu->AddUIElement(std::move(chck));


    chck = std::make_unique<UICheckbox>(1);
    chck->SetName("Windowed");
    chck->SetValue(game.GetSetting("Windowed")? true : false);
    menu->AddUIElement(std::move(chck));

    
    sldr->SetName("Team size");
    sldr->SetMax(100);
    sldr->SetMin(1);
    sldr->SetValue(game.GetSetting("Team size"));
    menu->AddUIElement(std::move(sldr));


    sldr = std::make_unique<UISlider>(1);
    sldr->SetName("Max fps");
    sldr->SetMax(121);
    sldr->SetMin(1);
    sldr->SetValue(game.GetSetting("Max fps"));
    menu->AddUIElement(std::move(sldr));
    

    btnApply->SetName("Apply");
    btnApply->SetOnClick([]() {
        game.UpdateSettings();
        });
    menu->AddUIElement(std::move(btnApply));


    quitBtn->SetName("Back to main");
    quitBtn->SetOnClick([]() { game.SetInterface(InterfaceState::MAIN_MENU); });
    menu->AddUIElement(std::move(quitBtn));

    return menu;
}

std::unique_ptr<Menu> CreateGameMenu()
{
    float width = static_cast<float>(GetScreenWidth()) * 0.5f;
    float height = static_cast<float>(GetScreenHeight()) * 0.45f;
    auto menu = std::make_unique<Menu>(Vector2{ (static_cast<float>(GetScreenWidth()) - width) * 0.5f,
                                                (static_cast<float>(GetScreenHeight()) - height) * 0.5f },
        Vector2{ width, height },
        "Game menu",
        RAYWHITE,
        BLACK);
    // Creating elements

    auto btn = std::make_unique<UIButton>(2);
    auto drop = std::make_shared<UIDropbox>(1);


    btn->SetName("Start");

    btn->SetOnClick([drop]() { 
        SceneState scene = 
            drop->GetValue() == 0 ? SceneState::TEAM_BATTLE :
            drop->GetValue() == 1 ? SceneState::WAVE_MODE :
            drop->GetValue() == 2 ? SceneState::BOSS_FIGHT :
                                    SceneState::FREE_MODE;
        game.GetScene()->GetWorld()->Deactivate();
        game.GetScene()->SetScene(scene);
        game.SetInterface(InterfaceState::GAME_RUNNING);
        });

    menu->AddUIElement(std::move(btn));


    btn = std::make_unique<UIButton>(1);
    btn->SetOnClick([drop]() {
        SceneState scene =
            drop->GetValue() == 0 ? SceneState::TEAM_BATTLE :
            drop->GetValue() == 1 ? SceneState::WAVE_MODE :
            drop->GetValue() == 2 ? SceneState::BOSS_FIGHT :
            SceneState::FREE_MODE;
        
        game.GetScene()->SetScene(scene);
        game.SetInterface(InterfaceState::GAME_RUNNING);
        });

    btn->SetName("Continue");
    menu->AddUIElement(std::move(btn));
    


    drop->AddItem("Team Battle");
    drop->AddItem("Wave Mode");
    drop->AddItem("Boss Fight");
    drop->AddItem("Free Mode");
    drop->SetValue(3);
    menu->AddUIElement(drop);


    btn = std::make_unique<UIButton>(1);
    btn->SetName("Ship constructor");
    btn->SetOnClick([]() { game.SetInterface(InterfaceState::GAME_MENU); });
    menu->AddUIElement(std::move(btn));


    btn = std::make_unique<UIButton>(1);
    btn->SetName("Back to main");
    btn->SetOnClick([]() { game.SetInterface(InterfaceState::MAIN_MENU); });
    menu->AddUIElement(std::move(btn));


    return menu;
}

std::unique_ptr<UIList> CreateGameRunningInterface() {
    float width = static_cast<float>(GetScreenWidth());
    float height = static_cast<float>(GetScreenHeight());
    SceneState scene = game.GetSceneState();
    std::string name = scene == SceneState::TEAM_BATTLE ? "Team battle" :
        scene == SceneState::WAVE_MODE ? "Wave mode" :
        scene == SceneState::BOSS_FIGHT ? "Boss fight" :
        "Free mode";

    auto interface = std::make_unique<Menu>(Vector2{ (static_cast<float>(GetScreenWidth()) - width) * 0.5f,
                                                (static_cast<float>(GetScreenHeight()) - height + TEXT_SIZE * 2) * 0.5f },
        Vector2{ width, height },
        name,
        RAYWHITE,
        BLACK);
    auto list = std::make_unique<UIList>(3);
    list->SetHorisontal(true);
    auto label = std::make_unique<UILabel>(17);

    auto list_2 = std::make_unique<UIList>(1);

    auto info_label = std::make_unique<UILabel>(1);
    info_label->SetName("Hp");

    list_2->SetHorisontal(false);
    list_2->AddUIElement(std::move(info_label));

    info_label = std::make_unique<UILabel>(1);
    info_label->SetName("Score");

    list_2->AddUIElement(std::move(info_label));
    list->AddUIElement(std::move(list_2));

    list_2 = std::make_unique<UIList>(1);
    auto list_3 = std::make_unique<UIList>(1);
    list_3->SetHorisontal(true);

    info_label = std::make_unique<UILabel>(6);
    list_3->AddUIElement(std::move(info_label));

    for (int i = 0; i < 3; i++) {
        info_label = std::make_unique<UILabel>(1);
        //info_label->SetName("0");
        Color color;
        switch (i) {
        case 1:
            color = RED;
            break;
        case 2:
            color = GREEN;
            break;
        default:
            color = BLUE;
            break;
        }
        info_label->SetTextColor(color);
        list_3->AddUIElement(std::move(info_label));
    }

    list_2->SetHorisontal(false);
    list_2->AddUIElement(std::move(list_3));
    list_2->AddUIElement(std::move(std::make_unique<UILabel>(1)));


    
    list->AddUIElement(std::move(list_2));

    interface->AddUIElement(std::move(list));
    interface->AddUIElement(std::move(label));
    

    return interface;
}
