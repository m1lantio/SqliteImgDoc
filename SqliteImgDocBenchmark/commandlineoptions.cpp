#include "commandlineoptions.h"
#include <cxxopts.hpp>

using namespace std;

CommandLineOptions::CommandLineOptions()
{
    this->SetDefaults();
}

void CommandLineOptions::SetDefaults()
{
    this->paramsTestCase1.zCount = 1000;
    this->paramsTestCase1.tCount = 1000;

    this->paramsTestCase2.rowCount = 600;
    this->paramsTestCase2.columnCount = 600;
    this->paramsTestCase2.tileWidth = 1024;
    this->paramsTestCase2.tileHeight = 1024;

    this->paramsTestCase3.rowCount = 100;
    this->paramsTestCase3.columnCount = 100;
    this->paramsTestCase3.tileWidth = 1024;
    this->paramsTestCase3.tileHeight = 1024;
    this->paramsTestCase3.tCount = 20;

    this->paramsTestCase4.rowCount = 100;
    this->paramsTestCase4.columnCount = 100;
    this->paramsTestCase4.tileWidth = 1024;
    this->paramsTestCase4.tileHeight = 1024;
    this->paramsTestCase4.tCount = 20;
    this->paramsTestCase4.zCount = 20;
    this->paramsTestCase4.cCount = 20;
}

bool CommandLineOptions::ParseArguments(int argc, char** argv)
{
    return true;
}

const ParametersTestCase1& CommandLineOptions::GetParamsTestCase1() const
{
    return this->paramsTestCase1;
}

const ParametersTestCase2& CommandLineOptions::GetParamsTestCase2() const
{
    return this->paramsTestCase2;
}

const ParametersTestCase3& CommandLineOptions::GetParamsTestCase3() const
{
    return this->paramsTestCase3;
}

const ParametersTestCase4& CommandLineOptions::GetParamsTestCase4() const
{
    return this->paramsTestCase4;
}