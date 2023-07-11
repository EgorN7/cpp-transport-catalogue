#include "transport_catalogue.h"
#include<set>
#include<iostream>
#include<algorithm>
#include <sstream>

 
 
bool TransportCatalogue::details::Stop::operator==(const Stop& rhs) const
{
    return stop_name == rhs.stop_name;
}
bool TransportCatalogue::details::Stop::operator!=(const Stop& rhs) const
{
    return !(stop_name == rhs.stop_name);
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


size_t TransportCatalogue::details::StopsHasher::operator()(const std::pair<Stop*, Stop*>& stop_to_stop) const
{
    size_t hash_fist_stop = hasher_(stop_to_stop.first->stop_name);
    size_t hash_second_stop = hasher_(stop_to_stop.second->stop_name);
    return hash_fist_stop + hash_second_stop * 17;
}
 
void TransportCatalogue::TransportCatalogue::AddStop(const std::string& name, const Coordinates& coordinates)
{
    details::Stop temp_stop;
    temp_stop.stop_name = name;
    temp_stop.coords = coordinates;
    stops_.push_back(temp_stop);
    stopname_to_stop_[stops_.back().stop_name] = &stops_.back();
}
 
void TransportCatalogue::TransportCatalogue::AddBus(const std::string& bus_name, const std::vector<std::string>& stops, bool circle)
{
    details::Bus temp_bus;
 
    temp_bus.bus_name = bus_name;
 
    for (const auto& stop : stops){
        temp_bus.single_marshrut.push_back(stopname_to_stop_.at(stop));
    }
    if (!circle) {
        for (auto it = stops.rbegin()+1; it != stops.rend(); ++it){
            temp_bus.single_marshrut.push_back(stopname_to_stop_.at(*it));
        }
    }
    buses_.push_back(temp_bus);
 
    busname_to_bus_[buses_.back().bus_name] = &buses_.back();
}

void TransportCatalogue::TransportCatalogue::AddDistanceBetweenStops(const std::string& first_stop, const std::string& second_stop, int distance_between_stops) {
    distance_[std::make_pair(FindStop(first_stop), FindStop(second_stop))] = distance_between_stops;
}

TransportCatalogue::details::Stop* TransportCatalogue::TransportCatalogue::FindStop(std::string_view find_name) const
{
    return stopname_to_stop_.at(find_name);
}
 
TransportCatalogue::details::Bus* TransportCatalogue::TransportCatalogue::FindBus(std::string_view find_name) const
{
    if (busname_to_bus_.count(find_name)) {
        return busname_to_bus_.at(find_name);
    } else {
        return nullptr;
    }
}

int TransportCatalogue::TransportCatalogue::GetDistanceBetweenTwoStops(details::Stop* first_stop, details::Stop* second_stop) const 
{
    int distance = 0;
    std::pair<details::Stop*, details::Stop*> stop_to_next_stop = std::make_pair(first_stop, second_stop);
    std::pair<details::Stop*, details::Stop*> next_stop_to_stop = std::make_pair(second_stop, first_stop);
    if (distance_.count(stop_to_next_stop)) {
        distance += distance_.at(stop_to_next_stop);
    }
    else if (distance_.count(next_stop_to_stop))
    {
        distance += distance_.at(next_stop_to_stop);
    }
    else {
        distance += 0;
    }
    return distance;
}

TransportCatalogue::details::BusRouteDistance TransportCatalogue::TransportCatalogue::GetBusRouteDistance(details::Bus* bus) const
{
    int real_distance = 0;
    double coordinates_distance = 0.0;
    for (int i = 0; i < bus->single_marshrut.size() - 1; i++) {
        real_distance += GetDistanceBetweenTwoStops(bus->single_marshrut[i], bus->single_marshrut[i + 1]);
        coordinates_distance += ComputeDistance(bus->single_marshrut[i]->coords,
            bus->single_marshrut[i + 1]->coords);
    }
    return { real_distance , coordinates_distance };
}

TransportCatalogue::details::BusInfo TransportCatalogue::TransportCatalogue::GetBusInfo(const std::string& find_bus_name) const
{
    details::BusInfo bus_info;
    bus_info.bus_name = find_bus_name;
    details::Bus* find_bus = FindBus(find_bus_name);
    if (find_bus){
        bus_info.in_cataloge = true;
        
        details::BusRouteDistance bus_route_distance = GetBusRouteDistance(find_bus);
        std::set<details::Stop*> unique_stops(find_bus->single_marshrut.begin(), find_bus->single_marshrut.end());

        bus_info.stops_on_route = find_bus->single_marshrut.size();
        bus_info.unique_stops = unique_stops.size();
        bus_info.route_length = bus_route_distance.real_distance;
        bus_info.curvature = 1.0 * bus_route_distance.real_distance / bus_route_distance.coordinates_distance;
        return bus_info;
    }else{
        return bus_info;
    }
}
 
TransportCatalogue::details::StopInfo TransportCatalogue::TransportCatalogue::SearchStop(const std::string& find_stop_name) const
{
    details::StopInfo temp_stop;
    temp_stop.stop_name = find_stop_name;
    std::vector <std::string> buses_for_stop;
 
    if (stopname_to_stop_.count(find_stop_name)){
        temp_stop.in_cataloge = true;
        for (const auto& [bus_name, addr_bus] : busname_to_bus_){
            auto it = std::find_if(addr_bus->single_marshrut.begin(), addr_bus->single_marshrut.end(),
                [find_stop_name](const details::Stop* stop){
                    return stop->stop_name == find_stop_name;
                });
            if (it != addr_bus->single_marshrut.end()) {
                temp_stop.buses.push_back(std::string(bus_name));
            }
        }
        if (temp_stop.buses.empty()){ return temp_stop;}
        sort(temp_stop.buses.begin(), temp_stop.buses.end());
        return temp_stop;
    } else {
        return temp_stop;
    }
}