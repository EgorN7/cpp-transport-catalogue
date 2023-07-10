#include "transport_catalogue.h"
#include<set>
#include<iostream>
#include<algorithm>
#include <sstream>
 
 
bool TransportCatalogue::details::Stop::operator==(const Stop& rhs) const
{
    return this->stop_name == rhs.stop_name;
}
bool TransportCatalogue::details::Stop::operator!=(const Stop& rhs) const
{
    return !(this->stop_name == rhs.stop_name);
}
 
TransportCatalogue::details::Stop::Stop() {}

TransportCatalogue::details::Stop::Stop(std::string name)
    :stop_name(name) {
}
 
bool TransportCatalogue::details::Bus::operator==(const Bus& rhs) const
{
    return bus_name == rhs.bus_name;
}
bool TransportCatalogue::details::Bus::operator!=(const Bus& rhs) const
{
    return !(bus_name == rhs.bus_name);
}
 
void TransportCatalogue::TransportCatalogue::AddStop(details::Stop  stop)
{
    stops.push_back(stop);
 
    stopname_to_stop[stops.back().stop_name] = &stops.back();
}
 
void TransportCatalogue::TransportCatalogue::AddBus(std::string bus_name, std::vector<std::string> stops, bool circle)
{
    details::Bus temp_bus;
 
    temp_bus.bus_name = bus_name;
 
    for (const auto& stop : stops){
        temp_bus.single_marshrut.push_back(stopname_to_stop.at(stop));
    }
    if (!circle) {
        for (auto it = stops.rbegin()+1; it != stops.rend(); ++it){
            temp_bus.single_marshrut.push_back(stopname_to_stop.at(*it));
        }
    }
    buses.push_back(temp_bus);
 
    busname_to_bus[buses.back().bus_name] = &buses.back();
}

TransportCatalogue::details::Stop* TransportCatalogue::TransportCatalogue::FindStop(std::string_view find_name) const
{
    return stopname_to_stop.at(find_name);
}
 
TransportCatalogue::details::Bus* TransportCatalogue::TransportCatalogue::FindBus(std::string_view find_name) const
{
    if (busname_to_bus.count(find_name)) {
        return busname_to_bus.at(find_name);
    } else {
        return nullptr;
    }
}

double TransportCatalogue::TransportCatalogue::DistanceCoord(details::Bus* bus) const
{
    double distance = 0.0;
    for (int i = 0; i < bus->single_marshrut.size() - 1; i++){
        distance += ComputeDistance(bus->single_marshrut[i]->coords,
            bus->single_marshrut[i + 1]->coords);
    }
    return distance;
}
 
std::string TransportCatalogue::TransportCatalogue::GetBusInfo(const std::string& find_bus_name) const
{
    std::string search_stop_info;
    int distance = 0;
    details::Bus* find_bus = FindBus(find_bus_name);
    if (find_bus){
        std::vector<details::Stop*> stops_vector = find_bus->single_marshrut;
 
        for (int i = 0; i < stops_vector.size() - 1; i++){
 
            std::unordered_map<std::string, int> m = stops_vector[i]->stops_distances;
            std::unordered_map<std::string, int> m2 = stops_vector[i + 1]->stops_distances;
 
            if (m.count(stops_vector[i + 1]->stop_name)){
                distance += m[stops_vector[i + 1]->stop_name];
            }
            else if (m2.count(stops_vector[i]->stop_name))
            {
                distance += m2[stops_vector[i]->stop_name];
            } else {
                distance += 0;
            }
        }
 
        std::set<details::Stop*> sw(find_bus->single_marshrut.begin(), find_bus->single_marshrut.end());
        search_stop_info = "Bus "s + find_bus->bus_name + ": "s + std::to_string(find_bus->single_marshrut.size())
            + " stops on route, "s + std::to_string(sw.size()) + " unique stops, "s + std::to_string(distance)
            + " route length, "s + std::to_string(1.0 *distance / DistanceCoord(find_bus)) +" curvature"s ;
        return search_stop_info;
    }else{
        search_stop_info = "Bus "s + std::string(find_bus_name) + ": not found"s;
        return search_stop_info;
    }
}
 
std::string TransportCatalogue::TransportCatalogue::SearchStop(const std::string& find_stop_name) const
{
    std::string search_stop_info;
    std::vector <std::string> buses_for_stop;
 
    if (stopname_to_stop.count(find_stop_name)){
        for (const auto& [bus_name, addr_bus] : busname_to_bus){
            auto it = std::find_if(addr_bus->single_marshrut.begin(), addr_bus->single_marshrut.end(),
                [find_stop_name](const details::Stop* stop){
                    return stop->stop_name == find_stop_name;
                });
            if (it != addr_bus->single_marshrut.end()) {
                buses_for_stop.push_back(std::string(bus_name));
            }
        }
 
        if (buses_for_stop.empty()){
            search_stop_info = "Stop " + find_stop_name + ": no buses";
            return search_stop_info;
        }
 
        sort(buses_for_stop.begin(), buses_for_stop.end());
 
        search_stop_info = "Stop "s + find_stop_name + ": buses"s;
        for (const auto& bus : buses_for_stop)
        {
            search_stop_info += ' ' + bus;
        }
        return search_stop_info;
    } else {
        search_stop_info = "Stop "s + find_stop_name + ": not found"s;
        return search_stop_info;
    }
}