#include <sstream>
#include <variant>
#include "json_reader.h"

using Stop = transport_catalogue::details::Stop;
using Bus = transport_catalogue::details::Bus;
 
namespace transport_catalogue {
    namespace details {
        namespace json {

            JSONReader::JSONReader(Document doc) : in_document_(std::move(doc)) {}
            JSONReader::JSONReader(std::istream& input) : in_document_(json::Load(input)) {}

            void JSONReader::ParseMakeBase(transport_catalogue::TransportCatalogue& catalogue, map_renderer::RenderSettings& render_settings,
                transport_router::details::RoutingSettings& routing_settings, serialization::SerializationSettings& serialization_settings) {
                ParseMakeBaseNode(in_document_.GetRoot(), catalogue, render_settings, routing_settings, serialization_settings);
            }

            void JSONReader::ParseRequest(std::vector<details::StatRequest>& stat_request, serialization::SerializationSettings& serialization_settings) {
                ParseRequestNode(in_document_.GetRoot(), stat_request, serialization_settings);
            }

            void JSONReader::ParseMakeBaseNode(const Node& root, TransportCatalogue& catalogue, map_renderer::RenderSettings& render_settings,
                transport_router::details::RoutingSettings& routing_settings, serialization::SerializationSettings& serialization_settings) {
                Dict root_dict;

                if (root.IsMap()) {
                    root_dict = root.AsMap();

                    try {
                        ParseNodeBase(root_dict.at("base_requests"), catalogue);
                    }
                    catch (...) {
                        std::cout << "Error: base_requests is empty";
                    }
                    try {
                        ParseNodeRender(root_dict.at("render_settings"), render_settings);
                    }
                    catch (...) {
                        std::cout << "render_settings is empty";
                    }
                    try {
                        ParseNodeRouteSetting(root_dict.at("routing_settings"), routing_settings);
                    }
                    catch (...) {
                        std::cout << "routing_settings is empty";
                    }

                    try {
                        ParseNodeSerialization(root_dict.at("serialization_settings"), serialization_settings);
                    }
                    catch (...) {
                        std::cout << "serialization settings is empty";
                    }
                }
                else {
                    std::cout << "Error: root is not map";
                }
            }

            void JSONReader::ParseRequestNode(const Node& root, std::vector<details::StatRequest>& stat_request, serialization::SerializationSettings& serialization_settings) {
                Dict root_dict;

                if (root.IsMap()) {
                    root_dict = root.AsMap();

                    try {
                        ParseNodeStat(root_dict.at("stat_requests"), stat_request);
                    }
                    catch (...) {
                        std::cout << "stat_requests is empty";
                    }
                    try {
                        ParseNodeSerialization(root_dict.at("serialization_settings"), serialization_settings);
                    }
                    catch (...) {
                        std::cout << "serialization settings is empty";
                    }
                }
                else {
                    std::cout << "Error: root is not map";
                }
            }




            void JSONReader::ParseNodeBase(const Node& root, TransportCatalogue& catalogue) {
                Array base_requests;
                Dict req_map;
                Node req_node;

                std::vector<Node> buses;
                std::vector<Node> stops;

                if (root.IsArray()) {
                    base_requests = root.AsArray();

                    for (Node& node : base_requests) {
                        if (node.IsMap()) {
                            req_map = node.AsMap();

                            try {
                                req_node = req_map.at("type");
                                if (req_node.IsString()) {

                                    if (req_node.AsString() == "Bus") {
                                        buses.push_back(req_map);

                                    }
                                    else if (req_node.AsString() == "Stop") {
                                        stops.push_back(req_map);

                                    }
                                    else {
                                        std::cout << "Error: base_requests have bad type";
                                    }
                                }

                            }
                            catch (...) {
                                std::cout << "Error: base_requests not have type value";
                            }
                        }
                    }

                    for (auto stop : stops) {
                        ParseNodeStop(catalogue, stop);
                    }

                    for (auto stop : stops) {
                        ParseNodeDistances(catalogue, stop);
                    }

                    for (auto bus : buses) {
                        ParseNodeBus(catalogue, bus);
                    }

                }
                else {
                    std::cout << "Error: base_requests is not array";
                }
            }

            void JSONReader::ParseNodeStat(const Node& node, std::vector<details::StatRequest>& stat_request) {
                Array stat_requests;
                Dict req_map;
                StatRequest req;

                if (node.IsArray()) {
                    stat_requests = node.AsArray();

                    for (Node req_node : stat_requests) {

                        if (req_node.IsMap()) {

                            req_map = req_node.AsMap();
                            req.id = req_map.at("id").AsInt();
                            req.type = req_map.at("type").AsString();

                            if (req.type != "Map" && req.type != "Route") {
                                req.name = req_map.at("name").AsString();
                            }
                            else {
                                req.name = "";
                                if (req.type == "Route") {
                                    req.from_stop = req_map.at("from").AsString();
                                    req.to_stop = req_map.at("to").AsString();
                                }
                            }
                            if (req.type != "Route") {
                                req.from_stop = "";
                                req.to_stop = "";
                            }

                            stat_request.push_back(req);
                        }
                    }

                }
                else {
                    std::cout << "Error: base_requests is not array";
                }
            }


            void JSONReader::ParseNodeStop(TransportCatalogue& catalogue, Node& node) {

                geo::Coordinates coordinates;
                Dict stop_node;

                if (node.IsMap()) {

                    stop_node = node.AsMap();
                    coordinates.lat = stop_node.at("latitude").AsDouble();
                    coordinates.lng = stop_node.at("longitude").AsDouble();
                    catalogue.AddStop(stop_node.at("name").AsString(), coordinates);
                }
            }

            void JSONReader::ParseNodeDistances(TransportCatalogue& catalogue, Node& node) {
                Dict stop_node;
                Dict stop_road_map;
                std::string begin_name;
                std::string last_name;
                int distance;

                if (node.IsMap()) {
                    stop_node = node.AsMap();
                    begin_name = stop_node.at("name").AsString();

                    try {
                        stop_road_map = stop_node.at("road_distances").AsMap();

                        for (auto [key, value] : stop_road_map) {
                            last_name = key;
                            distance = value.AsInt();
                            catalogue.AddDistanceBetweenStops(begin_name, last_name, distance);
                        }
                    }

                    catch (...) {
                        std::cout << "Error: Road invalide" << std::endl;
                    }
                }
            }

            void JSONReader::ParseNodeBus(TransportCatalogue& catalogue, Node& node) {
                Bus bus;
                Dict bus_node;
                Array bus_stops;
                std::vector<std::string> stops;

                if (node.IsMap()) {
                    bus_node = node.AsMap();
                    std::string bus_name = bus_node.at("name").AsString();

                    bool circle = bus_node.at("is_roundtrip").AsBool();

                    try {
                        bus_stops = bus_node.at("stops").AsArray();
                        for (Node stop : bus_stops) {
                            stops.push_back(stop.AsString());
                        }
                        catalogue.AddBus(bus_name, stops, circle);
                    }
                    catch (...) {
                        std::cout << "Error: base_requests: bus: stops is empty" << std::endl;
                    }
                }
            }

            void JSONReader::ParseNodeRender(const Node& node, map_renderer::RenderSettings& render_settings) {
                Dict rend_map;
                Array bus_lab_offset;
                Array stop_lab_offset;
                Array arr_palette;

                if (node.IsMap()) {
                    rend_map = node.AsMap();

                    try {
                        render_settings.width_ = rend_map.at("width").AsDouble();
                        render_settings.height_ = rend_map.at("height").AsDouble();
                        render_settings.padding_ = rend_map.at("padding").AsDouble();
                        render_settings.line_width_ = rend_map.at("line_width").AsDouble();
                        render_settings.stop_radius_ = rend_map.at("stop_radius").AsDouble();
                        render_settings.bus_label_font_size_ = rend_map.at("bus_label_font_size").AsInt();

                        if (rend_map.at("bus_label_offset").IsArray()) {
                            bus_lab_offset = rend_map.at("bus_label_offset").AsArray();
                            render_settings.bus_label_offset_ = std::make_pair(bus_lab_offset[0].AsDouble(),
                                bus_lab_offset[1].AsDouble());
                        }

                        render_settings.stop_label_font_size_ = rend_map.at("stop_label_font_size").AsInt();

                        if (rend_map.at("stop_label_offset").IsArray()) {
                            stop_lab_offset = rend_map.at("stop_label_offset").AsArray();
                            render_settings.stop_label_offset_ = std::make_pair(stop_lab_offset[0].AsDouble(),
                                stop_lab_offset[1].AsDouble());
                        }

                        render_settings.underlayer_color_ = ParseNodeArrayColor(rend_map.at("underlayer_color"));
                        render_settings.underlayer_width_ = rend_map.at("underlayer_width").AsDouble();

                        if (rend_map.at("color_palette").IsArray()) {
                            arr_palette = rend_map.at("color_palette").AsArray();

                            for (Node color_palette : arr_palette) {
                                render_settings.color_palette_.push_back(ParseNodeArrayColor(color_palette));
                            }
                        }
                    }
                    catch (...) {
                        std::cout << "unable to parsse init settings";
                    }

                }
                else {
                    std::cout << "render_settings is not map";
                }
            }

            void JSONReader::ParseNodeRouteSetting(const Node& node, transport_router::details::RoutingSettings& routing_settings) {
                Dict rend_map;

                if (node.IsMap()) {
                    rend_map = node.AsMap();

                    try {
                        routing_settings.bus_wait_time_ = rend_map.at("bus_wait_time").AsDouble();
                        routing_settings.bus_velocity_ = rend_map.at("bus_velocity").AsDouble();
                    }
                    catch (...) {
                        std::cout << "unable to parsse init settings";
                    }

                }
                else {
                    std::cout << "routing_settings is not map";
                }
            }

            void JSONReader::ParseNodeSerialization(const Node& node, serialization::SerializationSettings& serialization_set) {

                Dict serialization;

                if (node.IsMap()) {
                    serialization = node.AsMap();

                    try {
                        serialization_set.file_name = serialization.at("file").AsString();

                    }
                    catch (...) {
                        std::cout << "unable to parse serialization settings";
                    }

                }
                else {
                    std::cout << "serialization settings is not map";
                }
            }

            svg::Color JSONReader::ParseNodeArrayColor(const Node& node) {
                double opacity_;

                if (node.IsString()) {
                    return svg::Color(node.AsString());
                }
                else if (node.IsArray()) {
                    Array arr_color = node.AsArray();
                    uint8_t red_ = arr_color[0].AsInt();
                    uint8_t green_ = arr_color[1].AsInt();
                    uint8_t blue_ = arr_color[2].AsInt();

                    if (arr_color.size() == 4) {
                        opacity_ = arr_color[3].AsDouble();
                        return svg::Color(svg::Rgba(red_,
                            green_,
                            blue_,
                            opacity_));
                    }
                    else if (arr_color.size() == 3) {
                        return svg::Color(svg::Rgb(red_,
                            green_,
                            blue_));
                    }
                    return {};
                }
                return {};
            }

            const Document& JSONReader::GetInDocument() const {
                return in_document_;
            }

            Document JSONReader::ExecuteQueries(const request_handler::RequestHandler& handler,std::vector<transport_catalogue::details::StatRequest>& stat_requests) {
                std::vector<Node> result_request;

                for (transport_catalogue::details::StatRequest& req : stat_requests) {

                    if (req.type == "Stop") {
                        result_request.push_back(ExecuteMakeNodeStop(req.id, handler.GetStopInfo(req.name)));

                    }
                    else if (req.type == "Bus") {
                        result_request.push_back(ExecuteMakeNodeBus(req.id, handler.GetBusStat(req.name)));
                    }
                    else if (req.type == "Map") {
                        result_request.push_back(ExecuteMakeNodeMap(req.id, handler.RenderMap()));
                    }
                    else if (req.type == "Route") {
                        result_request.push_back(ExecuteMakeNodeRoute(req.id, handler.GetRouteInfo(req.from_stop, req.to_stop)));
                    }
                }
                return Document{ Node{result_request} };
            }

            Node JSONReader::ExecuteMakeNodeStop(int id_request, const transport_catalogue::details::StopInfo& stop_info) {
                Array buses;
                Builder builder;

                if (stop_info.in_cataloge) {
                    builder
                        .StartDict()
                            .Key("request_id").Value(id_request)
                            .Key("buses").StartArray();

                    for (std::string bus_name : stop_info.buses) {
                        builder.Value(bus_name);
                    }

                    builder.EndArray().EndDict();
                }
                else {
                    builder
                        .StartDict()
                            .Key("request_id").Value(id_request)
                            .Key("error_message").Value("not found")
                        .EndDict();
                }
                return builder.Build();
            }

            Node JSONReader::ExecuteMakeNodeBus(int id_request, const transport_catalogue::details::BusInfo& bus_info) {
                if (bus_info.in_cataloge) {
                    return Builder{}
                            .StartDict()
                                .Key("request_id").Value(id_request)
                                .Key("curvature").Value(bus_info.curvature)
                                .Key("route_length").Value(bus_info.route_length)
                                .Key("stop_count").Value(bus_info.stops_on_route)
                                .Key("unique_stop_count").Value(bus_info.unique_stops)
                            .EndDict()
                        .Build()
                        ;
                }
                else {
                    return Builder{}
                            .StartDict()
                                .Key("request_id").Value(id_request)
                                .Key("error_message").Value("not found")
                            .EndDict()
                        .Build();
                }
            }

            Node JSONReader::ExecuteMakeNodeMap(int id_request,const svg::Document& map) {
                std::ostringstream map_stream;
                std::string map_str;
                map.Render(map_stream);
                map_str = map_stream.str();
                return Builder{}
                        .StartDict()
                            .Key("request_id").Value(id_request)
                            .Key("map").Value(map_str)
                        .EndDict()
                    .Build();
            }

            Node JSONReader::ExecuteMakeNodeRoute(int id_request, const std::optional<transport_router::details::RouteInfo>& route_info) {

                if (!route_info) {
                    return Builder{}
                            .StartDict()
                                .Key("request_id").Value(id_request)
                                .Key("error_message").Value("not found")
                            .EndDict()
                        .Build();
                }

                Array items;
                for (const auto& item : route_info->edges) {
                    items.emplace_back(std::visit(EdgeInfoGetter{}, item));
                }

                return Builder{}
                        .StartDict()
                            .Key("request_id").Value(id_request)
                            .Key("total_time").Value(route_info->total_time)
                            .Key("items").Value(items)
                        .EndDict()
                    .Build();
            }

           
            Node EdgeInfoGetter::operator()(const transport_router::details::StopEdge& edge_info) {
                using namespace std::literals;

                return Builder{}
                        .StartDict()
                            .Key("type").Value("Wait")
                            .Key("stop_name").Value(std::string(edge_info.name))
                            .Key("time").Value(edge_info.time)
                        .EndDict()
                    .Build();
            }

            Node EdgeInfoGetter::operator()(const transport_router::details::BusEdge& edge_info) {
                using namespace std::literals;

                return Builder{}
                        .StartDict()
                            .Key("type").Value("Bus")
                            .Key("bus").Value(std::string(edge_info.bus_name))
                            .Key("span_count").Value(static_cast<int>(edge_info.span_count))
                            .Key("time").Value(edge_info.time)
                        .EndDict()
                    .Build();
            }

        }
    }
}