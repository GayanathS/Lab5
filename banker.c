/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Gayanath Sasitharanathan
 * All rights reserved.
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "banker.h"


#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3


// Put global environment variables here
// Available amount of each resource
int available[NUM_RESOURCES];

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];

pthread_mutex_t mutex;

void * entered(void *arg);

bool is_safety_check()
{
    bool is_safe = false;
    int work_load[NUM_RESOURCES];
    bool finish[NUM_CUSTOMERS] = {0};
    int num_finished = NUM_CUSTOMERS;

    while(num_finished > 0)
    {
        bool found = false;
        for(int i = 0; i < NUM_RESOURCES; i ++)
        {
            work_load[i] = available[i];
        }
        for(int i = 0; i < NUM_CUSTOMERS; i ++)
        {
            for(int c = 0; c < NUM_RESOURCES; c ++)
            {
                if(need[i][c] <= work_load[c] && finish[i] == false)
                {
                    work_load[c] += allocation[i][c];
                    finish[i] = true;
                    found = true;
                    num_finished --;
                }
            }
        }
        if(!found)
        {
            break;
        }
       
    }
    if(num_finished ==0)
    {
        is_safe = true;   
        printf("The system is is_safe\n");
    }
    else
    {
        printf("The system is unis_safe\n");
    }
    return is_safe;
}

// Define functions declared in banker.h here
bool request_res(int n_customer, int request[])
{
    printf("thread #%d requesting\n", n_customer + 1);
    for(int i = 0; i < NUM_RESOURCES; i ++)
    {
        printf("%d from resource %d\n", request[i], i + 1);
    }
    bool success = false;
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < NUM_RESOURCES; i ++)
    {
        if(request[i] <= need[n_customer][i])
        {
            if(request[i] <= available[i])
            {
                available[i] -= request[i];
                allocation[n_customer][i] += request[i];
                need[n_customer][i] -= request[i];

                if(is_safety_check())
                {
                    success = true;
                    printf("resources requested successfully\n");
                }
                else
                {
                    printf("resource request denied\n");
                    available[i] += request[i];
                    allocation[n_customer][i] -= request[i];
                    need[n_customer][i] += request[i];
                }               
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    
    return success;
}

// Release resources
void release_res(int n_customer, int release[])
{
    printf("thread #%d releasing\n", n_customer + 1);
    for(int i = 0; i < NUM_RESOURCES; i ++)
    {
        printf("%d from resource %d\n", release[i], i + 1);
    }
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < NUM_RESOURCES; i ++)
    {  
        available[i] += release[i];
        allocation[n_customer][i] -= release[i];
        need[n_customer][i] += release[i];
    }
    pthread_mutex_unlock(&mutex);
}


int main(int argc, char *argv[])
{

	if(argc != NUM_RESOURCES + 1)
	{
		return 1;
	}

	for(int i = 0; i < NUM_RESOURCES; i ++)
	{
        available[i] = atoi(argv[i + 1]);
	}
    time_t t;
    srand((unsigned) time(&t));

    pthread_t threads[NUM_CUSTOMERS];

    for(int i = 0; i < NUM_CUSTOMERS; i ++)
    {
        for(int c = 0; c < NUM_RESOURCES; c ++)
        {
            allocation[i][c] = 0;
            maximum[i][c] = rand() % available[c];
            need[i][c] = maximum[i][c] - allocation[i][c];
        }
    }
      
    for(int i = 0; i < NUM_CUSTOMERS; i ++)
    {
        pthread_create(&threads[i], NULL, entered, (void *)i);
    }

     for(int i = 0; i < NUM_CUSTOMERS; i ++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void * entered(void *arg)
{
    fflush(stdout);

    int customer = (int)arg;
    int a = 0;
    while(1)
    {
        int request[NUM_RESOURCES];
        
        for(int i = 0; i < NUM_RESOURCES; i ++)
        {
            int random_number = 0;
            if(need[customer][i] > 0)
            {
                random_number = rand() % need[customer][i];
            }
            request[i] = random_number;
        }
        sleep(2);
        while(!request_res(customer, request))
        {
            sleep(2);
        }
        sleep(2);
        release_res(customer, request);
        sleep(2);
    }
  
}