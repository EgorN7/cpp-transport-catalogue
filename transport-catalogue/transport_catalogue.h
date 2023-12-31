#pragma once
#include<deque>
#include<unordered_map>
#include<string_view>
#include<map>

#include "domain.h"

 
namespace transport_catalogue {
    using namespace std::literals;

    class TransportCatalogue
    {
    public:


        void AddStop(const std::string& name, const geo::Coordinates& coordinates);
        void AddStop(details::Stop&& stop);
        void AddBus(const std::string& bus_name, const std::vector<std::string>& stops, bool circle);
        void AddBus(details::Bus&& bus);
        void AddDistanceBetweenStops(const std::string& first_stop, const std::string& second_stop, int distance_between_stops);

        details::Stop* FindStop(std::string_view find_name) const;
        details::Bus* FindBus(std::string_view find_name) const;
        std::unordered_map<std::string_view, details::Bus*> GetBusnameToBus() const;
        std::unordered_map<std::string_view, details::Stop*> GetStopnameToStop() const;

        details::StopInfo SearchStop(const std::string& find_stop_name) const;
        details::BusInfo GetBusInfo(const std::string& find_bus_name) const;

        std::vector<geo::Coordinates> GetStopsCoordinates() const;
        std::vector<std::string_view> GetSortBusesNames() const;

        int GetDistanceBetweenTwoStops(details::Stop* first_stop, details::Stop* second_stop) const;

        std::deque<details::Stop> GetStops() const;
        std::deque<details::Bus> GetBuses() const;
        std::unordered_map<std::pair<details::Stop*, details::Stop*>, int, details::StopsHasher> GetDistance() const;

    private:

        std::deque<details::Stop> stops_;

        std::deque<details::Bus> buses_;

        std::unordered_map<std::string_view, details::Stop*>  stopname_to_stop_;

        std::unordered_map<std::string_view, details::Bus*>  busname_to_bus_;

        std::unordered_map<std::pair<details::Stop*, details::Stop*>, int, details::StopsHasher>  distance_;

        
        details::BusRouteDistance GetBusRouteDistance(details::Bus* bus) const;
    };
}