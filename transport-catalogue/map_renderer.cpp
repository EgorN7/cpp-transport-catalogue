#include "map_renderer.h"

namespace map_renderer {

    bool SphereProjector::IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    MapRenderer::MapRenderer(RenderSettings& render_settings) : render_settings_(render_settings) {}

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
    }

    SphereProjector MapRenderer::GetSphereProjector(const std::vector<geo::Coordinates>& points) const {
        return SphereProjector(points.begin(),
            points.end(),
            render_settings_.width_,
            render_settings_.height_,
            render_settings_.padding_);
    }

    void MapRenderer::FillMap(const transport_catalogue::TransportCatalogue& catalogue) {
        InitSphereProjector(catalogue.GetStopsCoordinates());
        std::vector<std::pair<transport_catalogue::details::Bus*, int>> buses_palette;
        std::vector<transport_catalogue::details::Stop*> stops_sort;
        int palette_size = 0;
        int palette_index = 0;

        palette_size = GetPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return;
        }

        auto buses = catalogue.GetSortBusesNames();

        if (buses.size() > 0) {

            for (auto& bus_name : buses) {
                auto busptr = catalogue.FindBus(std::string(bus_name));
                if (busptr->single_marshrut.size() > 0) {
                    buses_palette.push_back(std::make_pair(busptr, palette_index));
                    palette_index++;

                    if (palette_index == palette_size) {
                        palette_index = 0;
                    }
                }
            }

            if (buses_palette.size() > 0) {
                AddLine(buses_palette);
                AddBusesName(buses_palette);
            }
        }

        auto stops = catalogue.GetStopnameToStop();
        if (stops.size() > 0) {
            std::vector<std::string_view> stops_name;

            for (auto& [stop_name, stop] : stops) {

                if (catalogue.SearchStop(std::string(stop_name)).buses.size() > 0) {
                    stops_name.push_back(stop_name);
                }
            }

            std::sort(stops_name.begin(), stops_name.end());

            for (std::string_view stop_name : stops_name) {
                transport_catalogue::details::Stop* stop = catalogue.FindStop(stop_name);
                if (stop) {
                    stops_sort.push_back(stop);
                }
            }

            if (stops_sort.size() > 0) {
                AddStopsCircle(stops_sort);
                AddStopsName(stops_sort);
            }
        }
    }

    void MapRenderer::InitSphereProjector(std::vector<geo::Coordinates> points) {
        sphere_projector_ = SphereProjector(points.begin(),
            points.end(),
            render_settings_.width_,
            render_settings_.height_,
            render_settings_.padding_);
    }

    RenderSettings MapRenderer::GetRenderSettings() const {
        return render_settings_;
    }

    int MapRenderer::GetPaletteSize() const {
        return render_settings_.color_palette_.size();
    }

    svg::Color MapRenderer::GetColor(int line_number) const {
        return render_settings_.color_palette_[line_number];
    }

    void MapRenderer::SetLineProperties(svg::Polyline& polyline,
        [[maybe_unused]] int line_number) const {
        using namespace std::literals;

        polyline.SetStrokeColor(GetColor(line_number));
        polyline.SetFillColor("none"s);
        polyline.SetStrokeWidth(render_settings_.line_width_);
        polyline.SetStrokeLinecap(svg::StrokeLineCap::ROUND);
        polyline.SetStrokeLinejoin(svg::StrokeLineJoin::ROUND);
    }

    void MapRenderer::SetRouteTextCommonProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {
        using namespace std::literals;

        text.SetPosition(position);
        text.SetOffset({ render_settings_.bus_label_offset_.first,
                         render_settings_.bus_label_offset_.second });
        text.SetFontSize(render_settings_.bus_label_font_size_);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(name);
    }

    void MapRenderer::SetRouteTextAdditionalProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {
        SetRouteTextCommonProperties(text,
            name,
            position);

        text.SetFillColor(render_settings_.underlayer_color_);
        text.SetStrokeColor(render_settings_.underlayer_color_);
        text.SetStrokeWidth(render_settings_.underlayer_width_);
        text.SetStrokeLinejoin(svg::StrokeLineJoin::ROUND);
        text.SetStrokeLinecap(svg::StrokeLineCap::ROUND);
    }

    void MapRenderer::SetRouteTextColorProperties(svg::Text& text,
        const std::string& name,
        int palette,
        svg::Point position) const {
        SetRouteTextCommonProperties(text,
            name,
            position);

        text.SetFillColor(GetColor(palette));
    }

    void MapRenderer::SetStopsCirclesProperties(svg::Circle& circle,
        svg::Point position) const {
        using namespace std::literals;

        circle.SetCenter(position);
        circle.SetRadius(render_settings_.stop_radius_);
        circle.SetFillColor("white");
    }

    void MapRenderer::SetStopsTextCommonProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {
        using namespace std::literals;

        text.SetPosition(position);
        text.SetOffset({ render_settings_.stop_label_offset_.first,
                         render_settings_.stop_label_offset_.second });
        text.SetFontSize(render_settings_.stop_label_font_size_);
        text.SetFontFamily("Verdana");
        text.SetData(name);
    }

    void MapRenderer::SetStopsTextAdditionalProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {
        using namespace std::literals;
        SetStopsTextCommonProperties(text,
            name,
            position);

        text.SetFillColor(render_settings_.underlayer_color_);
        text.SetStrokeColor(render_settings_.underlayer_color_);
        text.SetStrokeWidth(render_settings_.underlayer_width_);
        text.SetStrokeLinejoin(svg::StrokeLineJoin::ROUND);
        text.SetStrokeLinecap(svg::StrokeLineCap::ROUND);
    }

    void MapRenderer::SetStopsTextColorProperties(svg::Text& text,
        const std::string& name,
        svg::Point position) const {
        using namespace std::literals;

        SetStopsTextCommonProperties(text,
            name,
            position);
        text.SetFillColor("black");
    }

    void MapRenderer::AddLine(std::vector<std::pair<transport_catalogue::details::Bus*, int>>& buses_palette) {
        std::vector<geo::Coordinates> stops_geo_coords;

        for (auto [bus, palette] : buses_palette) {

            for (transport_catalogue::details::Stop* stop : bus->single_marshrut) {
                geo::Coordinates coordinates;
                coordinates.lat = stop->coords.lat;
                coordinates.lng = stop->coords.lng;

                stops_geo_coords.push_back(coordinates);
            }

            svg::Polyline bus_line;
            bool bus_empty = true;

            for (auto& coord : stops_geo_coords) {
                bus_empty = false;
                bus_line.AddPoint(sphere_projector_(coord));
            }

            if (!bus_empty) {
                SetLineProperties(bus_line,
                    palette);
                map_svg_.Add(bus_line);
            }

            stops_geo_coords.clear();
        }
    }

    void MapRenderer::AddBusesName(std::vector<std::pair<transport_catalogue::details::Bus*, int>>& buses_palette) {
        std::vector<geo::Coordinates> stops_geo_coords;
        bool bus_empty = true;

        for (auto [bus, palette] : buses_palette) {

            for (transport_catalogue::details::Stop* stop : bus->single_marshrut) {
                geo::Coordinates coordinates;
                coordinates.lat = stop->coords.lat;
                coordinates.lng = stop->coords.lng;

                stops_geo_coords.push_back(coordinates);

                if (bus_empty) bus_empty = false;
            }

            svg::Text route_name_roundtrip;
            svg::Text route_title_roundtrip;
            svg::Text route_name_notroundtrip;
            svg::Text route_title_notroundtrip;

            if (!bus_empty) {

                SetRouteTextAdditionalProperties(route_name_roundtrip,
                    std::string(bus->bus_name),
                    sphere_projector_(stops_geo_coords[0]));
                map_svg_.Add(route_name_roundtrip);

                SetRouteTextColorProperties(route_title_roundtrip,
                    std::string(bus->bus_name),
                    palette,
                    sphere_projector_(stops_geo_coords[0]));
                map_svg_.Add(route_title_roundtrip);

                if (stops_geo_coords[0] != stops_geo_coords[stops_geo_coords.size() / 2] && !(bus->circle)) {
                    SetRouteTextAdditionalProperties(route_name_notroundtrip,
                        std::string(bus->bus_name),
                        sphere_projector_(stops_geo_coords[stops_geo_coords.size() / 2]));
                    map_svg_.Add(route_name_notroundtrip);

                    SetRouteTextColorProperties(route_title_notroundtrip,
                        std::string(bus->bus_name),
                        palette,
                        sphere_projector_(stops_geo_coords[stops_geo_coords.size() / 2]));
                    map_svg_.Add(route_title_notroundtrip);
                }
            }

            bus_empty = false;
            stops_geo_coords.clear();
        }
    }

    void MapRenderer::AddStopsCircle(std::vector<transport_catalogue::details::Stop*>& stops) {
        std::vector<geo::Coordinates> stops_geo_coords;
        svg::Circle icon;

        for (transport_catalogue::details::Stop* stop_info : stops) {

            if (stop_info) {
                geo::Coordinates coordinates;
                coordinates.lat = stop_info->coords.lat;
                coordinates.lng = stop_info->coords.lng;

                SetStopsCirclesProperties(icon,
                    sphere_projector_(coordinates));
                map_svg_.Add(icon);
            }
        }
    }

    void MapRenderer::AddStopsName(std::vector<transport_catalogue::details::Stop*>& stops) {
        std::vector<geo::Coordinates> stops_geo_coords;

        svg::Text svg_stop_name;
        svg::Text svg_stop_name_title;

        for (transport_catalogue::details::Stop* stop_info : stops) {

            if (stop_info) {
                geo::Coordinates coordinates;
                coordinates.lat = stop_info->coords.lat;
                coordinates.lng = stop_info->coords.lng;

                SetStopsTextAdditionalProperties(svg_stop_name,
                    stop_info->stop_name,
                    sphere_projector_(coordinates));
                map_svg_.Add(svg_stop_name);

                SetStopsTextColorProperties(svg_stop_name_title,
                    stop_info->stop_name,
                    sphere_projector_(coordinates));
                map_svg_.Add(svg_stop_name_title);
            }
        }
    }

    const svg::Document& MapRenderer::GetRanderMap() const {
        return map_svg_;
    }   
}