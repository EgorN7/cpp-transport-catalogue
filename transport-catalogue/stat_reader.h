#pragma once
#include "transport_catalogue.h"
#include<iostream>
 
namespace StatReader {
	void Output(TransportCatalogue::TransportCatalogue& transport_catalogue, std::istream& in, std::ostream& out);
}
