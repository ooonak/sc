macro(enable_clang_tidy)
    find_program(clang_tidy_cmd NAMES "clang-tidy")
    if(NOT clang_tidy_cmd)
        message(WARNING "clang-tidy not found!")
    else()
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/.clang-tidy")
            message(FATAL_ERROR "'${CMAKE_SOURCE_DIR}/.clang-tidy' configuration file not found!")
        endif()
        set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_cmd}")
    endif()
endmacro()

macro(enable_cppcheck)
  find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
  if (CMAKE_CXX_CPPCHECK)
    list(
        APPEND CMAKE_CXX_CPPCHECK
            "--enable=all"
            "--inconclusive"
            "--force"
            "--inline-suppr"
            "--std=c++${CMAKE_CXX_STANDARD}"
            "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
    )
  endif()
endmacro()
