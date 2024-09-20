#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void scatter_reduce(float *data, float *recv_buf, int total_data_size, int data_size_per_node) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Scatter-Reduce阶段：每个进程从数据中获得一部分，并进行加法
    for (int i = 0; i < data_size_per_node; i++) {
        recv_buf[i] = data[rank * data_size_per_node + i];
    }

    // 使用MPI_Reduce进行加法归约，结果保存在根进程
    float *result_buf = NULL;
    if (rank == 0) {
        result_buf = (float *)malloc(total_data_size * sizeof(float));
    }
    MPI_Reduce(recv_buf, result_buf, data_size_per_node, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    // 根进程打印结果
    if (rank == 0) {
        printf("Reduced result: ");
        for (int i = 0; i < total_data_size; i++) {
            printf("%f ", result_buf[i]);
        }
        printf("\n");
        free(result_buf);
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

    int total_data_size = 5;  // 总共只有5个数据
    int num_nodes, data_size_per_node;
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);  // 获取进程数量
    data_size_per_node = total_data_size / num_nodes;  // 每个进程处理的数据块

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 生成并初始化数据，只在根进程初始化
    float *data = NULL;
    if (rank == 0) {
        data = (float *)malloc(total_data_size * sizeof(float));
        for (int i = 0; i < total_data_size; i++) {
            data[i] = i + 1;  // 简单初始化为1到5
        }
    }

    // 接收缓冲区，用于存储分发到每个节点的数据块
    float *recv_buf = (float *)malloc(data_size_per_node * sizeof(float));

    // 先阻塞
    MPI_Barrier(MPI_COMM_WORLD);

    // Scatter-Reduce阶段
    scatter_reduce(data, recv_buf, total_data_size, data_size_per_node);

    allgather(recv_buf, num_nodes, data_size_per_node);

    // 释放内存
    if (rank == 0) {
        free(data);
    }
    free(recv_buf);

    MPI_Finalize();
    return 0;
}
