cmake_minimum_required(VERSION 3.10)

# 1. Définition du projet et de la version
project(olivechess VERSION 0.3.3 LANGUAGES CXX)

# 2. Gestion du mode Release par défaut
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type de build" FORCE)
endif()

# 3. Configuration C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Génération du fichier version.h
# On le génère dans le dossier 'include' pour correspondre à votre structure 
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h"
)

# 5. Collecte des sources
file(GLOB SOURCES "src/*.cpp")

# 6. CRÉATION DE L'EXÉCUTABLE (Indispensable avant target_...)
add_executable(${PROJECT_NAME} ${SOURCES})

# 7. Configuration des dossiers d'inclusion
# Maintenant que 'olivechess' existe, on peut lui assigner des dossiers
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 8. Flags et Définitions spécifiques à l'OS [cite: 2]
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_definitions(${PROJECT_NAME} PRIVATE IS_64BIT)
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE IA32)
    endif()
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE OSX)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LINUX)
endif()

# 9. Flags de compilation selon le mode [cite: 3]
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()

# 10. Liaison des bibliothèques (Threads) 
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)