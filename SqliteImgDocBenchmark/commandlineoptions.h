#pragma once
#include <string>

class ParametersTestCase1
{
public:
    int zCount;
    int tCount;
};

class ParametersTestCase2
{
public:
    int rowCount;
    int columnCount;
    int tileWidth;
    int tileHeight;
};

class ParametersTestCase3
{
public:
    int rowCount;
    int columnCount;
    int tileWidth;
    int tileHeight;
    int tCount;
};

class ParametersTestCase4
{
public:
    int rowCount;
    int columnCount;
    int tileWidth;
    int tileHeight;
    int tCount;
    int zCount;
    int cCount;
};

class CommandLineOptions
{
private:
    std::string testDataFolder;
    ParametersTestCase1 paramsTestCase1;
    ParametersTestCase2 paramsTestCase2;
    ParametersTestCase3 paramsTestCase3;
    ParametersTestCase4 paramsTestCase4;
public:
    CommandLineOptions();

    bool ParseArguments(int argc, char** argv);

    const ParametersTestCase1& GetParamsTestCase1() const;
    const ParametersTestCase2& GetParamsTestCase2() const;
    const ParametersTestCase3& GetParamsTestCase3() const;
    const ParametersTestCase4& GetParamsTestCase4() const;
private:
    void SetDefaults();
};