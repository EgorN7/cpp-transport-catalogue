#pragma once
#include<string>
#include<vector>
#include<set>
#include<iostream>
#include<variant>

#include "geo.h"
#include "graph.h"

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

        struct StopsHasher 
        {
            size_t operator() (const std::pair<Stop*, Stop*>& stop_to_stop) const;

        private:
            std::hash<std::string> hasher_;
        };

        struct StatRequest 
        {
            int id;
            std::string type;
            std::string name;
            std::string from_stop;
            std::string to_stop;
        };

        void PrintStopInfo(const StopInfo& stop, std::ostream& out);
        void PrintBusInfo(const BusInfo& bus, std::ostream& out);
    }
}

namespace transport_router {

    namespace details {

        struct RoutingSettings
        {
            double bus_wait_time_ = 0;
            double bus_velocity_ = 0;
        };

        struct BusEdge {
            std::string bus_name;
            size_t span_count = 0;
            double time = 0;
        };

        struct StopEdge {
            std::string name;
            double time = 0;
        };

        struct RouteInfo {
            double total_time = 0.;
            std::vector<std::variant<StopEdge, BusEdge>> edges;
        };

        struct RouterByStop {
            graph::VertexId bus_wait_start;
            graph::VertexId bus_wait_end;
        };
    }

}
namespace serialization {

    struct SerializationSettings { std::string file_name; };
}


std::ostream& operator<< (std::ostream& out, const transport_catalogue::details::StopInfo& stop_info);
std::ostream& operator<< (std::ostream& out, const transport_catalogue::details::BusInfo& bus_info);