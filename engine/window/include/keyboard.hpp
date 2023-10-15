/**
 * @copyright 2023-present Brian Cairl
 *
 * @file key.hpp
 */
#pragma once

// C++ Standard Library
#include <cctype>

namespace tyl::engine
{

struct KeyState
{
  enum Code : std::uint8_t
  {
    PRESSED,
    RELEASED,
    HELD,
    NONE
  };

  Code code = NONE;

  constexpr void reset() { code = NONE; }
  constexpr bool is_none() const { return code == NONE; }
  constexpr bool is_released() const { return code == RELEASED; }
  constexpr bool is_pressed() const { return code == PRESSED; }
  constexpr bool is_held() const { return code == HELD; }
  constexpr bool is_down() const { return is_pressed() or is_held(); }
  constexpr bool is_up() const { return is_released() or is_none(); }

  KeyState& operator=(const KeyState& other) = default;

  KeyState& operator=(const KeyState::Code code)
  {
    this->code = code;
    return *this;
  };
};

struct KeyInfo
{
  enum KeyCode
  {
    NUM1,
    NUM2,
    NUM3,
    NUM4,
    NUM5,
    NUM6,
    NUM7,
    NUM8,
    NUM9,
    NUM0,
    Q,
    W,
    E,
    A,
    S,
    D,
    Z,
    X,
    C,
    SPACE,
    L_SHIFT,
    R_SHIFT,
    L_CTRL,
    R_CTRL,
    L_ALT,
    R_ALT,
    _KEY_COUNT,
  };

  static constexpr auto kKeyCount = static_cast<std::size_t>(_KEY_COUNT);

  constexpr KeyState& operator[](const KeyCode key) { return state[static_cast<std::size_t>(key)]; }
  constexpr const KeyState& operator[](const KeyCode key) const { return state[static_cast<std::size_t>(key)]; }

  constexpr auto* begin() { return state; }
  constexpr auto* end() { return state + kKeyCount; }

  constexpr const auto* begin() const { return state; }
  constexpr const auto* end() const { return state + kKeyCount; }

  KeyState state[kKeyCount];
};

}  // namespace tyl::engine
