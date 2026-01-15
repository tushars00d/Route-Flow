# RouteFlow: OSPF Network Routing Simulator

**RouteFlow** is a C++ simulation of the **OSPF (Open Shortest Path First)** Link-State routing protocol. It models a network topology of routers and links, calculating optimal data paths using **Dijkstra’s Algorithm**. The simulator features dynamic fault tolerance, allowing it to re-route traffic in real-time when network links fail or are restored.

---

## 🚀 Features

* **Optimal Path Calculation:** Uses **Dijkstra’s Algorithm** (with `std::priority_queue`) to determine the lowest-cost path between routers in $O(E \log V)$ time.
* **Dynamic Topology Simulation:** Supports real-time addition of routers and bidirectional weighted links.
* **Network Convergence:** Simulates link failures (`linkDown`) and recoveries (`linkUp`), automatically triggering route re-calculation to find backup paths.
* **Routing Tables:** Generates and displays full routing tables for any node in the network, showing the next hop and total metric cost.
* **Standard C++ STL:** Built using modern C++ features including `unordered_map`, `priority_queue`, and `vector` for efficient memory management.

---

## 🛠️ Tech Stack

* **Language:** C++ (compatible with C++11, C++14, C++17)
* **Core Concepts:** Graph Theory, Priority Queues, Greedy Algorithms, Network Protocols (OSPF Logic).
* **Tools:** GCC/Clang Compiler, Make/CMake (optional).

---

## 📋 How It Works

1.  **Topology Creation:** The network is modeled as a weighted undirected graph where nodes represent **Routers** and edges represent **Network Links** (latency/cost).
2.  **Link-State Database:** The system maintains an adjacency list of all connections.
3.  **Pathfinding:** When a route is requested:
    * The system initializes a priority queue.
    * It explores neighbors greedily based on the lowest cumulative cost.
    * It updates the routing table with the shortest known distance to every other router.
4.  **Failover Handling:** If a link is marked as "DOWN", the algorithm ignores that edge during traversal, effectively simulating a reroute around a network outage.

---

## 💻 Usage

### 1. Compilation
You can compile the project using any standard C++ compiler (g++ or clang).

```bash
g++ -o routeflow src.cpp
