#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include "List1.h"

using namespace std;

//float clk;
//float t_quantum;
int processes[100][2];

#define NUM_THREADS 4
void *incClock(void *data);
void *jobEntry(void *data);
void *scheduling(void *tq);
void *finishedQueue(void *tq);

struct ClockTracker {
    float clck;
    pthread_mutex_t lock;
};
ClockTracker myClock;

struct ProcessTracker{
    List readyState;
    List completed;
    List currentlyRunning;
    pthread_mutex_t lock;
};
ProcessTracker myProcess;

bool running =true;
bool updated = false;

int main()
{
   // bool running = true;
    pthread_mutex_init(&myClock.lock, NULL);
    pthread_mutex_init(&myProcess.lock, NULL);
    myClock.clck = 0;

    int t_quantum;
    cout<<"Enter time quantum: " << endl;
    cin>>t_quantum;

    PCB newList;
    List dataFromFile;
    initializeList(&dataFromFile);

    int processID;
    int proc_arrival_time;
    int proc_service_time;

    ifstream myfile;
    myfile.open("/Users/princessamf/Documents/IonaCollegeClasses/CS451/projects/Assignment6_PCB/Simulation-data-1.txt");
    //myfile.open("Simulation-data-1.txt");

    if(myfile.is_open())
    {

        while(myfile >> processID >> proc_arrival_time >> proc_service_time)
        {
            //myfile >> processID >> proc_arrival_time >> proc_service_time;
           // cout << processID << " " << proc_arrival_time << "  " << proc_service_time << endl;
            //check if file is being read correctly
            newList.pid = processID;
            newList.procArrivalTime = proc_arrival_time;
            newList.procServiceTime = proc_service_time;
            newList.timeOnReady = 0;
            newList.timeDone = 0;
            newList.timeInRunning =0;
            newList.remainingTime = proc_service_time;
            newList.finishedTime = 0;
            pushBack(dataFromFile, newList);

        }
    }

    myfile.close();
    //cout << "Back1: " << dataFromFile->prev->info.pid << " Back2: " << dataFromFile->prev->prev->info.pid << " Back3: " << dataFromFile->prev->prev->prev->info.pid << endl;
    //last process is duplicating, check for fixing

    pthread_t threads[NUM_THREADS];
    int rc;
    void *status;

    initializeList(&myProcess.readyState);
    initializeList(&myProcess.completed);
    initializeList(&myProcess.currentlyRunning);


    rc = pthread_create(&threads[0], NULL, incClock,(void*)&dataFromFile);
    if(rc)
		{
			cout << "ERROR. Return code from thread " << rc << endl;
			exit(-1);
		}
    while(running)
    {

        rc = pthread_create(&threads[1], NULL, jobEntry, (void *)&dataFromFile);
        if(rc)
		{
			cout << "ERROR. Return code from thread " << rc << endl;
			exit(-1);
		}
            //}
        rc = pthread_create(&threads[2], NULL, scheduling, (void *)&t_quantum);
        if(rc)
		{
			cout << "ERROR. Return code from thread " << rc << endl;
			exit(-1);
		}
        rc = pthread_create(&threads[3], NULL, finishedQueue, (void *)&t_quantum);
        if(rc)
		{
			cout << "ERROR. Return code from thread " << rc << endl;
			exit(-1);
		}

        if(isEmpty(myProcess.readyState) && isEmpty(myProcess.currentlyRunning) && isEmpty(dataFromFile)&&!isEmpty(myProcess.completed))
            running = false;
    }


    for(int t = 0; t < NUM_THREADS; ++t)
	{
	   rc = pthread_join(threads[t], &status);
	   if(rc)
	   {
		   cout << "Error, return code from thread_join() is " << rc << endl;
		   exit(-1);
	   }
	}

	//print out jobs and write to file

	ofstream newfile;
	newfile.open("/Users/princessamf/Documents/IonaCollegeClasses/CS451/projects/Assignment6_PCB/processInfo.txt");
	//newfile.open("/processesInfo.txt");
	//location of file

    newfile << "With Time Quantum: " << t_quantum << endl;
    newfile << endl;

    cout << endl;
    cout << "With Time Quantum: " << t_quantum << endl;
    cout << endl;

	NodeType* current = myProcess.completed->next;;
    int totalTime=0;
    int totalProcesses = 0;
	while(current->next && current!=myProcess.completed)
    {
        cout << "Process: "<< current->info.pid << "  Finished at: " << current->info.finishedTime << "  Total time on Ready State: " << current->info.timeOnReady<< endl;
        newfile << "Process: "<< current->info.pid << "  Finished at: " << current->info.finishedTime << "  Total time on Ready State: " << current->info.timeOnReady<< endl;

        current = current->next;
        totalTime = totalTime + current->info.timeOnReady;
        totalProcesses++;
    }
    cout << endl;
    cout<< "Total time spent on Ready: " << totalTime << endl;
    cout<< "Average time spent on Ready: " << totalTime/totalProcesses << endl;
    cout<< "Time all processes finished: " << myClock.clck << endl;

    newfile << endl;
    newfile << "Total time spent on Ready: " << totalTime << endl;
    newfile << "Average time spent on Ready: " << totalTime/totalProcesses << endl;
    newfile << "Time all processes finished: " << myClock.clck << endl;

    myfile.close();

    return EXIT_SUCCESS;
}

void initializeList(List *listPtr)
{
    //NodeType node = new NodeType();
    NodeType *node = (struct NodeType*)malloc(sizeof(struct NodeType));
    node->next = node;
    node->prev = node;
    *listPtr = node;
}

bool isEmpty(const List list)
{
    if(list->next==list && list->prev==list)
        return true;
   // else if(list == NULL)
   //     return true;
    else
        return false;
}

void clearList(List *listPtr)
{
    while(!isEmpty(*listPtr))
    {
        popFront(*listPtr);
    }
    *listPtr = NULL;
}

void pushFront(const List list, ListInfo value)
{
    NodeType *temp;
    temp=(NodeType*)malloc(sizeof(NodeType));
    temp->info = value;

   /* if(isEmpty(list))
    {
        temp->next = list;
        temp->prev = list;
        list->next = temp;
        list->prev = temp;
    }*/
    temp->next = list->next;
    temp->prev = list;
    list->next->prev = temp;
    list->next = temp;
}

void pushBack(const List list, ListInfo value)
{
    NodeType *temp;
    temp = (NodeType*)malloc(sizeof(NodeType));
    temp->info = value;

    /*if(isEmpty(list))
    {
        temp->next = list;
        temp->prev = list;
        list->next = temp;
        list->prev = temp;
    }*/
    temp->prev = list->prev;
    temp->next = list;
    list->prev->next = temp;
    list->prev = temp;
}

ListInfo popFront(const List list)
{
    NodeType *temp;
   // if(!isEmpty(list))
   // {
        temp = list->next;
        ListInfo data = temp->info;
        list->next->next->prev = list;
        list->next = list->next->next;
        free(temp);
        return data;
    //}
    //return NULL;

}

ListInfo popBack(const List list)
{
    NodeType *temp;
   // if(!isEmpty(list))
   // {
        temp = list->prev;
        ListInfo data = temp->info;
        list->prev = list->prev->prev;
        list->prev->prev->next = list;
        free(temp);
        return data;
   // }
   // return NULL;

}

bool findInList(const List list, ListInfo value)
{
    NodeType *temp;
    temp = list;

    while(temp->next!=list)
    {
        temp = temp->next;
        if(temp->info.pid==value.pid)
            return true;
    }

    return false;
}

ListInfo getFront(const List list)
{
    NodeType *temp;
   // if(!isEmpty(list))
   // {
        temp = list->next;
        ListInfo data = temp->info;
        return data;
   // }
  //  return NULL;
}

ListInfo getBack(const List list)
{
    NodeType *temp;
   // if(!isEmpty(list))
    //{
        temp = list->prev;
        ListInfo data = temp->info;
        return data;
    //}
    //return NULL;
}

void *incClock(void *data)
{
    while(running)
    {
       // pthread_mutex_lock( &myClock.lock );
        myClock.clck ++;
        updated = true;
        usleep(50000); //make 500 for faster time and remove outputs that may add confusion with process transitions

        //pthread_mutex_unlock( &myClock.lock );
        cout << "Time: " << myClock.clck <<endl; //check if clock is working
    }
}
void *jobEntry(void *data)
{
    //cout<<"check if ready is empty: " << myClock.clck<< " " << isEmpty(myProcess.readyState)<<endl;
    List *dataFile = (List *)data;
    if(!isEmpty(*dataFile))
    {
        PCB pcb = getFront(*dataFile);
        int arrival = pcb.procArrivalTime;

        if(myClock.clck >= arrival)
        {
            pthread_mutex_lock(&myProcess.lock);
            pushBack(myProcess.readyState, popFront(*dataFile));
            pthread_mutex_unlock(&myProcess.lock);
           // cout<< "check back of ready: " << getBack(myProcess.readyState).pid << " " << arrival << " " << myClock.clck<<endl;
        //check make sure moving to ready states
        }
       // cout<<"ready empty?: " << isEmpty(myProcess.readyState)<< endl;

    }
    //cout<< getBack(myProcess.readyState).pid << " " << myClock.clck<<endl;
   //make sure clock is still running


}
void *scheduling(void *tq)
{
    int *timeQ = (int *)tq;
    if(isEmpty(myProcess.currentlyRunning))
    {
        if(!isEmpty(myProcess.readyState))
        {

            pthread_mutex_lock(&myProcess.lock);
            myProcess.readyState->next->info.timeOnReady = myClock.clck - getFront(myProcess.readyState).procArrivalTime;
            pushBack(myProcess.currentlyRunning, popFront(myProcess.readyState));
            myProcess.currentlyRunning->next->info.timeInRunning = myClock.clck;
            pthread_mutex_unlock(&myProcess.lock);
            cout<<"added: " << getBack(myProcess.currentlyRunning).pid << " to running at: "<< getBack(myProcess.currentlyRunning).timeInRunning <<endl;

        }

    }

    if(!isEmpty(myProcess.currentlyRunning))
    {

        if(updated)
        {

            pthread_mutex_lock(&myProcess.lock);
            myProcess.currentlyRunning->next->info.remainingTime = getFront(myProcess.currentlyRunning).procServiceTime - (myClock.clck - (getFront(myProcess.currentlyRunning).timeInRunning));

            pthread_mutex_unlock(&myProcess.lock);
            updated = false;
            cout<<"Process Running: " << getBack(myProcess.currentlyRunning).pid<< "  Time in Running: "<<getBack(myProcess.currentlyRunning).timeInRunning<< " Current Time: " <<myClock.clck<< " Remaining Time: " << getBack(myProcess.currentlyRunning).remainingTime<<endl;
            //cout<<"check back of running: " << getBack(myProcess.currentlyRunning).pid<<endl;

           // if(myClock.clck - (getBack(myProcess.currentlyRunning).timeInRunning) >= *timeQ && (getBack(myProcess.currentlyRunning).remainingTime >0))
            if(((getBack(myProcess.currentlyRunning).procServiceTime - getBack(myProcess.currentlyRunning).remainingTime)>=*timeQ) && (getBack(myProcess.currentlyRunning).remainingTime >0))
            {
              //  cout<<"place back in ready"<<endl;
                pthread_mutex_lock(&myProcess.lock);
                myProcess.currentlyRunning->next->info.procServiceTime = myProcess.currentlyRunning->next->info.remainingTime;
                myProcess.currentlyRunning->next->info.procArrivalTime = myClock.clck;
                pushBack(myProcess.readyState, popBack(myProcess.currentlyRunning));
                pthread_mutex_unlock(&myProcess.lock);
                //cout<< "Updated service time req: " << getBack(myProcess.readyState).procServiceTime << endl;
              //  cout<<"check back of ready: " << getBack(myProcess.readyState).procServiceTime<< "running empty: " << isEmpty(myProcess.currentlyRunning) <<endl;
               // cout<<"back of running: " << getBack(myProcess.currentlyRunning).procServiceTime << endl;
               // cout<<isEmpty(myProcess.currentlyRunning)<<endl;
            }
        }
         // cout<<getBack(myProcess.currentlyRunning).pid<< " "<<getBack(myProcess.currentlyRunning).timeInRunning<< " " <<myClock.clck<< " " << getBack(myProcess.currentlyRunning).remainingTime<<endl;


    }
    //cout<<isEmpty(myProcess.currentlyRunning)<<endl;

}
void *finishedQueue(void *tq)
{

    if(!isEmpty(myProcess.currentlyRunning))
    {
        if(getBack(myProcess.currentlyRunning).remainingTime <= 0)
        {
            pthread_mutex_lock(&myProcess.lock);
            myProcess.currentlyRunning->next->info.finishedTime = myClock.clck;
            pushBack(myProcess.completed, popBack(myProcess.currentlyRunning));
            pthread_mutex_unlock(&myProcess.lock);
            cout<<"process done: " << getBack(myProcess.completed).pid<< " Finished at: " << getBack(myProcess.completed).finishedTime <<endl;

        }

    }

}



