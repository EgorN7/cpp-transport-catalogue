#include<string>
#include "stat_reader.h"

void StatReader::Output(TransportCatalogue::TransportCatalogue& TT, std::istream& in)
{
    std::string str;
    int count_requests = 0;
    in >> count_requests;
 
    std::getline(in, str);
 
    for (int i = 0; i < count_requests; i++)
    {
        std::getline(in, str);
 
        if (str[0] == 'B')
        {
            std::string bus_name = str.substr(4);
            std::cout << TT.GetBusInfo(bus_name) << std::endl;
        }
        else if (str[0] == 'S')
        {
            std::string bus_name = str.substr(5);
            std::cout << TT.SearchStop(bus_name) << std::endl;
        }   
    }
}