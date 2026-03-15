# SmartRide – Graph-Based Ride Routing System

SmartRide is a C++ simulation of a ride routing platform that models how riders are matched with nearby drivers using graph algorithms and dynamic pricing.

The system represents a city as a graph where nodes correspond to locations and edges represent roads. Dijkstra's algorithm is used to compute the optimal route between pickup and destination points.

## Features

- Graph-based city map representation
- Shortest route computation using Dijkstra’s Algorithm
- Driver selection based on proximity and rating
- Traffic simulation affecting travel time
- Dynamic fare calculation with surge multiplier
- Ride history tracking for riders and drivers
- Driver earnings calculation
- Driver rating system
- Interactive command-line interface

## Algorithms Used

- Dijkstra's Shortest Path Algorithm
- Priority Queue (Min Heap)
- Graph Adjacency List Representation
- Sorting for driver selection

## System Modules

### Graph Module
Represents the city road network using an adjacency list.

### Driver Module
Maintains driver location, rating, availability, and earnings.

### Ride Module
Stores ride information such as pickup, destination, distance, and fare.

### Ride System
Handles driver matching, route computation, traffic simulation, and pricing.

## Example Menu

1 Show Graph  
2 Add Node  
3 Add Road  
4 Request Ride  
5 Simulate Traffic  
6 Rider History  
7 Driver History  
8 Driver Earnings  
9 Rate Driver  
10 Exit  

## Technologies Used

- C++
- Standard Template Library (STL)
- Graph Algorithms

## Future Improvements

- Map visualization
- Real-time driver movement
- Ride pooling
- Graphical user interface
- Database integration

## Author

Project developed as an algorithm and system design simulation in C++.
