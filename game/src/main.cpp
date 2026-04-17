#include <core/Engine.hpp>
#include <ecs/Entity.hpp>
#include <ecs/SparseSet.hpp>
#include <ecs/Registry.hpp>
#include <ecs/View.hpp>
#include "GameScene.hpp"
#include <cassert>
#include <cstdio>

// --------------------------------------------------------------------------
// ECS unit tests (Steps 10-12)
// --------------------------------------------------------------------------
struct Position { float x, y; };
struct Velocity { float vx, vy; };
struct Tag      { int value; };

static void runECSTests()
{
    using namespace engine;
    std::printf("=== ECS Unit Tests ===\n");

    // Entity
    {
        Entity e = Entity::make(42, 7);
        assert(e.index() == 42 && e.generation() == 7);
        assert(NULL_ENTITY != e);
    }

    // SparseSet
    {
        SparseSet<int> set;
        Entity a = Entity::make(0,1), b = Entity::make(1,1), c = Entity::make(5,1);
        set.emplace(a,10); set.emplace(b,20); set.emplace(c,30);
        assert(set.size()==3 && set.get(a)==10);
        set.remove(b);
        assert(set.size()==2 && !set.contains(b) && set.get(c)==30);
        set.clear(); assert(set.empty());
    }

    // Registry
    {
        Registry reg;
        Entity a = reg.create(), b = reg.create();
        assert(reg.alive(a) && reg.alive(b) && reg.size()==2);
        reg.destroy(a);
        assert(!reg.alive(a) && reg.size()==1);
        Entity a2 = reg.create();
        assert(a2.index()==a.index() && a2.generation()!=a.generation());

        reg.emplace<Position>(b, 1.0f, 2.0f);
        reg.emplace<Velocity>(b, 3.0f, 4.0f);
        assert(reg.has<Position>(b) && reg.get<Position>(b).x==1.0f);
        reg.remove<Position>(b);
        assert(!reg.has<Position>(b));

        reg.destroy(b);
        Entity b2 = reg.create();
        assert(!reg.has<Velocity>(b2));
    }

    // View
    {
        Registry reg;
        Entity a = reg.create(); reg.emplace<Position>(a,1,2); reg.emplace<Velocity>(a,3,4);
        Entity b = reg.create(); reg.emplace<Position>(b,5,6);

        int count = 0;
        for (auto [e, pos, vel] : reg.view<Position, Velocity>()) {
            assert(e==a && pos.x==1.0f && vel.vx==3.0f);
            ++count;
        }
        assert(count==1);

        for (auto [e, pos] : reg.view<Position>())
            pos.x += 10.0f;
        assert(reg.get<Position>(a).x == 11.0f);
    }

    std::printf("All ECS tests passed.\n\n");
}

int main()
{
    runECSTests();

    engine::Engine engine;

    engine.sceneManager().registerScene("game", []() {
        return std::make_unique<GameScene>();
    });
    engine.sceneManager().applyPendingTransitions(engine.assetManager()); // no-op
    engine.sceneManager().push("game");
    engine.sceneManager().applyPendingTransitions(engine.assetManager());

    engine.run();
    return 0;
}
