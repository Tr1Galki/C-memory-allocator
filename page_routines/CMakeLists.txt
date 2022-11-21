add_library(platform_page_routines OBJECT)
target_include_directories(platform_page_routines PUBLIC .)

if (WIN32)
    target_sources(platform_page_routines PRIVATE windows.c)
else()
    target_sources(platform_page_routines PRIVATE unix.c)
endif()

add_library(page_routines OBJECT page_routines.c)
target_link_libraries(page_routines PUBLIC platform_page_routines)

add_library(page_routines_test OBJECT page_routines_test.c)
target_link_libraries(page_routines_test PUBLIC platform_page_routines)
