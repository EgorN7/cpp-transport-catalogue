#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
#include <iostream>

using JSONReader = transport_catalogue::details::json::JSONReader;

int main() {
    std::vector<transport_catalogue::details::StatRequest> stat_request;
    map_renderer::RenderSettings render_settings;
    transport_catalogue::TransportCatalogue catalogue;
    transport_router::details::RoutingSettings routing_settings;

    JSONReader json_reader(std::cin);
    json_reader.Parse(catalogue, stat_request, render_settings, routing_settings);

    map_renderer::MapRenderer renderer(render_settings);
    transport_router::TransportRouter router(catalogue, routing_settings);
    renderer.FillMap(catalogue);
    request_handler::RequestHandler handler(catalogue, renderer, router);
    

    Print(json_reader.ExecuteQueries(handler, stat_request), std::cout);
}