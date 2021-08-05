// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../SqliteImgDoc/external/Interface.h"
#include <random>
#include <chrono>
#include "inc_libczi.h"
#include "pugixml.hpp"

using namespace SlImgDoc;
using namespace std;
using namespace std::chrono;

class CSimpleCoord : public ITileCoordinate
{
public:
    int c, z, t, m;

public:
    virtual bool TryGetCoordinate(char dim, int* coordVal) const
    {
        int v;
        switch (tolower(dim))
        {
        case 'c':v = this->c; break;
        case 'z':v = this->z; break;
        case 't':v = this->t; break;
        case 'm':v = this->m; break;
        default:return false;
        }

        if (coordVal != nullptr)
        {
            *coordVal = v;
        }

        return true;
    }

    virtual void EnumCoordinates(std::function<bool(TileDim, int)> f) const
    {
        char d[4] = { 'C','Z','T','M' };
        for (int i = 0; i < sizeof(d) / sizeof(d[0]); ++i)
        {
            int v;
            this->TryGetCoordinate(d[i], &v);
            if (f(d[i], v) == false)
            {
                break;
            }
        }
    }
};

class CSimpleDataObjUncmp : public IDataObjUncompressedBitmap
{
private:
    BinHdrUncompressedBitmap hdr;
    uint8_t* data;
    size_t size;
public:
    CSimpleDataObjUncmp(int w, int h)
    {
        this->size = w * h;
        this->data = (uint8_t*)malloc(this->size);
        this->hdr.width = w;
        this->hdr.height = h;
        //this->hdr.pixeltype = PixelType::RGB24;
        this->hdr.stride = w;

        // populate with dummy data 0,1,2,3...size-1
        for (int i = 0; i < this->size; ++i)
        {
            this->data[i] = (uint8_t)i;
        }
    }
    virtual void GetData(const void** p, size_t* s) const
    {
        if (p != nullptr) { *p = this->data; }
        if (s != nullptr) { *s = this->size; }
    }
    virtual void GetHeader(const void** p, size_t* s) const
    {
        if (p != nullptr) { *p = &this->hdr; }
        if (s != nullptr) { *s = sizeof(this->hdr); }
    }
    virtual const BinHdrUncompressedBitmap& GetBinHdr() const
    {
        return hdr;
    }
};

static void WriteMosaic(IDbWrite* dbw, int rows, int columns, int sizeX, int sizeY)
{
    CSimpleCoord simpleCoord = {};
    LogicalPositionInfo posInfo;
    posInfo.posX = posInfo.posY = 0;
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.pyrLvl = 0;
    CSimpleDataObjUncmp dataObj(sizeX, sizeY);

    TileBaseInfo baseInfo;
    baseInfo.pixelWidth = sizeX;
    baseInfo.pixelHeight = sizeY;
    baseInfo.pixelType = PixelType::BGR24;

    // create a doc with C=1, Z=1, T=10
    // for each T, M = 0...rows*columns-1
    // Total W = columns*sizeX
    // Total H = rows*sizeY
    // Total number of tiles = rows*columns*10

    dbw->BeginTransaction();
    for (int i = 0; i < 10; ++i)
    {
        simpleCoord.t = i;
        simpleCoord.m = 0;
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < columns; ++c)
            {
                posInfo.posX = c * sizeX;
                posInfo.posY = r * sizeY;

                dbw->AddTile(&simpleCoord, &posInfo, &baseInfo, DataTypes::UNCOMPRESSED_BITMAP, &dataObj);
                simpleCoord.m++;
            }
        }
    }

    dbw->CommitTransaction();
}

void TestRead()
{
    OpenOptions opts;
    opts.dbFilename = "C:\\_SQLITE_DBs\\test.db";
    auto readDb = IDbFactory::OpenExisting(opts);
    auto read = readDb->GetReader();// IDbFactory::OpenExisting(opts);

    TileCoordinate tileCoord;
    LogicalPositionInfo posInfo;
    read->ReadTileInfo(15, &tileCoord, &posInfo);

    TilePixelInfo tilePixelInfo;
    BlobOnHeap data;
    read->ReadTileData(15, &tilePixelInfo, &data);

    RectangleD rect;
    rect.x = 100; rect.y = 100; rect.w = 20; rect.h = 1100;
    auto r = read->GetTilesIntersectingRect(rect);

    LineThruTwoPointsD line;
    line.a.x = 0; line.a.y = 0;
    line.b.x = 499; line.b.y = 500;
    r = read->GetTilesIntersectingWithLine(line);
}

void TestCreateAndWrite()
{
    CreateOptions opts;
    opts.dbFilename = "C:\\_SQLITE_DBs\\test.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    auto db = IDbFactory::CreateNew(opts);
    auto dbw = db->GetWriter();
    WriteMosaic(dbw.get(), 50, 50, 256, 256);
}

void TestRead2()
{
    OpenOptions opts;
    opts.dbFilename = "C:\\_SQLITE_DBs\\test.db";
    {
        auto readDb = IDbFactory::OpenExisting(opts);
        auto read = readDb->GetReader();

        CDimCoordinateQueryClause queryClause;
        queryClause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ 0,2 });
        queryClause.AddListClause('M', IDimCoordinateQueryClause::ListClause{ {1,2,3} });
        read->Query(&queryClause);
    }

    return;
}

static void WriteMosaicAndPerTileData(IDbWrite* dbw, int rows, int columns, int sizeX, int sizeY)
{
    CSimpleCoord simpleCoord = {};
    LogicalPositionInfo posInfo;
    posInfo.posX = posInfo.posY = 0;
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.pyrLvl = 0;
    CSimpleDataObjUncmp dataObj(sizeX, sizeY);

    TileBaseInfo baseInfo;
    baseInfo.pixelWidth = sizeX;
    baseInfo.pixelHeight = sizeY;
    baseInfo.pixelType = PixelType::BGR24;

    // create a doc with C=1, Z=1, T=10
    // for each T, M = 0...rows*columns-1
    // Total W = columns*sizeX
    // Total H = rows*sizeY
    // Total number of tiles = rows*columns*10

    dbw->BeginTransaction();
    for (int i = 0; i < 10; ++i)
    {
        simpleCoord.t = i;
        simpleCoord.m = 0;
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < columns; ++c)
            {
                posInfo.posX = c * sizeX;
                posInfo.posY = r * sizeY;

                auto rowid = dbw->AddTile(&simpleCoord, &posInfo, &baseInfo, DataTypes::UNCOMPRESSED_BITMAP, &dataObj);

                dbw->AddPerTileData(
                    rowid,
                    [r, c](int no, SlImgDoc::KeyVariadicValuePair& kv)->bool
                    {
                        switch (no)
                        {
                        case 0:
                            kv.Data.DataType = "FLOAT";
                            kv.Name = "AcquisitionTime";
                            kv.Data.doubleValue = 42 + r;
                            return true;
                        case 1:
                            kv.Data.DataType = "FLOAT";
                            kv.Name = "FocusPosition";
                            kv.Data.doubleValue = 43 + c;
                            return true;
                        default:
                            return false;
                        }
                    });

                simpleCoord.m++;
            }
        }
    }

    dbw->CommitTransaction();
}

void TestCoordinateData1()
{
    CreateOptions opts;
    opts.dbFilename = "C:\\_SQLITE_DBs\\test_coord.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "AcquisitionTime","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "FocusPosition","FLOAT" });

    auto db = IDbFactory::CreateNew(opts);
    auto dbw = db->GetWriter();

    WriteMosaicAndPerTileData(dbw.get(), 10, 10, 1024, 1024);

    /* TileCoordinate tc{ { 'T',1 }, { 'C',2 } };
     dbw->AddCoordinateData(&tc,
         [](int no, SlImgDoc::CoordinateData& cd)->bool
     {
         switch (no)
         {
         case 0:
             cd.DataType = "FLOAT";
             cd.Name = "AcquisitionTime";
             cd.doubleValue = 42;
             return true;
         case 1:
             cd.DataType = "FLOAT";
             cd.Name = "FocusPosition";
             cd.doubleValue = 43;
             return true;
         default:
             return false;
         }
     });*/
}

static void TestRead3()
{
    OpenOptions opts;
    opts.dbFilename = "C:\\_SQLITE_DBs\\test.db";
    //opts.readOnly = false;

    auto readDb = IDbFactory::OpenExisting(opts);
    auto read = readDb->GetReader();

    CDimCoordinateQueryClause queryClause;
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ 0,0 });
    queryClause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ 0,0 });
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 0,0 });
    // queryClause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0,0 });

    TileInfoQueryClause tileInfoQuery(ConditionalOperator::Equal, 5);
    auto r = read->Query(&queryClause, &tileInfoQuery);

    bool isSindexed = read->IsDimensionIndexIndexed('S');

    auto dimBounds = read->QueryDimensionBounds();

    read.reset();

    auto writeDb = readDb->GetWriter();
    if (isSindexed)
    {
        writeDb->DropIndexOnCoordinate('S');
    }
    else
    {
        writeDb->CreateIndexOnCoordinate('S');
    }
}

static std::vector<SlImgDoc::RectangleD> GenerateRandomQueryRects(int count, double width, double height, double minX, double maxX, double minY, double maxY)
{
    default_random_engine generator;

    uniform_real_distribution<double> distPosX(minX, maxX - width);
    uniform_real_distribution<double> distPosY(minY, maxY - height);

    vector<RectangleD> result;
    result.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        result.emplace_back(RectangleD{ distPosX(generator), distPosY(generator), width, height });
    }

    return result;
}

static bool WriteRectanglesToXML(const std::vector<SlImgDoc::RectangleD>& rectangles, string path)
{
    pugi::xml_document doc;

    // Generate XML declaration
    auto declarationNode = doc.append_child(pugi::node_declaration);
    declarationNode.append_attribute(L"version") = L"1.0";
    declarationNode.append_attribute(L"encoding") = L"ISO-8859-1";
    declarationNode.append_attribute(L"standalone") = L"yes";
    
    // A valid XML doc must contain a single root node of any name
    auto root = doc.append_child(L"rectangles");
    root.append_attribute(L"count") = rectangles.size();

    for (const SlImgDoc::RectangleD& rect : rectangles)
    {
        auto nodeChild = root.append_child(L"rect");
        nodeChild.append_attribute(L"x") = rect.x;
        nodeChild.append_attribute(L"y") = rect.y;
        nodeChild.append_attribute(L"w") = rect.w;
        nodeChild.append_attribute(L"h") = rect.h;
    }

    // Save XML tree to file.
    // Remark: second optional param is indent string to be used;
    // default indentation is tab character.
    return doc.save_file(path.c_str());
}

static bool WriteResultsToXML(const std::vector<SlImgDoc::dbIndex>& results, string path)
{
    pugi::xml_document doc;

    // Generate XML declaration
    auto declarationNode = doc.append_child(pugi::node_declaration);
    declarationNode.append_attribute(L"version") = L"1.0";
    declarationNode.append_attribute(L"encoding") = L"ISO-8859-1";
    declarationNode.append_attribute(L"standalone") = L"yes";

    // A valid XML doc must contain a single root node of any name
    auto root = doc.append_child(L"results");
    root.append_attribute(L"count") = results.size();

    for (const SlImgDoc::dbIndex& ind : results)
    {
        auto child = root.append_child(L"index");
        child.append_attribute(L"i") = ind;
    }

    // Save XML tree to file.
    // Remark: second optional param is indent string to be used;
    // default indentation is tab character.
    return doc.save_file(path.c_str());
}

static void TestRead4(double rectScale)
{
    int tW = 2056;
    int tH = 2464;
    int numC = 14;

    double minX = 0.0;
    double maxX = 103892.0;
    double minY = 0.0;
    double maxY = 64580.0;

    OpenOptions opts;
    opts.dbFilename = "C:\\_SQLITE_DBs\\Tauri_13N2.db";

    auto readDb = IDbFactory::OpenExisting(opts);
    auto read = readDb->GetReader();

    // pyramid level 0
    TileInfoQueryClause tileInfoQuery(ConditionalOperator::Equal, 0);

    /*
    SELECT min(TilePosX), max(TilePosX), min(TIlePosY), max(TilePosY)
    FROM TILESINFO
    WHERE PyramidLevel = 0;
    */
    auto queryRectangles = GenerateRandomQueryRects(10000, tW * rectScale, tH * rectScale, minX, maxX, minY, maxY);

    bool ret = WriteRectanglesToXML(queryRectangles, "C:\\_SQLITE_DBs\\rectangles.xml");

    uniform_int_distribution<int> cDistribution(0, numC - 1);
    default_random_engine generator;

    std::vector<dbIndex> results;
    
    // Get starting timepoint
    auto start = high_resolution_clock::now();
    
    for (auto& queryRect : queryRectangles)
    {
        CDimCoordinateQueryClause queryClause;
        const int c = cDistribution(generator);
        queryClause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ c, c });
        auto result = read->GetTilesIntersectingRect(queryRect, &queryClause, &tileInfoQuery);
        results.insert(results.end(), result.begin(), result.end());
    }

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    WriteResultsToXML(results, "C:\\_SQLITE_DBs\\resultsCP0.xml");
    cout << results.size() << endl;
    results.clear();

    duration<double> runTime = stop - start;
    cout << "10.000 RectQueries + 1 random C + only PyrLevel = 0 --- Runtime: " << runTime.count() << "s" << endl;

    // Get starting timepoint
    start = high_resolution_clock::now();

    for (auto& queryRect : queryRectangles)
    {
        CDimCoordinateQueryClause queryClause;
        const int c = cDistribution(generator);
        queryClause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ c, c });
        auto result = read->GetTilesIntersectingRect(queryRect, &queryClause, nullptr);
        results.insert(results.end(), result.begin(), result.end());
    }

    // Get ending timepoint
    stop = high_resolution_clock::now();

    WriteResultsToXML(results, "C:\\_SQLITE_DBs\\resultsC.xml");
    cout << results.size() << endl;
    results.clear();

    runTime = stop - start;
    cout << "10.000 RectQueries + 1 random C + All PyrLevels --- Runtime: " << runTime.count() << "s" << endl;

    // Get starting timepoint
    start = high_resolution_clock::now();

    for (auto& queryRect : queryRectangles)
    {
        auto result = read->GetTilesIntersectingRect(queryRect);
        results.insert(results.end(), result.begin(), result.end());
    }

    // Get ending timepoint
    stop = high_resolution_clock::now();

    WriteResultsToXML(results, "C:\\_SQLITE_DBs\\results.xml");
    cout << results.size() << endl;
    results.clear();

    runTime = stop - start;
    cout << "10.000 RectQueries + All Channels + All PyrLevels --- Runtime: " << runTime.count() << "s" << endl;
}

int main()
{
    //TestRead();
    //TestRead2();
    //TestCreateAndWrite();
    //TestCoordinateData1();
    //TestRead3();
    //TestRead4(1.5);
    TestRead4(3.0);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
