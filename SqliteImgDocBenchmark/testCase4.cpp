#include "testCase4.h"
#include "inc_sqliteimgdoc.h"
#include <chrono>
#include <sstream>
#include <random>
#include "../SqliteImgDoc/src/Db.h"
#include "../SqliteImgDoc/src/DbDiscover.h"
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace SlImgDoc;

TestCase4::TestCase4(const ParametersTestCase4& params)
    : rowCount(params.rowCount),
    columnCount(params.columnCount),
    tileWidth(params.tileWidth),
    tileHeight(params.tileHeight),
    tCount(params.tCount),
    zCount(params.zCount),
    cCount(params.cCount)
{
}

BenchmarkItem TestCase4::RunTest1()
{
    auto db = this->CreateDb(true, false, false);

    db->GetWriter()->CreateIndexOnCoordinate('M');
    db->GetWriter()->CreateIndexOnCoordinate('T');
    db->GetWriter()->CreateIndexOnCoordinate('Z');
    db->GetWriter()->CreateIndexOnCoordinate('C');

    // Test: Discover dimensions
    auto internalDb = std::dynamic_pointer_cast<IInternalDb>(db);

    CDbDiscover discover(&internalDb->GetDb());
    discover.DoIt();

    auto docInfo = discover.GetDocInfo();
    auto tileDims = docInfo->GetTileDimensions();
    
    // COUNT(DISTINCT Dim_M), COUNT(DISTINCT Dim_T), COUNT(DISTINCT Dim_Z), COUNT(DISTINCT Dim_C),
    stringstream dimsString;
    for (const auto& d : tileDims)
    {
        string dimName;
        if (docInfo->GetTileInfoColumnNameForDimension(d, dimName))
        {
            dimsString << "COUNT(DISTINCT " << dimName << "),";
        }
    }
    
    /*
    WITH dimTable AS
    (SELECT COUNT(DISTINCT TileWidth), COUNT(DISTINCT TileHeight),
        COUNT(DISTINCT Dim_C), COUNT(DISTINCT Dim_T), COUNT(DISTINCT Dim_Z), COUNT(DISTINCT Dim_M)
        FROM TILESINFO WHERE PyramidLevel = 0 GROUP BY ?1)
        SELECT COUNT(*) = 1
        FROM(SELECT DISTINCT * FROM dimTable)
    */

    stringstream dimIsUniform;
    dimIsUniform << "WITH dimTable AS" <<
        " (SELECT " <<
        dimsString.str() <<
        "COUNT(DISTINCT " << docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << ")," <<
        "COUNT(DISTINCT " << docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << ")" <<
        " FROM " << docInfo->GetTableName(IDbDocInfo::TableType::TilesInfo) <<
        " WHERE " << docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << " = 0" <<
        " GROUP BY ?1)" <<
        " SELECT COUNT(*)=1" <<
        " FROM (SELECT DISTINCT * FROM dimTable)";

    bool isUniform = true;

    SQLite::Statement query(internalDb->GetDb(), dimIsUniform.str());

    // Get starting timepoint
    auto start = high_resolution_clock::now();
    
    // check if every dimension is Uniform
    for (const auto& d : tileDims)
    {
        // except dimension M
        if (d == 'M')
        {
            continue;
        }

        string dimName;
        if (docInfo->GetTileInfoColumnNameForDimension(d, dimName))
        {
            query.reset();
            query.clearBindings();

            // i.e. Dim_C
            query.bind(1, dimName);

            try
            {
                // expect only one row!
                if (query.executeStep())
                {
                    // dimension is not uniform
                    if (query.getColumn(0).getInt() == 0)
                    {
                        isUniform = false;
                        break;
                    }
                }
            }
            catch (SQLite::Exception& excp)
            {
                std::cout << excp.what();
                isUniform = false;
                break;
            }
        }
    }

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << "Perform 'IsUniform' on a database with " << this->columnCount << " x " << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles in regular grid (with indices for 'T', 'Z' and 'C') " <<
        "and C = " << this->cCount << ", Z = " << this->zCount << ", T = " << this->tCount;
    item.benchmarkName = ss.str();
    
    ss = stringstream();
    ss.imbue(std::locale(""));
    ss << "On a database (in memory) with " << this->columnCount << "x" << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles and indices for ('T','Z','C') " <<
        "and C = " << this->cCount << ", Z = " << this->zCount << ", T = " << this->tCount << ", perform 'IsUniform' and return " << isUniform;
    item.explanation = ss.str();
    item.executionTime = (stop - start);
    
    return item;
}

std::shared_ptr<SlImgDoc::IDb> TestCase4::CreateDb(bool withTransaction, bool createIndices, bool withSpatialIndex)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('M');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('C');
    opts.sizeOfDataBinHdrField = 4;
    opts.createSpatialIndex = withSpatialIndex;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    if (createIndices)
    {
        dbWrite->CreateIndexOnCoordinate('M');
        dbWrite->CreateIndexOnCoordinate('T');
        dbWrite->CreateIndexOnCoordinate('Z');
        dbWrite->CreateIndexOnCoordinate('C');
    }

    if (withTransaction)
    {
        dbWrite->BeginTransaction();
    }

    for (int color = 0; color < this->cCount; ++color)
    {
        for (int z = 0; z < this->zCount; ++z)
        {
            for (int t = 0; t < this->tCount; ++t)
            {
                for (int r = 0; r < this->rowCount; ++r)
                {
                    for (int c = 0; c < this->columnCount; ++c)
                    {
                        LogicalPositionInfo posInfo;
                        posInfo.posX = c * this->tileWidth;
                        posInfo.posY = r * this->tileHeight;
                        posInfo.width = this->tileWidth;
                        posInfo.height = this->tileHeight;
                        posInfo.pyrLvl = 0;
                        TileBaseInfo tileBaseInfo;
                        tileBaseInfo.pixelWidth = this->tileWidth;;
                        tileBaseInfo.pixelHeight = this->tileWidth;;
                        tileBaseInfo.pixelType = PixelType::GRAY8;

                        // and now we add a coordinate which has dimension 'M', 'T', 'Z' and 'C'
                        TileCoordinate tc({ { 'M', r * this->columnCount + c }, {'T', t}, {'Z', z}, {'C', color} });
                        CDataObjCustom dataCustom(4, 1);

                        dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
                    }
                }
            }
        }
    }

    if (withTransaction)
    {
        dbWrite->CommitTransaction();
    }

    return db;
}