#pragma once
#include "base.h"
#include "game.h"
#include "menu_templates.h"

std::unique_ptr<Menu> CreateMainMenu();
std::unique_ptr<Menu> CreateOptionMenu();
std::unique_ptr<Menu> CreateGameMenu();
std::unique_ptr<UIList> CreateGameRunningInterface();