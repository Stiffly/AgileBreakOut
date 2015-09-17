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

namespace dd
{

class Viewport : public Frame
{
public:
    Viewport(Frame *parent, std::string name, std::shared_ptr<dd::World> world)
            : Frame(parent, name), m_World(world) { }

    EntityID CameraEntity() const { return m_CameraEntity; }

    void SetCameraEntity(EntityID cameraEntity) {
        m_CameraEntity = cameraEntity;
        auto transformComponent = m_World->GetComponent<dd::Components::Transform>(cameraEntity);
        if (!transformComponent)
            return;
        auto cameraComponent = m_World->GetComponent<dd::Components::Camera>(cameraEntity);
        if (!cameraComponent)
            return;

        m_FrameCamera = std::make_shared<dd::Camera>(cameraComponent->FOV, cameraComponent->NearClip, cameraComponent->FarClip);
    }


    void Update(double dt) override {
        if (!m_TransformSystem)
            m_TransformSystem = m_World->GetSystem<dd::Systems::TransformSystem>();

        auto transformComponent = m_World->GetComponent<dd::Components::Transform>(m_CameraEntity);
        if (!transformComponent)
            return;

        auto cameraComponent = m_World->GetComponent<dd::Components::Camera>(m_CameraEntity);
        if (!cameraComponent)
            return;

        dd::Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(m_CameraEntity);

        m_FrameCamera->SetFOV(cameraComponent->FOV);
        m_FrameCamera->SetNearClip(cameraComponent->NearClip);
        m_FrameCamera->SetFarClip(cameraComponent->FarClip);
        m_FrameCamera->SetPosition(absoluteTransform.Position);
        m_FrameCamera->SetOrientation(absoluteTransform.Orientation);
    }

    void Draw(std::shared_ptr<dd::Renderer> renderer) override {
        if (!m_FrameCamera)
            return;

        renderer->SetCamera(m_FrameCamera);
        renderer->SetViewport(AbsoluteRectangle());
        renderer->DrawWorld(m_Parent->RenderQueue);
    }

private:
    std::shared_ptr<dd::World> m_World;
    std::shared_ptr<dd::Systems::TransformSystem> m_TransformSystem;
    std::shared_ptr<dd::Camera> m_FrameCamera;
    EntityID m_CameraEntity;
};

}

#endif //DAYDREAM_VIEWPORT_H
