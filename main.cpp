#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "Line.hpp"
#include "Line.tpp"

using std::endl, std::cout;
using std::hex, std::dec;
using std::vector;
//using Logs::SortType;
//using Logs::Type;
using namespace Logs;


long counter = 0;

template <class T>
T Sum(T t)
{
    return t;
}

template <class T, class ...Args>
T Sum(T t, Args... args)
{
    return t+Sum(args...);
}

int main()
{
    // vector<Line> vecLines;
    // vecLines.push_back(Line(counter++, "2021-09-23T11:43:27.187", "mpl", "ERROR", 0x7fc90958b600, 0x7f0ec07ead47a0b3, 
    // "ClientConnectionGuard", "Client forcefully disconnected ('10.10.0.1', '44454')"));

    // vecLines.push_back(Line(counter++, "2021-09-23T11:43:27.187", "mpl", "ERROR", 0x7fc90958b600, 0x7f0ec07ead47a0b3, 
    // "ClientConnectionGuard", "Client forcefully disconnected ('10.10.0.1', '44454')"));

    // std::string t = "2021-09-23T20:54:54.786|mpl            |ERROR |THD:7f89602db600|32104bd69d03754|ClientConnectionGuard|Client disconnected ('10.10.0.1', '54466')";
    // Line l(t);

    // cout << "l.timestamp: " << l.timestamp << endl;
    // cout << "l.entity: " << l.entity << endl;
    // cout << "l.type: " << l.type << endl;
    // cout << "l.threadID: " << hex << l.threadID << endl;
    // cout << "l.traceID: " << hex << l.traceID << dec << endl;
    // cout << "l.componentName: " << l.componentName << endl;
    // cout << "l.message: " << l.message << endl;


    //MEASURE TIME
    auto start = std::chrono::high_resolution_clock::now();

    //const char* FILE_NAME="/home/arch/Desktop/MPL LOG.txt";
    const char* FILE_NAME="/home/arch/Desktop/mpl.log";

    std::fstream file(FILE_NAME);
    cout << "Is File Opened: " << ((file.is_open() == true) ? "True" : "False") << endl;


    const auto file_size = std::filesystem::file_size(FILE_NAME);
    const auto line_cout = file_size / AVERAGE_LINE_LENGTH * 1.2;
    std::vector<Line> vecData;
    vecData.reserve(line_cout);
    if (file.is_open())
    {

        // file.seekg(0, std::ios::end);
        // int len = file.tellg();
        // file.seekg(0, std::ios::beg);
        // //char data[100000];
        // data = new char[len];
        // file.read(data, len);

        std::string line;
        std::string temp;
    try{
        while (!file.eof())  //&& (file.rdstate() ^ file.goodbit) == 0
        {   
            std::getline(file, temp);  
            if (temp.starts_with("2") && line.size() > 0)
            {
                vecData.push_back(line);
                line = "";
            }   
            line += temp;
        }
        if (line.size() > 0)
            vecData.push_back(line);
        
        }
        catch (...)
        {   
            std::cout << line << std::endl;
        }

    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto uDuration = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
    auto msDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    cout << "Time: " << uDuration.count() << " (" << msDuration.count() << " miliseconds)" << endl;
    cout << "Lines: " << vecData.size() << endl << endl<< endl;

    std::vector<Line> vecOutput;
    //bool data1 = Logs::Search<long>(vecData, {850}, vecOutput, Logs::Type::SequenceID, false);
    //bool data2 = Logs::Search<long>(vecOutput, {2879657948966281140}, vecOutput, Logs::Type::TraceID, true);
    Logs::Search<const char*>(vecData, {"08:30:"}, vecOutput, Type::Timestamp, false);
    //bool data4 = Logs::Sort(vecOutput, Logs::Type::Timestamp, false, false);
    Logs::Sort(vecOutput, Logs::Type::TraceID, false, SortType::Ascending);
    
    Print(vecOutput);

    //cout << Sum(2.5, 3.5, 1.5) << endl;
    //cout << UnixTimeStamp("2021") << endl;

    return 0;
}