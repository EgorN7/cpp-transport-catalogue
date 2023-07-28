#include "request_handler.h"
#include <sstream>

using Bus = transport_catalogue::details::Bus;
using Stop = transport_catalogue::details::Stop;

using namespace transport_catalogue::details::json;

namespace request_handler {

	RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& catalogue, map_renderer::MapRenderer& renderer )
		: catalogue_(catalogue), renderer_(renderer) {
	}

    transport_catalogue::details::BusInfo RequestHandler::GetBusStat(const std::string& bus_name) const {
        return catalogue_.GetBusInfo(bus_name);
    }

    transport_catalogue::details::StopInfo RequestHandler::GetStopInfo(const std::string& stop_name) const {
        return catalogue_.SearchStop(stop_name);
    }





    Document RequestHandler::ExecuteQueries(std::vector<transport_catalogue::details::StatRequest>& stat_requests) {
        std::vector<Node> result_request;

        for (transport_catalogue::details::StatRequest& req : stat_requests) {

            if (req.type == "Stop") {
                result_request.push_back(ExecuteMakeNodeStop(req.id, GetStopInfo(req.name)));

            }
            else if (req.type == "Bus") {
                result_request.push_back(ExecuteMakeNodeBus(req.id, GetBusStat(req.name)));
            }
            else if (req.type == "Map") {
            result_request.push_back(ExecuteMakeNodeMap(req.id));
            }
        }
        return Document{ Node{result_request} };
    }

    transport_catalogue::details::json::Node RequestHandler::ExecuteMakeNodeStop(int id_request, const transport_catalogue::details::StopInfo& stop_info) {
        Dict result;
        Array buses;
        std::string str_not_found = "not found";

        if (stop_info.in_cataloge) {
            result.emplace("request_id", Node{ id_request });

            for (std::string bus_name : stop_info.buses) {
                buses.push_back(Node{ bus_name });
            }

            result.emplace("buses", Node{ buses });

        }
        else {
            result.emplace("request_id", Node{ id_request });
            result.emplace("error_message", Node{ str_not_found });
        }

        return Node{ result };
    }

    transport_catalogue::details::json::Node RequestHandler::ExecuteMakeNodeBus(int id_request, const transport_catalogue::details::BusInfo& bus_info) {
        Dict result;
        std::string str_not_found = "not found";

        if (bus_info.in_cataloge) {
            result.emplace("request_id", Node{ id_request });
            result.emplace("curvature", Node{ bus_info.curvature });
            result.emplace("route_length", Node{ bus_info.route_length });
            result.emplace("stop_count", Node{ bus_info.stops_on_route });
            result.emplace("unique_stop_count", Node{ bus_info.unique_stops });
        }
        else {
            result.emplace("request_id", Node{ id_request });
            result.emplace("error_message", Node{ str_not_found });
        }

        return Node{ result };
    }

    transport_catalogue::details::json::Node RequestHandler::ExecuteMakeNodeMap(int id_request) {
        transport_catalogue::details::json::Dict result;
        std::ostringstream map_stream;
        std::string map_str;
        renderer_.InitSphereProjector(GetStopsCoordinates());
        ExecuteRenderMap();
        renderer_.GetStreamMap(map_stream);
        map_str = map_stream.str();

        result.emplace("request_id", Node(id_request));
        result.emplace("map", Node(map_str));

        return Node(result);
    }

    void RequestHandler::ExecuteRenderMap() const {
        std::vector<std::pair<Bus*, int>> buses_palette;
        std::vector<Stop*> stops_sort;
        int palette_size = 0;
        int palette_index = 0;

        palette_size = renderer_.GetPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return;
        }

        auto buses = GetSortBusesNames();

        if (buses.size() > 0) {

            for (auto& bus_name : buses) {
                auto busptr = catalogue_.FindBus(std::string(bus_name));
                if (busptr->single_marshrut.size() > 0) {
                    buses_palette.push_back(std::make_pair(busptr, palette_index));
                    palette_index++;

                    if (palette_index == palette_size) {
                        palette_index = 0;
                    }
                }
            }

            if (buses_palette.size() > 0) {
                renderer_.AddLine(buses_palette);
                renderer_.AddBusesName(buses_palette);
            }
        }

        auto stops = catalogue_.GetStopnameToStop();
        if (stops.size() > 0) {
            std::vector<std::string_view> stops_name;

            for (auto& [stop_name, stop] : stops) {

                if (GetStopInfo(std::string(stop_name)).buses.size() > 0) {
                    stops_name.push_back(stop_name);
                }
            }

            std::sort(stops_name.begin(), stops_name.end());

            for (std::string_view stop_name : stops_name) {
                Stop* stop = catalogue_.FindStop(stop_name);
                if (stop) {
                    stops_sort.push_back(stop);
                }
            }

            if (stops_sort.size() > 0) {
                renderer_.AddStopsCircle(stops_sort);
                renderer_.AddStopsName(stops_sort);
            }
        }
    }
    std::vector<geo::Coordinates> RequestHandler::GetStopsCoordinates() const {

        std::vector<geo::Coordinates> stops_coordinates;
        auto buses = catalogue_.GetBusnameToBus();

        for (auto& [busname, bus] : buses) {
            for (auto& stop : bus->single_marshrut) {
                geo::Coordinates coordinates;
                coordinates.lat = stop->coords.lat;
                coordinates.lng = stop->coords.lng;

                stops_coordinates.push_back(coordinates);
            }
        }
        return stops_coordinates;
    }

    
    std::vector<std::string_view> RequestHandler::GetSortBusesNames() const {
        std::vector<std::string_view> buses_names;

        auto buses = catalogue_.GetBusnameToBus();
        if (buses.size() > 0) {

            for (auto& [busname, bus] : buses) {
                buses_names.push_back(busname);
            }

            std::sort(buses_names.begin(), buses_names.end());

            return buses_names;
        }
        else {
            return {};
        }
    }
}