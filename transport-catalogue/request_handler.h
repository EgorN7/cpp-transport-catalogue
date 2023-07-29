#pragma once
#include <optional>
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace request_handler {

    class RequestHandler {
    public:
        // MapRenderer понадобится в следующей части итогового проекта
        RequestHandler(const transport_catalogue::TransportCatalogue& catalogue,const map_renderer::MapRenderer& renderer);

        // Возвращает информацию о маршруте (запрос Bus)
        transport_catalogue::details::BusInfo GetBusStat(const std::string& bus_name) const;

        // Возвращает маршруты, проходящие через
        transport_catalogue::details::StopInfo GetStopInfo(const std::string& stop_name) const;

        // Этот метод будет нужен в следующей части итогового проекта
        const svg::Document& RenderMap() const;

        // Возвращает список координат всех остановок
        std::vector<geo::Coordinates> GetStopsCoordinates() const;

        // Возвращает отсортированыый список автобусных маршрутов
        std::vector<std::string_view> GetSortBusesNames() const;

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const transport_catalogue::TransportCatalogue& catalogue_;
        const map_renderer::MapRenderer& renderer_;
    };

}