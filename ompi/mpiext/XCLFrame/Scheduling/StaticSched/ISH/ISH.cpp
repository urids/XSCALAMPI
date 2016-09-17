/*
 * ISH.cpp
 *
 *  Created on: Jul 23, 2016
 *      Author: uriel
 */

#include "ISH.h"
using namespace std;

ISH::ISH() {
	// TODO Auto-generated constructor stub

}

ISH::~ISH() {
	// TODO Auto-generated destructor stub
}



int getOD_v(vector<float> Adj){
	return std::count_if(Adj.begin(), Adj.end(), [](float i)-> int {
		int od=0;
		if(i) od++;
		return od;
	});
}

bool pairCompareDesc(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem) {
	//We break (rank_u) tie by task ID.
	if(firstElem.second == secondElem.second)
		return firstElem.first < secondElem.first; //smaller taskID first
	else
		return firstElem.second > secondElem.second; //else Descending sorted
}

bool pairCompareAsc(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem) {
	//We break (rank_u) tie by task ID.
	if(firstElem.second == secondElem.second)
		return firstElem.first < secondElem.first;
	else
		return firstElem.second < secondElem.second;
}



int computebLevel(stack<int> &revTopList,list<pair<int,int> > *AdjList,
		vector<pair<int,float> >& bLevel, float * W, int NumDvs, int NumTsk){
	for(int i=0;i<NumTsk;i++)
		bLevel[i]=make_pair(i,0);
	int max;
	while(revTopList.empty() == false){
		int ni= revTopList.top();
		revTopList.pop();
		max=0;
		for(list<pair<int,int> >::iterator ny=AdjList[ni].begin(); ny!=AdjList[ni].end();ny++){
			int tmpMax=(*ny).second+bLevel[(*ny).first].second;
			max=tmpMax>max?tmpMax:max;
			/*if(tmpMax>max){
				max=tmpMax;
			}*/

		}
		bLevel[ni].second=W[NumDvs*ni]+max;
	}
	return 0;
}

int reverseStack(stack<int> inStack, stack<int> &outStack){
	if(inStack.empty()== false){
		outStack.push(inStack.top());
		inStack.pop();
		reverseStack(inStack, outStack);
	}
	return 0;
}

struct {
	void operator()(vector<float> &AFT,float &max_AST, pair<int,float> Predecesor){ //<task , msg Size>
		float predDelay=AFT[Predecesor.first]+(Predecesor.second);
		max_AST=(max_AST > predDelay)? max_AST : predDelay;
	}
}getMaxPredecesorDelay;

struct {
	void operator()(vector<pair<int, float>> &readyList, vector<pair<int, float>> &bLevel,
			map<int,int> &taskMap, list<pair<int,int> > AdjList[],
			list<pair<int,int> >*&AdjList_T, int taskId){
		for(auto subSeq: AdjList[taskId]){
			map<int,int>::iterator it;
			it=taskMap.find(subSeq.first);
			if(it==taskMap.end()){ //1.- if this subseq has not been scheduled
				int allParents=1;
				map<int,int>::iterator iit;
				for(auto parent: AdjList_T[subSeq.first]){
					iit=taskMap.find(parent.first);
					allParents &= (iit!=taskMap.end());//This parent is scheduled
				}
				if(allParents){ //2.- if ALL parents are squeduled
					//find this subsequent in the blevel List.
					vector<pair<int,float>>::iterator bLevelIt=find_if(bLevel.begin(),bLevel.end(),[&subSeq](pair<int,float> list){
						return subSeq.first==list.first;
					});
					readyList.push_back(*bLevelIt); //enqueue in the readyList
				}
			}
		}
		sort(readyList.begin(),readyList.end(),pairCompareDesc);
	}
}updateReadyList;


int ISH::matchMake(int NumTsk, int NumDvs,float * W, int * AdjMtx,const int* SR,const int* AS, int* schedule){
	int i,j,k;
	map<int,int> taskMap;
	stack<int> topoSortList;
	list<pair<int,int> > AdjList[NumTsk]; //< Adj, Weight >
	buildAdjList(AdjMtx,AdjList,NumTsk);

	int *AdjMtx_T=(int*)malloc(NumTsk*NumTsk*sizeof(int));
	transpose(AdjMtx,AdjMtx_T,NumTsk);

	list<pair<int,int> > *AdjList_T=new list<pair<int,int> >[NumTsk]; // < predecessor, edgeCost >
	buildAdjList(AdjMtx_T,AdjList_T,NumTsk);

	topoSort(NumTsk,AdjList,topoSortList);
	stack<int> revTopList;
	reverseStack(topoSortList,revTopList);

	//1.- Compute the static bLevel
	vector<pair<int,float> > bLevel(NumTsk); // bLevel<task,bLevelVAL>
	computebLevel(revTopList,AdjList,bLevel,W,NumDvs,NumTsk);

	//2.- Make a readyList
	//2.1 Sort bLevel in descending order.
	sort(bLevel.begin(),bLevel.end(),pairCompareDesc);


/*	cout<<"\n-------------\n" <<endl;
	cout<<"BLEVELs:" <<endl;
	for_each(bLevel.begin() ,bLevel.end() ,[](pair<int, float> bl){
		cout<<bl.first<<" "<<bl.second<<"\n";
	});
	cout<<endl;

	for(int i=0;i<NumTsk;i++){
		for(int j=0;j<NumDvs;j++){
			cout<<" "<<W[NumDvs*i+j];
		}
		cout<<"\n";
	}*/


	//2.2 Initially the readyList contains only entry points
	vector<pair<int, float>> readyList; // <taskId, bLevelVAL>
	readyList.push_back(bLevel.front());


	vector<float> avail(NumDvs,0);
	vector<float> AFT(NumTsk,0); //<task(assigned) Finish time>
	vector<pair<int, float> > EST;// <dev, max(avail,max_AST)>
	vector<int> newScheduled; // <taskId>
	vector<pair<float, float> > WinSlots;
	vector<int> OD_v;
	int Tn;

	//3.- Repeat
	do{

/*
		cout<<"\n-------------\n" <<endl;
		cout<<"READYLIST:" <<endl;
		for_each(readyList.begin() ,readyList.end() ,[](pair<int, float> rl){
			cout<<" "<<rl.first;
		});
		cout<<endl;
*/

		newScheduled.clear();
		//STEP a) create Q prime.
		vector<int> Qp;
		for(int dev_i=0;dev_i<NumDvs;dev_i++){
			//int ASDev_i=SBs->benchInfo.AS[dev_i];
			//if(ASDev_i > SR[ur.first]){
			Qp.push_back(dev_i);
			//}
		}
		assert(Qp.size()>0);

		Tn=(readyList.front()).first;
		//--cout<<"Tn: "<<Tn<<endl;
		EST.clear();

		//EST of Tn on each Device.
		for_each(Qp.begin(),Qp.end(),[&Tn, &AdjList_T,&AFT,&EST,&avail]
		                              (int deviceID){
			float max_AST_Tn=0.0;
			for (auto predecesor:AdjList_T[Tn]){
				getMaxPredecesorDelay(AFT,max_AST_Tn,predecesor);
			}
			float est=(avail[deviceID]>max_AST_Tn)?avail[deviceID]:max_AST_Tn;
			EST.push_back(make_pair(deviceID ,est));

		});
		sort(EST.begin(),EST.end(),pairCompareAsc);


		int bestDevice=EST[0].first;
		//--cout<<"bestDevice: "<< bestDevice<<endl;

		taskMap[Tn]=bestDevice;
		newScheduled.push_back(Tn);

		vector<pair<int, float>>::iterator taskFinder_it;
		taskFinder_it=find_if(readyList.begin(),readyList.end(),[&Tn](pair<int, float> task){
			return task.first==Tn;
		});
		readyList.erase(taskFinder_it);
		AFT[Tn]=EST[0].second+W[NumDvs*Tn]; //devices are sorted by EST hence [0] refers to the best time.

		//4.- If Scheduling this node causes a slot
		if(avail[bestDevice]<EST[0].second){
			//--cout<<avail[bestDevice]<<","<<EST[0].second<<" win length:"<<EST[0].second- avail[bestDevice]<<endl;
			WinSlots.clear();
			WinSlots.push_back(make_pair(avail[bestDevice],EST[0].second));

			for_each(readyList.begin() ,readyList.end() , [&readyList,&newScheduled,&W,
			                                               &taskMap,&NumDvs,&Tn,&Qp,&OD_v,
			                                               &WinSlots,&bestDevice,&avail,
			                                               &AdjList_T, &AFT]
			                                               (pair<int, float> Task){ //<task,bLevel>
				int Tm=Task.first;

				//EST of Tm on the best Device.
				float max_AST_Tm=0.0;
				for (auto predecesor:AdjList_T[Tm]){
						getMaxPredecesorDelay(AFT,max_AST_Tm,predecesor);
				}
				float est_Tm=(avail[bestDevice]>max_AST_Tm)?avail[bestDevice]:max_AST_Tm;

				find_if(WinSlots.begin() ,WinSlots.end() ,[&WinSlots,&readyList,
				                                           &newScheduled,&W,&taskMap,
				                                           &NumDvs,&est_Tm,&bestDevice,
				                                           &Tm,&Tn,&Qp,&AdjList_T,
				                                           &AFT,&avail,&max_AST_Tm]
				                                           (pair <float, float> Ws){

					//EST of Tm on the Any other Device.
/***					float max_STTm=0;
					for_each(Qp.begin(),Qp.end(),[&Tm,&max_STTm,&AdjList_T, &max_AST_Tm,&avail]
					                              (int dev){

						float est=(avail[dev]>max_AST_Tm)?avail[dev]:max_AST_Tm;
						max_STTm=(max_STTm>est)?max_STTm:est;

					});


					if(est_Tm>=Ws.first &&					  //|_
							est_Tm+W[NumDvs*Tm]<=Ws.second && //|  if fits.
							est_Tm <= max_STTm){ // and if starts earlier than in any other device
*/

					float min_STTmIOD=est_Tm; //start with the current device
					for_each(Qp.begin(),Qp.end(),[&Tm,&min_STTmIOD,&AdjList_T, &max_AST_Tm,&avail]
					                              (int dev){

						float estITD=(avail[dev]>max_AST_Tm)?avail[dev]:max_AST_Tm;
						min_STTmIOD=(estITD<min_STTmIOD)?estITD:min_STTmIOD;

					});


					if(est_Tm>=Ws.first &&					  //|_
					   est_Tm+W[NumDvs*Tm]<=Ws.second && //|  if fits.
					   est_Tm <= min_STTmIOD){


						cout<<"Task" << Tm << "fits in slot after "<<Tn <<endl;
						taskMap[Tm]=bestDevice;
						newScheduled.push_back(Tm);
						//Delete Tm from the readyList
						vector<pair<int, float>>::iterator taskFinder_it;
						taskFinder_it=find_if(readyList.begin(),readyList.end(),[&Tm](pair<int, float> task){
							return task.first==Tm;
						});
						readyList.erase(taskFinder_it);
						AFT[Tm]=est_Tm+W[NumDvs*Tm];
						// add two new time slots.
						WinSlots.push_back(make_pair(Ws.first,est_Tm));
						WinSlots.push_back(make_pair(est_Tm+W[NumDvs*Tm],Ws.second));

						vector<pair<float, float>>::iterator winFinder_it;
						winFinder_it=find_if(WinSlots.begin(),WinSlots.end(),[&Ws](pair<float, float> iter){
							return iter.first==Ws.first;
						});
						WinSlots.erase(winFinder_it);
						return true;
					}
					return false;
				});//end if eachSlot
			});//end if T_m in readyList
		}  //end if Slot
		avail[bestDevice]=AFT[Tn];

		//5.- Update the ready list inserting all nodes now ready.
		for(auto TaskId : newScheduled){
			updateReadyList(readyList,bLevel,taskMap, AdjList,AdjList_T,TaskId);
		}

	}while(!readyList.empty());

	//return Results.
	for(auto tID:taskMap){
		schedule[tID.first]=tID.second;
	}

	return 0;
}
