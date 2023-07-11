#include<string>
#include "stat_reader.h"

using namespace std::literals;

void StatReader::Output(const TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in, std::ostream& out)
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
            PrintBusInfo(transport_catalogue, bus_name, out);
        }
        else if (str[0] == 'S')
        {
            std::string stop_name = str.substr(5);            
            PrintStopInfo(transport_catalogue, stop_name, out);
        }   
    }
}

void StatReader::PrintStopInfo(const TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& find_stop_name, std::ostream& out) {
    out << transport_catalogue.SearchStop(find_stop_name) << std::endl;

}
void StatReader::PrintBusInfo(const TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& find_bus_name, std::ostream& out) {
    out << transport_catalogue.GetBusInfo(find_bus_name) << std::endl;
}

std::ostream& operator<<(std::ostream& out, const TransportCatalogue::details::StopInfo& stop_info)
{
    out << "Stop "s << stop_info.stop_name;
    if (!stop_info.in_cataloge) {
        out << ": not found"s;
        return out;
    }
    if (stop_info.buses.empty()) {
        out << ": no buses"s;
        return out;
    }
    out << ": buses"s;
    for (const auto& bus : stop_info.buses) {
        out << ' ' << bus;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const TransportCatalogue::details::BusInfo& bus_info)
{
    out << "Bus "s << bus_info.bus_name;
    if (!bus_info.in_cataloge) {
        out << ": not found"s;
        return out;
    }
    out << ": "s << bus_info.stops_on_route << " stops on route, "s
        << bus_info.unique_stops << " unique stops, "s
        << bus_info.route_length << " route length, "s
        << bus_info.curvature << " curvature"s;
    return out;
}