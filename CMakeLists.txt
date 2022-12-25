cmake_minimum_required(VERSION 3.22)
project(aquarium-coral VERSION 1.0.0
        LANGUAGES C
        DESCRIPTION "Abstract data structures in C")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Dependencies
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)
include(cmake/FetchAquariumRock.cmake)

# Sources
set(EXPORTED_HEADER_FILES
        include/coral/array_list.h
        include/coral/error.h
        include/coral/linked_queue.h
        include/coral/linked_red_black_tree_set.h
        include/coral/linked_stack.h
        include/coral/red_black_tree_map.h
        include/coral/red_black_tree_set.h
        include/coral.h)
set(SOURCES
        ${EXPORTED_HEADER_FILES}
        src/array_list.c
        src/coral.c
        src/error.c
        src/linked_queue.c
        src/linked_red_black_tree_set.c
        src/linked_stack.c
        src/red_black_tree_map.c
        src/red_black_tree_set.c)

if(DOXYGEN_FOUND)
    set(DOXYGEN_EXTRACT_ALL YES)
    doxygen_add_docs(${PROJECT_NAME}-documentation
            ${EXPORTED_HEADER_FILES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    include(cmake/FetchCMocka.cmake)
    add_library(${PROJECT_NAME} STATIC "")
    add_compile_definitions(TEST)
    target_sources(${PROJECT_NAME}
            PRIVATE
                ${SOURCES}
                src/test/cmocka.h
                src/test/cmocka.c)
    target_link_libraries(${PROJECT_NAME}
            PUBLIC
                ${CMAKE_THREAD_LIBS_INIT}
                ${CMOCKA_SHARED_LIBRARY}
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
    # aquarium-coral-red-black-tree-set-unit-test
    add_executable(${PROJECT_NAME}-red-black-tree-set-unit-test
            test/test_red_black_tree_set.c)
    target_include_directories(${PROJECT_NAME}-red-black-tree-set-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-red-black-tree-set-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-red-black-tree-set-unit-test
            ${PROJECT_NAME}-red-black-tree-set-unit-test)
    # aquarium-coral-red-black-tree-map-unit-test
    add_executable(${PROJECT_NAME}-red-black-tree-map-unit-test
            test/test_red_black_tree_map.c)
    target_include_directories(${PROJECT_NAME}-red-black-tree-map-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-red-black-tree-map-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-red-black-tree-map-unit-test
            ${PROJECT_NAME}-red-black-tree-map-unit-test)
    # aquarium-coral-linked-red-black-tree-set-unit-test
    add_executable(${PROJECT_NAME}-linked-red-black-tree-set-unit-test
            test/test_linked_red_black_tree_set.c)
    target_include_directories(${PROJECT_NAME}-linked-red-black-tree-set-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-linked-red-black-tree-set-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-linked-red-black-tree-set-unit-test
            ${PROJECT_NAME}-linked-red-black-tree-set-unit-test)
    # aquarium-coral-linked-queue-unit-test
    add_executable(${PROJECT_NAME}-linked-queue-unit-test
            test/test_linked_queue.c)
    target_include_directories(${PROJECT_NAME}-linked-queue-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-linked-queue-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-linked-queue-unit-test
            ${PROJECT_NAME}-linked-queue-unit-test)
    # aquarium-coral-linked-stack-unit-test
    add_executable(${PROJECT_NAME}-linked-stack-unit-test
            test/test_linked_stack.c)
    target_include_directories(${PROJECT_NAME}-linked-stack-unit-test
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
    target_link_libraries(${PROJECT_NAME}-linked-stack-unit-test
            PRIVATE
                ${PROJECT_NAME})
    add_test(${PROJECT_NAME}-linked-stack-unit-test
            ${PROJECT_NAME}-linked-stack-unit-test)
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
