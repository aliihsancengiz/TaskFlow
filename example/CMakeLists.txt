set(TARGET "task-flow-ex")

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} pthread)
target_include_directories(
	${TARGET} 
	PUBLIC
		${CMAKE_SOURCE_DIR}/include
)