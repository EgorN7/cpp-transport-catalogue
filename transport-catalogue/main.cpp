#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include <iostream>

using JSONReader = transport_catalogue::details::json::JSONReader;

int main() {
    std::vector<transport_catalogue::details::StatRequest> stat_request;
    map_renderer::RenderSettings render_settings;
    transport_catalogue::TransportCatalogue catalogue;

    JSONReader json_reader(std::cin);
    json_reader.Parse(catalogue, stat_request, render_settings);

    map_renderer::MapRenderer renderer (render_settings);
    request_handler::RequestHandler handler(catalogue, renderer);

    Print(handler.ExecuteQueries(stat_request), std::cout);
}