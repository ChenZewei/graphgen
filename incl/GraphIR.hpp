#ifndef _GRAPHIR_HPP
#define _GRAPHIR_HPP

#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "Commons.hpp"
//#include "Globals.h"

class GraphIR
{
	public:
	int64_t numberOfVertices,					// Number of vertices in G(V, E)
	    	ncon;					// Number of connections in G(V, E)
	std::vector < int64_t > vwgt; 	// Constraint vector.
	std::vector < int64_t > singleConstraintVwgt; 	// Constraint vector.
	std::vector < ConnectivityMatrix > connectivityMatrix;
	std::vector < int64_t > IDMap;
	GraphIR operator = (GraphIR);

	int parseMETIS(std::string fileName, int offset=1);
	int parseXML(std::string fileName);
	int createMETISFile( int64_t numberOfNodes, int64_t numberOfEdges, int ncon, std::string fileName, int64_t seed=0, int beta=0, bool heftStyleGraphs=false); 
	int makeGraphDirected();
	
	GraphIR();
	~GraphIR();
};

#endif
