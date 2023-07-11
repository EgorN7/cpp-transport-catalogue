#pragma once
#include<iostream>
#include<string>
#include <vector>
#include "transport_catalogue.h"

namespace InputReader {
	void ParseStop(TransportCatalogue::TransportCatalogue& transport_catalogue,const std::string& str);
 
	void ParseBus(TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& request_bus);
 
	void Parse(TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in);

	std::pair<std::string, int> ParseStopDistances(const std::string& str_distances);
}