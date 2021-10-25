#pragma once
#include <execution>
#include <cstring>
#include <exception>
#include <regex>
#include <algorithm>
#include "Line.hpp"

namespace Logs
{
    enum Type
    {
        Timestamp = 0,
        SequenceID,
        TraceID,
        Transaction
    };
    enum SortType
    {
        Descending = 0,
        Ascending
    };

    ///Generic Search Function
    ///Supports various of attributes to search along with exact and partial match:
    ///option 0 == timestamp search (char*) (default) yyyy-MM-ddTHH:mm:ss.SSS
    ///option 1 == seqid search (long) ex. 257294537
    ///option 2 == traceID search (long)
    ///option 3 == transaction search (char*)
    ///
    ///Returns vector with the results
    template <class T1>
    bool Search(const std::vector<Line>& vecHaystack, const std::vector<T1>& vecNeedle, std::vector<Line>& vecOutput, Type option, bool exactSearch = false)
    {
        std::vector<Line> vecFound;

        for (auto needle : vecNeedle)
        {
            std::copy_if(vecHaystack.begin(), vecHaystack.end(), 
                std::back_inserter(vecFound), 
                [&](Line l) 
                { 
                    if (option == Type::SequenceID)
                    {                           
                        if (exactSearch)
                        {
                            // char phrase[30] = "seqid\": ";   //seqid": 257294537
                            // strcat(phrase, std::to_string(needle).c_str());
                            // //strcat(phrase, ",");
                            // return (strstr(l.message, phrase)) != nullptr;

                            //char phrase[30] = "seqid.{3}.*";
                            char phrase[30] = "seqid.{3}";
                            strcat(phrase, std::to_string(needle).c_str());
                            //if (exactSearch) 
                            strcat(phrase, "[,|}]");
                            std::regex r (std::string(phrase).c_str());
                            bool match = std::regex_search (std::string(l.message), r);
                            return match == true;
                        }      
                        else
                        {
                            //char phrase[30] = "seqid.{3}.*.+?";
                            char phrase[30] = "seqid.{2}.{1,10}";
                            strcat(phrase, std::to_string(needle).c_str());
                            strcat(phrase, ".+?([, ]|[}])");         
                            std::regex r (std::string(phrase).c_str());
                            bool match = std::regex_search (std::string(l.message), r);
                            return match == true;
                        }
                    }
                    else if (option == Type::TraceID)
                    {
                        if (exactSearch)
                            return l.traceID == needle || (strstr(l.message, (std::to_string(needle)).c_str() )) != nullptr;
                        else
                             throw std::exception();
                    }
                        
                    //"trcid": 5421791668057426814
                    else
                       throw std::exception();

                });
        }

        vecOutput.assign(vecFound.begin(), vecFound.end());
        return (vecFound.size() > 0) ? true : false;
    }


     ///Generic Search Function
    ///Supports various of attributes to search along with exact and partial match:
    ///option 0 == timestamp search (char*) (default) yyyy-MM-ddTHH:mm:ss.SSS
    ///option 1 == seqid search (long) ex. 257294537
    ///option 2 == traceID search (long)
    ///option 3 == transaction search (char*)
    ///
    ///Returns vector with the results
    template <> inline
    bool Search<const char*>(const std::vector<Line>& vecHaystack, const std::vector<const char*>& vecNeedle, std::vector<Line>& vecOutput, Type option, bool exactSearch)
    {
        std::vector<Line> vecFound;

        for (auto needle : vecNeedle)
        {
            std::copy_if(vecHaystack.begin(), vecHaystack.end(), 
                std::back_inserter(vecFound), 
                [&](Line l) 
                { 
                    if (option == Type::Timestamp)
                    {
                        if (exactSearch) 
                            return l.timestamp == (char*)needle; 
                        else
                        {
                            return (strstr(l.timestamp, (char*)needle)) != nullptr;
                        }
                    }             

                    else
                       throw std::exception();

                });
        }

        vecOutput.assign(vecFound.begin(), vecFound.end());
        return (vecFound.size() > 0) ? true : false;
    }

    inline long UnixTimeStamp(const char* datetime)
    {
        char chDatetime[19]; 
        char chMiliseconds[3];
        strncpy(chDatetime, datetime, 19);
        strncpy(chMiliseconds, datetime+20, 3);
        int miliseconds = atoi(chMiliseconds);

            std::tm tm = {};
        //strptime("Thu Jan 9 2014 12:35:34", "%a %b %d %Y %H:%M:%S", &tm);
        strptime(chDatetime, "%Y-%m-%dT%H:%M:%S", &tm);
        //auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        

        // time_t rawtime;
        // struct tm * timeinfo;

        // int year=2013, month=5, day=7, hour = 5, min = 1, sec = 0;

        // /* get current timeinfo: */
        // time ( &rawtime ); //or: rawtime = time(0);
        // /* convert to struct: */
        // timeinfo = localtime ( &rawtime ); 

        // /* now modify the timeinfo to the given date: */
        // timeinfo->tm_year   = year - 1900;
        // timeinfo->tm_mon    = month - 1;    //months since January - [0,11]
        // timeinfo->tm_mday   = day;          //day of the month - [1,31] 
        // timeinfo->tm_hour   = hour;         //hours since midnight - [0,23]
        // timeinfo->tm_min    = min;          //minutes after the hour - [0,59]
        // timeinfo->tm_sec    = sec;          //seconds after the minute - [0,59]

        /* call mktime: create unix time stamp from timeinfo struct */
        auto date = timegm ( &tm );
        date *= 1000;
        date += miliseconds;

        //printf ("Until the given date, since 1970/01/01 %li seconds have passed.\n", date);
        //printf ("UnixTimeStamp %li\n", date);
        return date;
    }

    inline bool Sort(std::vector<Line>& vecData, Type option, bool parallel = false, SortType sorttype = SortType::Ascending)
    {
        if (option == Type::Timestamp)
        {
            //std::execution::par doesnt work on custom types?
            std::sort(std::execution::par, vecData.begin(), vecData.end(), [&parallel, &sorttype](const Line& l1, const Line& l2)
            {
                auto t1 = UnixTimeStamp(l1.timestamp);
                auto t2 = UnixTimeStamp(l2.timestamp);
                return (sorttype == SortType::Ascending) ? (t1 < t2) : (t1 > t2);
            });
            return true;
        }
        else if (option == Type::TraceID)
        {
            std::sort(std::execution::seq, vecData.begin(), vecData.end(), [&parallel, &sorttype](const Line& l1, const Line& l2)
            {
                return (sorttype == SortType::Ascending) ? (l1.traceID < l2.traceID) : (l1.traceID > l2.traceID);
            });

            return true;
        }
        else
        {
            return false;
        }
    }
}