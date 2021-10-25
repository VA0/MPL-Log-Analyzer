#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <regex>
#include <typeinfo>
#include "Line.hpp"

void ReplaceAll(std::string& str,
               const std::string& oldStr,
               const std::string& newStr)
{
  std::string::size_type pos = 0u;
  while((pos = str.find(oldStr, pos)) != std::string::npos){
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

Line::Line(long id, const char* time, const char* entity, const char* type, long thread, long trace, const char* component, const char* data)
{
    this->id = id;
    strcpy(this->timestamp, time);
    strcpy(this->entity, entity);
    strcpy(this->type, type);
    this->threadID = thread;
    this->traceID = trace;
    strcpy(this->componentName, component);
    strcpy(this->message, data);
}

//Constructor from log line
Line::Line(std::string& line)
{
    // int pos = line.find('|');
    // this->id = line.substr(0, pos);
    // line = line.erase(pos);

    //this->id = 
    int segments_count = std::count(line.begin(), line.end(), '|');

    //line = ReplaceAll(line, "\\033[31m", "");
    ReplaceAll(line, "\033[31m", "");
    ReplaceAll(line, "\033[0m", "");
    //line = ReplaceAll(line, "\\033[0m", "");
    //ReplaceAll('\\033[31m', 's'); 
    //std::replace( line.begin(), line.end(), 'a', 'b'); 
    //std::replace( line.begin(), line.end(), '\\033[0m", "");


    int pos = line.find('|');
    strcpy(this->timestamp, line.substr(0, pos).c_str());
    line = line.erase(0, pos+1);

    pos = line.find('|');
    strcpy(this->entity, line.substr(0, pos).c_str());
    line = line.erase(0, pos+1);

    pos = line.find('|');
    strcpy(this->type, line.substr(0, pos).c_str());
    line = line.erase(0, pos+1);

    pos = line.find('|');
    //this->threadID = atol(line.substr(4, pos-4).c_str()); //position 4 to skip 'THD:'
    this->threadID = std::strtoul(line.substr(4, pos-4).c_str(), nullptr, 16); //position 4 to skip 'THD:'
    line = line.erase(0, pos+1);

    if (segments_count == 6)
    {
        pos = line.find('|');
        this->traceID = std::strtoul(line.substr(0, pos).c_str(), nullptr, 16);
        line = line.erase(0, pos+1);
    }
    else
        this->traceID = -1;

    if (segments_count >= 5 )  //line.length() <= sizeof(this->componentName)
    {
        pos = line.find('|');
        strncpy(this->componentName, line.substr(0, pos).c_str(), sizeof(this->componentName));
        line = line.erase(0, pos+1);
    }
    else
        strcpy(this->componentName, "");

    pos = line.find('|');
    strncpy(this->message, line.substr(0, pos).c_str(), sizeof(this->message));
    line = line.erase(0);
}

Line::Line() : id(-1), timestamp(""), entity(""), type(""), 
               threadID(-1), traceID(-1), componentName(""), message("") {}

void Line::Print() const
{
    std::cout << this->timestamp << "|" << this->entity << "|" << this->type << "|" << std::hex << this->threadID << 
    "|" << this->traceID << std::dec << "|" << this->componentName << "|" << this->message <<
     std::endl << std::endl << std::endl;
}
void Print(const std::vector<Line>& vecData)
{
    std::for_each(vecData.begin(), vecData.end(), [](const Line& l) { l.Print(); });
}