/**
 * @copyright 2023-present Brian Cairl
 *
 * @file widget.hpp
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
 * @brief Resources used to update a widget
 */
struct WidgetResources
{
  /// Current time
  Clock::Time now = Clock::Time::min();
  /// Handle to active engine GUI framework context
  void* gui_context;
  /// Drag-and-drop payloads
  std::vector<std::filesystem::path> drop_payloads = {};
  /// Location at which
  Vec2f drop_cursor_position = Vec2f::Zero();
};

/**
 * @brief Resources used to update a widget
 */
struct WidgetSharedState
{
  /// Thread pool for deferred work execution
  async::ThreadPool thread_pool;
};


/**
 * @brief Errors used on widget creation failures
 */
enum class WidgetCreationError
{

};

/**
 * @brief Statuses used on widget update
 */
enum class WidgetStatus
{
  kOk
};

template <typename IStreamT> using WidgetIArchive = tyl::serialization::binary_iarchive<IStreamT>;

template <typename OStreamT> using WidgetOArchive = tyl::serialization::binary_oarchive<OStreamT>;

template <typename WidgetT> struct WidgetOptions;
// {
//   using type = WidgetTOptions;
// };

template <typename WidgetT> using widget_options_t = typename WidgetOptions<WidgetT>::type;

/**
 * @brief Defines a common widget interface
 */
template <typename WidgetT> class WidgetBase : public crtp_base<WidgetBase<WidgetT>>
{
public:
  template <typename StreamT> void save(WidgetOArchive<StreamT>& oar) const { this->derived().SaveImpl(oar); }

  template <typename StreamT> void load(WidgetIArchive<StreamT>& iar) { this->derived().LoadImpl(iar); }

  WidgetStatus update(Scene& scene, WidgetSharedState& shared, const WidgetResources& resources)
  {
    return this->derived().UpdateImpl(scene, shared, resources);
  }

  [[nodiscard]] static expected<WidgetT, WidgetCreationError> create(const widget_options_t<WidgetT>& options)
  {
    return WidgetT::CreateImpl(options);
  }

  [[nodiscard]] static std::string_view name() { return WidgetT::NameImpl(); }

private:
  template <typename StreamT> static constexpr void SaveImpl([[maybe_unused]] WidgetOArchive<StreamT>& oar) {}

  template <typename StreamT> static constexpr void LoadImpl([[maybe_unused]] WidgetIArchive<StreamT>& iar) {}
};

}  // namespace tyl::engine
