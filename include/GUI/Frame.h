//
// Created by Administrator on 2015-09-16.
//

#ifndef DAYDREAM_FRAME_H
#define DAYDREAM_FRAME_H

namespace dd
{

#include <memory>
#include <map>

#include "Util/Rectangle.h"
#include "Core/EventBroker.h"
#include "Core/EKeyDown.h"
#include "Core/EKeyUp.h"
#include "Input/EInputCommand.h"
#include "Core/ResourceManager.h"
#include "Core/Renderer.h"
#include "Core/RenderQueue.h"
#include "Core/Texture.h"

class Frame : public Rectangle
{
public:
    enum class Anchor
    {
        Left,
        Right,
        Top,
        Bottom
    };

    static const int BaseWidth = 1280;
    static const int BaseHeight = 720;

    // Set up a base frame with an event broker
    Frame(std::shared_ptr<dd::EventBroker> eventBroker, std::shared_ptr<dd::ResourceManager> resourceManager)
            : EventBroker(eventBroker)
            , ResourceManager(resourceManager)
            , Rectangle()
            , m_Parent(nullptr)
            , m_Name("UIParent")
            , m_Layer(0)
            , m_Hidden(false)
    { Initialize(); }

    // Create a frame as a child
    Frame(Frame* parent, std::string name)
            : m_Name(name)
            , m_Layer(0)
            , m_Hidden(false)
    { SetParent(parent); Initialize(); }

    ~Frame()
    {
        /*for (auto layer : m_Children)
        {
            for (auto child : layer.second)
            {
                delete child.second;
            }
        }
        if (m_Parent)
        {
            m_Parent->RemoveChild(this);
        }*/
    }

    dd::RenderQueuePair RenderQueue;

    Frame* Parent() const { return m_Parent; }
    void SetParent(Frame* parent)
    {
        if (parent == nullptr)
        {
            LOG_ERROR("Failed to create frame \"%s\": Invalid parent", m_Name.c_str());
            return;
        }

        Width = parent->Width;
        Height = parent->Height;
        m_Layer = parent->Layer() + 1;
        parent->AddChild(this);
        m_Parent = parent;
        EventBroker = parent->EventBroker;
        ResourceManager = parent->ResourceManager;
    }

    void AddChild(Frame* child)
    {
        m_Children[child->m_Layer].insert(std::make_pair(child->Name(), child));
        if (m_Parent)
        {
            m_Parent->AddChild(child);
        }
    }

    void RemoveChild(Frame* child)
    {
        auto it = m_Children.find(child->m_Layer);
        if (it != m_Children.end())
        {
            m_Children.erase(it);
        }

        if (m_Parent)
        {
            m_Parent->RemoveChild(child);
        }
    }

    std::string Name() const { return m_Name; }
    void SetName(std::string val) { m_Name = val; }
    int Layer() const { return m_Layer; }
    bool Hidden() const
    {
        if (m_Parent)
            return m_Parent->Hidden() || m_Hidden;
        else
            return m_Hidden;
    }

    bool Visible() const
    {
        return !Hidden();
    }

    virtual void Hide() { m_Hidden = true; }
    virtual void Show() { m_Hidden = false; }

    int Left() const override
    {
        if (m_Parent)
            return m_Parent->Left() + X;
        else
            return X;
    }
    int Right() const override
    {
        if (m_Parent)
            return std::min(m_Parent->Right(), Left() + Width);
        else
            return Left() + Width;
    }
    int Top() const override
    {
        if (m_Parent)
            return m_Parent->Top() + Y;
        else
            return Y;
    }
    int Bottom() const override
    {
        if (m_Parent)
            return std::min(m_Parent->Bottom(), Top() + Height);
        else
            return Top() + Height;
    }

    glm::vec2 Scale()
    {
        if (m_Parent)
            return m_Parent->Scale();
        else
            return glm::vec2(Width, Height) / glm::vec2(BaseWidth, BaseHeight);
    }

    Rectangle AbsoluteRectangle()
    {
        int left = Left();
        if (m_Parent)
            left = std::max(left, m_Parent->Left());
        int top = Top();
        if (m_Parent)
            top = std::max(top, m_Parent->Top());
        int width = Right() - left;
        int height = Bottom() - top;
        return Rectangle(left, top, width, height);
    }

    void UpdateLayered(double dt)
    {
        // Update ourselves
        this->Update(dt);

        // Update children
        for (auto &pairLayer : m_Children)
        {
            auto children = pairLayer.second;
            for (auto &pairChild : children)
            {
                auto child = pairChild.second;
                child->Update(dt);
            }
        }
    }
    virtual void Update(double dt) { }

    void DrawLayered(std::shared_ptr<Renderer> renderer)
    {
        if (this->Hidden())
            return;

        // Draw ourselves
        renderer->SetScissor(AbsoluteRectangle());
        this->Draw(renderer);

        // Draw children
        for (auto &pairLayer : m_Children)
        {
            auto children = pairLayer.second;
            for (auto &pairChild : children)
            {
                auto child = pairChild.second;
                if (child->Hidden())
                    continue;
                Rectangle rect = child->AbsoluteRectangle();
                renderer->SetScissor(rect);
                child->Draw(renderer);
            }
        }
    }

    virtual void Draw(std::shared_ptr<Renderer> renderer) { }

protected:
    std::shared_ptr<EventBroker> EventBroker;
    std::shared_ptr<ResourceManager> ResourceManager;

    std::string m_Name;
    int m_Layer;
    bool m_Hidden;

    Frame* m_Parent;
    typedef std::multimap<std::string, Frame*> Children_t; // name -> frame
    std::map<int, Children_t> m_Children; // layer -> Children_t

    virtual bool OnKeyDown(const dd::Events::KeyDown &event) { return false; }
    virtual bool OnKeyUp(const dd::Events::KeyUp &event) { return false; }
    //virtual bool OnMouseDown(const Events::KeyDown &event) { }
    //virtual bool OnMouseUp(const Events::KeyDown &event) { }
    virtual bool OnCommand(const dd::Events::InputCommand &event) { return false; }

private:
    EventRelay<Frame, dd::Events::KeyDown> m_EKeyDown;
    EventRelay<Frame, dd::Events::KeyUp> m_EKeyUp;
    EventRelay<Frame, dd::Events::InputCommand> m_EInputCommand;

    void Initialize()
    {
        EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Frame::OnKeyDown);
        EVENT_SUBSCRIBE_MEMBER(m_EKeyUp, &Frame::OnKeyUp);
        EVENT_SUBSCRIBE_MEMBER(m_EInputCommand, &Frame::OnCommand);
    }
};

}

#endif //DAYDREAM_FRAME_H
