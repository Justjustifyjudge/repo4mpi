#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// 从文件读取数据并存储在数组中
int read_data_from_file(const char *filename, float **data) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("无法打开文件");
        return -1;
    }

    int count = 0;
    float temp;

    // 计算文件中浮点数的数量
    while (fscanf(file, "%f", &temp) == 1) {
        count++;
    }

    // 重新将文件指针移到文件开头
    fseek(file, 0, SEEK_SET);

    // 分配内存并读取数据
    *data = (float *)malloc(count * sizeof(float));
    for (int i = 0; i < count; i++) {
        fscanf(file, "%f", &(*data)[i]);
    }

    fclose(file);
    return count;  // 返回浮点数的总数
}

void scatter_reduce(float *data, float *recv_buf, float *send_to_buf, int num_nodes, int data_size_per_node, int total_data_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 从全局数据中分配数据区
    for (int i = 0; i < data_size_per_node; i++) {
        recv_buf[i] = (rank * data_size_per_node + i < num_nodes * data_size_per_node) ? data[rank * data_size_per_node + i] : 0;
    }

    for (int step = 0; step < num_nodes - 1; step++) {
        int send_to = (rank + 1 + step) % num_nodes;
        float *send_to_buf_singal = (float *)malloc(data_size_per_node * sizeof(float));
        for (int i = 0; i < data_size_per_node; i++) {
            send_to_buf_singal[i] = (((rank + step + 1) % num_nodes) * data_size_per_node + i < num_nodes * data_size_per_node)
                                      ? data[((rank + step + 1) % num_nodes) * data_size_per_node + i] : 0;
        }

        int recv_from = (rank - 1 + num_nodes - step) % num_nodes;
        float *temp_buf = (float *)malloc(data_size_per_node * sizeof(float));
        MPI_Status status;

        MPI_Send(send_to_buf_singal, data_size_per_node, MPI_FLOAT, send_to, 0, MPI_COMM_WORLD);
        MPI_Recv(temp_buf, data_size_per_node, MPI_FLOAT, recv_from, 0, MPI_COMM_WORLD, &status);

        for (int i = 0; i < data_size_per_node; i++) {
            recv_buf[i] += temp_buf[i];
        }

        free(temp_buf);
    }
}

void allgather(float *recv_buf, int num_nodes, int data_size_per_node) {
    float *gathered_data = (float *)malloc(num_nodes * data_size_per_node * sizeof(float));
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

    float *all_data = NULL;
    int total_data_size = read_data_from_file("../helper/random_floats.txt", &all_data);
    if (total_data_size < 0) {
        MPI_Finalize();
        return -1;
    }

    int data_per_node = (total_data_size + num_nodes - 1) / num_nodes;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float *recv_buf = (float *)malloc(data_per_node * sizeof(float));
    float *send_to_buf = (float *)malloc(total_data_size * sizeof(float));

    scatter_reduce(all_data, recv_buf, send_to_buf, num_nodes, data_per_node, total_data_size);
    allgather(recv_buf, num_nodes, data_per_node);

    float *allreduce_result = (float *)malloc(total_data_size * sizeof(float));
    MPI_Allreduce(all_data, allreduce_result, total_data_size, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    // printf("Rank %d Allreduce result: ", rank);
    // for (int i = 0; i < total_data_size; i++) {
    //     printf("%f ", allreduce_result[i]);
    // }
    // printf("\n");

    int is_correct = 1;
    for (int i = 0; i < data_per_node; i++) {
        if (recv_buf[i] != allreduce_result[i]) {
            is_correct = 0;
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