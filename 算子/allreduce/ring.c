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


// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>

// void ring_allreduce(float *sendbuf, float *recvbuf, int count, MPI_Comm comm) {
//     int size, rank;
//     MPI_Comm_size(comm, &size);  // 获取总进程数
//     MPI_Comm_rank(comm, &rank);  // 获取当前进程号

//     // 初始化recvbuf
//     for (int i = 0; i < count; i++) {
//         recvbuf[i] = sendbuf[i];
//     }

//     int left = (rank - 1 + size) % size;   // 左手卡（环形相连）
//     int right = (rank + 1) % size;         // 右手卡（环形相连）

//     // Scatter-Reduce阶段
//     for (int i = 0; i < size - 1; i++) {
//         int send_offset = ((rank - i + size) % size) * count / size;
//         int recv_offset = ((rank - i - 1 + size) % size) * count / size;

//         MPI_Sendrecv_replace(recvbuf + send_offset, count / size, MPI_FLOAT, right, 0,
//                              recvbuf + recv_offset, count / size, MPI_FLOAT, left, 0,
//                              comm, MPI_STATUS_IGNORE);

//         // 在接收之后进行局部的归约操作（累加）
//         for (int j = 0; j < count / size; j++) {
//             recvbuf[recv_offset + j] += recvbuf[send_offset + j];
//         }
//     }

//     // All-Gather阶段
//     for (int i = 0; i < size - 1; i++) {
//         int send_offset = ((rank - i - 1 + size) % size) * count / size;
//         int recv_offset = ((rank - i - 2 + size) % size) * count / size;

//         MPI_Sendrecv(recvbuf + send_offset, count / size, MPI_FLOAT, right, 0,
//                      recvbuf + recv_offset, count / size, MPI_FLOAT, left, 0,
//                      comm, MPI_STATUS_IGNORE);
//     }
// }

// int main(int argc, char** argv) {
//     MPI_Init(&argc, &argv);

//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     int count = 8; // 假设我们有8个float数据需要进行allreduce
//     float *sendbuf = (float*) malloc(count * sizeof(float));
//     float *recvbuf = (float*) malloc(count * sizeof(float));

//     // 初始化sendbuf
//     for (int i = 0; i < count; i++) {
//         sendbuf[i] = rank + i;
//     }

//     // 调用ring allreduce
//     ring_allreduce(sendbuf, recvbuf, count, MPI_COMM_WORLD);

//     // 打印结果
//     printf("Rank %d: ", rank);
//     for (int i = 0; i < count; i++) {
//         printf("%f ", recvbuf[i]);
//     }
//     printf("\n");

//     free(sendbuf);
//     free(recvbuf);

//     MPI_Finalize();
//     return 0;
// }

// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>

// void ring_allreduce(float *sendbuf, float *recvbuf, int count, MPI_Comm comm) {
//     int rank, size;
//     MPI_Comm_rank(comm, &rank);
//     MPI_Comm_size(comm, &size);

//     // 创建一个临时缓冲区存储中间结果
//     float *tempbuf = (float *)malloc(count * sizeof(float));
    
//     // 初始化 recvbuf
//     for (int i = 0; i < count; i++) {
//         recvbuf[i] = sendbuf[i];
//     }

//     // Scatter-Reduce 阶段
//     for (int i = 0; i < size - 1; i++) {
//         int send_to = (rank + 1) % size;
//         int recv_from = (rank - 1 + size) % size;

//         MPI_Sendrecv(recvbuf, count, MPI_FLOAT, send_to, 0, 
//                      tempbuf, count, MPI_FLOAT, recv_from, 0, 
//                      comm, MPI_STATUS_IGNORE);

//         // 累加接收到的值
//         for (int j = 0; j < count; j++) {
//             recvbuf[j] += tempbuf[j];
//         }
//     }

//     // All-Gather 阶段
//     for (int i = 0; i < size - 1; i++) {
//         int send_to = (rank + 1) % size;
//         int recv_from = (rank - 1 + size) % size;

//         MPI_Sendrecv(recvbuf, count, MPI_FLOAT, send_to, 0, 
//                      tempbuf, count, MPI_FLOAT, recv_from, 0, 
//                      comm, MPI_STATUS_IGNORE);

//         // 更新 recvbuf 中的数据
//         for (int j = 0; j < count; j++) {
//             recvbuf[j] = tempbuf[j];
//         }
//     }

//     free(tempbuf);
// }

// int main(int argc, char **argv) {
//     MPI_Init(&argc, &argv);

//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     int count = 4;  // 每个进程发送和接收4个浮点数
//     float sendbuf[4], recvbuf[4];

//     // 初始化发送缓冲区
//     for (int i = 0; i < count; i++) {
//         sendbuf[i] = rank + 1.0;  // 每个进程填充不同的数据
//     }

//     // 调用 Ring Allreduce
//     ring_allreduce(sendbuf, recvbuf, count, MPI_COMM_WORLD);

//     // 打印每个进程的最终结果
//     printf("Process %d final result: ", rank);
//     for (int i = 0; i < count; i++) {
//         printf("%f ", recvbuf[i]);
//     }
//     printf("\n");

//     MPI_Finalize();
//     return 0;
// }






// #include <mpi.h>
// #include <stdio.h>
// #include <stdlib.h>
// // 9月5日15点50分
// // yf
// // void ring_allreduce(float *sendbuf, float *recvbuf, int count, MPI_Comm comm) {
// //     int rank, size;
// //     MPI_Comm_rank(comm, &rank);
// //     MPI_Comm_size(comm, &size);

// //     // 进程数多于buf
// //     if (size > count)
// //     {
// //         int* cyc=(int*)malloc(sizeof(int)*count);
// //         for (int i = 0; i < count; i++)
// //         {
// //             cyc[i] = 0;
// //         }
        
// //         if(rank>=count){
// //             //ToDo
// //         }
// //         //前count个进程需要计算对应的数据
// //         if(rank<count){
// //             int left=(rank-1+count)%count;
// //             int right=(rank+1)%count;
// //             for(int i=0;i<count;i++){
// //                 //要发送的数据
// //                 if(cyc[i]+i==rank+1){
// //                     int* send=sendbuf+i;
// //                     //Todo
// //                     // MPI_Send(&send, 1, MPI_Float, right)
// //                 }
// //             }
// //         }
// //     }
    
// // }

// void ring_allreduce(float *sendbuf, float *recvbuf, int count, MPI_Comm comm) {
//     int rank, size;
//     MPI_Comm_rank(comm, &rank);
//     MPI_Comm_size(comm, &size);
//     // 创建一个临时缓冲区存储中间结果
//     float *tempbuf = (float *)malloc(count * sizeof(float));
//     for(int i=0;i<count;i++){
//         tempbuf[i]=0;
//     }
//     // 进程数大于buf数
//     if (size > count) {
//         int* cyc = (int*)malloc(sizeof(int) * count);
//         for (int i = 0; i < count; i++) {
//             cyc[i] = 0;
//         }
        
//         if (rank >= count) {
//             // rank大于等于count的进程通过流水线接收数据
//             int src_rank = rank % count;
//             int right = (rank + 1) % size;
//             int left = (rank - 1 + size) % size;

//             //接收来自src_rank进程的数据
//             for (int step = 0; step < count; step++) {
//                 MPI_Recv(&recvbuf[step], 1, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);
//                 //继续传递给右侧的进程
//                 MPI_Send(&recvbuf[step-1], 1, MPI_FLOAT, right, 0, comm);
//             }
//         } else {
//             // 前count个进程进行正常的Scatter-Reduce和All-Gather
//             int left = (rank - 1 + count) % count;
//             int right = (rank + 1) % count;

//             for (int i = 0; i < count; i++) {
//                 if (cyc[i] + i == rank + 1) {
//                     // 要发送的数据
//                     float send = sendbuf[i];
//                     // 发送给右侧进程
//                     MPI_Send(&send, 1, MPI_FLOAT, right, 0, comm);

//                     // 从左侧进程接收数据
//                     MPI_Recv(&tempbuf[i], 1, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);
//                     recvbuf[i]+=tempbuf[i];
//                     sendbuf[i] = recvbuf[i];
//                     cyc[i]++;
//                 }
//             }

//             // 完成Scatter Reduce过程后，开始All-Gather
//             for (int step = 0; step < count - 1; step++) {
//                 float temp;
//                 MPI_Sendrecv(&recvbuf[step], 1, MPI_FLOAT, right, 0,
//                              &temp, 1, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);
//                 recvbuf[step] += temp;
//             }
//         }
//         free(cyc);
//     }
    
//     // 进程数少于buf数
//     else if (size < count) {
//         int left = (rank - 1 + size) % size;
//         int right = (rank + 1) % size;

//         // Scatter-Reduce阶段
//         for (int step = 0; step < size; step++) {
//             for (int i = 0; i < count; i++) {
//                 // 只处理属于该rank的buf，即buf % size == rank
//                 if (i % size == rank) {
//                     if (step == 0) {
//                         recvbuf[i] = sendbuf[i];  // 初始化recvbuf
//                     } else {
//                         float temp;
//                         // 从左侧进程接收
//                         MPI_Recv(&temp, 1, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);
//                         recvbuf[i] += temp;  // 累加结果
//                     }
//                 }
//             }
//             // 将负责的buf传递给右侧进程
//             for (int i = 0; i < count; i++) {
//                 if (i % size == rank) {
//                     MPI_Send(&recvbuf[i], 1, MPI_FLOAT, right, 0, comm);
//                 }
//             }
//         }

//         // All-Gather阶段
//         for (int step = 0; step < size - 1; step++) {
//             for (int i = 0; i < count; i++) {
//                 if (i % size == rank) {
//                     // 将recvbuf发送给右侧进程
//                     MPI_Send(&recvbuf[i], 1, MPI_FLOAT, right, 0, comm);
//                 }
//             }
//             for (int i = 0; i < count; i++) {
//                 if (i % size == rank) {
//                     float temp;
//                     // 接收左侧进程的数据
//                     MPI_Recv(&temp, 1, MPI_FLOAT, left, 0, comm, MPI_STATUS_IGNORE);
//                     recvbuf[i] = temp;  // 更新recvbuf
//                 }
//             }
//         }
//     }

    
// }

// int main(int argc, char **argv) {
//     MPI_Init(&argc, &argv);

//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     int count = 4;  // 每个进程发送和接收4个浮点数
//     float sendbuf[4], recvbuf[4];

//     // 初始化发送缓冲区
//     for (int i = 0; i < count; i++) {
//         sendbuf[i] = rank + 1.0;  // 每个进程填充不同的数据
//     }

//     // 调用 Ring Allreduce
//     ring_allreduce(sendbuf, recvbuf, count, MPI_COMM_WORLD);

//     // 打印每个进程的最终结果
//     printf("Process %d final result: ", rank);
//     for (int i = 0; i < count; i++) {
//         printf("%f ", recvbuf[i]);
//     }
//     printf("\n");

//     MPI_Finalize();
//     return 0;
// }


//////////////////////////////////////////
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