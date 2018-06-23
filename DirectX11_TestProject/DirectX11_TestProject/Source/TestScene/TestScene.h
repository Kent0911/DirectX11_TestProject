#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "../../KitEngine/KitEngine.h"
#include "../../KitEngine/Scene/Scene.h"

typedef struct VERTEX_ {
	float pos[3];
	float col[4];
	float tex[2];
};

struct ConstantBuffer {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

class Triangle : public kit::Engine::KitBehaviour {
public:
	Triangle();
	~Triangle();

	virtual void Update();
	virtual void Render();

	VERTEX_ mv_triangle[3];
	DWORD md_indexList[3 * 1];
private:
};

class TestScene :public kit::Engine::Scene {
public:
	TestScene();
	~TestScene();

	virtual void Update();
	virtual void Render();

private:
	std::unique_ptr<Triangle> m_triangle;

	// ワールド変換行列
	DirectX::XMMATRIX mx_world;

	// ビュー変換行列
	DirectX::XMMATRIX mx_view;

	// 射影変換行列
	DirectX::XMMATRIX mx_projection;

	HRESULT Create(HWND _hWnd);
};