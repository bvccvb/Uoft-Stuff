#include <algorithm>
#include <set>
#include <random>
#include <unittest++/UnitTest++.h>
#include "unit_test_util.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;

struct MapFixture {

    MapFixture() {
        //Load the map
        load_map("/cad2/ece297s/public/maps/saint-helena.streets.bin");

        //Initialize random number generators
        rng = std::minstd_rand(3);
        rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumberOfIntersections() - 1);
        rand_street = std::uniform_int_distribution<unsigned>(1, getNumberOfStreets() - 1);
        rand_segment = std::uniform_int_distribution<unsigned>(0, getNumberOfStreetSegments() - 1);
        rand_poi = std::uniform_int_distribution<unsigned>(0, getNumberOfPointsOfInterest() - 1);
        rand_lat = std::uniform_real_distribution<double>(43.48, 43.91998);
        rand_lon = std::uniform_real_distribution<double>(-79.78998, -79.00001);
    }

    ~MapFixture() {
        //Clean-up
        close_map();
    }

    std::minstd_rand rng;
    std::uniform_int_distribution<unsigned> rand_intersection;
    std::uniform_int_distribution<unsigned> rand_street;
    std::uniform_int_distribution<unsigned> rand_segment;
    std::uniform_int_distribution<unsigned> rand_poi;
    std::uniform_real_distribution<double> rand_lat;
    std::uniform_real_distribution<double> rand_lon;
};

SUITE(find_street_ids_from_names_saint_helena_perf) {
    //runs the function 1000000 and checks if it is below 250ms.

    TEST_FIXTURE(MapFixture, find_street_ids_from_name_perf) {

        std::vector<std::string> Test_Street_Names;

        for (size_t i = 0; i < 1000000; i++) {
            Test_Street_Names.push_back(getStreetName(rand_street(rng) % getNumberOfStreets()));
        }
        {
            //Timed Test
            ECE297_TIME_CONSTRAINT(250);
            std::vector<unsigned> result;
            for (size_t i = 0; i < 1000000; i++) {
                result = find_street_ids_from_name(Test_Street_Names[i]);
            }
        }
    }
}