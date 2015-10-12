#include "PrecompiledHeader.h"
#include "Rendering/ShaderProgram.h"

dd::ShaderProgram::ShaderProgram()
{
	m_VertexShaderBlob = nullptr;
	m_VertexShader = nullptr;
	m_PixelShaderBlob = nullptr;
	m_PixelShader = nullptr;
	m_InputLayout = nullptr;
}

dd::ShaderProgram::~ShaderProgram()
{
	if (m_VertexShader != nullptr)
		m_VertexShader->Release();
	if (m_VertexShaderBlob != nullptr)
		m_VertexShaderBlob->Release();
	if (m_PixelShader != nullptr)
		m_PixelShader->Release();
	if (m_PixelShaderBlob != nullptr)
		m_PixelShaderBlob->Release();
	if (m_InputLayout != nullptr)
		m_InputLayout->Release();
}

void dd::ShaderProgram::CompileVertexShader(std::wstring filename)
{
	//D3DX11CompileFromFile(filename.c_str(), 0, 0, "main", "vs_5_0", 0, 0, 0, &m_VertexShaderBlob, 0, 0);
	D3DCompileFromFile(filename.c_str(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &m_VertexShaderBlob, nullptr);
	StaticSystem::Get<Renderer>()->m_Device->CreateVertexShader(m_VertexShaderBlob->GetBufferPointer(), m_VertexShaderBlob->GetBufferSize(), NULL, &m_VertexShader);
}

void dd::ShaderProgram::CompilePixelShader(std::wstring filename)
{
	//D3DX11CompileFromFile(filename.c_str(), 0, 0, "main", "ps_5_0", 0, 0, 0, &m_PixelShaderBlob, 0, 0);
	D3DCompileFromFile(filename.c_str(), nullptr, nullptr, "main", "ps_5_0", 0, 0, &m_PixelShaderBlob, nullptr);
	StaticSystem::Get<Renderer>()->m_Device->CreatePixelShader(m_PixelShaderBlob->GetBufferPointer(), m_PixelShaderBlob->GetBufferSize(), NULL, &m_PixelShader);
}

void dd::ShaderProgram::Bind()
{
	if (m_VertexShader != nullptr)
		StaticSystem::Get<Renderer>()->m_DeviceContext->VSSetShader(m_VertexShader, 0, 0);
	if (m_PixelShader != nullptr)
		StaticSystem::Get<Renderer>()->m_DeviceContext->PSSetShader(m_PixelShader, 0, 0);
	if (m_InputLayout != nullptr)
		StaticSystem::Get<Renderer>()->m_DeviceContext->IASetInputLayout(m_InputLayout);
}

void dd::ShaderProgram::Unbind()
{
	if (m_VertexShader != nullptr) {
		StaticSystem::Get<Renderer>()->m_DeviceContext->VSSetShader(NULL, 0, 0);
	}
	if (m_PixelShader != nullptr) {
		StaticSystem::Get<Renderer>()->m_DeviceContext->PSSetShader(NULL, 0, 0);
	}
	//if (m_InputLayout != nullptr)
	//	dd::RENDERER->DeviceContext->IASetInputLayout(NULL);
}

HRESULT dd::ShaderProgram::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* ied, UINT numElements)
{
	return StaticSystem::Get<Renderer>()->m_Device->CreateInputLayout(ied, numElements, m_VertexShaderBlob->GetBufferPointer(), m_VertexShaderBlob->GetBufferSize(), &m_InputLayout);
}
