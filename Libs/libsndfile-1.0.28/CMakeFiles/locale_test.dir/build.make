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
include CMakeFiles/locale_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/locale_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/locale_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/locale_test.dir/flags.make

CMakeFiles/locale_test.dir/tests/locale_test.c.o: CMakeFiles/locale_test.dir/flags.make
CMakeFiles/locale_test.dir/tests/locale_test.c.o: tests/locale_test.c
CMakeFiles/locale_test.dir/tests/locale_test.c.o: CMakeFiles/locale_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/locale_test.dir/tests/locale_test.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/locale_test.dir/tests/locale_test.c.o -MF CMakeFiles/locale_test.dir/tests/locale_test.c.o.d -o CMakeFiles/locale_test.dir/tests/locale_test.c.o -c /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/tests/locale_test.c

CMakeFiles/locale_test.dir/tests/locale_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/locale_test.dir/tests/locale_test.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/tests/locale_test.c > CMakeFiles/locale_test.dir/tests/locale_test.c.i

CMakeFiles/locale_test.dir/tests/locale_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/locale_test.dir/tests/locale_test.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/tests/locale_test.c -o CMakeFiles/locale_test.dir/tests/locale_test.c.s

CMakeFiles/locale_test.dir/tests/utils.c.o: CMakeFiles/locale_test.dir/flags.make
CMakeFiles/locale_test.dir/tests/utils.c.o: tests/utils.c
CMakeFiles/locale_test.dir/tests/utils.c.o: CMakeFiles/locale_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/locale_test.dir/tests/utils.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/locale_test.dir/tests/utils.c.o -MF CMakeFiles/locale_test.dir/tests/utils.c.o.d -o CMakeFiles/locale_test.dir/tests/utils.c.o -c /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/tests/utils.c

CMakeFiles/locale_test.dir/tests/utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/locale_test.dir/tests/utils.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/tests/utils.c > CMakeFiles/locale_test.dir/tests/utils.c.i

CMakeFiles/locale_test.dir/tests/utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/locale_test.dir/tests/utils.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/tests/utils.c -o CMakeFiles/locale_test.dir/tests/utils.c.s

# Object files for target locale_test
locale_test_OBJECTS = \
"CMakeFiles/locale_test.dir/tests/locale_test.c.o" \
"CMakeFiles/locale_test.dir/tests/utils.c.o"

# External object files for target locale_test
locale_test_EXTERNAL_OBJECTS =

tests/locale_test: CMakeFiles/locale_test.dir/tests/locale_test.c.o
tests/locale_test: CMakeFiles/locale_test.dir/tests/utils.c.o
tests/locale_test: CMakeFiles/locale_test.dir/build.make
tests/locale_test: src/libsndfile.1.0.28.dylib
tests/locale_test: CMakeFiles/locale_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable tests/locale_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/locale_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/locale_test.dir/build: tests/locale_test
.PHONY : CMakeFiles/locale_test.dir/build

CMakeFiles/locale_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/locale_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/locale_test.dir/clean

CMakeFiles/locale_test.dir/depend:
	cd /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28 /Users/stp/Code/CSL6/Libs/libsndfile-1.0.28/CMakeFiles/locale_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/locale_test.dir/depend

