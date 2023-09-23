#include "serialization.h"
 
namespace serialization {

    void CatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue
                                , map_renderer::RenderSettings& render_settings
                                , const transport_router::TransportRouter& router, std::ostream& out) {

        transport_catalogue_protobuf::Catalogue catalogue_proto;

        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto = TransportCatalogueSerialization(transport_catalogue);
        transport_catalogue_protobuf::RenderSettings render_settings_proto = RenderSettingsSerialization(render_settings);
        transport_catalogue_protobuf::TransportRouterData transport_router_data_proto = TransportRouterDataSerialization(transport_catalogue, router);

        *catalogue_proto.mutable_transport_catalogue() = std::move(transport_catalogue_proto);
        *catalogue_proto.mutable_render_settings() = std::move(render_settings_proto);
        *catalogue_proto.mutable_transport_router_data() = std::move(transport_router_data_proto);
        catalogue_proto.SerializePartialToOstream(&out);

    }
    
    void CatalogueDeserialization(CatalogueBase& catalogue_base, std::istream& in) {

        transport_catalogue_protobuf::Catalogue catalogue_proto;
        auto success_parsing_catalogue_from_istream = catalogue_proto.ParseFromIstream(&in);

        if (!success_parsing_catalogue_from_istream) {
            throw std::runtime_error("cannot parse serialized file from istream");
        }

        TransportCatalogueDeserialization(catalogue_base, catalogue_proto.transport_catalogue());
        RenderSettingsDeserialization(catalogue_base, catalogue_proto.render_settings());
        RoutingSettingsDeserialization(catalogue_base, catalogue_proto.transport_router_data());
    }

 
    transport_catalogue_protobuf::TransportCatalogue TransportCatalogueSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue) {
    
        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto;
 
        const auto& stops = transport_catalogue.GetStops();
        const auto& buses = transport_catalogue.GetBuses();
        const auto& distances = transport_catalogue.GetDistance();;
    
        int id = 0;
        for (const auto& stop : stops) {
 
            transport_catalogue_protobuf::Stop stop_proto;
 
            stop_proto.set_id(id);
            stop_proto.set_name(stop.stop_name);
            stop_proto.set_latitude(stop.coords.lat);
            stop_proto.set_longitude(stop.coords.lng);
        
            *transport_catalogue_proto.add_stops() = std::move(stop_proto);
        
            ++id;
        }
 
        for (const auto& bus : buses) {
 
            transport_catalogue_protobuf::Bus bus_proto;
 
            bus_proto.set_name(bus.bus_name);
 
            for (auto stop : bus.single_marshrut) {
                uint32_t stop_id = CalculateIdStop(stops.cbegin(),
                                                stops.cend(), 
                                                stop->stop_name);
                bus_proto.add_stops(stop_id);
            }
 
            bus_proto.set_circle(bus.circle);
 
            *transport_catalogue_proto.add_buses() = std::move(bus_proto);
        }
    
        for (const auto& [pair_stops, distance] : distances) {
 
            transport_catalogue_protobuf::Distance distance_proto;
 
            distance_proto.set_start(CalculateIdStop(stops.cbegin(),
                                                  stops.cend(), 
                                                  pair_stops.first->stop_name));
                                              
            distance_proto.set_end(CalculateIdStop(stops.cbegin(),
                                                stops.cend(), 
                                                pair_stops.second->stop_name));
                                            
            distance_proto.set_distance(distance);
 
            *transport_catalogue_proto.add_distances() = std::move(distance_proto);
        }
 
        return transport_catalogue_proto;
    }
 
    
    void TransportCatalogueDeserialization(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto) {
 
        transport_catalogue::TransportCatalogue& transport_catalogue = catalogue_base.catalogue;
    
        const auto& stops_proto = transport_catalogue_proto.stops();
        const auto& buses_proto = transport_catalogue_proto.buses();
        const auto& distances_proto = transport_catalogue_proto.distances();
    
        for (const auto& stop : stops_proto) {
        
            transport_catalogue::details::Stop tc_stop;

            tc_stop.stop_name = stop.name();
            tc_stop.coords.lat = stop.latitude();
            tc_stop.coords.lng = stop.longitude();

            transport_catalogue.AddStop(std::move(tc_stop));
        }

        const auto& stops = transport_catalogue.GetStops();

        for (const auto& bus_proto : buses_proto) {

            transport_catalogue::details::Bus tc_bus;

            tc_bus.bus_name = bus_proto.name();

            for (auto stop_id : bus_proto.stops()) {
                auto name = stops[stop_id].stop_name;
                tc_bus.single_marshrut.push_back(transport_catalogue.FindStop(name));
            }

            tc_bus.circle = bus_proto.circle();

            transport_catalogue.AddBus(std::move(tc_bus));
        }
    
        for (const auto& distance : distances_proto) {
            transport_catalogue.AddDistanceBetweenStops(stops[distance.start()].stop_name, stops[distance.end()].stop_name, distance.distance());
        }      
    }
    
    transport_catalogue_protobuf::Color SerializeColor(const svg::Color& tc_color) {
        
        transport_catalogue_protobuf::Color color_proto;

        if (std::holds_alternative<std::monostate>(tc_color)) {
            color_proto.set_none(true);
                
        } else if (std::holds_alternative<svg::Rgb>(tc_color)) {
            svg::Rgb rgb = std::get<svg::Rgb>(tc_color);

            color_proto.mutable_rgb()->set_red_(rgb.red_);
            color_proto.mutable_rgb()->set_green_(rgb.green_);
            color_proto.mutable_rgb()->set_blue_(rgb.blue_);

        } else if (std::holds_alternative<svg::Rgba>(tc_color)) {
            svg::Rgba rgba = std::get<svg::Rgba>(tc_color);

            color_proto.mutable_rgba()->set_red_(rgba.red_);
            color_proto.mutable_rgba()->set_green_(rgba.green_);
            color_proto.mutable_rgba()->set_blue_(rgba.blue_);
            color_proto.mutable_rgba()->set_opacity_(rgba.opacity_);

        } else if (std::holds_alternative<std::string>(tc_color)) {
            color_proto.set_string_color(std::get<std::string>(tc_color));
        }

        return color_proto;
    }

    svg::Color DeserializeColor(const transport_catalogue_protobuf::Color& color_proto) {

        svg::Color color;

        if (color_proto.has_rgb()) {
            svg::Rgb rgb;

            rgb.red_ = color_proto.rgb().red_();
            rgb.green_ = color_proto.rgb().green_();
            rgb.blue_ = color_proto.rgb().blue_();

            color = rgb;

        } else if (color_proto.has_rgba()) {
            svg::Rgba rgba;

            rgba.red_ = color_proto.rgba().red_();
            rgba.green_ = color_proto.rgba().green_();
            rgba.blue_ = color_proto.rgba().blue_();
            rgba.opacity_ = color_proto.rgba().opacity_();

            color = rgba;

        } else {color = color_proto.string_color();}

        return color;
    }

    transport_catalogue_protobuf::RenderSettings RenderSettingsSerialization(const map_renderer::RenderSettings& render_settings) {

        transport_catalogue_protobuf::RenderSettings render_settings_proto;

        render_settings_proto.set_width_(render_settings.width_);
        render_settings_proto.set_height_(render_settings.height_);
        render_settings_proto.set_padding_(render_settings.padding_);
        render_settings_proto.set_line_width_(render_settings.line_width_);
        render_settings_proto.set_stop_radius_(render_settings.stop_radius_);
        render_settings_proto.set_bus_label_font_size_(render_settings.bus_label_font_size_);

        transport_catalogue_protobuf::Point bus_label_offset_proto;   
        bus_label_offset_proto.set_x(render_settings.bus_label_offset_.first);
        bus_label_offset_proto.set_y(render_settings.bus_label_offset_.second);

        *render_settings_proto.mutable_bus_label_offset_() = std::move(bus_label_offset_proto);

        render_settings_proto.set_stop_label_font_size_(render_settings.stop_label_font_size_);

        transport_catalogue_protobuf::Point stop_label_offset_proto;
        stop_label_offset_proto.set_x(render_settings.stop_label_offset_.first);
        stop_label_offset_proto.set_y(render_settings.stop_label_offset_.second);

        *render_settings_proto.mutable_stop_label_offset_() = std::move(stop_label_offset_proto);
        *render_settings_proto.mutable_underlayer_color_() = std::move(SerializeColor(render_settings.underlayer_color_));
        render_settings_proto.set_underlayer_width_(render_settings.underlayer_width_);

        const auto& colors = render_settings.color_palette_;
        for (const auto& color : colors) {
            *render_settings_proto.add_color_palette_() = std::move(SerializeColor(color));
        }

        return render_settings_proto;
    }

    void RenderSettingsDeserialization(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::RenderSettings& render_settings_proto) {

        map_renderer::RenderSettings& render_settings = catalogue_base.render_settings;

        render_settings.width_ = render_settings_proto.width_();
        render_settings.height_ = render_settings_proto.height_();
        render_settings.padding_ = render_settings_proto.padding_();
        render_settings.line_width_ = render_settings_proto.line_width_();
        render_settings.stop_radius_ = render_settings_proto.stop_radius_();
        render_settings.bus_label_font_size_ = render_settings_proto.bus_label_font_size_();

        render_settings.bus_label_offset_.first = render_settings_proto.bus_label_offset_().x();
        render_settings.bus_label_offset_.second = render_settings_proto.bus_label_offset_().y();

        render_settings.stop_label_font_size_ = render_settings_proto.stop_label_font_size_();

        render_settings.stop_label_offset_.first = render_settings_proto.stop_label_offset_().x();
        render_settings.stop_label_offset_.second = render_settings_proto.stop_label_offset_().y();

        render_settings.underlayer_color_ = DeserializeColor(render_settings_proto.underlayer_color_());
        render_settings.underlayer_width_ = render_settings_proto.underlayer_width_();

        for (const auto& color_proto : render_settings_proto.color_palette_()) {
            render_settings.color_palette_.push_back(DeserializeColor(color_proto));
        }
    }  
    
    
    transport_catalogue_protobuf::TransportRouterData TransportRouterDataSerialization(const transport_catalogue::TransportCatalogue& transport_catalogue
        , const transport_router::TransportRouter& router){

        const auto& stops = transport_catalogue.GetStops();
        const auto& buses = transport_catalogue.GetBuses();

        transport_catalogue_protobuf::TransportRouterData transport_router_data;

        const transport_router::details::RoutingSettings& routing_settings = router.GetRoutingSettings();
        
        transport_router_data.mutable_settings()->set_bus_wait_time(routing_settings.bus_wait_time_);
        transport_router_data.mutable_settings()->set_bus_velocity(routing_settings.bus_velocity_);

        const std::unordered_map<transport_catalogue::details::Stop*, transport_router::details::RouterByStop>& stop_to_router = router.GetStopToRouter();
        for (const auto& [stop, router_by_stop] : stop_to_router) {

            transport_catalogue_protobuf::StopToRouter stop_to_router_photo;

            stop_to_router_photo.set_stop_id(CalculateIdStop(stops.cbegin(),
                stops.cend(),
                stop->stop_name));

            stop_to_router_photo.mutable_router_by_stop()->set_bus_wait_start(router_by_stop.bus_wait_start);
            stop_to_router_photo.mutable_router_by_stop()->set_bus_wait_end(router_by_stop.bus_wait_end);

            *transport_router_data.add_stop_to_router() = std::move(stop_to_router_photo);
        }


        const std::unordered_map<size_t, std::variant<transport_router::details::StopEdge, transport_router::details::BusEdge>>& edge_id_to_edge = router.GetEdgeIdToEdge();
        for (const auto& [edge_id, edge] : edge_id_to_edge) {

            transport_catalogue_protobuf::EdgeIdToEdge edge_id_to_edge_photo;
            
            edge_id_to_edge_photo.set_edge_id(edge_id);
            std::visit(overloaded{
                            [&stops, &edge_id_to_edge_photo](const transport_router::details::StopEdge& arg)
                                {
                                    transport_catalogue_protobuf::StopEdge  stop_edge_photo;
                                    stop_edge_photo.set_stop_name(arg.name);
                                    stop_edge_photo.set_time(arg.time);
                                    *edge_id_to_edge_photo.mutable_stop_edge() = std::move(stop_edge_photo);
                                },
                            [&buses, &edge_id_to_edge_photo](const transport_router::details::BusEdge& arg)
                                {
                                    transport_catalogue_protobuf::BusEdge  bus_edge_photo;
                                    bus_edge_photo.set_bus_name(arg.bus_name);
                                    bus_edge_photo.set_span_count(arg.span_count);
                                    bus_edge_photo.set_time(arg.time);
                                    *edge_id_to_edge_photo.mutable_bus_edge() = std::move(bus_edge_photo);
                                },
                            }, edge);
            *transport_router_data.add_edge_id_to_edge() = std::move(edge_id_to_edge_photo);
        }

        transport_catalogue_protobuf::Graph graph_photo;

        for (int i = 0; i < router.GetGraph().GetEdgeCount(); ++i) {         
            graph::Edge edge = router.GetGraph().GetEdge(i); 
            transport_catalogue_protobuf::Edge base_edge_photo; 
            base_edge_photo.set_from(edge.from); 
            base_edge_photo.set_to(edge.to);
            base_edge_photo.set_weight(edge.weight);
            *graph_photo.add_edges() = std::move(base_edge_photo);
        }
        for (size_t i = 0; i < router.GetGraph().GetVertexCount(); ++i) {
            graph_photo.add_incidence_lists();
            transport_catalogue_protobuf::IncidenceList& base_inc_list_photo = *graph_photo.mutable_incidence_lists(i);
            int j = 0;
            for (const auto inc_edge : router.GetGraph().GetIncidentEdges(i)) { 
                base_inc_list_photo.add_edges(inc_edge);
                ++j;
            }
        }
        *transport_router_data.mutable_graph() = std::move(graph_photo);

        int count_in_bin = 0;
        for (const auto& routers_internal_data : router.GetRouter().GetRoutesInternalData()) {

            transport_catalogue_protobuf::RoutesInternalData routers_internal_data_photo;
            for (const auto& router_internal_data : routers_internal_data) {
                transport_catalogue_protobuf::RouteInternalData route_internal_data_photo;
                if (router_internal_data) {
                    transport_catalogue_protobuf::Weight weight_photo;
                    weight_photo.set_weight(router_internal_data->weight);
                    *route_internal_data_photo.mutable_weight() = std::move(weight_photo);
                    if (router_internal_data->prev_edge.has_value()) {
                        transport_catalogue_protobuf::PrevEdge prev_edge_photo;
                        prev_edge_photo.set_prev_edge(router_internal_data->prev_edge.value());

                        *route_internal_data_photo.mutable_prev_edge() = prev_edge_photo;
                    }
                }
                *routers_internal_data_photo.add_route_internal_data() = std::move(route_internal_data_photo);
                ++count_in_bin;

            }
            *transport_router_data.add_routes_inrernal_data() = std::move(routers_internal_data_photo);
        }        
        
        return transport_router_data;
    }

    void RoutingSettingsDeserialization(CatalogueBase& catalogue_base, const transport_catalogue_protobuf::TransportRouterData& transport_router_data_proto){
        catalogue_base.transport_router_data.routing_settings.bus_wait_time_ = transport_router_data_proto.settings().bus_wait_time();
        catalogue_base.transport_router_data.routing_settings.bus_velocity_ = transport_router_data_proto.settings().bus_velocity();
        
        const auto& stops = catalogue_base.catalogue.GetStops();
        const auto& buses = catalogue_base.catalogue.GetBuses();
        const auto& stop_to_router = transport_router_data_proto.stop_to_router();

        for (const auto& stop_photo : stop_to_router) {
            auto name = stops[stop_photo.stop_id()].stop_name;
            catalogue_base.transport_router_data.stop_to_router[catalogue_base.catalogue.FindStop(name)]
                = transport_router::details::RouterByStop{ stop_photo.router_by_stop().bus_wait_start(), stop_photo.router_by_stop().bus_wait_end()};
        }

        const auto& edge_id_to_edge = transport_router_data_proto.edge_id_to_edge();
        for (const auto& edge_photo : edge_id_to_edge) {
            std::variant<transport_router::details::StopEdge, transport_router::details::BusEdge> edge;
            if (!edge_photo.has_bus_edge()) {
                transport_router::details::StopEdge stop_edge;
                stop_edge.name = edge_photo.stop_edge().stop_name();
                stop_edge.time = edge_photo.stop_edge().time();
                edge = stop_edge;
            }else {
                transport_router::details::BusEdge bus_edge;
                bus_edge.bus_name = edge_photo.bus_edge().bus_name();
                bus_edge.span_count = edge_photo.bus_edge().span_count();
                bus_edge.time = edge_photo.bus_edge().time();
                edge = bus_edge;
            }
            catalogue_base.transport_router_data.edge_id_to_edge[edge_photo.edge_id()] = std::move(edge);
        }

        std::vector<graph::Edge<double>> edges;;
        edges.reserve(transport_router_data_proto.graph().edges_size());
        for (int i = 0; i < transport_router_data_proto.graph().edges_size(); ++i) {
            graph::Edge<double> edge;
            edge.from = transport_router_data_proto.graph().edges(i).from();
            edge.to = transport_router_data_proto.graph().edges(i).to();
            edge.weight = transport_router_data_proto.graph().edges(i).weight();
            edges.emplace_back(std::move(edge));
        }

        std::vector<graph::IncidenceList> incidence_lists;
        incidence_lists.reserve(transport_router_data_proto.graph().incidence_lists_size());
        for (int i = 0; i < transport_router_data_proto.graph().incidence_lists_size(); ++i) {
            graph::IncidenceList incidence_list;
            incidence_list.reserve(transport_router_data_proto.graph().incidence_lists(i).edges_size());
            for (int j = 0; j < transport_router_data_proto.graph().incidence_lists(i).edges_size(); ++j) {
                incidence_list.emplace_back(transport_router_data_proto.graph().incidence_lists(i).edges(j));
            }
            incidence_lists.emplace_back(std::move(incidence_list));
        }

        catalogue_base.transport_router_data.graph = std::make_unique<graph::DirectedWeightedGraph<double>>(2 * catalogue_base.catalogue.GetStopnameToStop().size());
        catalogue_base.transport_router_data.graph->ModifyEdges() = std::move(edges);
        catalogue_base.transport_router_data.graph->ModifyIncidenceLists() = std::move(incidence_lists);


        graph::Router<double>::RoutesInternalData router_internal_data;
        router_internal_data.reserve(transport_router_data_proto.routes_inrernal_data_size());
        for (int i = 0; i < transport_router_data_proto.routes_inrernal_data_size(); ++i) {
            std::vector<std::optional<graph::Router<double>::RouteInternalData>> route_internal_data;
            route_internal_data.reserve(transport_router_data_proto.routes_inrernal_data(i).route_internal_data_size());
            for (int j = 0; j < transport_router_data_proto.routes_inrernal_data(i).route_internal_data_size(); ++j) {
                if (transport_router_data_proto.routes_inrernal_data(i).route_internal_data(j).has_weight()) {
                    graph::Router<double>::RouteInternalData internal_data;
                    internal_data.weight = transport_router_data_proto.routes_inrernal_data(i).route_internal_data(j).weight().weight();
                    if (transport_router_data_proto.routes_inrernal_data(i).route_internal_data(j).has_prev_edge()) {
                        internal_data.prev_edge = transport_router_data_proto.routes_inrernal_data(i).route_internal_data(j).prev_edge().prev_edge();
                    } else {
                        internal_data.prev_edge = std::nullopt;
                    }
                    route_internal_data.emplace_back(internal_data);
                } else {
                    route_internal_data.emplace_back(std::nullopt);
                }
            }
            router_internal_data.emplace_back(std::move(route_internal_data));
        }
        catalogue_base.transport_router_data.router_internal_data = std::move(router_internal_data);

    }
    
}//end namespace serialization