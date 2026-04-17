#pragma once

namespace engine {
class Registry;
class RenderQueue;
class AssetManager;

class RenderSystem {
public:
    void extract(Registry& reg, AssetManager& assets, RenderQueue& queue, float alpha);
};

} // namespace engine
