#pragma once
#include "Handle.hpp"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <string>
#include <cassert>

namespace engine
{

// --------------------------------------------------------------------------
// Type-erased base — lets AssetManager iterate pools without knowing T.
// --------------------------------------------------------------------------
struct IAssetPool {
    virtual ~IAssetPool() = default;
};

template<typename T> struct AssetPool : IAssetPool {
    std::unordered_map<std::string, uint32_t> keyToId;
    std::unordered_map<uint32_t, std::unique_ptr<T>> assets;
    uint32_t nextId = 1;
};

// --------------------------------------------------------------------------
// AssetManager — single owner of all loaded assets.
//
// Load eagerly (e.g. in Scene::onEnter), never on first access.
// Hold Handle<T>s in ECS components; resolve to references when needed.
// --------------------------------------------------------------------------
class AssetManager
{
public:
    // Construct asset in-place and return a handle.
    // Idempotent: if key is already loaded, returns the existing handle.
    template<typename T, typename... Args> Handle<T> load(const std::string &key, Args &&...args)
    {
        auto &pool = getOrCreate<T>();
        auto it = pool.keyToId.find(key);
        if (it != pool.keyToId.end())
            return Handle<T>{it->second};

        uint32_t id = pool.nextId++;
        pool.keyToId[key] = id;
        pool.assets.emplace(id, std::make_unique<T>(std::forward<Args>(args)...));
        return Handle<T>{id};
    }

    // Look up handle by key. Asset must already be loaded.
    template<typename T> Handle<T> get(const std::string &key) const
    {
        const auto &pool = getPool<T>();
        auto it = pool.keyToId.find(key);
        assert(it != pool.keyToId.end() && "Asset not loaded");
        return Handle<T>{it->second};
    }

    // Resolve handle → asset reference. Handle must be valid.
    template<typename T> T &resolve(Handle<T> handle)
    {
        assert(handle.valid() && "Resolving invalid handle");
        auto &pool = getOrCreate<T>();
        auto it = pool.assets.find(handle.id);
        assert(it != pool.assets.end() && "Stale handle");
        return *it->second;
    }

    template<typename T> const T &resolve(Handle<T> handle) const
    {
        assert(handle.valid() && "Resolving invalid handle");
        const auto &pool = getPool<T>();
        auto it = pool.assets.find(handle.id);
        assert(it != pool.assets.end() && "Stale handle");
        return *it->second;
    }

    template<typename T> bool isLoaded(const std::string &key) const
    {
        auto it = m_pools.find(std::type_index(typeid(T)));
        if (it == m_pools.end())
            return false;
        return static_cast<const AssetPool<T> &>(*it->second).keyToId.count(key) > 0;
    }

    // Destroy asset and invalidate all handles to it.
    template<typename T> void unload(const std::string &key)
    {
        auto &pool = getOrCreate<T>();
        auto it = pool.keyToId.find(key);
        if (it == pool.keyToId.end())
            return;
        pool.assets.erase(it->second);
        pool.keyToId.erase(it);
    }

    // Destroy all assets of type T.
    template<typename T> void unloadAll() { m_pools.erase(std::type_index(typeid(T))); }

private:
    template<typename T> AssetPool<T> &getOrCreate()
    {
        auto key = std::type_index(typeid(T));
        auto it = m_pools.find(key);
        if (it == m_pools.end()) {
            auto [ins, ok] = m_pools.emplace(key, std::make_unique<AssetPool<T>>());
            return static_cast<AssetPool<T> &>(*ins->second);
        }
        return static_cast<AssetPool<T> &>(*it->second);
    }

    template<typename T> const AssetPool<T> &getPool() const
    {
        auto it = m_pools.find(std::type_index(typeid(T)));
        assert(it != m_pools.end() && "Asset type not found");
        return static_cast<const AssetPool<T> &>(*it->second);
    }

    std::unordered_map<std::type_index, std::unique_ptr<IAssetPool>> m_pools;
};

} // namespace engine
