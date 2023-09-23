#include <fstream>
#include <iostream>
#include <string_view>

#include "serialization.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"

using namespace std::literals;

using JSONReader = transport_catalogue::details::json::JSONReader;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);
    
    

    if (mode == "make_base"sv) {

        transport_catalogue::TransportCatalogue catalogue;
        map_renderer::RenderSettings render_settings;
        transport_router::details::RoutingSettings routing_settings;
        serialization::SerializationSettings serialization_settings;
        
        JSONReader json_reader(std::cin);
        json_reader.ParseMakeBase(catalogue, render_settings, routing_settings, serialization_settings);
        
        transport_router::TransportRouter router(catalogue, routing_settings);

        std::ofstream out_file(serialization_settings.file_name, std::ios::binary);    
        serialization::CatalogueSerialization(catalogue, render_settings, router, out_file);

    } else if (mode == "process_requests"sv) {
        
        serialization::SerializationSettings serialization_settings;
        serialization::CatalogueBase catalogue_base;
        std::vector<transport_catalogue::details::StatRequest> stat_request;
        
        JSONReader json_reader(std::cin);
        json_reader.ParseRequest(stat_request, serialization_settings);

        std::ifstream in_file(serialization_settings.file_name, std::ios::binary);        
        serialization::CatalogueDeserialization(catalogue_base, in_file);
        
        map_renderer::MapRenderer renderer(catalogue_base.render_settings);
        transport_router::TransportRouter router(catalogue_base.catalogue
            , catalogue_base.transport_router_data.routing_settings
            , catalogue_base.transport_router_data.stop_to_router
            , catalogue_base.transport_router_data.edge_id_to_edge
            , catalogue_base.transport_router_data.graph
            , catalogue_base.transport_router_data.router_internal_data);

        renderer.FillMap(catalogue_base.catalogue);
        request_handler::RequestHandler handler(catalogue_base.catalogue, renderer, router);

        Print(json_reader.ExecuteQueries(handler, stat_request), std::cout);

    } else {
        PrintUsage();
        return 1;
    }
}