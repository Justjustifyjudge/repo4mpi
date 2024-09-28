# xx.c编译命令
mpicc -o xxx xx.c -lm                                         
mpirun -np 3 allreduce
## 在root用户下运行
可以加上参数--allow-run-as-root
## 使用的电脑不支持过高的进程数
可以使用
```cmd
mpirun --oversubscribe -np 6 ./your_program
```
命令完成，“--oversubscribe”：允许MPI在超过物理核心数的情况下启动更多的进程。