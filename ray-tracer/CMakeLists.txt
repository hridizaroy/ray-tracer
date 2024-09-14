# CMakeList.txt : CMake project for ray-tracer

include_directories(
  "${PROJECT_SOURCE_DIR}/glm/glm"
  "${PROJECT_SOURCE_DIR}/SDL2-2.30.6/include"
)

add_executable (ray-tracer "ray-tracer.cpp"  "myApp.cpp" "myApp.h" "renderer.h" "renderer.cpp" "scene.h" "config.h" "ray.h")

target_link_libraries(ray-tracer
"${PROJECT_SOURCE_DIR}/SDL2-2.30.6/lib/x64/SDL2.lib"
"${PROJECT_SOURCE_DIR}/SDL2-2.30.6/lib/x64/SDL2Main.lib"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ray-tracer PROPERTY CXX_STANDARD 20)
endif()
