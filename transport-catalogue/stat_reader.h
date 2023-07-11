#pragma once
#include "transport_catalogue.h"
#include<iostream>
 
namespace StatReader {

	void Output(const TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in, std::ostream& out);
	void PrintStopInfo(const TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& find_stop_name, std::ostream& out);
	void PrintBusInfo(const TransportCatalogue::TransportCatalogue& transport_catalogue, const std::string& find_bus_name, std::ostream& out);
}

std::ostream& operator<< (std::ostream& out, const TransportCatalogue::details::StopInfo& stop_info);
std::ostream& operator<< (std::ostream& out, const TransportCatalogue::details::BusInfo& bus_info);