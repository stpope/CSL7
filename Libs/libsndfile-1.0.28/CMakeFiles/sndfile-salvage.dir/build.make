# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.24.1/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.24.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28

# Include any dependencies generated for this target.
include CMakeFiles/sndfile-salvage.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sndfile-salvage.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sndfile-salvage.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sndfile-salvage.dir/flags.make

CMakeFiles/sndfile-salvage.dir/programs/common.c.o: CMakeFiles/sndfile-salvage.dir/flags.make
CMakeFiles/sndfile-salvage.dir/programs/common.c.o: programs/common.c
CMakeFiles/sndfile-salvage.dir/programs/common.c.o: CMakeFiles/sndfile-salvage.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/sndfile-salvage.dir/programs/common.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/sndfile-salvage.dir/programs/common.c.o -MF CMakeFiles/sndfile-salvage.dir/programs/common.c.o.d -o CMakeFiles/sndfile-salvage.dir/programs/common.c.o -c /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/programs/common.c

CMakeFiles/sndfile-salvage.dir/programs/common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sndfile-salvage.dir/programs/common.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/programs/common.c > CMakeFiles/sndfile-salvage.dir/programs/common.c.i

CMakeFiles/sndfile-salvage.dir/programs/common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sndfile-salvage.dir/programs/common.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/programs/common.c -o CMakeFiles/sndfile-salvage.dir/programs/common.c.s

CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o: CMakeFiles/sndfile-salvage.dir/flags.make
CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o: programs/sndfile-salvage.c
CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o: CMakeFiles/sndfile-salvage.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o -MF CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o.d -o CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o -c /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/programs/sndfile-salvage.c

CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/programs/sndfile-salvage.c > CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.i

CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/programs/sndfile-salvage.c -o CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.s

# Object files for target sndfile-salvage
sndfile__salvage_OBJECTS = \
"CMakeFiles/sndfile-salvage.dir/programs/common.c.o" \
"CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o"

# External object files for target sndfile-salvage
sndfile__salvage_EXTERNAL_OBJECTS =

programs/sndfile-salvage: CMakeFiles/sndfile-salvage.dir/programs/common.c.o
programs/sndfile-salvage: CMakeFiles/sndfile-salvage.dir/programs/sndfile-salvage.c.o
programs/sndfile-salvage: CMakeFiles/sndfile-salvage.dir/build.make
programs/sndfile-salvage: src/libsndfile.1.0.28.dylib
programs/sndfile-salvage: CMakeFiles/sndfile-salvage.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable programs/sndfile-salvage"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sndfile-salvage.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sndfile-salvage.dir/build: programs/sndfile-salvage
.PHONY : CMakeFiles/sndfile-salvage.dir/build

CMakeFiles/sndfile-salvage.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sndfile-salvage.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sndfile-salvage.dir/clean

CMakeFiles/sndfile-salvage.dir/depend:
	cd /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles/sndfile-salvage.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sndfile-salvage.dir/depend
