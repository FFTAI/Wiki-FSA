# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/afer/project_user/wiki-fsa/sdk-cpp/V4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/afer/project_user/wiki-fsa/build

# Include any dependencies generated for this target.
include build/CMakeFiles/demo_lookup.dir/depend.make

# Include the progress variables for this target.
include build/CMakeFiles/demo_lookup.dir/progress.make

# Include the compile flags for this target's objects.
include build/CMakeFiles/demo_lookup.dir/flags.make

build/CMakeFiles/demo_lookup.dir/demo_lookup.cpp.o: build/CMakeFiles/demo_lookup.dir/flags.make
build/CMakeFiles/demo_lookup.dir/demo_lookup.cpp.o: /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/demo_lookup.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/afer/project_user/wiki-fsa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object build/CMakeFiles/demo_lookup.dir/demo_lookup.cpp.o"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_lookup.dir/demo_lookup.cpp.o -c /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/demo_lookup.cpp

build/CMakeFiles/demo_lookup.dir/demo_lookup.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_lookup.dir/demo_lookup.cpp.i"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/demo_lookup.cpp > CMakeFiles/demo_lookup.dir/demo_lookup.cpp.i

build/CMakeFiles/demo_lookup.dir/demo_lookup.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_lookup.dir/demo_lookup.cpp.s"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/demo_lookup.cpp -o CMakeFiles/demo_lookup.dir/demo_lookup.cpp.s

build/CMakeFiles/demo_lookup.dir/fsa.cpp.o: build/CMakeFiles/demo_lookup.dir/flags.make
build/CMakeFiles/demo_lookup.dir/fsa.cpp.o: /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/fsa.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/afer/project_user/wiki-fsa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object build/CMakeFiles/demo_lookup.dir/fsa.cpp.o"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_lookup.dir/fsa.cpp.o -c /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/fsa.cpp

build/CMakeFiles/demo_lookup.dir/fsa.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_lookup.dir/fsa.cpp.i"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/fsa.cpp > CMakeFiles/demo_lookup.dir/fsa.cpp.i

build/CMakeFiles/demo_lookup.dir/fsa.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_lookup.dir/fsa.cpp.s"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/fsa.cpp -o CMakeFiles/demo_lookup.dir/fsa.cpp.s

build/CMakeFiles/demo_lookup.dir/os.cpp.o: build/CMakeFiles/demo_lookup.dir/flags.make
build/CMakeFiles/demo_lookup.dir/os.cpp.o: /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/os.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/afer/project_user/wiki-fsa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object build/CMakeFiles/demo_lookup.dir/os.cpp.o"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_lookup.dir/os.cpp.o -c /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/os.cpp

build/CMakeFiles/demo_lookup.dir/os.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_lookup.dir/os.cpp.i"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/os.cpp > CMakeFiles/demo_lookup.dir/os.cpp.i

build/CMakeFiles/demo_lookup.dir/os.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_lookup.dir/os.cpp.s"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/os.cpp -o CMakeFiles/demo_lookup.dir/os.cpp.s

build/CMakeFiles/demo_lookup.dir/logger.cpp.o: build/CMakeFiles/demo_lookup.dir/flags.make
build/CMakeFiles/demo_lookup.dir/logger.cpp.o: /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/logger.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/afer/project_user/wiki-fsa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object build/CMakeFiles/demo_lookup.dir/logger.cpp.o"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_lookup.dir/logger.cpp.o -c /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/logger.cpp

build/CMakeFiles/demo_lookup.dir/logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_lookup.dir/logger.cpp.i"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/logger.cpp > CMakeFiles/demo_lookup.dir/logger.cpp.i

build/CMakeFiles/demo_lookup.dir/logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_lookup.dir/logger.cpp.s"
	cd /home/afer/project_user/wiki-fsa/build/build && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source/logger.cpp -o CMakeFiles/demo_lookup.dir/logger.cpp.s

# Object files for target demo_lookup
demo_lookup_OBJECTS = \
"CMakeFiles/demo_lookup.dir/demo_lookup.cpp.o" \
"CMakeFiles/demo_lookup.dir/fsa.cpp.o" \
"CMakeFiles/demo_lookup.dir/os.cpp.o" \
"CMakeFiles/demo_lookup.dir/logger.cpp.o"

# External object files for target demo_lookup
demo_lookup_EXTERNAL_OBJECTS =

/home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup: build/CMakeFiles/demo_lookup.dir/demo_lookup.cpp.o
/home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup: build/CMakeFiles/demo_lookup.dir/fsa.cpp.o
/home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup: build/CMakeFiles/demo_lookup.dir/os.cpp.o
/home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup: build/CMakeFiles/demo_lookup.dir/logger.cpp.o
/home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup: build/CMakeFiles/demo_lookup.dir/build.make
/home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup: build/CMakeFiles/demo_lookup.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/afer/project_user/wiki-fsa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable /home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup"
	cd /home/afer/project_user/wiki-fsa/build/build && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_lookup.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build/CMakeFiles/demo_lookup.dir/build: /home/afer/project_user/wiki-fsa/sdk-cpp/V4/bin/demo_lookup

.PHONY : build/CMakeFiles/demo_lookup.dir/build

build/CMakeFiles/demo_lookup.dir/clean:
	cd /home/afer/project_user/wiki-fsa/build/build && $(CMAKE_COMMAND) -P CMakeFiles/demo_lookup.dir/cmake_clean.cmake
.PHONY : build/CMakeFiles/demo_lookup.dir/clean

build/CMakeFiles/demo_lookup.dir/depend:
	cd /home/afer/project_user/wiki-fsa/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/afer/project_user/wiki-fsa/sdk-cpp/V4 /home/afer/project_user/wiki-fsa/sdk-cpp/V4/source /home/afer/project_user/wiki-fsa/build /home/afer/project_user/wiki-fsa/build/build /home/afer/project_user/wiki-fsa/build/build/CMakeFiles/demo_lookup.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : build/CMakeFiles/demo_lookup.dir/depend

