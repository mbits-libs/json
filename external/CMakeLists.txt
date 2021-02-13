if (NOT TARGET mstch)
    set(WITH_INSTALLER OFF CACHE BOOL "")
    add_subdirectory(libmstch)
endif()
