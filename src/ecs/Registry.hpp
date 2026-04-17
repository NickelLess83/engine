#pragma once
#include "Entity.hpp"
#include "SparseSet.hpp"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <cassert>

namespace engine {

// --------------------------------------------------------------------------
// Type-erased pool interface — lets Registry::destroy() remove all components
// from an entity without knowing component types at call site.
// --------------------------------------------------------------------------
struct IComponentPool {
    virtual void remove(Entity e) = 0;
    virtual ~IComponentPool() = default;
};

template<typename T>
struct ComponentPool : IComponentPool {
    SparseSet<T> set;
    void remove(Entity e) override { if (set.contains(e)) set.remove(e); }
};

// --------------------------------------------------------------------------
// Registry — owns entities and all component pools.
// --------------------------------------------------------------------------
class Registry {
public:
    // Create a new entity. Recycles destroyed slots (with bumped generation).
    Entity create()
    {
        if (!m_freeList.empty()) {
            uint32_t idx = m_freeList.back();
            m_freeList.pop_back();
            return Entity::make(idx, m_generations[idx]);
        }
        uint32_t idx = m_nextIndex++;
        m_generations.push_back(0);
        return Entity::make(idx, 0);
    }

    // Destroy an entity: removes all components, bumps generation.
    void destroy(Entity entity)
    {
        assert(alive(entity) && "Destroying a stale or null entity");
        for (auto& [tid, pool] : m_pools)
            pool->remove(entity);
        uint32_t idx = entity.index();
        m_generations[idx] = (m_generations[idx] + 1) & Entity::GEN_MASK;
        m_freeList.push_back(idx);
    }

    // True only if the entity was created by this registry and not yet destroyed.
    bool alive(Entity entity) const
    {
        uint32_t idx = entity.index();
        return idx < m_generations.size()
            && m_generations[idx] == entity.generation();
    }

    // Construct a component T in-place for entity.
    template<typename T, typename... Args>
    T& emplace(Entity entity, Args&&... args)
    {
        assert(alive(entity));
        return getOrCreate<T>().set.emplace(entity, std::forward<Args>(args)...);
    }

    // Remove component T from entity.
    template<typename T>
    void remove(Entity entity)
    {
        assert(alive(entity));
        getOrCreate<T>().set.remove(entity);
    }

    // Get a reference to component T. Entity must have the component.
    template<typename T>
    T& get(Entity entity)
    {
        assert(alive(entity));
        return getOrCreate<T>().set.get(entity);
    }

    template<typename T>
    const T& get(Entity entity) const
    {
        assert(alive(entity));
        auto it = m_pools.find(std::type_index(typeid(T)));
        assert(it != m_pools.end());
        return static_cast<const ComponentPool<T>*>(it->second.get())->set.get(entity);
    }

    // True if entity currently has component T.
    template<typename T>
    bool has(Entity entity) const
    {
        auto it = m_pools.find(std::type_index(typeid(T)));
        if (it == m_pools.end()) return false;
        return static_cast<const ComponentPool<T>*>(it->second.get())->set.contains(entity);
    }

    // Direct access to the SparseSet<T> for a component type — used by View.
    template<typename T>
    SparseSet<T>& pool() { return getOrCreate<T>().set; }

    template<typename T>
    const SparseSet<T>& pool() const
    {
        auto it = m_pools.find(std::type_index(typeid(T)));
        assert(it != m_pools.end());
        return static_cast<const ComponentPool<T>*>(it->second.get())->set;
    }

    // Number of live entities.
    size_t size() const { return m_nextIndex - m_freeList.size(); }

    // Iterate entities that have ALL of Ts... — requires View.hpp to be included.
    template<typename... Ts>
    auto view();

private:
    template<typename T>
    ComponentPool<T>& getOrCreate()
    {
        auto key = std::type_index(typeid(T));
        auto it  = m_pools.find(key);
        if (it == m_pools.end()) {
            auto [ins, ok] = m_pools.emplace(key, std::make_unique<ComponentPool<T>>());
            return static_cast<ComponentPool<T>&>(*ins->second);
        }
        return static_cast<ComponentPool<T>&>(*it->second);
    }

    uint32_t              m_nextIndex  = 0;
    std::vector<uint32_t> m_generations;
    std::vector<uint32_t> m_freeList;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_pools;
};

} // namespace engine
