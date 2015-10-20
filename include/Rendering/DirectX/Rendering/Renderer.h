/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DD_RENDERER_H__
#define DD_RENDERER_H__

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <DirectXTK/SimpleMath.h>
using namespace DirectX::SimpleMath;
//#pragma comment (lib, "DirectXTK.lib")

#include "Rendering/BaseRenderer.h"

namespace dd
{

class Model;
class ShaderProgram;

class Renderer : public BaseRenderer
{
public:
	 ~Renderer();

	void Initialize() override;
	void Draw(RenderQueueCollection& rq) override;

	D3D11_RASTERIZER_DESC m_RasterDesc;

	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D11Device* m_Device = nullptr;
	ID3D11RasterizerState* m_RasterState = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;
	ID3D11RenderTargetView* m_BackBuffer = nullptr;
	ID3D11DepthStencilView* m_DepthBuffer = nullptr;
	ID3D11SamplerState* m_SamplerState = nullptr;

private:
	struct ConstantBufferStruct
	{
		Matrix MVP;
		Matrix ModelMatrix;
		Matrix ViewMatrix;
		Matrix ProjectionMatrix;
		Matrix InverseTransposeViewModel;
		Vector4 Color;
		Vector4 LightPositions[10];
		Vector4 LightColors[10];
		float LightRadii[10];
		unsigned int NumLights;
		float paddingBitch;
	} constantBufferStruct;

	ShaderProgram* m_ForwardShaderProgram = nullptr;
	ShaderProgram* m_GUIShaderProgram = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;

	Model* m_UnitQuad = nullptr;
	Texture* m_WhiteSphereTexture = nullptr;

	static DirectX::SimpleMath::Matrix GLMtoDXModelView(glm::mat4 m);
	static DirectX::SimpleMath::Matrix GLMtoDXProjection(glm::mat4 m);
	void setDefaultRasterState();
	void setGUIRasterState();
	static bool DepthSort(const std::shared_ptr<RenderJob> &i, const std::shared_ptr<RenderJob> &j) { return (i->Depth < j->Depth); }
};

}

#endif // Renderer_h__