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
        load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");

        //Initialize random number generators
        rng = std::minstd_rand(3);
        rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumberOfIntersections()-1);
        rand_street = std::uniform_int_distribution<unsigned>(1, getNumberOfStreets()-1);
        rand_segment = std::uniform_int_distribution<unsigned>(0, getNumberOfStreetSegments()-1);
        rand_poi = std::uniform_int_distribution<unsigned>(0, getNumberOfPointsOfInterest()-1);
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

SUITE(find_street_ids_from_names_toronto_canada) {
     //Checks the Functionality of the function find_street_ids_from_name
    //Part 1 takes the indices returned by the function and checks if each
    //one has the same name. Part 2 check if the function is returning the 
    //right number of indicies by checking how many times the name shows up
    //with the API call
    TEST_FIXTURE(MapFixture, func_find_street_ids_from_name_random_numbers_part1) {
        
        std::vector<unsigned> Index_from_Function;
        std::string StreetName;
        std::vector<std::string> Test_Street_Names;
        
        for(size_t i = 0; i < 1000; i++) {
            Test_Street_Names.push_back(getStreetName(rand_street(rng) % getNumberOfStreets()));
        }

        for (size_t k = 0; k < 1000; k++) {
            bool allmatch = true;
            Index_from_Function = find_street_ids_from_name(Test_Street_Names[k]); //There has to be an output of at least 1
            StreetName = getStreetName(Index_from_Function[0]);
            if (Index_from_Function.size() > 1) {
                for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                    if (getStreetName(Index_from_Function[i]) != StreetName) {
                        allmatch = false;
                    }
                }
            }

            CHECK(allmatch);
        }
        Test_Street_Names.clear();
        
    }
    
    TEST_FIXTURE(MapFixture, func_find_street_ids_from_name_random_numbers_part2) {
        
        int expected = 0;
        std::vector<unsigned> Index_from_Function;
        std::vector<std::string> Test_Street_Names;
        
        for(size_t i = 0; i < 1000; i++) {
            Test_Street_Names.push_back(getStreetName(rand_street(rng) % getNumberOfStreets()));
        }

        for (size_t k = 0; k < 1000; k++) {
           expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(2));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(2)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());
        }
        
        Test_Street_Names.clear();
    }
}