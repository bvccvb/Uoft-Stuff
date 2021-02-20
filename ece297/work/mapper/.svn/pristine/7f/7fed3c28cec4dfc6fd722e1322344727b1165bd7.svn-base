#include <algorithm>
#include <set>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "unit_test_util.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;

struct MapFixture {

    MapFixture() {
        //Load the map
        load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
    }

    ~MapFixture() {
        //Clean-up
        close_map();
    }
};

SUITE(find_street_ids_from_names_toronto_canada) {

    TEST_FIXTURE(MapFixture, street_name_does_not_exist) {
        //checks if the funrction returns an empty vector if a invalid name is given
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;

        expected = {};
        actual = find_street_ids_from_name("sdfasfasd");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {};
        actual = find_street_ids_from_name("sq#%@!@w");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {};
        actual = find_street_ids_from_name("0");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {};
        actual = find_street_ids_from_name(" ");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {};
        actual = find_street_ids_from_name("%$#");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

    }

    TEST_FIXTURE(MapFixture, functionality_part1) {
        //checks if the function outputs the proper street ID vectors
        //in m1_func will check the same thing but 1000 times with random number
        //check if the index returns all have the same name;
        std::vector<unsigned> Index_from_Function;
        std::string StreetName;

        bool allmatch = true;
        Index_from_Function = find_street_ids_from_name(getStreetName(1)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }
        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(2)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(3)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(4)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(5)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(6)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(7)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(8)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(9)); //There has to be an output of at least 1
        StreetName = getStreetName(Index_from_Function[0]);
        if (Index_from_Function.size() > 1) {
            for (unsigned i = 0; i < Index_from_Function.size(); i++) {
                if (getStreetName(Index_from_Function[i]) != StreetName) {
                    allmatch = false;
                }
            }
        }

        CHECK(allmatch);

        allmatch = true;
        Index_from_Function.clear();
        Index_from_Function = find_street_ids_from_name(getStreetName(10)); //There has to be an output of at least 1
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

    TEST_FIXTURE(MapFixture, functionality_part2) {
        //Checks if the the index is correct. Outputs the right number of street indicies because
        //part 1 does not confirm whether or not the write AMOUNT of indices is being produced

        int expected = 0;
        std::vector<unsigned> Index_from_Function;

        Index_from_Function = find_street_ids_from_name(getStreetName(1));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(1)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(2));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(2)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(3));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(3)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(4));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(4)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(5));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(5)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(6));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(6)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(7));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(7)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(8));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(8)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(9));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(9)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

        expected = 0;
        Index_from_Function = find_street_ids_from_name(getStreetName(10));

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            if (getStreetName(i) == getStreetName(10)) {
                expected++;
            }
        }
        CHECK_EQUAL(expected, Index_from_Function.size());

    }
} //closest_poi

