// // #include <mpi.h>
// // #include <stdio.h>
// // #include <stdlib.h>

// // void half_doubling_algorithm(int* data, int data_size) {
// //     int rank, size;
// //     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
// //     MPI_Comm_size(MPI_COMM_WORLD, &size);

// //     // Step 1: Reduce the number of processes to a power of two
// //     int p_prime = 1;
// //     while (p_prime * 2 <= size) p_prime *= 2;
// //     int r = size - p_prime;  // Number of processes to remove

// //     // Split the data into two halves
// //     int data_half_size = data_size / 2;
// //     int* first_half = (int*)malloc(data_half_size * sizeof(int));
// //     int* second_half = (int*)malloc(data_half_size * sizeof(int));

// //     // Copy data into two halves
// //     for (int i = 0; i < data_half_size; i++) {
// //         first_half[i] = data[i];
// //         second_half[i] = data[i + data_half_size];
// //     }

// //     // Process rank < 2r: exchange data between even and odd ranks
// //     if (rank < 2 * r) {
// //         MPI_Status status;
// //         if (rank % 2 == 0) {  // Even ranks send second half to odd ranks
// //             MPI_Send(second_half, data_half_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
// //             MPI_Recv(second_half, data_half_size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
// //         } else {  // Odd ranks send first half to even ranks
// //             MPI_Send(first_half, data_half_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
// //             MPI_Recv(first_half, data_half_size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
// //         }

// //         // Perform reduction on the received data
// //         for (int i = 0; i < data_half_size; i++) {
// //             if (rank % 2 == 0) {
// //                 first_half[i] += second_half[i];  // Reduction for even ranks
// //             } else {
// //                 second_half[i] += first_half[i];  // Reduction for odd ranks
// //             }
// //         }

// //         // Print the reduced data
// //         printf("Process %d reduced data: ", rank);
// //         for (int i = 0; i < data_half_size; i++) {
// //             printf("%d ", (rank % 2 == 0) ? first_half[i] : second_half[i]);
// //         }
// //         printf("\n");
// //     }

// //     free(first_half);
// //     free(second_half);
// // }

// // int main(int argc, char** argv) {
// //     MPI_Init(&argc, &argv);

// //     int rank;
// //     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

// //     // Create some dummy data for each process
// //     int data_size = 8;  // Example vector of size 8
// //     int* data = (int*)malloc(data_size * sizeof(int));

// //     // Initialize the data (different data for each process)
// //     srand(rank);
// //     for (int i = 0; i < data_size; i++) {
// //         data[i] = rand() % 100;
// //     }

// //     printf("Process %d initial data: ", rank);
// //     for (int i = 0; i < data_size; i++) {
// //         printf("%d ", data[i]);
// //     }
// //     printf("\n");

// //     // Run the half-doubling algorithm
// //     half_doubling_algorithm(data, data_size);

// //     free(data);
// //     MPI_Finalize();
// //     return 0;
// // }


// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>

// void allreduce_half_doubling_algorithm(int* data, int data_size) {
//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     // 使用临时数组来存储规约的中间结果
//     int* temp_data = (int*)malloc(data_size * sizeof(int));

//     // 复制初始数据
//     for (int i = 0; i < data_size; i++) {
//         temp_data[i] = data[i];
//     }

//     // 执行半加倍算法的allreduce
//     int p_prime = 1;
//     while (p_prime < size) {
//         int partner = rank ^ p_prime;  // 找到伙伴进程
//         if (partner < size) {
//             MPI_Status status;
//             int* recv_buffer = (int*)malloc(data_size * sizeof(int));

//             // 交换数据
//             MPI_Sendrecv(temp_data, data_size, MPI_INT, partner, 0,
//                          recv_buffer, data_size, MPI_INT, partner, 0,
//                          MPI_COMM_WORLD, &status);

//             // 执行规约操作（例如，求和）
//             for (int i = 0; i < data_size; i++) {
//                 temp_data[i] += recv_buffer[i];
//             }

//             free(recv_buffer);
//         }
//         p_prime *= 2;
//     }

//     // 规约完成后，每个进程的temp_data数组包含allreduce的结果
//     printf("Process %d final reduced data: ", rank);
//     for (int i = 0; i < data_size; i++) {
//         printf("%d ", temp_data[i]);
//     }
//     printf("\n");

//     free(temp_data);
// }

// int main(int argc, char** argv) {
//     MPI_Init(&argc, &argv);

//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     // 创建一些数据，假设每个进程都有相同大小的数据
//     int data_size = 8;  // 示例数据大小
//     int* data = (int*)malloc(data_size * sizeof(int));

//     // 初始化数据
//     srand(rank);
//     for (int i = 0; i < data_size; i++) {
//         data[i] = rand() % 100;
//     }

//     printf("Process %d initial data: ", rank);
//     for (int i = 0; i < data_size; i++) {
//         printf("%d ", data[i]);
//     }
//     printf("\n");

//     // 运行allreduce半加倍算法
//     allreduce_half_doubling_algorithm(data, data_size);

//     free(data);
//     MPI_Finalize();
//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// 实现 Halving-Doubling allreduce 算法
void halving_doubling_allreduce(int *sendbuf, int *recvbuf, int count, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int nearest_power_of_two = 1;
    while (nearest_power_of_two * 2 <= size) {
        nearest_power_of_two *= 2;
    }

    int extra_nodes = size - nearest_power_of_two;

    // Step 1: Handle extra nodes
    if (rank >= nearest_power_of_two) {
        // Extra nodes send data to the main part of the nodes
        MPI_Send(sendbuf, count, MPI_INT, rank - nearest_power_of_two, 0, comm);
    } else if (rank < extra_nodes) {
        // Main nodes receive data from extra nodes
        int temp[count];
        MPI_Recv(temp, count, MPI_INT, rank + nearest_power_of_two, 0, comm, MPI_STATUS_IGNORE);
        for (int i = 0; i < count; i++) {
            sendbuf[i] += temp[i];
        }
    }

    // Step 2: Perform halving-doubling on the nearest power of two part of nodes
    for (int step = 1; step < nearest_power_of_two; step *= 2) {
        int partner = rank ^ step;
        if (partner < nearest_power_of_two) {
            int temp[count];
            MPI_Sendrecv(sendbuf, count, MPI_INT, partner, 0, temp, count, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
            for (int i = 0; i < count; i++) {
                sendbuf[i] += temp[i];
            }
        }
    }

    // Step 3: Handle extra nodes (reduction results back to extra nodes)
    if (rank >= nearest_power_of_two) {
        // Main part sends final result to extra nodes
        MPI_Recv(recvbuf, count, MPI_INT, rank - nearest_power_of_two, 0, comm, MPI_STATUS_IGNORE);
    } else if (rank < extra_nodes) {
        // Extra nodes receive final result
        MPI_Send(sendbuf, count, MPI_INT, rank + nearest_power_of_two, 0, comm);
    }

    // Copy sendbuf to recvbuf
    for (int i = 0; i < count; i++) {
        recvbuf[i] = sendbuf[i];
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sendbuf[1] = {rank + 1}; // 每个进程的数据，简单初始化为rank+1
    int recvbuf[1];

    halving_doubling_allreduce(sendbuf, recvbuf, 1, MPI_COMM_WORLD);

    printf("Rank %d: result = %d\n", rank, recvbuf[0]);

    MPI_Finalize();
    return 0;
}
