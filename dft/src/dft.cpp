#include "dft.h"
#include "ap_int.h"   // 使用ap_int定义固定点数据类型

void dft(DTYPE real_sample[SIZE], DTYPE imag_sample[SIZE],    // 输入数组
         DTYPE real_op[SIZE], DTYPE imag_op[SIZE]) {          // 输出数组
 
  // 设置数组的AXI接口,定义burst写入深度
  #pragma HLS INTERFACE m_axi port=imag_op bundle=out depth=1024   //m_axi接口将数组连接到AXI总线,增大访问带宽
  #pragma HLS INTERFACE m_axi port=real_op bundle=out depth=1024
  #pragma HLS INTERFACE m_axi port=imag_sample bundle=in depth=1024
  #pragma HLS INTERFACE m_axi port=real_sample bundle=in depth=1024

  #pragma HLS INTERFACE s_axilite port=return bundle=ctrl  // 设置返回值AXI-Lite接口  用于启动执行

  #pragma HLS dataflow   // 开启dataflow  使用dataflow允许L2/L3和L4/L5不同任务间并行执行,提高并行度

  DTYPE c, s;
  DTYPE real_temp[SIZE];  // 临时数组
  DTYPE imag_temp[SIZE];

  L1: for (int i = 0; i < SIZE; i++) {    // 将临时数组初始化为0
    real_temp[i] = 0;
    imag_temp[i] = 0;
  }

  //计算实部
  L2: for (int j = 0; j < SIZE; j++) {  // 第一层循环,遍历时域样本

    #pragma HLS PIPELINE = 1          // 将L3层循环设为流水线  

  //数据流水线(Pipelining)对复杂的多层循环(L3和L5)进行流水线设置,使得循环体内的语句可以并行执行,提高总体吞吐量

    L3: for (int i = 0; i < SIZE; i++) {    // 第二层循环,遍历频域样本
      c = cos_coefficients_table[(ap_uint<10>)(i*j)];    
      s = sin_coefficients_table[(ap_uint<10>)(i*j)];   // 查表得到旋转因子 查表使用预先计算好的正弦和余弦查表提高计算速度

      real_temp[i] += real_sample[j] * c;   // 蝶形结构
      real_temp[i] -= imag_sample[j] * s;     
    }

  }

  L4: for (int j = 0; j < SIZE; j++) {   //同理计算虚部

    #pragma HLS PIPELINE = 1

    L5: for (int i = 0; i < SIZE; i++) {
      c = cos_coefficients_table[(ap_uint<10>)(i*j)];
      s = sin_coefficients_table[(ap_uint<10>)(i*j)];

      imag_temp[i] += real_sample[j] * s;
      imag_temp[i] += imag_sample[j] * c;
    }

  }

  L6: for (int i = 0; i < SIZE; i++) {   // 将计算结果写入输出数组
    real_op[i] = real_temp[i];
    imag_op[i] = imag_temp[i];
  }

}
