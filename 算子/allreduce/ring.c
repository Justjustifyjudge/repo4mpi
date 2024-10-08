#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void scatter_reduce(float *data, float *recv_buf, float *send_to_buf, int num_nodes, int data_size_per_node, int total_data_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 从全局数据中分配数据区
    for (int i = 0; i < data_size_per_node; i++) {
        recv_buf[i] = (rank * data_size_per_node + i < num_nodes * data_size_per_node) ? data[rank * data_size_per_node + i] : 0;

        //Debug
        // printf("Rank %d recv_buf[%d] = %f\n", rank, i, recv_buf[i]);
    }
    // 从全局数据中分配发送数据区
    for (int i=0; i<total_data_size; i++){
        send_to_buf[i] = data[i];
    }
    // Scatter-Reduce阶段
    // for (int step = 0; step < num_nodes - 1; step++) {
    //     int send_to = (rank + 1) % num_nodes;
    //     int recv_from = (rank - 1 + num_nodes) % num_nodes;

    //     float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
    //     MPI_Status status;

    //     // 发送和接收数据
    //     MPI_Send(recv_buf, data_size_per_node, MPI_FLOAT, send_to, 0, MPI_COMM_WORLD);
    //     MPI_Recv(temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0, MPI_COMM_WORLD, &status);

    //     // 累加接收到的数据
    //     for (int i = 0; i < data_size_per_node; i++) {
    //         recv_buf[i] += temp_buf[i];
    //     }
    //     free(temp_buf);
    // }
    for (int step = 0; step < num_nodes - 1; step++) {
        //确定发送的节点
        int send_to = (rank + 1 + step) % num_nodes;
        //确定发送的数据
        float *send_to_buf_singal = (float *)malloc(data_size_per_node * sizeof(float));
        for(int i=0;i<data_size_per_node;i++){
            send_to_buf_singal[i] = (((rank+step+1)%num_nodes) * data_size_per_node + i < num_nodes * data_size_per_node) ? data[((rank+step+1)%num_nodes) * data_size_per_node + i] : 0;
        }

        int recv_from = (rank - 1 + num_nodes - step) % num_nodes;

        float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
        MPI_Status status;

        // 发送当前的接收缓冲区数据
        MPI_Send(send_to_buf_singal, data_size_per_node, MPI_FLOAT, send_to, 0, MPI_COMM_WORLD);
        // 接收来自上一个节点的数据
        MPI_Recv(temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0, MPI_COMM_WORLD, &status);

        // 累加接收到的数据
        for (int i = 0; i < data_size_per_node; i++) {
            recv_buf[i] += temp_buf[i];
        }

        //Debug
        for(int i=0;i<data_size_per_node;i++){
            printf("Rank %d recv_buf[%d] = %f\n", rank, i, recv_buf[i]);
        }

        free(temp_buf);
    }
}

void allgather(float *recv_buf, int num_nodes, int data_size_per_node, int total_data_size) {
    float *gathered_data = (float *)malloc(num_nodes * data_size_per_node * sizeof(float));
    // float *gathered_data = (float *)malloc(total_data_size * sizeof(float));
    MPI_Allgather(recv_buf, data_size_per_node, MPI_FLOAT, gathered_data, data_size_per_node, MPI_FLOAT, MPI_COMM_WORLD);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
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

    int total_data_size = 17;
    int data_per_node = (total_data_size + num_nodes - 1) / num_nodes; // 每个节点的数据大小

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    float *all_data = (float *)malloc(total_data_size * sizeof(float));
    for (int i = 0; i < total_data_size; i++) {
        all_data[i] = i; // 随机生成数据
        // printf("%d\n",rank);
    }

    float *recv_buf = (float *)malloc(data_per_node * sizeof(float));
    float *send_to_buf = (float *)malloc(total_data_size * sizeof(float));
    scatter_reduce(all_data, recv_buf, send_to_buf, num_nodes, data_per_node, total_data_size);
    allgather(recv_buf, num_nodes, data_per_node, total_data_size);

    // 使用MPI_Allreduce进行结果验证
    float *allreduce_result = (float *)malloc(total_data_size * sizeof(float));
    MPI_Allreduce(all_data, allreduce_result, total_data_size, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    // 打印验证结果并进行比较
    printf("Rank %d Allreduce result: ", rank);
    for (int i = 0; i < total_data_size; i++) {
        printf("%f ", allreduce_result[i]);
    }
    printf("\n");

    // 比较scatter_reduce的结果和allreduce的结果
    int is_correct = 1; // 假设结果正确
    for (int i = 0; i < data_per_node; i++) {
        if (recv_buf[i] != allreduce_result[i]) {
            is_correct = 0; // 发现不一致，结果不正确
            break;
        }
    }

    if (rank == 1) {
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
