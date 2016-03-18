#ifndef _RANDOMGRAPH_CPP
#define _RANDOMGRAPH_CPP

#include "RAGGenerator.hpp"

RAGGenerator :: RAGGenerator()
{
	cout<<"Improper constructor call. Call with required graph parameters";
	exit( 0 );
}

RAGGenerator :: RAGGenerator(int64_t n, int64_t i, int64_t o, double c, double a, double b, double g, bool h)
{
	numberOfNodes = n;
	inDegree = i;
	outDegree = o;
	CCR = c;
	alpha = a;
	beta = b;
	gamma = g;
	heftStyleGraphs = h;

	if( heftStyleGraphs == true )
		numberOfConstraints = 1;
	else
		numberOfConstraints = 2;

	for(int i=0; i<n; ++i)
	{
		ConnectivityMatrix tempVector;
		randomGraph.connectivityMatrix.push_back( tempVector );
	}
	randomGraph.singleConstraintVwgt.resize( n, 0 );
	randomGraph.vwgt.resize( n * numberOfConstraints, 0 );

	// RGG-low
	highPriorityConstraint.first=pow(10,3);
	highPriorityConstraint.second=pow(10,4);
	lowPriorityConstraint.first=pow(10,2);
	lowPriorityConstraint.second=pow(10,3);

//	// RGG-medium
//	highPriorityConstraint.first=pow(10,4);
//	highPriorityConstraint.second=pow(10,5);
//	lowPriorityConstraint.first=pow(10,2);
//	lowPriorityConstraint.second=pow(10,3);

//	// RGG-high
//	highPriorityConstraint.first=pow(10,5);
//	highPriorityConstraint.second=pow(10,6);
//	lowPriorityConstraint.first=pow(10,2);
//	lowPriorityConstraint.second=pow(10,3);
}

int64_t // Returns random number in range [a,b]
randomInRange( int64_t a, int64_t b )
{
	return( (b-a == 0)? b : a + rand()%(b-a+1) );
}

int
RAGGenerator :: printDOTFile()
{
	filename = string("app-");
	ostringstream oss;
	oss<<filename<<numberOfNodes<<"-"<<outDegree<<"-"<<CCR<<"-"<<alpha<<"-"<<beta<<"-"<<gamma << ".dot";
	filename = oss.str();
	fstream file( filename.c_str(), fstream::out );
	file<<"digraph{\n";
	file<<"\tsplines=spline\n\tsplines=true\n\tarrowType=open\n";
	file<<"\n// Printing vertex IDs and attributes\n";
	for(int64_t i=0; i<height; ++i)
	{
		for(int64_t j=0; j<nodesInList[i].size(); ++j)
		{
			file<<"\t"<<(nodesInList[i][j] + 1)<<"[label=\""<<(nodesInList[i][j] + 1)<<"(";
			if( i==0 || i==height-1)
			{
				if(randomGraph.ncon == 2)
					file<<randomGraph.vwgt[ randomGraph.ncon*nodesInList[i][j] + 0 ]<<","<<randomGraph.vwgt[ randomGraph.ncon*nodesInList[i][j] + 1 ]<<")\",shape=rounded]\n";
				else
					file<<randomGraph.singleConstraintVwgt[ nodesInList[i][j] ]<<")\",shape=rounded]\n";	
			}
			else
			{
				if(randomGraph.ncon == 2)
					file<<randomGraph.vwgt[ randomGraph.ncon*nodesInList[i][j] + 0 ]<<","<<randomGraph.vwgt[ randomGraph.ncon*nodesInList[i][j] + 1 ]<<")\"]\n";
				else
					file<<randomGraph.singleConstraintVwgt[ nodesInList[i][j] ]<<")\"]\n";
			}
		}

	}
	file<<"\n// Printing edge IDs and attributes\n";
	for(int64_t i=0; i<numberOfNodes; ++i)
	{
		int64_t numberOfNeighbours = randomGraph.connectivityMatrix[i].size();
		for(int64_t j=0; j<numberOfNeighbours; ++j)
		{
			file<<"\t"<<(i + 1)<<" -> "<<(randomGraph.connectivityMatrix[i].list[j].first + 1);
			file<<" [label=\""<< randomGraph.connectivityMatrix[i].list[j].second <<"\"]\n";
		}
	}
	file<<"}\n";
	file.close();
}

bool
RAGGenerator :: hasMultipleHeads()
{
	DEBUG1(cout<<"\n[hasMultipleHeads] - Entering";)

	bool retVal=true;
	int64_t i=0, j=0;
	GraphIR randomGraphTranspose;
	for(i=0; i<numberOfNodes; ++i)
	{
		ConnectivityMatrix tempConnectivityMatrix;
		randomGraphTranspose.connectivityMatrix.push_back(tempConnectivityMatrix);
	}
	for(i=0; i<numberOfNodes; ++i)
	{
		int64_t parent=i;
		int64_t numberOfSuccessors=randomGraph.connectivityMatrix[ parent ].size();
		for(j=0; j<numberOfSuccessors; ++j)
		{
			int64_t currentNode=randomGraph.connectivityMatrix[ parent ].list[j].first;
			double parentComm=randomGraph.connectivityMatrix[ parent ].list[j].second;
			pair <int64_t, double> temp(parent, parentComm);
			randomGraphTranspose.connectivityMatrix[ currentNode ].list.push_back( temp );
		}
	}
	for(i=1; i<numberOfNodes; ++i)
	{
		int64_t currentNode = i;
		DEBUG4(cout<<"\n[hasMultipleHeads] - T["<<currentNode<<"].size()="<<randomGraphTranspose.connectivityMatrix[ currentNode ].size()<<" | ";)
		for(j=0; j<randomGraphTranspose.connectivityMatrix[ currentNode ].size(); ++j)
			DEBUG4(cout<<" "<<randomGraphTranspose.connectivityMatrix[ currentNode ].list[j].first;)

		if( randomGraphTranspose.connectivityMatrix[ currentNode ].size() == 0 ) // If currentNode has no parent
		{
			retVal=false;
			DEBUG4(cout<<"\n[hasMultipleHeads] - Faulty node that has no parent is T["<<currentNode<<"] | size()="<<randomGraphTranspose.connectivityMatrix[ currentNode ].size();)
			break;
			// int64_t t1 = randomInRange(0, i-1);
			// int64_t t2 = randomInRange(0, nodesInList[t1].size()-1);
			
			// pair < int64_t, double > temp;
			// temp.first = currentNode;
			// temp.second = ceil( randomGraph.singleConstraintVwgt[ nodesInList[t1][t2] ] * CCR );
			// randomGraph.connectivityMatrix[ nodesInList[t1][t2] ].list.push_back( temp );
			// DEBUG2(cout<<"\nT["<<currentNode<<"] doesnt have any parents | forming a connection from T["<<nodesInList[t1][t2]<<"] to "<<currentNode;)
			// numberOfEdges++;
		}
	}

	DEBUG1(cout<<"\n[hasMultipleHeads] - Exiting";)
	return retVal;
}

bool
RAGGenerator :: hasMultipleEdgesBetweenNodes()
{
	DEBUG1(cout<<"\n[hasMultipleEdgesBetweenNodes] - Entering";)

	bool retVal=true;
	int64_t i=0, j=0;
	for(i=0; i<numberOfNodes; ++i)
	{
		int64_t currentNode=i, numberOfSuccessors=randomGraph.connectivityMatrix[currentNode].size();
		vector<int64_t> successorList;
		for(j=0; j<numberOfSuccessors; ++j)
		{
			int64_t successor=randomGraph.connectivityMatrix[currentNode].list[j].first;
			if( find( successorList.begin(), successorList.end(), successor ) == successorList.end() )
				successorList.push_back( successor );
			else
			{
				cout<<"\n[hasMultipleEdgesBetweenNodes] - Node["<<i<<"] has multiple edges to Node["<<successor<<"]";
				retVal=false;
				goto exitFlag;
			}
		}
	}

	exitFlag:
	DEBUG1(cout<<"\n[hasMultipleEdgesBetweenNodes] - Exiting";)
	return retVal;
}

bool
RAGGenerator :: checkMETISFile(  )
{
	DEBUG1(cout<<"\n[checkMETISFile] - Entering";)

	bool retVal=true;
	GraphIR inputGraph;
	inputGraph.parseMETIS( filename );
	int64_t i=0, j=0;

	if( inputGraph.numberOfVertices != numberOfNodes )
	{
		DEBUG2(cout<<"\n[checkMETISFile] - inputGraph.numberOfVertices="<<inputGraph.numberOfVertices<<" != randomGraph.numberOfNodes="<<numberOfNodes;)
		retVal=false;
		goto retFlag;
	}

	for(i=0; i<inputGraph.numberOfVertices; ++i)
	{
		if( inputGraph.connectivityMatrix[i].size() != randomGraph.connectivityMatrix[i].size() )
		{
			DEBUG2(cout<<"\n[checkMETISFile] - Task["<<i<<"] has "<<inputGraph.connectivityMatrix[i].size()<<" children in inputGraph and "<<randomGraph.connectivityMatrix[i].size()<<" children in randomGraph";)
			retVal=false;
			goto retFlag;
		}
		for(j=0; j<inputGraph.connectivityMatrix[i].size(); ++j)
		{
			if( inputGraph.connectivityMatrix[i].list[j].first == randomGraph.connectivityMatrix[i].list[j].first )
			{
				if( inputGraph.connectivityMatrix[i].list[j].second != randomGraph.connectivityMatrix[i].list[j].second )
				{
					DEBUG2(cout<<"\n[checkMETISFile] - i="<<i<<" | j="<<j<<"\ninputGraph.connectivityMatrix["<<i<<"].list["<<j<<"].second="<<inputGraph.connectivityMatrix[i].list[j].second<<" | randomGraph.connectivityMatrix["<<i<<"].list["<<j<<"].second="<<randomGraph.connectivityMatrix[i].list[j].second;)
					retVal=false;
					goto retFlag;
				}
			}
			else
			{
				DEBUG2(cout<<"\n[checkMETISFile] - i="<<i<<" | j="<<j<<"\ninputGraph.connectivityMatrix["<<i<<"].list["<<j<<"].first="<<inputGraph.connectivityMatrix[i].list[j].first<<" | randomGraph.connectivityMatrix["<<i<<"].list["<<j<<"].first="<<randomGraph.connectivityMatrix[i].list[j].first;)
				retVal=false;
				goto retFlag;
			}

		}
	}

	retFlag:
	DEBUG1(cout<<"\n[checkMETISFile] - Exiting";)
	return retVal;
}

int
RAGGenerator :: run()
{
	DEBUG1(cout<<"\n[RAGGenerator::run] - Entering";)

	srand( time(NULL) );
	int i=0, j=0;
	level=0;
	int64_t numberOfEdges = 0;
	int64_t nodesAllocated = 0;
	double off = 0.3;

	cout<<fixed;
	// Calculate height and width of the graph
	height = ceil( sqrt( numberOfNodes )/alpha );
	avgWidth = ceil( sqrt( numberOfNodes ) * alpha );
	double applicationGraphBeta = (double) beta/100;
	DEBUG1(cout<<"\n[run] - height="<<height<<" | avgWidth="<<avgWidth;)
	for(level=0; ; level++)
	{
		vector < int64_t > buffer( 0 );
		nodesInList.push_back( buffer );
		if( numberOfNodes - nodesAllocated < avgWidth )
		{
			// In the last level, according to height, see how many nodesToBeAllocated
			// If there are more than wat can fit in one level, add one to height and continue
			while(numberOfNodes - nodesAllocated > 0)
			{
				nodesInList[level].push_back(nodesAllocated);
				++nodesAllocated;
			}
			++level;
			height = level;
			break;
		}
		else
		{
			// Assign one node to starting level.
			if( level == 0 )
			{
				nodesInList[level].push_back( 0 );
				++nodesAllocated;
			}
			else
			{
				// Assign between 1 and 2*avgWidth nodes in level i.
				int offset = randomInRange( avgWidth*(1-off), avgWidth*(1+off));
				for(i=0; i<offset; ++i)
				{
					nodesInList[level].push_back(nodesAllocated);
					++nodesAllocated;
					if(nodesAllocated == numberOfNodes)
						goto jumpOut;
				}
			}
		}
	}
	jumpOut:
	DEBUG1(cout<<"\nO/P: nodesInList.size="<<nodesInList.size()<<"level="<<level<<" height="<<height<<" width="<<avgWidth<<"nodesAllocated="<<nodesAllocated<<endl;)

	DEBUG1(cout<<"\nHeight before pruning : "<<height;)
	vector < vector < int64_t > >::iterator it1;
	for(it1=nodesInList.begin(); it1!=nodesInList.end(); ++it1)
		if((*it1).size() == 0)
		{
			DEBUG2(cout<<"***yoink***";)
			nodesInList.erase(it1);
			it1--;
		}
	height = nodesInList.size();

	double weightOfDAG = randomInRange( pow(10,4), pow(10,5) );
	// Assign vertex weights
	for(i=0; i<numberOfNodes; ++i)
	{
		randomGraph.singleConstraintVwgt[ i ] = 1;
		if(numberOfConstraints==1)
		{
//			randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = 1;
			randomGraph.singleConstraintVwgt[ i ] = randomGraph.vwgt[ i ] = randomInRange( 0, 2*weightOfDAG );
		}
		else if(numberOfConstraints==2)
		{
			// This is a GPU task {second constraint higher than first}
			if( randomInRange(0, 100) < beta )
			{
				randomGraph.vwgt[ i*numberOfConstraints + 0 ] = randomInRange( lowPriorityConstraint.first, lowPriorityConstraint.second );
				randomGraph.vwgt[ i*numberOfConstraints + 1 ] = randomInRange( highPriorityConstraint.first, highPriorityConstraint.second );
				randomGraph.singleConstraintVwgt[ i ] = randomGraph.vwgt[ i*numberOfConstraints + 0 ] + randomGraph.vwgt[ i*numberOfConstraints + 1 ];	
			}

			// This is CPU task
			else
			{
				randomGraph.vwgt[ i*numberOfConstraints + 0 ] = randomInRange( highPriorityConstraint.first, highPriorityConstraint.second );
				randomGraph.vwgt[ i*numberOfConstraints + 1 ] = randomInRange( lowPriorityConstraint.first, lowPriorityConstraint.second );
				randomGraph.singleConstraintVwgt[ i ] = randomGraph.vwgt[ i*numberOfConstraints + 0 ] + randomGraph.vwgt[ i*numberOfConstraints + 1 ];	
			}
		}
		else
		{
			cout<<"\n[run] - Unknown numberOfConstraints value. Exiting.";
			exit(0);
		}
	}


	// Next loop to form connections between nodes in the different levels.
	// In each iteration, go through all nodes in level i.
	for(i=0; i<height-1; ++i)
	{
		DEBUG2(cout<<"\nLevel"<<i;)
		vector < int64_t >::iterator it;
		for(it = nodesInList[i].begin(); it!= nodesInList[i].end(); ++it)
		{
			int64_t numberOfOutDegree = randomInRange( 1, outDegree*(1+off) );
			DEBUG2(cout<<"\n\tnumberOfOutDegree="<<numberOfOutDegree;)
			// For each such node j, make a connection to k where k's level is >= j's level.
			// Choose 1 to outDegree such k's for each j.

			vector < pair<int64_t, int64_t> > remainingNodes;
			for( j=i+1; j<height; ++j)
				for( int k=0; k<nodesInList[j].size(); ++k)
				{
					pair<int64_t, int64_t> temp(j, k);
					remainingNodes.push_back( temp );
				}

			if( numberOfOutDegree>remainingNodes.size() )
			{
				for(j=0; j<remainingNodes.size(); ++j)
				{
					pair < int64_t, double > temp;
					temp.first = nodesInList[ remainingNodes[j].first ][ remainingNodes[j].second ];
					//temp.second = ceil( randomGraph.singleConstraintVwgt[ (*it) ] * CCR );
					temp.second = ceil(randDoubleInRange( randomGraph.singleConstraintVwgt[ (*it) ] * CCR * ( 1-applicationGraphBeta/2 ), randomGraph.singleConstraintVwgt[ (*it) ] * CCR * ( 1+applicationGraphBeta/2 ) ));
					randomGraph.connectivityMatrix[(*it)].list.push_back( temp );
					numberOfEdges++;
				}
			}
			else
			{
				for(j=0; j<numberOfOutDegree; ++j)
				{
					pair < int64_t, double > temp;
					int64_t chosenIndex=rand()%remainingNodes.size();
					temp.first = nodesInList[ remainingNodes[chosenIndex].first ][ remainingNodes[chosenIndex].second ];
					temp.second = ceil(randDoubleInRange( randomGraph.singleConstraintVwgt[ (*it) ] * CCR * ( 1-applicationGraphBeta/2 ), randomGraph.singleConstraintVwgt[ (*it) ] * CCR * ( 1+applicationGraphBeta/2 ) ));
					//temp.second = ceil( randomGraph.singleConstraintVwgt[ (*it) ] * CCR );
					randomGraph.connectivityMatrix[(*it)].list.push_back( temp );
					numberOfEdges++;
					remainingNodes.erase( remainingNodes.begin()+chosenIndex );
				}
			}

			// for(j=0; j<numberOfOutDegree; ++j)
			// {
			// 	int64_t t1 = randomInRange(i+1, height-1);
			// 	int64_t t2 = randomInRange(0, nodesInList[t1].size()-1);
			// 	DEBUG3(cout<<" | ("<<t1<<","<<t2<<")";)			
			// 	pair < int64_t, double > temp;
			// 	temp.first = nodesInList[t1][t2];
			// 	temp.second = ceil( randomGraph.singleConstraintVwgt[ (*it) ] * CCR );
			// 	randomGraph.connectivityMatrix[(*it)].list.push_back( temp );
			// 	numberOfEdges++;
			// }
		}
	}

	// Create transpose adjacency lists to get parents of all nodes
	GraphIR randomGraphTranspose;
	for(i=0; i<numberOfNodes; ++i)
	{
		ConnectivityMatrix tempConnectivityMatrix;
		randomGraphTranspose.connectivityMatrix.push_back(tempConnectivityMatrix);
	}
	for(i=0; i<numberOfNodes; ++i)
	{
		for(int k=0; k<randomGraph.connectivityMatrix[i].size(); ++k)
		{
			pair < int64_t, double > tempPair;
			tempPair.first = i;
			tempPair.second = randomGraph.connectivityMatrix[i].list[k].second;
			randomGraphTranspose.connectivityMatrix[ randomGraph.connectivityMatrix[i].list[k].first ].list.push_back(tempPair);
		}
	}
	
	for(i=1; i<height; ++i)
	{
		// Final pass to clean up communication.
		// Iterate once over to figure out if there are any straddlers, i.e. nodes with 0 inDegree.
		// If there are any such nodes called i, make one connection to it from some random node j, where level(j) < level(i);
		int flip = 0;
		for(j=0; j<nodesInList[i].size(); ++j)
		{
			int64_t currentNode = nodesInList[i][j];
			if( randomGraphTranspose.connectivityMatrix[ currentNode ].size() == 0 ) // If currentNode has no parent
			{
				int64_t t1 = randomInRange(0, i-1);
				int64_t t2 = randomInRange(0, nodesInList[t1].size()-1);
				
				pair < int64_t, double > temp;
				temp.first = currentNode;
				temp.second = ceil(randDoubleInRange( randomGraph.singleConstraintVwgt[ nodesInList[t1][t2] ] * CCR * ( 1-applicationGraphBeta/2 ), randomGraph.singleConstraintVwgt[ nodesInList[t1][t2] ] * CCR * ( 1+applicationGraphBeta/2 ) ));
				//temp.second = ceil( randomGraph.singleConstraintVwgt[ nodesInList[t1][t2] ] * CCR );
				randomGraph.connectivityMatrix[ nodesInList[t1][t2] ].list.push_back( temp );
				DEBUG2(cout<<"\nT["<<currentNode<<"] doesnt have any parents | forming a connection from T["<<nodesInList[t1][t2]<<"] to "<<currentNode;)
				numberOfEdges++;
			}
		}
	}

	// Some nodes in the same level were not 

	DEBUG1(cout<<"\nHeight after pruning and introducing comm links: "<<height<<endl;)
	for(i=0; i<height; ++i)
	{
		DEBUG3(cout<<"Level "<<i+1<<" : ";)
		vector < int64_t >::iterator it;
		for(it = nodesInList[i].begin(); it!= nodesInList[i].end(); ++it)
		{
			DEBUG3(cout<<(*it)<<"-";)
		}
		DEBUG3(cout<<"\n";)
	}

	filename=string("app-");
	ostringstream oss;
	oss<<filename<<numberOfNodes<<"-"<<outDegree<<"-"<<CCR<<"-"<<alpha<<"-"<<beta<<"-"<<gamma << ".grf";
	filename = oss.str();	
	DEBUG1(cout<<"\nNumber of Edges = "<<numberOfEdges<<endl;)
	DEBUG1(cout<<"\nChecking for correctness of graph.\n\t1. Multiple heads - ";)
	if( hasMultipleHeads() == false )
		cout<<"Failed!\n\n+++++\nFile "<<filename<<"has mutliple heads.\n\n";
	else
		cout<<"Passed!";
	DEBUG1(cout<<"\n\t2. Multiple edges between nodes - ";)
	if( hasMultipleEdgesBetweenNodes() == false )
		cout<<"Failed!\n\n+++++\nFile "<<filename<<" has multiple edges between tasks.\n\n";
	else
		cout<<"Passed!";

	int seed = rand()%1000000;
	randomGraph.createMETISFile(numberOfNodes, numberOfEdges, numberOfConstraints, filename, seed, beta, heftStyleGraphs);
	DEBUG1(cout<<"\n\t3. Self check on .grf file";)
	if( checkMETISFile() == false )
		cout<<"Failed!\n\n+++++\n"<<filename<<" failed\n\n";
	else
		cout<<"Passed!";

	printDOTFile();

	DEBUG1(cout<<"\n[RAGGenerator::run] - Exiting\n";)
}

//GraphIR createRandomGraphForApplication(GraphIR application, )
//{
//	GraphIR retVal=application;
//	
//
//	return retVal;
//}

int
main(int argc, char *argv[])
{
	if(argc == 2)
	{

	}
	if(argc != 8 && argc != 2)
	{
		cout<<"\nImproper Usage. Arguments are as follows : ";
		cout<<"\n1. (n) Number of Nodes \\in Z^+";
		cout<<"\n2. (i) Indegree \\in Z^+ && i<n (ideally i<<n; currently not taken into account)";
		cout<<"\n3. (o) Outdegree \\in Z^+ && o<n (ideally o<<n)";
		cout<<"\n4. (c) Computation-to-Communication Ratio(CCR) \\in Q^+ (ideally c<<1)";
		cout<<"\n5. (a) Alpha \\in Q^+\n\tSmaller 'a' gives tall and skinny graphs, while larger 'a' gives fat and short graphs";
		cout<<"\n6. (b) Beta \\in Q^+ && 0<=b<100\n\tBeta determines how serial/parallel the graph is going to be. Smaller values of beta makes the actors 'for' loop intensive and larger values of beta makes the graph 'par' loop intensive.";
		cout<<"\n7. (g) Gamma \\in Q^+ && 0<=g<=100\n\tGamma decides how computation is spread across the graph. Smaller values of gamma gives uniformly distributed graphs while larger values give skewed graphs.\n";
		exit( 0 );
	}
	
	RAGGenerator g( atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]), true);
	g.run();
}

#endif
