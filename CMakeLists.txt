cmake_minimum_required(VERSION 3.10)
project(bmarks)

set(CMAKE_CXX_STANDARD 17)


include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_subdirectory(external/rapidfuzz-cpp)

# Add your executable.
add_executable(${PROJECT_NAME} main.cpp)

# Link the FTXUI library.
target_link_libraries(${PROJECT_NAME} ftxui::screen ftxui::dom ftxui::component )
target_link_libraries(${PROJECT_NAME} rapidfuzz::rapidfuzz )

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)