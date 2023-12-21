cmake_minimum_required(VERSION 3.25)
project(KG_MAG)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED COMPONENTS
        shaderc_combined)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw/glfw-3.3.8)
add_subdirectory(external/glm)

add_executable(Render
        main.cpp
        application.hpp
        application.cpp
        modules/affine_transform_2d/affine_transform_2d.hpp
        modules/affine_transform_2d/affine_transform_2d.cpp
        modules/affine_transform_3d/affine_transform_3d.hpp
        modules/affine_transform_3d/affine_transform_3d.cpp
        modules/model_2d/model_2d.hpp
        modules/model_2d/model_2d.cpp
        modules/model_3d/model_3d.hpp
        modules/model_3d/model_3d.cpp
        modules/camera_2d/camera_2d.hpp
        modules/camera_2d/camera_2d.cpp
        modules/camera_3d/camera_3d.hpp
        modules/camera_3d/camera_3d.cpp
        modules/view_2d/view_2d.hpp
        modules/view_2d/view_2d.cpp
        modules/view_3d/view_3d.hpp
        modules/view_3d/view_3d.cpp
        modules/scene_2d/scene_2d.hpp
        modules/scene_3d/scene_3d.hpp
        modules/application_2d/application_2d.cpp
        modules/application_2d/application_2d.hpp
        modules/application_3d/application_3d.cpp
        modules/application_3d/application_3d.hpp
        
        modules/vk_boilerplate/boilerplate.hpp
        modules/vk_boilerplate/boilerplate.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui.h
        external/imgui/imgui_impl_vulkan.cpp
        external/imgui/imgui_impl_vulkan.h
        external/imgui/imconfig.h
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_internal.h
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_impl_glfw.cpp
        external/imgui/imgui_impl_glfw.h
        modules/application_manual3d/application_manual3d.hpp
        modules/application_manual3d/application_manual3d.cpp
        application_manual.cpp
        application_manual.h
        modules/screen_3d/screen_3d.cpp
        modules/screen_3d/screen_3d.hpp)

target_include_directories(Render PUBLIC
        external
        modules)

target_link_libraries(Render
        glfw
        Vulkan::Vulkan
        Vulkan::shaderc_combined)

add_executable(Compiler
        compile.cpp)

target_link_libraries(Compiler
        Vulkan::shaderc_combined)