std::vector<unsigned> dijkstra2(IntersectionIndex start, IntersectionIndex end, double turn_penalty) {
std::vector<graph> node2 = node;
std::vector<unsigned> queue2= queue;
std::vector<unsigned> vertex2 = vertex;
std::vector<unsigned> visited2 =visited;
std::vector<unsigned> shortest_path2 = shortest_path;
std::vector<unsigned> shortest_path_street2 =shortest_path_street;

std::vector<unsigned> came_from2 = came_from;
std::vector<unsigned> came_from_street2 = came_from_street2;

    std::priority_queue<queue_element, std::vector<queue_element>,
            CompareIntersectionDistance> p_queue;
    

    shortest_path2.clear();
    shortest_path_street2.clear();
    visited2.clear();
    std::vector<double> weight(getNumberOfIntersections(), DBL_MAX);
    came_from2[start] = DBL_MAX;
    weight[start] = 0;

    bool complete = false;
    for (IntersectionIndex i = 0; i < node2.size(); i++) {
        node2[i].visited= false;
        node2[i].shortest_distance = DBL_MAX;
        p_queue.push(std::pair<IntersectionIndex, double> (i, DBL_MAX));
    }
    p_queue.push(std::pair<IntersectionIndex, double> (start, 0));
    LatLon end_position = getIntersectionPosition(end);    
    while (!p_queue.empty()) {
        queue_element current_node = p_queue.top();
        p_queue.pop();
        unsigned current = current_node.first;
            
        if (current == end) {
            complete = true;
            break;
        }
        if (current_node.second >= DBL_MAX){
            break;
        }
        if (!node2[current].visited) {
            if (node2[current].extra_street != 0) {

            } else {
                for (unsigned i = 0; i < node2[current].adjacent.size(); i++) {
                    double temp_weight = weight[current] + node2[current].weightings[i];
                    if (current != start) {
                        //There is something wrong.
                        if (street_segments2[came_from_street2[current]].StreetSegmentInfo_.streetID != street_segments2[node2[current].street[i]].StreetSegmentInfo_.streetID) {
                            temp_weight = temp_weight + turn_penalty;
                        }
                    }
                    if (temp_weight < weight[node2[current].adjacent[i]]) {
                        weight[node2[current].adjacent[i]] = temp_weight;
                        double weight_astar = find_distance_between_two_points(getIntersectionPosition(node2[current].adjacent[i]), end_position) / ((top_speed * 1000 / 3600) + 2);
                        came_from2[node2[current].adjacent[i]] = current;
                        came_from_street2[node2[current].adjacent[i]] = node2[current].street[i];
                        p_queue.push(std::pair<IntersectionIndex, double> (node2[current].adjacent[i], (temp_weight + weight_astar)));
                    }
                }
            }
        }
        node2[current].visited = true;
        visited2.push_back(current);
    }
    if (complete) {
        unsigned current_path = end;
        while (current_path != start) {
            shortest_path2.push_back(current_path);
            shortest_path_street2.push_back(came_from_street2[current_path]);
            current_path = came_from2[current_path];
        }
    } else {
        shortest_path2.clear();
        shortest_path_street2.clear();
        return shortest_path2;
    }
    std::reverse(shortest_path2.begin(), shortest_path2.end());
    std::reverse(shortest_path_street2.begin(), shortest_path_street2.end());
    for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
        weight[i] = DBL_MAX;
        came_from2[i] = DBL_MAX;
        node2[i].visited = false;
        node2[i].in_queue = false;
    }
    visited2.clear();
    //directions();
    return shortest_path_street2;
}