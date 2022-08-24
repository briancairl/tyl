/**
 * @copyright 2022-present Brian Cairl
 *
 * @file json_oarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/serialization/oarchive.hpp>
#include <tyl/serialization/object.hpp>
#include <tyl/serialization/ostream.hpp>
#include <tyl/serialization/types/common/named.hpp>

namespace tyl::serialization
{

template <typename OStreamT> class json_oarchive : public oarchive<json_oarchive<OStreamT>>
{
  using oarchive_base = oarchive<json_oarchive<OStreamT>>;

  friend oarchive_base;

public:
  explicit json_oarchive(ostream<OStreamT>& os) :
      os_{static_cast<OStreamT*>(std::addressof(os))}, skip_next_comma_{true}
  {
    os_->write("{", 1);
  }

  ~json_oarchive() { os_->write("}\n", 2); }

  template <typename ValueT> constexpr json_oarchive& operator<<(const ValueT& payload)
  {
    static_assert(!is_packet_v<ValueT>, "json_oarchive cannot serialize binary blobs");
    return oarchive_base::operator<<(payload);
  }

  template <typename IteratorT> constexpr json_oarchive& operator<<(const sequence<IteratorT>& sequence)
  {
    return oarchive_base::operator<<(sequence);
  }

  constexpr json_oarchive& operator<<(const label& l)
  {
    json_oarchive::update();
    return oarchive_base::operator<<(l);
  }

  using oarchive_base::operator&;

private:
  using oarchive_base::operator<<;

  void update()
  {
    if (skip_next_comma_)
    {
      skip_next_comma_ = false;
    }
    else
    {
      os_->write(",", 1);
    }
  }

  constexpr void write_impl(const label& l)
  {
    os_->write("\"", 1);
    os_->write(l.value.data(), l.value.size());
    os_->write("\":", 2);
  }

  template <typename IteratorT> constexpr void write_impl(const sequence<IteratorT>& sequence)
  {
    skip_next_comma_ = true;
    os_->write("[", 1);
    const auto [first, last] = sequence;
    for (auto itr = first; itr != last; ++itr)
    {
      json_oarchive::update();
      (*this) << (*itr);
    }
    os_->write("]", 1);
  }

  template <typename ValueT> friend struct save_json_primitive;

  OStreamT* os_;
  bool skip_next_comma_;
};

template <typename OStreamT> json_oarchive(ostream<OStreamT>& os) -> json_oarchive<OStreamT>;

template <typename ValueT> struct save_json_primitive
{
  static_assert(std::is_object_v<ValueT>);

  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, const ValueT& object)
  {
    ar.skip_next_comma_ = true;
    ar.os_->write("{", 1);
    save<JSONArchiveT, ValueT>{}(ar, object);
    ar.os_->write("}", 1);
    ar.skip_next_comma_ = false;
  }
};

template <typename OStreamT, typename ValueT>
struct save_impl<json_oarchive<OStreamT>, ValueT> : std::conditional_t<
                                                      /* if( cond  ) */ is_named_v<ValueT>,
                                                      /* ->( true  ) */ save<json_oarchive<OStreamT>, ValueT>,
                                                      /* ->( false ) */ save_json_primitive<ValueT>>
{};

/**
 * @brief JSON output archive <code>float</code> save implementation
 */
template <> struct save_json_primitive<float>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, float v)
  {
    auto str = std::to_string(v);
    ar.os_->write(str.data(), str.size());
  }
};

/**
 * @brief JSON output archive <code>int</code> save implementation
 */
template <> struct save_json_primitive<int>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, int v)
  {
    auto str = std::to_string(v);
    ar.os_->write(str.data(), str.size());
  }
};

/**
 * @brief JSON output archive <code> longint</code> save implementation
 */
template <> struct save_json_primitive<long int>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, long int v)
  {
    auto str = std::to_string(v);
    ar.os_->write(str.data(), str.size());
  }
};

/**
 * @brief JSON output archive <code>int</code> save implementation
 */
template <> struct save_json_primitive<unsigned int>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, unsigned int v)
  {
    auto str = std::to_string(v);
    ar.os_->write(str.data(), str.size());
  }
};

/**
 * @brief JSON output archive <code>int</code> save implementation
 */
template <> struct save_json_primitive<long unsigned int>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, long unsigned int v)
  {
    auto str = std::to_string(v);
    ar.os_->write(str.data(), str.size());
  }
};

/**
 * @brief JSON output archive <code>double</code> save implementation
 */
template <> struct save_json_primitive<double>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, double v)
  {
    auto str = std::to_string(v);
    ar.os_->write(str.data(), str.size());
  }
};


/**
 * @brief JSON output archive <code>std::string_view</code> save implementation
 */
template <> struct save_json_primitive<std::string_view>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, const std::string_view& str)
  {
    ar.os_->write("\"", 1);
    ar.os_->write(str.data(), str.size());
    ar.os_->write("\"", 1);
  }
};

/**
 * @brief JSON output archive <code>std::string</code> save implementation
 */
template <> struct save_json_primitive<std::string> : save_json_primitive<std::string_view>
{};

/**
 * @brief JSON output archive <code>const char*</code> save implementation
 */
template <> struct save_json_primitive<const char*> : save_json_primitive<std::string_view>
{};

// Force element/field-wise serialization
template <typename OStreamT, typename ValueT>
struct is_trivially_serializable<json_oarchive<OStreamT>, ValueT> : std::false_type
{};

}  // namespace tyl::serialization
