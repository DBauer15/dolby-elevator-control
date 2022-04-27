#include <iostream>
#include <chrono>
#include <ctime>
#include <stdlib.h>
#include <cassert>

#include "elevator.h"

#define CATCH_CLI_ARG(PARAM, VALUE) catch(...) { std::cout << "Unable to parse " << PARAM << " '" << VALUE << "'" << std::endl; return -1; }

int main(int ac, char** av) {
    if (ac < 3) {
        std::cout << "Please provide NUM_FLOORS and ELEVATOR_SPEED as parameters" << std::endl;
        return -1;
    }

    int NUM_FLOORS;
    int ELEVATOR_SPEED;
    float PASSENGER_FREQUENCY;

    // Parse the command line parameters
    try {
        NUM_FLOORS = atoi(av[1]);
    } CATCH_CLI_ARG("NUM_FLOORS", av[1])
    try {
        ELEVATOR_SPEED = atoi(av[2]);
    } CATCH_CLI_ARG("ELEVATOR_SPEED", av[2])
    try {
        PASSENGER_FREQUENCY = atof(av[3]);

    } CATCH_CLI_ARG("PASSENGER_FREQUENCY", av[3])

    // Validate parameters
    assert(NUM_FLOORS > 1);
    assert(ELEVATOR_SPEED > 0);
    assert(PASSENGER_FREQUENCY >= 0.0f);
    assert(PASSENGER_FREQUENCY <= 1.0f);

    // Seed the random generator
    std::srand(std::time(0));

    // Mark the time we started the program; values will be used as reference
    timepoint_t start = time_now();

    // Create new elevator instance
    Elevator elevator(NUM_FLOORS, ELEVATOR_SPEED);

    while(true) {

        // Update the elevator state
        elevator.update();

        // Get current timestamp
        timepoint_t now = time_now();
        if (duration<std::chrono::seconds>(start, now) > 1.0) {
            start = now;
            
            // There's a chance we add a new random request
            if (rand01() < PASSENGER_FREQUENCY) {
                ElevatorRequest request(NUM_FLOORS);
                elevator.add_request(request);
            }
        }
    }
}

