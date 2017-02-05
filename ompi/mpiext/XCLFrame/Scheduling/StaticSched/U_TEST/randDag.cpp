
#include "common.h"




//RANDOM DAG GENERATION
///////////////////////

randApplication::randApplication(){
	newRandDAG();
}

void buildExtendedRandAdj(int* EAdj, int* Adj , int N){
	int i,j;
	int xtndSz=N+2;
	int edgeWeight;
	for(i=0;i<N;i++){
		edgeWeight = MIN_EDGE_W
				+ (rand () % (MAX_EDGE_W - MIN_EDGE_W + 1));
		EAdj[i+1]=isSource(i,Adj,N)*edgeWeight; //call to BASE/utilities
	}

	for(i=1;i<N+1;i++){
		edgeWeight = MIN_EDGE_W
				+ (rand () % (MAX_EDGE_W - MIN_EDGE_W + 1));
		EAdj[xtndSz*i+(N+1)]=isSink(i-1,Adj,N)*edgeWeight;//call to BASE/utilities
	}

	for(i=0,j=1;i<N*N;i+=N,j++){ //copy one row at time
		memcpy(EAdj+(xtndSz*j+1),Adj+i,N*sizeof(int)); // advance EAdj j-rows of xtnd size.
	}

}

void randApplication::newRandDAG(){

	void *libMhandle;
	libMhandle = dlopen ("/usr/lib/x86_64-linux-gnu/libm.so", RTLD_LAZY);
	int i;
	int l, j, k,nodes = 0;

	srand (time (NULL));

	std::vector<std::pair<int,int>> *adjLists;

	int levels = MIN_LEVELS
			+ (rand () % (MAX_LEVELS - MIN_LEVELS + 1));

	for (l = 0; l < levels; l++){

		int new_nodes = MIN_PER_LEVEL
				+ (rand () % (MAX_PER_LEVEL - MIN_PER_LEVEL + 1));
		//printf ("level Nodes  %d  \n", new_nodes);

		//Node's linked list initialization with the src task
		if(!nodes){
			adjLists=(std::vector <std::pair<int,int>>*)malloc(new_nodes*sizeof(std::vector <std::pair<int,int> >));
			for(j=0;j<new_nodes;j++){
				new(adjLists+j)std::vector <std::pair<int,int>>();
			}

		}else{
			std::vector<std::pair<int,int>> *tmpAdjLists;
			tmpAdjLists=(std::vector <std::pair<int,int>>*)realloc(adjLists,(nodes+new_nodes)*sizeof(std::vector <std::pair<int,int> >));
			if(tmpAdjLists!=NULL){
				adjLists=tmpAdjLists;
				for(j=nodes;j<(nodes+new_nodes);j++){
					new(adjLists+j)std::vector <std::pair<int,int>>();
				}
			}
			else{
				printf("vector reallocation Error \n");
				exit(-1);
			}
		}

		/* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
		for (k = 0; k < new_nodes; k++){
			int parents= gammaNumber(3)+1;//NumParents
			//printf("NumParents: %d \n",parents);
			int j = nodes-1;
			while(parents && j>=0){
				//printf("j: %d \n",j);
				for (; j >= 0; j--){
					if ( (rand () % 100) < PERCENT){
						//printf (" %d -> %d; \n", j, k + nodes);
						int edgeWeight = MIN_EDGE_W
								+ (rand () % (MAX_EDGE_W - MIN_EDGE_W + 1));
						//Insert An Edge.
						(adjLists+j)->push_back(std::make_pair(k + nodes,edgeWeight));
						//printf ("  %d -> %d; EW: %d \n", j, k + nodes,edgeWeight);
						parents--;
						break;
					}
				}

			}
		}
		nodes += new_nodes; /* Accumulate into old node set.  */
	}

	printf("\n");

	std::vector<std::pair<int,int>>::iterator It;

	int* adjMtx= (int*)calloc(nodes*nodes,sizeof(int));
	for(l=0;l<nodes;l++){
		//printf("%d ",i);
		for(It=adjLists[l].begin();It!=adjLists[l].end();It++){
			adjMtx[nodes*l+((*It).first)]=(*It).second;
			//printf("--> (%d,%d)",(*It).first,(*It).second);
		}
		//printf("\n");
	}

	//Extend the matrix to put the src and sink tasks.
	EadjMtx= (int*)calloc((nodes+2)*(nodes+2),sizeof(int));
	buildExtendedRandAdj(EadjMtx, adjMtx , nodes);

	numTasks=nodes+2;

	//task memory requirement
	r=(int*)malloc(numTasks*sizeof(int));
	for(i=0;i<numTasks;i++){
		r[i]=gaussianNumber(REQ,0.5*REQ);
		//printf("%d \n", r[i]);
	}


/*	FILE* digraphFile;
	digraphFile=fopen("./digraph.dot","w");
	fprintf(digraphFile,"digraph {\n");
	for(i=0;i<numTasks;i++){
		for(j=0;j<numTasks;j++){
			if(EadjMtx[numTasks*i+j]){
				fprintf (digraphFile," %d -> %d;\n",i,j);
			}
		}
	}
	fprintf (digraphFile,"}\n");
	fseek(digraphFile, SEEK_SET, 0);
	fclose(digraphFile);*/



}



////////////////////////////////
//END OF RANDOM GRAPH GENERATION
