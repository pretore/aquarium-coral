include(FetchContent)

FetchContent_Declare(
        aquarium-rock
        GIT_REPOSITORY https://github.com/pretore/aquarium-rock.git
        GIT_TAG v5.0.2
        GIT_SHALLOW 1
)

FetchContent_MakeAvailable(aquarium-rock)
