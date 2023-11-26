/**
 * @copyright 2023-present Brian Cairl
 *
 * @file script.hpp
 */
#pragma once

// C++ Standard Library
#include <filesystem>
#include <string_view>
#include <vector>

// Tyl
#include <tyl/async.hpp>
#include <tyl/clock.hpp>
#include <tyl/crtp.hpp>
#include <tyl/ecs.hpp>
#include <tyl/expected.hpp>
#include <tyl/rect.hpp>
#include <tyl/serialization/archive_fwd.hpp>

namespace tyl::engine
{
// Forward
struct Scene;

/**
 * @brief Resources used to update a script
 */
struct ScriptResources
{
  /// Current time
  Clock::Time now = Clock::Time::min();
  /// Handle to active engine GUI framework context
  void* gui_context;
  /// Drag-and-drop payloads
  std::vector<std::filesystem::path> drop_payloads = {};
  /// Location of cursor at time of drop
  Vec2f drop_cursor_position = Vec2f::Zero();
  /// Size of current view port
  Vec2f viewport_size = Vec2f::Zero();
  /// Current position of cursor in viewport
  Vec2f viewport_cursor_position = Vec2f::Zero();
  /// Current position of cursor in viewport (normalized)
  Vec2f viewport_cursor_position_normalized = Vec2f::Zero();
};

/**
 * @brief Resources used to update a script
 */
struct ScriptSharedState
{
  /// Thread pool for deferred work execution
  async::ThreadPool thread_pool;
};


/**
 * @brief Errors used on script creation failures
 */
enum class ScriptCreationError
{
  kInvalidOption,
  kInternalSetupFailure,
};

/**
 * @brief Statuses used on script update
 */
enum class ScriptStatus
{
  kOk
};

template <typename IStreamT> using ScriptIArchive = tyl::serialization::binary_iarchive<IStreamT>;

template <typename OStreamT> using ScriptOArchive = tyl::serialization::binary_oarchive<OStreamT>;

template <typename ScriptT> struct ScriptOptions;
// {
//   using type = ScriptTOptions;
// };

template <typename ScriptT> using script_options_t = typename ScriptOptions<ScriptT>::type;

/**
 * @brief Defines a common script interface
 */
template <typename ScriptT> class ScriptBase : public crtp_base<ScriptBase<ScriptT>>
{
public:
  template <typename StreamT> void save(ScriptOArchive<StreamT>& oar) const { this->derived().SaveImpl(oar); }

  template <typename StreamT> void load(ScriptIArchive<StreamT>& iar) { this->derived().LoadImpl(iar); }

  ScriptStatus update(Scene& scene, ScriptSharedState& shared, const ScriptResources& resources)
  {
    return this->derived().UpdateImpl(scene, shared, resources);
  }

  [[nodiscard]] static expected<ScriptT, ScriptCreationError> create(const script_options_t<ScriptT>& options)
  {
    return ScriptT::CreateImpl(options);
  }

  [[nodiscard]] static std::string_view name() { return ScriptT::NameImpl(); }

private:
  template <typename StreamT> static constexpr void SaveImpl([[maybe_unused]] ScriptOArchive<StreamT>& oar) {}

  template <typename StreamT> static constexpr void LoadImpl([[maybe_unused]] ScriptIArchive<StreamT>& iar) {}
};

}  // namespace tyl::engine
