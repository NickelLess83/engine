#pragma once
#include "ecs/Registry.hpp"

namespace engine
{

class AssetManager;
class InputManager;
class RenderQueue;
class DebugDraw;

class Scene
{
public:
    virtual void onEnter(AssetManager &assets) = 0;
    virtual void onExit(AssetManager &assets) = 0;
    virtual void update(float dt, InputManager &input) = 0;
    virtual void render(RenderQueue &queue, AssetManager &assets, DebugDraw &debug,
                        float alpha) = 0;

    // isTransparent: should scenes below this one also render?  (e.g. pause overlay)
    // isActive:      should scenes below this one also update?  (default: no)
    virtual bool isTransparent() const { return false; }
    virtual bool isActive() const { return false; }

    virtual ~Scene() = default;

protected:
    Registry registry;
};

} // namespace engine
