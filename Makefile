# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = /home/joi/Workdir/codecrafts/codecrafters-git-cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/joi/Workdir/codecrafts/codecrafters-git-cpp

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/joi/Workdir/codecrafts/codecrafters-git-cpp/CMakeFiles /home/joi/Workdir/codecrafts/codecrafters-git-cpp//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/joi/Workdir/codecrafts/codecrafters-git-cpp/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named server

# Build rule for target.
server: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 server
.PHONY : server

# fast build rule for target.
server/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/build
.PHONY : server/fast

src/Error.o: src/Error.cpp.o
.PHONY : src/Error.o

# target to build an object file
src/Error.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/Error.cpp.o
.PHONY : src/Error.cpp.o

src/Error.i: src/Error.cpp.i
.PHONY : src/Error.i

# target to preprocess a source file
src/Error.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/Error.cpp.i
.PHONY : src/Error.cpp.i

src/Error.s: src/Error.cpp.s
.PHONY : src/Error.s

# target to generate assembly for a file
src/Error.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/Error.cpp.s
.PHONY : src/Error.cpp.s

src/Server.o: src/Server.cpp.o
.PHONY : src/Server.o

# target to build an object file
src/Server.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/Server.cpp.o
.PHONY : src/Server.cpp.o

src/Server.i: src/Server.cpp.i
.PHONY : src/Server.i

# target to preprocess a source file
src/Server.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/Server.cpp.i
.PHONY : src/Server.cpp.i

src/Server.s: src/Server.cpp.s
.PHONY : src/Server.s

# target to generate assembly for a file
src/Server.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/Server.cpp.s
.PHONY : src/Server.cpp.s

src/zlib_implement.o: src/zlib_implement.cpp.o
.PHONY : src/zlib_implement.o

# target to build an object file
src/zlib_implement.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/zlib_implement.cpp.o
.PHONY : src/zlib_implement.cpp.o

src/zlib_implement.i: src/zlib_implement.cpp.i
.PHONY : src/zlib_implement.i

# target to preprocess a source file
src/zlib_implement.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/zlib_implement.cpp.i
.PHONY : src/zlib_implement.cpp.i

src/zlib_implement.s: src/zlib_implement.cpp.s
.PHONY : src/zlib_implement.s

# target to generate assembly for a file
src/zlib_implement.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/zlib_implement.cpp.s
.PHONY : src/zlib_implement.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... server"
	@echo "... src/Error.o"
	@echo "... src/Error.i"
	@echo "... src/Error.s"
	@echo "... src/Server.o"
	@echo "... src/Server.i"
	@echo "... src/Server.s"
	@echo "... src/zlib_implement.o"
	@echo "... src/zlib_implement.i"
	@echo "... src/zlib_implement.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

