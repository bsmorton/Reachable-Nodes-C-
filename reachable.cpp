// Submitter: bsmorton(Morton, Bradley)
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <w32api/docobjectservice.h>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"


typedef ics::ArraySet<std::string>          NodeSet;
typedef ics::pair<std::string,NodeSet>      GraphEntry;

bool graph_entry_gt (const GraphEntry& a, const GraphEntry& b)
{return a.first<b.first;}

typedef ics::ArrayPriorityQueue<GraphEntry,graph_entry_gt> GraphPQ;
typedef ics::ArrayMap<std::string,NodeSet>  Graph;


//Read an open file of edges (node names separated by semicolons, with an
//  edge going from the first node name to the second node name) and return a
//  Graph (Map) of each node name associated with the Set of all node names to
//  which there is an edge from the key node name.

bool b1(const ics::pair<std::string,ics::ArraySet<std::string>>& a, const ics::pair<std::string,ics::ArraySet<std::string>>& b)
{return a.first < b.first;}

Graph read_graph(std::ifstream &file) {
	Graph graph;
    std::string line;
    ics::ArraySet<std::string> temp;
    ics::ArrayPriorityQueue<ics::pair<std::string,ics::ArraySet<std::string>>,b1> temp2;
    while(std::getline(file,line)){
        std::vector<std::string> spline=ics::split(line,";");
        std::string t1=spline[0];
        std::string t2=spline[1];
        if(graph.has_key(t1)==1){
            graph[t1].insert(t2);
        }
        else{
            graph.put(t1,temp);
            temp.insert(t2);
            graph[t1].insert(t2);
            temp.clear();
        }

    }
    temp2.enqueue_all(graph);
    graph.clear();
    graph.put_all(temp2);
    return graph;
}


//Print a label and all the entries in the Graph in alphabetical order
//  (by source node).
//Use a "->" to separate the source node name from the Set of destination
//  node names to which it has an edge.
void print_graph(const Graph& graph) {
    std::cout << "Graph: source -> set[destination nodes]" << std::endl;
    for(ics::pair<std::string,ics::ArraySet<std::string>>& j:graph){
        std::cout << "  " << j.first << " -> " << j.second << std::endl;
    }
}


//Return the Set of node names reaching in the Graph starting at the
//  specified (start) node.
//Use a local Set and a Queue to respectively store the reachable nodes and
//  the nodes that are being explored.
NodeSet reachable(const Graph& graph, std::string start) {
    NodeSet s;
    ics::ArrayQueue<std::string> q;
    q.enqueue(start);
    std::string temp;
    while (q.empty()==0){
        temp=q.dequeue();
        if (graph.has_key(temp)==0){
            std::cout << "  " << temp << " is not a source node name in the graph" << std::endl;
            return s;
        }
        s.insert(temp);
        for(std::string j:graph[temp]){
            if (s.contains(j)==0){
                if (graph.has_key(j)==1){
                    q.enqueue(j);
                }
                else{
                    s.insert(j);
                }
            }
        }
    }
    return s;
}





//Prompt the user for a file, create a graph from its edges, print the graph,
//  and then repeatedly (until the user enters "quit") prompt the user for a
//  starting node name and then either print an error (if that the node name
//  is not a source node in the graph) or print the Set of node names
//  reachable from it by using the edges in the Graph.
int main() {
  try {
      std::string file;
      std::cout << "Enter a graph file's name: ";
      std::getline(std::cin,file);
      if (file=="quit"){
          return 0;
      }
      std::ifstream g(file);
      Graph graph=read_graph(g);
      print_graph(graph);
      while (True){
          std::string node;
          std::cout << "Enter a starting node's name: ";
          std::getline(std::cin,node);
          if (node=="quit"){
              break;
          }
          NodeSet n=reachable(graph,node);
          if(n.empty()==0){
              std::cout << "Reachable from node name " << node << " = " << n << std::endl;
          }
      }


  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
