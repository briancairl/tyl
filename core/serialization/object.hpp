/**
 * @copyright 2022-present Brian Cairl
 *
 * @file object.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <utility>

namespace tyl::serialization
{

struct serialize_not_implemented
{};

template <typename ArchiveT, typename ObjectT> struct serialize : serialize_not_implemented
{};

template <typename ArchiveT, typename ObjectT>
struct serialize_is_implemented
    : std::integral_constant<bool, !std::is_base_of_v<serialize_not_implemented, serialize<ArchiveT, ObjectT>>>
{};

template <typename ArchiveT, typename ObjectT>
constexpr bool serialize_is_implemented_v = serialize_is_implemented<ArchiveT, ObjectT>::value;

template <typename SerializeImpl> struct load_implicit_from_serialize : private SerializeImpl
{
  template <typename IArchiveT, typename ObjectT>
  constexpr decltype(auto) operator()(IArchiveT&& ar, const ObjectT& object)
  {
    return SerializeImpl::operator()(std::forward<IArchiveT>(ar), const_cast<ObjectT&>(object));
  }
};

struct load_not_implemented
{};

template <typename IArchiveT, typename ObjectT>
struct load : std::conditional_t<
                serialize_is_implemented_v<IArchiveT, ObjectT>,
                load_implicit_from_serialize<serialize<IArchiveT, ObjectT>>,
                load_not_implemented>
{};

template <typename IArchiveT, typename ObjectT>
struct load_is_implemented
    : std::integral_constant<bool, !std::is_base_of_v<load_not_implemented, load<IArchiveT, ObjectT>>>
{};

template <typename IArchiveT, typename ObjectT>
constexpr bool load_is_implemented_v = load_is_implemented<IArchiveT, ObjectT>::value;

struct save_not_implemented
{};

template <typename SerializeImpl> struct save_implicit_from_serialize : private SerializeImpl
{
  template <typename OArchiveT, typename ObjectT>
  constexpr decltype(auto) operator()(OArchiveT&& ar, const ObjectT& object)
  {
    return SerializeImpl::operator()(std::forward<OArchiveT>(ar), const_cast<ObjectT&>(object));
  }
};

template <typename OArchiveT, typename ObjectT>
struct save : std::conditional_t<
                serialize_is_implemented_v<OArchiveT, ObjectT>,
                save_implicit_from_serialize<serialize<OArchiveT, ObjectT>>,
                save_not_implemented>
{};

template <typename OArchiveT, typename ObjectT>
struct save_is_implemented
    : std::integral_constant<bool, !std::is_base_of_v<save_not_implemented, save<OArchiveT, ObjectT>>>
{};

template <typename OArchiveT, typename ObjectT>
constexpr bool save_is_implemented_v = save_is_implemented<OArchiveT, ObjectT>::value;

template <typename ArchiveT, typename ObjectT> struct is_trivially_serializable : std::is_trivial<ObjectT>
{};

template <typename ArchiveT, typename ObjectT>
const bool is_trivially_serializable_v = is_trivially_serializable<ArchiveT, ObjectT>::value;

/**
 * @brief Proxy object for use during de-serialization while bypassing default object construction
 */
template <typename ComponentT> class bypass_default_constructor
{
public:
  template <typename... CTorArgTs> void construct(CTorArgTs&&... ctor_args)
  {
    new (self()) ComponentT{std::forward<CTorArgTs>(ctor_args)...};
  }

  constexpr operator ComponentT&&() { return std::move(*self()); }

  ~bypass_default_constructor() { self()->~ComponentT(); }

private:
  ComponentT* self() { return reinterpret_cast<ComponentT*>(buffer_); }

  alignas(ComponentT) std::byte buffer_[sizeof(ComponentT)];
};

}  // namespace tyl::serialization
