configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Include/cps/Config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/Include/cps/Config.h
)

set(CPS_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	${CMAKE_CURRENT_BINARY_DIR}/Include
	${EIGEN3_INCLUDE_DIR}
	${SPDLOG_INCLUDE_DIR}
)

set(CPS_LIBRARIES
	Threads::Threads
)

if(Linux_FOUND OR MacOS_FOUND)
	# TODO This doesn't actually depend on which OS or compiler is used, but which
	# stdlib. For now, under Linux only GNU's libstdc++ seems to work anyway.
	# With LLVM's libc++, -lc++fs should probably be used
	list(APPEND CPS_LIBRARIES stdc++fs)
endif()

add_subdirectory(Source)
