#ifndef ShaderProgram_h__
#define ShaderProgram_h__

#include <string>

#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment (lib, "D3DCompiler.lib")
#include "Core/StaticSystem.h"
#include "Rendering/Renderer.h"

namespace dd
{

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void CompileVertexShader(std::wstring filename);
	void CompilePixelShader(std::wstring filename);

	HRESULT CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* ied, UINT numElements);
	ID3D11InputLayout* GetInputLayout() const { return m_InputLayout; }

	void Bind();
	void Unbind();

private:
	ID3D10Blob* m_VertexShaderBlob;
	ID3D11VertexShader* m_VertexShader;
	ID3D10Blob* m_PixelShaderBlob;
	ID3D11PixelShader* m_PixelShader;

	ID3D11InputLayout* m_InputLayout;
};

}

#endif // ShaderProgram_h__
