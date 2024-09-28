#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, num_nodes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

    // 创建并初始化长度为 17 的数组
    int length = 17;
    float *send_buf = (float *)malloc(length * sizeof(float));
    float *recv_buf = (float *)malloc(length * sizeof(float));

    for (int i = 0; i < length; i++) {
        send_buf[i] = rank + 1; // 每个进程初始化不同的值
    }

    // 使用 MPI_Allreduce 进行归约操作
    MPI_Allreduce(send_buf, recv_buf, length, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    // 打印结果
    printf("Rank %d: Result of Allreduce = ", rank);
    for (int i = 0; i < length; i++) {
        printf("%f ", recv_buf[i]);
    }
    printf("\n");

    // 释放内存
    free(send_buf);
    free(recv_buf);
    MPI_Finalize();
    return 0;
}
