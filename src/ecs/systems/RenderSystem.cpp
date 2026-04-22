#include "RenderSystem.hpp"
#include "ecs/Registry.hpp"
#include "ecs/View.hpp"
#include "ecs/Components.hpp"
#include "assets/AssetManager.hpp"
#include "renderer/RenderQueue.hpp"
#include "renderer/Texture.hpp"

namespace engine
{

void RenderSystem::extract(Registry &reg, AssetManager &assets, RenderQueue &queue, float alpha)
{
    (void)alpha; // interpolation deferred until previous-state components exist

    for (auto [entity, transform, sprite] : reg.view<Transform, Sprite>()) {
        if (!sprite.texture.valid())
            continue;

        DrawCommand cmd;
        cmd.textureID = assets.resolve(sprite.texture).id();
        cmd.sourceRect = sprite.sourceRect;
        cmd.position = transform.position;
        cmd.scale = transform.scale;
        cmd.rotation = transform.rotation;
        cmd.colorTint = sprite.color;
        cmd.layer = sprite.layer;
        queue.submit(cmd);
    }
}

} // namespace engine
