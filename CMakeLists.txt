# Встановлення змінних для шляху до SFML
set(PROJECT_NAME "TheFirst")
set(PROJECT_VERSION "0.1.0")

set(SFML_ROOT "${CMAKE_SOURCE_DIR}/liberis/SFML-3.0.0")
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
set(SFML_LIB_DIR "${SFML_ROOT}/lib")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
# Установлюємо тип збірки на Debug, якщо він ще не заданий
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Визначаємо директорію для вихідних файлів
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/TheFirstGame")
# Створюємо директорію, якщо її немає
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# Налаштування для проекту
cmake_minimum_required(VERSION 3.31)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_CXX_STANDARD 17)  # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Обов'язково використовувати цей стандарт
set(CMAKE_CXX_EXTENSIONS OFF)  # Вимкнути розширення компілятора

# Додаємо файли
set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/source")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Вказуємо шлях до заголовків SFML
include_directories("${SFML_INCLUDE_DIR}")

# Знайти бібліотеки SFML
find_library(SFML_GRAPHICS sfml-graphics PATHS ${SFML_LIB_DIR})
find_library(SFML_WINDOW sfml-window PATHS ${SFML_LIB_DIR})
find_library(SFML_SYSTEM sfml-system PATHS ${SFML_LIB_DIR})

# Лінкуємо бібліотеки SFML до проекту
target_link_libraries(${PROJECT_NAME} ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM})


# Копіюємо виконуваний файл після збірки
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${OUTPUT_DIR}
)

# Копіюємо всю деректорію з resource
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resource ${OUTPUT_DIR}/resource
)


