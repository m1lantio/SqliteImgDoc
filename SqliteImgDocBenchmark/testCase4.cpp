#include "testCase4.h"
#include "inc_sqliteimgdoc.h"
#include <chrono>
#include <sstream>
#include <random>

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
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(true, false, false);

    db->GetWriter()->CreateIndexOnCoordinate('M');
    db->GetWriter()->CreateIndexOnCoordinate('T');
    db->GetWriter()->CreateIndexOnCoordinate('Z');
    db->GetWriter()->CreateIndexOnCoordinate('C');

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << "add " << this->columnCount << " x " << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles in regular grid (w/ transaction, indices for 'M', 'T', 'Z' and 'C' are created after tiles addition)";
    item.benchmarkName = ss.str();
    ss = stringstream();
    ss.imbue(std::locale(""));
    ss << "Create a database (in memory) with " << this->columnCount << "x" << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles. All indices are created after tiles addition. " <<
        "The information is added within a single transaction.";
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