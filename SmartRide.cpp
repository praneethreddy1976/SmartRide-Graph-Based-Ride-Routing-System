#include <bits/stdc++.h>
using namespace std;

/* ---------- CONSTANTS ---------- */
static const double BASE_FARE            = 40.0;
static const double COST_PER_KM          = 10.0;
static const double COST_PER_MIN         = 2.0;
static const double AVG_SPEED_KMPH       = 30.0;
static const double SURGE_HIGH           = 2.0;
static const double SURGE_MED            = 1.5;
static const double SURGE_THRESHOLD_HIGH = 2.0;
static const double SURGE_THRESHOLD_MED  = 1.0;
static const int    MAX_TOP_DRIVERS      = 3;
static const int    MAX_TRAFFIC_EXTRA    = 3;


/* ---------- EDGE ---------- */
struct Edge {
    int node;
    int distance;
    int time;
};

/* ---------- GRAPH ---------- */
class Graph {
public:
    unordered_map<int, vector<Edge> > adj;
    void addNode(int node) {
        if (adj.count(node)) {
            cout << "Node " << node << " already exists\n";
            return;
        }
        adj[node] = vector<Edge>();
        cout << "Node " << node << " added\n";
    }

    bool nodeExists(int node) const {
        return adj.count(node) > 0;
    }

    bool addEdge(int u, int v, int dist) {
        if (!nodeExists(u) || !nodeExists(v)) {
            cout << "Cannot add road: one or both nodes do not exist\n";
            return false;
        }
        adj[u].push_back((Edge){v, dist, dist});
        adj[v].push_back((Edge){u, dist, dist});
        return true;
    }

    void displayGraph() const {
        cout << "\n--- CITY GRAPH ---\n";
        for (auto it = adj.begin(); it != adj.end(); ++it) {
            int node = it->first;
            const vector<Edge>& edges = it->second;
            cout << "Node " << node << " -> ";
            for (size_t i = 0; i < edges.size(); i++) {
                const Edge& e = edges[i];
                cout << "(" << e.node
                     << ", dist:" << e.distance
                     << ", time:" << e.time << ") ";
            }
            cout << "\n";
        }
    }
    pair<int, vector<int> > shortestPath(int start, int dest) const {
        unordered_map<int, int> dist, parent;
        for (auto it = adj.begin(); it != adj.end(); ++it)
            dist[it->first] = INT_MAX;
        priority_queue<pair<int,int>,vector<pair<int,int> >,greater<pair<int,int> > > pq;
        dist[start] = 0;
        pq.push(make_pair(0, start));
        while (!pq.empty()) {
            pair<int,int> top = pq.top();
            pq.pop();
            int d = top.first;
            int u = top.second;
            if (d > dist[u]) continue;
            const vector<Edge>& edges = adj.at(u);
            for (size_t i = 0; i < edges.size(); i++) {
                const Edge& e = edges[i];
                int v = e.node;
                if (dist[u] + e.time < dist[v]) {
                    dist[v] = dist[u] + e.time;
                    parent[v] = u;
                    pq.push(make_pair(dist[v], v));
                }
            }
        }
        if (dist[dest] == INT_MAX)
            return make_pair(INT_MAX, vector<int>());

        vector<int> path;
        int cur = dest;
        while (parent.count(cur)) {
            path.push_back(cur);
            cur = parent[cur];
        }
        path.push_back(start);
        reverse(path.begin(), path.end());
        return make_pair(dist[dest], path);
    }

    int pathDistance(const vector<int>& path) const {
        int total = 0;
        for (size_t i = 0; i + 1 < path.size(); i++) {
            const vector<Edge>& edges = adj.at(path[i]);
            for (size_t j = 0; j < edges.size(); j++) {
                const Edge& e = edges[j];
                if (e.node == path[i + 1]) {
                    total += e.distance;
                    break;
                }
            }
        }
        return total;
    }

    void simulateTraffic(mt19937& rng) {
        uniform_int_distribution<int> trafficDist(0, MAX_TRAFFIC_EXTRA - 1);
        map<pair<int,int>, int> trafficMap;
        for (auto it = adj.begin(); it != adj.end(); ++it) {
            int node = it->first;
            const vector<Edge>& edges = it->second;
            for (size_t i = 0; i < edges.size(); i++) {
                const Edge& e = edges[i];
                pair<int,int> key = make_pair(min(node, e.node), max(node, e.node));
                if (!trafficMap.count(key))
                    trafficMap[key] = trafficDist(rng);
            }
        }
        for (auto it = adj.begin(); it != adj.end(); ++it) {
            int node = it->first;
            vector<Edge>& edges = it->second;
            for (size_t i = 0; i < edges.size(); i++) {
                Edge& e = edges[i];
                pair<int,int> key = make_pair(min(node, e.node), max(node, e.node));
                e.time = e.distance + trafficMap[key];
            }
        }
        cout << "Traffic updated\n";
    }
};

/* ---------- DRIVER ---------- */
class Driver {
    int id_;
    int location_;
    double rating_;
    bool available_;
    double earnings_;
public:
    Driver(int id, int loc, double rating)
        : id_(id), location_(loc), rating_(rating),
          available_(true), earnings_(0.0) {}
    int getId() const { return id_; }
    int getLocation() const { return location_; }
    double getRating() const { return rating_; }
    bool isAvailable() const { return available_; }
    double getEarnings() const { return earnings_; }

    void setLocation(int loc) { location_ = loc; }
    void setAvailable(bool avail) { available_ = avail; }
    void addEarnings(double amt) { earnings_ += amt; }

    void updateRating(double newRating) {
        rating_ = (rating_ + newRating) / 2.0;
    }
};

/* ---------- RIDE ---------- */
class Ride {
    int riderID_;
    int driverID_;
    int pickup_;
    int destination_;
    int distance_;
    double time_;
    double baseFare_;
    double distanceCost_;
    double timeCost_;
    double surge_;
    double totalFare_;
public:
    Ride(int riderID, int driverID, int pickup, int destination,
         int distance, double time,
         double baseFare, double distanceCost,
         double timeCost, double surge, double totalFare)
        : riderID_(riderID), driverID_(driverID),
          pickup_(pickup), destination_(destination),
          distance_(distance), time_(time),
          baseFare_(baseFare), distanceCost_(distanceCost),
          timeCost_(timeCost), surge_(surge), totalFare_(totalFare) {}
    int getRiderID() const { return riderID_; }
    int getDriverID() const { return driverID_; }
    int getPickup() const { return pickup_; }
    int getDestination() const { return destination_; }
    int getDistance() const { return distance_; }
    double getTotalFare() const { return totalFare_; }
};

/* ---------- SYSTEM ---------- */
class RideSystem {
    unordered_map<int, int> driverIndexById;
    double calculateFare(int dist, double time,
                         double& base, double& distCost,
                         double& timeCost, double& surge) const {
        base = BASE_FARE;
        distCost = dist * COST_PER_KM;
        timeCost = time * COST_PER_MIN;
        surge = 1.0;
        int demand = history.size() + 1;
        int supply = 0;
        for (size_t i = 0; i < drivers.size(); i++)
            if (drivers[i].isAvailable()) supply++;
        if (supply == 0) surge = SURGE_HIGH;
        else {
            double r = (double)demand / supply;
            if (r > SURGE_THRESHOLD_HIGH) surge = SURGE_HIGH;
            else if (r > SURGE_THRESHOLD_MED) surge = SURGE_MED;
        }
        return (base + distCost + timeCost) * surge;
    }

    vector<pair<double,int> > findNearestDrivers(int pickup) const {
        vector<pair<double,int> > candidates;
        for (size_t i = 0; i < drivers.size(); i++) {
            if (!drivers[i].isAvailable()) continue;
            pair<int, vector<int> > result =
                graph.shortestPath(drivers[i].getLocation(), pickup);
            int cost = result.first;
            if (cost == INT_MAX) continue;
            double score =
                0.6 * cost +
                0.3 * (5.0 - drivers[i].getRating());
            candidates.push_back(make_pair(score, (int)i));
        }
        sort(candidates.begin(), candidates.end());
        if (candidates.size() > MAX_TOP_DRIVERS)
            candidates.resize(MAX_TOP_DRIVERS);
        return candidates;
    }

public:
    Graph graph;
    vector<Driver> drivers;
    vector<Ride> history;
    void addDriver(int id, int loc, double rating) {

        if (driverIndexById.count(id)) {
            cout << "Driver " << id << " already exists\n";
            return;
        }

        driverIndexById[id] = (int)drivers.size();

        drivers.push_back(Driver(id, loc, rating));
    }

    void simulateTraffic(mt19937& rng) {
        graph.simulateTraffic(rng);
    }

    void requestRide(int riderID, int pickup, int dest) {

        if (!graph.nodeExists(pickup) || !graph.nodeExists(dest)) {
            cout << "Invalid node(s)\n";
            return;
        }
        vector<pair<double,int> > candidates = findNearestDrivers(pickup);
        if (candidates.empty()) {
            cout << "No available drivers can reach your pickup\n";
            return;
        }
        cout << "\nNearest Drivers\n";
        for (size_t i = 0; i < candidates.size(); i++) {
            double score = candidates[i].first;
            int idx = candidates[i].second;
            cout << "Driver " << drivers[idx].getId()
                 << " score "  << score << "\n";
        }
        int driverIndex = candidates[0].second;
        pair<int, vector<int> > result = graph.shortestPath(pickup, dest);
        int routeTime = result.first;
        vector<int> route = result.second;
        if (routeTime == INT_MAX) {
            cout << "No route exists\n";
            return;
        }
        int dist = graph.pathDistance(route);
        double time = (dist / AVG_SPEED_KMPH) * 60.0;
        double base, distCost, timeCost, surge;
        double fare = calculateFare(dist, time, base, distCost, timeCost, surge);
        cout << "\n--- RIDE COMPLETED ---\n";
        cout << "Driver " << drivers[driverIndex].getId() << "\n";
        cout << "Route: ";
        for (size_t i = 0; i < route.size(); i++)
            cout << route[i] << " ";
        cout << "\nDistance: " << dist << " km\n";
        cout << "Time: " << time << " minutes\n";
        cout << "\nPrice Breakdown\n";
        cout << "Base Fare Rs." << base << "\n";
        cout << "Distance Cost Rs." << distCost << "\n";
        cout << "Time Cost Rs." << timeCost << "\n";
        cout << "Surge x" << surge << "\n";
        cout << "Total Fare Rs." << fare << "\n";
        drivers[driverIndex].addEarnings(fare);
        history.push_back(
            Ride(riderID,
                 drivers[driverIndex].getId(),
                 pickup,
                 dest,
                 dist,
                 time,
                 base,
                 distCost,
                 timeCost,
                 surge,
                 fare));
    }

    void rateDriver(int driverID, double rating) {
        if (rating < 1.0 || rating > 5.0) {
            cout << "Rating must be between 1 and 5\n";
            return;
        }
        if (!driverIndexById.count(driverID)) {
            cout << "Driver not found\n";
            return;
        }
        int idx = driverIndexById[driverID];
        drivers[idx].updateRating(rating);
        cout << "Driver " << driverID
             << " new rating: "
             << drivers[idx].getRating() << "\n";
    }

    void riderHistory(int id) const {
        cout << "\nRides for Rider " << id << "\n";
        bool found = false;
        for (size_t i = 0; i < history.size(); i++) {
            if (history[i].getRiderID() == id) {
                cout << "Driver "
                     << history[i].getDriverID()
                     << " Trip "
                     << history[i].getPickup() << " -> " << history[i].getDestination()
                     << " Fare Rs."
                     << history[i].getTotalFare()
                     << " Distance "
                     << history[i].getDistance()
                     << " km\n";
                found = true;
            }
        }
        if (!found)
            cout << "No rides found\n";
    }
    void driverHistory(int id) const {
        cout << "\nRides by Driver " << id << "\n";
        bool found = false;
        for (size_t i = 0; i < history.size(); i++) {
            if (history[i].getDriverID() == id) {
                cout << "Rider "
                     << history[i].getRiderID()
                     << " Trip "
                     << history[i].getPickup() << " -> " << history[i].getDestination()
                     << " Fare Rs."
                     << history[i].getTotalFare()
                     << " Distance "
                     << history[i].getDistance()
                     << " km\n";
                found = true;
            }
        }
        if (!found)
            cout << "No rides found\n";
    }

    void driverEarnings() const {
        cout << "\nDriver Earnings\n";
        for (size_t i = 0; i < drivers.size(); i++) {
            cout << "Driver "
                 << drivers[i].getId()
                 << " Rs."
                 << drivers[i].getEarnings()
                 << " Rating "
                 << drivers[i].getRating()
                 << "\n";
        }
    }
};

/* ---------- SAFE INPUT ---------- */
template<typename T>
bool safeRead(T& val) {
    if (cin >> val) return true;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid input. Please try again.\n";
    return false;
}

/* ---------- MAIN ---------- */
int main() {

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    RideSystem system;
    for (int i = 1; i <= 6; i++)
        system.graph.addNode(i);

    system.graph.addEdge(1,2,4);
    system.graph.addEdge(1,3,3);
    system.graph.addEdge(2,4,5);
    system.graph.addEdge(3,4,2);
    system.graph.addEdge(3,5,4);
    system.graph.addEdge(4,6,3);
    system.graph.addEdge(5,6,2);

    system.addDriver(1,1,4.8);
    system.addDriver(2,3,4.6);
    system.addDriver(3,5,4.9);

    system.graph.displayGraph();

    int choice;
    while(true){
        cout << "\nMENU\n";
        cout << " 1  Show Graph\n";
        cout << " 2  Add Node\n";
        cout << " 3  Add Road\n";
        cout << " 4  Request Ride\n";
        cout << " 5  Simulate Traffic\n";
        cout << " 6  Rider History\n";
        cout << " 7  Driver History\n";
        cout << " 8  Driver Earnings\n";
        cout << " 9  Rate Driver\n";
        cout << "10  Exit\n";
        if (!safeRead(choice)) continue;
        if(choice==1){
            system.graph.displayGraph();
        } 
        else if(choice==2){
            int node;
            cout<<"Enter node ID: ";
            if(safeRead(node))
                system.graph.addNode(node);
        }
         else if(choice==3){
            int u,v,w;
            cout<<"Enter node1 node2 weight: ";
            if(safeRead(u) && safeRead(v) && safeRead(w))
                system.graph.addEdge(u,v,w);
        } 
        else if(choice==4){
            int r,p,d;
            cout<<"Rider ID: ";
            if(!safeRead(r)) continue;
            cout<<"Pickup node: ";
            if(!safeRead(p)) continue;
            cout<<"Destination node: ";
            if(!safeRead(d)) continue;
            system.requestRide(r,p,d);
        }
        else if(choice==5){
            system.simulateTraffic(rng);
        }
        else if(choice==6){
            int id;
            cout<<"Rider ID: ";
            if(safeRead(id))
                system.riderHistory(id);
        } 
        else if(choice==7){
            int id;
            cout<<"Driver ID: ";
            if(safeRead(id))
                system.driverHistory(id);
        } 
        else if(choice==8){
            system.driverEarnings();
        } 
        else if(choice==9){
            int id;
            double rating;
            cout<<"Driver ID: ";
            if(!safeRead(id)) continue;
            cout<<"Rating (1-5): ";
            if(safeRead(rating))
                system.rateDriver(id,rating);
        } 
        else if(choice==10){
            cout<<"Goodbye\n";
            break;
        } 
        else {
            cout<<"Invalid choice\n";
        }
    }
    return 0;
}