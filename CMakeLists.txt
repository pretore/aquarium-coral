cmake_minimum_required(VERSION 3.22)
project(aquarium-coral VERSION 3.0.5
        LANGUAGES C
        DESCRIPTION "Generic data structures in C")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Dependencies
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(cmake/FetchAquariumCMocka.cmake)
endif()
include(cmake/FetchAquariumSeaUrchin.cmake)
include(cmake/FetchAquariumSeagrass.cmake)
include(cmake/FetchAquariumRock.cmake)

# Sources
set(EXPORTED_HEADER_FILES
        include/coral/array_list.h
        include/coral/forward_queue.h
        include/coral/forward_stack.h
        include/coral/linked_red_black_tree_container.h
        include/coral/red_black_tree_container.h
        include/coral.h)
set(SOURCES
        ${EXPORTED_HEADER_FILES}
        src/array_list.c
        src/coral.c
        src/forward_queue.c
        src/forward_stack.c
        src/linked_red_black_tree_container.c
        src/red_black_tree_container.c)

if(DOXYGEN_FOUND)
    set(DOXYGEN_EXTRACT_ALL YES)
    doxygen_add_docs(${PROJECT_NAME}-documentation
            ${EXPORTED_HEADER_FILES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    add_library(${PROJECT_NAME} STATIC "")
    add_compile_definitions(TEST)
    target_sources(${PROJECT_NAME}
            PRIVATE
                ${SOURCES})
    target_link_libraries(${PROJECT_NAME}
            PUBLIC
                ${CMAKE_THREAD_LIBS_INIT}
                aquarium-cmocka
                aquarium-sea-urchin
                aquarium-seagrass
                aquarium-rock)
    target_include_directories(${PROJECT_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
    # Unit Tests
    enable_testing()
    # aquarium-coral-unit-test
    add_executable(${PROJECT_NAME}-unit-test test/test_coral.c)
    target_include_directories(${PROJECT_NAME}-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-unit-test ${PROJECT_NAME}-unit-test)
    # aquarium-coral-forward-queue-unit-test
    add_executable(${PROJECT_NAME}-forward-queue-unit-test
            test/test_forward_queue.c)
    target_include_directories(${PROJECT_NAME}-forward-queue-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-forward-queue-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-forward-queue-unit-test
            ${PROJECT_NAME}-forward-queue-unit-test)
    # aquarium-coral-forward-stack-unit-test
    add_executable(${PROJECT_NAME}-forward-stack-unit-test
            test/test_forward_stack.c)
    target_include_directories(${PROJECT_NAME}-forward-stack-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-forward-stack-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-forward-stack-unit-test
            ${PROJECT_NAME}-forward-stack-unit-test)
    # aquarium-coral-array-list-unit-test
    add_executable(${PROJECT_NAME}-array-list-unit-test
            test/test_array_list.c)
    target_include_directories(${PROJECT_NAME}-array-list-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-array-list-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-array-list-unit-test
            ${PROJECT_NAME}-array-list-unit-test)
    # aquarium-coral-linked-red-black-tree-container-unit-test
    add_executable(${PROJECT_NAME}-linked-red-black-tree-container-unit-test
            test/test_linked_red_black_tree_container.c)
    target_include_directories(
            ${PROJECT_NAME}-linked-red-black-tree-container-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(
            ${PROJECT_NAME}-linked-red-black-tree-container-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-linked-red-black-tree-container-unit-test
            ${PROJECT_NAME}-linked-red-black-tree-container-unit-test)
    # aquarium-coral-red-black-tree-container-unit-test
    add_executable(${PROJECT_NAME}-red-black-tree-container-unit-test
            test/test_red_black_tree_container.c)
    target_include_directories(
            ${PROJECT_NAME}-red-black-tree-container-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-red-black-tree-container-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-red-black-tree-container-unit-test
            ${PROJECT_NAME}-red-black-tree-container-unit-test)
else()
    add_library(${PROJECT_NAME} "")
    target_sources(${PROJECT_NAME}
            PRIVATE
                ${SOURCES})
    target_include_directories(${PROJECT_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
    target_link_libraries(${PROJECT_NAME}
            PUBLIC
                ${CMAKE_THREAD_LIBS_INIT}
                aquarium-sea-urchin
                aquarium-seagrass
                aquarium-rock)
    set_target_properties(${PROJECT_NAME}
            PROPERTIES
                VERSION ${PROJECT_VERSION}
                SOVERSION ${PROJECT_VERSION_MAJOR})
    include(GNUInstallDirs)
    install(DIRECTORY include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
