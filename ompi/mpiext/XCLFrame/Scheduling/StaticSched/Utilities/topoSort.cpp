
#include "utilities.h"
using namespace std;

void topologicalSortUtil(int v, bool visited[], list<pair<int,int> > * adj, stack<int> &Stack)
{
    // Mark the current node as visited.
    visited[v] = true;

    // Recur for all the vertices adjacent to this vertex
    list<pair<int,int> >::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[(*i).first])
            topologicalSortUtil((*i).first, visited, adj, Stack);

    // Push current vertex to stack which stores result
    Stack.push(v);
}

// inputs: DAG in adjacency lists form.
// Output: List (topological sorted vertices)

int topoSort(int V, list<pair<int,int> > * adj, stack<int> &Stack){

	//stack<int> Stack;
	// Mark all the vertices as not visited
	bool *visited = new bool[V];
	for (int i = 0; i < V; i++)
		visited[i] = false;

	// Call the recursive helper function to store Topological
	// Sort starting from all vertices one by one
	for (int i = 0; i < V; i++)
		if (visited[i] == false)
			topologicalSortUtil(i, visited, adj, Stack);

	return 0;
}
