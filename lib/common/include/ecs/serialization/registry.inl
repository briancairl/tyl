/**
 * @copyright 2021 Brian Cairl
 */
#pragma once

// Boost
#include <boost/serialization/nvp.hpp>

// Tyl
#include <tyl/ecs/serialization/entity.inl>

namespace tyl::ecs
{
namespace detail
{

/**
 * @brief boost::serialization output archive adaptor for EnTT registry
 *
 *        See https://skypjack.github.io/entt/md_docs_md_entity.html (Archives)
 */
template <typename OutputArchiveT> struct SnapshotOutputArchive
{
public:
  SnapshotOutputArchive(OutputArchiveT& oa, const registry& registry) :
      oa_{std::addressof(oa)},
      registry_{std::addressof(registry)}
  {}

  inline void operator()(const entt::entity id) { (*oa_) << boost::serialization::make_nvp("id", id); }

  inline void operator()(const std::underlying_type_t<entt::entity> size)
  {
    (*oa_) << boost::serialization::make_nvp("size", size);
  }

  template <typename T> inline void operator()(const entt::entity id, const T& value)
  {
    (*oa_) << boost::serialization::make_nvp("id", id);
    BeforeComponentSave<T>{}(*oa_, *registry_, value, id);
    (*oa_) << boost::serialization::make_nvp("value", value);
    AfterComponentSave<T>{}(*oa_, *registry_, value, id);
  }

private:
  OutputArchiveT* oa_;
  const entt::registry* registry_;
};

/**
 * @brief boost::serialization input archive adaptor for EnTT registry
 *
 *        See https://skypjack.github.io/entt/md_docs_md_entity.html (Archives)
 */
template <typename InputArchiveT> struct SnapshotInputArchive
{
public:
  SnapshotInputArchive(InputArchiveT& ia, entt::registry& registry) :
      ia_{std::addressof(ia)},
      registry_{std::addressof(registry)}
  {}

  inline void operator()(entt::entity& id) { (*ia_) >> boost::serialization::make_nvp("id", id); }

  inline void operator()(std::underlying_type_t<entt::entity>& size)
  {
    (*ia_) >> boost::serialization::make_nvp("size", size);
  }

  template <typename T> inline void operator()(entt::entity& id, T& value)
  {
    (*ia_) >> boost::serialization::make_nvp("id", id);
    BeforeComponentLoad<T>{}(*ia_, *registry_, value, id);
    (*ia_) >> boost::serialization::make_nvp("value", value);
    AfterComponentLoad<T>{}(*ia_, *registry_, value, id);
  }

private:
  InputArchiveT* ia_;
  entt::registry* registry_;
};

}  // namespace detail

template <typename... ComponentTs, typename ArchiveT>
void save(ArchiveT& ar, const registry& reg)
{
  detail::SnapshotOutputArchive<ArchiveT> snap_oa{ar, reg};
  entt::snapshot{reg}.entities(snap_oa).template component<ComponentTs...>(snap_oa);
}

template <typename... ComponentTs, typename ArchiveT>
void load(ArchiveT& ar, registry& reg)
{
  reg.clear();
  detail::SnapshotInputArchive<ArchiveT> snap_ia{ar, reg};
  entt::snapshot_loader{reg}.entities(snap_ia).template component<ComponentTs...>(snap_ia);
}

}  // tyl::ecs
