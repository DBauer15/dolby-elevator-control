#ifndef ELEVATOR_H
#define ELEVATOR_H


#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <memory>
#include "util.h"

enum ElevatorDirection {
    UP,
    DOWN
};

struct ElevatorRequest {
    int on_floor;
    int to_floor;
    bool picked_up;

    ElevatorRequest(int num_floors) {
        picked_up = false;
        on_floor = (int)randrange(1, num_floors+1);
        to_floor = on_floor;
        do {
            to_floor = (int)randrange(1, num_floors+1);
        } while(to_floor == on_floor);
    }
};

struct ElevatorState {
    bool stopped;
    int floor;
    ElevatorDirection direction;

    timepoint_t time_since_movement_change;
};


class Elevator {

    public:
        Elevator(int num_floors, double speed) : num_floors(num_floors), speed(speed) {
            state.floor = 1;
            state.stopped = true;
            state.direction = ElevatorDirection::UP;
            state.time_since_movement_change = time_now();
            log_status("Starting Elevator (floors = " + std::to_string(num_floors) + ", speed = " + std::to_string(speed) + ")");
        }

        void add_request(ElevatorRequest request, bool rerank = true);
        void update();
        void log_status(std::string message = "");

    private:
        int num_floors;
        double speed;

        ElevatorState state;

        std::unique_ptr<ElevatorRequest> current_request;
        std::map<int, std::vector<ElevatorRequest>> requests;
        std::set<int> request_targets;
        std::vector<int> schedule;

        void rank_requests();
        void handle_requests();
        void handle_idle();
        bool advance();
};

#endif