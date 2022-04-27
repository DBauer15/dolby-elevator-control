#include "elevator.h"
#include <algorithm>
#include <string>


/**
 * @brief Adds a new request to the elevator control
 *
 * @param request The request to be added
 */
void Elevator::add_request(ElevatorRequest request, bool rerank)
{

    // Requests with the picked_up flag indicate passengers that have just entered the cabin and have pressed a button
    // For those, we use the to_floor floor number to make the cabin move to the passenger's destination
    if (request.picked_up) {
        requests[request.to_floor].push_back(request);
        request_targets.insert(request.to_floor);
        if (rerank) rank_requests();
        log_status("[INSIDE] passenger on floor " + std::to_string(request.on_floor) + " going to floor " + std::to_string(request.to_floor));
    } else { // For calls we need to move the cabin to the on_floor location where the passenger pressed the button
        requests[request.on_floor].push_back(request);
        request_targets.insert(request.on_floor);
        if (rerank) rank_requests();
        log_status("[CALLED] received call on floor " + std::to_string(request.on_floor));
    }
}

/**
 * @brief Update the elevator state
 */
void Elevator::update()
{
    // If there's no requests, handle idle state
    if (schedule.size() == 0)
    {
        handle_idle();
    }
    else // If there's requests, handle them
    {
        // If the elevator was waiting, we need to reset and redetermine some values
        if (state.stopped) {
            state.time_since_movement_change = time_now();
            state.stopped = false;
            state.direction = state.floor < schedule[0] ? ElevatorDirection::UP : ElevatorDirection::DOWN;
            log_status("[MOVE] starting to move " + std::string((state.direction == ElevatorDirection::UP) ? "UP" : "DOWN"));
        }

        // Handle the current list of requests
        handle_requests();
    }
}

/**
 * @brief Prints the elevator status
 */
void Elevator::log_status(std::string message)
{
    // print message
    std::cout << message << std::endl;

    // print detailed stats
    std::cout << "[STATUS] {";
    std::cout << " floor = " << state.floor;
    std::cout << ", stopped = " << (state.stopped ? "YES" : "NO");
    std::cout << ", direction = " << (state.direction == ElevatorDirection::UP ? "UP" : "DOWN");
    std::cout << ", schedule = ";
    for (int floor : schedule)
        std::cout << floor << "; ";
    std::cout << ", number of target floors = " << request_targets.size() << " }" << std::endl;
    

    // visualize the elevator
    std::cout << "L [";
    for (int i = 1; i <= num_floors; i++) {
        if (i == state.floor)
            std::cout << "X";
        else if (request_targets.find(i) != request_targets.end())
            std::cout << "o";
        else
            std::cout << ".";
    }
    std::cout << "] H\n" << std::endl;
}

/**
 * @brief Implements the LOOK algorithm to rank requests
 *
 */
void Elevator::rank_requests()
{
    // Define two directions to collect requests relative to the current cabin position
    std::vector<int> dir1, dir2;

    // Put requests into their respective directional lists
    for (int floor : request_targets)
    {
        if (floor < state.floor)
            dir1.push_back(floor);
        else if (floor > state.floor)
            dir2.push_back(floor);
        else {
            if (state.direction == ElevatorDirection::UP)
                dir1.push_back(floor);
            else
                dir2.push_back(floor);
        } 

    }

    // Sort both directions to diverge from the current position
    std::sort(dir1.begin(), dir1.end(), std::greater<int>());
    std::sort(dir2.begin(), dir2.end());

    // Depending on the current cabin direction we might need to swap the directional lists
    if (state.direction == ElevatorDirection::UP)
        std::swap(dir1, dir2);

    // Clear the schedule and populate with the directional lists
    schedule.clear();
    schedule.reserve(dir1.size() + dir2.size());
    schedule.insert(schedule.end(), dir1.begin(), dir1.end());
    schedule.insert(schedule.end(), dir2.begin(), dir2.end());
}

/**
 * @brief Updates the elevator state based on the list of target floors
 *
 */
void Elevator::handle_requests()
{
    // Advance the cabin; If a floor is reached, print a message
    if (advance()) {
        if (state.floor == schedule[0]) {    
            log_status("[MOVE] reached destination floor " + std::to_string(state.floor));    
        } else {
            log_status("[MOVE] reached floor " + std::to_string(state.floor));
        }
    }

    // If a target floor is reached, deal with the relevant requests and recompute the schedule
    if (state.floor == schedule[0]) {

        // Remove target floor from target set
        request_targets.erase(schedule[0]);

        // Retrieve all requests associated with target floor
        std::vector<ElevatorRequest> floor_requests = requests[schedule[0]];

        // Remove entries from target map
        requests.erase(schedule[0]);

        // Handle all relevant requests from this floor
        for (auto request : floor_requests) {

            // If the request is a call, pick up the passenger and re-add the request with the picked_up flag set
            if (!request.picked_up) {
                request.picked_up = true;
                add_request(request, false); // Skip reranking for now; We'll do it in bulk below
            }
        }

        // Re-rank all requests
        rank_requests();

        // Determine if we need to change directions to reach the next target
        state.direction = state.floor > schedule[0] ? ElevatorDirection::DOWN : ElevatorDirection::UP;
    }
}

/**
 * @brief Updates the elevator state in case there are currently no requests
 *
 */
void Elevator::handle_idle()
{
    // Stop the cabin if it is still not stopped
    if (!state.stopped) {
        state.stopped = true;
        log_status("[IDLE] Waiting on floor " + std::to_string(state.floor));
    }
}

bool Elevator::advance() {

    auto now = time_now();
    
    // If the cabin is moving and we have exceeded the elevator time between floors, we have reached a new floor
    if (!state.stopped && duration<std::chrono::seconds>(state.time_since_movement_change, now) > speed) {  
        state.floor += state.direction == ElevatorDirection::UP ? 1 : -1; // Update floor number based on direction
        state.stopped = schedule.size() == 0; // Determine if the cabin should stop
        state.time_since_movement_change = time_now(); // Reset the time since the cabin last moved
        return true;
    }

    return false;
}