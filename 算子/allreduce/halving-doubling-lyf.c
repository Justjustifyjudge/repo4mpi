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
//计算离k最近的二次幂
int largest_power_of_2(int k) {
    // 如果 k 是 0，返回 0（虽然 0 没有合法的2次幂）
    if (k == 0) {
        return 0;
    }
    k>>=1;
    int res=1;
    for(;k>0;k>>=1){
        res<<=1;
    }

    return res;
}

int ranktemp2rank(int rank_temp, int r){
    if(rank_temp<r){
        return rank_temp*2;
    }
    else{
        return rank_temp+r;
    }
}


void collect_two_nodes_step1(int total_data_size, float *buf, int rank) {
    //接收节点，如0、2等
    if(rank%2==0){
        float *temp=malloc(total_data_size*sizeof(float));
        MPI_Recv(temp,total_data_size,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for(int i=0;i<total_data_size;i++){
            buf[i]+=temp[i];
        }
        free(temp);
    }
    //发送节点，如1、3等
    else{
        float *temp=malloc(total_data_size*sizeof(float));
        for(int i=0;i<total_data_size;i++){
            temp[i]=buf[i];
        }
        MPI_Send(temp,total_data_size,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD);
        free(temp);
    }
}

void collect_two_nodes_step2(int total_data_size, float *buf, int rank) {
    //发送节点，如0、2等
    if(rank%2==0){
        float *temp=malloc(total_data_size*sizeof(float));
        for(int i=0;i<total_data_size;i++){
            temp[i]=buf[i];
        }
        MPI_Send(temp,total_data_size,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD);
        free(temp);
    }
    //接收节点，如1、3等
    else{
        float *temp=malloc(total_data_size*sizeof(float));
        MPI_Recv(temp,total_data_size,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for(int i=0;i<total_data_size;i++){
            buf[i]=temp[i];
        }
        free(temp);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int num_nodes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

    int total_data_size = 1000000;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    float *all_data = (float *)malloc(total_data_size * sizeof(float));
    float *sur_data = (float *)malloc(total_data_size * sizeof(float));

    for (int i = 0; i < total_data_size; i++) {
        all_data[i] = rand()%10; // 随机生成数据
        sur_data[i] = all_data[i]; // 复制数据
        // printf("%d\n",rank);
    }

    //先计算r
    int r=num_nodes-largest_power_of_2(num_nodes);
    //
    int p=num_nodes;
    //再获取p-2*r
    int p_mins_2r=num_nodes-2*r;

    // 1. 先把前2*r个节点halving
    if(rank<2*r){
        collect_two_nodes_step1(total_data_size,all_data,rank);
    }

    // 2. 再抛弃前2*r个节点中rank为奇数的节点，把前2*r个节点中rank为偶数的节点和后p-2*r个节点的节点重新计算rank值
    //// 新的rank值
    int rank_temp=-1;
    if(rank<2*r && rank%2==0){
        // 计算新的rank值
        rank_temp=rank/2;
    }
    else if(rank>=2*r && rank<num_nodes){
        // 前面2*r去掉了一半，新的rank值在原有基础上-r
        rank_temp=rank-r;
    }

    // 3. 把重新计算rank值的节点整合在一起完成halving-doubling_allreduce
    // 计算halving一共需要多少步
    int steps=p-r;
    if(rank_temp>=0){
        for(int i=1;i<steps;i<<=1){
            //确定是计算的节点还是跳过的节点
            if(rank_temp%i==0){
                //确定是留下来的节点还是抛弃的节点
                if(rank_temp%(i*2)==0){
                    // 留下来的节点
                    int partner_temp=rank_temp+i;
                    int partner=ranktemp2rank(partner_temp,r);
                    // 接收数据
                    float *temp=malloc(total_data_size*sizeof(float));
                    MPI_Recv(temp, total_data_size, MPI_FLOAT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    // 计算数据
                    for(int j=0;j<total_data_size;j++){
                        all_data[j]+=temp[j];
                    }
                    free(temp);
                }
                else{
                    // 抛弃的节点
                    int partner_temp=rank_temp-i;
                    int partner=ranktemp2rank(partner_temp,r);
                    //  发送数据
                    float *temp=malloc(total_data_size*sizeof(float));
                    for(int j=0;j<total_data_size;j++){
                        temp[j]=all_data[j];
                    }
                    MPI_Send(temp, total_data_size, MPI_FLOAT, partner, 0, MPI_COMM_WORLD);
                    free(temp);
                }
            }
        }
    }

    // 4. doubling_1阶段，所有节点的值都可以归结到rank=0的节点上
    // 所有参与第三步的节点都参与doubling_1阶段
    if(rank_temp>=0){
        // halving的逆向过程
        for(int i=steps>>1;i>=1;i>>=1){
            //确定是计算的节点还是跳过的节点
            if(rank_temp%i==0){
                //确定是分发的节点还是接收的节点
                if(rank_temp%(i*2)==0){
                    // 分发的节点
                    int partner_temp=rank_temp+i;
                    int partner=ranktemp2rank(partner_temp,r);
                    // 发送数据
                    float *temp=malloc(total_data_size*sizeof(float));
                    for(int j=0;j<total_data_size;j++){
                        temp[j]=all_data[j];
                    }
                    MPI_Send(temp, total_data_size, MPI_FLOAT, partner, 0, MPI_COMM_WORLD);
                    free(temp);
                }
                else{
                    // 接收的节点
                    int partner_temp=rank_temp-i;
                    int partner=ranktemp2rank(partner_temp,r);
                    // 接收数据
                    float *temp=malloc(total_data_size*sizeof(float));
                    MPI_Recv(temp, total_data_size, MPI_FLOAT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    // 同步数据
                    for(int j=0;j<total_data_size;j++){
                        all_data[j]=temp[j];
                    }
                }
            }
        }
    }

    // 5. 最后同步之前消去的r个节点
    if(rank<2*r){
        collect_two_nodes_step2(total_data_size,all_data,rank);
    }

    // 使用MPI_Allreduce进行结果验证
    float *allreduce_result = (float *)malloc(total_data_size * sizeof(float));
    MPI_Allreduce(sur_data, allreduce_result, total_data_size, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    // 打印验证结果并进行比较
    printf("Rank %d My_hb_allreduce result: ", rank);
    for(int i=0;i<total_data_size;i++){
        printf("%f ", all_data[i]);
    }
    printf("\n");

    printf("Rank %d Allreduce result: ", rank);
    for (int i = 0; i < total_data_size; i++) {
        printf("%f ", allreduce_result[i]);
    }
    printf("\n");

    // 比较scatter_reduce的结果和allreduce的结果
    int is_correct = 1; // 假设结果正确
    for (int i = 0; i < total_data_size; i++) {
        if (all_data[i] != allreduce_result[i]) {
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
    free(sur_data);
    free(allreduce_result);
    MPI_Finalize();
    return 0;
}
