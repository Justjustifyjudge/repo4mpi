#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char** argv){
    MPI_Init(NULL,NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    if(world_size!=2) {
        fprintf(stderr,"Must run with exactly 2 processes\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    const int MAX_NUMBERS = 100;
    int numbers[MAX_NUMBERS];
    int number_amount;
    if(world_rank==0){
        srand(time(NULL));
        number_amount = (rand()/(float)RAND_MAX)*MAX_NUMBERS;
        MPI_Send(numbers, number_amount, MPI_INT,1, 0, MPI_COMM_WORLD);
        printf("Process 0 sent %d numbers to process 1\n", number_amount);
    }else if (world_rank==1)
    {
        MPI_Status status;
        printf("prereceive\n");
        MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &number_amount);
        printf("received %d numbers from process 0\nMessage source: %d\nMessage tag: %d\n", number_amount,
                status.MPI_SOURCE, status.MPI_TAG);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}

// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>

// int main(int argc, char** argv) {
//   MPI_Init(NULL, NULL);

//   int world_size;
//   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//   if (world_size != 2) {
//     fprintf(stderr, "Must use two processes for this example\n");
//     MPI_Abort(MPI_COMM_WORLD, 1);
//   }
//   int world_rank;
//   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

//   const int MAX_NUMBERS = 100;
//   int numbers[MAX_NUMBERS];
//   int number_amount;
//   if (world_rank == 0) {
//     // Pick a random amount of integers to send to process one
//     srand(time(NULL));
//     number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
//     // Send the amount of integers to process one
//     MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
//     printf("0 sent %d numbers to 1\n", number_amount);
//   } else if (world_rank == 1) {
    // MPI_Status status;
    // // Receive at most MAX_NUMBERS from process zero
    // MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    // // After receiving the message, check the status to determine how many
    // // numbers were actually received
    // MPI_Get_count(&status, MPI_INT, &number_amount);
    // // Print off the amount of numbers, and also print additional information
    // // in the status object
    // printf("1 received %d numbers from 0. Message source = %d, tag = %d\n",
    //        number_amount, status.MPI_SOURCE, status.MPI_TAG);
//   }
//   MPI_Barrier(MPI_COMM_WORLD);
//   MPI_Finalize();
// }