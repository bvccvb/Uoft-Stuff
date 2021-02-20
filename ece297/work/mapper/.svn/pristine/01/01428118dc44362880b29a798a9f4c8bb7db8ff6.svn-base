/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   map_data_structure.h
 * Author: kostivna
 *
 * Created on February 20, 2018, 4:04 PM
 */

#ifndef MAP_DATA_STRUCTURE_H
#define MAP_DATA_STRUCTURE_H

#include <cmath>
#include <algorithm>
#include <unordered_map>
#include "StreetsDatabaseAPI.h"
#include "m1.h"
//added for rtree
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <functional>
#include <vector>
#include <thread>
//added for rtree
namespace bg = boost::geometry; // lets us use bg::  instead of  boost::geometry::
namespace bgi = boost::geometry::index; // lets us use bgi::  instead of  boost::geometry::index::


class map_data_structure {

    private:
        //structures are private members and can only be accessed by get functions in public
        std::vector <std::vector<unsigned>> street_street_segments;
        std::vector <float> street_segment_travel_time;
        std::vector <float> street_segment_lengths;
        std::vector <float> street_segment_speed;
        std::vector <float> street_seg_id;
        std::vector <float> street_lengths;
        std::vector< std::vector<unsigned> > street_ids_and_intersections;
        std::unordered_map<std::string, std::vector<unsigned>> street_names_to_ids;
        std::vector <std::vector <unsigned> > directly_connected_intersections;
        std::vector <std::vector <unsigned> > intersection_street_segments;
        
        // for rtree 
        typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> pointIDx;
        typedef std::pair<pointIDx, unsigned> value;
        bgi::rtree<value, bgi::linear<32> > r_tree_poi; // creates the tree for poi
        bgi::rtree<value, bgi::linear<32> > r_tree_intersection; // creates the tree for intersections


    public:
        //Public Class Functions
        
        //Constructor
        map_data_structure();

        //deletes map data structures;
        ~map_data_structure();

        // Loads the rtree and stores the points of interest values in it
        void load_rtree_poi();

        // Loads the rtree and stores the intersection values in it
        void load_rtree_intersection();

        //Returns street length
        double get_street_length(unsigned street_id);

        //Pre-loads the street length
        void load_street_lengths();

        //Data structure for segment speed and ID, doing this separately improves speed and performance
        //when loading some of the vectors, we realized the API calls slowed down speed. We did the API
        //calls separately and found it ran faster
        void load_segment_speed_and_IDs();

        //Functions concerning the loading and retrieval of street segments data (street segment lengths, 
        //street segment travel time and the street segments of a street)
        float get_street_segment_lengths(unsigned street_segment_id);

        //Returns travel time given a street segment id
        float get_street_segment_travel_time(unsigned street_segment_id);

        //Returns a vector of street segments for a given street_id
        std::vector<unsigned> get_street_street_segments(unsigned street_id);

        //Loads street segment street segment lengths, street segment travel time and the street segments of a street
        void load_street_segment_lengths_travel_time_street_ids_and_segments();    

        //Loads data structure for street ids to intersections (unordered map)
        void load_street_ids_and_intersections();

        //Returns a vector of intersections for a given street id
        std::vector<unsigned> get_street_ids_and_intersections(unsigned street_id);

        //Loads data structure for street names to ids (vector)
        void load_street_names_to_ids();

        //Returns a vector of street ids with a given name
        std::vector<unsigned> get_street_names_to_ids(std::string name);

        //Loads data structure for directly connected intersections and intersection street segments 
        void load_directly_connected_intersections_and_intersection_street_segments();

        //Returns a vector of street segments for a given intersection id
        std::vector<unsigned> get_intersection_street_segments(unsigned intersection_id);

        //Returns a vector of directly connected intersections for a given intersection id
        std::vector<unsigned> get_directly_connected_intersections(unsigned intersection_id);

        //Returns the closest POI
        unsigned closest_poi(LatLon my_position);

        //Returns the closest intersection
        unsigned closest_intersection(LatLon my_position);
        
        //Points of interest data structures
        std::vector <POIIndex> restaurant_pois;
        std::vector <POIIndex> cafe_pois;
        std::vector <POIIndex> fast_food_pois;
        std::vector <POIIndex> food_pois;
        std::vector <POIIndex> drinking_pois;
        std::vector <POIIndex> fuel_pois;
        std::vector <POIIndex> school_pois;
        std::vector <POIIndex> top_schools_pois;
        std::vector <POIIndex> health_pois;
        std::vector <POIIndex> hospitals_pois;
        std::vector <POIIndex> bank_pois;
        std::vector <POIIndex> church_pois;
        std::vector <POIIndex> emergency_pois;
        std::vector <POIIndex> cinema_pois;
        std::vector <POIIndex> other_pois;
        //OSM pois
        std::vector <unsigned> airport_pois;
        std::vector <unsigned> subway_pois;
        std::vector <unsigned> tourism_pois;
        std::vector <unsigned> neighbourhoods;
        void load_POI_data_structures();

};

#endif /* MAP_DATA_STRUCTURE_H */

