﻿# CMakeList.txt : CMake project for CmdSqliteImgDoc, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(CmdSqliteImgDoc CmdSqliteImgDoc.cpp inc_libczi.h)

# Link test executable against gtest & gtest_main
target_link_libraries(CmdSqliteImgDoc SqliteImgDoc sqlite3 SQLiteCpp libCZIStatic)

##target_link_libraries(SqliteImgDocTests PRIVATE SQLiteCpp)
if(UNIX AND NOT APPLE)
   # Linux
   target_link_libraries(CmdSqliteImgDoc SQLiteCpp sqlite3 pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(CmdSqliteImgDoc SQLiteCpp sqlite3 pthread)
endif()

set_target_properties(CmdSqliteImgDoc  PROPERTIES
           CXX_STANDARD 11
           CXX_EXTENSIONS OFF)


