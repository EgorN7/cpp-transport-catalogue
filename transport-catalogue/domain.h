#pragma once
#include<string>
#include<vector>
#include<set>
#include<iostream>

#include "geo.h"

namespace transport_catalogue {

    namespace details {
        struct Stop
        {
            std::string stop_name;
            geo::Coordinates coords;

            Stop();
            Stop(std::string name);

            bool operator==(const Stop& rhs) const;
            bool operator!=(const Stop& rhs) const;

        };

        struct Bus
        {
            std::string bus_name;
            std::vector <Stop*>  single_marshrut;
            bool circle;

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
            std::set<std::string> buses;
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

        struct StatRequest {
            int id;
            std::string type;
            std::string name;
        };

        void PrintStopInfo(const StopInfo& stop, std::ostream& out);
        void PrintBusInfo(const BusInfo& bus, std::ostream& out);
    }
}



std::ostream& operator<< (std::ostream& out, const transport_catalogue::details::StopInfo& stop_info);
std::ostream& operator<< (std::ostream& out, const transport_catalogue::details::BusInfo& bus_info);