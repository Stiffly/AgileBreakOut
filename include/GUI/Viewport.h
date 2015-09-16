//
// Created by Administrator on 2015-09-16.
//

#ifndef DAYDREAM_VIEWPORT_H
#define DAYDREAM_VIEWPORT_H

#include <memory>

#include "GUI/Frame.h"
#include "Core/World.h"
#include "Transform/TransformSystem.h"
#include "Core/CTransform.h"
#include "Rendering/CCamera.h"
#include "Core/RenderQueue.h"
#include "Core/Camera.h"

namespace GUI
{

    class Viewport : public Frame
    {
    public:
        Viewport(Frame* parent, std::string name, std::shared_ptr<World> world)
                : Frame(parent, name)
                , m_World(world)
        { }

        EntityID CameraEntity() const { return m_CameraEntity; }
        void SetCameraEntity(EntityID cameraEntity)
        {
            m_CameraEntity = cameraEntity;
            auto transformComponent = m_World->GetComponent<Components::Transform>(cameraEntity);
            if (!transformComponent)
                return;
            auto cameraComponent = m_World->GetComponent<Components::Camera>(cameraEntity);
            if (!cameraComponent)
                return;

            m_Camera = std::make_shared<Camera>(cameraComponent->FOV, cameraComponent->NearClip, cameraComponent->FarClip);
        }


        void Update(double dt) override
        {
            if (!m_TransformSystem)
                m_TransformSystem = m_World->GetSystem<Systems::TransformSystem>();

            auto transformComponent = m_World->GetComponent<Components::Transform>(m_CameraEntity);
            if (!transformComponent)
                return;

            auto cameraComponent = m_World->GetComponent<Components::Camera>(m_CameraEntity);
            if (!cameraComponent)
                return;

            Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(m_CameraEntity);

            m_Camera->SetFOV(cameraComponent->FOV);
            m_Camera->SetNearClip(cameraComponent->NearClip);
            m_Camera->SetFarClip(cameraComponent->FarClip);
            m_Camera->SetPosition(absoluteTransform.Position);
            m_Camera->SetOrientation(absoluteTransform.Orientation);
        }

        void Draw(std::shared_ptr<Renderer> renderer) override
        {
            if (!m_Camera)
                return;

            renderer->SetCamera(m_Camera);
            renderer->SetViewport(AbsoluteRectangle());
            renderer->DrawWorld(m_Parent->RenderQueue);
        }

    private:
        std::shared_ptr<World> m_World;
        std::shared_ptr<Systems::TransformSystem> m_TransformSystem;
        std::shared_ptr<Camera> m_Camera;
        EntityID m_CameraEntity;
    };

}

#endif //DAYDREAM_VIEWPORT_H
