// #include <mpi.h>
// #include <stdio.h>
// const int N=7;
// const int D=3;
// int main(int argc, char **argv) {
//     int rank, size;
//     MPI_Init(argc, argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     srand(time(NULL));
//     for (int i = 0; i < D; ++i) {
//         a[i] = rand() % 10;
//     }
//     printf("init rank %d: %d %d %d\n", rank, a[0], a[1], a[2]);
//     //
    
//     //
//     MPI_Finalize();
//     return 0;
// }


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void ring_allreduce(float *sendbuf, float *recvbuf, int count, MPI_Op op, MPI_Comm comm) {
    int size, rank;
    MPI_Comm_size(comm, &size);  // 获取进程总数
    MPI_Comm_rank(comm, &rank);  // 获取当前进程的rank

    // 初始化recvbuf为sendbuf的值
    for (int i = 0; i < count; i++) {
        recvbuf[i] = sendbuf[i];
    }

    int left = (rank - 1 + size) % size;  // 左邻居进程
    int right = (rank + 1) % size;        // 右邻居进程

    float *tempbuf = (float*)malloc(sizeof(float) * count);

    // 阶段1：归约阶段
    for (int i = 0; i < size - 1; i++) {
        // 将recvbuf中的数据发送给右邻居，同时接收左邻居的数据到tempbuf
        MPI_Sendrecv(recvbuf, count, MPI_FLOAT, right, 0, tempbuf, count, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);

        // 对recvbuf和tempbuf进行归约操作
        for (int j = 0; j < count; j++) {
            if (op == MPI_SUM) {
                recvbuf[j] += tempbuf[j];
            } else if (op == MPI_PROD) {
                recvbuf[j] *= tempbuf[j];
            } else if (op == MPI_MAX) {
                if (tempbuf[j] > recvbuf[j]) recvbuf[j] = tempbuf[j];
            } else if (op == MPI_MIN) {
                if (tempbuf[j] < recvbuf[j]) recvbuf[j] = tempbuf[j];
            }
        }
    }

    // 阶段2：广播阶段
    for (int i = 0; i < size - 1; i++) {
        // 将recvbuf中的数据发送给右邻居，同时接收左邻居的数据到tempbuf
        MPI_Sendrecv(recvbuf, count, MPI_FLOAT, right, 0, tempbuf, count, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);

        // 复制tempbuf到recvbuf
        for (int j = 0; j < count; j++) {
            recvbuf[j] = tempbuf[j];
        }
    }

    free(tempbuf);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    float sendbuf = rank + 1;  // 每个进程的发送数据
    float recvbuf;

    ring_allreduce(&sendbuf, &recvbuf, 1, MPI_SUM, MPI_COMM_WORLD);

    printf("Rank %d: result = %f\n", rank, recvbuf);

    MPI_Finalize();
    return 0;
}
