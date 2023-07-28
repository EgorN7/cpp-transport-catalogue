#pragma once
#include <iostream>
#include <optional>
#include <algorithm>
#include <cstdlib>

#include "domain.h"
#include "geo.h"
#include "svg.h"

namespace map_renderer {

    inline const double EPSILON = 1e-6;

    class SphereProjector {
    public:
        SphereProjector() = default;

        template <typename InputIt>
        SphereProjector(InputIt points_begin,
            InputIt points_end,
            double max_width,
            double max_height,
            double padding);

        svg::Point operator()(geo::Coordinates coords) const;

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;

        bool IsZero(double value);
    };

    struct RenderSettings {
        double width_;
        double height_;
        double padding_;
        double line_width_;
        double stop_radius_;
        int bus_label_font_size_;
        std::pair<double, double> bus_label_offset_;
        int stop_label_font_size_;
        std::pair<double, double> stop_label_offset_;
        svg::Color underlayer_color_;
        double underlayer_width_;
        std::vector<svg::Color> color_palette_;
    };

    class MapRenderer {

    public:
        MapRenderer(RenderSettings& render_settings);

        void InitSphereProjector(std::vector<geo::Coordinates> points);
        int GetPaletteSize() const;
        void GetStreamMap(std::ostream& stream_);

        void AddLine(std::vector<std::pair<transport_catalogue::details::Bus*, int>>& buses_palette);
        void AddBusesName(std::vector<std::pair<transport_catalogue::details::Bus*, int>>& buses_palette);
        void AddStopsCircle(std::vector<transport_catalogue::details::Stop*>& stops_name);
        void AddStopsName(std::vector<transport_catalogue::details::Stop*>& stops_name);

    private:
        SphereProjector sphere_projector_;
        RenderSettings& render_settings_;
        svg::Document map_svg_;

        SphereProjector GetSphereProjector(const std::vector<geo::Coordinates>& points) const;
        RenderSettings GetRenderSettings() const;
        svg::Color GetColor(int line_number) const;
        void SetLineProperties(svg::Polyline& polyline, int line_number) const;
        void SetRouteTextCommonProperties(svg::Text& text, const std::string& name, svg::Point position) const;
        void SetRouteTextAdditionalProperties(svg::Text& text, const std::string& name, svg::Point position) const;
        void SetRouteTextColorProperties(svg::Text& text, const std::string& name, int palette, svg::Point position) const;
        void SetStopsCirclesProperties(svg::Circle& circle, svg::Point position) const;
        void SetStopsTextCommonProperties(svg::Text& text, const std::string& name, svg::Point position) const;
        void SetStopsTextAdditionalProperties(svg::Text& text, const std::string& name, svg::Point position) const;
        void SetStopsTextColorProperties(svg::Text& text, const std::string& name, svg::Point position) const;
    };

    template <typename InputIt>
    SphereProjector::SphereProjector(InputIt points_begin,
        InputIt points_end,
        double max_width,
        double max_height,
        double padding) : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it,
            right_it] = std::minmax_element(points_begin,
                points_end,
                [](auto lhs, auto rhs) {
                    return lhs.lng < rhs.lng;
                });

        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it,
            top_it] = std::minmax_element(points_begin,
                points_end,
                [](auto lhs, auto rhs) {
                    return lhs.lat < rhs.lat;
                });

        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!is_zero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding)
                / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!is_zero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding)
                / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom,
                *height_zoom);
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;

        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;

        }
    }

}