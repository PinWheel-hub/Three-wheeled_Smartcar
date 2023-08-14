#include "img_process.h"
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
图像未进行索引变换，即图像最上侧索引为0，最下侧为img_height-1
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
uint8 ips_flag=0;

int16 valid_row = img_height - 1;		//有效行索引
int16 longest_col=img_width/2;
int16 shortest_col=img_width/2;
int16 narrow_row;

uint8 image[MT9V03X_H][MT9V03X_W];
uint8 Threshold=Default_Threshold;
uint8 threshold[img_height];
int16 weight_array[img_height];

int16 delta=1;//每隔几列扫描边线
int16 EdgeL,EdgeR,lastEdgeL,lastEdgeR;//记录左右边界
int16 lostLeft_Sign,lostRight_Sign,leftLostLine_Cnt,rightLostLine_Cnt;//丢线标志

int8 stateRing_Sign=0,directRing_Sign=-1;//圆环相关标志位
int8 stateBranch_Sign=0,directBranch_Sign=2;//岔路相关标志位
int8 stateGarage_Sign=0,directGarage_Sign=Garage_Out;//车库相关标志位
int8 branch_num;
int8 stateRelay_Sign=0;//接力相关标志位
int distance1=6800;
bool find_car;
bool run_direction=0;

int16 repair_Cnt;
int16 d_neg,d_pos;
int16 track_Width[img_height];//记录扫描边界得到的赛道宽度
int16 min_Length;
track_Type_Enum track=Straight;

int16 Border[3][img_height];
int16 halftrack_Width[img_height];
uint8 islost_record[3][img_height];
int16 col_length[img_width];

int8 left_Delta_Exceed_Cnt;//圆环识别辅助位
int8 right_Delta_Exceed_Cnt;//圆环识别辅助位
int16 inflection_A,inflection_B,inflection_C;//圆环由近到远定义三个拐点
int16 inflection_G;//车库拐点

uint8 stop_flag=0;//停车标志
//const uint8 track_width[img_height] = { 0 ,0 ,0 ,2 ,4 ,6 ,9 ,13 ,19 ,25 ,
//	32 ,37 ,45 ,49 ,53 ,60 ,63 ,71 ,75 ,79 ,
//	87 ,91 ,97 ,101 ,106 ,112 ,116 ,120 ,126 ,130 ,
//	135 ,139 ,144 ,150 ,153 ,156 ,159 ,164 ,167 ,169 ,
//	171 ,173 ,175 ,178 ,179 ,181 ,183 ,183 ,187 ,187 };


//*******************************************************绘制标识*******************************************
void Show_Line()
{
	  for (int16 i = img_height - 1; i >= valid_row; i--)
    {
        image[i][Border[0][i]] = 0;
        image[i][Border[1][i]] = 0;
		  	image[i][Border[2][i]] = 0;
    }	
	  ips114_showint16(188,0,valid_row);
		ips114_showint16(188,1,distance1);
		ips114_showint16(188,2,stateRing_Sign);
		ips114_showint16(188,3,branch_num);
}
//*******************************************************绘制标识*******************************************

//判断边线范围内是否是赛道
uint8 is_Track(uint8 Row_Index)
{
   
}

//***************************************************边线预处理****************************************************
//平滑边线
void Smooth_Edge()
{
    for (int16 i = img_height - 1; i > valid_row + 2; i--)
    {
        //单点异常
        if (abs(Border[RIGHT][i] - Border[RIGHT][i - 2]) < 5
            && abs(Border[RIGHT][i] - Border[RIGHT][i - 1]) > 10)
        {
            Border[RIGHT][i - 1] = (Border[RIGHT][i] + Border[RIGHT][i - 2]) / 2;
        }
        if (abs(Border[LEFT][i] - Border[LEFT][i - 2]) < 5 && abs(Border[LEFT][i] - Border[LEFT][i - 1]) > 10)
        {
            Border[LEFT][i - 1] = (Border[LEFT][i] + Border[LEFT][i - 2]) / 2;
        }
        //双点异常
        if (abs(Border[RIGHT][i] - Border[RIGHT][i - 3]) < 5
            && abs(Border[RIGHT][i - 1] - Border[RIGHT][i - 2]) < 5
                && abs(Border[RIGHT][i] - Border[RIGHT][i - 1]) > 10)
        {
            Border[RIGHT][i - 1] = (Border[RIGHT][i] + Border[RIGHT][i - 3]) / 2;
            Border[RIGHT][i - 2] = (Border[RIGHT][i] + Border[RIGHT][i - 3]) / 2;
        }
        if (abs(Border[LEFT][i] - Border[LEFT][i - 3]) < 5
            && abs(Border[LEFT][i - 1] - Border[LEFT][i - 2]) < 5
                && abs(Border[LEFT][i] - Border[LEFT][i - 1]) > 10)
        {
            Border[LEFT][i - 1] = (Border[LEFT][i] + Border[LEFT][i - 3]) / 2;
            Border[LEFT][i - 2] = (Border[LEFT][i] + Border[LEFT][i - 3]) / 2;
        }
        //三点异常
        if (abs(Border[RIGHT][i] - Border[RIGHT][i - 4]) < 5
            && abs(Border[RIGHT][i - 1] - Border[RIGHT][i - 3]) < 5
                && abs(Border[RIGHT][i] - Border[RIGHT][i - 1]) > 10)
        {
            Border[RIGHT][i - 1] = (Border[RIGHT][i] + Border[RIGHT][i - 4]) / 2;
            Border[RIGHT][i - 2] = (Border[RIGHT][i] + Border[RIGHT][i - 4]) / 2;
            Border[RIGHT][i - 3] = (Border[RIGHT][i] + Border[RIGHT][i - 4]) / 2;
        }
        if (abs(Border[LEFT][i] - Border[LEFT][i - 4]) < 5
            && abs(Border[LEFT][i - 1] - Border[LEFT][i - 3]) < 5
                && abs(Border[LEFT][i] - Border[LEFT][i - 1]) > 10)
        {
            Border[LEFT][i - 1] = (Border[LEFT][i] + Border[LEFT][i - 4]) / 2;
            Border[LEFT][i - 2] = (Border[LEFT][i] + Border[LEFT][i - 4]) / 2;
            Border[LEFT][i - 3] = (Border[LEFT][i] + Border[LEFT][i - 4]) / 2;
        }
    }
}

//***************************************************边线预处理****************************************************
track_Type_Enum current_element;
void refresh_current_element()
{
	if(find_car)
	{
		if(relay_stop())
		{
			stop_flag=2;
		  stateRelay_Sign=1;
			send_message(5);
		}
		else
		{
			repair_car();
		}
	}
	else if(judge_Garage())
	{
		repair_Garage();
	} 
	else if(judge_Ring())
	{
		repair_Ring();
	}
	else if(judge_Branch())
	{
		repair_Branch();
	}
}
//*************************************************my****************************************************
void threshold_init()
{
	for (int16 i = img_height - 1; i >= 0; i--)
  {
		threshold[i]=Threshold-(i-img_height/2)*80/(img_height - 1);
	}
}
void weight_array_init()
{
	memset(col_length,0,img_height*sizeof(int16));
	for(int16 i=img_height-1;i>=img_height/2;i--)
  {
		weight_array[i]=img_height-(img_height*3/4-i)*(img_height*3/4-i)*16/img_height;
	}
}
void get_longest_col()
{
	int16 max_length=0;
	int16 i,j,flag=0;
	memset(col_length,0,img_width*sizeof(int16));
	for (j = img_width - 51; j >= 50; j-=1)
	{
		for (i = img_height - 1; i >= 2; i-=2)
		{ 
			if(mt9v03x_image[i][j]<=threshold[i])
			{
				col_length[j]=img_height-1-i;
				if(max_length < img_height-1-i)
		    {
			    max_length=img_height-1-i;
			    longest_col=j;
			    valid_row=i;
					flag=1;
		    }
				break;
			}
		}
		if(i<=2)
			{
				max_length=img_height-1;
			  longest_col=j;
			  valid_row=i;
			  flag=1;
				break;
			}
	}
	if(!flag)
	{
		longest_col=img_width/2;
		valid_row=img_height-1;
	}
}

void get_shortest_col()
{
	int16 min_len=img_height;
	int16 shortest_col=0;
	int16 i;
	for (i= 75; i <= img_width-76; i++)
	{
		if(col_length[i]<min_len)
		{
			min_len=col_length[i];
			shortest_col=i;
		}
	}
	min_Length=img_height-1-min_len;
}

void mid_point_filter()
{
	for (int16 i = img_height - 1; i >= valid_row; i--)
  {
		Border[MID][i]= (Border[LEFT][i]+Border[RIGHT][i])/2;
    if(i<img_height - 1)
		{			
		if (Border[MID][i] - Border[MID][i+1] > 5)
        {
            Border[MID][i] = Border[MID][i+1] + 2;
        }
    else if (Border[MID][i+1] - Border[MID][i] > 5)
        {
            Border[MID][i] = Border[MID][i+1] - 2;
        }
		}
	}
}

bool if_narrow_track()
{
	for(int16 i = img_height - 2; i >= valid_row; i--)
	{
		if(track_Width[i]<track_Width[i+1]-50&&track_Width[i]<20)
		{
			narrow_row=i;
			return 1;
		}
	}
	return 0;
}

void erzhihua()
{
//	uint8 thre=OTSU(mt9v03x_image[0]);
//	ips114_showint16(188,3,thre);
	for (int16 i = img_height - 1; i >= 0; i--)
  {
		for (int j = img_width - 1; j >= 0; j--)
  {
		if(mt9v03x_image[i][j]<=threshold[i]
//			&&(mt9v03x_image[i+2][j]<=threshold[i]
//		||mt9v03x_image[i-2][j]<=threshold[i]
//		||mt9v03x_image[i][j+2]<=threshold[i]
//		||mt9v03x_image[i][j-2]<=threshold[i])
		)
			image[i][j]=0;
		else
			image[i][j]=255;		
	}		
	}
}

uint8 find_Zebra()
{
	int16 a=0;
	int16 i,j,jump;
	for(i = narrow_row; i >= valid_row; i-=2)
	{
		jump=0;
		for(j = Border[RIGHT][118]; j>=Border[LEFT][118]; j--)
		{
			if(mt9v03x_image[i][j]>threshold[i]&&mt9v03x_image[i][j+1]<threshold[i])
			{
				jump++;
			}
		}
		if(jump>=6)
		{
			a++;
		}
		else
		{
			a=0;
		}
		if(a>=2)
		{
			return 1;
		}
	}
	return 0;
}
int16 count_Zebra()
{
	int16 a=0;
	int16 i,j,jump;
	for(i = img_height - 1; i >= valid_row; i-=1)
	{
		jump=0;
		for(j = Border[RIGHT][118]; j>=Border[LEFT][118]; j--)
		{
			if(mt9v03x_image[i][j]>threshold[i]&&mt9v03x_image[i][j+1]<threshold[i])
			{
				jump++;
			}
		}
		if(jump>=6)
		{
			a++;
		}
		else
		{
			a=0;
		}
		if(a>=2)
		{
			return i+1;
		}
	}
	return 0;
}

uint8 find_Zebra_y()
{
	int16 a=0;
	int16 i,j,jump;
	for(j = 50; j <= img_width-51; j+=1)
	{
		jump=0;
		for(i = img_height-2; i >= valid_row; i--)
		{
			if((mt9v03x_image[i][j]>threshold[i]&&mt9v03x_image[i+1][j]<threshold[i])
				||(mt9v03x_image[i][j]<threshold[i]&&mt9v03x_image[i+1][j]>threshold[i]))
			{
				jump++;
			}
		}
		if(jump>=12)
		{
			a++;
		}
		else
		{
			a=0;
		}
		if(a>=2)
		{
			return i;
		}
	}
	return 0;
}

int16 count_Zebra_y()
{
	int16 i,j,jump,start,flag=0;
	for(j = 50; j <= img_width-51; j+=2)
	{
		jump=0;
		flag=0;
		for(i = img_height-2; i >= valid_row+1; i--)
		{
			if(mt9v03x_image[i][j]>threshold[i]&&mt9v03x_image[i+1][j]<threshold[i])
			{
				if(!flag)
				start=i;
				flag=1;
				jump++;
			}
		}
		if(jump>=6)
		{
			return start;
		}
	}
		return 0;
}

uint8 find_Inflection_G(int16 start,int16 end)
{
	for(int16 i=start+1;i<=end-1;i++)
        {
            if((Border[RIGHT][i]-Border[RIGHT][i+1]<-5)
						&&(Border[RIGHT][i]-Border[RIGHT][i-1]>=0))
            {
                    inflection_G=i;
                    return 1;
                
            }
        }
	inflection_G=0;
	return 0;
}

void repair_Zebra()
{
	for(int16 i=img_height-2;i>=valid_row;i--)
	{
		if(track_Width[i]<track_Width[i+1]-10)
		{
		  Border[RIGHT][i]=Border[RIGHT][i+1]-1;
		  Border[LEFT][i]=Border[LEFT][i+1]+1;
		  track_Width[i]=track_Width[i+1]-2;
	  }
	}
}
void repair_car()
{
	if(track_Width[img_height-1]<40)
	{
		valid_row=img_height-1;
	}
	else
	{
	  for(int16 i=img_height-2;i>=valid_row;i--)
	{
		if(track_Width[i]<track_Width[i+1]-10)
		{
		  Border[RIGHT][i]=Border[RIGHT][i+1]-1;
		  Border[LEFT][i]=Border[LEFT][i+1]+1;
		  track_Width[i]=track_Width[i+1]-2;
	  }
	}
	}
}
uint8 relay_stop()
{
	int16 sum1=0,max_sum1=0;
//	int16 inflection_relay_l=0;
//  int16 inflection_relay_r=0;
//	if(Border[LEFT][119]>35||Border[RIGHT][119]<img_width-36)
//		return 0;
//	for(int16 i = 51; i <= img_width/2-1; i++)
//	{
//		if(col_length[i]>col_length[i-1]&&col_length[i]>col_length[i+1])
//		{
//			inflection_relay_l=i;
//			break;
//		}			
//	}
//	//ips114_showint16(188,3,inflection_relay_l);
//	if(!inflection_relay_l)
//		return 0;
//	for(int16 i = img_width-52; i >= img_width/2+1; i--)
//	{
//		if(col_length[i]>col_length[i+1]&&col_length[i]>col_length[i-1])
//		{
//			inflection_relay_r=i;
//			break;
//		}			
//	}
	//ips114_showint16(188,4,inflection_relay_r);
//	if(inflection_relay_r-inflection_relay_l<10)
//		return 0;
	for(int16 i = 50; i <= img_width-51; i++)
	{
		if(col_length[i]<25)
		{
			sum1++;
			if(sum1>max_sum1)
			{
				max_sum1=sum1;
				if(max_sum1>40)
		      return 1;
			}
		}
		else
		{
			sum1=0;
		}
	}
		return 0;
}
uint8 relay_start()
{
	if(receive_message()==5)
		return 1;
	else
		return 0;
}
void judge_relay()
{
	switch(stateRelay_Sign)
	{
		case 0:
			distance1=Ultrasonic_receive();
			if(distance1<1000&&distance1>0)
			{
				find_car=1;
				//gpio_set(BUZZER_PIN, GPIO_HIGH);
			}
			else
			{
			  find_car=0;
				//gpio_set(BUZZER_PIN, GPIO_LOW);
			}
			break;
		case 1:
			send_message(5);
			if(relay_start())
			{
				stateRelay_Sign=0;
				stop_flag=0;
			}
			break;
	}
}
void send_message(uint8 message)
{
		uart_putchar(UART_4,message);
}
uint8 receive_message()
{
	uint8 receive_data=0;
	uart_getchar(UART_4,&receive_data);
	return receive_data;
}
void if_stop()
{
  if (stateRelay_Sign==1||(directGarage_Sign==Garage_In&&stateGarage_Sign==3))
	{
		stop_flag=2;
	}
	else if (ips_flag||valid_row>115)
	{
		stop_flag=1;
	}
	else 
		stop_flag=0;
}
//*************************************************chen****************************************************
uint8 absolute(uint8 a,uint8 b)//绝对值函数
{
    if(a>b)
        return a-b;
    else
        return b-a;
}
uint8 OTSU(uint8 *pre_image)//动态计算阈值
{
    uint16 piexlCount[256]={0};
    uint8 threshold_Max=250,threshold_Min=120;
    uint8 ostu_threshold;
    for(int i=0;i<img_height;i++)//统计每个灰度有多少个像素
        for(int j=0;j<img_width;j++)
        {
            piexlCount[*(pre_image+i*img_width+j)]++;
        }
    int deltaMax=0,deltaTmp;//类间方差
    for(int i=threshold_Min;i<=threshold_Max;i++)//寻找使类间方差最大的阈值
    {
        int N0,N1,U0,U1,U0tmp,U1tmp;
        N0=N1=U0=U1=U0tmp=U1tmp=0;
        for(int j=0;j<256;j++)
        {
            if(j<=i)//较暗部分
            {
                N0=N0+piexlCount[j];
                U0tmp=U0tmp+j*piexlCount[j];
            }
            else//较亮部分
            {
                N1=N1+piexlCount[j];
                U1tmp=U1tmp+j*piexlCount[j];
            }
        }
        U0=U0tmp/N0;
        U1=U1tmp/N1;
        deltaTmp=N0*N1*(U0-U1)*(U0-U1);
        if(deltaTmp>deltaMax)
        {
            deltaMax=deltaTmp;
            ostu_threshold=i;
        }
    }

    return ostu_threshold;
}

uint8 trackBorder_Get_Cnt=0;
int16 last_start;
void trackBorder_Get()//预处理边界
{
    memset(Border[LEFT],0,img_height*sizeof(int));//数据复位
    memset(Border[MID],img_width/2,img_height*sizeof(int));
    memset(Border[RIGHT],img_width-1,img_height*sizeof(int));

    left_Delta_Exceed_Cnt=0;
	  right_Delta_Exceed_Cnt=0;
    leftLostLine_Cnt=0;//丢线数量初始化
    rightLostLine_Cnt=0;
    lostLeft_Sign=0;
    lostRight_Sign=0;

    min_Length=-1;
    for(int16 i=img_height-1;i>=valid_row;i--)//扫描左右边线并计算中线
    {
			  for(int j=longest_col-delta;j>=0;j=j-delta)//扫描左边界
        {
            if(mt9v03x_image[i][j]<threshold[i])
            {
                EdgeL=j;
                islost_record[LEFT][i]=0;
                break;
            }
            else if(j==0)//未找到跳变点
            {
                EdgeL=0;
                lostLeft_Sign=1;
                islost_record[LEFT][i]=1;
                leftLostLine_Cnt++;
                break;
            }
        }
        for(int j=longest_col+delta;j<img_width-1;j=j+delta)//扫描右边界
        {
            if(mt9v03x_image[i][j]<threshold[i])
            {
                EdgeR=j;
                islost_record[RIGHT][i]=0;
                break;
            }
            else if(j==img_width-2)//未找到跳变点
            {
                EdgeR=img_width-2;
                lostRight_Sign=1;
                islost_record[RIGHT][i]=1;
                rightLostLine_Cnt++;
                break;
            }
        }

        Border[LEFT][i]=EdgeL;//左边界
        Border[RIGHT][i]=EdgeR;//右边界
        Border[MID][i]=(EdgeL+EdgeR)>>1;//临时中线

        track_Width[i]=Border[RIGHT][i]-Border[LEFT][i];

        if(i<img_height-1)//为检测圆环做准备
        {
					if(Border[LEFT][i]-Border[LEFT][i+1]>20||Border[LEFT][i]-Border[LEFT][i+1]<-20)
						left_Delta_Exceed_Cnt++;
				  if(Border[RIGHT][i]-Border[RIGHT][i+1]>20||Border[RIGHT][i]-Border[RIGHT][i+1]<-20)
						right_Delta_Exceed_Cnt++;
        }
    }
}



uint8 is_Straight(int16 start_index,int16 end_index,uint8 side)//判断某一边从起点到终点是否为直线
{
        int16 white_cnt=0;
			  float k = (float)(Border[side][start_index] - Border[side][end_index]) / (start_index - end_index);
            for (int i = (start_index < end_index ? start_index : end_index);
                 i <= (start_index < end_index ? end_index : start_index); i++)
            {
                if (abs(Border[side][i] - (k * (i - start_index) + Border[side][start_index])) > 1.2)
                {
                    return 0;
                }
								if(Border[side][i]<=1||Border[side][i]>=img_width-2)
								{
									white_cnt++;
									if (white_cnt>=8)
										return 0;
								}
								
                if (i == (start_index < end_index ? end_index : start_index))
                {
                    return 1;
                }
            }
        return 0;
}

uint8 find_Inflection_C(int16 start,int16 end,int dir)//远处拐点
{
    if(dir==LEFT)
    {
        for(int16 i=end-1;i>=start+1;i--)
        {
            if(Border[LEFT][i]-Border[LEFT][i+1]>7)
            {
                    if((Border[LEFT][i]+Border[RIGHT][i])/2>80
										&&Border[LEFT][i]-Border[LEFT][i-1]>=-5
										&&track_Width[i]<95
										)
										{
							        inflection_C=i;
                      return 1;
										}
                
            }
        }
    }
    else
    {
        for(int16 i=end-1;i>=start+1;i--)
        {
            if(Border[RIGHT][i]-Border[RIGHT][i+1]<-7)
            {
                    if((Border[LEFT][i]+Border[RIGHT][i])/2<img_width-81
									  &&Border[RIGHT][i]-Border[RIGHT][i-1]<=5
										&&track_Width[i]<95
										)
										{
							        inflection_C=i;
                      return 1;
										}
            }
        }
    }
    inflection_C=0;
    return 0;
}

uint8 find_Inflection_B(int16 start,int16 end,int dir)//中间拐点
{
    if(dir==LEFT)
    {
        for(int16 i=end-1;i>=start+1;i--)
            if(Border[LEFT][i]-Border[LEFT][i-1]>0)
            {
              if(Border[LEFT][i]-Border[LEFT][i+1]>=0)
						  {
							  inflection_B=i;
                return 1;
						  }
            }
    }
    else
    {
        for(int16 i=end-1;i>=start+1;i--)
            if(Border[RIGHT][i]-Border[RIGHT][i-1]<0)
            {
              if(Border[RIGHT][i]-Border[RIGHT][i+1]<=0) 
							{
							  inflection_B=i;
                return 1;
							}
            }
    }
    inflection_B=0;
    return 0;
}

uint8 find_Inflection_A(int16 start,int16 end,int dir,int sign)//近处拐点
{
    if(dir==LEFT)
    {
        if(sign==0)//进环找A点
        {
            for(int16 i=end-1;i>=start+2;i--)
                if(Border[LEFT][i]-Border[LEFT][i-1]>15
								&&Border[LEFT][i]-Border[LEFT][i-2]>15)
								{
                  if(Border[LEFT][i]-Border[LEFT][i+1]>=0&&Border[LEFT][i]-Border[LEFT][i+1]<5||1)
									{
									  inflection_A=i;
                    return 1;
									}
                }
        }
        else if(sign==1)//出环找A点
        {
            for(int16 i=end-1;i>=start+2;i--)
                if(Border[RIGHT][i]<Border[RIGHT][i-1]-8&&Border[RIGHT][i]<Border[RIGHT][i-2]-15)
                {
									if(Border[RIGHT][i]<=Border[RIGHT][i+1])
									{
                    inflection_A=i;
                    return 1;
									}
                }
        }
    }
    else
    {
        if(sign==0)//进环找A点
        {
            for(int16 i=end-1;i>=start+2;i--)
                if(Border[RIGHT][i]-Border[RIGHT][i-1]<-15
								&&Border[RIGHT][i]-Border[RIGHT][i-2]<-15)
                {
                  if(Border[RIGHT][i]-Border[RIGHT][i+1]<=0&&Border[RIGHT][i]-Border[RIGHT][i+1]>-5||1) 
                  {										
									  inflection_A=i;
                    return 1;
									}
                }
        }
        else if(sign==1)//出环找A点
        {
            for(int16 i=end-1;i>=start+2;i--)
                if(Border[LEFT][i]>Border[LEFT][i-1]+8&&Border[LEFT][i]>Border[LEFT][i-2]+15)
                {
									if(Border[LEFT][i]>=Border[LEFT][i+1])
									{
                    inflection_A=i;
                    return 1;
									}
                }
        }
    }
    inflection_A=0;
    return 0;
}

int centre_line_get()//获取中线偏差值
{
    int sum_A=0,sum_B=0;
    int16 cnt=1;

	for(int16 i=img_height-1;i>=valid_row&&i>=img_height/2;i--)
    {
        Border[MID][i]=(Border[LEFT][i]+Border[RIGHT][i])>>1;
//			  if(stateGarage_Sign==2&&directGarage_Sign==Garage_In)
//		    {
//	      if(i<=inflection_G)		
//				{
//					Border[MID][i]=Border[MID][inflection_G+1]+8.5*(inflection_G+1-i);
//				}
//			  }
//			  else 
				if(stateRing_Sign==2)
		    {
	        if(i<=inflection_C)		
				  {
					  Border[MID][i]=Border[MID][inflection_C+1]+2.5*(inflection_C+1-i)*(directRing_Sign-1);
				  }
			  }
        sum_A=sum_A+weight_array[i]*(img_width/2-Border[MID][i]);
        sum_B=sum_B+weight_array[i];
    }
    return sum_A/sum_B;
}

uint8 judge_Ring()//识别圆环并判断所处状态
{
    switch(stateRing_Sign)
    {
    case 0://起始状态，检测是否遇到圆环
        if(right_Delta_Exceed_Cnt>=2&&left_Delta_Exceed_Cnt<=0)//可能是圆环
        {
            //ips114_showint16(188,3,11);
					  if(find_Inflection_A(valid_row+30, img_height-1, RIGHT,0))//是右环？
            {
							//ips114_showint16(188,4,22);
							if(is_Straight(valid_row+10, img_height-11, LEFT))//
							{
								//ips114_showint16(188,5,33);
								if(find_Inflection_B(valid_row+5, inflection_A-5, RIGHT))
							  {
									//ips114_showint16(188,6,44);
                  stateRing_Sign=1;
                  track=Ring;
                  directRing_Sign=RIGHT;
							    return 1;
								}
							}
            }
				}
				else if(left_Delta_Exceed_Cnt>=2&&right_Delta_Exceed_Cnt<=0)
				{
            if(find_Inflection_A(valid_row+30, img_height-1, LEFT,0))//是左环？
            {
							if(is_Straight(valid_row+10, img_height-11, RIGHT))//
							{
								if(find_Inflection_B(valid_row+5, inflection_A-5, LEFT))
								{
                  stateRing_Sign=1;
                  track=Ring;
                  directRing_Sign=LEFT;
							    return 1;
								}
							}
            }
        }
				//ips114_showint16(188,7,00);
        return 0;
    case 1://补第1根线状态，检测不到A拐点时进入下一阶段
        if(!find_Inflection_A(50, img_height-1, directRing_Sign,0)&&find_Inflection_C(valid_row, img_height-21, directRing_Sign))
            stateRing_Sign=2;
        return 1;
    case 2://补第2根线状态，检测不到C拐点时进入下一状态
        if(!find_Inflection_C(valid_row, img_height-1, directRing_Sign))
            stateRing_Sign=3;
        return 1;
    case 3://环内状态，检测到A拐点时代表该出环补线
        if(find_Inflection_A(valid_row, img_height-1, directRing_Sign,1))
            stateRing_Sign=4;
        return 1;
    case 4://补第3根线状态，能检测到C拐点代表已出环
        if((valid_row<45)&&find_Inflection_C(valid_row, img_height-1, directRing_Sign))
            stateRing_Sign=5;
        return 1;
    case 5://补第4根线状态，当左右两边最近行都不丢线代表离开圆环
        if(!find_Inflection_C(valid_row, img_height-1, directRing_Sign))
        {
                stateRing_Sign=0;
                track=Straight;
					      return 0;
        }
        return 1;
    }
    return 0;
}

void repair_Ring()//圆环补线
{
    switch(stateRing_Sign)
    {
    case 1://补第1根线
        if(directRing_Sign==LEFT)//左环补线
        {
            int16 i=inflection_A-1;
            repair_Cnt=1;
            int16 tmp=Border[LEFT][inflection_A]+1*repair_Cnt;
            while(i>=0&&tmp>Border[LEFT][i])
            {
                Border[LEFT][i]=tmp;
                repair_Cnt++;
                i--;
                tmp=Border[LEFT][inflection_A]+1.1*repair_Cnt;
            }
        }
        else //右环补线
        {
            int16 i=inflection_A-1;
            repair_Cnt=1;
            int16 tmp=Border[RIGHT][inflection_A]-1*repair_Cnt;
            while(i>=0&&tmp<Border[RIGHT][i])
            {
                Border[RIGHT][i]=tmp;
                repair_Cnt++;
                i--;
                tmp=Border[RIGHT][inflection_A]-1.1*repair_Cnt;
            }
        }
        break;
    case 2://补第2根线
        if(directRing_Sign==LEFT)//左环补线
        {
            int16 i=inflection_C+1;
            repair_Cnt=1;
            float k;
            k=3;//1.8
            while(i<img_height&&Border[LEFT][inflection_C]+k*repair_Cnt<Border[RIGHT][i])//补C拐点的线
            {
               int16 tmp=Border[LEFT][inflection_C]+k*repair_Cnt;//系数待定
               if(tmp>img_width-1)
                   tmp=img_width-1;
               Border[RIGHT][i]=tmp;
               repair_Cnt++;
               i++;
            }

            if(find_Inflection_B(inflection_C+2, img_height-6, LEFT))//能找到B拐点，则补B拐点的线
            {
               repair_Cnt=1;
               for(i=inflection_B+1;i<img_height;i++)
               {
                   int16 tmp=Border[LEFT][inflection_B]-1.1*repair_Cnt;//系数待定
                   if(tmp<0)
                       tmp=0;
                   Border[LEFT][i]=tmp;
                   repair_Cnt++;
               }
            }
        }
        else//右环补线
        {
            int16 i=inflection_C+1;
            repair_Cnt=1;
            float k;
            k=3;//2

            while(i<img_height&&Border[RIGHT][inflection_C]-k*repair_Cnt>Border[LEFT][i])//补C拐点的线
            {
               int16 tmp=Border[RIGHT][inflection_C]-k*repair_Cnt;//系数待定
               if(tmp<0)
                   tmp=0;
               Border[LEFT][i]=tmp;
               repair_Cnt++;
               i++;
            }

            if(find_Inflection_B(inflection_C+2, img_height-6, RIGHT))//能找到B拐点，则补B拐点的线
            {
               repair_Cnt=1;
               for(i=inflection_B+1;i<img_height;i++)
               {
                   int16 tmp=Border[RIGHT][inflection_B]+1.1*repair_Cnt;//系数待定
                   if(tmp>img_width-1)
                       tmp=img_width-1;
                   Border[RIGHT][i]=tmp;
                   repair_Cnt++;
               }
            }

        }
        break;
		case 4://补第3根线
			  float k;
		    if(directRing_Sign==LEFT)
        {		  
            int16 i=img_height-1;
					  int16 inflection=img_height-1;
            repair_Cnt=1;
					  k=1.5;
					  if(find_Inflection_A(valid_row, img_height-1, LEFT,1)==1)
            {
							 i=inflection_A-1;
							 inflection=inflection_A-1;
						}							
							 while(i>valid_row&&Border[RIGHT][inflection]-k*repair_Cnt>Border[LEFT][i])//补A拐点的线
            {
                   int16 tmp=Border[RIGHT][inflection]-k*repair_Cnt;//系数待定
                   if(tmp<0)
                   tmp=0;
                   Border[RIGHT][i]=tmp;
                   repair_Cnt++;
                   i--;
            }
						valid_row=i;
        }
        else
        {
            int16 i=img_height-1;
					  int16 inflection=img_height-1;
            repair_Cnt=1;
					  k=1.5;
					  if(find_Inflection_A(valid_row, img_height-1, RIGHT,1)==1)
            {
							  i=inflection_A-1;
							  inflection=inflection_A-1;
						}
							  while(i>valid_row&&Border[LEFT][inflection]+k*repair_Cnt<Border[RIGHT][i])//补A拐点的线
            {
                   int16 tmp=Border[LEFT][inflection]+k*repair_Cnt;//系数待定
                  if(tmp>img_width-1)
                       tmp=img_width-1;
                   Border[LEFT][i]=tmp;
                   repair_Cnt++;
                   i--;
            }
						valid_row=i;
				}
			  break;
    case 5://补第4根线
        if(directRing_Sign==LEFT)
        {
            if(find_Inflection_C(valid_row, img_height-1, LEFT)==1)
            {
                repair_Cnt=1;
                for(int i=inflection_C+1;i<img_height;i++)
                {
                    int16 tmp=Border[LEFT][inflection_C]-1.1*repair_Cnt;
                    if(tmp<0)
                        tmp=0;
                    Border[LEFT][i]=tmp;
                    repair_Cnt++;
                }
            }
        }
        else
        {
            if(find_Inflection_C(valid_row, img_height-1, RIGHT)==1)
            {
                repair_Cnt=1;
                for(int i=inflection_C+1;i<img_height;i++)
                {
                    int16 tmp=Border[RIGHT][inflection_C]+1.1*repair_Cnt;
                    if(tmp>img_width-1)
                        tmp=img_width-1;
                    Border[RIGHT][i]=tmp;
                    repair_Cnt++;
                }
            }
        }
        break;
    }
}

uint8 is_Branch()
{
	get_shortest_col();
	if(shortest_col>80&&shortest_col<img_width-81&&col_length[shortest_col]<65)
	{
		int16 i;
		bool track_flag=0;
    for(i=img_height-1;i>=min_Length+2;i--)
    {
			if(Border[RIGHT][i]-Border[RIGHT][i-1]<0			
      &&Border[LEFT][i]-Border[LEFT][i-1]>0
			&&Border[RIGHT][i]-Border[RIGHT][i-1]>-5			
      &&Border[LEFT][i]-Border[LEFT][i-1]<5)
			{
				track_flag=1;
				break;
			}
		}
		if(!track_flag)
			return 0;
		int16 track_row=i;
		int8 white_black=0,black_white=0;
		i=min_Length-2;
		for(int16 j=Border[LEFT][track_row];j<=Border[RIGHT][track_row]-1;j++)
		{
			if(mt9v03x_image[i][j]>threshold[i]&&mt9v03x_image[i][j+1]<threshold[i])
				white_black++;
			else if(mt9v03x_image[i][j]<threshold[i]&&mt9v03x_image[i][j+1]>threshold[i])
				black_white++;
			if(white_black&&black_white)
				return 1;
		}
	}
  return 0;
}


uint8 judge_Branch()
{
    switch(stateBranch_Sign)
    {
    case 0:
        if(is_Branch())
        {
            stateBranch_Sign=1;
					  return 1;
        }
        return 0;
    case 1:
			  get_shortest_col();
        if(shortest_col<80||shortest_col>img_width-81||col_length[shortest_col]>65)
				{
           branch_num++;
					 stateBranch_Sign=0;
           return 0;
				}
				return 1;
    }
		return 0;
}

void repair_Branch()//岔路补线
{
    switch(stateBranch_Sign)
    {
    case 1:
        if(directBranch_Sign==2)//左转进岔路
        {
            repair_Cnt=1;
            float k=2;
            for(int16 i=min_Length+1;i<=img_height-1;i++)
            {
                int16 tmp=shortest_col+k*repair_Cnt;
                if(tmp>img_width-1)
                    tmp=img_width-1;
                Border[RIGHT][i]=tmp;
                repair_Cnt++;
            }
        }
        else if(directBranch_Sign==1)//右转进岔路
        {
            repair_Cnt=1;
            float k=2;
            for(int16 i=min_Length+1;i<=img_height-1;i++)
            {
                int16 tmp=shortest_col-k*repair_Cnt;
                if(tmp<0)
                    tmp=0;
                Border[LEFT][i]=tmp;
                repair_Cnt++;
            }
        }
        break;
		}
}

uint8 judge_Garage()//识别车库
{
    if(directGarage_Sign==Garage_Out)//先写右出库
    {
        switch(stateGarage_Sign)
        {
        case 0:
            if(find_Zebra_y())
						{
              stateGarage_Sign=1;
							return 1;
						}
						else
						{
							directGarage_Sign=Garage_In;
						  return 0;
						}
        case 1:
            if(!find_Zebra_y())
						{
              stateGarage_Sign=2;
						}
            return 1;
				case 2:
            if(valid_row<65)
						{
              stateGarage_Sign=0;
							directGarage_Sign=Garage_In;
							return 0;
						}
            return 1;
        }
				return 0;
    }
    else if(directGarage_Sign==Garage_In) {
			switch(stateGarage_Sign)
        {
        case 0:
            if(if_narrow_track()&&find_Zebra())
						{
							stateGarage_Sign=1;
              return 1;
						}
            return 0;   
        case 1:
					  stateRing_Sign=0;
					  int16 a=count_Zebra();
            if(!a||a>68)
                stateGarage_Sign=2;
            return 1;
				case 2:
					  if(valid_row>80)
						{
							stateGarage_Sign=3;
						}						
						return 1;
				case 3:
					  stop_flag=2;				
						return 1;
        }
				return 0;
    }
}

void repair_Garage()//车库补线
{
    if(directGarage_Sign==Garage_Out)
    {
        switch(stateGarage_Sign)
        {
				case 1:
					repair_Zebra();
				  break;
        case 2:
					  repair_Zebra();
					  int16 i=img_height-1;
					  int16 inflection=img_height-1;
            repair_Cnt=1;
				    float k=6;
				    if(!run_direction)
						{					  
							  while(i>valid_row&&Border[LEFT][inflection]+k*repair_Cnt<Border[RIGHT][i])//补A拐点的线
            {
                   int16 tmp=k*repair_Cnt;//系数待定
                  if(tmp>img_width-1)
                       tmp=img_width-1;
                   Border[LEFT][i]=tmp;
                   repair_Cnt++;
                   i--;
            }
						valid_row=i;
					 }
						else
            {
							  while(i>valid_row&&Border[RIGHT][inflection]-k*repair_Cnt>Border[LEFT][i])//补A拐点的线
            {
                   int16 tmp=img_width-1-k*repair_Cnt;//系数待定
                  if(tmp<0)
                       tmp=0;
                   Border[RIGHT][i]=tmp;
                   repair_Cnt++;
                   i--;
            }
						valid_row=i;
						}
						break;
        }
    }
    else if(directGarage_Sign==Garage_In) {
			switch(stateGarage_Sign)
			{
				case 2://补线
					  repair_Zebra();
						int16 i=img_height-1;
					  int16 inflection=img_height-1;
            repair_Cnt=1;
					  float k=3;
				    if(!run_direction)
						{
						while(i>valid_row&&Border[LEFT][inflection]+k*repair_Cnt<Border[RIGHT][i])//补A拐点的线
            {
                   int16 tmp=k*repair_Cnt;//系数待定
                  if(tmp>img_width-1)
                       tmp=img_width-1;
                   Border[LEFT][i]=tmp;
                   repair_Cnt++;
                   i--;
            }
						valid_row=i;
					  }
						else
						{
							while(i>valid_row&&Border[RIGHT][inflection]-k*repair_Cnt>Border[LEFT][i])//补A拐点的线
            {
                   int16 tmp=img_width-1-k*repair_Cnt;//系数待定
                  if(tmp<0)
                       tmp=0;
                   Border[RIGHT][i]=tmp;
                   repair_Cnt++;
                   i--;
            }
						valid_row=i;
						}
            break;
			}
    }
}

float cal_Curvature()
{
    float a_x=img_height-1,a_y=Border[MID][img_height-1];
    float b_x=(img_height-1+valid_row)>>1,b_y=Border[MID][(img_height-1+valid_row)>>1];
    float c_x=valid_row,c_y=Border[MID][valid_row];

    float tmp,s,a,b,c;
    tmp=a_x*b_y+b_x*c_y+c_x*a_y-a_y*b_x-b_y*c_x-c_y*a_x;
    if(tmp<0)
        tmp=-tmp;
    s=tmp/2;
    a=sqrtf((b_x-c_x)*(b_x-c_x)+(b_y-c_y)*(b_y-c_y));
    b=sqrtf((a_x-c_x)*(a_x-c_x)+(a_y-c_y)*(a_y-c_y));
    c=sqrtf((a_x-b_x)*(a_x-b_x)+(a_y-b_y)*(a_y-b_y));
    return (a*b*c)/(4*s);
}

