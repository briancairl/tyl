/**
 * @copyright 2023-present Brian Cairl
 *
 * @file render_pipeline_2D.hpp
 */
#pragma once

// C++ Standard Library
#include <memory>

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class RenderPipeline2D;

struct RenderPipeline2DOptions
{
  const char* name = "Render Pipeline 2D";
  std::size_t max_vertex_count = 1000;
};

template <> struct ScriptOptions<RenderPipeline2D>
{
  using type = RenderPipeline2DOptions;
};

class RenderPipeline2D : public ScriptBase<RenderPipeline2D>
{
  friend class ScriptBase<RenderPipeline2D>;

public:
  RenderPipeline2D(RenderPipeline2D&&) = default;

  ~RenderPipeline2D();

private:
  static expected<RenderPipeline2D, ScriptCreationError> CreateImpl(const RenderPipeline2DOptions& options);

  template <typename StreamT> void SaveImpl(ScriptOArchive<StreamT>& oar) const;

  template <typename StreamT> void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  RenderPipeline2DOptions options_;

  class Impl;
  std::unique_ptr<Impl> impl_;
  RenderPipeline2D(const RenderPipeline2DOptions& options, std::unique_ptr<Impl>&& impl);
};

}  // namespace tyl::engine
