/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "life.h"
#include "load.h"
#include "save.h"
#include "util.h"

#define BOARD( __board, __i, __j )  (__board[LDA*(__i) + (__j)])

unsigned char * in_;
unsigned char * temp_;
unsigned num_threads;
int ncols_;
int nrows_;
pthread_mutex_t lock;

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
void *work(void* arg){

  int thread_number = *((int *)arg);
  int section = nrows_/num_threads;
  int start = thread_number*section;
  int end = (thread_number+1)*section;

  if(thread_number == num_threads-1)
    end = nrows_;


        for (int i = start; i < end; i++){

            if(i == start || i == start -1 || i == start + 1 || i == end -1 || i == end -2 ||i == end){
              // printf("%d\n", i);
              pthread_mutex_lock(&lock);
            }

            const int inorth = mod (i-1, nrows_);
            const int isouth = mod (i+1, nrows_);

            for (int j = 0; j < ncols_; j++){


              if (in_[i*nrows_+j] == 0){
                // printf("hello\n");
                continue;
              }


                const int jwest = mod (j-1, ncols_);
                const int jeast = mod (j+1, ncols_);


              int neighbor_count = in_[i*nrows_+j] >> 1;


              // if(neighbor_count >8)
              //   printf("%d iteration %d\n", neighbor_count);

              if (in_[i*nrows_+j] & 0x01){
                if(neighbor_count != 2 && neighbor_count != 3){

                  temp_[i*nrows_+j] &= ~0x01;
                  temp_[i*nrows_+jwest] -= 0x02;
                  temp_[i*nrows_+jeast] -= 0x02;
                  temp_[inorth*nrows_+jwest] -= 0x02;
                  temp_[inorth*nrows_+j] -= 0x02;
                  temp_[inorth*nrows_+jeast] -= 0x02;
                    temp_[isouth*nrows_+jwest] -= 0x02;
                    temp_[isouth*nrows_+j] -= 0x02;
                    temp_[isouth*nrows_+jeast] -= 0x02;
                }
              } else {
                if(neighbor_count == 3){
                    // printf("b %d\n", temp_[i*nrows+j] & 0x01);
                  temp_[i*nrows_+j] |= 0x01;
                  // printf("a %d\n", temp_[i*nrows+j] & 0x01);

//                   if(temp_[i*nrows_+jwest] >> 1 > 8 ||
//                     temp_[inorth*nrows_+j]  >> 1 > 8 ||
//                     temp_[inorth*nrows_+jeast] >> 1 > 8 ||
//                     temp_[i*nrows_+jwest] >> 1 > 8 ||
//                     temp_[i*nrows_+jeast] >> 1 > 8 ||
//                     temp_[isouth*nrows_+jwest] >> 1 > 8 ||
//                     temp_[isouth*nrows_+j] >> 1 > 8 ||
//                     temp_[isouth*nrows_+jeast] >> 1 > 8 )
//                      printf("%d, %d, %d, %d, %d, %d, %d, %d, %d iteration %d\n", 
//                       temp_[i*nrows_+jwest] >> 1,
//                       temp_[inorth*nrows_+j] >> 1,
// temp_[inorth*nrows_+jeast] >> 1,
// temp_[i*nrows_+jwest] >> 1,
// temp_[i*nrows_+jeast] >> 1,
// temp_[isouth*nrows_+jwest] >> 1,
// temp_[isouth*nrows_+j] >> 1,
// temp_[isouth*nrows_+jeast] >> 1,
// neighbor_count, i);

                  temp_[inorth*nrows_+jwest] += 0x02;
                  temp_[inorth*nrows_+j] += 0x02;
                  temp_[inorth*nrows_+jeast] += 0x02;
                    temp_[i*nrows_+jwest] += 0x02;
                    temp_[i*nrows_+jeast] += 0x02;
                    temp_[isouth*nrows_+jwest] += 0x02;
                    temp_[isouth*nrows_+j] += 0x02;
                    temp_[isouth*nrows_+jeast] += 0x02;


                }
              }

            }

            if(i == start || i == start -1 || i == start + 1 || i == end -1 || i == end -2 ||i == end){
              // printf("%d\n", i);
              pthread_mutex_unlock(&lock);
            }
        }

  return NULL;
}


char* parallel_sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    const int LDA = nrows;
    int curgen;

    in_ = malloc (nrows * ncols * sizeof (unsigned char));
    temp_ = malloc (nrows * ncols * sizeof (unsigned char));
    memset(in_, 0, nrows * ncols * sizeof (unsigned char));
    memset(in_, 0, nrows * ncols * sizeof (unsigned char));


  // Copy in to our array
      for (int i = 0; i < nrows; i++)
        {
            const int inorth = mod (i-1, nrows);
            const int isouth = mod (i+1, nrows);
            for (int j = 0; j < ncols; j++)
            {


                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

             //    printf("Hello %d,%d\n", i, j);
              // printf("%d,%d,%d,%d\n", inorth, isouth, jwest, jeast);


                if(BOARD (inboard, i, j)){
                  in_[i*nrows+j] |= 0x01;
                  in_[inorth*nrows+jwest] += 0x02;
                  in_[inorth*nrows+j] += 0x02;
                  in_[inorth*nrows+jeast] += 0x02;
                    in_[i*nrows+jwest] += 0x02;
                    in_[i*nrows+jeast] += 0x02;
                    in_[isouth*nrows+jwest] += 0x02;
                    in_[isouth*nrows+j] += 0x02;
                    in_[isouth*nrows+jeast] += 0x02;
                }
            }
        }

      // for (int i = 0; i < nrows; i++)
      //   {
      //       for (int j = 0; j < ncols; j++)
      //       {
      //         int neighbor_count = in_[i*nrows_+j] >> 1;


      //         if(neighbor_count >8 || neighbor_count<0)
      //           printf("%x\n", neighbor_count);

      //       }
      //   }

    // printf("alive %d\n", alive);

    ncols_ = ncols;
    nrows_ = nrows;

    for (curgen = 0; curgen < gens_max; curgen++){


      memcpy(temp_, in_, nrows * ncols * sizeof (unsigned char));

        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
      num_threads = 8;
      pthread_t thread[num_threads];
      pthread_mutex_init(&lock, NULL);

      for(int iter = 0; iter < num_threads; iter++){
        int *arg = (int *)malloc(sizeof(int*));
        *arg = iter;
        //printf("iter is %d\n", iter);
        pthread_create(&thread[iter], NULL, work, arg);
      }

      for(int iter = 0; iter < num_threads; iter++)
        pthread_join(thread[iter], NULL);

      memcpy(in_, temp_, nrows * ncols * sizeof (unsigned char));

        // printf("skipped %d, total %d\n", skipped, total);
    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */

     for (int i = 0; i < nrows; i++)
        {
            for (int j = 0; j < ncols; j++)
            {
                    BOARD (inboard, i, j) = in_[i*nrows+j] & 0x01;
            }
        }

    return inboard;
}

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
  return parallel_sequential_game_of_life(outboard, inboard, nrows, ncols, gens_max);
}
