#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include "domain.h"

#include <deque>
#include <unordered_map>
#include <iostream>
#include <memory>

namespace transport_router {

	using namespace graph;

	namespace details {
	
		static const uint16_t KILOMETER = 1000;
		static const uint16_t HOUR = 60;
	
        struct TransportRouterData {
            RoutingSettings routing_settings;
            std::unordered_map<transport_catalogue::details::Stop*, details::RouterByStop> stop_to_router;
            std::unordered_map<EdgeId, std::variant<details::StopEdge, details::BusEdge>> edge_id_to_edge;
            std::unique_ptr<DirectedWeightedGraph<double>> graph;
            Router<double>::RoutesInternalData router_internal_data;
        };
	}

    class TransportRouter {
    public:
        TransportRouter(const transport_catalogue::TransportCatalogue& catalogue
            , details::RoutingSettings& routing_settings);

        TransportRouter(const transport_catalogue::TransportCatalogue& catalogue
            , details::RoutingSettings& routing_settings
            , std::unordered_map<transport_catalogue::details::Stop*, details::RouterByStop>& stop_to_router 
            , std::unordered_map<EdgeId, std::variant<details::StopEdge, details::BusEdge>>& edge_id_to_edge
            , std::unique_ptr<DirectedWeightedGraph<double>>& graph
            , Router<double>::RoutesInternalData& router_internal_data);


        std::optional<details::RouteInfo> GetRouteInfo(const std::string& from_stop, const std::string& to_stop) const;

        const details::RoutingSettings& GetRoutingSettings() const;
        const std::unordered_map<transport_catalogue::details::Stop*, details::RouterByStop>& GetStopToRouter() const;
        const std::unordered_map<EdgeId, std::variant<details::StopEdge, details::BusEdge>>& GetEdgeIdToEdge() const;

        const DirectedWeightedGraph<double>& GetGraph() const;
        const Router<double>& GetRouter() const;

    private:

        std::unordered_map<transport_catalogue::details::Stop*, details::RouterByStop> stop_to_router_;
        std::unordered_map<EdgeId, std::variant<details::StopEdge, details::BusEdge>> edge_id_to_edge_;

        std::unique_ptr<DirectedWeightedGraph<double>> graph_;
        std::unique_ptr<Router<double>> router_;

        const transport_catalogue::TransportCatalogue& catalogue_;
        details::RoutingSettings& routing_settings_;

        void SetGraph();
        std::deque<transport_catalogue::details::Stop*> GetStopsPtr();
        std::deque<transport_catalogue::details::Bus*> GetBusPtr();
        void SetStops(const std::deque<transport_catalogue::details::Stop*>& stops);

        void AddEdgeToStop();
        void AddEdgeToBus();

        template <typename Iterator>
        void ParseBusToEdges(Iterator first,
            Iterator last,
            const transport_catalogue::details::Bus* bus);

        Edge<double> MakeEdgeToBus(transport_catalogue::details::Stop* start
            , transport_catalogue::details::Stop* end, const double distance) const;

        const std::variant<details::StopEdge, details::BusEdge>& GetEdge(EdgeId id) const;

        std::optional<details::RouterByStop> GetRouterByStop(transport_catalogue::details::Stop* stop) const;
   
    };



    template <typename Iterator>
    void TransportRouter::ParseBusToEdges(Iterator first,
        Iterator last,
        const transport_catalogue::details::Bus* bus) {

        for (auto it = first; it != last; ++it) {
            size_t distance = 0;
            size_t span = 0;

            for (auto it2 = std::next(it); it2 != last; ++it2) {
                distance += catalogue_.GetDistanceBetweenTwoStops(*prev(it2), *it2);
                ++span;

                EdgeId id = graph_->AddEdge(MakeEdgeToBus(*it, *it2, distance));

                edge_id_to_edge_[id] = details::BusEdge{ bus->bus_name, span, graph_->GetEdge(id).weight };
            }
        }
    }

}