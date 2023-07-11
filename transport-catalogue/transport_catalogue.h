#pragma once
#include<deque>
#include<unordered_map>
#include<string>
#include<string_view>
#include<vector>
#include<map>
 
#include "geo.h"
 
namespace TransportCatalogue {
    using namespace std::literals;

    namespace details {
        struct Stop
        {
            std::string stop_name;
            Coordinates coords;

            Stop();
            Stop(std::string name);

            bool operator==(const Stop& rhs) const;
            bool operator!=(const Stop& rhs) const;

        };

        struct Bus
        {
            std::string bus_name;
            std::vector <Stop*>  single_marshrut;

            bool operator==(const Bus& rhs) const;
            bool operator!=(const Bus& rhs) const;
        };

        struct BusRouteDistance 
        {
            int real_distance = 0;
            double coordinates_distance = 0;
        };

        struct StopInfo 
        {
            std::string stop_name;
            std::vector<std::string> buses;
            bool in_cataloge = false;
        };

        struct BusInfo
        {
            std::string bus_name;
            int stops_on_route = 0;
            int unique_stops = 0;
            int route_length = 0;
            double curvature = 0.0;
            bool in_cataloge = false;
        };

        struct StopsHasher {
            size_t operator() (const std::pair<Stop*, Stop*>& stop_to_stop) const;

        private:
            std::hash<std::string> hasher_;
        };
    }

    class TransportCatalogue
    {
    public:


        void AddStop(const std::string& name, const Coordinates& coordinates);
        void AddBus(const std::string& bus_name, const std::vector<std::string>& stops, bool circle);
        void AddDistanceBetweenStops(const std::string& first_stop, const std::string& second_stop, int distance_between_stops);

        details::Stop* FindStop(std::string_view find_name) const;
        details::Bus* FindBus(std::string_view find_name) const;

        details::StopInfo SearchStop(const std::string& find_stop_name) const;
        details::BusInfo GetBusInfo(const std::string& find_bus_name) const;

        void PrintStopInfo(const std::string& find_stop_name, std::ostream& out);
        void PrintBusInfo(const std::string& find_bus_name, std::ostream& out);

    private:

        std::deque<details::Stop> stops_;

        std::deque<details::Bus> buses_;

        std::unordered_map<std::string_view, details::Stop*>  stopname_to_stop_;

        std::unordered_map<std::string_view, details::Bus*>  busname_to_bus_;

        std::unordered_map<std::pair<details::Stop*, details::Stop*>, int, details::StopsHasher>  distance_;

        int GetDistanceBetweenTwoStops(details::Stop* first_stop, details::Stop* second_stop) const;
        details::BusRouteDistance GetBusRouteDistance(details::Bus* bus) const;
    };
}

std::ostream& operator<< (std::ostream& out, const TransportCatalogue::details::StopInfo& stop_info);
std::ostream& operator<< (std::ostream& out, const TransportCatalogue::details::BusInfo& bus_info);
