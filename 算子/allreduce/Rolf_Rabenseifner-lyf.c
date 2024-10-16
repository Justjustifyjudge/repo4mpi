// Rolf Rabenseifner implementation of MPI_Allreduce
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int nearest_power_of_two(int x) {
    int res=1;
    for(;x>1;res<<=1,x>>=1);
    return res;
}
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int num_nodes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

    int total_data_size = 1000000;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    float *data=malloc(total_data_size*sizeof(float));
    //初始化需要计算的数据
    for(int i=0;i<total_data_size;i++){
        data[i]=rand()%20;
    }

    int used_rank=0;
    for(;used_rank<num_nodes;){
        int size=nearest_power_of_two(num_nodes-used_rank);
        //第一个rank
        rankfirst=used_rank;
        ranklast=used_rank+size-1;
        if(rank>=used_rank&&rank<=used_rank+size-1){
            //判断是否只有一个节点
                //只有一个节点，不参与reduce操作
            if(size==1){
                continue;
            } else{
                float* temp=malloc(size*sizeof(float));
                //判断是留下来的计算节点还是被合并的计算节点
                    //留下来的节点
                if((rank-used_rank)%2==0){
                    
                    MPI_Recv(temp,size,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                    for(int i=0;i<total_data_size;i++){
                        data[i]+=temp[i];
                    }
                }
                    //被合并的节点
                else{
                    
                    for(int i=0;i<total_data_size;i++){
                        temp[i]=data[i];
                    }
                    MPI_Send(temp,size,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD);
                }
                free(temp);
            }
        }
        used_rank+=size;
    }


    //结束和释放
    free(data);
    MPI_Finalize();
    return 0;
}