# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.blkFxgRD9C

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.blkFxgRD9C/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/DownloadManager.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/DownloadManager.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/DownloadManager.dir/flags.make

CMakeFiles/DownloadManager.dir/main.cpp.o: CMakeFiles/DownloadManager.dir/flags.make
CMakeFiles/DownloadManager.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.blkFxgRD9C/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/DownloadManager.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/DownloadManager.dir/main.cpp.o -c /tmp/tmp.blkFxgRD9C/main.cpp

CMakeFiles/DownloadManager.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DownloadManager.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.blkFxgRD9C/main.cpp > CMakeFiles/DownloadManager.dir/main.cpp.i

CMakeFiles/DownloadManager.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DownloadManager.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.blkFxgRD9C/main.cpp -o CMakeFiles/DownloadManager.dir/main.cpp.s

# Object files for target DownloadManager
DownloadManager_OBJECTS = \
"CMakeFiles/DownloadManager.dir/main.cpp.o"

# External object files for target DownloadManager
DownloadManager_EXTERNAL_OBJECTS =

DownloadManager: CMakeFiles/DownloadManager.dir/main.cpp.o
DownloadManager: CMakeFiles/DownloadManager.dir/build.make
DownloadManager: CMakeFiles/DownloadManager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.blkFxgRD9C/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable DownloadManager"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/DownloadManager.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/DownloadManager.dir/build: DownloadManager

.PHONY : CMakeFiles/DownloadManager.dir/build

CMakeFiles/DownloadManager.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/DownloadManager.dir/cmake_clean.cmake
.PHONY : CMakeFiles/DownloadManager.dir/clean

CMakeFiles/DownloadManager.dir/depend:
	cd /tmp/tmp.blkFxgRD9C/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.blkFxgRD9C /tmp/tmp.blkFxgRD9C /tmp/tmp.blkFxgRD9C/cmake-build-debug /tmp/tmp.blkFxgRD9C/cmake-build-debug /tmp/tmp.blkFxgRD9C/cmake-build-debug/CMakeFiles/DownloadManager.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/DownloadManager.dir/depend

