#pragma once

// C++ Standard Library
#include <chrono>
#include <iterator>
#include <memory>
#include <regex>
#include <string>
#include <vector>

// Tyl
#include <tyl/common/filesystem.hpp>

namespace tyl::ui
{

using fs_clock = std::chrono::system_clock;

class FileDialogue;

/**
 * @brief Path with meta information
 */
struct FileListing
{
  filesystem::path path;
  filesystem::file_type type;
  fs_clock::time_point write_time;
  bool is_selected;

  inline FileListing(
    filesystem::path _path,
    const filesystem::file_type _type,
    const bool _is_selected = false,
    const fs_clock::time_point _write_time = fs_clock::time_point{});
};

/**
 * @brief Iterator adapter used to inspect files selected with FileDialogue
 */
template <typename BaseIteratorT> class FileListingIterator
{
public:
  /**
   * @brief Returns underlying iterator object
   */
  inline BaseIteratorT base() const { return curr_p_; }

  /**
   * @brief Advances to next SELETED file listing
   */
  inline FileListingIterator& operator++()
  {
    do
    {
      ++curr_p_;
    } while (curr_p_ != last_p_ and !curr_p_->is_selected);
    return *this;
  }

  /**
   * @brief Advances to next SELETED file listing
   */
  inline FileListingIterator operator++(int)
  {
    FileListingIterator previous{curr_p_, last_p_, dir_p_};
    this->operator++();
    return previous;
  }

  /**
   * @brief Returns absolute path to file listing
   */
  inline filesystem::path operator*() const { return (*dir_p_ / curr_p_->path); }

  inline bool operator==(const FileListingIterator& other) const { return this->curr_p_ == other.curr_p_; }

  inline bool operator!=(const FileListingIterator& other) const { return this->curr_p_ != other.curr_p_; }

  FileListingIterator(const FileListingIterator& other) :
      curr_p_{other.curr_p_},
      last_p_{other.last_p_},
      dir_p_{other.dir_p_} {};

private:
  friend class FileDialogue;

  FileListingIterator(BaseIteratorT curr_path_p, BaseIteratorT const last_path_p, const filesystem::path* const dir_p) :
      curr_p_{curr_path_p},
      last_p_{last_path_p},
      dir_p_{dir_p}
  {
    // Iterate to first selected file listing; may iterate to the (end) listing
    if (curr_p_ != last_p_ and !curr_p_->is_selected)
    {
      this->operator++();
    }
  };

  /// Current listing iterator
  BaseIteratorT curr_p_;

  /// One-past last listing iterator (end)
  const BaseIteratorT last_p_;

  /// Active file listing directory
  const filesystem::path* const dir_p_;
};

template <typename BaseIteratorT>
FileListingIterator(BaseIteratorT, BaseIteratorT, filesystem::path*)->FileListingIterator<BaseIteratorT>;

/**
 * @brief Dialogue options
 */
enum class FileDialogueOptions : unsigned
{
  None = 0,
  FileMustExist = 1 << 0,  //< Require that selected path/filename must exist in the filesystem
  FileMustNotExist = 1 << 1,  //< Require that selected path/filename must NOT exist in the filesystem
  AllowSelectRegularFile = 1 << 2,  //< Allow selection of regular files
  AllowSelectDirectory = 1 << 3,  //< Allow selection of directory files
  NoRegularFiles = 1 << 4,  //< Do not display regular files
  NoDirectories = 1 << 5,  //< Do not display directory files
  NoMultiSelect = 1 << 6,  //< Do not allow multi-file selection
};

/**
 * @brief Options aggregation operator
 */
constexpr FileDialogueOptions operator|(const FileDialogueOptions lhs, const FileDialogueOptions rhs)
{
  return static_cast<FileDialogueOptions>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

/**
 * @brief A file/path selection widget
 */
class FileDialogue
{
public:
  using Options = FileDialogueOptions;

  /**
   * @brief Status codes returned on update
   */
  enum class UpdateStatus
  {
    Working,  //< Dialogue has not reached a terminal state
    Closing,  //< Dialogue closed without selection
    Selected,  //< Dialogue closed with a valid selection
  };

  explicit FileDialogue(
    const char* selection_action_name = "open",
    const char* file_listing_regex = nullptr,
    const Options options = Options::FileMustExist | Options::AllowSelectRegularFile,
    filesystem::path initial_directory = filesystem::current_path(),
    const std::size_t selection_buffer_len = 256);

  ~FileDialogue() = default;

  /**
   * @brief Draws file dilague and handles users interactions
   */
  UpdateStatus update(const std::size_t max_directory_segments = 5UL);

  /**
   * @brief Returns current editor directory
   * @note Always valid after FileDialogue::update()
   */
  inline const filesystem::path& current_directory() const { return current_dir_; }

  /**
   * @brief Returns first selected path iterator
   * @warning Only valid when FileDialogue::update() returns <code>UpdateStatus::Selected</code>
   */
  inline auto begin() const
  {
    return FileListingIterator{file_listing_cache_.begin(), file_listing_cache_.end(), std::addressof(current_dir_)};
  }

  /**
   * @brief Returns one-past-last selected path iterator
   * @warning Only valid when FileDialogue::update() returns <code>UpdateStatus::Selected</code>
   */
  inline auto end() const
  {
    return FileListingIterator{file_listing_cache_.end(), file_listing_cache_.end(), std::addressof(current_dir_)};
  }

private:
  /**
   * @brief Repopulated file_listing_cache_ given current_dir_
   */
  void update_current_directory(const filesystem::path& next_directory, const bool cache_previous = true);

  /**
   * @brief Update current directory navigation
   */
  void update_path_navigation(const std::size_t max_directory_segments);

  /// Display/interaction options
  Options options_;

  /// Current error string
  const char* error_;

  /// Action label used for selection button
  const char* selection_action_name_;

  /// Buffer used to hold editable path text
  std::unique_ptr<char[]> selection_edit_buffer_;

  /// Max edit buffer capacity
  std::size_t selection_edit_buffer_len_;

  /// Regex pattern used with updating listing cache
  std::regex file_listing_regex_;

  /// Current filepath being displayed
  filesystem::path current_dir_;

  /// Cache of current directory parts
  std::vector<std::string> current_dir_parts_;

  /// Cache of previous directories navigated to
  std::vector<filesystem::path> previous_dirs_;

  /// Cache of files displayed in the current directory
  std::vector<FileListing> file_listing_cache_;
};

}  // namespace tyl::ui


namespace std
{

template <typename BaseIteratorT> struct iterator_traits<::tyl::ui::FileListingIterator<BaseIteratorT>>
{
  using difference_type = std::ptrdiff_t;
  using value_type = ::tyl::filesystem::path;
  using pointer = void;
  using reference = void;
  using iterator_category = std::forward_iterator_tag;
};

}  // namespace std
