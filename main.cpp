#include <QGuiApplication>
#include <hayai.hpp>

#include <fstream>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    std::ofstream htmlFile("/Volumes/HD2/Projects/All/Tech/Pencil2D/Benchmarks/benchmark.html");
    std::ofstream jsonFile("/Volumes/HD2/Projects/All/Tech/Pencil2D/Benchmarks/benchmark.json");
    hayai::HtmlOutputter htmlOutputter(htmlFile);
    hayai::JsonOutputter jsonOutputter(jsonFile);
    hayai::ConsoleOutputter consoleOutputter;

    hayai::Benchmarker::AddOutputter(htmlOutputter);
    hayai::Benchmarker::AddOutputter(jsonOutputter);
    hayai::Benchmarker::AddOutputter(consoleOutputter);
    hayai::Benchmarker::RunAllTests();
    return 0;
}
