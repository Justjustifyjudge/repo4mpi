# 导入必要的库
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# 定义数据结构
data = {
    "data packet size (MB)": [4, 8, 16, 32, 64, 128],
    "node bandwidth (Mbps)": [187.503325, 206.929719, 211.874928, 212.557425, 214.716727, 212.833784],
    "total bandwidth (Mbps)": [187.503325, 206.929719, 211.874928, 212.557425, 214.716727, 212.833784],
    "allreduce time cost (s)": [0.021333, 0.038660, 0.075516, 0.150548, 0.298067, 0.601408]
}

    # "data packet size (MB)": [4, 8, 16, 32, 64, 128, 256, 512],
    # "node bandwidth (Mbps)": [187.503325, 206.929719, 211.874928, 212.557425, 214.716727, 212.833784, 181.775901, 167.836248],
    # "total bandwidth (Mbps)": [187.503325, 206.929719, 211.874928, 212.557425, 214.716727, 212.833784, 181.775901, 167.836248],
    # "allreduce time cost (s)": [0.021333, 0.038660, 0.075516, 0.150548, 0.298067, 0.601408, 1.408327, 3.050592]

# 转换为数据框以便展示
df = pd.DataFrame(data)

# 显示数据表格
print("数据表格：\n", df)

# 对数据包大小取对数
log_packet_size = np.log2(df["data packet size (MB)"])

# 绘制节点带宽随数据包大小变化的折线图
plt.figure(figsize=(10, 6))
plt.plot(log_packet_size, df["node bandwidth (Mbps)"], marker='o', color='g', label="Node Bandwidth")

# 设置图表标签和标题
plt.xlabel("Data Packet Size (MB)")
plt.ylabel("Node Bandwidth (Mbps)")
plt.title("Variation of Node Bandwidth with Data Packet Size (Log2)")
plt.xticks(log_packet_size, labels=df["data packet size (MB)"])  # 显示原始数据包大小
plt.ylim(0, 250)  # 设置纵轴范围
plt.legend()
plt.grid(True)
plt.show()

# 绘制总体带宽随数据包大小变化的折线图
plt.figure(figsize=(10, 6))
plt.plot(log_packet_size, df["total bandwidth (Mbps)"], marker='o', color='r', label="Total Bandwidth")

# 设置图表标签和标题
plt.xlabel("Data Packet Size (MB)")
plt.ylabel("Total Bandwidth (Mbps)")
plt.title("Variation of Total Bandwidth with Data Packet Size (Log2)")
plt.xticks(log_packet_size, labels=df["data packet size (MB)"])  # 显示原始数据包大小
plt.ylim(0, 250)  # 设置纵轴范围
plt.legend()
plt.grid(True)
plt.show()

# 绘制Allreduce耗时随数据包大小变化的折线图
plt.figure(figsize=(10, 6))
plt.plot(log_packet_size, df["allreduce time cost (s)"], marker='o', color='b', label="Allreduce Time Cost")

# 设置图表标签和标题
plt.xlabel("Data Packet Size (MB)")
plt.ylabel("Allreduce Time Cost (s)")
plt.title("The Influence of Data Packet Size on Allreduce Time Cost (Log2)")
plt.xticks(log_packet_size, labels=df["data packet size (MB)"])  # 显示原始数据包大小
plt.legend()
plt.grid(True)
plt.show()

# # 绘制节点带宽随数据包大小变化的折线图
# plt.figure(figsize=(10, 6))
# plt.plot(log_packet_size, df["node bandwidth (Mbps)"], marker='o', color='g', label="Node Bandwidth")

# # 设置图表标签和标题
# plt.xlabel("Log of Data Packet Size (log2(MB))")
# plt.ylabel("Node Bandwidth (Mbps)")
# plt.title("Variation of Node Bandwidth with Logarithm of Data Packet Size")
# plt.legend()
# plt.grid(True)
# plt.show()

# # 绘制总体带宽随数据包大小变化的折线图
# plt.figure(figsize=(10, 6))
# plt.plot(log_packet_size, df["total bandwidth (Mbps)"], marker='o', color='r', label="Total Bandwidth")

# # 设置图表标签和标题
# plt.xlabel("Log of Data Packet Size (log2(MB))")
# plt.ylabel("Total Bandwidth (Mbps)")
# plt.title("Variation of Total Bandwidth with Logarithm of Data Packet Size")
# plt.legend()
# plt.grid(True)
# plt.show()

# # 绘制折线图
# plt.plot(log_packet_size, df["allreduce time cost (s)"], marker='o', color='b', label="Allreduce time cost")

# # 设置图表标签和标题
# plt.xlabel("data packet size (MB)")
# plt.ylabel("log2 Allreduce Time Cost (ms)")
# plt.title("The influence of data packet size on Allreduce time cost")
# plt.legend()
# plt.grid(True)
# plt.show()

# # 绘制折线图
# plt.figure(figsize=(10, 6))
# plt.plot(df["data packet size (MB)"], df["allreduce time cost (s)"], marker='o', color='b', label="Allreduce time cost")

# # 设置图表标签和标题
# plt.xlabel("data packet size (MB)")
# plt.ylabel("Allreduce Time Cost (ms)")
# plt.title("The influence of data packet size on Allreduce time cost")
# plt.legend()
# plt.grid(True)
# plt.show()

# # 绘制总体带宽随数据包大小变化的折线图
# plt.figure(figsize=(10, 6))
# plt.plot(df["data packet size (MB)"], df["total bandwidth (Mbps)"], marker='o', color='r', label="Total Bandwidth")

# # 设置图表标签和标题
# plt.xlabel("Data Packet Size (MB)")
# plt.ylabel("Total Bandwidth (Mbps)")
# plt.title("Variation of Total Bandwidth with Data Packet Size")
# plt.legend()
# plt.grid(True)
# plt.show()