﻿# CMakeList.txt : CMake project for SqliteImgDoc, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (SqliteImgDoc STATIC 
						src/BlobOnHeap.cpp 
						src/CCustomQueries.cpp 
						src/CCustomQueries.h 
						src/Db.cpp 
						src/Db.h 
						src/DbBase.cpp 
						src/DbBase.h 
						src/DbCreationCommon.h
						src/DbCreation.cpp 
						src/DbCreation.h 
						src/DbCreation3D.cpp 
						src/DbCreation3D.h 
						src/DbDiscover.cpp 
						src/DbDiscover.h 
						src/DbGlobalInfo.h 
						src/DbGlobalInfo.cpp 
						src/DbGlobalInfo3D.h 
						src/DbGlobalInfo3D.cpp 
						src/DbRead.cpp 
						src/DbRead.h 
						src/DbReadCommon.h 
						src/DbRead3D.cpp 
						src/DbRead3D.h 
						src/DbWrite.cpp 
						src/DbWrite.h 
						src/DbWrite3D.cpp 
						src/DbWrite3D.h 
						src/DbWriteBase.cpp 
						src/DbWriteBase.h 
						src/pch.cpp 
						src/pch.h 
						src/IInternalDb.h 
						src/miscutilities.h
						src/miscutilities.cpp
						src/dbutils.h
						src/dbutils.cpp
						src/DbMasterInfoTable.h
						src/DbMasterInfoTable.cpp
						src/CoordinateData.cpp
						external/DataBinHdr.h 
						external/IDataObj.h 
						external/BlobOnHeap.h 
						external/DataObjImpl.h 
						external/DataTypes.h 
						external/DimCoordinateQueryClause.h 
						external/IBlob.h 
						external/IDimCoordinateQueryClause.h 
						external/ImportExport.h 
						external/Interface.h 
						external/ITileCoordinate.h 
						external/LogicalPositionInfo.h 
						external/TileCoordinate.h 
						external/Types.h 
						external/utilities.h
						external/SqliteImgDocException.h
						external/BlobData.h 
						"external/ITileInfoQueryClause.h" 
						"external/TileInfoQueryClause.h" "src/QueryBuildUtils.h" "src/QueryBuildUtils.cpp")

target_link_libraries(SqliteImgDoc PRIVATE SQLiteCpp)
#set_target_properties(SqliteImgDoc PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
set_target_properties(SqliteImgDoc PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS OFF)

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(SqliteImgDoc PUBLIC _SQLITEIMGDOCSTATICLIB=1)

##target_compile_features(SqliteImgDoc PRIVATE cxx_std_17)
#target_compile_features(SqliteImgDoc PRIVATE CXX_STANDARD 11 CXX_EXTENSIONS OFF)


# TODO: Add tests and install targets if needed.
