#include "convolution.hpp"

/*
 * ������������
 *  ������onboard.bin�ļ��ɣ�IP_testbench�ļ����ɣ���Ҫ��¼real_data_generate.py��ӡ��T_total���ݣ��滻layer�е�scaler
 * 	���ֿ�ϵ������������2*2*2*2
 *  ������˲�����stride=1��stride=2
 * 	������˳ߴ磺kernel=1��kernel=3
 *
 * */
#define RC_OUT_SIZE 32
int main()
{
	bool check_flag=true;
	//     		layer_id RC_block_num RC_block_size RC_real_size RC_out_size CHI_block_num CHI_block_size CHI_real_size CHO_block_num CHO_block_size CHO_real_size kernel stride   scaler	    is_yolo
	//���Ծ����Ϊ1,��Ҫ����scaler���� RC_OUT_SIZE 64
	//Layer_Type layer = {0,	 1,		  64,	    	64,			 64,		 1,			   32, 			  32,		   	1,			  64,		 	 64,		   1,	  1,3.6251296933187405e-06,0};

	//���Ծ���˲���Ϊ1,��Ҫ����scaler�� RC_OUT_SIZE 64
	Layer_Type layer = {0,	 1,		  66,	    	64,			 64,		 1,			   32, 			  32,		   	1,			  64,		 	 64,		   3,	  1,2.0781274088221835e-06,0};

	//���Ծ������Ϊ2����Ҫ����scaler�� RC_OUT_SIZE 32
	//Layer_Type layer = {0,	 1,		  66,	    	64,			 32,		 1,			   32, 			  32,		   	1,			  64,		 	 64,		   3,	  2,1.8876104377341107e-06,0};

	//yoloǰһ��������Ҫ����scaler�� RC_OUT_SIZE 64
	//Layer_Type layer = {0,	 1,		  64,	    	64,			 64,		 1,			   32, 			  32,		   	1,			  64,		 	 64,		   1,	  1,3.2641714824421797e-06,1};

	cout<< "layer_scaler��"<<setprecision(20)<<float(layer.scaler)<<endl;
	int ifm_len = layer.RC_block_num * layer.RC_block_num * layer.CHO_block_num * layer.CHI_block_num
			* (layer.RC_block_size) * (layer.RC_block_size) * layer.CHI_block_size  / 16;

	int weights_len = layer.RC_block_num * layer.RC_block_num * layer.CHO_block_num * layer.CHI_block_num
			* (layer.CHO_block_size + layer.kernel * layer.kernel * layer.CHO_real_size * layer.CHI_block_size) / 16;

	int ofm_len = layer.RC_block_num * layer.RC_block_num * layer.CHO_block_num
			* layer.RC_out_size * layer.RC_out_size * layer.CHO_block_size / 16 / 2;

	// int ifm_len = (66 * 66 * 16) / 16;
	// int weights_len = (32 * 4 + 3 * 3 * 16 * 32) / 16;
	// int ofm_len = 32 * 32 * 32 / 16 / 2;

	AXI_VAL_I input_0[ifm_len];
	AXI_VAL_I input_1[weights_len];

	AXI_VAL_O output_0[ofm_len];
	AXI_VAL_O output_1[ofm_len];

	FILE * FP_0 = fopen("E:/0_compacting_FPGA/compact/IP_testbech/test_ifm_int8_onboard.bin", "rb");
	fseek(FP_0, 0, SEEK_END);//���ļ�ָ��FP_0��λ�����õ����ļ�ĩβ0�ֽڴ���
	int len_0 = ftell(FP_0);
	cout <<"len_0:" <<len_0 << endl;
	dtype * pBuf_0 = new dtype[len_0];
	rewind(FP_0);
	fread(pBuf_0, 1, len_0, FP_0);
	fclose(FP_0);

	int index_0 = 0;
	for(int i = 0; i < ifm_len; i ++){
		for(int j = 0; j < 16; j ++){
			input_0[i].data[j] = pBuf_0[index_0];
			index_0 ++;
			//cout << int(pBuf_0[index_0]) << " ";
		}
	}

	FILE * FP_1 = fopen("E:/0_compacting_FPGA/compact/IP_testbech/test_bias_int32_w_int8_onboard.bin", "rb");
	fseek(FP_1, 0, SEEK_END);
	int len_1 = ftell(FP_1);
	cout <<"len_1:" <<len_1 << endl;
	dtype * pBuf_1 = new dtype[len_1];
	rewind(FP_1);
	fread(pBuf_1, 1, len_1, FP_1);
	fclose(FP_1);

	int index_1 = 0;
	for(int i = 0; i < weights_len; i ++){
		for(int j = 0; j < 16; j ++){
			input_1[i].data[j] = pBuf_1[index_1];
			index_1 ++;
		}
	}

	AXI_VAL_I *str_in_0 = &input_0[0];
	AXI_VAL_I *str_in_1 = &input_1[0];

	AXI_VAL_O *str_out_0 = &output_0[0];
	AXI_VAL_O *str_out_1 = &output_1[0];

	convolution(layer, str_in_0, str_in_1, str_out_0, str_out_1);

	FILE * FP_2 = fopen("E:/0_compacting_FPGA/compact/IP_testbech/test_ofw_int8_onboard.bin", "rb");
	fseek(FP_2, 0, SEEK_END);
	int len_2 = ftell(FP_2);
	cout <<"len_2:" << len_2 << endl;
	dtype * pBuf_2 = new dtype[len_2];
	rewind(FP_2);
	fread(pBuf_2, 1, len_2, FP_2);
	fclose(FP_2);

	AXI_VAL_O output[RC_OUT_SIZE * RC_OUT_SIZE * 2];
	int index_2 = 0;
	for(int i = 0; i < RC_OUT_SIZE * RC_OUT_SIZE *  2; i ++){
		for(int j = 0; j < 16; j ++){
			output[i].data[j] = pBuf_2[index_2];
			index_2 ++;
		}

	}
	cout<< "check" <<endl;
	for(int i = 0; i < RC_OUT_SIZE * RC_OUT_SIZE * 2  ; i ++){
		for(int j = 0; j < 16; j ++){
			int res = output_0[i].data[j] - output[i].data[j];
			if(res != 0)
			{
				check_flag = false;
				cout << "i:"<<i<<" j:"<<j<<" res:"<<(int)res << endl;
			}
		}
	}

	for(int i = 0; i < 1; i ++){
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
