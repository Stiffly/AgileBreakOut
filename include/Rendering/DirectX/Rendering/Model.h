#ifndef Model_h__
#define Model_h__

#include <string>

#include <d3d11.h>

#include "Rendering/RawModel.h"
#include "Core/StaticSystem.h"
#include "Rendering/Renderer.h"

namespace dd
{

class Model : public RawModel
{
	friend class ResourceManager;

private:
	Model(std::string fileName);

public:
	~Model();

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	unsigned int NumVertices;
};

}

#endif // Model_h__
