#include "WinMain.h"

kit::Engine::CONFIG Setup() {
	kit::Engine::CONFIG config;
	config.mlp_str = (LPSTR)GAME_TITLE;
	config.mc_frameRate.Numerator(60);
	config.mv_windowSize = kit::vec2(600, 600);
//	config.msptr_startScene = std::make_shared<TestScene>();
	return config;
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_engine.reset(new kit::Engine::KitEngine(hInstance, nCmdShow, &Setup()));

	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			g_engine->Update();
			g_engine->Render();
		}
	}

	g_engine.release();

	return (int)msg.wParam;
}
