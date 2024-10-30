#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_random_floats_to_file(size_t count, const char *filename) {
    FILE *file = fopen(filename, "w");  // 打开文件用于写入
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    
    for (size_t i = 0; i < count; i++) {
        fprintf(file, "%f ", (float)rand() / RAND_MAX);  // 将随机数写入文件
    }
    
    fclose(file);  // 关闭文件
    printf("随机数已成功存储到 %s 文件中\n", filename);
}

int main() {
    size_t size_in_bytes;
    printf("请输入需要的存储空间大小（MB）：");
    scanf("%zu", &size_in_bytes);

    size_t float_count = size_in_bytes / sizeof(float);  // 计算可以容纳的 float 数量

    srand((unsigned int)time(NULL));  // 设置随机数种子

    const char *filename = "random_floats.txt";
    generate_random_floats_to_file(float_count, filename);

    return 0;
}