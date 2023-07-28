#include "json_reader.h"

using Stop = transport_catalogue::details::Stop;
using Bus = transport_catalogue::details::Bus;
 
namespace transport_catalogue {
    namespace details {
        namespace json {

            JSONReader::JSONReader(Document doc) : in_document_(std::move(doc)) {}
            JSONReader::JSONReader(std::istream& input) : in_document_(json::Load(input)) {}

            void JSONReader::Parse(transport_catalogue::TransportCatalogue& catalogue, std::vector<details::StatRequest>& stat_request, map_renderer::RenderSettings& render_settings) {
                ParseNode(in_document_.GetRoot(), catalogue, stat_request, render_settings);
            }

            void JSONReader::ParseNode(const Node& root, TransportCatalogue& catalogue, std::vector<details::StatRequest>& stat_request, map_renderer::RenderSettings& render_settings) {
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
                        ParseNodeStat(root_dict.at("stat_requests"), stat_request);
                    }
                    catch (...) {
                        std::cout << "stat_requests is empty";
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

                            if (req.type != "Map") {
                                req.name = req_map.at("name").AsString();
                            }
                            else {
                                req.name = "";
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
                Array arr_color;
                Array arr_palette;
                uint8_t red_;
                uint8_t green_;
                uint8_t blue_;
                double opacity_;

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

                        if (rend_map.at("underlayer_color").IsString()) {
                            render_settings.underlayer_color_ = svg::Color(rend_map.at("underlayer_color").AsString());
                        }
                        else if (rend_map.at("underlayer_color").IsArray()) {
                            arr_color = rend_map.at("underlayer_color").AsArray();
                            red_ = arr_color[0].AsInt();
                            green_ = arr_color[1].AsInt();
                            blue_ = arr_color[2].AsInt();

                            if (arr_color.size() == 4) {
                                opacity_ = arr_color[3].AsDouble();
                                render_settings.underlayer_color_ = svg::Color(svg::Rgba(red_,
                                    green_,
                                    blue_,
                                    opacity_));
                            }
                            else if (arr_color.size() == 3) {
                                render_settings.underlayer_color_ = svg::Color(svg::Rgb(red_,
                                    green_,
                                    blue_));
                            }

                        }

                        render_settings.underlayer_width_ = rend_map.at("underlayer_width").AsDouble();

                        if (rend_map.at("color_palette").IsArray()) {
                            arr_palette = rend_map.at("color_palette").AsArray();

                            for (Node color_palette : arr_palette) {

                                if (color_palette.IsString()) {
                                    render_settings.color_palette_.push_back(svg::Color(color_palette.AsString()));
                                }
                                else if (color_palette.IsArray()) {
                                    arr_color = color_palette.AsArray();
                                    red_ = arr_color[0].AsInt();
                                    green_ = arr_color[1].AsInt();
                                    blue_ = arr_color[2].AsInt();

                                    if (arr_color.size() == 4) {
                                        opacity_ = arr_color[3].AsDouble();
                                        render_settings.color_palette_.push_back(svg::Color(svg::Rgba(red_,
                                            green_,
                                            blue_,
                                            opacity_)));
                                    }
                                    else if (arr_color.size() == 3) {
                                        render_settings.color_palette_.push_back(svg::Color(svg::Rgb(red_,
                                            green_,
                                            blue_)));
                                    }
                                }
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

            const Document& JSONReader::GetInDocument() const {
                return in_document_;
            }

        }
    }
}