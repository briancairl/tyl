// C++ Standard Library
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <utility>

// ImGui
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

// Art
#include <tyl/ui/file_dialogue.hpp>

namespace tyl::ui
{
namespace  //  anonymous
{

inline void sort_by_type(std::vector<FileListing>& listing)
{
  std::sort(listing.begin(), listing.end(), [](const auto& lhs, const auto& rhs) { return lhs.type > rhs.type; });
}

constexpr bool is_option_set(const FileDialogueOptions lhs, const FileDialogueOptions rhs)
{
  return static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs);
}

}  // namespace anonymous

FileListing::FileListing(
  filesystem::path _path,
  const filesystem::file_type _type,
  const bool _is_selected,
  const fs_clock::time_point _write_time) :
    path{std::move(_path)},
    type{_type},
    write_time{_write_time},
    is_selected{_is_selected}
{}

FileDialogue::FileDialogue(
  const char* selection_action_name,
  const char* file_listing_regex,
  const Options options,
  filesystem::path initial_directory,
  const std::size_t selection_buffer_len) :
    options_{options},
    error_{nullptr},
    selection_action_name_{selection_action_name},
    selection_edit_buffer_{
      [selection_buffer_len]() -> std::unique_ptr<char[]>{if (selection_buffer_len < 2){
                                 throw std::invalid_argument{"selection_buffer_len must be larger than 1"};
}
return std::make_unique<char[]>(selection_buffer_len);
}
()
}
, selection_edit_buffer_len_{selection_buffer_len},
  file_listing_regex_{(file_listing_regex == nullptr) ? "" : file_listing_regex}
{
  update_current_directory(initial_directory, false /* no caching previous directory */);
}

void FileDialogue::update_current_directory(const filesystem::path& next_directory, const bool cache_previous)
{
  // Add current directory to history
  if (!current_dir_.empty() and cache_previous)
  {
    previous_dirs_.push_back(current_dir_);
  }

  // Set current directort
  current_dir_ = next_directory;

  // Reset active selection
  selection_edit_buffer_[0] = '\0';

  // Reset the listing container
  file_listing_cache_.clear();
  file_listing_cache_.reserve(30);

  // Helper to check if file is allowed to be listed
  const auto is_accepted_file_type = [opt = options_,
                                      &re = file_listing_regex_](const auto& type, const auto& path) -> bool {
    std::cmatch m;
    return (!is_option_set(opt, Options::NoRegularFiles) and type == filesystem::file_type::regular and
            std::regex_search(path, m, re)) or
      (!is_option_set(opt, Options::NoDirectories) and type == filesystem::file_type::directory);
  };

  // Build valid file listing
  for (const auto& dir_path : filesystem::directory_iterator{current_dir_})
  {
    std::error_code ec;
    if (const auto status = dir_path.status(ec);
        is_accepted_file_type(status.type(), dir_path.path().filename().c_str()))
    {
      file_listing_cache_.emplace_back(
        dir_path.path().filename(), status.type(), false, last_write_time(dir_path.path()));
    }
  }

  // Do a default sorting on the listing
  sort_by_type(file_listing_cache_);

  // Caches parts of this filename
  current_dir_parts_.clear();
  std::transform(
    current_dir_.begin(),
    current_dir_.end(),
    std::back_inserter(current_dir_parts_),
    [](const auto& path_part) -> std::string { return path_part.string(); });
}

void FileDialogue::update_path_navigation(const std::size_t max_directory_segments)
{
  // Draw menu bar with current directory
  if (current_dir_parts_.empty())
  {
    return;
  }

  // Handle navigation history back-stepping
  {
    if (ImGui::ArrowButton("back", ImGuiDir_Left) and !previous_dirs_.empty())
    {
      const filesystem::path next_directory = previous_dirs_.back();
      previous_dirs_.pop_back();
      update_current_directory(next_directory, false /* no caching previous directory */);
    }

    if (!previous_dirs_.empty() and ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("go back to %s", previous_dirs_.back().c_str());
    }

    ImGui::SameLine();
  }

  // Get ImGui vars
  const auto& style = ImGui::GetStyle();
  const ImVec4* const ColorLookup = style.Colors;
  auto* drawlist = ImGui::GetWindowDrawList();

  // Get the last N-directory segments
  const std::size_t rewind_count =
    std::min<std::size_t>(max_directory_segments, std::distance(current_dir_parts_.begin(), current_dir_parts_.end()));
  const auto start_itr = std::prev(current_dir_parts_.end(), rewind_count);
  const auto last_itr = std::prev(current_dir_parts_.end(), 1);

  // Directory is drawn as selectable segments such that when a segment is hovered + clicked,
  // the user can navigate to that parent directory
  if (current_dir_parts_.begin() != start_itr)
  {
    ImGui::TextColored(ColorLookup[ImGuiCol_TextDisabled], "...");
    ImGui::SameLine();
    ImGui::TextUnformatted(" / ");
    ImGui::SameLine();
  }
  for (auto itr = start_itr; itr != current_dir_parts_.end(); ++itr)
  {
    if (itr == last_itr)
    {
      ImGui::TextUnformatted(itr->c_str());
    }
    else
    {
      ImGui::TextColored(ColorLookup[ImGuiCol_TextDisabled], itr->c_str());
      if (ImGui::IsItemHovered())
      {
        drawlist->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 200, 40));
      }
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
      {
        // Build new current directory up to selected sub-directory
        filesystem::path next_directory;
        std::for_each(current_dir_parts_.begin(), std::next(itr), [&next_directory](const std::string& path_part) {
          next_directory /= path_part;
        });

        // Update current directory info
        update_current_directory(next_directory);
      }
      ImGui::SameLine();
      ImGui::TextUnformatted(" / ");
      ImGui::SameLine();
    }
  }
}

FileDialogue::UpdateStatus FileDialogue::update(const std::size_t max_directory_segments)
{
  UpdateStatus status{UpdateStatus::Working};

  const auto& style = ImGui::GetStyle();
  const ImVec4* const ColorLookup = style.Colors;
  auto* drawlist = ImGui::GetWindowDrawList();

  // Draw menu bar with current directory
  update_path_navigation(max_directory_segments);

  // Get content origin/size before starting table element for main profiler panels
  const auto toggle_selection = [this](bool& toggle_selected) -> void {
    // If control is not pressed, do not multi-select; unselect all files first
    if (const auto& io = ImGui::GetIO(); is_option_set(options_, Options::NoMultiSelect) or !io.KeyCtrl)
    {
      for (auto& s : file_listing_cache_)
      {
        s.is_selected = false;
      }
    }

    // Toggle relevant flag
    toggle_selected = !toggle_selected;
  };

  ImGui::BeginChild(
    "##listing",
    ImVec2{0.f, std::max(0.f, ImGui::GetWindowSize().y - 6.f * ImGui::GetTextLineHeightWithSpacing())},
    true,
    ImGuiWindowFlags_AlwaysVerticalScrollbar);

  const ImVec2 content_pos{ImGui::GetWindowPos()};
  const ImVec2 content_size{ImGui::GetWindowSize() - ImVec2{style.ScrollbarSize + style.FramePadding.x, 0.f}};

  ImGui::BeginChild(
    "##listing_contents",
    ImVec2{0.f, file_listing_cache_.size() * ImGui::GetTextLineHeightWithSpacing()},
    false,
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  if (ImGui::BeginTable("##listing_contents_cols", 2, ImGuiTableFlags_Resizable))
  {
    // Clip highlights
    drawlist->PushClipRect(content_pos, content_pos + content_size);

    for (auto& [path, type, write_time, selected] : file_listing_cache_)
    {
      ImGui::TableNextColumn();

      // Draw listing text
      if (type == filesystem::file_type::directory)
      {
        ImGui::TextColored(ColorLookup[ImGuiCol_TextDisabled], path.c_str());
      }
      else if (is_option_set(options_, Options::AllowSelectRegularFile))
      {
        ImGui::TextUnformatted(path.c_str());
      }
      else
      {
        ImGui::TextColored(ColorLookup[ImGuiCol_TextDisabled], path.c_str());
      }

      // Highlight selected items
      if (selected)
      {
        const auto lower = ImGui::GetItemRectMin();
        const auto upper = ImGui::GetItemRectMax();
        drawlist->AddRectFilled(lower, ImVec2{lower.x + content_size.x, upper.y}, IM_COL32(255, 50, 50, 50));
      }

      // Highlight hovered items
      if (ImGui::IsItemHovered())
      {
        drawlist->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 200, 40));
      }

      // Handle selection/interaction of listed items
      if (!ImGui::IsItemClicked(ImGuiMouseButton_Left))
      {
        // pass
      }
      else if (type == filesystem::file_type::regular and is_option_set(options_, Options::AllowSelectRegularFile))
      {
        toggle_selection(selected);
        error_ = nullptr;
        std::strncpy(selection_edit_buffer_.get(), path.c_str(), selection_edit_buffer_len_);
      }
      else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
      {
        const filesystem::path next_directory{current_dir_ / path};
        update_current_directory(next_directory);
      }
      else if (is_option_set(options_, Options::AllowSelectDirectory))
      {
        toggle_selection(selected);
        error_ = nullptr;
        std::strncpy(selection_edit_buffer_.get(), path.c_str(), selection_edit_buffer_len_);
      }

      ImGui::TableNextColumn();
      const std::time_t write_time_cvt = fs_clock::to_time_t(write_time);
      ImGui::TextUnformatted(std::asctime(std::localtime(&write_time_cvt)));
    }
    drawlist->PopClipRect();

    ImGui::EndTable();
  }
  ImGui::EndChild();
  ImGui::EndChild();

  // Drawn the selected file information and allow filename editing
  bool target_file_selected = false;

  target_file_selected |= ImGui::InputText(
    "##selection_path_text",
    selection_edit_buffer_.get(),
    selection_edit_buffer_len_,
    ImGuiInputTextFlags_EnterReturnsTrue);
  ImGui::SameLine();
  target_file_selected |= ImGui::Button(selection_action_name_);

  if (!target_file_selected)
  {
    // pass
  }
  else if (const filesystem::path filename{selection_edit_buffer_.get()};
           std::find_if(file_listing_cache_.begin(), file_listing_cache_.end(), [&filename](const auto& f) {
             return filename == f.path;
           }) != file_listing_cache_.end())
  {
    status = UpdateStatus::Selected;
    ImGui::CloseCurrentPopup();
  }
  else if (const filesystem::path candidate_path{current_dir_ / filename};
           is_option_set(options_, Options::FileMustExist) and !exists(candidate_path))
  {
    error_ = "Selected file does not exist!";
  }
  else if (is_option_set(options_, Options::FileMustNotExist) and exists(candidate_path))
  {
    error_ = "Selected file already exists!";
  }
  else
  {
    file_listing_cache_.emplace_back(candidate_path.filename(), filesystem::file_type::regular, true /*selected*/);
    status = UpdateStatus::Selected;
    ImGui::CloseCurrentPopup();
  }

  // Draw any error information
  if (error_ != nullptr)
  {
    ImGui::TextColored(ImVec4{1.0f, 0.2f, 0.2f, 1.0f}, error_);
  }

  return status;
}

}  // namespace tyl::ui
