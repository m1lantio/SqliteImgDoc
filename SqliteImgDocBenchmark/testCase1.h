#pragma once
#include "benchmarkitem.h"
#include <memory>
#include "inc_sqliteimgdoc.h"
#include "commandlineoptions.h"

class TestCase1
{
private:
    int zCount;
    int tCount;
public:
    TestCase1(const ParametersTestCase1& params);
    TestCase1(int zCount, int tCount);

    BenchmarkItem RunTest1();
    BenchmarkItem RunTest2();
    BenchmarkItem RunTest3();
    BenchmarkItem RunTest4();

    BenchmarkItem RunTest5();
    BenchmarkItem RunTest6();

    BenchmarkItem RunTest7();
    BenchmarkItem RunTest8();
    BenchmarkItem RunTest9();
    BenchmarkItem RunTest10();
    BenchmarkItem RunTest11();
private:
    std::shared_ptr<SlImgDoc::IDb> CreateDb(bool withTransaction,bool createIndices, bool withSpatialIndex);
    std::vector<SlImgDoc::CDimCoordinateQueryClause> GenerateRandomSingeCoordinateQueryClauses(int count);
};