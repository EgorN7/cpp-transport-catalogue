#pragma once
#include "transport_catalogue.h"
#include<iostream>
 
namespace StatReader {
	void Output(TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in, std::ostream& out);
}

std::ostream& operator<< (std::ostream& out, const TransportCatalogue::details::StopInfo& stop_info);
std::ostream& operator<< (std::ostream& out, const TransportCatalogue::details::BusInfo& bus_info);
