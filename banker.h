/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Damon Barton, Irfaan Ali, Alex Ly
  * All rights reserved.
 * 
 */
#ifndef BANKER_H_
#define BANKER_H_

#include <stdbool.h>

// Add any additional data structures or functions you want here
// perhaps make a clean bank structure instead of several arrays...

// Request resources, returns true if successful
extern bool request_res(int n_customer, int request[]);

// Release resources, returns true if successful
extern void release_res(int n_customer, int release[]);

extern bool check_safety();

#endif /* BANKER_H_ */