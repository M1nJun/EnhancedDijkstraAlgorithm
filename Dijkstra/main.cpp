                #include "graph.h"
#include <iostream>
#include <ctime>

int main() {
	graph G;
	// Run the Dijkstra algorithm, the og version
	G.dijkstra(0, 1);
	
    vertex *winner = nullptr;
    int maxD = 0;
    for(int n = 0; n < G.getVertexCount(); n++) {
        vertex* v = G.getVertex(n);
        if(v != nullptr && v->d > maxD && v->d != IFTY) {
            winner = v;
            maxD = v->d;
        }
    }

    std::cout << "Most distant vertex is " << winner->key << std::endl;
	std::cout << "Shortest path to " << winner->key << std::endl;
    
    
    std::list<int> L;
    G.pathTo(winner->key,L);
	auto itr = L.begin();
	while(itr != L.end()) {
		std::cout << *itr << std::endl;
		itr++;
	}
	
	std::cout << "Distance is " << winner->d << std::endl;
    
    // Run new version of the Dijkstra algorithm and do the same operations
    
    // Compare the two algorithms. Is that the same path
}
