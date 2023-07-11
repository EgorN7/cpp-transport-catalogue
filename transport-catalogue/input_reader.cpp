#include"input_reader.h"
#include <map>
#include <unordered_set>

void InputReader::ParseStop(TransportCatalogue::TransportCatalogue& transport_catalogue,const std::string& str)
{
    std::string stop_name;
    double lat = 0;
    double lng = 0;
    std::vector<std::pair<std::string, int>> vector_distances;


    size_t colon_position = str.find_first_of(':');
    stop_name = str.substr(0, colon_position);

    size_t p_latitude1 = str.find_first_not_of(' ', colon_position + 1);
    size_t p_latitude2 = str.find_first_of(',', p_latitude1);

    size_t p_longitude1 = str.find_first_not_of(' ', p_latitude2 + 1);
    size_t p_longitude2 = str.find_first_of(',', p_longitude1);

    lat = std::stod(str.substr(p_latitude1, p_latitude2 - p_latitude1));
    lng = std::stod(str.substr(p_longitude1, p_longitude2 - p_longitude1));
 

    size_t start_pos = p_longitude2;
    while (start_pos != -1)
    {
        size_t start_distances = str.find_first_not_of(' ', start_pos + 1);
        size_t end_distance = str.find_first_of(',', start_distances);
        vector_distances.push_back(ParseStopDistances(str.substr(start_distances, end_distance - start_distances)));
        start_pos = end_distance;
    }
 
    transport_catalogue.AddStop(stop_name, lat, lng, vector_distances);
 
}
 
std::pair<std::string, int> InputReader::ParseStopDistances(const std::string& str_distances)
{
    std::string str_dis;
    size_t m_position = str_distances.find_first_of('m');
    str_dis = str_distances.substr(0, m_position);

    int dist = stoi(str_dis);

    size_t space_pos = str_distances.find_first_not_of(' ', m_position + 4);

    std::string stop = str_distances.substr(space_pos);

    return std::make_pair(stop, dist);
}
 
 
void InputReader::ParseBus(TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& request_bus)
{
    std::string bus_name;
    std::vector<std::string> names_stops;
  
    size_t colon_position = request_bus.find_first_of(':');
    bus_name = request_bus.substr(0, colon_position);

    bool circle = true;


    size_t start_pos = colon_position + 1;

    while (start_pos != -1)
    {

        size_t start_distances = request_bus.find_first_not_of(' ', start_pos + 1);
        size_t end_distance = request_bus.find_first_of("->", start_distances);
        if (end_distance != -1 && request_bus[end_distance] == '-') 
        { circle = false; }
        names_stops.push_back(request_bus.substr(start_distances, (end_distance - start_distances - (end_distance == -1 ? 0: 1))));
        start_pos = end_distance;
    }
 
    transport_catalogue.AddBus(bus_name, names_stops, circle);
 
}
 
void InputReader::Parse(TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in)
{
    std::vector <std::string> request_buses;
 
    std::string str;
    int n = 0;
    in >> n;
    std::getline(in, str);
    for (int i = 0; i < n; i++)
    {
        getline(in, str);
        if (str[0] == 'S')
        {
            ParseStop(transport_catalogue, str.substr(5));
        }
        else if (str[0] == 'B')
        {
            request_buses.push_back(str.substr(4));
        }
 
    }
    for (const std::string& request_bus : request_buses){
        ParseBus(transport_catalogue, request_bus);
    }
}