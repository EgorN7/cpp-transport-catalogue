#include<string>
#include "stat_reader.h"


void StatReader::Output(TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in, std::ostream& out)
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
            transport_catalogue.PrintBusInfo(bus_name, out);
        }
        else if (str[0] == 'S')
        {
            std::string stop_name = str.substr(5);            
            transport_catalogue.PrintStopInfo(stop_name, out);
        }   
    }
}