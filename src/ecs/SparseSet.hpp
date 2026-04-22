#pragma once
#include "Entity.hpp"
#include <vector>
#include <cassert>
#include <cstdint>

namespace engine
{

// SparseSet<T> — O(1) add/remove/get, cache-friendly linear iteration.
//
// sparse[entity.index()] = index into dense arrays (INVALID if absent)
// dense[i]  = T component data    (contiguous)
// entities[i] = Entity owning dense[i]  (parallel to dense)
//
// Removal uses swap-and-pop to keep the dense arrays packed.
template<typename T> class SparseSet
{
public:
    static constexpr uint32_t INVALID = ~0u;

    // Add a component for entity. Entity must not already have one.
    template<typename... Args> T &emplace(Entity entity, Args &&...args)
    {
        uint32_t idx = entity.index();
        grow(idx);
        assert(m_sparse[idx] == INVALID && "Entity already has this component");

        m_sparse[idx] = static_cast<uint32_t>(m_dense.size());
        m_entities.push_back(entity);
        return m_dense.emplace_back(std::forward<Args>(args)...);
    }

    // Remove the component. Entity must have one.
    void remove(Entity entity)
    {
        uint32_t idx = entity.index();
        assert(idx < m_sparse.size() && m_sparse[idx] != INVALID &&
               "Entity does not have this component");

        uint32_t denseIdx = m_sparse[idx];
        uint32_t lastIdx = static_cast<uint32_t>(m_dense.size()) - 1u;

        if (denseIdx != lastIdx) {
            // Swap with last to keep dense arrays packed
            m_dense[denseIdx] = std::move(m_dense[lastIdx]);
            m_entities[denseIdx] = m_entities[lastIdx];
            m_sparse[m_entities[denseIdx].index()] = denseIdx;
        }

        m_dense.pop_back();
        m_entities.pop_back();
        m_sparse[idx] = INVALID;
    }

    // Get a reference to the component. Entity must have one.
    T &get(Entity entity)
    {
        uint32_t idx = entity.index();
        assert(idx < m_sparse.size() && m_sparse[idx] != INVALID &&
               "Entity does not have this component");
        return m_dense[m_sparse[idx]];
    }

    const T &get(Entity entity) const
    {
        uint32_t idx = entity.index();
        assert(idx < m_sparse.size() && m_sparse[idx] != INVALID &&
               "Entity does not have this component");
        return m_dense[m_sparse[idx]];
    }

    bool contains(Entity entity) const
    {
        uint32_t idx = entity.index();
        return idx < m_sparse.size() && m_sparse[idx] != INVALID;
    }

    // Direct access to dense arrays for cache-friendly iteration
    std::vector<T> &data() { return m_dense; }
    std::vector<Entity> &entities() { return m_entities; }

    const std::vector<T> &data() const { return m_dense; }
    const std::vector<Entity> &entities() const { return m_entities; }

    size_t size() const { return m_dense.size(); }
    bool empty() const { return m_dense.empty(); }

    void clear()
    {
        m_dense.clear();
        m_entities.clear();
        std::fill(m_sparse.begin(), m_sparse.end(), INVALID);
    }

private:
    void grow(uint32_t idx)
    {
        if (idx >= m_sparse.size())
            m_sparse.resize(idx + 1, INVALID);
    }

    std::vector<uint32_t> m_sparse; // entity index → dense index
    std::vector<T> m_dense;         // contiguous component data
    std::vector<Entity> m_entities; // parallel to m_dense
};

} // namespace engine
