cmake_minimum_required(VERSION 3.21)

set(LIBRARY_NAME ucilib)
project(${LIBRARY_NAME})

add_compile_definitions(WIN32_LEAN_AND_MEAN)

set(COMMANDS 
	uci/commands.hpp
	uci/commands/command.cpp
	uci/commands/command.h
	uci/commands/position.cpp
	uci/commands/position.h
)

set(NOTES
	notes/classes.txt
	notes/example1.txt
	notes/state_machine.txt
	notes/state_machine_examples.txt
	notes/synchronization.txt
	notes/uci_client_examples.txt
	notes/uci_definition.txt
)

set(OPTIONS
	"uci/options/option.cpp"
	"uci/options/option.h"
	"uci/options/list.cpp"
	"uci/options/list.h"
	"uci/options/setoption.h"
)

set(UCI
	uci/debug.cpp
	uci/debug.h
	uci/go.cpp
	uci/go.h
	uci/info.cpp
	uci/info.h
	uci/uci.hpp
	uci/uci_base.cpp
	uci/uci_base.h
	uci/uci_client.cpp
	uci/uci_client.h
	uci/uci_server.cpp
	uci/uci_server.h
)

set(STATES 
	uci/states.hpp
	uci/states/base.cpp  
	uci/states/base.h
)

set(STATES_CLIENT
	uci/states/client/base.cpp
	uci/states/client/base.h
	uci/states/client/commit.cpp
	uci/states/client/commit.h
	uci/states/client/info.cpp
	uci/states/client/info.h
	uci/states/client/init.cpp
	uci/states/client/init.h
	uci/states/client/new_game.cpp
	uci/states/client/new_game.h
	uci/states/client/new_position.cpp
	uci/states/client/new_position.h
	uci/states/client/options.cpp
	uci/states/client/options.h
	uci/states/client/search.cpp
	uci/states/client/search.h
	uci/states/client/start_up.cpp
	uci/states/client/start_up.h
	uci/states/client/wait.cpp
	uci/states/client/wait.h
)

set(STATES_SERVER
	uci/states/server/base.cpp
	uci/states/server/base.h
	uci/states/server/commit.cpp
	uci/states/server/commit.h
	uci/states/server/info.cpp
	uci/states/server/info.h
	uci/states/server/init.cpp
	uci/states/server/init.h
	uci/states/server/new_game.cpp
	uci/states/server/new_game.h
	uci/states/server/new_position.cpp
	uci/states/server/new_position.h
	uci/states/server/options.cpp
	uci/states/server/options.h
	uci/states/server/search.cpp
	uci/states/server/search.h
	uci/states/server/start_up.cpp
	uci/states/server/start_up.h
	uci/states/server/wait.cpp
	uci/states/server/wait.h
)

set(STATE_MACHINES
	uci/state_machines/client.cpp
	uci/state_machines/client.h
	uci/state_machines/server.cpp
	uci/state_machines/server.h
	uci/state_machines/state_machine_base.cpp
	uci/state_machines/state_machine_base.h
)

add_library(${LIBRARY_NAME} STATIC 
	${COMMANDS}
	${NOTES}
	${OPTIONS}
	${UCI}
	${STATES}
	${STATES_CLIENT}
	${STATES_SERVER}
	${STATE_MACHINES}
)

target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_17)

include_directories(${LIBRARY_NAME}	".")

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

source_group(commands FILES ${COMMANDS})
source_group(notes FILES ${NOTES})
source_group("options" FILES ${OPTIONS})
source_group(states FILES ${STATES})
source_group(states/client FILES ${STATES_CLIENT})
source_group(states/server FILES ${STATES_SERVER})
source_group(state_machines FILES ${STATE_MACHINES})
source_group(uci FILES ${UCI})

# --- Libraries (VCPKG) ---
if (MSVC)
	message(STATUS "\nVCPKG packages will be automatically linked to visual studio\n")

	add_compile_definitions(_WIN32_WINNT=0x0601)	# same as '#define _WIN32_WINNT'
	add_compile_definitions(WIN32_LEAN_AND_MEAN)

else ()
	# --- For VCPKG packages ---
	#message(STATUS "\nCall cmake with: cmake .. -DCMAKE_TOOLCHAIN_FILE=/__path__/__to__/vcpkg/scripts/buildsystems/vcpkg.cmake")

	# ----- BOOST -----

	message(STATUS "\n----- BOOST -----")
	set(Boost_USE_STATIC_LIBS		OFF)
	set(Boost_USE_MULTITHREADED		OFF)
	set(Boost_USE_STATIC_RUNTIME	OFF)

	find_package(Boost REQUIRED COMPONENTS
		#algorithm		# leave out header-only packages
		#asio 			# leave out header-only packages
		#process		# leave out header-only packages
		system 
		filesystem 
		regex
		thread
	)

	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})

	target_link_libraries(${PROJECT_NAME} PUBLIC
		#Boost::algorithm	# leave out header-only packages
		#Boost::asio		# leave out header-only packages
		#Boost::process		# leave out header-only packages
		Boost::system
		Boost::filesystem
		Boost::regex
		${Boost_LIBRARIES}	# TODO: do we need this
	)

	message(STATUS "Include dirs ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost libraries ${Boost_LIBRARIES}")

	target_include_directories(${PROJECT_NAME} PUBLIC
		${Boost_INCLUDE_DIRS}
	)
endif (MSVC)

# --- Guten ---
message(STATUS "\n----- ForgeLib -----\n")

target_link_libraries(${LIBRARY_NAME} PUBLIC forgelib)

