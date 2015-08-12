#ifndef _GRAPHIR_CPP
#define _GRAPHIR_CPP

#include "GraphIR.hpp"

using namespace std;

int
GraphIR::makeGraphDirected()
{
	int64_t i=0, j=0, numberOfSuccessors, k=0;
	for(i=0; i<numberOfVertices; ++i)
	{
		numberOfSuccessors = connectivityMatrix[i].size();
		for(j=0; j<numberOfSuccessors; ++j)
		{
			int64_t childID = connectivityMatrix[i].list[j].first;
			int64_t childsSuccessors = connectivityMatrix[childID].size();
			bool found = false;
			for(k=0; k<childsSuccessors; ++k)
				if(connectivityMatrix[childID].list[k].first == i)
				{
					found = true;
					break;
				}
			if(found == true)
				connectivityMatrix[childID].list.erase(connectivityMatrix[childID].list.begin() + k);
		}
	}
}

int
GraphIR::parseMETIS(std::string fileName, int offset)
{
	DEBUG1(cout<<"\n[parseMETIS] - Entering | fileName="<<fileName;)
	const string& delimiters = " ";
	string fileExtension = fileName.substr( fileName.size() - 3 );
	if( fileExtension.compare("xml") == 0 )
	{
		parseXML( fileName );
		return 0;
	}
	ifstream input_file;
	input_file.open( fileName.c_str() );
	int64_t line_no = 0;
	while( input_file.good() )
	{
		string line;
		vector < int64_t > tokens;
		getline( input_file, line );
		string::size_type lastPos = line.find_first_not_of( delimiters, 0 );
		string::size_type pos = line.find_first_of( delimiters, lastPos );
		while( string::npos != pos || string::npos != lastPos )
		{
			string temp = line.substr( lastPos, pos - lastPos );
			tokens.push_back( atoll ( temp.c_str() ) );
			lastPos = line.find_first_not_of( delimiters, pos );
			pos = line.find_first_of( delimiters, lastPos );
		}
		if( tokens.size() == 0)
			continue;
		else if( line_no == 0 )
		{
			if( tokens.size() != 4 )
			{
				cout << "\ntokens.size()="<<tokens.size()<<" Error: Input graph file format incorrect" << endl;
				for(int i=0; i<tokens.size();++i)
				{
					cout<<"\ntokens["<<i<<"]="<<tokens[i];
				}
				return -1;
			}
			if( tokens[ 2 ] != 11 )
			{
				cout << "Error: Input graph without constraints specified" << endl;
				return -1;
			}
			numberOfVertices = tokens[ 0 ];
			ncon = tokens[ 3 ];
			line_no++;
			for(int i=0; i<numberOfVertices; ++i)
			{
				ConnectivityMatrix tempVector;
				connectivityMatrix.push_back( tempVector );
			}
			singleConstraintVwgt.resize( numberOfVertices, 0 );

			vwgt.resize( numberOfVertices * ncon, 0 );
		}
		else
		{
			if( ncon > 1 )
				if( tokens[ 1 ] == 0 && tokens[ 0 ] == 0 )
					singleConstraintVwgt[ line_no - offset ] = 1;
				else
				{
					if( tokens[ 1 ] == 0 )
						singleConstraintVwgt[ line_no - offset ] = tokens[ 0 ] * 1;
					else if( tokens[ 0 ] == 0 )
						singleConstraintVwgt[ line_no - offset ] = tokens[ 1 ] * 1;
					else
						singleConstraintVwgt[ line_no - offset ] = tokens[ 0 ] * tokens[ 1 ];
				}
			else if( ncon == 1 )
				singleConstraintVwgt[ line_no - offset ] = tokens[ 0 ];
			for(int i = 0; i<ncon; ++i)
			{
				if(tokens[i]==0)
					tokens[i]=1;
				vwgt[ (line_no - offset) * ncon + i ] = tokens[i];
			}
			// line_no - offset will give node ID
			for(int i = ncon; i<tokens.size(); i=i+2)
			{
				pair < int64_t, double > temp;
				temp.first = tokens[ i ] - offset;
				temp.second = tokens[ i+1 ];
				connectivityMatrix[ line_no - offset ].list.push_back(temp);
				DEBUG5(cout<<"a"<<connectivityMatrix[ line_no - offset ].list.size();)
			}
			DEBUG5(cout<<"ding"<<line_no<<" | connectivityMatrix["<< line_no - offset<<"].size="<<connectivityMatrix[ line_no - offset ].list.size()<<"\n";)
			line_no++;
		}
	}
	input_file.close();
	// double sum = 0;
	// int64_t numberOfEdges = 0, i, j;
	// DEBUG3(cout<<"\nnumberOfVertices="<<numberOfVertices<<endl);
	// for(i=0; i<numberOfVertices; ++i)
	// {
	// 	DEBUG3(cout<<connectivityMatrix[i].list.size()<<" | ";)
	// 	for(j=0; j<connectivityMatrix[i].list.size(); ++j)
	// 	{
	// 		DEBUG3(cout<<"("<<i<<","<<j<<")\t";)
	// 		sum += connectivityMatrix[i].list[j].second;
	// 		numberOfEdges++;
	// 	}
	// }
	// double meanEdgeWeight = sum/numberOfEdges;
	// DEBUG3(cout<<"\n[readApplicationGraph] - Sum of edge weights - "<<sum;)
	// DEBUG2(cout<<"\n[readApplicationGraph] - Exiting";)
	DEBUG1(cout<<"===> parseMETIS : Exiting";)
}

int
GraphIR :: createMETISFile( int64_t numberOfNodes, int64_t numberOfEdges, int ncon, string fileName, int64_t seed, int beta, bool heftStyleGraphs )
{
	fstream metis, udmetis;
	int64_t i, j, counter = 0;
	string temp("grf"), temp1("-udag"), temp2;
	string::iterator it;
	for(it=fileName.end()-3; it<fileName.end(); it++, counter++)
		*it = temp[counter];
	temp2 = fileName + temp1;
	metis.open(fileName.c_str(), fstream::out);
	//udmetis.open(temp2.c_str(), fstream::out);
	metis<<fixed<<numberOfNodes<<" "<<numberOfEdges<<" 011 "<<ncon;
	if( heftStyleGraphs == true )
		metis<<" "<<seed<<" "<<beta;
	for(i=0; i<numberOfNodes; ++i)
	{
		metis<<endl;
		for(j=0; j<ncon; ++j)
			metis<<vwgt[i*ncon + j]<<" ";
		for(j=0; j<connectivityMatrix[i].size(); ++j)
		{
			if( connectivityMatrix[i].list[j].second > 0 )
			{	
				metis<<connectivityMatrix[i].list[j].first+1<<" "<<(int64_t)connectivityMatrix[i].list[j].second<<" ";
			}
		}
	}
	
	numberOfEdges = 0;
	for(i=0; i<numberOfNodes; ++i)
		for(j=0; j<connectivityMatrix[i].size(); ++j)
			if( connectivityMatrix[i].list[j].second != 0 )
				++numberOfEdges;

	cout<<"\nNumber of edges = "<<numberOfEdges;
	/*
	udmetis<<numberOfNodes<<" "<<numberOfEdges<<" 011 "<<ncon;
	for(i=0; i<numberOfNodes; i++)
	{
		udmetis<<endl;
		for(j=0; j<ncon; ++j)
			udmetis<<vwgt[i*ncon + j]<<" ";
		for(j=0; j<numberOfNodes; ++j)
			if( connectivityMatrix[i][j] > 0 )
				udmetis<<j+1<<" "<<connectivityMatrix[i][j]<<" ";
	}
	*/
	metis.close();
	//udmetis.close();
}

int
GraphIR :: parseXML( std::string fileName )
{
	enum
	{
		_NODE = 1,
		_EDGE = 1,
		_NEND = 3,
		_NSTART = 5,
		_WEIGHT = 7
	};
	const string& delimiters = "\"";
	ifstream input_file;
	input_file.open( fileName.c_str() );
	int64_t line_no = 0, numberOfNodes = 0, numberOfEdges = 0;
	int i, blade = 0;
	while( input_file.good() )
	{
		string line;
		vector < string > tokens;
		getline( input_file, line );
		string::size_type lastPos = line.find_first_not_of( delimiters, 0 );
		string::size_type pos = line.find_first_of( delimiters, lastPos );
		while( string::npos != pos || string::npos != lastPos )
		{
			string temp = line.substr( lastPos, pos - lastPos );
			tokens.push_back( temp );
			lastPos = line.find_first_not_of( delimiters, pos );
			pos = line.find_first_of( delimiters, lastPos );
		}
		if(tokens.size() == 1)
		{
			if( tokens[0].find("<Nodes>") != string::npos )
			{
				blade = 1;
				ncon = 1;
			}
			else if( tokens[0].find("</Nodes>") != string::npos )
			{
				fflush(stdout);
				numberOfVertices = numberOfNodes;
				for(int i=0; i<numberOfVertices; ++i)
				{
					ConnectivityMatrix tempConnectivityMatrix;
					connectivityMatrix.push_back( tempConnectivityMatrix );
				}
				singleConstraintVwgt.resize( numberOfVertices, 0 );
				cout<<"\nTotal number of nodes found = "<<numberOfNodes;
				fflush( stdout );
			}
			else if( tokens[0].find("<Edges>") != string::npos )
			{
				blade = 3;
			}
			else if( tokens[0].find("</Edges>") != string::npos )
			{
				blade = 4;
				createMETISFile( numberOfNodes, numberOfEdges, ncon, fileName );
			}
			else if( tokens[0].find("</Graph>") != string::npos )
				break;
		}
		else
		{
			switch( blade )
			{
				case 1:
				{
					IDMap.push_back( atoll( tokens[_NODE].c_str() ) );
					for(int i = 0; i<ncon; ++i)
						vwgt.push_back( 0 ); // Assume equal weights for all intersections
					++numberOfNodes;
					break;
				}
				case 3:
				{
					// Find the indices of the two IDs in our IDMap
					int64_t temp = atoll( tokens[ _NSTART ].c_str() );
					int64_t nStart, nEnd;
					for(i=0; i<IDMap.size(); ++i)
						if( IDMap[i] == temp )
						{	nStart = i; break;	}
					temp = atoll( tokens[ _NEND ].c_str() );
					for(i=0; i<IDMap.size(); ++i)
						if( IDMap[i] == temp )
						{	nEnd = i; break;	}

					// Create a pair of the endpoint and communication weight and insert
					pair < int64_t, double > tempPair;
					tempPair.first = nEnd;
					tempPair.second = atoll( tokens[ _WEIGHT ].c_str() );
					bool found=false;
					for(i=0; i<connectivityMatrix[ nStart ].list.size(); ++i)
					{
						if(connectivityMatrix[ nStart ].list[ i ].first == nEnd)
						{
							found = true;
							connectivityMatrix[ nStart ].list[ i ].second = (int64_t)( connectivityMatrix[ nStart ].list[ i ].second + tempPair.second )/2;
							break;
						}
					}
					if(found == false)
						connectivityMatrix[ nStart ].list.push_back(tempPair);
					
					// Adjust vertex weights of the starting and ending node of this edge
					vwgt[ nStart*ncon ] += connectivityMatrix[ nStart ][ nEnd ];
					vwgt[ nEnd*ncon ] += connectivityMatrix[ nStart ][ nEnd ];
					++numberOfEdges;
					break;
				}
				default:
					break;
			}
		}
		line_no++;
	}
	singleConstraintVwgt.resize(numberOfVertices, 0);
	for(i=0; i<numberOfVertices; ++i)
		for(int j=0; j<ncon; ++j)
			singleConstraintVwgt[i] += vwgt[i*ncon + j];
		
	input_file.close();
}

GraphIR::GraphIR()
{
	numberOfVertices = ncon = 0;
}

GraphIR::~GraphIR()
{
	// free memory for the all the variables. call vector.clear();
}

GraphIR GraphIR::operator = (GraphIR param)
{
	GraphIR temp;
	temp.numberOfVertices = param.numberOfVertices;
	temp.ncon = param.ncon;
	temp.vwgt = param.vwgt;
	return (temp);
}

#endif
