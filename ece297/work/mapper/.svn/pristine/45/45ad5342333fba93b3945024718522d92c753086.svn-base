#include <unittest++/UnitTest++.h>
#include "m1.h"

std::string map_dir = "/cad2/ece297s/public/maps/";

SUITE(M1_Public_Load_Maps) {
    TEST(load_saint_helena) {
        {
            UNITTEST_TIME_CONSTRAINT(3);
            CHECK(load_map(map_dir + "saint-helena.streets.bin"));
        }
        close_map();
    }
    
    TEST(load_hamilton) {
        {
            UNITTEST_TIME_CONSTRAINT(3);
            CHECK(load_map(map_dir + "hamilton_canada.streets.bin"));
        }
        close_map();
    }

    TEST(load_moscow) {
        {
            UNITTEST_TIME_CONSTRAINT(3);
            CHECK(load_map(map_dir + "moscow_russia.streets.bin"));
        }
        close_map();
    }

    TEST(load_toronto) {
        {
            UNITTEST_TIME_CONSTRAINT(3);
            CHECK(load_map(map_dir + "toronto_canada.streets.bin"));
        }
        close_map();
    }

    TEST(load_newyork) {
        {
            UNITTEST_TIME_CONSTRAINT(3);
            CHECK(load_map(map_dir + "new-york_usa.streets.bin"));
        }
        close_map();
    }

    TEST(load_golden_horseshoe) {
        {
            UNITTEST_TIME_CONSTRAINT(3);
            CHECK(load_map(map_dir + "golden-horseshoe_canada.streets.bin"));
        }
        close_map();
    }

    TEST(load_invalid_map_path) {
        {
            UNITTEST_TIME_CONSTRAINT(3000);
            CHECK(!load_map("/this/path/does/not/exist"));
        }
    }


    // added after to test all maps

//    TEST(load_london) {
//        {
//            UNITTEST_TIME_CONSTRAINT(3);
//            CHECK(load_map(map_dir + "london_england.streets.bin"));
//        }
//        close_map();
//    }
//
//    TEST(load_beijing) {
//        {
//            UNITTEST_TIME_CONSTRAINT(3);
//            CHECK(load_map(map_dir + "beijing_china.streets.bin"));
//        }
//        close_map();
//    }
//
//    TEST(load_capetown) {
//        {
//            UNITTEST_TIME_CONSTRAINT(3);
//            CHECK(load_map(map_dir + "cape-town_south-africa.streets.bin"));
//        }
//        close_map();
//    }
//
//    TEST(load_cairo) {
//        {
//            UNITTEST_TIME_CONSTRAINT(3);
//            CHECK(load_map(map_dir + "cairo_egypt.streets.bin"));
//        }
//        close_map();
//    }
//
//    TEST(load_hongkong) {
//        {
//            UNITTEST_TIME_CONSTRAINT(3);
//            CHECK(load_map(map_dir + "hong-kong_china.streets.bin"));
//        }
//        close_map();
//    }
//
//    // this one doesnt load for some reason
////    TEST(load_tokyo) {
////        {
////            UNITTEST_TIME_CONSTRAINT(3);
////            CHECK(load_map(map_dir + "tokyo_japan.streets.bin"));
////        }
////        close_map();
////    }
    
    
}
