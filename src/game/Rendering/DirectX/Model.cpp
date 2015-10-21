#include "PrecompiledHeader.h"
#include "Rendering/Model.h"

dd::Model::Model(std::string fileName)
	: dd::RawModel(fileName)
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.ByteWidth = sizeof(RawModel::Vertex) * m_Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vsrd = { m_Vertices.data(), 0, 0 };
	StaticSystem::Get<Renderer>()->m_Device->CreateBuffer(&vbd, &vsrd, &VertexBuffer);

	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = sizeof(unsigned int) * m_Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA idata = { 0 };
	idata.pSysMem = m_Indices.data();
	StaticSystem::Get<Renderer>()->m_Device->CreateBuffer(&ibd, &idata, &IndexBuffer);
}

