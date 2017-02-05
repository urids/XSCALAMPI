/*
 * HEFT.cpp
 *
 *  Created on: Aug 20, 2016
 *      Author: uriel
 */

#include "HEFT.h"
#define DEBUG 1

using namespace std;
HEFT::HEFT(){
}
HEFT::~HEFT(){

}

HEFT::HEFT(char* Path):storagePath(Path){
}

//Average computations

float get_WAvg(vector<float>  WT_i){
	float WAvg_i=0;
	vector<float>::iterator it;
	for(it=WT_i.begin();it!=WT_i.end();it++){
		WAvg_i+=*it;
	}
	return WAvg_i/WT_i.size();
}

void get_CAvg(float LAvg, float BWAvg, vector<float> Adj_Mtx,vector<float> CAvg){
	vector<float>::iterator itC,itAdj;
	for(itC=CAvg.begin(),itAdj=Adj_Mtx.begin(); itAdj!=Adj_Mtx.end();itC++,itAdj++){
		*itC=LAvg+*itAdj/BWAvg;
	}
}

static int getOD_v(vector<float> Adj){
	//vector<float>::iterator it1;
	return std::count_if(Adj.begin(), Adj.end(), [](float i)-> int {
		int od=0;
		if(i) od++;
		return od;
	});
}

bool pairCompare(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem) {
	//We break (rank_u) tie by task ID.
	if(firstElem.second == secondElem.second)
		return firstElem.first < secondElem.first; //smaller taskID first
	else
		return firstElem.second > secondElem.second; //else Descending sorted
}

static bool pairCompareAsc(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem) {
	//We break (rank_u) tie by task ID.
	if(firstElem.second == secondElem.second)
		return firstElem.first < secondElem.first;
	else
		return firstElem.second < secondElem.second;
}


void readLMtx(char* storagePath, int numDevs, double* L_Mtx){
	FILE* L_FH;
	char LMtx_FileName[1024];
	memcpy(LMtx_FileName,storagePath,sizeof(LMtx_FileName));
	strcat(LMtx_FileName,"/L_File.dat");
	L_FH=fopen(LMtx_FileName,"r");
	if(L_FH==NULL){
		perror("unable to find latency file");
	}
	int readsz;
	//float* L1_Mtx=(float*)malloc(sizeof(float)*numDevs*numDevs);
	readsz=fread(L_Mtx,sizeof(double),numDevs*numDevs,L_FH);
	if(!readsz){
		printf("\n %d: ",readsz);
		perror("unable to read data from latency file");
		exit(-1);
	}
}

void readBWMtx(char* storagePath, int numDevs, double* BW_Mtx){
	FILE* BW_FH;
	char BWMtx_FileName[1024];
	memcpy(BWMtx_FileName,storagePath,sizeof(BWMtx_FileName));
	strcat(BWMtx_FileName,"/BW_File.dat");
	BW_FH=fopen(BWMtx_FileName,"r");
	if(BW_FH==NULL){
		perror("unable to find latency file");
	}
	int readsz;
	//float* L1_Mtx=(float*)malloc(sizeof(float)*numDevs*numDevs);
	readsz=fread(BW_Mtx,sizeof(double),numDevs*numDevs,BW_FH);
	if(!readsz){
		perror("unable to read data from BandWidth file");
		exit(-1);
	}
}

#if DEBUG
int HEFT::matchMake(int volatile NumTsk, int NumDvs,float * W, int * AdjMtx,const int* SR,const int* AS, int* schedule){
#else
int HEFT::matchMake(int NumTsk, int NumDvs,float * volatile W, int * AdjMtx,const int* SR,const int* AS, int* schedule){
#endif
	map<int,int> taskMap; // <task, device>
	/*************************
	 * STEP 01
	 *************************/
	// compute average computation costs:
	std::vector<float> AvgWT;
	for(int i=0;i<NumTsk;++i){
		std::vector<float> WT_i (W+NumDvs*i, W+(NumDvs*i+NumDvs));
		AvgWT.push_back(get_WAvg(WT_i));
	}

	// compute average communication cost.
	double * L_Mtx=new double[NumDvs*NumDvs];
	double * BW_Mtx=new double[NumDvs*NumDvs];

	readLMtx(this->storagePath,NumDvs,L_Mtx);
	readBWMtx(this->storagePath,NumDvs,BW_Mtx);

	/*for(int i=0;i<NumTsk;i++){
		printf("| ");
		for(int j=0;j<NumDvs;j++){
			printf(" %8.8f |",W[NumDvs*i+j]); //printed in microseconds
		}
		printf("\n-------------------------------\n");
	}

	for(int i=0;i<NumTsk;i++){
		printf("| ");
		for(int j=0;j<NumTsk;j++){
			printf(" %d |",AdjMtx[NumTsk*i+j]); //printed in microseconds
		}
		printf("\n-------------------------------\n");
	}


	for(i=0;i<NumDvs;i++){
			printf("| ");
			for(j=0;j<NumDvs;j++){
				printf("  %8.6f |",BW_Mtx[NumDvs*i+j]/(float)(ONEGB));
				//fwrite(&BW_Mtx[g_PUs*i+j],sizeof(float),1,FP_BW_Mtx);
			}
			printf("\n-------------------------------\n");
		}*/

	float Avg_L=get_LAvg(L_Mtx,NumDvs);
	float Avg_BW=get_BWAvg(BW_Mtx,NumDvs);

	vector<float> AdjM(AdjMtx,AdjMtx+(NumTsk*NumTsk)); //adjacency mtx converted to vector ??
	vector<float> AvgC(NumTsk*NumTsk); //estimated cost of every edge using average L && BW.
	get_CAvg(Avg_L, Avg_BW, AdjM, AvgC); // fill edge estimated cost (seconds)


	/*************************
	 * STEP 02
	 *************************/
	//compute the O_D(v).

	vector<int> OD_v;
	for(int i=0;i<NumTsk;++i){
		std::vector<float> Adj_i (AdjM.begin()+NumTsk*i, AdjM.begin()+(NumTsk*i+NumTsk));
		OD_v.push_back(getOD_v(Adj_i));
	}

	/*************************
	 * STEP 03
	 *************************/
	//compute the rank_u starting from the exit task.


	//3.1- Find the sink task.
	static int s = 0;
	find_if(OD_v.begin(), OD_v.end(),[](int i) {
		if(i==0) return true;
		else {
			++s;
			return false;
		}
	});



	//3.2- Compute the transposed graph
	int *T_AdjMtx=(int*)malloc(NumTsk*NumTsk*sizeof(int));
	for(int i=0;i<NumTsk;++i){
		for(int j=0;j<NumTsk;++j){
			T_AdjMtx[NumTsk*j+i]=AdjMtx[NumTsk*i+j];
		}
	}

	list<pair<int,int> > *AdjList_T; // < predecessor, edgeCost >
	AdjList_T=new list<pair<int,int> >[NumTsk];

	for(int i=0;i<NumTsk;++i){
		for(int j=0;j<NumTsk;++j){
			if(T_AdjMtx[NumTsk*i+j]){
				AdjList_T[i].push_back(make_pair(j,T_AdjMtx[NumTsk*i+j]));
			}
		}
	}

	/*for(int j=22;j>=0;j--){
	for_each(AdjList[j].begin(),AdjList[j].end(),[](pair<int,int> i){cout<<"--"<<i.first;});
cout<<endl;
}*/

	//3.3 compute the upperRank //we can use BFS without marking visited because there are no cycles!!! =).
	 vector<pair<int,float> > UR(NumTsk); // UR<task,urVAL>
	for(int i=0;i<NumTsk;i++)
		UR[i]=make_pair(i,0);

	list<int> queue;

	queue.push_back(s);
	UR[s].second=AvgWT[s];

	list<pair<int,int> >::iterator i;
	while(!queue.empty()){
		s=queue.front();
		queue.pop_front();

		/*for(i=AdjList[s].begin();i!=AdjList[s].end();++i){
			queue.push_back(*i);
		}*/

			for_each(AdjList_T[s].begin(), AdjList_T[s].end(), [&queue,&UR,&AvgWT](pair<int,int> i) {
			//cout<<s<<"-"<<i.first<<endl;
			queue.push_back(i.first);
			float val=AvgWT[i.first]+ i.second+UR[s].second;
			UR[i.first].second=(UR[i.first].second<val)?val:UR[i.first].second;
			//cout << UR[i.first].second<<endl; // upperRank
		});
	}

	/*************************
	 * STEP 04
	 *************************/
	//sort the tasks by UR.

	sort(UR.begin(),UR.end(),pairCompare);
	//for_each(UR.begin(),UR.end(),[](pair<int,float> i){cout<<i.first<<"  "<<i.second<<endl;}); //sorted UpperRank print


	/*************************
	 * STEP 05,06
	 *************************/
	//schedule the tasks.

	vector<float> avail(NumDvs,0);
	vector<float> AFT(NumTsk,0);
	vector<pair<int, float> > EFT; //<dev, est+Wij>
	vector<float> EST(NumTsk,0);
	//float *EST=(float*)calloc(NumTsk*NumDvs,sizeof(float));

	//FOR EACH TASK.
	for_each(UR.begin(),UR.end(),[BW_Mtx, L_Mtx,&SR,&NumDvs,&NumTsk,&W, &AdjList_T, &AFT ,&EFT, &EST, &avail, &taskMap,&OD_v](pair<int,float> ur){
		//STEP 07 create Q prime.
		vector<int> Qp;
		for(int dev_i=0;dev_i<NumDvs;dev_i++){
			//**int ASDev_i=SBs->benchInfo.AS[dev_i];
			//**if(ASDev_i > SR[ur.first]){
			Qp.push_back(dev_i);
			//**}
		}

		//STEP 08 if there exists devices with space.
		if(Qp.size()>0){
			EFT.clear();
			//STEP 09 find the fastest device
			cout <<endl<<"TASK: "<<ur.first <<"  "<<endl ;

			//FOR EACH DEVICE.
			for_each(Qp.begin(),Qp.end(),[BW_Mtx, L_Mtx,&ur,&NumDvs,&NumTsk,&W, &AdjList_T, &AFT,&EST, &EFT,&avail,&taskMap,&OD_v]
			                              (int dev){
				//STEP 10 compute the EFT
				//STEP 10.1 get the MAX time of all dependencies.
				float max_AST=0.0;

				//FOR EACH PREDECESSOR.
				for_each(AdjList_T[ur.first].begin(),AdjList_T[ur.first].end(),[BW_Mtx, L_Mtx,&NumDvs,&dev,&AFT,&max_AST,&taskMap]
				                                                                (pair<int,float> Pred_t){ //task , msg Size
					//int L=0;
					int predDev=taskMap.find(Pred_t.first)->second;

					//**float BW=SBs->benchInfo.BW_Mtx[NumDvs*dev+predDev];
					//**float L=SBs->benchInfo.L_Mtx[NumDvs*dev+predDev];
					float BW=BW_Mtx[NumDvs*dev+predDev];
					float L=L_Mtx[NumDvs*dev+predDev];

					//cout<<"Pred:"<<Pred_t.first <<"--"<<AFT[Pred_t.first]<<endl;
					//cout<<endl<<Pred_t.first<<"--> "<<AFT[Pred_t.first]<<"+"<<Pred_t.second<<endl;
					max_AST=(max_AST > (AFT[Pred_t.first]+(Pred_t.second/BW+L) )? max_AST : (AFT[Pred_t.first]+(Pred_t.second/BW+L)));
				});
				//cout<<"max start time on device"<< dev <<": "<<max_AST<<endl;
				//STEP 10.2 compute the EST

				float est=(avail[dev]>max_AST)?avail[dev]:max_AST;
				EST[ur.first]=est; //updates the EST[T_i] (used to print_schedule)

				cout<<"EST: "<<est<<" - DEV: "<<dev<<"   avail[dev]: "<<avail[dev]<<", max_AST: "<<max_AST<<endl;

				EFT.push_back(make_pair(dev ,W[NumDvs*ur.first+dev]+est));
			});
			//STEP 12.1 find the device that minimizes the EFT.
			int bestDev;

			//--bestDev=distance(EFT.begin(),min_element(EFT.begin(),EFT.end()));
			sort(EFT.begin(),EFT.end(),pairCompareAsc);


			//for_each(EFT.begin(),EFT.end(),[](pair<int,float> eft){cout<<eft.first<<"--"<<eft.second <<"  ";});

			bestDev=EFT[0].first;

			//cout<<" \n BD "<<bestDev;

			//cout << "TASK: "<<ur.first <<" BestDevice: "<< bestDev <<endl;

			//STEP 12.2 Assign the task to it.
			taskMap[ur.first]=bestDev;
			AFT[ur.first]=EFT[0].second; //devices are sorted by EFT hence [0] refers to the best time.
			avail[bestDev]=EFT[0].second;

			//STEP 13: Reduce the amount of memory on the device.
			//**SBs->benchInfo.AS[bestDev]-=SR[ur.first];

			//STEP 14: find the predecessors of this task:
			for_each(AdjList_T[ur.first].begin(),AdjList_T[ur.first].end(),[&SR,&OD_v,&taskMap](pair<int,float> tm){
				OD_v[tm.first]--;
				if(!OD_v[tm.first]){
					//STEP 17: recover the memory space.
					int relDev=taskMap.find(tm.first)->second;
					//**SBs->benchInfo.AS[relDev]+=SR[tm.first];
				}
			}); //end for each dev in Qp
		}
		else{
			printf("Tasks could not be scheduled due to the lack of memory space \n");
			//cout << "Tasks could not be scheduled due to the lack of memory space";
			exit (EXIT_FAILURE);
		}

	});

	//return Results in c array format.
	for(auto tID:taskMap){
		schedule[tID.first]=tID.second;
	}


	return 0;
}

extern "C"{
int dummy(int a){

	return a;
}
}


