cmake_minimum_required(VERSION 3.10)

# Nom et version extraits du Makefile [cite: 1]
project(olivechess VERSION 0.3.3 LANGUAGES CXX)

# --- CONFIGURATION DU MODE PAR DÉFAUT ---
# Si l'utilisateur fait juste "cmake ..", on force le mode Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type de build" FORCE)
    message(STATUS "Build type non spécifié : passage en mode Release par défaut.")
endif()

# Standard C++17 (le Makefile utilisait -ansi/-std=c++14) [cite: 1]
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags de compilation globaux [cite: 1]
add_compile_options(-W -Wall -pedantic)

# Détection de l'OS et définitions (équivalent des blocs ifeq/else du Makefile) [cite: 2]
if(WIN32)
    add_definitions(-DWIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DIS_64BIT)
    else()
        add_definitions(-DIA32)
    endif()
elseif(APPLE)
    add_definitions(-DOSX)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DLINUX)
endif()

# Inclusion du dossier des headers [cite: 1]
include_directories(include)

# Récupération de tous les fichiers sources 
file(GLOB SOURCES "src/*.cpp")

# Définition de l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Gestion des Threads (équivalent de -lpthread) [cite: 1]
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# --- GESTION DES FLAGS SELON LE MODE ---
# Équivalent des cibles 'debug:' et 'release:' du Makefile 
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -g0)
endif()