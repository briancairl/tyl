/**
 * @copyright 2023-present Brian Cairl
 *
 * @file scene_management.hpp
 */
#pragma once

// Tyl
#include <tyl/engine/script/script.hpp>

namespace tyl::engine
{
class SceneManagement;

struct SceneManagementOptions
{
  const char* name = "Scene";
};

template <> struct ScriptOptions<SceneManagement>
{
  using type = SceneManagementOptions;
};

class SceneManagement : public ScriptBase<SceneManagement>
{
  friend class ScriptBase<SceneManagement>;

public:
  SceneManagement(SceneManagement&&) = default;

  ~SceneManagement();

private:
  static expected<SceneManagement, ScriptCreationError> CreateImpl(const SceneManagementOptions& options);

  template <typename StreamT> static void SaveImpl(ScriptOArchive<StreamT>& oar);

  template <typename StreamT> static void LoadImpl(ScriptIArchive<StreamT>& iar);

  ScriptStatus UpdateImpl(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources);

  SceneManagementOptions options_;

  SceneManagement(const SceneManagementOptions& options);
};

}  // namespace tyl::engine
