
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "hash2.h"
#include <pthread.h>

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "Team",                  /* Team name */

    "Lam Fung Ng",                    /* Member full name */
    "1003435525",                 /* Member student number */
    "lam.ng@mail.utoronto.ca",                 /* Member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;

void *work(void* arg){
  int rnum;
  unsigned key;
  sample *s;
  int thread_number = *((int *)arg);

  int section = NUM_SEED_STREAMS/num_threads;
  int start = thread_number*section;
  int end = (thread_number+1)*section;

  //printf("section %d start %d end %d\n", section, start, end);
  // process streams starting with different initial numbers
  for (int i=start; i<end; i++){
    //printf("hello %d\n",i);
    rnum = i;

    // collect a number of samples
    for (int j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (int k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      h.ListLock(key); 
      // if this sample has not been counted before
      if (!(s = h.lookup(key))){
        // insert a new element for it into the hash table
        s = new sample(key);
        h.insert(s);
      }
      // increment the count for the sample
      s->count++;
      h.ListUnlock(key); 
    }
  }
  return NULL;
}

int main (int argc, char* argv[]){

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  pthread_t thread[num_threads];

  // initialize a 16K-ent+ ry (2**14) hash of empty lists
  h.setup(14);

  for(int iter = 0; iter < num_threads; iter++){
    int *arg = (int *)malloc(sizeof(int*));
    *arg = iter;
    //printf("iter is %d\n", iter);
    pthread_create(&thread[iter], NULL, work, arg);
  }

  for(int iter = 0; iter < num_threads; iter++)
    pthread_join(thread[iter], NULL);


  // print a list of the frequency of all samples
  h.print();
}
