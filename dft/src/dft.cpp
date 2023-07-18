#include "dft.h"
#include "ap_int.h"   // ʹ��ap_int����̶�����������

void dft(DTYPE real_sample[SIZE], DTYPE imag_sample[SIZE],    // ��������
         DTYPE real_op[SIZE], DTYPE imag_op[SIZE]) {          // �������
 
  // ���������AXI�ӿ�,����burstд�����
  #pragma HLS INTERFACE m_axi port=imag_op bundle=out depth=1024   //m_axi�ӿڽ��������ӵ�AXI����,������ʴ���
  #pragma HLS INTERFACE m_axi port=real_op bundle=out depth=1024
  #pragma HLS INTERFACE m_axi port=imag_sample bundle=in depth=1024
  #pragma HLS INTERFACE m_axi port=real_sample bundle=in depth=1024

  #pragma HLS INTERFACE s_axilite port=return bundle=ctrl  // ���÷���ֵAXI-Lite�ӿ�  ��������ִ��

  #pragma HLS dataflow   // ����dataflow  ʹ��dataflow����L2/L3��L4/L5��ͬ����䲢��ִ��,��߲��ж�

  DTYPE c, s;
  DTYPE real_temp[SIZE];  // ��ʱ����
  DTYPE imag_temp[SIZE];

  L1: for (int i = 0; i < SIZE; i++) {    // ����ʱ�����ʼ��Ϊ0
    real_temp[i] = 0;
    imag_temp[i] = 0;
  }

  //����ʵ��
  L2: for (int j = 0; j < SIZE; j++) {  // ��һ��ѭ��,����ʱ������

    #pragma HLS PIPELINE = 1          // ��L3��ѭ����Ϊ��ˮ��  

  //������ˮ��(Pipelining)�Ը��ӵĶ��ѭ��(L3��L5)������ˮ������,ʹ��ѭ�����ڵ������Բ���ִ��,�������������

    L3: for (int i = 0; i < SIZE; i++) {    // �ڶ���ѭ��,����Ƶ������
      c = cos_coefficients_table[(ap_uint<10>)(i*j)];    
      s = sin_coefficients_table[(ap_uint<10>)(i*j)];   // ���õ���ת���� ���ʹ��Ԥ�ȼ���õ����Һ����Ҳ����߼����ٶ�

      real_temp[i] += real_sample[j] * c;   // ���νṹ
      real_temp[i] -= imag_sample[j] * s;     
    }

  }

  L4: for (int j = 0; j < SIZE; j++) {   //ͬ������鲿

    #pragma HLS PIPELINE = 1

    L5: for (int i = 0; i < SIZE; i++) {
      c = cos_coefficients_table[(ap_uint<10>)(i*j)];
      s = sin_coefficients_table[(ap_uint<10>)(i*j)];

      imag_temp[i] += real_sample[j] * s;
      imag_temp[i] += imag_sample[j] * c;
    }

  }

  L6: for (int i = 0; i < SIZE; i++) {   // ��������д���������
    real_op[i] = real_temp[i];
    imag_op[i] = imag_temp[i];
  }

}
