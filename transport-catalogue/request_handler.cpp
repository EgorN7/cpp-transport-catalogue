#include "request_handler.h"

using Bus = transport_catalogue::details::Bus;
using Stop = transport_catalogue::details::Stop;

using namespace transport_catalogue::details::json;

namespace request_handler {

	RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& catalogue, const map_renderer::MapRenderer& renderer )
		: catalogue_(catalogue), renderer_(renderer) {
	}

    transport_catalogue::details::BusInfo RequestHandler::GetBusStat(const std::string& bus_name) const {
        return catalogue_.GetBusInfo(bus_name);
    }

    transport_catalogue::details::StopInfo RequestHandler::GetStopInfo(const std::string& stop_name) const {
        return catalogue_.SearchStop(stop_name);
    }

    const svg::Document& RequestHandler::RenderMap() const {
        return renderer_.GetRanderMap();
    }


    std::vector<geo::Coordinates> RequestHandler::GetStopsCoordinates() const {
        return catalogue_.GetStopsCoordinates();
    }

    
    std::vector<std::string_view> RequestHandler::GetSortBusesNames() const {
        return catalogue_.GetSortBusesNames();
    }
}