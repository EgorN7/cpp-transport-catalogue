#include<iostream>

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
//#include "log_duration.h"


int main()
{
    TransportCatalogue::TransportCatalogue transport_catalogue;

    InputReader::Parse(transport_catalogue, std::cin);
    StatReader::Output(transport_catalogue, std::cin, std::cout);

    return 0;
}