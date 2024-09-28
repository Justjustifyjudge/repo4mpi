#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
const int D = 3;
const int N = 3;
int main(int argc, char *argv[]) {
    int M = (int)pow(2, (int)log2(N));
    int rank, size;
    int a[D];
    // 初始化MPI环境
    MPI_Init(&argc, &argv);
    // 获取进程的rank和进程的总数
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(time(NULL));
    for (int i = 0; i < D; ++i) {
        a[i] = rand() % 10;
    }
    printf("init rank %d: %d %d %d\n", rank, a[0], a[1], a[2]);
    // MPI_Barrier(MPI_COMM_WORLD);
    if (rank >= M) {
        printf("rank %d send message to rank %d\n", rank, rank - M);
        MPI_Send(a, N, MPI_INT, rank - M, 0, MPI_COMM_WORLD);
    }
    if (rank < N - M) {
        int temp[D];
        printf("rank %d recv message from rank %d\n", rank, rank + M);
        MPI_Recv(temp, N, MPI_INT, rank + M, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < D; ++i) {
            a[i] += temp[i];
        }
    }
    // 所有进程再次同步
    if (rank < M) {
        int temp[D];
        // halving
        for (int k = 0; k < log2(M); ++k) {
            if ((rank + 1) % (int)pow(2, k) == 0 && (rank + 1) % (int)pow(2, k + 1) != 0) {
                MPI_Send(a, N, MPI_INT, rank + pow(2, k), 0, MPI_COMM_WORLD);
            }
            if ((rank + 1) % (int)pow(2, k + 1) == 0) {
                MPI_Recv(temp, N, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i = 0; i < D; ++i) {
                    a[i] += temp[i];
                }
            }
        }
        // doubling
        for (int k = log2(M) - 1; k >= 0; --k) {
            if ((rank + 1) % (int)pow(2, k + 1) == 0) {
                MPI_Send(a, N, MPI_INT, rank - pow(2, k), 0, MPI_COMM_WORLD);
            }
            if ((rank + 1) % (int)pow(2, k) == 0 && (rank + 1) % (int)pow(2, k + 1) != 0) {
                MPI_Recv(temp, N, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                memcpy(a, temp, sizeof(int) * D);
            }
        }
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    if (rank < N - M) {
        printf("rank %d send message to rank %d\n", rank, rank + M);
        MPI_Send(a, N, MPI_INT, rank + M, 0, MPI_COMM_WORLD);
    }
    if (rank >= M) {
        int temp[D];
        printf("rank %d recv message from rank %d\n", rank, rank - M);
        MPI_Recv(temp, N, MPI_INT, rank - M, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        memcpy(a, temp, sizeof(int) * D);
    }
    printf("rank %d: %d %d %d\n", rank, a[0], a[1], a[2]);
    // 清理MPI环境
    MPI_Finalize();
    return 0;
}