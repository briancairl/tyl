#pragma once

#include <tyl/serialization/named.hpp>
#include <tyl/serialization/object.hpp>

struct Trivial
{
  int x, y;
};

struct NonTrivial
{
  int x, y;
  NonTrivial() = default;
  NonTrivial(int _x, int _y) : x{_x}, y{_y} {}
};

const bool operator==(const Trivial& lhs, const Trivial& rhs) { return lhs.x == rhs.x and lhs.y == rhs.y; }

const bool operator==(const NonTrivial& lhs, const NonTrivial& rhs) { return lhs.x == rhs.x and lhs.y == rhs.y; }

namespace tyl::serialization
{

template <typename OArchive> struct save<OArchive, Trivial>
{
  void operator()(OArchive& ar, const Trivial& tv)
  {
    ar << named{"x", tv.x};
    ar << named{"y", tv.y};
  }
};

template <typename IArchive> struct load<IArchive, Trivial>
{
  void operator()(IArchive& ar, Trivial& tv)
  {
    ar >> named{"x", tv.x};
    ar >> named{"y", tv.y};
  }
};

template <typename OArchive> struct save<OArchive, NonTrivial>
{
  void operator()(OArchive& ar, const NonTrivial& ntv)
  {
    ar << named{"x", ntv.x};
    ar << named{"y", ntv.y};
  }
};

template <typename IArchive> struct load<IArchive, NonTrivial>
{
  void operator()(IArchive& ar, NonTrivial& ntv)
  {
    ar >> named{"x", ntv.x};
    ar >> named{"y", ntv.y};
  }
};

}  // namespace tyl::serialization
