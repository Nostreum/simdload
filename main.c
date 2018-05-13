#include <emmintrin.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

typedef float float_a __attribute__ ((aligned(4)));

void sum_loadu(float_a **m, int size){

  __m128 sum = _mm_set_ps1(0.0f);

  for(int i=0; i<size; i++){
    for(int j=0; j<size; j+=4){
      __m128 row = _mm_loadu_ps(&m[i][j]);
      sum = _mm_add_ps(sum, row);
    }
  }
}

void sum_load(float **m, int size){

  __m128 sum = _mm_set_ps1(0.0f);

  for(int i=0; i<size; i++){
    for(int j=0; j<size; j+=4){
      __m128 row = _mm_load_ps(&m[i][j]);
      sum = _mm_add_ps(sum, row);
    }
  }
}

uint64_t compute_time(struct timespec t1, struct timespec t2){

  return ( t2.tv_nsec - t1.tv_nsec ) + ( t2.tv_sec - t1.tv_sec ) * 1000000000L;

}

uint64_t start_test(void (*callback)(float_a **, int), float_a **m, int size, int nb_test){

  struct timespec t1, t2;

  clock_gettime(CLOCK_MONOTONIC, &t1);
  for(int i=0; i<nb_test; i++)
    callback(m, size);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  return compute_time(t1, t2) / nb_test;
}

int main(int argc, char *argv[]){

  uint64_t loadu_t, load_t;

  int size = 128;
  int nb_test = 1000;

  if(argc > 1)
    size = atoi(argv[1]);

  float_a **ma  = (float_a**)   malloc(size * sizeof(float_a*));
  float   **mua = (float**)     malloc(size * sizeof(float*));

  for(int i=0; i<size; i++){
    ma[i]  = (float_a*) malloc(size * sizeof(float_a));
    mua[i] = (float*)   malloc(size * sizeof(float));
    for(int j=0; j<size; j++){
      ma[i][j] = (i*j)%size;
      mua[i][j] = (i*j)%size;
    }
  }

  printf("Starting sum with loadu intresics functions... \n");
  loadu_t = start_test(sum_loadu, mua, size, nb_test);
  
  printf("Starting sum with load intresics functions... \n");
  load_t = start_test(sum_load, ma, size, nb_test);
  
  printf("LOADU : %llu ns\n", (long long unsigned int)loadu_t);
  printf("LOAD  : %llu ns\n", (long long unsigned int)load_t);

  return 0;
}
