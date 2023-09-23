#include "transport_router.h"

namespace transport_router {

	TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& catalogue
		, details::RoutingSettings& routing_settings)
		: catalogue_(catalogue), routing_settings_(routing_settings){
	    
        SetGraph();
        router_ = std::make_unique<Router<double>>(*graph_);
	}

    TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& catalogue
        , details::RoutingSettings& routing_settings
        , std::unordered_map<transport_catalogue::details::Stop*, details::RouterByStop>& stop_to_router
        , std::unordered_map<EdgeId, std::variant<details::StopEdge, details::BusEdge>>& edge_id_to_edge
        , std::unique_ptr<DirectedWeightedGraph<double>>& graph
        , Router<double>::RoutesInternalData& router_internal_data)
        : catalogue_(catalogue), routing_settings_(routing_settings)
        , stop_to_router_(stop_to_router)
        , edge_id_to_edge_(edge_id_to_edge)
    {
        graph_.swap(graph);
        router_ = std::make_unique<Router<double>>(*graph_, router_internal_data);
    }

    void TransportRouter::SetGraph() {

        const auto stops_ptr = GetStopsPtr();
        graph_ = std::make_unique<DirectedWeightedGraph<double>>(2 * stops_ptr.size());

        SetStops(stops_ptr);
        AddEdgeToStop();
        AddEdgeToBus();
    }

    std::deque<transport_catalogue::details::Stop*> TransportRouter::GetStopsPtr() {
        std::deque<transport_catalogue::details::Stop*> stops_ptr;

        for (auto [_, stop_ptr] : catalogue_.GetStopnameToStop()) {
            stops_ptr.push_back(stop_ptr);
        }

        return stops_ptr;
    }

    std::deque<transport_catalogue::details::Bus*> TransportRouter::GetBusPtr() {
        std::deque<transport_catalogue::details::Bus*> buses_ptr;

        for (auto [_, bus_ptr] : catalogue_.GetBusnameToBus()) {
            buses_ptr.push_back(bus_ptr);
        }

        return buses_ptr;
    }

    void TransportRouter::SetStops(const std::deque<transport_catalogue::details::Stop*>& stops) {
        size_t i = 0;

        for (const auto stop : stops) {
            VertexId first = i++;
            VertexId second = i++;

            stop_to_router_[stop] = details::RouterByStop{ first, second };
        }
    }

    void TransportRouter::AddEdgeToStop() {

        for (const auto [stop, num] : stop_to_router_) {
            EdgeId id = graph_->AddEdge(Edge<double>{num.bus_wait_start,
                num.bus_wait_end,
                routing_settings_.bus_wait_time_});

            edge_id_to_edge_[id] = details::StopEdge{ stop->stop_name, routing_settings_.bus_wait_time_ };
        }
    }

    void TransportRouter::AddEdgeToBus() {

        for (auto bus : GetBusPtr()) {
            ParseBusToEdges(bus->single_marshrut.begin(),
                bus->single_marshrut.end(),
                bus);

            if (!bus->circle) {
                ParseBusToEdges(bus->single_marshrut.rbegin(),
                    bus->single_marshrut.rend(),
                    bus);
            }
        }
    }

    Edge<double> TransportRouter::MakeEdgeToBus(transport_catalogue::details::Stop* start, transport_catalogue::details::Stop* end, const double distance) const {
        Edge<double> result;

        result.from = stop_to_router_.at(start).bus_wait_end;
        result.to = stop_to_router_.at(end).bus_wait_start;
        result.weight = distance * 1.0 / (routing_settings_.bus_velocity_ * details::KILOMETER / details::HOUR);

        return result;
    }

    const std::variant<details::StopEdge, details::BusEdge>& TransportRouter::GetEdge(EdgeId id) const { return edge_id_to_edge_.at(id); }


    std::optional<details::RouteInfo> TransportRouter::GetRouteInfo(const std::string& from_stop, const std::string& to_stop) const {
    
        const auto& route_info = router_->BuildRoute(
            GetRouterByStop(catalogue_.FindStop(from_stop))->bus_wait_start,
            GetRouterByStop(catalogue_.FindStop(to_stop))->bus_wait_start);

        if (route_info) {
            details::RouteInfo result;
            result.total_time = route_info->weight;

            for (const auto edge : route_info->edges) {
                result.edges.emplace_back(GetEdge(edge));
            }

            return result;

        }
        else {
            return std::nullopt;
        }
    }

    std::optional<details::RouterByStop> TransportRouter::GetRouterByStop(transport_catalogue::details::Stop* stop) const {
        if (stop_to_router_.count(stop)) {
            return stop_to_router_.at(stop);
        }
        else {
            return std::nullopt;
        }
    }

    const details::RoutingSettings& TransportRouter::GetRoutingSettings() const {
        return routing_settings_;
    }

    const std::unordered_map<transport_catalogue::details::Stop*, details::RouterByStop>& TransportRouter::GetStopToRouter() const {
        return stop_to_router_;
    }

    const std::unordered_map<EdgeId, std::variant<details::StopEdge, details::BusEdge>>& TransportRouter::GetEdgeIdToEdge() const {
        return edge_id_to_edge_;
    }

    const DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
        return *graph_;
    }

    const Router<double>& TransportRouter::GetRouter() const {
        return *router_;
    }
}
