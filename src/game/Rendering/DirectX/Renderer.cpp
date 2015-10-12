#include "PrecompiledHeader.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderProgram.h"

void dd::Renderer::Initialize()
{
	StaticSystem::Set(this);

	// Initialize GLFW
	if (!glfwInit()) {
		LOG_ERROR("GLFW: Initialization failed");
		exit(EXIT_FAILURE);
	}

	// Create a window
	GLFWmonitor* monitor = nullptr;
	if (m_Fullscreen) {
		monitor = glfwGetPrimaryMonitor();
	}
	glfwWindowHint(GLFW_SAMPLES, 8);
	m_Window = glfwCreateWindow(m_Resolution.Width, m_Resolution.Height, "daydream", monitor, nullptr);
	if (!m_Window) {
		LOG_ERROR("GLFW: Failed to create window");
		exit(EXIT_FAILURE);
	}
	//glfwMakeContextCurrent(m_Window);

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = glfwGetWin32Window(m_Window);
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_SwapChain,
		&m_Device,
		NULL,
		&m_DeviceContext);

	// Set the back buffer render target
	ID3D11Texture2D* pBackBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_BackBuffer);
	pBackBuffer->Release();
	// Depth buffer
	D3D11_TEXTURE2D_DESC texd = { };
	texd.Width = m_Resolution.Width;
	texd.Height = m_Resolution.Height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 4;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ID3D11Texture2D* pDepthBuffer;
	m_Device->CreateTexture2D(&texd, NULL, &pDepthBuffer);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = { };
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	m_Device->CreateDepthStencilView(pDepthBuffer, &dsvd, &m_DepthBuffer);
	pDepthBuffer->Release();
	// Set back and depth buffer
	m_DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);

	// Rasterizer settings
	D3D11_RASTERIZER_DESC rd;
	memset(&rd, 0, sizeof(D3D11_RASTERIZER_DESC));
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = true;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;
	ID3D11RasterizerState* rasterState;
	m_Device->CreateRasterizerState(&rd, &rasterState);
	m_DeviceContext->RSSetState(rasterState);

	// Set the viewport
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_Resolution.Width;
	viewport.Height = m_Resolution.Height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	m_DeviceContext->RSSetViewports(1, &viewport);


	testModel = ResourceManager::Load<Model>("Models/Core/UnitSphere.obj");

	shaderProgram = new ShaderProgram();
	shaderProgram->CompileVertexShader(L"vertex.hlsl");
	shaderProgram->CompilePixelShader(L"pixel.hlsl");
	D3D11_INPUT_ELEMENT_DESC VertexIED[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	shaderProgram->CreateInputLayout(VertexIED, sizeof(VertexIED) / sizeof(VertexIED[0]));
	shaderProgram->Bind();

	// Create constant buffer
	// TODO: Put this in shader
	D3D11_BUFFER_DESC bd = { };
	bd.ByteWidth = 64*6;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Device->CreateBuffer(&bd, NULL, &constantBuffer);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);
}

void dd::Renderer::Draw(RenderQueueCollection& rq)
{
	float color[] = { 0.f, 0.f, 0.f, 1.f };
	m_DeviceContext->ClearRenderTargetView(m_BackBuffer, color);
	m_DeviceContext->ClearDepthStencilView(m_DepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

	constantBufferStruct.MVP = GLMtoDX(glm::mat4(1.f));
	constantBufferStruct.ModelMatrix = GLMtoDX(glm::mat4(1.f));
	constantBufferStruct.ViewMatrix = GLMtoDX(glm::mat4(1.f));
	constantBufferStruct.ProjectionMatrix = GLMtoDX(glm::mat4(1.f));
	constantBufferStruct.InverseTransposeViewModel = GLMtoDX(glm::mat4(1.f));
	constantBufferStruct.LightPosition = DirectX::SimpleMath::Vector3(0.f, 0.f, 100.f);

	m_DeviceContext->UpdateSubresource(constantBuffer, 0, 0, &constantBufferStruct, 0, 0);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Model::Vertex);
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, &testModel->VertexBuffer, &stride, &offset);

	for (auto matGroup : testModel->TextureGroups) {
		//m_DeviceContext->PSSetShaderResources(0, 1, &matGroup.Texture);
		m_DeviceContext->Draw(matGroup.EndIndex - matGroup.StartIndex, matGroup.StartIndex);
	}

	m_SwapChain->Present(0, 0);
}

//void dd::Renderer::Resize(int width, int height)
//{
//	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
//	m_BackBuffer->Release();
//
//	HRESULT hr; // TODO: Error handling
//	// Preserve the existing buffer count and format.
//	hr = m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
//
//	ID3D11Texture2D* pBuffer;
//	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
//	hr = m_Device->CreateRenderTargetView(pBuffer, NULL, &m_BackBuffer);
//	pBuffer->Release();
//	m_DeviceContext->OMSetRenderTargets(1, &BackBuffer, NULL);
//
//	// Set the viewport
//	D3D11_VIEWPORT viewport = { 0 };
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = width;
//	viewport.Height = height;
//	m_DeviceContext->RSSetViewports(1, &viewport);
//}

dd::Renderer::~Renderer()
{
	m_SwapChain->Release();
	m_BackBuffer->Release();
	m_Device->Release();
	m_DeviceContext->Release();
}

DirectX::SimpleMath::Matrix dd::Renderer::GLMtoDX(glm::mat4 gm)
{
	gm = glm::transpose(gm);
	DirectX::SimpleMath::Matrix dm(
		gm[0][0], gm[0][1], gm[0][2], gm[0][3],
		gm[1][0], gm[1][1], gm[1][2], gm[1][3],
		gm[2][0], gm[2][1], gm[2][2], gm[2][3],
		gm[3][0], gm[3][1], gm[3][2], gm[3][3]
	);
	return dm;
}
