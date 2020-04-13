/*
PLEASE, DO NOT CHANGE void display(bool verbose, bool testing), int getUsableDay() and int getUsableTime() FUNCTIONS.
YOU HAVE TO WRITE THE REQUIRED  FUNCTIONS THAT IS MENTIONED ABOVE. YOU CAN ADD NEW FUNCTIONS IF YOU NEED.
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

#include "task_management_tool.h"

using namespace std; 


void WorkPlan::display(bool verbose,bool testing)
{
	string inone="***";
	if (head!=NULL)
	{
		Task *pivot =new Task;
		Task *compeer =new Task;
			
		pivot=head;
		do
		{
			if(testing)
				inone+=" ";
			else
				cout << pivot->day <<". DAY"<< endl;
			compeer=pivot;
			while(compeer!=NULL)
			{
				string PREV= compeer->previous!=NULL?compeer->previous->name:"NULL";
				string NEXT=compeer->next!=NULL?compeer->next->name:"NULL";
				string CONT=compeer->counterpart!=NULL?compeer->counterpart->name:"NULL";
				if (testing)
					inone+=compeer->name;
				else
					if(verbose)
						cout<<"\t"<<setw(2)<< compeer->time <<":00\t"<< PREV<<"\t<- "<< compeer->name <<"("<< compeer->priority <<")->\t"<<NEXT <<"\t |_"<<CONT<<endl;
					else
						cout<<"\t"<<setw(2)<< compeer->time <<":00\t"<< compeer->name <<"("<< compeer->priority <<")"<<endl;				
				compeer=compeer->counterpart;
			}
			pivot=pivot->next;
		}while(pivot!=head);
		if(testing)
		{
			cout<<inone<<endl;
			cout<<"(checking cycled list:";
			if (checkCycledList())
				cout<<" PASS)"<<endl;
			else
				cout<<" FAIL)"<<endl;
		}
	}
	else
		cout<<"There is no task yet!"<<endl;
}

int WorkPlan::getUsableDay()
{
	return usable_day;
}

int WorkPlan::getUsableTime()
{
	return usable_time;
}


void WorkPlan::create()
{		
	//THIS FUNCTION WILL BE CODED BY YOU

	head=NULL;
	usable_day=0;
	usable_time=0;
}

void WorkPlan::close()
{
	//THIS FUNCTION WILL BE CODED BY YOU

	Task *traverse,*sub;
	traverse=head->next;
	sub=traverse->counterpart;
	while(traverse!=head){
		while(sub){
			traverse->counterpart=sub->counterpart;
			sub->counterpart=NULL;
			delete sub;
			sub=traverse->counterpart;
		}
		delete traverse;
		traverse=traverse->next;
		sub=traverse->counterpart;
	}
	if(traverse==head && !traverse->counterpart){
		delete traverse;
	}
	else if(traverse==head && traverse->counterpart){
		while(sub){
			traverse->counterpart=sub->counterpart;
			sub->counterpart=NULL;
			delete sub;
			sub=traverse->counterpart;
		}
	}
}

void WorkPlan::add(Task *task)
{
	//THIS FUNCTION WILL BE CODED BY YOU
	Task *traverse,*appointment,*tail;
	traverse=head;
	tail=traverse;
	appointment=new Task;
	appointment->name=new char[strlen(task->name)];
	strcpy(appointment->name,task->name);
	appointment->day=task->day;
	appointment->time=task->time;
	appointment->priority=task->priority;
	
	if(head==NULL){ //add first appointment
		head=appointment;
		head->next=appointment;
		head->previous=appointment;
		return;
	}
		
	if(traverse->day > appointment->day){ // add before head
		(traverse->previous)->next=appointment;
		appointment->next=traverse;
		appointment->previous=traverse->previous;
		traverse->previous=appointment;
		head=appointment;
		return;
	}

	while(traverse->day < appointment->day){ // add appointment after head's day
		traverse=traverse->next;
		if(traverse==head){ //add to end
			(traverse->previous)->next=appointment;
			appointment->next=traverse;
			appointment->previous=traverse->previous;
			traverse->previous=appointment;
			return;
		}
		
		if(traverse->day > appointment->day){ // add between two days
			(traverse->previous)->next=appointment;
			appointment->next=traverse;
			appointment->previous=traverse->previous;
			traverse->previous=appointment;
			return;
		}
	
	}
	
	if(traverse->day == appointment->day){ 
		
		if(traverse->time > appointment->time){ // add appointment as the first appointment of day
			appointment->counterpart=traverse;
			(traverse->previous)->next=appointment;
			appointment->previous=traverse->previous;
			(traverse->next)->previous=appointment;
			appointment->next=traverse->next;
			traverse->next=NULL;
			traverse->previous=NULL;
			if(traverse==head){
				head=appointment;
				return;
			}else return;
			
		}
		else{
			while(traverse){
				tail=traverse;
				if((!traverse->counterpart) && traverse->time < appointment->time){ // add appointment as the second appointment of day
					traverse->counterpart=appointment;
					return;
				}
				if(traverse->time == appointment->time){
					if(traverse->priority > appointment->priority){ // add low priority appointment 
						checkAvailableNextTimesFor(appointment);
						appointment->day=getUsableDay();
						appointment->time=getUsableTime();
						add(appointment);
						delete appointment;
						return;
					}
					
					if(traverse->priority < appointment->priority){ // add high priority appointment
						if(traverse->next){
							(traverse->next)->previous=appointment;
							appointment->next=traverse->next;
							(traverse->previous)->next=appointment;
							appointment->previous=traverse->previous;
							traverse->next=NULL;
							traverse->previous=NULL;
							if(traverse==head){
								head=appointment;
							}
						}
						
						if(traverse->counterpart || tail->counterpart){
							if(tail==traverse){
								appointment->counterpart=traverse->counterpart;
								traverse->counterpart=NULL;
							}
							else{
								tail->counterpart=appointment;
								if(traverse->counterpart){
									appointment->counterpart=traverse->counterpart;
								}
								traverse->counterpart=NULL;
							}	
						}
						checkAvailableNextTimesFor(traverse);
						traverse->day=getUsableDay();
						traverse->time=getUsableTime();
						add(traverse);
						delete traverse;
						return;
					}
				}
				
				traverse=traverse->counterpart;
				if(traverse->time > appointment->time){ // add appointment between two appointments on same day
					tail->counterpart=appointment;
					appointment->counterpart=traverse;
					return;
				}
			}
		}
	}
}

Task * WorkPlan::getTask(int day, int time)
{
	//THIS FUNCTION WILL BE CODED BY YOU
	Task *task,*traverse;
	traverse=head;
	
	while(traverse->day!=day){
		traverse=traverse->next;
	}
	while(traverse->time!=time){
		traverse=traverse->counterpart;
	}
	task=new Task;
	*task=*traverse;
	return task;
}

void WorkPlan::checkAvailableNextTimesFor(Task *delayed)	
{	//THIS FUNCTION WILL BE CODED BY YOU
	Task *traverse,*tail;
	traverse=head;
	tail=traverse;
	while(traverse->day != delayed->day){// search delayed day
		traverse=traverse->next;
		tail=traverse;
	}
	
	while(traverse->time!=delayed->time){// search delayed time
		traverse=traverse->counterpart;
	}
	for(int i=(delayed->time)+1;i<=17;i++){ // search available time on the same day 
		if((!(traverse->counterpart)) || i==17){
			traverse=tail;
			break;
		} 
		traverse=traverse->counterpart;
		if(i < traverse->time){
			usable_day=traverse->day;
			usable_time=i;
			return;
		}
	}
	
	while(traverse != head){ //search available time on next days
		traverse=traverse->next;
		tail=traverse;
		for(int j=8;j<=16;j++){
			if(j != traverse->time){
				usable_time=j;
				usable_day=traverse->day;
				return;
			}
			traverse=traverse->counterpart;
			if(!traverse){
				traverse=tail;
				break;
			}
		}
	}
	if(traverse == head){ // if there is no available time on next days
		while(traverse->day != delayed->day){
			traverse=traverse->next;
			tail=traverse;
		}
		int a=delayed->day;
		while(traverse->day == a){
			traverse=traverse->next;
			a++;
			if(traverse->day > a || traverse == head){
				usable_day=a;
				usable_time=8;
				return;
			}
		}
	}
}

void WorkPlan::delayAllTasksOfDay(int day)
{
	//THIS FUNCTION WILL BE CODED BY YOU
	Task *traverse,*newapp;
	traverse=head;
	
	while(traverse->day != day){ // search the day
		traverse=traverse->next;
	}
	newapp=new Task;
	*newapp=*traverse;
	checkAvailableNextTimesFor(newapp);
	newapp->day=getUsableDay();
	newapp->time=getUsableTime();
	add(newapp);
	
	if(traverse->counterpart){ 
		remove(traverse);
		delayAllTasksOfDay(day);
	}
	else{
		remove(traverse);
		return;
	}
}

void WorkPlan::remove(Task *target)
{
	//THIS FUNCTION WILL BE CODED BY YOU
	Task *traverse,*tail;
	traverse=head;
	tail=traverse;

	while(traverse->day!=target->day){ // search target day
		traverse=traverse->next;
	}
	while(traverse->time!=target->time){ // search target time
		tail=traverse;
		traverse=traverse->counterpart;
	}
	if(!(traverse->next)){ // if target is not first appointment 
		tail->counterpart=traverse->counterpart;
		delete traverse;
		return;
	}
	else if(traverse==head){ 
		if(!(traverse->counterpart)){ // if target is head and there is no appointment after that
			head=traverse->next;
			(traverse->previous)->next=traverse->next;
			(traverse->next)->previous=traverse->previous;
			delete traverse;
			return;
		}
		else{ // if target is head and there is appointment after that
			head=head->counterpart;
			(traverse->previous)->next=traverse->counterpart;
			(traverse->counterpart)->previous=traverse->previous;
			(traverse->next)->previous=traverse->counterpart;
			(traverse->counterpart)->next=traverse->next;
			delete traverse;
			return;
		}
	}
	else{
		if(!(traverse->counterpart)){ // if target is not head and the only appointment on that day
			(traverse->previous)->next=traverse->next;
			(traverse->next)->previous=traverse->previous;
			delete traverse;
			return;
		}
		else{ // if target is not head and not only appointment on that day
			(traverse->previous)->next=traverse->counterpart;
			(traverse->counterpart)->previous=traverse->previous;
			(traverse->next)->previous=traverse->counterpart;
			(traverse->counterpart)->next=traverse->next;
			delete traverse;
			return;
		}
	}
}

bool WorkPlan::checkCycledList()
{				
	Task *pivot=new Task();
	pivot=head;
	int patient=100;
	bool r=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->previous;
		if(pivot==head)
		{
			r=true;
			break;
		}
	}
	cout<<"("<<100-patient<<")";
	patient=100;
	bool l=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->next;
		if(pivot==head)
		{
			l=true;
			break;
		}
	}
	return r&l;
}
