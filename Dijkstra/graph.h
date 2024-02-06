#include <string>
#include <list>
#include <map>
#include <set>
#include <cmath>
#include <fstream>

#define IFTY 100000


// Forward declaration of vertex class
class vertex;

// Global varibale for the meeting point
vertex *m;

class edge
{
public:
	vertex *u, *v;
	int weight;

	edge(vertex* a,vertex* b,int w)
	{
		u = a; v = b; weight = w;
	}
};

// Each vertex contains within it a list of edges
// to the other vertices the vertex is connected to.
class vertex
{
public:
	int key;
	double lon, lat;
	double d1;
    double d2;
    // Introduced color
    std::string color1;
    std::string color2;
	vertex *p1;
    vertex *p2;
	std::list<edge> edges;

    vertex() {}
    vertex(int v) { key = v; d1 = IFTY; d2 = IFTY; p1 = NULL; p2 = NULL;}
};

class graph
{
public:
	graph();
	~graph();
    // Made a change to accept the start and the end vertices.
	void dijkstra(int s, int f);
	
	void pathToInverted(int middle,std::list<int>& L);
    void pathToForward(int middle,std::list<int>& L);

    int getVertexCount() { return V.size(); }
	vertex* getVertex(int n) { return V[n]; }
private:
	// Since the edges are stored in the vertices, we only need
	// a list of vertices in the graph. For convenience, this is
	// stored as a map.
	std::map<int,vertex*> V;
};

graph::graph()
{
	std::ifstream in;
	in.open("graph.txt");
	
	int nV;
	in >> nV;
	int key;
		
	for(int n = 0;n < nV;n++) {
		in >> key;
		V[key] = new vertex(key);
	}
	
	int nE;
	in >> nE;
	int from, to, w;
	for(int n = 0;n < nE;n++) {
		in >> from >> to >> w;
		vertex *u = V[from];
		vertex *v = V[to];
		u->edges.push_back(edge(u, v, w));
		v->edges.push_back(edge(v, u, w));
	}
	in.close();
}

graph::~graph() {
	auto itr = V.begin();
	while(itr != V.end()) {
		delete itr->second;
		itr++;
	}
}

// Helper class for Dijkstra's algorithm
// We need this because we are going to be storing
// pointers to vertices in a set in dijkstra. The
// set will need a comparator object to determine
// the order of elements in the set: we will want to
// order these pointers by the d values of the vertices
// that the pointers point to.
// A further hack here is some logic that treats
// two vertices with the same d value as different. We
// need this to work around a key characteristic of the
// STL set class, which is that STL set does not allow
// us to insert duplicates.
class vertex_less1
{
public:
	bool operator()(vertex* a,vertex* b) const
	{ if(a->d1 == b->d1) return a < b; else return a->d1 < b->d2; }
};

class vertex_less2
{
public:
    bool operator()(vertex* a,vertex* b) const
    { if(a->d2 == b->d2) return a < b; else return a->d2 < b->d2; }
};

void graph::dijkstra(int s, int f) {

	// Dijkstra's algorithm is driven by a priority queue.
	// Since the STL priority queue does not offer support
	// for the decrease key operation, we implement the 
	// priority queue via a set. Since the set class orders
	// its contents, we can always find the minimum priority
	// element in the set by calling begin().
	std::set<vertex*,vertex_less1 > Q1;
    std::set<vertex*,vertex_less2 > Q2;
	
	auto itr = V.begin();
	while(itr != V.end()) {
		if(itr->first != s) {
            itr->second->d1 = INFINITY;
            itr->second->p1 = NULL;
        } else {
            // question: this is will ran two time when s and f comes along as the iterator. would that be a problem?
			itr->second->d1 = 0;
			itr->second->p1 = itr->second;
		}
		Q1.insert(itr->second);
		itr++;
	}
    
    itr = V.begin();
    while(itr != V.end()) {
        if(itr->first != f) {
            itr->second->d2 = INFINITY;
            itr->second->p2 = NULL;
        } else {
            // question: this is will ran two time when s and f comes along as the iterator. would that be a problem?
            itr->second->d2 = 0;
            itr->second->p2 = itr->second;
        }
        Q2.insert(itr->second);
        itr++;
    }
    

	while(!Q1.empty() || !Q2.empty()) {
		vertex *u = *(Q1.begin());
        u->color1 = "gray";
        if(u->color2 == "gray") {
            m = u;
            break;
        }
        
        vertex *x = *(Q2.begin());
        x->color2 = "gray";
        if(x->color1 == "gray") {
            m = x;
            break;
        }
        
		Q1.erase(Q1.begin());
        Q2.erase(Q2.begin());

		std::list<edge>& edges = u->edges;
		auto itr2 = edges.begin();
		while(itr2 != edges.end()) {
            //I want to relax the vertice that I just took off from Q2
			vertex* v = itr2->v;
			if(u->d1 + itr2->weight < v->d1) {
				// Since the STL set class does not support a
				// decrease key operation, we simulate a decrease
				// key by removing v, decreasing its key, and then
				// reinserting it in the set. Since erase, find,
				// and insert are all O(lg(N)) operations in a set,
				// this is almost as fast as a legitimate decrease
				// key operation in a binomial heap.
                
				Q1.erase(Q1.find(v));
				v->d1 = u->d1 + itr2->weight;
				v->p1 = u;
				Q1.insert(v);
			}
			itr2++;
		}
        
        
        // question: I copied the for loop from above but it feels problamtic because it wouldn't really be "running at the same time". So I should have these in a single loop
        
        // question: But then I don't see how I can put it into a single loop since the iterator can iterate through only one list of edges for either the start or the final vertex
        edges = x->edges;
        itr2 = edges.begin();
        while(itr2 != edges.end()) {
            //I want to relax the vertice that I just took off from Q2
            vertex* v = itr2->v;
            if(x->d2 + itr2->weight < v->d2) {
                // Since the STL set class does not support a
                // decrease key operation, we simulate a decrease
                // key by removing v, decreasing its key, and then
                // reinserting it in the set. Since erase, find,
                // and insert are all O(lg(N)) operations in a set,
                // this is almost as fast as a legitimate decrease
                // key operation in a binomial heap.
                
                Q2.erase(Q2.find(v));
                v->d2 = x->d2 + itr2->weight;
                v->p2 = x;
                Q2.insert(v);
            }
            itr2++;
        }
        
	}
}

void graph::pathToInverted(int middle,std::list<int>& L)
{
	vertex *v = V[middle];
	if(v->d1 == 0)
		L.push_back(middle);
	else if(v->p1 == NULL)
		return;
	else {
		this->pathToInverted(v->p1->key,L);
		L.push_back(middle);
	}
}

void graph::pathToForward(int middle,std::list<int>& L)
{
    vertex *currentVertex = V[middle];
    
    while (currentVertex != NULL) {
        L.push_back(currentVertex->key);
        
        currentVertex = currentVertex->p2;
    }
    
}
