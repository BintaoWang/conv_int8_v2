#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

using namespace std;
using namespace hls;

#define IFM_MAX      66
#define OFM_MAX      64
#define KERNEL    3
#define TN  64
#define TM 32

#define BIT_WIDH  7
#define MUL_BIT_WIDTH 16

typedef signed char dtype;
typedef ap_int<MUL_BIT_WIDTH>  mul_type;
typedef ap_int<MUL_BIT_WIDTH+1>  acc_type0;
typedef ap_int<MUL_BIT_WIDTH+2>  acc_type1;
typedef ap_int<MUL_BIT_WIDTH+3>  acc_type2;
typedef ap_int<MUL_BIT_WIDTH+4>  acc_type3;
typedef ap_int<MUL_BIT_WIDTH+5>  acc_type4;
typedef ap_int<MUL_BIT_WIDTH+6>  acc_type5;

union s8_s32
{
	signed char union_s8[4];
	signed int union_s32;
};

struct AXI_VAL_I{
	dtype data[16];
};

struct AXI_VAL_O{
	dtype data[16];
	bool last;
};

struct BindingType{
	dtype data[16];
};

struct Layer_Type{
	int layer_id;

	int RC_block_num; // ָ��� �ֿ�ϵ��
	int RC_block_size; // ָ��ģ�͵�������,�����ά��
	int RC_real_size; // ָ��ģ����ʵ��ģ���ж�Ӧ�Ŀ��
	int RC_out_size; // ָ��ģ�͵������ߣ���Ҫ�ܾ��������Ӱ��

	int CHI_block_num;   // ʵ��ģ�Ͷ�Ӧ��ģ������ͨ���ķֿ�ϵ��
	int CHI_block_size; // ��ģ����ʵ��ģ���еĶ�Ӧ������ͨ����,��Ҫ��16�ı���
	int CHI_real_size;  // һ������µ���CHI_block_size

	int CHO_block_num;  // ʵ��ģ�Ͷ�Ӧ��ģ�����ͨ���ķֿ�ϵ��
	int CHO_block_size;  //  ��ģ����ʵ��ģ���еĶ�Ӧ�����ͨ����
	int CHO_real_size;  //   һ������µ���CHO_block_size

	int kernel;  // ����˵ĳߴ磬1����3
	int stride;  // ����˵Ĳ�����1����2
	float pscaler;  // ����������
	bool is_yolo;  // �Ƿ�������㣬�����Ļ�������relu

	float nscaler;  // ����������
	Layer_Type(int id,
			int rbn,int rbs,int rrs,int ros,
			int ibn,int ibs,int irs,
			int obn,int obs,int ors,
			int k,int s,float sc,bool i):
		 layer_id(id),

		 RC_block_num(rbn),
		 RC_block_size(rbs),
		 RC_real_size(rrs),
		 RC_out_size(ros),

		 CHI_block_num(ibn),
		 CHI_block_size(ibs),
		 CHI_real_size(irs),

		 CHO_block_num(obn),
		 CHO_block_size(obs),
		 CHO_real_size(ors),

		 kernel(k),
		 stride(s),
		 pscaler(sc),
		 is_yolo(i)
	{
		 nscaler=sc*0.1;
	};

};

void convolution(
		Layer_Type layer,

		AXI_VAL_I * str_in_0,  // HP0
		AXI_VAL_I * str_in_1,  // HP1

		AXI_VAL_O * str_out_0,  // HP2
		AXI_VAL_O * str_out_1);  // HP3
