/**
 * @copyright 2023-present Brian Cairl
 *
 * @file drawing_2D.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class Drawing2D;

struct Drawing2DOptions
{
  const char* name = "Drawing 2D";
};

template <> struct ScriptOptions<Drawing2D>
{
  using type = Drawing2DOptions;
};

class Drawing2D : public ScriptBase<Drawing2D>
{
  friend class ScriptBase<Drawing2D>;

public:
  Drawing2D(Drawing2D&&) = default;

  ~Drawing2D();

private:
  static expected<Drawing2D, ScriptCreationError> CreateImpl(const Drawing2DOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  Drawing2DOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  Drawing2D(const Drawing2DOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
