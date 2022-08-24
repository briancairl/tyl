/**
 * @copyright 2022-present Brian Cairl
 *
 * @file save.hpp
 */
#pragma once

// C++ Standard Library
#include <iterator>

// Tyl
#include <tyl/common/typestr.hpp>
#include <tyl/ecs/ecs.hpp>
#include <tyl/serialization/oarchive.hpp>
#include <tyl/serialization/sequence.hpp>
#include <tyl/serialization/types/common/named.hpp>

namespace tyl::serialization
{

/**
 * @brief Wrapper which represents a component and the entity which it belongs to
 */
template <typename ComponentT> struct SaveComponentProxy
{
  constexpr SaveComponentProxy(ecs::entity _e, const ComponentT* const _c) : e{_e}, c{_c} {}

  /// ID of entity who has ComponentT
  ecs::entity e;

  /// Reference to component to serialize
  const ComponentT* c;
};

/**
 * @brief Serializes SaveComponentProxy
 */
template <typename ArchiveT, typename ComponentT> struct save<ArchiveT, SaveComponentProxy<ComponentT>>
{
  void operator()(ArchiveT& ar, const SaveComponentProxy<ComponentT>& s)
  {
    ar << named{"id", static_cast<ecs::entity_int_t>(s.e)};
    ar << named{"value", *(s.c)};
  }
};

/**
 * @brief Iterator wrapper used to access a sequence of component values
 */
template <typename ComponentT, typename UnderlyingIteratorT, typename ViewT> class SaveComponentProxyIterator
{
public:
  SaveComponentProxyIterator(UnderlyingIteratorT itr, ViewT view) : itr_{itr}, view_{view} {}

  SaveComponentProxyIterator& operator++()
  {
    ++itr_;
    return *this;
  }

  constexpr auto operator*()
  {
    return SaveComponentProxy<ComponentT>{(*itr_), std::addressof(view_.template get<const ComponentT>(*itr_))};
  }

  constexpr bool operator!=(const SaveComponentProxyIterator& other) const { return itr_ != other.itr_; }

private:
  UnderlyingIteratorT itr_;
  ViewT view_;
};

/**
 * @brief Iterator wrapper used to access a sequence of entity IDs
 */
template <typename UnderlyingIteratorT> class SaveComponentProxyIterator<ecs::entity, UnderlyingIteratorT, void>
{
public:
  explicit SaveComponentProxyIterator(UnderlyingIteratorT itr) : itr_{itr} {}
  SaveComponentProxyIterator& operator++()
  {
    ++itr_;
    return *this;
  }
  constexpr auto operator*() const { return *(reinterpret_cast<const ecs::entity_int_t*>(itr_)); }
  constexpr bool operator!=(const SaveComponentProxyIterator& other) const { return itr_ != other.itr_; }

private:
  UnderlyingIteratorT itr_;
};

/**
 * @brief Helper to create component sequence wrapper
 */
template <typename ComponentT, typename UnderlyingIteratorT, typename ViewT>
auto make_save_component_iterator(UnderlyingIteratorT itr, ViewT view)
{
  return SaveComponentProxyIterator<ComponentT, UnderlyingIteratorT, ViewT>{itr, view};
}

/**
 * @brief Helper to create entity sequence wrapper
 */
template <typename ComponentT, typename UnderlyingIteratorT> auto make_save_component_iterator(UnderlyingIteratorT itr)
{
  return SaveComponentProxyIterator<ComponentT, UnderlyingIteratorT, void>{itr};
}

/**
 * @brief Wrapper which represents an array of components
 */
template <typename ComponentT, typename SequenceT> struct SaveComponent
{
  std::size_t size;
  SequenceT data;

  explicit SaveComponent(SequenceT _sequence) : size{0UL}, data{_sequence}
  {
    for (auto itr = data.first; itr != data.last; ++itr, ++size)
    {}
  }
};

/**
 * @brief Wrapper which represents an array of components
 */
template <typename SequenceT> struct SaveComponent<ecs::entity, SequenceT>
{
  std::size_t size;
  SequenceT data;
  ecs::entity free_list;

  explicit SaveComponent(SequenceT _sequence, ecs::entity _free_list) :
      size{static_cast<std::size_t>(std::distance(_sequence.first, _sequence.last))},
      data{_sequence},
      free_list{_free_list}
  {}
};

/**
 * @brief Serializes SaveComponent
 */
template <typename ArchiveT, typename ComponentT, typename SequenceT>
struct save<ArchiveT, SaveComponent<ComponentT, SequenceT>>
{
  void operator()(ArchiveT& ar, const SaveComponent<ComponentT, SequenceT>& component)
  {
    ar << named{"size", component.size};
    ar << named{"data", component.data};
  }
};

/**
 * @brief Serializes SaveComponent
 */
template <typename ArchiveT, typename SequenceT> struct save<ArchiveT, SaveComponent<ecs::entity, SequenceT>>
{
  void operator()(ArchiveT& ar, const SaveComponent<ecs::entity, SequenceT>& component)
  {
    ar << named{"free", static_cast<const ecs::entity_int_t>(component.free_list)};
    ar << named{"size", component.size};
    ar << named{"data", component.data};
  }
};

/**
 * @brief Helper to create SaveComponent
 */
template <typename ComponentT> auto make_save_component(const ecs::registry& reg)
{
  auto view = reg.view<ComponentT>();
  auto first = make_save_component_iterator<ComponentT>(view.begin(), view);
  auto last = make_save_component_iterator<ComponentT>(view.end(), view);
  auto seq = make_sequence(first, last);
  return SaveComponent<ComponentT, decltype(seq)>{seq};
}

/**
 * @brief Helper to create SaveComponent
 */
template <> auto make_save_component<ecs::entity>(const ecs::registry& reg)
{
  auto first = make_save_component_iterator<ecs::entity>(reg.data());
  auto last = make_save_component_iterator<ecs::entity>(reg.data() + reg.size());
  auto seq = make_sequence(first, last);
  return SaveComponent<ecs::entity, decltype(seq)>{make_sequence(first, last), reg.released()};
}

template <typename ArchiveT, typename ComponentT> void save_dispatch(ArchiveT& ar, const ecs::registry& reg)
{
  ar << named{typestr<ComponentT>(), make_save_component<ComponentT>(reg)};
}

template <typename... ComponentTs> struct save_components_are_valid : std::true_type
{};

template <typename FirstComponent, typename... OtherComponentTs>
struct save_components_are_valid<FirstComponent, ecs::entity, OtherComponentTs...> : std::false_type
{};

template <typename... OtherComponentTs>
struct save_components_are_valid<ecs::entity, OtherComponentTs...> : save_components_are_valid<OtherComponentTs...>
{};

/**
 * @brief Serializes SaveComponent
 */
template <typename ArchiveT, typename... ComponentTs> struct save<ArchiveT, ecs::writer<ComponentTs...>>
{
  static_assert(sizeof...(ComponentTs), "Must specify at least once component");
  static_assert(
    save_components_are_valid<ComponentTs...>(),
    "ecs::entity must be the first component, or not included in ComponentTs");

  void operator()(ArchiveT& ar, const ecs::writer<ComponentTs...>& w)
  {
    auto n = ((save_dispatch<ArchiveT, ComponentTs>(ar, *w), 1) + ...);
    (void)n;
  }
};

}  // namespace tyl::serialization

namespace std
{

template <typename ComponentT, typename UnderlyingIteratorT, typename ViewT>
struct iterator_traits<::tyl::serialization::SaveComponentProxyIterator<ComponentT, UnderlyingIteratorT, ViewT>>
{
  using difference_type = std::ptrdiff_t;
  using value_type = ::tyl::serialization::SaveComponentProxy<ComponentT>;
  using pointer = void;
  using reference = ::tyl::serialization::SaveComponentProxy<ComponentT>;
  using iterator_category = std::forward_iterator_tag;
};

}  // namespace std
