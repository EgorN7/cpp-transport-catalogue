#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "transport_catalogue.h"

namespace InputReader {
	void ParseStop(TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& str
		, std::vector<std::pair<std::string, std::string>>& distance_between_stops);
 
	void ParseBus(TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& request_bus);
 
	void Parse(TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in);

	void ParseStopDistances(TransportCatalogue::TransportCatalogue& transport_catalogue
		, const std::string& first_stop, const std::string& str_distances);
}