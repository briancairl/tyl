/**
 * @copyright 2022-present Brian Cairl
 *
 * @file dynamic_bitset.hpp
 */
#pragma once

// C++ Standard Library
#include <type_traits>
#include <variant>

namespace tyl
{

/**
 * @brief Drop-in replacement for C++23 unexpected
 */
template <typename ErrorT> class unexpected
{
public:
  unexpected(unexpected&& _) = default;

  unexpected(const unexpected& _) = default;

  explicit unexpected(const ErrorT& e) : error_{e} {}

  explicit unexpected(ErrorT&& e) : error_{std::move(e)} {}

  constexpr const ErrorT& error() const& noexcept { return error_; }

  constexpr ErrorT& error() & noexcept { return error_; }

  constexpr const ErrorT&& error() const&& noexcept { return error_; }

  constexpr ErrorT&& error() && noexcept { return std::move(error_); }

private:
  /// Underlying error value
  ErrorT error_;
};

/**
 * @brief Drop-in replacement for C++23 expected
 */
template <typename ValueT, typename ErrorT> class expected
{
  static_assert(!std::is_same_v<ValueT, ErrorT>, "'ValueT' and 'ErrorT' should not be the same");

public:
  using value_type = ValueT;
  using error_type = ErrorT;

  constexpr expected(const unexpected<ErrorT>& unexpected) : u_{std::in_place_type<ErrorT>, unexpected.error()} {}

  constexpr expected(unexpected<ErrorT>&& unexpected) : u_{std::in_place_type<ErrorT>, std::move(unexpected).error()} {}

  constexpr expected(const ValueT& value) : u_{std::in_place_type<ValueT>, value} {}

  constexpr expected(ValueT&& value) : u_{std::in_place_type<std::remove_reference_t<ValueT>>, std::move(value)} {}

  template <typename T, typename... Args> constexpr T& emplace(Args&&... args)
  {
    return u_.template emplace<T>(std::forward<Args>(args)...);
  }

  constexpr bool has_value() const noexcept { return std::holds_alternative<ValueT>(u_); }
  constexpr operator bool() const noexcept { return has_value(); }

  constexpr const ValueT& value() const& noexcept { return std::get<ValueT>(u_); }
  constexpr ValueT& value() & noexcept { return std::get<ValueT>(u_); }
  constexpr const ValueT&& value() const&& noexcept { return std::get<ValueT>(u_); }
  constexpr ValueT&& value() && noexcept { return std::move(std::get<ValueT>(u_)); }

  constexpr const ErrorT& error() const& noexcept { return std::get<ErrorT>(u_); }
  constexpr ErrorT& error() & noexcept { return std::get<ErrorT>(u_); }
  constexpr const ErrorT&& error() const&& noexcept { return std::get<ErrorT>(u_); }
  constexpr ErrorT&& error() && noexcept { return std::move(std::get<ErrorT>(u_)); }

  constexpr const ValueT& operator*() const& noexcept { return value(); }
  constexpr ValueT& operator*() & noexcept { return value(); }
  constexpr const ValueT&& operator*() const&& noexcept { return value(); }
  constexpr ValueT&& operator*() && noexcept { return value(); }

  constexpr const ValueT* operator->() const noexcept { return &value(); }
  constexpr ValueT* operator->() noexcept { return &value(); }

  constexpr bool operator==(const expected<ValueT, ErrorT>& rhs) const { return this->u_ == rhs.u_; }
  constexpr bool operator!=(const expected<ValueT, ErrorT>& rhs) const { return this->u_ != rhs.u_; }

private:
  /// Underlying storage
  std::variant<ValueT, ErrorT> u_;
};

/**
 * @copydoc expected
 *
 * Specialization for <code>ValueT == void</code>
 */
template <typename ErrorT> class expected<void, ErrorT>
{
public:
  using value_type = void;
  using error_type = ErrorT;

  constexpr expected(const unexpected<ErrorT>& unexpected) : u_{std::in_place_type<ErrorT>, unexpected.error()} {}

  constexpr expected(unexpected<ErrorT>&& unexpected) : u_{std::in_place_type<ErrorT>, std::move(unexpected).error()} {}

  constexpr expected() = default;

  constexpr bool has_value() const noexcept { return std::holds_alternative<std::monostate>(u_); }
  constexpr operator bool() const noexcept { return has_value(); }

  constexpr const ErrorT& error() const& noexcept { return std::get<ErrorT>(u_); }
  constexpr ErrorT& error() & noexcept { return std::get<ErrorT>(u_); }
  constexpr const ErrorT&& error() const&& noexcept { return std::get<ErrorT>(u_); }
  constexpr ErrorT&& error() && noexcept { return std::move(std::get<ErrorT>(u_)); }

  constexpr bool operator==(const expected<void, ErrorT>& rhs) const { return this->u_ == rhs.u_; }
  constexpr bool operator!=(const expected<void, ErrorT>& rhs) const { return this->u_ != rhs.u_; }

private:
  /// Underlying storage
  std::variant<std::monostate, ErrorT> u_;
};

template <typename ValueT, typename ErrorT>
constexpr bool operator==(const expected<ValueT, ErrorT>& lhs, const ErrorT rhs)
{
  return !lhs.has_value() and lhs.error() == rhs;
}

template <typename ValueT, typename ErrorT>
constexpr bool operator!=(const expected<ValueT, ErrorT>& lhs, const ErrorT rhs)
{
  return lhs.has_value() or lhs.error() != rhs;
}

}  // namespace tyl
