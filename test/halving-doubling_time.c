#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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

    float *all_data = NULL;
    double start_time, end_time;
    int total_data_size = read_data_from_file("../helper/random_floats.txt", &all_data);
    if (total_data_size < 0) {
        MPI_Finalize();
        return -1;
    }

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    

    //先计算r
    int r=num_nodes-largest_power_of_2(num_nodes);
    //
    int p=num_nodes;
    //再获取p-2*r
    int p_mins_2r=num_nodes-2*r;

    // 分配数据之前的时间
    start_time = MPI_Wtime();

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

    // scatter_reduce完成后的时间
    end_time = MPI_Wtime();

    // 计算目前节点的预测带宽
    double node_bandwidth = (total_data_size) * sizeof(float) / (end_time - start_time) / num_nodes; // 单位：字节/秒

    // 计算总体带宽
    double total_bandwidth;
    MPI_Reduce(&node_bandwidth, &total_bandwidth, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 输出带宽结果
    if (rank == 0) {
        printf("节点带宽: %f MB/s\n", node_bandwidth / (1024 * 1024));
        printf("总体带宽: %f MB/s\n", total_bandwidth / (1024 * 1024));
        printf("Ring-Allreduce阶段总耗时: %f 秒\n", end_time - start_time);
    }

    free(all_data);
    MPI_Finalize();
    return 0;
}