cmake_minimum_required(VERSION 3.20)

project(HeavyMinOTAut LANGUAGES CXX)

include(CTest)
include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.54 REQUIRED COMPONENTS filesystem system)

# Build libvata's library target without its separate CLI and test suite.
FetchContent_Declare(
    libvata
    GIT_REPOSITORY https://github.com/ondrik/libvata.git
    GIT_TAG 8f9023ada7c467af28508376b2beb00a358f64dc
    SOURCE_SUBDIR src
)
FetchContent_MakeAvailable(libvata)

target_include_directories(libvata PUBLIC "${libvata_SOURCE_DIR}/include")
target_link_libraries(libvata PUBLIC Boost::filesystem Boost::system)

add_executable(minotaut
    src/Common/automatonHelper.cpp
    src/Common/combinationsGenerator.cpp
    src/Common/common.cpp
    src/Common/debugging.cpp
    src/Common/exceptions.cpp
    src/Common/executionOptions.cpp
    src/Common/historiesOfAttacks.cpp
    src/Common/simulationHelper.cpp
    src/Common/statisticalResults.cpp
    src/combined_relation.cpp
    src/dw_simulation.cpp
    src/main.cpp
    src/minimizationTechniques.cpp
    src/pruning.cpp
    src/quotienting.cpp
    src/saturation.cpp
    src/step.cpp
    src/up_simulation.cpp
    testers/common.cpp
    testers/complement_tester.cpp
    testers/minimizationAndComplement_testers.cpp
    testers/minimizationTechniques_testers.cpp
    testers/relationsComputation_testers.cpp
)

target_include_directories(minotaut PRIVATE include)
target_compile_features(minotaut PRIVATE cxx_std_11)
set_target_properties(minotaut PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(minotaut PRIVATE libvata Boost::filesystem Boost::system)

install(TARGETS minotaut RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(BUILD_TESTING)
    add_test(NAME minotaut-help COMMAND minotaut -help)
endif()
