#pragma once
#include <optional>
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace request_handler {

    class RequestHandler {
    public:
        // MapRenderer понадобится в следующей части итогового проекта
        RequestHandler(const transport_catalogue::TransportCatalogue& catalogue, map_renderer::MapRenderer& renderer);

        // Возвращает информацию о маршруте (запрос Bus)
        transport_catalogue::details::BusInfo GetBusStat(const std::string& bus_name) const;

        // Возвращает маршруты, проходящие через
        transport_catalogue::details::StopInfo GetStopInfo(const std::string& stop_name) const;

        // Этот метод будет нужен в следующей части итогового проекта
        //svg::Document RenderMap() const;
        transport_catalogue::details::json::Document ExecuteQueries(std::vector<transport_catalogue::details::StatRequest>& stat_requests);

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const transport_catalogue::TransportCatalogue& catalogue_;
        map_renderer::MapRenderer& renderer_;

        
        void ExecuteRenderMap() const;

        transport_catalogue::details::json::Node ExecuteMakeNodeStop(int id_request, const transport_catalogue::details::StopInfo& stop_info);
        transport_catalogue::details::json::Node ExecuteMakeNodeBus(int id_request, const transport_catalogue::details::BusInfo& bus_info);
        transport_catalogue::details::json::Node ExecuteMakeNodeMap(int id_request);
        std::vector<geo::Coordinates> GetStopsCoordinates() const;
        std::vector<std::string_view> GetSortBusesNames() const;
    };

}