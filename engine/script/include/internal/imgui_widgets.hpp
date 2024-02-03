/**
 * @copyright 2023-present Brian Cairl
 *
 * @file imgui_widgets.hpp
 */
#pragma once

// C++ Standard Library
#include <cstring>
#include <optional>
#include <string>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

// Type
#include <tyl/crtp.hpp>


template <typename PopUpT> class SimplePopUpBase : public tyl::crtp_base<SimplePopUpBase<PopUpT>>
{
public:
  SimplePopUpBase(const char* widget_name = "#PopUp", const float width = 400.f) :
      open_at_pos_{std::nullopt}, widget_name_{widget_name}, widget_width_{width}
  {}

  void open(const ImVec2 pos) { open_at_pos_ = pos; }

  void open()
  {
    const auto r_min = ImGui::GetItemRectMin();
    const auto r_max = ImGui::GetItemRectMax();
    open_at_pos_.emplace(r_min.x, r_max.y);
  }

  void close()
  {
    if (open_at_pos_)
    {
      ImGui::CloseCurrentPopup();
    }
    this->derived().reset_impl();
  }

  bool is_open() const { return open_at_pos_.has_value(); }

  float width() const { return widget_width_; }

  float width_internal() const { return width() - 2 * ImGui::GetStyle().WindowPadding.x; }

protected:
  template <typename IsComplete> bool check(IsComplete is_complete)
  {
    if (open_at_pos_ and !ImGui::IsPopupOpen(widget_name_))
    {
      ImGui::SetNextWindowPos(*open_at_pos_);
      ImGui::SetNextWindowSize(ImVec2{widget_width_, 0});
      ImGui::OpenPopup(widget_name_);
    }

    static constexpr auto kPopUpFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    bool p_open = open_at_pos_.has_value();
    bool complete = false;
    if (ImGui::BeginPopupModal(widget_name_, &p_open, kPopUpFlags))
    {
      complete = is_complete();
      ImGui::EndPopup();
    }

    if (!p_open or complete)
    {
      on_complete();
    }

    return complete;
  }

private:
  void on_complete()
  {
    this->derived().reset_impl();
    open_at_pos_.reset();
    ImGui::CloseCurrentPopup();
  }

  static constexpr void reset_impl(){};

  std::optional<ImVec2> open_at_pos_ = std::nullopt;
  const char* widget_name_ = "none";
  float widget_width_ = 400.f;
};

template <std::size_t kBufferSize = 100UL> class InputTextPopUp : public SimplePopUpBase<InputTextPopUp<kBufferSize>>
{
  using Base = SimplePopUpBase<InputTextPopUp<kBufferSize>>;
  friend Base;

public:
  explicit InputTextPopUp(
    const char* default_text = "enter a name",
    const char* widget_name = "#PopUp",
    const float width = 400.f) :
      Base{widget_name, width}, text_default_{default_text}
  {
    reset_impl();
  }

  using Base::open;

  void open(const std::string& default_text)
  {
    text_default_ = default_text;
    reset_impl();
    this->open();
  }

  template <typename OnSubmitLabel> bool update(OnSubmitLabel on_submit_label)
  {
    return this->check([this, &on_submit_label]() -> bool {
      static constexpr auto kInputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue;
      const float input_text_width = this->width() - (ImGui::GetCursorPos().x + ImGui::GetStyle().WindowPadding.x);
      ImGui::SetNextItemWidth(input_text_width);
      if (ImGui::InputText("##name", text_buffer_, kBufferSize, kInputTextFlags))
      {
        on_submit_label(text_buffer_);
        return true;
      }
      return false;
    });
  }

private:
  void reset_impl() { std::strcpy(text_buffer_, text_default_.c_str()); }

  std::string text_default_ = "none";
  char text_buffer_[100];
};

class SubmissionPopUp : public SimplePopUpBase<SubmissionPopUp>
{
  using Base = SimplePopUpBase<SubmissionPopUp>;
  friend Base;

public:
  explicit SubmissionPopUp(const char* widget_name = "#PopUp", const float width = 400.f) : Base{widget_name, width} {}

  template <typename OnSubmit> [[nodiscard]] bool is_submitted(OnSubmit on_submit)
  {
    return this->check([&on_submit]() -> bool { return on_submit(); });
  }
};

class ConfirmationPopUp : public SimplePopUpBase<ConfirmationPopUp>
{
  using Base = SimplePopUpBase<ConfirmationPopUp>;
  friend Base;

public:
  explicit ConfirmationPopUp(const char* widget_name = "Are you sure?", const float width = 400.f) :
      Base{widget_name, width}
  {}

  [[nodiscard]] bool is_confirmed()
  {
    bool answer_yes = false;
    this->check([&answer_yes]() -> bool {
      bool answer_submitted = false;
      if (ImGui::Button("yes"))
      {
        answer_yes = true;
        answer_submitted = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("no"))
      {
        answer_yes = false;
        answer_submitted = true;
      }
      return answer_submitted;
    });
    return answer_yes;
  }
};