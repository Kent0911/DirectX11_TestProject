#include "TestScene.h"

Triangle::Triangle() {
	mv_triangle[0].pos[0] = 0.0f;
	mv_triangle[0].pos[1] = 0.0f;
	mv_triangle[0].pos[2] = 0.0f;

	mv_triangle[1].pos[0] = 0.0f;
	mv_triangle[1].pos[1] = 300.0f;
	mv_triangle[1].pos[2] = 0.0f;

	mv_triangle[2].pos[0] = 300.0f;
	mv_triangle[2].pos[1] = 0.0f;
	mv_triangle[2].pos[2] = 0.0f;

	for (int i = 0; i < 3; ++i) {
		for (int c = 0; c < 4; ++c) {
			mv_triangle[i].col[c] = 0.0f;
			if (3 == c) { mv_triangle[i].col[c] = 1.0f; }
		}
	}
	
	world.scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	world.rotationOrigin = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	world.rotation = 0.0f;
	world.translation = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

}

Triangle::~Triangle() {

}

void Triangle::Update() {

}

void Triangle::Render() {

}


TestScene::TestScene() {
	m_triangle.reset(new Triangle);

	Create(kit::Engine::g_hWnd);
}

TestScene::~TestScene() {

}

HRESULT TestScene::Create(HWND _hWnd) {
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(kit::Engine::VERTEX_) * 3;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = m_triangle.get()->mv_triangle;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	hr = kit::Engine::g_pd3dDevice->CreateBuffer(&bufferDesc, &subResourceData, &kit::Engine::g_pVertexBuffer);
	if (FAILED(hr)) { return hr; }

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 3 * 1;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = m_triangle.get()->md_indexList;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	hr = kit::Engine::g_pd3dDevice->CreateBuffer(&indexBufferDesc, &initData, &kit::Engine::g_pIndexBuffer);
	if (FAILED(hr)) { return hr; }

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	hr = kit::Engine::g_pd3dDevice->CreateBuffer(&cbDesc, nullptr, &kit::Engine::g_pConstantBuffer);
	if (FAILED(hr)) { return hr; }

	return S_OK;
}

void TestScene::Update() {

}

void TestScene::Render() {
	RECT rc;
	GetClientRect(kit::Engine::g_hWnd, &rc);
	UINT width = rc.right - rc.left + 16;
	UINT height = rc.bottom - rc.top + 31;

	// ワールド変換行列
//	mx_world = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	mx_world = DirectX::XMMatrixAffineTransformation2D(m_triangle.get()->world.scale, m_triangle.get()->world.rotationOrigin,
		m_triangle.get()->world.rotation, m_triangle.get()->world.translation);


	// ビュー変換行列
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mx_view = DirectX::XMMatrixLookAtLH(eye, at, up);

	// 透視射影変換行列
	float fov = DirectX::XMConvertToRadians(45.0f);
	float aspect = (float)height / (float)width;
	float nearZ = 0.1f;
	float farZ = 100.0f;
	mx_projection = DirectX::XMMatrixOrthographicLH((float)width, (float)height, nearZ, farZ);

	ConstantBuffer cb;
	DirectX::XMStoreFloat4x4(&cb.world, DirectX::XMMatrixTranspose(mx_world));
	DirectX::XMStoreFloat4x4(&cb.view, DirectX::XMMatrixTranspose(mx_view));
	DirectX::XMStoreFloat4x4(&cb.projection, DirectX::XMMatrixTranspose(mx_projection));

	kit::Engine::g_pImmediateContext->UpdateSubresource(kit::Engine::g_pConstantBuffer, 0, NULL, &cb, 0, 0);

	float clearColor[4] = { 0.0f,0.5f,1.0f,1.0f };
	kit::Engine::g_pImmediateContext->ClearRenderTargetView(kit::Engine::g_pRenderTargetView, clearColor);
	kit::Engine::g_pImmediateContext->ClearDepthStencilView(kit::Engine::g_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtv[1] = { kit::Engine::g_pRenderTargetView };
	kit::Engine::g_pImmediateContext->OMSetRenderTargets(1, rtv, kit::Engine::g_pDepthStencilView);

	// Setup viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	kit::Engine::g_pImmediateContext->RSSetViewports(1, &vp);


	UINT strides[] = { sizeof(kit::Engine::VERTEX_) };
	UINT offsets[] = { 0 };
	kit::Engine::g_pImmediateContext->IASetInputLayout(kit::Engine::g_pInputLayout);
	kit::Engine::g_pImmediateContext->IASetVertexBuffers(0, 1, &kit::Engine::g_pVertexBuffer, strides, offsets);
	kit::Engine::g_pImmediateContext->IASetIndexBuffer(kit::Engine::g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	kit::Engine::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	kit::Engine::g_pImmediateContext->VSSetConstantBuffers(0, 1, &kit::Engine::g_pConstantBuffer);
	kit::Engine::g_pImmediateContext->VSSetShader(kit::Engine::g_pVertexShader, nullptr, 0);
	kit::Engine::g_pImmediateContext->PSSetShader(kit::Engine::g_pPixcelShader, nullptr, 0);
	kit::Engine::g_pImmediateContext->OMSetRenderTargets(1, &kit::Engine::g_pRenderTargetView, kit::Engine::g_pDepthStencilView);

	kit::Engine::g_pImmediateContext->Draw(3, 0);
}