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

    // Scatter-Reduce阶段：每个节点累加其他节点的数据块
    for (int step = 0; step < size - 1; step++) {
        int send_to = (rank + 1) % size;
        int recv_from = (rank - 1 + size) % size;

        float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
        MPI_Sendrecv(recv_buf, data_size_per_node, MPI_FLOAT, send_to, 0,
                     temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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

    // 分配一个缓冲区用于存储所有节点的数据
    float *gathered_data = (float *)malloc(num_nodes * data_size_per_node * sizeof(float));

    // Allgather阶段：每个节点将自己的数据块发送给所有节点
    MPI_Allgather(recv_buf, data_size_per_node, MPI_FLOAT, gathered_data, data_size_per_node, MPI_FLOAT, MPI_COMM_WORLD);

    // 打印每个节点收集到的完整数据
    printf("Rank %d gathered data: ", rank);
    for (int i = 0; i < num_nodes * data_size_per_node; i++) {
        printf("%f ", gathered_data[i]);
    }
    printf("\n");

    free(gathered_data);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int num_nodes, data_size_per_node;
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);  // 获取进程数量
    data_size_per_node = 5;  // 每个节点上的数据大小

    int total_data_size = num_nodes * data_size_per_node;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 生成并初始化数据
    
    float *data = NULL;
    
    data = (float *)malloc(total_data_size * sizeof(float));
    for (int i = 0; i < total_data_size; i++) {
        data[i] = rand() % 10;
    }
    printf("Initial data on rank %d: ", rank);
    for (int i = 0; i < total_data_size; i++) {
        printf("%f ", data[i]);
    }
    printf("\n");

    // 接收缓冲区，用于存储分发到每个节点的数据块
    float *recv_buf = (float *)malloc(data_size_per_node * sizeof(float));

    // 先阻塞
    MPI_Barrier(MPI_COMM_WORLD);

    // Scatter-Reduce阶段
    scatter_reduce(data, recv_buf, num_nodes, data_size_per_node);

    // Allgather阶段
    allgather(recv_buf, num_nodes, data_size_per_node);

    // 释放内存
    
        free(data);

    free(recv_buf);

    MPI_Finalize();
    return 0;
}