cmake_minimum_required(VERSION 3.20)

project(BankingSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.52.0
        GIT_SHALLOW TRUE
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
        GIT_SHALLOW TRUE
)

FetchContent_Declare(
        argon2_source
        GIT_REPOSITORY https://github.com/P-H-C/phc-winner-argon2.git
        GIT_TAG 20190702
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR unused-cmake-directory
)

FetchContent_MakeAvailable(
        httplib
        nlohmann_json
        argon2_source
)

add_library(
        Argon2
        STATIC
        ${argon2_source_SOURCE_DIR}/src/argon2.c
        ${argon2_source_SOURCE_DIR}/src/core.c
        ${argon2_source_SOURCE_DIR}/src/encoding.c
        ${argon2_source_SOURCE_DIR}/src/ref.c
        ${argon2_source_SOURCE_DIR}/src/thread.c
        ${argon2_source_SOURCE_DIR}/src/blake2/blake2b.c
)

target_include_directories(
        Argon2
        PUBLIC ${argon2_source_SOURCE_DIR}/include
        PRIVATE ${argon2_source_SOURCE_DIR}/src
)

target_link_libraries(Argon2 PRIVATE Threads::Threads)

add_library(
        BankingCore
        Account.cpp
        Account.h
        Transaction.cpp
        Transaction.h
        AccountManager.cpp
        AccountManager.h
        Utils.cpp
        Utils.h
        PinHasher.cpp
        PinHasher.h
        SecureRandom.cpp
        SecureRandom.h
)

target_include_directories(BankingCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BankingCore PUBLIC Argon2)
if(WIN32)
    target_link_libraries(BankingCore PUBLIC bcrypt)
endif()

add_executable(
        BankingSimulator
        main.cpp
        FirebaseDatabase.cpp
        FirebaseDatabase.h
        README.md

        web/index.html
        web/styles.css
        web/app.js
)

target_link_libraries(
        BankingSimulator
        PRIVATE
        httplib::httplib
        nlohmann_json::nlohmann_json
        CURL::libcurl
        BankingCore
)

add_custom_target(
        BankingWebFiles ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/web
                $<TARGET_FILE_DIR:BankingSimulator>/web
        COMMENT "Copying web frontend next to BankingSimulator"
)

add_dependencies(BankingSimulator BankingWebFiles)

include(CTest)
if(BUILD_TESTING)
    add_executable(SecurityTests tests/SecurityTests.cpp)
    target_link_libraries(SecurityTests PRIVATE BankingCore)
    add_test(NAME SecurityTests COMMAND SecurityTests)
endif()
