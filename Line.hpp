#pragma once

constexpr const u_int AVERAGE_LINE_LENGTH = 633;

class Line
{
public:
    long id;
    char timestamp[24];
    char entity[30];
    char type[5];
    long threadID;
    long traceID;
    char componentName[50];
    char message[700];
public:
    Line(long, const char*, const char*, const char*, long, long, const char*, const char*);
    Line(std::string&);
    Line();
    void Print() const;
};

void Print(const std::vector<Line>& vecData);
std::string ReplaceAll(std::string const & in, std::string const & from, std::string const & to );

#include "Line.tpp"