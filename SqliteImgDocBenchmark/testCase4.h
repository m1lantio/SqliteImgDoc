#pragma once
#include "benchmarkitem.h"
#include <memory>
#include "inc_sqliteimgdoc.h"
#include "commandlineoptions.h"

class TestCase4
{
private:
    int rowCount;
    int columnCount;
    int tileWidth;
    int tileHeight;
    int tCount;
    int zCount;
    int cCount;
public:
    TestCase4(const ParametersTestCase4& params);

    BenchmarkItem RunTest1();
private:
    std::shared_ptr<SlImgDoc::IDb> CreateDb(bool withTransaction, bool createIndices, bool withSpatialIndex);
};