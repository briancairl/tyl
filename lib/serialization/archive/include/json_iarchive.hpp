/**
 * @copyright 2022-present Brian Cairl
 *
 * @file json_iarchive.hpp
 */
#pragma once

// C++ Standard Library
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// Tyl
#include <tyl/serialization/iarchive.hpp>
#include <tyl/serialization/istream.hpp>
#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

namespace tyl::serialization
{

template <typename ValueT> struct load_json_primitive
{
  static_assert(std::is_object_v<ValueT>);

  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, ValueT& object)
  {
    ar.template eat<'{'>();
    load<JSONArchiveT, ValueT>{}(ar, object);
    ar.template eat<'}'>();
  }
};

template <typename IStreamT> class json_iarchive : public iarchive<json_iarchive<IStreamT>>
{
  using iarchive_base = iarchive<json_iarchive<IStreamT>>;

  friend iarchive_base;

public:
  explicit json_iarchive(istream<IStreamT>& is) : is_{static_cast<IStreamT*>(std::addressof(is))}
  {
    json_iarchive::eat<'{'>();
  }

  ~json_iarchive() { json_iarchive::eat<'}'>(); }

  template <typename ValueT> constexpr json_iarchive& operator>>(ValueT& v)
  {
    load_json_primitive<ValueT>{}(*this, v);
    return *this;
  }

  template <typename ValueT> constexpr json_iarchive& operator>>(named<ValueT>& nv)
  {
    this->operator>>(label{nv.name});
    this->operator>>(nv.value);
    return *this;
  }

  template <typename IteratorT> constexpr json_iarchive& operator>>(sequence<IteratorT> sequence)
  {
    return iarchive_base::operator>>(sequence);
  }

  constexpr json_iarchive& operator>>(label l) { return iarchive_base::operator>>(l); }

  using iarchive_base::operator>>;
  using iarchive_base::operator&;

private:
  template <char TargetChar> void eat()
  {
    while (true)
    {
      if (is_->available() == 0)
      {
        throw std::runtime_error{"JSON is ill-formed"};
      }

      const bool keep_eating = is_->peek() != TargetChar;

      char eatchar;
      is_->read(reinterpret_cast<void*>(&eatchar), 1);

      if (!keep_eating)
      {
        break;
      }
    }
  }

  constexpr void read_impl(label& l)
  {
    json_iarchive::eat<'"'>();
    json_iarchive::eat<'"'>();
    json_iarchive::eat<':'>();
  }

  template <typename IteratorT> constexpr void read_impl(sequence<IteratorT>& sequence)
  {

    json_iarchive::eat<'['>();
    const auto [first, last] = sequence;

    bool is_first = true;
    for (auto itr = first; itr != last; ++itr)
    {
      if (is_first)
      {
        is_first = false;
      }
      else
      {
        json_iarchive::eat<','>();
      }
      (*this) >> (*itr);
    }
    json_iarchive::eat<']'>();
  }

  template <typename ValueT> friend struct load_json_primitive;
  friend struct load_json_numeric;

  IStreamT* is_;
};

template <typename IStreamT> json_iarchive(istream<IStreamT>& os) -> json_iarchive<IStreamT>;

template <typename IStreamT, typename ValueT>
struct load_impl<json_iarchive<IStreamT>, ValueT> : std::conditional_t<
                                                      /* if( cond  ) */ is_named_v<ValueT>,
                                                      /* ->( true  ) */ load<json_iarchive<IStreamT>, ValueT>,
                                                      /* ->( false ) */ load_json_primitive<ValueT>>
{};

/**
 * @brief JSON input archive <code>float</code> save implementation
 */
struct load_json_numeric
{
  template <typename JSONArchiveT, typename NumericT> void operator()(JSONArchiveT& ar, NumericT& v)
  {
    std::stringstream ss;
    while (true)
    {
      if (ar.is_->available() == 0)
      {
        throw std::runtime_error{"JSON is ill-formed. Error while reading numeric type."};
      }
      else if (const char pc = ar.is_->peek(); std::isspace(pc))
      {
        char digit;
        ar.is_->read(reinterpret_cast<void*>(&digit), 1);
        continue;
      }
      else if (std::isdigit(pc) or pc == '.')
      {
        char digit;
        ar.is_->read(reinterpret_cast<void*>(&digit), 1);
        ss << digit;
      }
      else
      {
        break;
      }
    }
    ss >> v;
  }
};

/**
 * @brief JSON output archive <code>bool</code> save implementation
 */
template <> struct load_json_primitive<bool>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, bool& v)
  {
    if (ar.is_->peek() == 'T')
    {
      static char buf[4];
      ar.is_->read(buf, sizeof(buf));
      v = true;
    }
    else if (ar.is_->peek() == 'F')
    {
      static char buf[5];
      ar.is_->read(buf, sizeof(buf));
      v = false;
    }
    else
    {
      throw std::runtime_error{"JSON is ill-formed. Error while reading bool type."};
    }
  }
};

/**
 * @brief JSON input archive <code>float</code> save implementation
 */
template <> struct load_json_primitive<float> : load_json_numeric
{};

/**
 * @brief JSON input archive <code>double</code> save implementation
 */
template <> struct load_json_primitive<double> : load_json_numeric
{};

/**
 * @brief JSON input archive <code>int</code> save implementation
 */
template <> struct load_json_primitive<int> : load_json_numeric
{};

/**
 * @brief JSON input archive <code>long int</code> save implementation
 */
template <> struct load_json_primitive<long int> : load_json_numeric
{};

/**
 * @brief JSON input archive <code>unsigned int</code> save implementation
 */
template <> struct load_json_primitive<unsigned int> : load_json_numeric
{};

/**
 * @brief JSON input archive <code>long unsigned int</code> save implementation
 */
template <> struct load_json_primitive<long unsigned int> : load_json_numeric
{};

/**
 * @brief JSON input archive <code>unsigned char</code> save implementation
 */
template <> struct load_json_primitive<unsigned char>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, unsigned char& v)
  {
    ar.is_->read(reinterpret_cast<void*>(&v), 1);
  }
};

/**
 * @brief JSON input archive <code>unsigned char</code> save implementation
 */
template <> struct load_json_primitive<char>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, char& v)
  {
    ar.is_->read(reinterpret_cast<void*>(&v), 1);
  }
};

/**
 * @brief JSON input archive <code>std::string</code> save implementation
 */
template <> struct load_json_primitive<std::string>
{
  template <typename JSONArchiveT> void operator()(JSONArchiveT& ar, std::string& v)
  {
    ar.template eat<'"'>();
    while (ar.is_->peek() != '"')
    {
      if (ar.is_->available() == 0)
      {
        throw std::runtime_error{"JSON is ill-formed. Error while reading string type."};
      }
      char character;
      ar.is_->read(reinterpret_cast<void*>(&character), 1);
      v.push_back(character);
    }
    ar.template eat<'"'>();
  }
};

// Force element/field-wise serialization
template <typename IStreamT, typename ValueT>
struct is_trivially_serializable<json_iarchive<IStreamT>, ValueT> : std::false_type
{};

}  // namespace tyl::serialization
