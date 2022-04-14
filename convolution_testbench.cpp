#include "convolution.hpp"

/*
 * 测试用例类型
 *  ·测试onboard.bin文件由，IP_testbench文件生成，需要记录real_data_generate.py打印的T_total数据，替换layer中的scaler
 * 	·分块系数建议至少用2*2*2*2
 *  ·卷积核步长：stride=1和stride=2
 * 	·卷积核尺寸：kernel=1和kernel=3
 *
 * */
#define RC_OUT_SIZE 64
int id=0;
int r_bn = 4;
int r_bs = 66;
int r_rs = 64;
int r_os = RC_OUT_SIZE;
int i_bn = 1;
int i_bs = 64;
int i_rs = 64;
int o_bn = 1;
int o_bs = 32;
int o_rs = 32;
int kernel = 3;
int stride = 1;
float sc=0.003405190771446;
bool  ib = false;
int main()
{
	bool check_flag=true;
	//     		layer_id RC_block_num RC_block_size RC_real_size RC_out_size CHI_block_num CHI_block_size CHI_real_size CHO_block_num CHO_block_size CHO_real_size kernel stride   scaler	    is_yolo
	//测试卷积核为1,需要更改scaler，和 RC_OUT_SIZE 64
	//Layer_Type layer = {0,	 1,		  64,	    	64,			 64,		 1,			   64, 			  64,		   	1,			  64,		 	 64,		   1,	  1,3.6251296933187405e-06,0};

	//测试卷积核步长为1,需要更改scaler和 RC_OUT_SIZE 64
	Layer_Type layer = {id,	 r_bn,		  r_bs,	    	r_rs,			 r_os,		 i_bn,		i_bs, 			i_rs,		 o_bn,			o_bs,		 o_rs, kernel,stride,  sc,ib};

	//测试卷积步长为2，需要更改scaler和 RC_OUT_SIZE 32
	//Layer_Type layer = {0,	 1,		  66,	    	64,			 32,		 1,			   64, 			  64,		   	1,			  64,		 	 64,		   3,	  2,1.8876104377341107e-06,0};

	//yolo前一层卷积，需要更改scaler和 RC_OUT_SIZE 64
	//Layer_Type layer = {0,	 1,		  64,	    	64,			 64,		 1,			   64, 			  64,		   	1,			  64,		 	 64,		   1,	  1,3.2641714824421797e-06,1};

//	cout<< "layer_pscaler："<<setprecision(20)<<float(layer.pscaler)<< " layer_nscaler："<<setprecision(20)<<float(layer.nscaler)<<endl;
//	cout<<"CHI_block_size:"<<int(layer.CHI_block_size)<<endl;
////	cout <<"layer_len_i:" <<layer.len_i *16 <<" layer_len_w:" <<layer.len_w*16 <<" layer_len_o:" <<layer.len_o * 32<< endl;
//	int ifm_len = (int)layer.RC_block_num * (int)layer.RC_block_num * (int)layer.CHO_block_num * (int)layer.CHI_block_num
//			* (int)(layer.RC_block_size) * (int)(layer.RC_block_size) * (int)layer.CHI_block_size  / 16;
//
//	int weights_len = layer.RC_block_num * layer.RC_block_num * layer.CHO_block_num * layer.CHI_block_num
//			* (layer.CHO_block_size * 4 + layer.kernel * layer.kernel * layer.CHO_real_size * layer.CHI_block_size) / 16;
//
//	int ofm_len = layer.RC_block_num * layer.RC_block_num * layer.CHO_block_num
//			* layer.RC_out_size * layer.RC_out_size * layer.CHO_block_size / 16 / 2;
//	cout <<"ifm_len:" <<ifm_len*16 <<"Byte. weights_len:" <<weights_len*16 <<"Byte. ofm_len:" <<ofm_len*16*2<<"Byte."<< endl;
	int ifm_len = r_bn *r_bn* o_bn * i_bn* r_bs * r_bs * i_bs / 16;
	int weights_len = r_bn *r_bn* o_bn * i_bn* (o_bs * 4 + kernel * kernel * o_rs * i_rs) / 16;
	int ofm_len = r_bn *r_bn* o_bn * r_os * r_os * o_bs / 16 /2;
	cout <<"ifm_len:" <<ifm_len*16 <<"Byte. weights_len:" <<weights_len*16 <<"Byte. ofm_len:" <<ofm_len*16*2<<"Byte."<< endl;
	// int ifm_len = (66 * 66 * 16) / 16;
	// int weights_len = (32 * 4 + 3 * 3 * 16 * 32) / 16;
	// int ofm_len = 32 * 32 * 32 / 16 / 2;

	AXI_VAL_I input_0[ifm_len];
	AXI_VAL_I input_1[weights_len];

	AXI_VAL_O output_0[ofm_len];
	AXI_VAL_O output_1[ofm_len];

	FILE * FP_0 = fopen("E:/0_compacting_FPGA/tempv1/hls_v4/i_0.bin", "rb");
	fseek(FP_0, 0, SEEK_END);//将文件指针FP_0的位置设置到离文件末尾0字节处。
	int len_0 = ftell(FP_0);
	cout <<"len_i:" <<len_0 << endl;
	dtype * pBuf_0 = new dtype[len_0];
	rewind(FP_0);
	fread(pBuf_0, 1, len_0, FP_0);
	fclose(FP_0);

	int index_0 = 0;
	for(int i = 0; i < ifm_len; i ++){
		for(int j = 0; j < 16; j ++){
			input_0[i].data[j] = pBuf_0[index_0];
//			cout << int(pBuf_0[index_0]) << " ";
			index_0 ++;
		}
	}
	cout<<endl;
	FILE * FP_1 = fopen("E:/0_compacting_FPGA/tempv1/hls_v4/w_0.bin", "rb");
	fseek(FP_1, 0, SEEK_END);
	int len_1 = ftell(FP_1);
	cout <<"len_w:" <<len_1 << endl;
	dtype * pBuf_1 = new dtype[len_1];
	rewind(FP_1);
	fread(pBuf_1, 1, len_1, FP_1);
	fclose(FP_1);

	int index_1 = 0;
	for(int i = 0; i < weights_len; i ++){
		for(int j = 0; j < 16; j ++){
			input_1[i].data[j] = pBuf_1[index_1];
//			cout << int(pBuf_1[index_1]) << " ";
			index_1 ++;
		}
	}
	cout<<endl;
	AXI_VAL_I *str_in_0 = &input_0[0];
	AXI_VAL_I *str_in_1 = &input_1[0];

	AXI_VAL_O *str_out_0 = &output_0[0];
	AXI_VAL_O *str_out_1 = &output_1[0];

	convolution(layer, str_in_0, str_in_1, str_out_0, str_out_1);

	FILE * FP_2 = fopen("E:/0_compacting_FPGA/tempv1/hls_v4/o_0.bin", "rb");
	fseek(FP_2, 0, SEEK_END);
	int len_2 = ftell(FP_2);
	cout <<"len_o:" << len_2 << endl;
	dtype * pBuf_2 = new dtype[len_2];
	rewind(FP_2);
	fread(pBuf_2, 1, len_2, FP_2);
	fclose(FP_2);

	AXI_VAL_O output[r_bn*r_bn*o_bn*RC_OUT_SIZE * RC_OUT_SIZE];
	int index_2 = 0;
	for(int i = 0; i < r_bn*r_bn*o_bn*RC_OUT_SIZE * RC_OUT_SIZE; i ++){
		for(int j = 0; j < 16; j ++){
			output[i].data[j] = pBuf_2[index_2];
//			cout << int(pBuf_2[index_2]) << " ";
			index_2 ++;
		}
	}
	cout<<endl;
	cout<< "check" <<endl;
	for(int i = 0; i <r_bn*r_bn*o_bn*RC_OUT_SIZE * RC_OUT_SIZE; i ++){
		for(int j = 0; j < 16; j ++){
			int res = output_0[i].data[j] - output[i].data[j];
			if(res != 0)
			{
				check_flag = false;
				cout << "i:"<<i<<" j:"<<j<<" res:"<<(int)res <<" ouput_0:"<<(int)output_0[i].data[j]<<" output:"<<(int)output[i].data[j]<<endl;
			}
		}
	}

	for(int i = 31; i < 32; i ++){
		for(int j = 0; j < 16; j ++){
			cout << int(output_0[i].data[j]) << " ";
		}
		cout << endl;
	}

	if(check_flag == false)
		cout << "sorry,the result is wrong "<<endl;
	else
		cout << "Success!Congratulation "<<endl;

	return 0;
}
