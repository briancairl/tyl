/**
 * @copyright 2023-present Brian Cairl
 *
 * @file crtp.hpp
 */
#pragma once

namespace tyl
{

template <typename DerivedT> class crtp_base;

template <template <typename> class CRTPBaseTemplate, typename DerivedT> class crtp_base<CRTPBaseTemplate<DerivedT>>
{
protected:
  constexpr DerivedT* derived_ptr() { return reinterpret_cast<DerivedT*>(this); }
  constexpr const DerivedT* derived_ptr() const { return reinterpret_cast<const DerivedT*>(this); }
  constexpr DerivedT& derived() { return *derived_ptr(); }
  constexpr const DerivedT& derived() const { return *derived_ptr(); }
};

}  // namespace tyl
