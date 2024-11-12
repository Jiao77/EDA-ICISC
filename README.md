# EDA-ICISC
第六届中国研究生创“芯”·EDA 精英挑战赛 赛题四：多层图形匹配与设计单元级版图验证算法

- 目前完成的部分在rtree.cpp里，静态编译后的可执行文件是pm，可以按照下面的命令试一试能不能运行（txt文件要对应） 
```
./pm -layout ./testset/small/small_layout.txt -lib ./testset/small/small_pattern.txt
```
- 暂时还未实现xor的准确输出，所有关键曼哈顿图形的搜索，以及输出文件保存
## 可参考开源库：
1. 曼哈顿图形异或操作库：[polyops](https://github.com/Rouslan/polyops?tab=readme-ov-file)


 ## 可参考文献
### 23年广立微赛题相关
> [Edge Pair-Based Layout Pattern Matching using Space-filling Curve | IEEE Conference Publication | IEEE Xplore](https://ieeexplore.ieee.org/document/10617982)
- 一等奖团队论文
> [Pattern Match in VLSI Layout with Window Dance | IEEE Conference Publication | IEEE Xplore](https://ieeexplore.ieee.org/document/10617563)
- 二等奖团队论文 [专利](https://www.patentguru.com/cn/CN118135261A)
 
 ### 官方赛题文件提供的参考文献

> H. Yao, S. Sinha, C. Chiang, X. Hong and Y. Cai, ["Efficient Process-Hotspot Detection Using Range Pattern Matching,"](https://ieeexplore.ieee.org/document/4110242) 2006 IEEE/ACM International Conference on Computer Aided Design, San Jose, CA, USA, 2006, pp. 625-632, doi:10.1109/ICCAD.2006.320026.   
- 提取拓扑结构，可用于初步的模糊匹配排除不可能的情况，但细网格划分会导致数据量大

> J. W. Park, R. Todd and X. Song, ["Geometric Pattern Match Using Edge Driven Dissected Rectangles and Vector Space,"](https://ieeexplore.ieee.org/document/7422009) in IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems, vol. 35, no. 12, pp. 2046-2055, 2016, doi:10.1109/TCAD.2016.2535908.
- 将曼哈顿图像进一步划分为矩形，储存向量（角度+距离），象限位置，到总边界框的距离，宽度与高度等空间信息，可减少旋转与镜像迭代次数，

> Izumi Nitta, Yuzi Kanazawa, Tsutomu Ishida, and Koji Banno ["A fuzzy pattern matching method based on graph kernel for lithography hotspot detection"](https://doi.org/10.1117/12.2257654), Proc. SPIE 10148, Design-Process-Technology Co-optimization for Manufacturability XI,101480U (28 March 2017); https://doi.org/10.1117/12.2257654  


> Uwe Paul Schroeder, Janam Bakshi, Ahmed Mounir Elsemary, and Fadi Batarseh ["Dynamic pattern matching flow to enable low escape rate weak point detection"](https://doi.org/10.1117/12.2551739), Proc. SPIE 11328, Design-Process-Technology Co-optimization for Manufacturability XIV, 113280D (23 March 2020); https://doi.org/10.1117/12.2551739   


> Piyush Verma, Fadi Batarseh, Shikha Somani, Jingyu Wang, Sarah McGowan, and Sriram Madhavan ["Pattern-based pre-OPC operation to improve model-based OPC runtime"](https://doi.org/10.1117/12.2068998), Proc. SPIE 9235, Photomask Technology 2014, 923506 (8 October 2014); https://doi.org/10.1117/12.2068998    
### 其他
