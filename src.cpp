#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// Edge structure representing a network link
struct Edge {
    string dest;
    int cost;
    bool isUp;
    
    Edge(string d, int c) : dest(d), cost(c), isUp(true) {}
};

// Node for priority queue in Dijkstra's algorithm
struct Node {
    string id;
    int dist;
    
    Node(string i, int d) : id(i), dist(d) {}
    
    bool operator>(const Node& other) const {
        return dist > other.dist;
    }
};

// OSPF Network Simulator Class
class RouteFlow {
private:
    unordered_map<string, vector<Edge>> graph;
    unordered_set<string> nodes;
    
public:
    // Add a router to the network
    void addRouter(const string& router) {
        nodes.insert(router);
        if (graph.find(router) == graph.end()) {
            graph[router] = vector<Edge>();
        }
    }
    
    // Add a bidirectional link between two routers
    void addLink(const string& src, const string& dest, int cost) {
        addRouter(src);
        addRouter(dest);
        graph[src].push_back(Edge(dest, cost));
        graph[dest].push_back(Edge(src, cost));
        cout << "Link added: " << src << " <-> " << dest 
             << " (cost: " << cost << ")" << endl;
    }
    
    // Simulate link failure
    void linkDown(const string& src, const string& dest) {
        bool found = false;
        
        for (auto& edge : graph[src]) {
            if (edge.dest == dest) {
                edge.isUp = false;
                found = true;
                break;
            }
        }
        
        for (auto& edge : graph[dest]) {
            if (edge.dest == src) {
                edge.isUp = false;
                break;
            }
        }
        
        if (found) {
            cout << "\n[FAILOVER] Link DOWN: " << src << " <-> " << dest << endl;
        }
    }
    
    // Restore a failed link
    void linkUp(const string& src, const string& dest) {
        bool found = false;
        
        for (auto& edge : graph[src]) {
            if (edge.dest == dest) {
                edge.isUp = true;
                found = true;
                break;
            }
        }
        
        for (auto& edge : graph[dest]) {
            if (edge.dest == src) {
                edge.isUp = true;
                break;
            }
        }
        
        if (found) {
            cout << "\n[RECOVERY] Link UP: " << src << " <-> " << dest << endl;
        }
    }
    
    // Dijkstra's Algorithm - O(E log V) using priority queue
    pair<unordered_map<string, int>, unordered_map<string, string>> 
    dijkstra(const string& source) {
        unordered_map<string, int> dist;
        unordered_map<string, string> prev;
        priority_queue<Node, vector<Node>, greater<Node>> pq;
        
        // Initialize distances
        for (const auto& node : nodes) {
            dist[node] = numeric_limits<int>::max();
        }
        dist[source] = 0;
        
        pq.push(Node(source, 0));
        
        while (!pq.empty()) {
            Node curr = pq.top();
            pq.pop();
            
            // Skip if we've found a better path already
            if (curr.dist > dist[curr.id]) continue;
            
            // Check all neighbors
            for (const auto& edge : graph[curr.id]) {
                // Skip down links
                if (!edge.isUp) continue;
                
                int newDist = dist[curr.id] + edge.cost;
                
                if (newDist < dist[edge.dest]) {
                    dist[edge.dest] = newDist;
                    prev[edge.dest] = curr.id;
                    pq.push(Node(edge.dest, newDist));
                }
            }
        }
        
        return {dist, prev};
    }
    
    // Reconstruct path from source to destination
    vector<string> getPath(const string& src, const string& dest,
                          const unordered_map<string, string>& prev) {
        vector<string> path;
        string curr = dest;
        
        if (prev.find(dest) == prev.end() && dest != src) {
            return path; // No path exists
        }
        
        while (curr != src) {
            path.push_back(curr);
            if (prev.find(curr) == prev.end()) break;
            curr = prev.at(curr);
        }
        path.push_back(src);
        reverse(path.begin(), path.end());
        
        return path;
    }
    
    // Display routing table for a source router
    void displayRoutingTable(const string& source) {
        auto [dist, prev] = dijkstra(source);
        
        cout << "\n========================================" << endl;
        cout << "ROUTING TABLE FOR ROUTER: " << source << endl;
        cout << "========================================" << endl;
        cout << left << setw(15) << "Destination" 
             << setw(10) << "Cost" 
             << "Path" << endl;
        cout << "----------------------------------------" << endl;
        
        vector<string> sortedNodes(nodes.begin(), nodes.end());
        sort(sortedNodes.begin(), sortedNodes.end());
        
        for (const auto& node : sortedNodes) {
            if (node == source) continue;
            
            cout << left << setw(15) << node;
            
            if (dist[node] == numeric_limits<int>::max()) {
                cout << setw(10) << "INF" << "No path available" << endl;
            } else {
                cout << setw(10) << dist[node];
                vector<string> path = getPath(source, node, prev);
                for (size_t i = 0; i < path.size(); i++) {
                    cout << path[i];
                    if (i < path.size() - 1) cout << " -> ";
                }
                cout << endl;
            }
        }
        cout << "========================================\n" << endl;
    }
    
    // Find optimal route between two routers
    void findRoute(const string& src, const string& dest) {
        cout << "\n>>> Finding route from " << src << " to " << dest << endl;
        
        auto [dist, prev] = dijkstra(src);
        vector<string> path = getPath(src, dest, prev);
        
        if (path.empty() || (path.size() == 1 && src != dest)) {
            cout << "❌ NO ROUTE AVAILABLE" << endl;
            return;
        }
        
        cout << "✓ Optimal Path: ";
        for (size_t i = 0; i < path.size(); i++) {
            cout << path[i];
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << "\n✓ Total Cost: " << dist[dest] << endl;
    }
    
    // Display network topology
    void displayTopology() {
        cout << "\n========================================" << endl;
        cout << "NETWORK TOPOLOGY" << endl;
        cout << "========================================" << endl;
        
        for (const auto& [router, edges] : graph) {
            cout << router << " connects to:" << endl;
            for (const auto& edge : edges) {
                cout << "  -> " << edge.dest 
                     << " (cost: " << edge.cost 
                     << ", status: " << (edge.isUp ? "UP" : "DOWN") 
                     << ")" << endl;
            }
            cout << endl;
        }
        cout << "========================================\n" << endl;
    }
};

// Demo function
int main() {
    RouteFlow network;
    
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║  RouteFlow: OSPF Network Simulator     ║" << endl;
    cout << "║  Link-State Routing with Dijkstra      ║" << endl;
    cout << "╚════════════════════════════════════════╝\n" << endl;
    
    // Build mesh network topology
    cout << "Building network topology..." << endl;
    network.addLink("R1", "R2", 4);
    network.addLink("R1", "R3", 2);
    network.addLink("R2", "R3", 1);
    network.addLink("R2", "R4", 5);
    network.addLink("R3", "R4", 8);
    network.addLink("R3", "R5", 10);
    network.addLink("R4", "R5", 2);
    network.addLink("R4", "R6", 6);
    network.addLink("R5", "R6", 3);
    
    // Display topology
    network.displayTopology();
    
    // Show routing table for R1
    network.displayRoutingTable("R1");
    
    // Test routing
    network.findRoute("R1", "R6");
    network.findRoute("R1", "R5");
    
    // Simulate link failure
    cout << "\n\n========== SIMULATING LINK FAILURE ==========" << endl;
    network.linkDown("R1", "R3");
    
    // Show updated routing
    network.displayRoutingTable("R1");
    network.findRoute("R1", "R6");
    
    // Simulate another failure
    network.linkDown("R2", "R3");
    network.findRoute("R1", "R5");
    
    // Restore links
    cout << "\n\n========== RESTORING NETWORK ==========" << endl;
    network.linkUp("R1", "R3");
    network.linkUp("R2", "R3");
    
    network.displayRoutingTable("R1");
    network.findRoute("R1", "R6");
    
    cout << "\n✓ Simulation complete!" << endl;
    
    return 0;
}