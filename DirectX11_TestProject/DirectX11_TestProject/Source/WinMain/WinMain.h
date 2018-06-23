#pragma once

#include <Windows.h>
#include <d3d11.h>

#include "../../KitEngine/KitEngine.h"
#include "../TestScene/TestScene.h"

#define GAME_TITLE "TestProject"

static std::unique_ptr<kit::Engine::KitEngine> g_engine;

kit::Engine::CONFIG Setup();