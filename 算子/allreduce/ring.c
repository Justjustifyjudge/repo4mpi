// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>

// void scatter_reduce(float *data, float *recv_buf, int num_nodes, int data_size_per_node) {
//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     for (int i = 0; i < data_size_per_node; i++) {
//         recv_buf[i] = data[rank * data_size_per_node + i];
//     }

//     // Scatter-Reduce阶段：每个节点累加其他节点的数据块
//     for (int step = 0; step < size - 1; step++) {
//         int send_to = (rank + 1) % size;
//         int recv_from = (rank - 1 + size) % size;

//         float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
//         MPI_Sendrecv(recv_buf, data_size_per_node, MPI_FLOAT, send_to, 0,
//                      temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0,
//                      MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//         // 累加接收到的数据
//         for (int i = 0; i < data_size_per_node; i++) {
//             recv_buf[i] += temp_buf[i];
//         }
//         free(temp_buf);
//     }
// }

// void allgather(float *recv_buf, int num_nodes, int data_size_per_node) {
//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     // 分配一个缓冲区用于存储所有节点的数据
//     float *gathered_data = (float *)malloc(num_nodes * data_size_per_node * sizeof(float));

//     // Allgather阶段：每个节点将自己的数据块发送给所有节点
//     MPI_Allgather(recv_buf, data_size_per_node, MPI_FLOAT, gathered_data, data_size_per_node, MPI_FLOAT, MPI_COMM_WORLD);

//     // 打印每个节点收集到的完整数据
//     printf("Rank %d gathered data: ", rank);
//     for (int i = 0; i < num_nodes * data_size_per_node; i++) {
//         printf("%f ", gathered_data[i]);
//     }
//     printf("\n");

//     free(gathered_data);
// }

// int main(int argc, char **argv) {
//     MPI_Init(&argc, &argv);

//     // int num_nodes, data_size_per_node;
//     int num_nodes;
//     MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);  // 获取进程数量
//     // data_size_per_node = 5;  // 每个节点上的数据大小

//     int total_data_size = 17;
//     // Debug
//     // printf("total_data_size = %d\n", total_data_size);
//     int data_per_node = (total_data_size+num_nodes-1) / num_nodes;
//     // Debug
//     printf("data_per_node = %d\n", data_per_node);
//     int data_for_last_node;

//     if( total_data_size % num_nodes != 0)
//         data_for_last_node = total_data_size % num_nodes;
//     else
//         data_for_last_node = data_per_node;
    
//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     // 生成并初始化数据  
//     float* all_data = (float *)malloc(total_data_size * sizeof(float));

//     for (int i = 0; i < total_data_size; i++) {
//         all_data[i] = rand() % 10;
//     }

//     // Debug 打印每个rank的初始数据
//     // 
//     // printf("Initial data on rank %d: ", rank);
//     // if( rank != num_nodes - 1)
//     // for (int i = 0; i < total_data_size; i++) {
//     //     printf("%f ", all_data[i]);
//     // }
//     // printf("\n");

//     // 接收缓冲区，用于存储分发到每个节点的数据块
//     float *recv_buf = NULL;
//     if(rank != num_nodes - 1)
//         recv_buf = (float *)malloc(data_per_node * sizeof(float));
//     else
//         recv_buf = (float *)malloc(data_for_last_node * sizeof(float));

//     // 先阻塞
//     MPI_Barrier(MPI_COMM_WORLD);

//     // Scatter-Reduce阶段
//     if(rank != num_nodes - 1)
//         scatter_reduce(all_data, recv_buf, num_nodes, data_per_node);
//     else
//         scatter_reduce(all_data, recv_buf, num_nodes, data_for_last_node);

//     // Allgather阶段
//     if(rank != num_nodes - 1)
//         allgather(recv_buf, num_nodes, data_per_node);
//     else
//         allgather(recv_buf, num_nodes, data_for_last_node);

//     // 释放内存
    
//         free(all_data);

//     free(recv_buf);

//     MPI_Finalize();
//     return 0;
// }

//下述ring_allreduce代码因为在不均匀分块的时候，最后一个节点接收和发出的数据量与其他节点不同，会发生报错，请你使用MPI_Status来完成代码的重构。

// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>

// void scatter_reduce(float *data, float *recv_buf, int num_nodes, int data_size_per_node) {
//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     // 从主进程接收数据
//     for (int i = 0; i < data_size_per_node; i++) {
//         recv_buf[i] = data[rank * data_size_per_node + i];
//     }

//     // Scatter-Reduce阶段
//     for (int step = 0; step < size - 1; step++) {
//         int send_to = (rank + 1) % size;
//         int recv_from = (rank - 1 + size) % size;

//         float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
//         MPI_Status status;

//         // 发送和接收数据
//         MPI_Sendrecv(recv_buf, data_size_per_node, MPI_FLOAT, send_to, 0,
//                      temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0,
//                      MPI_COMM_WORLD, &status);

//         // 累加接收到的数据
//         for (int i = 0; i < data_size_per_node; i++) {
//             recv_buf[i] += temp_buf[i];
//         }
//         free(temp_buf);
//     }
// }

// void allgather(float *recv_buf, int num_nodes, int data_size_per_node) {
//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     float *gathered_data = (float *)malloc(num_nodes * data_size_per_node * sizeof(float));
//     MPI_Allgather(recv_buf, data_size_per_node, MPI_FLOAT, gathered_data, data_size_per_node, MPI_FLOAT, MPI_COMM_WORLD);

//     printf("Rank %d gathered data: ", rank);
//     for (int i = 0; i < num_nodes * data_size_per_node; i++) {
//         printf("%f ", gathered_data[i]);
//     }
//     printf("\n");

//     free(gathered_data);
// }

// int main(int argc, char **argv) {
//     MPI_Init(&argc, &argv);

//     int num_nodes;
//     MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

//     int total_data_size = 16;
//     int data_per_node = (total_data_size + num_nodes - 1) / num_nodes;
//     int data_for_last_node = total_data_size % num_nodes ? total_data_size % num_nodes : data_per_node;

//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     float *all_data = (float *)malloc(total_data_size * sizeof(float));
//     for (int i = 0; i < total_data_size; i++) {
//         all_data[i] = rand() % 10;
//     }

//     float *recv_buf = (float *)malloc((rank == num_nodes - 1 ? data_for_last_node : data_per_node) * sizeof(float));

//     MPI_Barrier(MPI_COMM_WORLD);

//     scatter_reduce(all_data, recv_buf, num_nodes, (rank == num_nodes - 1 ? data_for_last_node : data_per_node));
//     allgather(recv_buf, num_nodes, (rank == num_nodes - 1 ? data_for_last_node : data_per_node));

//     free(all_data);
//     free(recv_buf);
//     MPI_Finalize();
//     return 0;
// }


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void scatter_reduce(float *data, float *recv_buf, int num_nodes, int data_size_per_node) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < data_size_per_node; i++) {
        recv_buf[i] = data[rank * data_size_per_node + i];
    }

    // Scatter-Reduce阶段
    for (int step = 0; step < size - 1; step++) {
        int send_to = (rank + 1) % size;
        int recv_from = (rank - 1 + size) % size;

        float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
        MPI_Status status;

        // 发送和接收数据
        MPI_Send(recv_buf, data_size_per_node, MPI_FLOAT, send_to, 0, MPI_COMM_WORLD);
        MPI_Recv(temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0, MPI_COMM_WORLD, &status);

        // 累加接收到的数据
        for (int i = 0; i < data_size_per_node; i++) {
            recv_buf[i] += temp_buf[i];
        }
        free(temp_buf);
    }
}

void allgather(float *recv_buf, int num_nodes, int data_size_per_node) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *gathered_data = (float *)malloc(num_nodes * data_size_per_node * sizeof(float));
    MPI_Allgather(recv_buf, data_size_per_node, MPI_FLOAT, gathered_data, data_size_per_node, MPI_FLOAT, MPI_COMM_WORLD);

    printf("Rank %d gathered data: ", rank);
    for (int i = 0; i < num_nodes * data_size_per_node; i++) {
        printf("%f ", gathered_data[i]);
    }
    printf("\n");

    free(gathered_data);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int num_nodes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

    int total_data_size = 16;
    int data_per_node = (total_data_size + num_nodes - 1) / num_nodes;
    int data_for_last_node = total_data_size % num_nodes ? total_data_size % num_nodes : data_per_node;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *all_data = (float *)malloc(total_data_size * sizeof(float));
    for (int i = 0; i < total_data_size; i++) {
        all_data[i] = rand() % 10; // 随机生成数据
    }

    float *recv_buf = (float *)malloc((rank == num_nodes - 1 ? data_for_last_node : data_per_node) * sizeof(float));

    MPI_Barrier(MPI_COMM_WORLD);

    scatter_reduce(all_data, recv_buf, num_nodes, (rank == num_nodes - 1 ? data_for_last_node : data_per_node));
    allgather(recv_buf, num_nodes, (rank == num_nodes - 1 ? data_for_last_node : data_per_node));

    // 使用MPI_Allreduce进行结果验证
    float *allreduce_result = (float *)malloc((total_data_size) * sizeof(float));
    MPI_Allreduce(all_data, allreduce_result, (total_data_size), MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    // 打印验证结果并进行比较
    printf("Rank %d Allreduce result: ", rank);
    for (int i = 0; i < (total_data_size); i++) {
        printf("%f ", allreduce_result[i]);
    }
    printf("\n");

    // 比较scatter_reduce的结果和allreduce的结果
    int is_correct = 1; // 假设结果正确
    for (int i = 0; i < (rank == num_nodes - 1 ? data_for_last_node : data_per_node); i++) {
        if (recv_buf[i] != allreduce_result[i]) {
            is_correct = 0; // 发现不一致，结果不正确
            break;
        }
    }

    if (rank == 0) {
        if (is_correct) {
            printf("Results match.\n");
        } else {
            printf("Results do not match!\n");
        }
    }

    free(all_data);
    free(recv_buf);
    free(allreduce_result);
    MPI_Finalize();
    return 0;
}
