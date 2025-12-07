add_library(${PROJECT_NAME} SHARED
    optimizer.c
)
target_link_libraries(${PROJECT_NAME} m)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
