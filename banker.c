/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "banker.h"
#include <unistd.h>


pthread_mutex_t mutex;

int num_available[resource_num];
int maximum[number_of_customers][resource_num];
int allocation[number_of_customers][resource_num];
int amount_needed[number_of_customers][resource_num];

int main(int argc, char *argv[])
{
    system("clear");
    if(argc -1 != resource_num)
    {
		printf("ERROR: Expected %d reources, recieved %d\n", resource_num, argc);
		exit(EXIT_FAILURE);
    }
    
    for(int a = 0; a < resource_num; a++)
    {
		num_available[a] = atoi(argv[a+1]);
    }
   
    print_array("num of available Resources", num_available, resource_num, true);
    printf("\n");
    
    pthread_mutex_init(&mutex,NULL);


    customer_struct processes[number_of_customers];
    for (int i = 0; i < number_of_customers; i++)
    {
		customer_struct s;
   
		s.thread_num = i;
    
			for(int j = 0; j < resource_num; j++)
			{
				int resource_max = rand() % atoi(argv[j+1]);
				s.max[j] = resource_max;
				allocation[i][j] = 0;
				maximum[i][j] = resource_max;
				amount_needed[i][j] = resource_max;
			}

		print_struct(s);
		processes[i] = s;
	}

	printf("\nPress enter to continue\n");
	printf("===================================\n");

    pthread_t customers[number_of_customers];
    
	for(int q = 0; q < number_of_customers; q++)
	{
		pthread_create(&customers[q], 0, customer, (void *) &processes[q]);
    }
    
	for (int k = 0; k < number_of_customers; k++)
	{
		pthread_join(customers[k],0);
	}

   
	printf("\nAll processes completed program closing\n\n");
    
	pthread_mutex_destroy(&mutex);
    
    return EXIT_SUCCESS;
}

void *customer(void *arg)
{

	customer_struct* s = (customer_struct*) arg;
	int has[resource_num] = {0};
	bool complete = false;

	while(!complete)
	{
		getchar();
  
	
		int req[resource_num];
		for(int i = 0; i < resource_num; i++)
		{
			req[i] = rand() % (s->max[i] - has[i] + 1);
		}  
		pthread_mutex_lock(&mutex);
		bool request_granted = request_res(s->thread_num, req);
		pthread_mutex_unlock(&mutex);   
		if(request_granted)
		{
 
			for(int j = 0; j < resource_num; j++)
			{
				has[j] += req[j];
			}   
			complete = true;
			for(int k = 0; k < resource_num; k++)
			{
				if(has[k] != s->max[k])
				{
					complete = false;
				}
			}
		}
   }
	printf("Process %d completed.\n", s->thread_num);
   
	pthread_mutex_lock(&mutex);
	release_res(s->thread_num, has);
	pthread_mutex_unlock(&mutex);
	return (void *) NULL;
}

bool request_res(int n_customer, int request[])
{
  
		printf("Proc%d Req", n_customer);
		print_array("", request, resource_num, true);
   
		print_array("num available", num_available, resource_num, true);
		puts("");

		for(int i = 0; i < resource_num; i++)
		{
			if(request[i] > amount_needed[n_customer][i])
			{
				printf("ERROR: Thread number %d requested more resources than maximum\n", n_customer);
			}
		}

		for(int j = 0; j < resource_num; j++)
		{
			if(request[j] > num_available[j])
			{
				printf("Resources not num_available\n");
				puts("===================================");
				return false;
			}
		}


		for(int k = 0; k < resource_num; k++)
		{
			num_available[k] -= request[k];
			allocation[n_customer][k] += request[k];
			amount_needed[n_customer][k] -= request[k];
		}


		if(safe_checking(num_available,allocation,amount_needed))
		{
			printf("\nGRANTED: Proc%d now has", n_customer);
			print_array("", allocation[n_customer], resource_num, true);
			puts("===================================");
		
			return true;
		}
		else
		{
 
			for(int m = 0; m < resource_num; m++)
			{
				num_available[m] += request[m];
				allocation[n_customer][m] -= request[m];
				amount_needed[n_customer][m] += request[m];
			}

			printf("\nUnsafe state!\n");
			puts("===================================");

			return false;
		}
}

bool release_res(int n_customer, int release[])
{

	printf("Process %d Releasing all resources.\n", n_customer);
	puts("===================================");


	for(int i = 0; i < resource_num; i++)
	{
		num_available[i] += release[i];
		allocation[n_customer][i] = 0;
		maximum[n_customer][i] = 0;
	}

  return true;
}

bool safe_checking(int num_available[], int allocation[][resource_num], int amount_needed[][resource_num])
{
  
	int work[resource_num] = {0};
	int finish[number_of_customers] = {0};
	int local_allocation[number_of_customers][resource_num];
	int local_amount_needed[number_of_customers][resource_num];
	
	for(int e = 0; e < number_of_customers; e++)
	{
		for(int f = 0; f < resource_num; f++)
		{
			local_allocation[e][f] = allocation[e][f];
			local_amount_needed[e][f] = amount_needed[e][f];
		}
	}
  
	for(int i = 0; i < resource_num; i++)
	{
		work[i] = num_available[i];
	}


	print_matricies();
	
	printf("\n");
	print_array("Post Request", work, resource_num, true);
	printf("\n");
	
	printf("#   [ Avail ][Require][ Alloc ][    Fin    ]\n");

	while(1)
	{
		int n_start_finished_processes = 0;
		int n_end_finished_processes = 0;
	
			for(int n = 0; n < number_of_customers; n++)
			{
				if(finish[n])
				{
					n_start_finished_processes++;
				}
			}
			for(int j = 0; j < number_of_customers; j++)
			{
				if(finish[j] == 0)
				{

					bool can_finish = false;

					for(int k = 0; k < resource_num; k++)
					{
						if(work[k] < local_amount_needed[j][k])
						{
							break;
						}

						if(k == resource_num - 1)
						{
							can_finish = true;
						}
					}

					printf("%d", j);
					print_array("", work, resource_num, false);
					print_array("", local_amount_needed[j], resource_num, false);
					print_array("", local_allocation[j],resource_num, false);
					print_array("", finish, number_of_customers, false);

					if(can_finish)
					{
	
						printf(" | \u2713 Process %d can finish\n", j);

						for(int l = 0; l < resource_num; l++)
						{
							work[l] += local_allocation[j][l];
							local_allocation[j][l] = 0;
						}

						finish[j] = 1;
					}
					else
					{
						printf(" | \u2717 Process %d can't finish\n", j);
					}
				}
			}

			for(int m = 0; m < number_of_customers; m++)
			{
				if(finish[m])
				{
					n_end_finished_processes++;
				}
			}


			if(n_end_finished_processes == n_start_finished_processes)
			{
				return false;
			}
			
			if(n_end_finished_processes == number_of_customers)
			{
				return true;
			}

	}
}

void print_struct(customer_struct s)
{
	printf("Process Num: %d\n", s.thread_num);

	printf("Max Resources: [ ");

	for(int p = 0; p < resource_num; p++)
	{
		printf("%d ", s.max[p]);
	}

	printf("]\n");
}

void print_array(char* title, int a[], int length, bool newline)
{
	printf("%s : [ ", title);

	for(int i = 0; i < length; i++)
	{
		printf("%d ", a[i]);
	}

	printf("]");
	
	if(newline)
	{
		printf("\n");
	}
}

void print_matricies()
{
	printf("#   [Require]   [ Alloc ]   [  Max  ]\n");

	for(int i = 0; i < number_of_customers; i++)
	{
		printf("%d : [ ", i);

		for(int j = 0; j < resource_num; j++)
		{	
			printf("%d ", amount_needed[i][j]);
		}

		printf("] | [ ");

		for(int j = 0; j < resource_num; j++)
		{
			printf("%d ", allocation[i][j]);
		}

		printf("] | [ ");

		for(int j = 0; j < resource_num; j++)
		{	
			printf("%d ", maximum[i][j]);
		}

		puts("]");
	}
}
