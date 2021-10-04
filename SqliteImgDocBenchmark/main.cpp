#include "commandlineoptions.h"
#include "testCase1.h"
#include "testCase2.h"
#include "testCase3.h"
#include "testCase4.h"
#include "utils.h"
#include <iostream>
#if _WINDOWSENV
#include <windows.h>
#endif
#if _LINUXENV
#include <clocale>
#endif

using namespace std;
using namespace std::chrono;

static void RunTestCase1(const CommandLineOptions& cmdLineOptions);
static void RunTestCase2(const CommandLineOptions& cmdLineOptions);
static void RunTestCase3(const CommandLineOptions& cmdLineOptions);
static void RunTestCase4(const CommandLineOptions& cmdLineOptions);

int main(int argc, char** argv)
{
#if _WINDOWSENV
    SetConsoleOutputCP(CP_UTF8);
#endif
#if _LINUXENV
    setlocale(LC_CTYPE, "");
#endif
    CommandLineOptions cmdlineopts;
    cmdlineopts.ParseArguments(argc, argv);
    RunTestCase1(cmdlineopts);
    RunTestCase2(cmdlineopts);
    RunTestCase3(cmdlineopts);
    RunTestCase4(cmdlineopts);

    cout << u8"äöüß" << endl;
}

static void PrintBenchmarkItem(const BenchmarkItem& item)
{
    cout << "Benchmark: " << item.benchmarkName << endl;

    auto detailLines = Utils::wrap(Utils::convertUtf8ToWchart(item.explanation).c_str(), 78);
    cout << "Details: " << endl;
    for (const auto& l : detailLines)
    {
        cout << Utils::convertToUtf8(l) << endl;
    }

    cout << "Runtime: " << (item.executionTime.count()) << "s" << endl;
}

/*static*/void RunTestCase1(const CommandLineOptions& cmdLineOptions)
{
    TestCase1 testcase1(cmdLineOptions.GetParamsTestCase1());

    PrintBenchmarkItem(testcase1.RunTest1());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest2());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest3());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest4());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest5());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest6());
    cout << endl;

    PrintBenchmarkItem(testcase1.RunTest7());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest8());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest9());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest10());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest11());
    cout << endl;
}

/*static*/void RunTestCase2(const CommandLineOptions& cmdLineOptions)
{
    TestCase2 testcase2(cmdLineOptions.GetParamsTestCase2());

    PrintBenchmarkItem(testcase2.RunTest1());
    cout << endl;

    PrintBenchmarkItem(testcase2.RunTest2());
    cout << endl;

    PrintBenchmarkItem(testcase2.RunTest3());
    cout << endl;

    PrintBenchmarkItem(testcase2.RunTest4());
    cout << endl;

    PrintBenchmarkItem(testcase2.RunTest5());
    cout << endl;

    PrintBenchmarkItem(testcase2.RunTest6());
    cout << endl;
}

/*static*/ void RunTestCase3(const CommandLineOptions & cmdLineOptions)
{
    TestCase3 testcase3(cmdLineOptions.GetParamsTestCase3());

    PrintBenchmarkItem(testcase3.RunTest1());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTest2());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTest3());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTest4());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTest5());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTest6());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTestReal());
    cout << endl;

    PrintBenchmarkItem(testcase3.RunTest7());
    cout << endl;
}

/*static*/ void RunTestCase4(const CommandLineOptions& cmdLineOptions)
{
    TestCase4 testcase4(cmdLineOptions.GetParamsTestCase4());

    PrintBenchmarkItem(testcase4.RunTest1());
    cout << endl;
}
