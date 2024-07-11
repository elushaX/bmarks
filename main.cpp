// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for char_traits, operator+, string, basic_string

#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border

#include <rapidfuzz/fuzz.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

void saveResult(const std::string& res) {
  std::ofstream file(std::filesystem::path(getenv("HOME")) / std::filesystem::path(".bookmarks_res"));

  if (file.is_open()) {
    file << res << std::endl;
    file.close();
  }
}

void saveBookmarks(const std::vector<std::string> &bookmarks) {
  std::ofstream file(std::filesystem::path(getenv("HOME")) / std::filesystem::path(".bookmarks"));

  if (file.is_open()) {
    for (const auto &bookmark: bookmarks) {
      file << bookmark << "\n";
    }
    file.close();
  } else {
    std::cerr << "Unable to open file for writing.\n";
  }
}

std::vector<std::string> loadBookmarks() {
  std::vector<std::string> bookmarks;
  std::ifstream file(std::filesystem::path(getenv("HOME")) / std::filesystem::path(".bookmarks"));

  std::string line;
  while (std::getline(file, line)) {
    bookmarks.push_back(line);
  }
  file.close();

  return bookmarks;
}

std::vector<std::string> fuzzySortBookmarks(const std::string &pattern, const std::vector<std::string> &in) {
  std::vector<std::string> out;

  using Rating = std::pair<double, std::string>;

  std::vector<Rating> ratings;
  for (auto &str: in) {
    double score = rapidfuzz::fuzz::ratio(pattern, str);
    ratings.push_back({score, str});
  }

  std::sort(ratings.begin(), ratings.end(), [](Rating &first, Rating &second) { return first.first > second.first; });

  for (auto &rating: ratings) {
    out.push_back(rating.second);
  }

  return out;
}


int main() {
  using namespace ftxui;

  bool confirm = false;

  auto screen = ScreenInteractive::Fullscreen();

  std::vector<std::string> bookmarks;
  std::vector<std::string> bookmarks_filtered;

  bookmarks = loadBookmarks();
  bookmarks_filtered = bookmarks;

  int selected = 0;

  auto ui = Menu(&bookmarks_filtered, &selected);

  std::string first_name;

  InputOption option;
  option.multiline = false;

  option.on_enter = [&]() {
    confirm = true;
    screen.Exit();
  };

  option.on_change = [&]() {
    bookmarks_filtered = fuzzySortBookmarks(first_name, bookmarks);
  };

  option.transform = [](InputState state) {
    // state.element |= borderEmpty;

    if (state.is_placeholder) {
      // state.element |= dim;
    }

    // state.element |= borderDouble;
    // state.element |= bgcolor(Color::White);
    // state.element |= color(Color::Black);

    return state.element;
  };

  Component input_first_name = Input(&first_name, "bookmark name", option);

  input_first_name |= CatchEvent([&](Event event) {
    const auto character = event.character();
    auto skip = character == "-" || character == "+";

    option.on_change();

    return skip;
  });

  auto component = Container::Vertical({input_first_name});

  auto renderer = Renderer(component, [&] {
    return vbox({
                    separator(),
                    hbox(text(" Search Pattern: "), input_first_name->Render()),
                    ui->Render() | vscroll_indicator | frame | border | flex,
                });
  });

  renderer |= CatchEvent([&](Event event) -> bool {
    if (event.character() == "-") {
      if (!bookmarks.empty())
        bookmarks.erase(bookmarks_filtered.begin());
      // screen.Exit();
      //entries.pop_back();
    }
    if (event.character() == "+") {
      auto pwd = std::filesystem::current_path();

      if (std::find(bookmarks.begin(), bookmarks.end(), pwd) == bookmarks.end()) {
        bookmarks.push_back(pwd);
      }
    }

    if (event == Event::ArrowDown) {
      selected = std::clamp(selected + 1, 0, (int) bookmarks.size());
    }

    if (event == Event::ArrowUp) {
      selected = std::clamp(selected - 1, 0, (int) bookmarks.size());
    }

    return false;
  });

  screen.Loop(renderer);

  saveBookmarks(bookmarks);

  if (!bookmarks.empty() && confirm) {
    // printf("%s\n", bookmarks_filtered[selected].c_str());
    saveResult(bookmarks_filtered[selected].c_str());
  } else {
    auto pwd = std::filesystem::current_path();
    saveResult(pwd.c_str());
    // printf("%s\n", pwd.c_str());
  }
}