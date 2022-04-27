# Simple Elevator Control

This program simulates a single elevator serving a building. 

## How to build and run

The project requires a C++11 compatible compiler and can be built using cmake (>= 3.16). To build, run the following commands in the project directory:

```
mkdir build
cd build
cmake ..
cmake --build .
```

To run the elevator control:
```
./ec NUM_FLOORS ELEVATOR_SPEED PASSENGER_FREQUENCY
```
Where NUM_FLOORS determines the number of floors to serve, ELEVATOR_SPEED is the number of seconds it takes the elevator to travel from one floor to the next, and PASSENGER_FREQUENCY determines how frequently new calls are added. Both values need to be integers.


An example would be:
```
./ec 12 2 0.1
```
This creates an elevator with 12 floors that moves 1 floor every 2 seconds and there is a 10% chance every second that a new call is added.

## Assumptions made for this problem
I have made a few assumption for the implementation and will describe them here.

### Scheduling Algorithm
This program implements the LOOK disk scheduling algorithm to pick up and drop off passengers. In this context, I make no difference between an outside elevator call and an in-cabin request. Both require the elevator to travel to a certain floor. The LOOK algorithm provides very good overall waiting time for all passengers without requiring too many directional changes from the cabin - thus also preserving energy. I chose this algorithm because I thought this was a good trade-off between energy consumption and convenience. 

Any time a new call is received or a new passenger is picked up, the schedule is recomputed based on this algorithm. 

### Elevator Calls

For convenience, the system randomly generates calls at arbitrary points in time which emulates the behavior of a real elevator operation.
Each call contains information about where the passenger is and where they want to go. We assume that the destination is already known to the passenger at call time. However, it is not revealed to the elevator until the passenger is picked up at their floor.

### Number of floors
The number of floors served by the elevator can be set via command line parameters. Floor numbering starts at 1.

### Elevator Movement
The elevator has a fixed speed that can be set via a command line parameter. This value represents the time the cabin needs to move from one floor to the next regardless of direction. To simplify the simulation, I assume that once the cabin reaches a floor, all passenger transactions happen instantaneously. This allows the cabin to move on immediately. If there are no further requests the cabin will halt on the current floor. 