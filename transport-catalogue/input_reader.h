#pragma once
#include<iostream>
#include<string>
#include <vector>
#include "transport_catalogue.h"

namespace InputReader {
	void ParseStop(TransportCatalogue::TransportCatalogue& TT,const std::string& str);
 
	void ParseBus(TransportCatalogue::TransportCatalogue& TT, const std::string& request_bus);
 
	void Parse(TransportCatalogue::TransportCatalogue& TT, std::istream& in);

	std::pair<std::string, int> ParseStopDistances(const std::string& str_distances);
}