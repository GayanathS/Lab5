/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef BANKER_H_
#define BANKER_H_

#include <stdbool.h>

#define number_of_customers 5
#define resource_num 3

typedef struct
{
    int thread_num;
    int max[resource_num];
}customer_struct;

// Request resources, returns true if successful
extern bool request_res(int n_customer, int request[]);

// Release resources, returns true if successful
extern bool release_res(int n_customer, int release[]);

extern void *customer(void *arg);
extern bool safe_checking(int available[], int allocation[][resource_num], int need[][resource_num]);
extern void print_struct(customer_struct s);
extern void print_array(char* title, int a[], int length, bool newline);
extern void print_matricies();

#endif /* BANKER_H_ */