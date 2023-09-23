#pragma once
 

#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include "domain.h"
#include "svg.pb.h"
#include "map_renderer.h"
#include "map_renderer.pb.h"
#include "transport_router.h"
#include "transport_router.pb.h"
 
#include <iostream>
#include <fstream>
 
namespace serialization {

    // helper type for the visitor #4
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };
    // explicit deduction guide (not needed as of C++20)
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;


    struct CatalogueBase {
        transport_catalogue::TransportCatalogue catalogue;
        map_renderer::RenderSettings render_settings;
        transport_router::details::TransportRouterData transport_router_data;
    };

    void CatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue
        , map_renderer::RenderSettings& render_settings
        , const transport_router::TransportRouter& router, std::ostream& out);
    void CatalogueDeserialization(CatalogueBase& catalogue_base, std::istream& in);

    transport_catalogue_protobuf::TransportCatalogue TransportCatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue);
    void TransportCatalogueDeserialization(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto);

    template <typename It>
    uint32_t CalculateIdStop(It start, It end, std::string_view name);

    template <typename It>
    uint32_t CalculateIdBus(It start, It end, std::string_view name);

    transport_catalogue_protobuf::Color SerializeColor(const svg::Color& tc_color);
    svg::Color DeserealizeColor(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::Color& color_proto);
    transport_catalogue_protobuf::RenderSettings RenderSettingsSerialization(const map_renderer::RenderSettings& render_settings);
    void RenderSettingsDeserialization(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::RenderSettings& render_settings_proto);
    transport_catalogue_protobuf::TransportRouterData TransportRouterDataSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue
        , const transport_router::TransportRouter& router);
    void RoutingSettingsDeserialization(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::TransportRouterData& transport_router_data_proto);
}

template <typename It>
uint32_t serialization::CalculateIdStop(It start, It end, std::string_view name) {

    auto stop_it = std::find_if(start, end, [&name](const transport_catalogue::details::Stop stop) {return stop.stop_name == name; });
    return std::distance(start, stop_it);
}

template <typename It>
uint32_t serialization::CalculateIdBus(It start, It end, std::string_view name) {

    auto bus_it = std::find_if(start, end, [&name](const transport_catalogue::details::Bus bus) {return bus.bus_name == name; });
    return std::distance(start, bus_it);
}