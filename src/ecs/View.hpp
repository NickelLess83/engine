#pragma once
#include "Registry.hpp"
#include <tuple>
#include <vector>

namespace engine
{

// --------------------------------------------------------------------------
// View<Ts...> — lazy iteration over entities that have ALL of Ts...
//
// Iterates the first type's SparseSet (the "pivot"); skips entities that are
// absent from any of the other pools. Do not add/remove components or create/
// destroy entities while iterating.
// --------------------------------------------------------------------------
template<typename... Ts> class View
{
    static_assert(sizeof...(Ts) > 0, "View requires at least one component type");

    using FirstT = std::tuple_element_t<0, std::tuple<Ts...>>;

public:
    explicit View(Registry &reg) : m_reg(&reg) {}

    struct Iterator {
        Registry *reg;
        const std::vector<Entity> *entities;
        size_t idx;

        bool allPresent() const
        {
            Entity e = (*entities)[idx];
            return (reg->has<Ts>(e) && ...);
        }

        void advance()
        {
            while (idx < entities->size() && !allPresent())
                ++idx;
        }

        Iterator &operator++()
        {
            ++idx;
            advance();
            return *this;
        }

        bool operator!=(const Iterator &o) const { return idx != o.idx; }

        std::tuple<Entity, Ts &...> operator*() const
        {
            Entity e = (*entities)[idx];
            return {e, reg->get<Ts>(e)...};
        }
    };

    Iterator begin()
    {
        auto &ents = m_reg->pool<FirstT>().entities();
        Iterator it{m_reg, &ents, 0};
        it.advance();
        return it;
    }

    Iterator end()
    {
        auto &ents = m_reg->pool<FirstT>().entities();
        return Iterator{m_reg, &ents, ents.size()};
    }

private:
    Registry *m_reg;
};

// --------------------------------------------------------------------------
// Registry::view<Ts...>() — defined here so View is complete at the call site.
// Include View.hpp (not just Registry.hpp) to use this method.
// --------------------------------------------------------------------------
template<typename... Ts> auto Registry::view() { return View<Ts...>(*this); }

} // namespace engine
