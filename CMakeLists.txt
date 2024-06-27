cmake_minimum_required(VERSION 3.20)
project(Ninjutso_SoraV2_state C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(hidapi)

add_executable(Ninjutso_SoraV2_state main.c)

# Linking hidapi library
target_link_libraries(Ninjutso_SoraV2_state PRIVATE hidapi::hidapi)

add_custom_command(TARGET Ninjutso_SoraV2_state POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:hidapi::hidapi> $<TARGET_FILE_DIR:Ninjutso_SoraV2_state>)