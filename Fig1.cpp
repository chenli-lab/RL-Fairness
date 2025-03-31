/******************************************************************************************/
/*                          Two-player ultimatum game  Q-learning                         */
/******************************************************************************************/
#include <iostream>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <stack>
#include <iomanip>
#define PI 3.1415926

using namespace std;

//-- array creation and destruction --//
//1D
void make1Darray(int *& array,int rows)
{
    array=new int [rows];
}

void delete1Darray(int *array)
{
    delete []array;
    array=0;
}

void make1Darray(double *& array,int rows)
{
    array=new double [rows];
}

void delete1Darray(double *array)
{
    delete []array;
    array=0;
}

//2D
void make2Darray(int **& array,int rows,int cols)
{
    array=new int *[rows];
    for(int i=0;i<rows;i++)  
        array[i]=new int [cols];
}

void delete2Darray(int **array,int rows)
{
    for(int i=0;i<rows;i++)
        delete []array[i];
    delete []array;
    array=0;
}

void make2Darray(double **& array,int rows,int cols)
{
    array=new double *[rows];
    for(int i=0;i<rows;i++)  
        array[i]=new double [cols];
}

void delete2Darray(double **array,int rows)
{
    for(int i=0;i<rows;i++)
        delete []array[i];
    delete []array;
    array=0;
}

//3D
void make3Darray(int *** &array,int high,int row,int col)
{
    int i,j;
    array=new int **[high];
    for(i=0;i<high;i++)
        array[i]=new int*[row];
    
    for(i=0;i<high;i++)
        for(j=0;j<row;j++)
            array[i][j]=new int[col];
}

void delete3Darray(int ***array,int hight, int row)
{
    int i,j;
    for(i=0;i<hight;i++)
        for(j=0;j<row;j++)
            delete []array[i][j] ;
    
    for(i=0;i<hight;i++)
        delete []array[i] ;
    delete []array;
    array=0 ;
}

void make3Darray(double ***&array,int high,int row,int col)
{
    int i,j;
    array=new double **[high];
    for(i=0;i<high;i++)
        array[i]=new double* [row];
    
    for(i=0;i<high;i++)
        for(j=0;j<row;j++)
            array[i][j]=new double[col];
}

void delete3Darray(double ***array,int hight, int row)
{
    int i,j;
    for(i=0;i<hight;i++)
        for(j=0;j<row;j++)
            delete []array[i][j] ;
    for(i=0;i<hight;i++)
        delete []array[i] ;
    delete []array;
    array=0 ;
}

double payoff_proposer;
double payoff_responder;

void Payoff_function(double p1,double q1,double p2,double q2)  //Returns to the proposer's proceeds
{
    if(p1>=q2)                                                 //player 1 as the proposer,player 2 as the responer
    {
        payoff_proposer=1-p1;
        payoff_responder=p1;
    }
    else
    {
        payoff_proposer=0;
        payoff_responder=0;
    }
}
double Modification_function(double x)
{
    if(x>=1)
        return 1.0;
    else if(x<=0)
        return 0.0;
    else
        return x;
}



#define size 2
#define tmax 100001000                          
int main()
{
	int U,O,n_ave,realization;
	double LLP[50][50],MMP[50][50],HHP[50][50],LLQ[50][50],MMQ[50][50],HHQ[50][50];
	int i,j,k,t,it,N,t_log,T; 
	int number;
	int lp,mp,hp,lq,mq,hq;
	int combin[3][3];
	int num_state,num_action;
	double p_1[size],q_1[size];
	double ave_lp,ave_mp,ave_hp,ave_lq,ave_mq,ave_hq;
	double Frac_lp,Frac_mp,Frac_hp;
	double Frac_lq,Frac_mq,Frac_hq;
	double p_ix,p_it,q_ix,q_it;
	double ***Qtable_P,***Qtable_R,*Reward_P,*Reward_R,Q_max_P,Q_max_R,Q_max_next_P,Q_max_next_R;
	int *Action_P,*Action_R,*State_new_P,*State_new_R,*State_P,*State_R;
	double noise,r,epsilon,alpha,gamma;
	double low,medium,high;
	double t_log_exp;
	N = size;
	
    epsilon=0.01;                              //exploration rate
    
    num_action=3;                              //number of actions
    num_state=9;                               //number of states
    
    low = 0.3;
	medium = 0.5;
	high = 0.8; 
    
    t_log=int(pow(10,3));         //starting at 1024 for log output
    t_log_exp=3;
	
	make1Darray(State_P,N);                                   // State array for proposers
	make1Darray(State_R,N);      							  // State array for responders
    make1Darray(State_new_P,N);                               // New state array for proposers
    make1Darray(State_new_R,N);                               // New state array for responders
    make1Darray(Action_P,N);                                  // Action array for proposers
    make1Darray(Action_R,N);  								  // Action array for responders
    make1Darray(Reward_P,N);                                  //the Reward obtained
    make1Darray(Reward_R,N);                                  //the Reward obtained
	make3Darray(Qtable_P,N,num_state,num_action);             //Q table for each proposer
	make3Darray(Qtable_R,N,num_state,num_action);             //Q table for each responder
	
	ofstream outfile("fra_lp.txt");
    if(!outfile){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }
    ofstream outfile1("fra_mp.txt");
    if(!outfile1){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }
    ofstream outfile2("fra_hp.txt");
    if(!outfile2){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }
    ofstream outfile3("fra_lq.txt");
    if(!outfile3){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }
    ofstream outfile4("fra_mq.txt");
    if(!outfile4){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }
    ofstream outfile5("fra_hq.txt");
    if(!outfile5){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }
	
	srand48((long)time(NULL));
//    srand((unsigned)time(NULL));	
for(U=0;U<50;U++)   
{
	alpha = (U+1)*0.02;  //Adjust the learning rate 
	for(O=0;O<50;O++)
	{
		gamma = O*0.02;   //Adjust the discount factor
		
		ave_lp = 0;
		ave_mp = 0;
		ave_hp = 0;
		
		ave_lq = 0;
		ave_mq = 0;
		ave_hq = 0;
		for(realization=0;realization<100;realization++)
		{
		
			//---------------------- System Initialization -----------------//
			number = 0;
			for(i=0;i<3;i++)
			{
				for(j=0;j<3;j++)
				{
					combin[i][j] = number;
					number++;
				}
			}
			
		    for(i=0;i<N;i++)
		    {
		        for(j=0;j<num_state;j++)
		        {
		            for(k=0;k<num_action;k++)
		            {
		                Qtable_P[i][j][k] = drand48();                  
			            Qtable_R[i][j][k] = drand48(); 
		            }
		        }
		        Action_P[i] = rand()%num_action;                         //0 or 1 or 2
		        Action_R[i] = rand()%num_action;                         //0 or 1 or 2
		  		
		        Reward_P[i] = 0;                                //No reward at the beginning
		        Reward_R[i] = 0;
		    }
		    //----------------Statistical state-----------------//
		    for(i=0;i<N;i++)
			{    
				it = ((i+1)%size+size)%size;
				
			    State_P[i] = combin[Action_P[i]][Action_R[it]];                                 
		        State_R[i] = combin[Action_R[i]][Action_P[it]];	    
			}
			//-----------------------Dynamical Evolution---------------------------------------//
			n_ave = 0;
			Frac_lp = 0;
			Frac_mp = 0;
			Frac_hp = 0;
			Frac_lq = 0;
			Frac_mq = 0;
			Frac_hq = 0;
			
			for(t=1;t<tmax;t++)
			{
				//--------Choose an action------------//
		
				for(i=0;i<N;i++)
				{
					r = drand48();
					if(r<epsilon)                         //Exploration with greedy algorithm
					{
						Action_P[i] = rand()%num_action;             
					}
					else                                  //Exploitation for the optimal action
					{
						Q_max_P = -1000;                     //starting with a very small value
						for(k=0;k<num_action;k++)                                //go through all neighbors
						{
						    if(Qtable_P[i][State_P[i]][k] > Q_max_P)            //Search for the iterm with the largest Q value within the row with given state State[i]
						    {
						        Q_max_P = Qtable_P[i][State_P[i]][k];
						        Action_P[i] = k;                                      
						    }      
						}
					}
					
					r = drand48();
					if(r<epsilon)                        //Exploration with greedy algorithm
					{
						Action_R[i] = rand()%num_action;           
					}
					else								//Exploitation for the optimal action
					{
						Q_max_R = -1000;                     //starting with a very small value
						for(k=0;k<num_action;k++)                                //go through all neighbors
						{
						    if(Qtable_R[i][State_R[i]][k] > Q_max_R)                    //Search for the iterm with the largest Q value within the row with given state State[i]
						    {
						        Q_max_R = Qtable_R[i][State_R[i]][k];
						        Action_R[i] = k;                               
						    }      
						}
					}
				
				}
				//---------------------Perform action --------------------//
			    lp = 0;
				mp = 0;
				hp = 0;
				
				lq = 0;
				mq = 0;
				hq = 0;
				for(i=0;i<N;i++)
				{
					if(Action_P[i] == 0)
					{
						p_1[i] = low;
						lp++;
					}
					else if(Action_P[i] == 1)
					{
						p_1[i] = medium;
						mp++;
					}
					else
					{
						p_1[i] = high;
						hp++;
					}
					
					if(Action_R[i] == 0)
					{
						q_1[i] = low;
						lq++;	
					}
					else if(Action_R[i] == 1)
					{
						q_1[i] = medium;
						mq++;
					}
					else
					{
					    q_1[i] = high;
					    hq++;
					}
					
				}
				
				//---------------- Output ----------------//
			    if(t>=100000000&&t%10==0)     //After 10^8 rounds, select one data point every 10 rounds within a 1000-round time window.
			    {
			    	Frac_lp += lp;
				   	Frac_mp += mp;
				   	Frac_hp += hp;           //Perform accumulation of data over different time intervals
				   		
				   	Frac_lq += lq;
				   	Frac_mq += mq;
				   	Frac_hq += hq;
		
					n_ave++; 
				}
							
			//-------------measure the reward---------------//
				for(i=0;i<N;i++)
				{
					it = ((i+1)%size+size)%size; 
						
					p_ix=p_1[i];                               //effective p of node ix;
				    p_it=p_1[it];                               //effective p of node it;
				        
					q_ix=q_1[i];                               //effective q of node ix;
				    q_it=q_1[it];  
						
					Payoff_function(p_ix,q_ix,p_it,q_it);
						
					Reward_P[i] = payoff_proposer;
					Reward_R[it] = payoff_responder;
				}
			//--------------Statistics state-----------//
				for(i=0;i<N;i++)
				{    
					it = ((i+1)%size+size)%size;
					    
					State_new_P[i] = combin[Action_P[i]][Action_R[it]];                                     
				    State_new_R[i] = combin[Action_R[i]][Action_P[it]];	    
				}
				for(i=0;i<N;i++)
				{
				    Q_max_next_P = -1000;
				 
					for(k=0;k<num_action;k++)                                   //go through all actions
				    {
				        if(Qtable_P[i][State_new_P[i]][k] > Q_max_next_P)                //Search for the iterm with the largest Q value
				        {
				            Q_max_next_P = Qtable_P[i][State_new_P[i]][k];
				        }    
				    }
				    Qtable_P[i][State_P[i]][Action_P[i]] = (1-alpha)*Qtable_P[i][State_P[i]][Action_P[i]] + alpha*(Reward_P[i] + gamma*Q_max_next_P);
				        
				    Q_max_next_R = -1000;
				 
					for(k=0;k<num_action;k++)                                   //go through all actions
				    {
				        if(Qtable_R[i][State_new_R[i]][k] > Q_max_next_R)                //Search for the iterm with the largest Q value
				        {
				            Q_max_next_R = Qtable_R[i][State_new_R[i]][k];
				        }    
				    }
				    Qtable_R[i][State_R[i]][Action_R[i]] = (1-alpha)*Qtable_R[i][State_R[i]][Action_R[i]] + alpha*(Reward_R[i] + gamma*Q_max_next_R);
				}
				        
				for(i=0;i<N;i++)
			    {
			        State_P[i]=State_new_P[i];
			        State_R[i]=State_new_R[i];
			    }
		    }
		    ave_lp += (double)Frac_lp/(N*n_ave);
		    ave_mp += (double)Frac_mp/(N*n_ave);
		    ave_hp += (double)Frac_hp/(N*n_ave);
		    ave_lq += (double)Frac_lq/(N*n_ave);    //Perform accumulation of the data after each realization.
		    ave_mq += (double)Frac_mq/(N*n_ave);
		    ave_hq += (double)Frac_hq/(N*n_ave);
		}
		LLP[U][O] = (double)ave_lp/100;
		MMP[U][O] = (double)ave_mp/100;
		HHP[U][O] = (double)ave_hp/100;
		LLQ[U][O] = (double)ave_lq/100;
		MMQ[U][O] = (double)ave_mq/100;
		HHQ[U][O] = (double)ave_hq/100;
	}
	
}
	for(U=0;U<50;U++)
    {
      	for(O=0;O<50;O++)
		{
			outfile<<LLP[U][O]<<"    ";	
			outfile1<<MMP[U][O]<<"    ";
			outfile2<<HHP[U][O]<<"    ";
			outfile3<<LLQ[U][O]<<"    ";	
			outfile4<<MMQ[U][O]<<"    ";
			outfile5<<HHQ[U][O]<<"    ";		
		}
		outfile<<endl;	
		outfile1<<endl;
		outfile2<<endl;
		outfile3<<endl;	
		outfile4<<endl;
		outfile5<<endl;		
	}
	
	delete1Darray(State_P);
	delete1Darray(State_R);
    delete1Darray(State_new_P);
    delete1Darray(State_new_R);
    
    delete1Darray(Action_P);
    delete1Darray(Action_R);
    delete1Darray(Reward_P);
    delete1Darray(Reward_R);
    delete3Darray(Qtable_P,N,num_state);
    delete3Darray(Qtable_R,N,num_state);
	return 0;
	
}
