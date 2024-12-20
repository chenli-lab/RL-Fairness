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

//-----------payoff function--------------//
double payoff_proposer;
double payoff_responder;

void Payoff_function(double p1,double q1,double p2,double q2)  //player 1 as the proposer,player 2 as the responer
{
    if(p1>=q2)                                                 
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

#define size 2                    
#define tmax 60000000                          

int main()
{
	int realization;
	int i,j,k,t,it,N,t_log; 
	int number;
	int lp,mp,hp,lq,mq,hq;
	int combin[3][3];
	int num_state,num_action;
	double p_1[size],q_1[size];
	double ave_lp,ave_mp,ave_hp,ave_lq,ave_mq,ave_hq;
	double density_lp[tmax],density_mp[tmax],density_hp[tmax];
	double density_lq[tmax],density_mq[tmax],density_hq[tmax];
	double p_ix,p_it,q_ix,q_it;
	double ***Qtable_P,***Qtable_R,*Reward_P,*Reward_R,Q_max_P,Q_max_R,Q_max_next_P,Q_max_next_R;
	int *Action_P,*Action_R,*State_new_P,*State_new_R,*State_P,*State_R;
	double r,epsilon,alpha,gamma;
	double low,medium,high;
	double t_log_exp;
	
	N = size;
	
	alpha=0.1;                                 //learning rate
    gamma=0.9;                                 //discount factor
    epsilon=0.01;                             //exploration rate
    
    num_action=3;                             //number of actions
    num_state=9;                              //number of states
    
    low = 0.3;                               //three fixed offers and acceptance thresholds
	medium = 0.5;
	high = 0.8; 
    
    t_log=int(pow(10,3));              
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
	
	ofstream outfile1("Basic_timeseries.txt");
    if(!outfile1){
        cout<<"You can't open the pointed fme!"<<endl;
        exit(1);
    }   

	srand48((long)time(NULL));
//  srand((unsigned)time(NULL));	
	
	//---------------------- System Initialization -----------------//
	for(i=0;i<tmax;i++)
	{
	    density_lp[i]=0;
	    density_mp[i]=0;
	    density_hp[i]=0;
	    
		density_lq[i]=0;
		density_mq[i]=0;	
		density_hq[i]=0;
	}
	
	number = 0;
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			combin[i][j] = number;
			number++;
		}
	}
	
	for(realization=0;realization<500;realization++)
	{	
	    for(i=0;i<N;i++)
	    {
	        for(j=0;j<num_state;j++)
	        {
	            for(k=0;k<num_action;k++)
	            {
	                Qtable_P[i][j][k] = drand48();                  //all iterms in Q-table are random assigned
		            Qtable_R[i][j][k] = drand48(); 
	            }
	        }
	        Action_P[i] = rand()%num_action;                         //set to 0 or 1 or 2 randomly
	        Action_R[i] = rand()%num_action;                      
	  		
	        Reward_P[i] = 0;                                		//No reward at the beginning
	        Reward_R[i] = 0;
	    }
    
        lp = 0;
		mp = 0;
		hp = 0;
		
		lq = 0;
		mq = 0;
		hq = 0;
		for(i=0;i<N;i++)
		{
			if(Action_P[i] == 0)          //low
			{
				lp++;
			}
			else if(Action_P[i] == 1)     //medium
			{
				mp++;
			}
			else                         //high
			{
				hp++;
			}
			
			if(Action_R[i] == 0)
			{
				lq++;	
			}
			else if(Action_R[i] == 1)
			{
				mq++;
			}
			else
			{
			    hq++;
			}
		}
		
		density_lp[0] += (double)lp/N;                        //Record the fraction of options under the initial conditions
		density_mp[0] += (double)mp/N;
		density_hp[0] += (double)hp/N;
		
		density_lq[0] += (double)lq/N;
		density_mq[0] += (double)mq/N;
		density_hq[0] += (double)hq/N;
		
	    //----------------Statistical state-----------------//
	    for(i=0;i<N;i++)
		{    
			it = ((i+1)%size+size)%size;
			
		    State_P[i] = combin[Action_P[i]][Action_R[it]];            
	        State_R[i] = combin[Action_R[i]][Action_P[it]];	    
		}
		//---------Dynamical Evolution--------------------//
		for(t=1;t<tmax;t++)
		{
			//--------Choose an action------------//
			for(i=0;i<N;i++)                     
			{
				//---------------for proposer-------------//	
				r = drand48();
				if(r<epsilon)                         //Exploration with greedy algorithm
				{
					Action_P[i] = rand()%num_action;             
				}
				else                                  //Exploitation for the optimal action
				{
					Q_max_P = -1000;                  //starting with a very small value
					for(k=0;k<num_action;k++)             			//go through all actions
					{
					    if(Qtable_P[i][State_P[i]][k] > Q_max_P)                  //Search for the iterm with the largest Q value within the row with given state State[i]
					    {
					        Q_max_P = Qtable_P[i][State_P[i]][k];
					        Action_P[i] = k;                                      
					    }      
					}
				}
				//---------------for responder-------------//		
				r = drand48();
				if(r<epsilon)                         //Exploration with greedy algorithm
				{
					Action_R[i] = rand()%num_action;            
				}
				else								  //Exploitation for the optimal action
				{
					Q_max_R = -1000;                  //starting with a very small value
					for(k=0;k<num_action;k++)                                //go through all actions
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
			
			density_lp[t] += (double)lp/N;
			density_mp[t] += (double)mp/N;
			density_hp[t] += (double)hp/N;         //Record the fraction of options of round t in this realization
			
			density_lq[t] += (double)lq/N;
			density_mq[t] += (double)mq/N;
			density_hq[t] += (double)hq/N;
			
			//-------------measure the reward---------------//
			for(i=0;i<N;i++)
			{
				it = ((i+1)%size+size)%size; 
				
				p_ix=p_1[i];                                //effective p of node ix;
		        p_it=p_1[it];                               //effective p of node it;
		        
				q_ix=q_1[i];                               //effective q of node ix;
		        q_it=q_1[it];  							   //effective q of node it;
				
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
			//--------------Update the Q table----------------------//		
			for(i=0;i<N;i++)
		    {
		    	
		    	//--------------for the proposer role----------//	
		        Q_max_next_P = -1000;
		 
				for(k=0;k<num_action;k++)                                     //go through all actions
		        {
		            if(Qtable_P[i][State_new_P[i]][k] > Q_max_next_P)         //Search for the iterm with the largest Q value
		            {
		                Q_max_next_P = Qtable_P[i][State_new_P[i]][k];
		            }    
		        }
		        Qtable_P[i][State_P[i]][Action_P[i]] = (1-alpha)*Qtable_P[i][State_P[i]][Action_P[i]] + alpha*(Reward_P[i] + gamma*Q_max_next_P);
		       
			   //--------------for the responder role----------// 
		        Q_max_next_R = -1000;
		 
				for(k=0;k<num_action;k++)                                     //go through all actions
		        {
		            if(Qtable_R[i][State_new_R[i]][k] > Q_max_next_R)          //Search for the iterm with the largest Q value
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
	}
		
	//-----------------output----------------------//
	for(t=0;t<tmax;t++)
	{	
		ave_lp=density_lp[t]/500;
		ave_mp=density_mp[t]/500;
		ave_hp=density_hp[t]/500;
		ave_lq=density_lq[t]/500;
		ave_mq=density_mq[t]/500;
		ave_hq=density_hq[t]/500;
			
			
		if(t<1000)
		{
		    outfile1<<t<<"  "<<ave_lp<<"  "<<ave_mp<<"  "<<ave_hp<<"  "<<ave_lq<<"  "<<ave_mq<<"  "<<ave_hq<<endl;  
		}
		else if(t==t_log)
		{
		    outfile1<<t<<"  "<<ave_lp<<"  "<<ave_mp<<"  "<<ave_hp<<"  "<<ave_lq<<"  "<<ave_mq<<"  "<<ave_hq<<endl;  
		                
		    t_log_exp += 0.002;
		    t_log=int(pow(10,t_log_exp));         //for visualization within log(t)
		}
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
