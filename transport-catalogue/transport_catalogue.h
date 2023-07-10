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
            std::unordered_map <std::string, int> stops_distances;

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
    }

    class TransportCatalogue
    {
    public:


        void AddStop(details::Stop  stop);

        void AddBus(std::string bus_name, std::vector<std::string> v, bool circle);

        details::Stop* FindStop(std::string_view find_name) const;
        details::Bus* FindBus(std::string_view find_name) const;

        double DistanceCoord(details::Bus* bus) const;

        std::string SearchStop(const std::string& find_stop_name) const;
        std::string GetBusInfo(const std::string& find_bus_name) const;


    private:

        std::deque<details::Stop> stops;

        std::deque <details::Bus> buses;

        std::unordered_map<std::string_view, details::Stop*>  stopname_to_stop;

        std::unordered_map<std::string_view, details::Bus*>  busname_to_bus;
    };
}