/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "m1.h"
#include "m3.h"
#include "m4.h"
#include "global.h"
#include "map_data_structure.h"
#include "graphics.h"
#include "StreetsDatabaseAPI.h"
#include <string>
#include <utility>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <time.h>

double path_before(const std::vector<unsigned>& intersections, const unsigned& swap_start, const unsigned& swap_end, const double& turn_penalty);
double path_after(const std::vector<unsigned>& intersections, const unsigned& swap_start, const unsigned& swap_end, const double& turn_penalty);
double courier_travel_time(const std::vector<unsigned>& intersections, const unsigned& swap_start, const unsigned& swap_end, const double& turn_penalty);
double courier_total_travel_time(const std::vector<unsigned>& intersections, const double& turn_penalty);
//std::vector<std::vector<node>> initial_load(const std::vector<unsigned>& deliveries, const float turn_penalty);


// This routine takes in a vector of N deliveries (pickUp, dropOff
// intersection pairs), another vector of M intersections that
// are legal start and end points for the path (depots) and a turn
// penalty in seconds (see m3.h for details on turn penalties).
//

// The first vector ’deliveries’ gives the delivery information: a set of
// pickUp/dropOff pairs of intersection ids which specify the
// deliveries to be made. A delivery can only be dropped-off after
// the associated item has been picked-up.
//
// The second vector ’depots’ gives the intersection
// ids of courier company depots containing trucks; you start at any
// one of these depots and end at any one of the depots.
//
// This routine returns a vector of street segment ids that form a
// path, where the first street segment id is connected to a depot
// intersection, and the last street segment id also connects to a
// depot intersection. The path must traverse all the delivery
// intersections in an order that allows all deliveries to be made --
// i.e. a package won’t be dropped off if you haven’t picked it up
// yet.
//
// You can assume that N is always at least one, and M is always at
// least one (i.e. both input vectors are non-empty).
//
// It is legal for the same intersection to appear multiple times in
// the pickUp or dropOff list (e.g. you might have two deliveries with
// a pickUp intersection id of #50). The same intersection can also
// appear as both a pickUp location and a dropOff location.
//
// If you have two pickUps to make at an intersection,
// traversing the intersection once is sufficient
// to pick up both packages, and similarly one traversal of an
// intersection is sufficient to drop off all the (already picked up)
// packages that need to be dropped off at that intersection.
//
// Depots will never appear as pickUp or dropOff locations for deliveries.
//
// If no path connecting all the delivery locations
// and a start and end depot exists, this routine must return an
// empty (size == 0) vector.

std::vector<unsigned> path_final; //holds the intersections
std::vector<std::vector<unsigned>> path_2;


std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots,
        const float turn_penalty) {
    std::srand(time(NULL));
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<unsigned> path; //holds the intersections
    std::vector<unsigned> path_ordered;
    std::vector<unsigned> path_ordered_dropoff;
    std::vector<unsigned> path_dropoff;
    double before_time, after_time;
    double after_distance;
    unsigned temp_unsigned;

    //initial load
    for (unsigned i = 0; i < deliveries.size(); i++) {
        path.push_back(deliveries[i].pickUp);
        path_dropoff.push_back(deliveries[i].dropOff);
    }

    temp_unsigned = std::abs(rand() % deliveries.size());
    
    //starts at a random intersection
    path_ordered.push_back(path[temp_unsigned]);
    
    //temp_path2 holds all the pick up points currently
    auto temp_path2 = path;
    temp_path2[temp_unsigned] = path_dropoff[temp_unsigned];
    for(unsigned q = 0; q< 2; q++){
    for (unsigned j = 0; j < path.size() -1 ; j++) { // goes through all the nodes
        double best_distance = DBL_MAX;
        unsigned temp_closest = 0;
        
        for (unsigned i = 0; i < temp_path2.size(); i++) { // checks for closest one
            if (temp_path2[i] != path_ordered[j] && temp_path2[i] != -1 ) {
                double temp_distance = find_distance_between_two_points(getIntersectionPosition(path_ordered[j]), getIntersectionPosition(temp_path2[i]));
                if (temp_distance < best_distance) {
                    best_distance = temp_distance;
                    temp_closest = i;
                }
            }
        }
        

            path_ordered.push_back(temp_path2[temp_closest]);
            bool is_dropoff = false;
            
            
            for(unsigned z = 0; z < path_dropoff.size(); z++){
                if (path_dropoff[z] == temp_path2[temp_closest]){
                    is_dropoff = true;
                }
            }

            if (is_dropoff == false) {
                temp_path2[temp_closest] = path_dropoff[temp_closest];
            } else {
                temp_path2[temp_closest] = -1;
            }
        }
}
  for (unsigned i = 0; i < temp_path2.size(); i++){
      if (temp_path2[i] != -1) 
          path_ordered.push_back(temp_path2[i]);
  }
       
//    path_ordered.insert(path_ordered.end(), path_ordered_dropoff.begin(), path_ordered_dropoff.end());
    path.clear();
    path.swap(path_ordered);
//    path.insert(path.end(), path_dropoff.begin(), path_dropoff.end());

//    for (unsigned qq = 0; qq < path.size(); qq++) {
//        std::cout << path[qq] << std::endl;
//    }
    
    
//    initial_load(path, turn_penalty);
    
//    std::random_shuffle(path.begin(), path.end());
//    std::random_shuffle(path_dropoff.begin(), path_dropoff.end());

    auto temp_current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time = temp_current_time - start_time;
    std::cout << "initial load: " << elapsed_time.count() << std::endl; 
    
    //Random swaps 
    //Swaps two neighboring intersections
//    double temperature = 100;
//    auto count = 0;
//    while (temperature > 4) {
////        count++;
//        unsigned random = abs((std::rand()) % (path.size() - 1));
//        unsigned random2 = abs(random + (std::rand()) % (5));
//        unsigned swap_start = path[random];
//        unsigned swap_end = path[random + 1];
//        bool swap = true;
//        //checks if the swap is valid        
//        for (unsigned i = 0; i < deliveries.size(); i++) {
//            if (deliveries[i].pickUp == swap_start) {
//                if (deliveries[i].dropOff == swap_end) {
//                    swap = false;
//                }
//            }
//        }
//        //If valid swap, check if it's a better result, if not then swap it back
//        if (swap) {
//            double random_acceptance = abs(std::rand() % 1000) / 1000.0;
//            before_time = path_before(path, random, random + 1, turn_penalty);
//            after_time =  path_after(path, random, random + 1, turn_penalty);
//            
//            //swap if new path is better or if previous path was better and the temperature is high
//            if (before_time < after_time) {
//                if (std::exp((before_time - after_time) / temperature) > random_acceptance) {
//                    std::iter_swap(path.begin() + random, path.begin() + random + 1);
//                }
//            } else {
//                std::iter_swap(path.begin() + random, path.begin() + random + 1);
//            }
//        }
//        auto current_time = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double, std::milli> elapsed_time = current_time - start_time;
////        temperature = std::abs(100 - (100 * elapsed_time.count() / 135000)); //The temperature should go from 100 to 0 in 25000ms (the math could be wrong)       
////        temperature = temperature* (1- 0.01);
//        temperature = 100 / (1 + (elapsed_time.count()/1000));
////        std::cout << temperature << std::endl;
////        std::cout << elapsed_time.count() << std::endl;
//    }
//    std::cout << "count: " << count << std::endl;
    unsigned depot_start = 0;
    unsigned depot_end = 0;
    double depot_to_start = DBL_MAX;
    double depot_to_end = DBL_MAX;
    //finds the closest start and end depots
    for (unsigned i = 0; i < depots.size(); i++) {
        auto path_ = find_path_between_intersections(depots[i], path[0], turn_penalty);
        auto path_2 = find_path_between_intersections(depots[i], path[path.size() - 1], turn_penalty);
        if (path_.size() != 0) {
            double temp_path_time = compute_path_travel_time(path_, turn_penalty);
            double temp_path_time_2 = compute_path_travel_time(path_2, turn_penalty);
            if (depot_to_start > temp_path_time) {
                depot_to_start = temp_path_time;
                depot_start = depots[i];
            }
            if (depot_to_end > temp_path_time_2) {
                depot_to_end = temp_path_time_2;
                depot_end = depots[i];
            }
        }
    }
//    auto temp_current_time2 = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double, std::milli> elapsed_time2 = temp_current_time2 - start_time;
//    std::cout << "With depot: " << elapsed_time2.count() << std::endl;

    path.insert(path.begin(), depot_start);
    path.push_back(depot_end);
    
    std::vector<unsigned> path_final; //holds the intersections
    std::vector<std::vector<unsigned>> path_2;
    

    
    
    
    std::vector<std::thread> path_threads;
    path_threads.clear();
    std::vector<std::thread> path_final_threads;
    path_final_threads.clear();
    

    resize_temps(path.size() - 1);
    
    for (unsigned i = 0; i < path.size() - 1; i++) {
        path_threads.push_back(std::thread(&find_path_between_intersections2, path[i], path[i + 1], turn_penalty, i));
    }

    for (unsigned i = 0; i < path.size() - 1; i++) {
        path_threads[i].join();
    }
    
   path_2 = return_temp();

    
    for (unsigned i = 0; i < path.size() - 1; i++) {
        path_final.insert(path_final.end(), path_2[i].begin(), path_2[i].end());
    }

    
//    auto temp_current_time3 = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double, std::milli> elapsed_time3 = temp_current_time3 - start_time;
//    std::cout << "End: " << elapsed_time3.count() << std::endl;
    
//    for (unsigned qq = 0; qq < path.size(); qq++) {
//        std::cout << path[qq] << std::endl;
//    }

    return path_final;

}


//
//std::vector<std::vector<node>> initial_load(const std::vector<unsigned>& deliveries, const float turn_penalty) {
//   std::vector<node> intersections;
//    std::vector<std::vector<node>> path;
//    std::vector<std::vector<unsigned>> temp_path;
//    double temp_distance;
//    for (unsigned i = 0; i < deliveries.size(); i++){
//        for (unsigned j = 0; j < deliveries.size(); j++){
//                temp_path = find_path_between_intersections(deliveries[i], deliveries[j], turn_penalty);
//                temp_distance = compute_path_travel_time(temp_path, turn_penalty);
//                intersections.push_back(std::make_pair(j, temp_distance));
//        }
//        path.push_back(intersections);
//    }
//    return path;
//}




double path_before(const std::vector<unsigned>& intersections, const unsigned& swap_start, 
        const unsigned& swap_end, const double& turn_penalty) {
    double time = 0;
    if (swap_start != 0){
        time = time + compute_path_travel_time(find_path_between_intersections(intersections[swap_start - 1], intersections[swap_start], turn_penalty), turn_penalty);
    }
    if (swap_end != intersections.size() - 1){
        time = time + compute_path_travel_time(find_path_between_intersections(intersections[swap_end], intersections[swap_end +1], turn_penalty), turn_penalty);
    }
    return time;
}
double path_after(const std::vector<unsigned>& intersections, const unsigned& swap_start, 
        const unsigned& swap_end, const double& turn_penalty) {
    double time = 0;
    if (swap_start != 0) {
        time = time + compute_path_travel_time(find_path_between_intersections(intersections[swap_start - 1], intersections[swap_end], turn_penalty), turn_penalty);
    }
    if (swap_end != intersections.size() - 1){
        time = time + compute_path_travel_time(find_path_between_intersections(intersections[swap_start], intersections[swap_end + 1], turn_penalty), turn_penalty);
    }
    return time;
}

//Calculates the travel time of the paths that swapped
double courier_travel_time(const std::vector<unsigned>& intersections, const unsigned& swap_start, const unsigned& swap_end, const double& turn_penalty) {
    double total_time = 0;
    std::vector<unsigned> current_path;
    if (swap_start != 0) {
        current_path = find_path_between_intersections(intersections[swap_start - 1], intersections[swap_start], turn_penalty);
        total_time = total_time + compute_path_travel_time(current_path, turn_penalty);
    }
    if (swap_end != intersections.size() - 1) {
        current_path = find_path_between_intersections(intersections[swap_end], intersections[swap_end + 1], turn_penalty);
        total_time = total_time + compute_path_travel_time(current_path, turn_penalty);
    }
    return total_time;
}

//double courier_total_travel_time(const std::vector<unsigned>& intersections, const double& turn_penalty) {
//    double total_time = 0;
//    for (unsigned i = 0; i < intersections.size() - 1; i++) {
//        std::vector<unsigned> current_path = find_path_between_intersections(intersections[i], intersections[i + 1], turn_penalty);
//        total_time = total_time + compute_path_travel_time(current_path, turn_penalty);
//    }
//    return total_time;
//}


//std::vector<std::vector<node>> initial_load(const std::vector<unsigned>& deliveries, const float turn_penalty) {
//    std::vector<node> intersections;
//    std::vector<std::vector<node>> path;
//    std::vector<unsigned> temp_path;
//    double temp_distance;
//    std::vector<std::vector<unsigned>> path_3;
//    std::vector<double> distance2;
//    
//    unsigned number = (deliveries.size()) * (deliveries.size());
//    resize_temps(deliveries.size()*deliveries.size()+deliveries.size());
//    
//    std::vector<std::thread> path_threads2;
//    std::vector<std::thread> distance_threads;
//
//    for (unsigned i = 0; i < deliveries.size(); i++) {
//        for (unsigned j = 0; j < deliveries.size(); j++) {
//            if (i != j) {
//                path_threads2.push_back(std::thread(&find_path_between_intersections2, deliveries[i], deliveries[j], turn_penalty, (i*deliveries.size())+j));
//            }
//        }
//    }
//    
//        for (unsigned i = 0; i < path_threads2.size(); i++) {
//                 path_threads2[i].join();
//        }
//    
//    path_3 = return_temp();
//    
//    for (unsigned i = 0; i < deliveries.size(); i++) {
//            for (unsigned j = 0; j < deliveries.size(); j++) {
//                if (i != j) {
//                 distance_threads.push_back(std::thread(&compute_path_travel_time2, path_3[i*deliveries.size()+j], turn_penalty, (i*deliveries.size())+j));
//                }
//            }
//        }
//    
//    for (unsigned i = 0; i < distance_threads.size(); i++) {
//                 distance_threads[i].join();
//        }
//    
//    distance2 = return_time2();
//    
//    
//    for (unsigned i = 0; i < deliveries.size(); i++){
//        for (unsigned j = 0; j < deliveries.size(); j++){
//            if(i!=j){
//                intersections.push_back(std::make_pair(j, distance2[i*deliveries.size()+j]));
//            }
//        }
//        
//        
//        path.push_back(intersections);
//    }
//    return path;
//}

//const float turn_penalty) {
//    std::srand(time(NULL));
//    auto start_time = std::chrono::high_resolution_clock::now();
//    std::vector<unsigned> path; //holds the intersections
//    std::vector<unsigned> path_ordered;
//    std::vector<unsigned> path_ordered_dropoff;
//    std::vector<unsigned> path_dropoff;
//    double before_time, after_time;
//    double after_distance;
//    unsigned temp_unsigned;
//
//    //initial load
//    for (unsigned i = 0; i < deliveries.size(); i++) {
//        path.push_back(deliveries[i].pickUp);
//        path_dropoff.push_back(deliveries[i].dropOff);
//    }
//
//    temp_unsigned = std::abs(rand() % deliveries.size());
//    
//    //starts at a random intersection
//    path_ordered.push_back(path[temp_unsigned]);
//    
//    //temp_path2 holds all the pick up points currently
//    auto temp_path2 = path;
//    temp_path2[temp_unsigned] = path_dropoff[temp_unsigned];
//    for(unsigned q = 0; q< 2; q++){
//    for (unsigned j = 0; j < path.size() -1 ; j++) { // goes through all the nodes
//        double best_distance = DBL_MAX;
//        unsigned temp_closest = 0;
//        
//        for (unsigned i = 0; i < temp_path2.size(); i++) { // checks for closest one
//            if (temp_path2[i] != path_ordered[j] && temp_path2[i] != -1 ) {
//                double temp_distance = find_distance_between_two_points(getIntersectionPosition(path_ordered[j]), getIntersectionPosition(temp_path2[i]));
//                if (temp_distance < best_distance) {
//                    best_distance = temp_distance;
//                    temp_closest = i;
//                }
//            }
//        }
//        
//
//            path_ordered.push_back(temp_path2[temp_closest]);
//            bool is_dropoff = false;
//            
//            
//            for(unsigned z = 0; z < path_dropoff.size(); z++){
//                if (path_dropoff[z] == temp_path2[temp_closest]){
//                    is_dropoff = true;
//                }
//            }
//
//            if (is_dropoff == false) {
//                temp_path2[temp_closest] = path_dropoff[temp_closest];
//            } else {
//                temp_path2[temp_closest] = -1;
//            }
//        }
//}
//  for (unsigned i = 0; i < temp_path2.size(); i++){
//      if (temp_path2[i] != -1) 
//          path_ordered.push_back(temp_path2[i]);
//  }