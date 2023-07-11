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


size_t TransportCatalogue::details::StopsHasher::operator()(const std::pair<std::string, std::string>& stop_to_stop) const
{
    size_t hash_fist_stop = hasher_(stop_to_stop.first);
    size_t hash_second_stop = hasher_(stop_to_stop.second);
    return hash_fist_stop + hash_second_stop * 17;
}
 
void TransportCatalogue::TransportCatalogue::AddStop(const std::string& name, double lat, double lng, const std::vector<std::pair<std::string, int>>& distances)
{
    details::Stop temp_stop;
    temp_stop.stop_name = name;
    temp_stop.coords.lat = lat;
    temp_stop.coords.lng = lng;
    for (auto& dic : distances) {
        distance_[std::make_pair(name, dic.first)] = dic.second;
    }
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

double TransportCatalogue::TransportCatalogue::DistanceCoord(details::Bus* bus) const
{
    double distance = 0.0;
    for (int i = 0; i < bus->single_marshrut.size() - 1; i++){
        distance += ComputeDistance(bus->single_marshrut[i]->coords,
            bus->single_marshrut[i + 1]->coords);
    }
    return distance;
}

TransportCatalogue::details::BusInfo TransportCatalogue::TransportCatalogue::GetBusInfo(const std::string& find_bus_name) const
{
    details::BusInfo bus_info;
    int distance = 0;
    bus_info.bus_name = find_bus_name;
    details::Bus* find_bus = FindBus(find_bus_name);
    if (find_bus){
        bus_info.in_cataloge = true;
        std::vector<details::Stop*> stops_vector = find_bus->single_marshrut;
        for (int i = 0; i < stops_vector.size() - 1; i++) {
            std::pair<std::string, std::string> stop_to_next_stop = std::make_pair(stops_vector[i]->stop_name, stops_vector[i + 1]->stop_name);
            std::pair<std::string, std::string> next_stop_to_stop = std::make_pair(stops_vector[i + 1]->stop_name, stops_vector[i]->stop_name);
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
        }
 
        std::set<details::Stop*> unique_stops(find_bus->single_marshrut.begin(), find_bus->single_marshrut.end());

        bus_info.stops_on_route = find_bus->single_marshrut.size();
        bus_info.unique_stops = unique_stops.size();
        bus_info.route_length = distance;
        bus_info.curvature = 1.0 * distance / DistanceCoord(find_bus);
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
