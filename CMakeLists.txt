project(lab11 C CXX)
cmake_minimum_required(VERSION 3.10)

set(ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/../)
set(UI_DIR ${ROOT_DIR}/ui)
set(SRC_DIR ${ROOT_DIR}/src/)
set(SRCS
	${SRC_DIR}/main.c
	${SRC_DIR}/main_frame.c
	${SRC_DIR}/table_template_frame.c
	${SRC_DIR}/database.c
	${SRC_DIR}/login_frame.c
)

set(CMAKE_C_FLAGS "-O0 -ggdb")

find_package(PkgConfig REQUIRED)
find_package(PostgreSQL REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${PostgreSQL_LIBRARY_DIRS})
add_definitions(
	${GTK3_CFLAGS_OTHER}
)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} ${PostgreSQL_LIBRARIES})
target_compile_definitions(lab11 PUBLIC UI_DIR="${UI_DIR}/")

