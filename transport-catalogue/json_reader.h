#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "domain.h"
#include "map_renderer.h"
#include "request_handler.h"
 
namespace transport_catalogue {
    namespace details {
        namespace json {

            class JSONReader {
            public:
                JSONReader() = default;
                JSONReader(Document doc);
                JSONReader(std::istream& input);


                void Parse(TransportCatalogue& catalogue, std::vector<details::StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);
                Document ExecuteQueries(const request_handler::RequestHandler& handler, std::vector<details::StatRequest>& stat_requests);
                const Document& GetInDocument() const;

            private:
                Document in_document_;

                void ParseNode(const Node& root, TransportCatalogue& catalogue, std::vector<details::StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);
                void ParseNodeBase(const Node& root, TransportCatalogue& catalogue);
                void ParseNodeStat(const Node& node, std::vector<details::StatRequest>& stat_request);
                void ParseNodeRender(const Node& node, map_renderer::RenderSettings& render_settings);

                void ParseNodeStop(TransportCatalogue& catalogue, Node& node);
                void ParseNodeDistances(TransportCatalogue& catalogue, Node& node);
                void ParseNodeBus(TransportCatalogue& catalogue, Node& node);
                svg::Color ParseNodeArrayColor(const Node& node);

                Node ExecuteMakeNodeStop(int id_request, const transport_catalogue::details::StopInfo& stop_info);
                Node ExecuteMakeNodeBus(int id_request, const transport_catalogue::details::BusInfo& bus_info);
                Node ExecuteMakeNodeMap(int id_request,const svg::Document& map);
            };

        }
    }
}