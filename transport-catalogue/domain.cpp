#include "domain.h"

using namespace std::literals;

bool transport_catalogue::details::Stop::operator==(const Stop& rhs) const
{
    return stop_name == rhs.stop_name;
}
bool transport_catalogue::details::Stop::operator!=(const Stop& rhs) const
{
    return !(stop_name == rhs.stop_name);
}

transport_catalogue::details::Stop::Stop() {}

transport_catalogue::details::Stop::Stop(std::string name)
    :stop_name(name) {
}

bool transport_catalogue::details::Bus::operator==(const Bus& rhs) const
{
    return bus_name == rhs.bus_name;
}
bool transport_catalogue::details::Bus::operator!=(const Bus& rhs) const
{
    return !(bus_name == rhs.bus_name);
}


size_t transport_catalogue::details::StopsHasher::operator()(const std::pair<Stop*, Stop*>& stop_to_stop) const
{
    size_t hash_fist_stop = hasher_(stop_to_stop.first->stop_name);
    size_t hash_second_stop = hasher_(stop_to_stop.second->stop_name);
    return hash_fist_stop + hash_second_stop * 17;
}

void transport_catalogue::details::PrintStopInfo(const StopInfo& stop, std::ostream& out) {
    out << stop << std::endl;

}
void transport_catalogue::details::PrintBusInfo(const BusInfo& bus, std::ostream& out) {
    out << bus << std::endl;
}

std::ostream& operator<<(std::ostream& out, const transport_catalogue::details::StopInfo& stop_info)
{
    out << "Stop "s << stop_info.stop_name;
    if (!stop_info.in_cataloge) {
        out << ": not found"s;
        return out;
    }
    if (stop_info.buses.empty()) {
        out << ": no buses"s;
        return out;
    }
    out << ": buses"s;
    for (const auto& bus : stop_info.buses) {
        out << ' ' << bus;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const transport_catalogue::details::BusInfo& bus_info)
{
    out << "Bus "s << bus_info.bus_name;
    if (!bus_info.in_cataloge) {
        out << ": not found"s;
        return out;
    }
    out << ": "s << bus_info.stops_on_route << " stops on route, "s
        << bus_info.unique_stops << " unique stops, "s
        << bus_info.route_length << " route length, "s
        << bus_info.curvature << " curvature"s;
    return out;
}