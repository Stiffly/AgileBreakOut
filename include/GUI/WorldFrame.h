#ifndef GUI_WorldFrame_h__
#define GUI_WorldFrame_h__

#include "GUI/Frame.h"
#include "GUI/Viewport.h"
#include "RenderQueue.h"
#include "World.h"
#include "Systems/TransformSystem.h"
#include "Events/SetViewportCamera.h"
#include "Components/Transform.h"
#include "Components/Model.h"
#include "Components/Sprite.h"
#include "Components/PointLight.h"
#include "Components/BlendMap.h"


namespace GUI
{

class WorldFrame : public Frame
{
public:
	WorldFrame(Frame* parent, std::string name, std::shared_ptr<World> world)
		: Frame(parent, name)
		, m_World(world)
	{
		EVENT_SUBSCRIBE_MEMBER(m_ESetViewportCamera, &WorldFrame::OnSetViewportCamera);
	}

	void Update(double dt) override
	{
		if (!m_TransformSystem)
			m_TransformSystem = m_World->GetSystem<Systems::TransformSystem>();

		m_World->Update(dt);
	}

	void Draw(std::shared_ptr<Renderer> renderer) override
	{
		RenderQueue.Clear();
		renderer->ClearPointLights();

		for (auto &pair : *m_World->GetEntities())
		{
			EntityID entity = pair.first;

			auto templateComponent = m_World->GetComponent<Components::Template>(entity);
			if (templateComponent)
				continue;

			auto transform = m_World->GetComponent<Components::Transform>(entity);
			if (!transform)
				continue;

			auto modelComponent = m_World->GetComponent<Components::Model>(entity);
			if (modelComponent)
			{
				Model* modelAsset = nullptr;
				if (modelComponent->AverageNormals)
				{
					modelAsset = ResourceManager->Load<Model>("AveragedModel", modelComponent->ModelFile);
				}
				else
				{
					modelAsset = ResourceManager->Load<Model>("Model", modelComponent->ModelFile);
				}

				
				if (modelAsset)
				{
					Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
					glm::mat4 modelMatrix = glm::translate(glm::mat4(), absoluteTransform.Position)
										    * glm::toMat4(absoluteTransform.Orientation)
											* glm::scale(absoluteTransform.Scale);
					auto blendmapComponent = m_World->GetComponent<Components::BlendMap>(entity);
					if (blendmapComponent)
					{
						BlendMapTexture RedTexture;
						RedTexture.Diffuse = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureRed);
						RedTexture.Normal = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureRedNormal);
						RedTexture.Specular = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureRedSpecular);

						BlendMapTexture GreenTexture;
						GreenTexture.Diffuse = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureGreen);
						GreenTexture.Normal = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureGreenNormal);
						GreenTexture.Specular = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureGreenSpecular);

						BlendMapTexture BlueTexture;
						BlueTexture.Diffuse = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureBlue);
						BlueTexture.Normal = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureBlueNormal);
						BlueTexture.Specular = *ResourceManager->Load<Texture>("Texture", blendmapComponent->TextureBlueSpecular);

						float textureRepeat = blendmapComponent->TextureRepeats;

						EnqueueBlendMapModel(modelAsset, RedTexture, GreenTexture, BlueTexture, textureRepeat, modelMatrix, modelComponent->Color);
					}
					else
					{
						EnqueueModel(modelAsset, modelMatrix, modelComponent->Transparent, modelComponent->Color);
					}

				}
			}

			auto spriteComponent = m_World->GetComponent<Components::Sprite>(entity);
			if (spriteComponent)
			{
				auto textureAsset = ResourceManager->Load<Texture>("Texture", spriteComponent->SpriteFile);
				if (textureAsset)
				{
					Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
					glm::quat orientation2D = glm::angleAxis(glm::eulerAngles(absoluteTransform.Orientation).z, glm::vec3(0, 0, -1));
					glm::mat4 modelMatrix = glm::translate(absoluteTransform.Position)
										  * glm::toMat4(orientation2D)
										  * glm::scale(absoluteTransform.Scale);
					EnqueueSprite(textureAsset, modelMatrix, spriteComponent->Color);
				}
			}

			auto pointLightComponent = m_World->GetComponent<Components::PointLight>(entity);
			if (pointLightComponent)
			{
				glm::vec3 position = m_TransformSystem->AbsolutePosition(entity);
				renderer->AddPointLightToDraw(
					position,
					pointLightComponent->Specular,
					pointLightComponent->Diffuse,
					pointLightComponent->specularExponent,
					pointLightComponent->ConstantAttenuation,
					pointLightComponent->LinearAttenuation,
					pointLightComponent->QuadraticAttenuation,
					pointLightComponent->Radius
					);
			}
		}

		RenderQueue.Sort();
	}

protected:
	std::shared_ptr<World> m_World;

private:
	struct BlendMapTexture
	{
		GLuint Diffuse;
		GLuint Normal;
		GLuint Specular;
	};

	EventRelay<Frame, Events::SetViewportCamera> m_ESetViewportCamera;
	bool OnSetViewportCamera(const Events::SetViewportCamera &event)
	{
		// Search next layer for viewports and update cameras
		auto itpair = m_Children[m_Layer + 1].equal_range(event.ViewportFrame);
		for (auto it = itpair.first; it != itpair.second; ++it)
		{
			auto viewportFrame = dynamic_cast<GUI::Viewport*>(it->second);
			if (!viewportFrame)
				continue;

			viewportFrame->SetCameraEntity(event.CameraEntity);
		}

		return true;
	}

	std::shared_ptr<Systems::TransformSystem> m_TransformSystem;

	void EnqueueModel(Model* model, glm::mat4 modelMatrix, float transparent, glm::vec4 color)
	{
		for (auto texGroup : model->TextureGroups)
		{
			ModelJob job;
			job.TextureID = texGroup.Texture->ResourceID;
			job.DiffuseTexture = *texGroup.Texture;
			job.NormalTexture = (texGroup.NormalMap) ? *texGroup.NormalMap : 0;
			job.SpecularTexture = (texGroup.SpecularMap) ? *texGroup.SpecularMap : 0;
			job.VAO = model->VAO;
			job.StartIndex = texGroup.StartIndex;
			job.EndIndex = texGroup.EndIndex;
			job.ModelMatrix = modelMatrix;
			job.Transparent = transparent;
			job.Color = color;

			if(job.Transparent)
			{
				RenderQueue.Forward.Add(job);
			}
			else
			{
				RenderQueue.Deferred.Add(job);
			}
		}
	}

	void EnqueueBlendMapModel(Model* model, BlendMapTexture textureRed, BlendMapTexture textureGreen,  BlendMapTexture textureBlue, float textureRepeat, glm::mat4 modelMatrix, glm::vec4 color)
	{
		for (auto texGroup : model->TextureGroups)
		{
			BlendMapModelJob job;
			job.TextureID = texGroup.Texture->ResourceID;
			job.DiffuseTexture = *texGroup.Texture;
			job.NormalTexture = (texGroup.NormalMap) ? *texGroup.NormalMap : 0;
			job.SpecularTexture = (texGroup.SpecularMap) ? *texGroup.SpecularMap : 0;
			job.BlendMapTextureRed = textureRed.Diffuse;
			job.BlendMapTextureRedNormal = textureRed.Normal;
			job.BlendMapTextureRedSpecular = textureRed.Specular;
			job.BlendMapTextureGreen = textureGreen.Diffuse;
			job.BlendMapTextureGreenNormal = textureGreen.Normal;
			job.BlendMapTextureGreenSpecular = textureGreen.Specular;
			job.BlendMapTextureBlue = textureBlue.Diffuse;
			job.BlendMapTextureBlueNormal = textureBlue.Normal;
			job.BlendMapTextureBlueSpecular = textureBlue.Specular;
			job.TextureRepeat = textureRepeat;
			job.VAO = model->VAO;
			job.StartIndex = texGroup.StartIndex;
			job.EndIndex = texGroup.EndIndex;
			job.ModelMatrix = modelMatrix;
			job.Color = color;

			RenderQueue.Deferred.Add(job);
		}
	}

	void EnqueueSprite(Texture* texture, glm::mat4 modelMatrix, glm::vec4 color)
	{
		SpriteJob job;
		job.TextureID = texture->ResourceID;
		job.Texture = *texture;
		job.ModelMatrix = modelMatrix;
		job.Color = color;

		RenderQueue.Forward.Add(job);
	}

};

}

#endif // GUI_WorldFrame_h__
