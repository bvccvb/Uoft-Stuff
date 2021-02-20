/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "map_data_structure.h"
#include <algorithm>

//Constructor
    map_data_structure::map_data_structure(){
        
    }
    
//Destructor- deletes map data structures;
    map_data_structure::~map_data_structure() {
        street_lengths.clear();
        street_seg_id.clear();
        street_segment_speed.clear();
        street_street_segments.clear();
        street_segment_travel_time.clear();
        street_segment_lengths.clear();
        street_ids_and_intersections.clear();
        street_names_to_ids.clear();
        directly_connected_intersections.clear();
        intersection_street_segments.clear();
        r_tree_poi.clear();
        r_tree_intersection.clear();
    }

// loads the rtree and stores the points of interest values in it
    void map_data_structure::load_rtree_poi() {
        // inserts points of interest into rtree 
        // loops through the whole list
        unsigned totalPoints = getNumberOfPointsOfInterest();
        for (unsigned i = 0; i < totalPoints; i++) {

            // gets the position for each point and stores them into a boost::point type variable
            pointIDx p = pointIDx(getPointOfInterestPosition(i).lat(), getPointOfInterestPosition(i).lon());
            // makes a pair of the location and index, then stores them in a std::pair type variable
            value v = value(p, i);
            r_tree_poi.insert(v); // inserts the std::pair type in the rtree
        }
    }
    
    // loads the rtree and stores the intersection values in it
    void map_data_structure::load_rtree_intersection() {
        // inserts intersections into rtree
        // loops through the whole list
        unsigned totalPoi = getNumberOfIntersections();
        for (unsigned i = 0; i < totalPoi; i++) {
            
            // gets the position for each point and stores them into a boost::point type variable
            pointIDx p = pointIDx(getIntersectionPosition(i).lat(), getIntersectionPosition(i).lon());
            // makes a pair of the location and index, then stores them in a std::pair type variable
            value v = value(p, i);
            r_tree_intersection.insert(v); // inserts the std::pair type in the rtree
        }
    }


    //function to get street length
    double map_data_structure::get_street_length(unsigned street_id) {
        return (street_lengths[street_id]);
    }

    //pre-load the street length
    void map_data_structure::load_street_lengths() {
        for (unsigned street_id = 0; street_id < getNumberOfStreets(); street_id++) {
            double street_length = 0;
            std::vector<unsigned> segments_of_street = find_street_street_segments(street_id);
            for (unsigned i = 0; i < segments_of_street.size(); i++) {
                street_length = street_length + find_street_segment_length(segments_of_street[i]);
            }
            street_lengths.push_back(street_length);
        }
        return;
    }


    //data structure for segment speed and ID, doing this separately improves speed and performance
    //when loading some of the vectors, we realised the API calls slowed down speed. We did the API
    //calls separately and found it ran faster
    void map_data_structure::load_segment_speed_and_IDs() {
        for (unsigned street_segment_id = 0; street_segment_id < getNumberOfStreetSegments(); street_segment_id++) {
            street_segment_speed.push_back(getStreetSegmentInfo(street_segment_id).speedLimit);
            street_seg_id.push_back(getStreetSegmentInfo(street_segment_id).streetID);
        }
        return;
    }

    //Functions concerning the loading and retrieval of street segments data (street segment lengths, 
    //street segment travel time and the street segments of a street)
    float map_data_structure::get_street_segment_lengths(unsigned street_segment_id) {
        return (street_segment_lengths[street_segment_id]);
    }

    float map_data_structure::get_street_segment_travel_time(unsigned street_segment_id) {
        return (street_segment_travel_time[street_segment_id]);
    }

    std::vector<unsigned> map_data_structure::get_street_street_segments(unsigned street_id) {
        return street_street_segments[street_id];
    }

    //loads street segment street segment lengths, street segment travel time and the street segments of a street
    void map_data_structure::load_street_segment_lengths_travel_time_street_ids_and_segments() {
        street_street_segments.resize(getNumberOfStreets());
        for (unsigned street_segment_id = 0; street_segment_id < getNumberOfStreetSegments(); street_segment_id++) {
            //for loading street segments and travel time. 
            double length = 0;
            unsigned curve_points = getStreetSegmentInfo(street_segment_id).curvePointCount;
            LatLon point1 = getIntersectionPosition(getStreetSegmentInfo(street_segment_id).from);
            LatLon point2 = getIntersectionPosition(getStreetSegmentInfo(street_segment_id).to);
            //iterates through and sums up the length from one curve point to the next
            if (curve_points == 0) {
                length = find_distance_between_two_points(point1, point2);
            } else if (curve_points > 1) {
                for (unsigned i = 0; i < curve_points - 1; i++) {
                    length = length + find_distance_between_two_points(getStreetSegmentCurvePoint(street_segment_id, i), getStreetSegmentCurvePoint(street_segment_id, i + 1));
                }
                length = length + find_distance_between_two_points(point1, getStreetSegmentCurvePoint(street_segment_id, 0));
                length = length + find_distance_between_two_points(getStreetSegmentCurvePoint(street_segment_id, curve_points - 1), point2);
            } else {

                length = length + find_distance_between_two_points(point1, getStreetSegmentCurvePoint(street_segment_id, 0));
                length = length + find_distance_between_two_points(getStreetSegmentCurvePoint(street_segment_id, curve_points - 1), point2);
            }
            //load street_ids_and_segments, also loads the speed and length.
            street_street_segments[street_seg_id[street_segment_id]].push_back(street_segment_id);
            street_segment_lengths.push_back(length);
            street_segment_travel_time.push_back(length / 1000 / street_segment_speed[street_segment_id] * 60 * 60);
        }
        return;
    }
    
    
    //loads data structure for street ids to intersections (unordered map)
    void map_data_structure::load_street_ids_and_intersections(){
        //To optimize find_all_street_intersections
        street_ids_and_intersections.resize(getNumberOfStreets());

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {

            std::vector<unsigned> all_street_segments = find_street_street_segments(i);
            std::set<unsigned> all_street_intersections;
            std::set<unsigned>::iterator it;
            std::vector<unsigned> all_street_intersections_vec;
            for (unsigned j = 0; j < all_street_segments.size(); j++) {
                unsigned from, to;
                from = getStreetSegmentInfo(all_street_segments[j]).from;
                to = getStreetSegmentInfo(all_street_segments[j]).to;

                //Check if the intersection was already added to the vector
//                if (std::find(all_street_intersections.begin(), all_street_intersections.end(), from) == all_street_intersections.end())
                    all_street_intersections.insert(from);
//                if (std::find(all_street_intersections.begin(), all_street_intersections.end(), to) == all_street_intersections.end())
                    all_street_intersections.insert(to);
            }
//            std::sort(all_street_intersections.begin(),all_street_intersections.end());
//          auto last = std::unique( all_street_intersections.begin(),all_street_intersections.end());
//              all_street_intersections.erase( std::unique( last,all_street_intersections.end()));
            for (auto it = all_street_intersections.begin(); it != all_street_intersections.end(); it++){
                all_street_intersections_vec.push_back(*it);
            }
            street_ids_and_intersections[i] = all_street_intersections_vec;
        }
        return;
    }
    
    //Returns a vector of intersections for a given street id
    std::vector<unsigned> map_data_structure::get_street_ids_and_intersections(unsigned street_id){
        return street_ids_and_intersections[street_id];
    }
    
    
    
    //Loads data structure for street names to ids (vector)

    void map_data_structure::load_street_names_to_ids() {
        //To optimize find_street_ids_from_name

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            street_names_to_ids[getStreetName(i)] = std::vector<unsigned>(0);
        }

        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
            street_names_to_ids[getStreetName(i)].push_back(i);
        }
        return;
    }
    
    //Returns a vector of street ids with a given name
    std::vector<unsigned> map_data_structure::get_street_names_to_ids(std::string name){
        return street_names_to_ids[name];
    }
    
    //Loads data structure for directly connected intersections and intersection street segments 
    void map_data_structure::load_directly_connected_intersections_and_intersection_street_segments(){
        //load DIRECTLY CONNECTED INTERSECTIONS
        //for every intersection
        for (unsigned i = 0; i < getNumberOfIntersections(); i++) {

            std::vector <unsigned> connectedIntersections;
            std::vector <unsigned> segments;
            //for each segment in the intersection record directly connected intersections
            for (unsigned j = 0; j < getIntersectionStreetSegmentCount(i); j++) {

                //if at 'from' add 'to' intersection to list
                if ((getStreetSegmentInfo(getIntersectionStreetSegment(i, j)).from) == i) {
                    connectedIntersections.push_back(getStreetSegmentInfo(getIntersectionStreetSegment(i, j)).to);

                    //if at 'to' and its not one way add 'from' 
                } else if (((getStreetSegmentInfo(getIntersectionStreetSegment(i, j)).to) == i) && (!getStreetSegmentInfo(getIntersectionStreetSegment(i, j)).oneWay)) {
                    connectedIntersections.push_back(getStreetSegmentInfo(getIntersectionStreetSegment(i, j)).from);

                }
                segments.push_back(getIntersectionStreetSegment(i, j));
            }
            directly_connected_intersections.push_back(connectedIntersections);
            intersection_street_segments.push_back(segments);
        }
        return;
    }
    
    //Returns a vector of street segments for a given intersection id
    std::vector<unsigned> map_data_structure::get_intersection_street_segments(unsigned intersection_id){
        return intersection_street_segments[intersection_id];
    }
    
    //Returns a vector of directly connected intersections for a given intersection id
    std::vector<unsigned> map_data_structure::get_directly_connected_intersections(unsigned intersection_id){
        return directly_connected_intersections[intersection_id];
    }

    unsigned map_data_structure::closest_poi(LatLon my_position) {
        // gets the provided position and stores them in a point type variable
        pointIDx p = pointIDx(pointIDx(my_position.lat(), my_position.lon()));
        std::vector<value> vec; // stores the nearest values
        double increment = 0.02; // dictates the size of the box
        bool found = false;
            do {
            if (vec.size()>= 1){
                vec.clear();
                found = true;
            }
            // creates a box around the location
            bg::model::box<pointIDx> box(pointIDx((p.get<0>() - increment), (p.get<1>() - increment)), pointIDx((p.get<0>() + increment), (p.get<1>() + increment)));
            // finds all the values in side that box and stores them in vec
            r_tree_poi.query(bgi::within(box), std::back_inserter(vec));
            increment = increment * 2;
        } while (!found && vec.size() < 100); // makes sure there is enough points in the box to avoid corner cases    
        // goes through the vector containing some of the closest value
        // and finds the closest one from those values
        unsigned closest_point = vec[0].second;
        double closest_distance = find_distance_between_two_points(my_position, getPointOfInterestPosition(vec[0].second));
        for (unsigned k = 1; k < vec.size(); k++) {
            double current_distance = find_distance_between_two_points(my_position, getPointOfInterestPosition(vec[k].second));
            if (current_distance < closest_distance) {
                closest_distance = current_distance;
                closest_point = vec[k].second;
            }
        }
        return closest_point; // returns the index of the closest point of interest

    }

    unsigned map_data_structure::closest_intersection(LatLon my_position) {
        // gets the provided position and stores them in a point type variable
        pointIDx p = pointIDx(pointIDx(my_position.lat(), my_position.lon()));
        double increment = 0.02; // dictates the size of the box
        std::vector<value> vec; // stores the nearest values
        bool found = false;
        do {
            if (vec.size()>= 1){
                vec.clear();
                found = true;
            }
            // creates a box around the location
            bg::model::box<pointIDx> box(pointIDx((p.get<0>() - increment), (p.get<1>() - increment)), pointIDx((p.get<0>() + increment), (p.get<1>() + increment)));
            // finds all the values in side that box and stores them in vec
            r_tree_intersection.query(bgi::within(box), std::back_inserter(vec));
            increment = increment * 2;
        } while (!found && vec.size() < 100); // makes sure there is enough points in the box to avoid corner cases
        // goes through the vector containing some of the closest value
        // and finds the closest one from those values
        unsigned closest_point = vec[0].second;
        double closest_distance = find_distance_between_two_points(my_position, getIntersectionPosition(vec[0].second));
        for (unsigned k = 1; k < vec.size(); k++) {
            double current_distance = find_distance_between_two_points(my_position, getIntersectionPosition(vec[k].second));
            if (current_distance < closest_distance) {
                closest_distance = current_distance;
                closest_point = vec[k].second;
            }
        }
                
        return closest_point; // returns the index of the closest point of interest

    }
    
    void map_data_structure::load_POI_data_structures(){
        for(unsigned i = 0; i < getNumberOfPointsOfInterest(); i++){
            if(getPointOfInterestType(i) == "restaurant"){
                restaurant_pois.push_back(i);
            }
            if(getPointOfInterestType(i) == "cafe"){
                cafe_pois.push_back(i);
            }
            if(getPointOfInterestType(i) == "fast_food"){
                fast_food_pois.push_back(i);
            }
            if((getPointOfInterestType(i) == "pub") || (getPointOfInterestType(i) == "food_court") || (getPointOfInterestType(i) == "ice_cream" )){
                food_pois.push_back(i);
            }
            else if((getPointOfInterestType(i) == "bank") || (getPointOfInterestType(i) == "atm")){
                bank_pois.push_back(i);
            }
            else if(getPointOfInterestType(i) == "hospital") hospitals_pois.push_back(i);
            else if((getPointOfInterestType(i) == "pharmacy") || (getPointOfInterestType(i) == "dentist") || (getPointOfInterestType(i) == "doctors" ) || (getPointOfInterestType(i) == "clinic")){
                health_pois.push_back(i);
            }
            else if(getPointOfInterestType(i) == "fuel"){
                fuel_pois.push_back(i);
            }
            else if(getPointOfInterestType(i) == "place_of_worship") church_pois.push_back(i);
            else if((getPointOfInterestType(i) == "university") || (getPointOfInterestType(i) == "college")) top_schools_pois.push_back(i);
            else if((getPointOfInterestType(i) == "school") || (getPointOfInterestType(i) == "library")){
                school_pois.push_back(i);
            }
            else if((getPointOfInterestType(i) == "bar") || (getPointOfInterestType(i) == "stripclub") || (getPointOfInterestType(i) == "nightclub")){
                drinking_pois.push_back(i);
            }
            else if((getPointOfInterestType(i) == "fire_station") || (getPointOfInterestType(i) == "police") || (getPointOfInterestType(i) == "ambulance_station")){
                emergency_pois.push_back(i);
            }
            else if((getPointOfInterestType(i) == "cinema") || (getPointOfInterestType(i) == "theatre")){
                cinema_pois.push_back(i);
            }
            else if((find(airport_pois.begin(), airport_pois.end(), i) == airport_pois.end()) && (find(subway_pois.begin(), subway_pois.end(), i) == subway_pois.end()) && (find(tourism_pois.begin(), tourism_pois.end(), i) == tourism_pois.end())){
                other_pois.push_back(i);  
            }
        }
    }
