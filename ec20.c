#include "uart.h"
#include "ec20.h"
 
int smd_fd = -1;
char GPS_Raw_Data[100]="";
int result = 0;
int Module_Test(void)
{
	printf("< Quectel OpenLinux: AT example >\n");
	
	Ql_SendAT("ATE0", "OK", 1000);
	Ql_SendAT("AT", "OK", 1000);
	Ql_SendAT("ATI", "OK", 1000);
	Ql_SendAT("AT+CFUN?", "OK", 1000);
	Ql_SendAT("AT+CPIN?", "OK", 1000);
	Ql_SendAT("AT+CCID", "OK", 1000);
	Ql_SendAT("AT+CIMI", "OK", 1000);
	Ql_SendAT("AT+QABC", "OK", 1000);   // send an inexistent AT command
	Ql_SendAT("AT+CSQ",  "OK", 1000);
	Ql_SendAT("AT+CREG?", "OK", 1000);
	Ql_SendAT("AT+COPS?", "OK", 1000);

    printf("< Quectel OpenLinux: AT example end >\n\n");
	return 0;
}

// int Upload_Image(char *image_path)
// {
// 	FILE               *from_file;	//need send to server
// 	char               buf[MAXLINE]; 
// 	char 		       receive_buf[100];
// 	int                read_len; 
//  	char		       str[10];
// 	Ql_SendAT2("ATE0", "OK", 1000, 0);
// 	Ql_SendAT2("AT+COPS?", "OK", 1000, 0);									//检查网络
 
// 	if((from_file=open(image_path,O_RDWR))==NULL)							//打开要读取的文件
// 	{
// 		printf("Can't open the file \n");
// 		exit(1);
// 	}
 
// 	Ql_SendAT2("AT+QIOPEN=1,2,\"UDP SERVICE\",\"127.0.0.1\",0,3030,0", "OK", 1000, 0);	//建立UDP服务
// 	Ql_SendAT2("AT+QISTATE=0,1", "OK", 1000, 0);										
 
// 	while ( (read_len = read(from_file, buf, MAXLINE)) > 0 )							//读取文件知道读完
// 	{
 
// 		printf("read_len = %d\n", read_len);								
// 		sprintf(str,"%d",read_len);														//将int型整数转为char型		
 
// 		sprintf(receive_buf, "AT+QISEND=2,%s,\"139.199.10.173\",5001", str);			//合成发送命令
 
// 		Ql_SendAT2(receive_buf, "OK", 1000, 0);											//发送发送命令
 
// 		Ql_SendAT2(buf, "SEND OK", 5000, read_len);										//添加发送的数据
// 		bzero(receive_buf, 100);														//清空数组
// 		bzero(buf, MAXLINE);
// 	}
// 	printf("read_len = %d\n", read_len);
// 	Ql_SendAT2("AT+QICLOSE=2", "OK", 1000, 0);								
// 	close(from_file);																	//关闭文件
//    	close(smd_fd);																		//关闭虚拟串口
//     printf("< Quectel OpenLinux: AT example end >\n\n");
// 	return 0;
// }
 
int Upload_Image_TCP(char *image_path)
{
	FILE               *from_file;	//need send to server
	char               buf[BIGNUMBER]; 
	int                read_len; 
 	char		       str[10];
	bzero(buf, BIGNUMBER);

	Ql_SendAT2("ATE0", "OK", 1000, 0);
	// Ql_SendAT2("AT+COPS?", "OK", 1000, 0);									//检查网络
 
	if((from_file=open(image_path,O_RDWR))==NULL)							//打开要读取的文件
	{
		printf("Can't open the file \n");
		exit(1);
	}
 
	Ql_SendAT2("AT+QIOPEN=1,0,\"TCP\",\"139.199.10.173\",5000,0,1", "OK", 1000, 0);	
	Ql_SendAT2("AT+QISTATE=1,0", "OK", 1000, 0);	

	read_len = read(from_file, buf, BIGNUMBER);
	printf("Read Length = %d\n", read_len);

	buf[read_len] = 0x1A;
	Ql_SendAT2("AT+QISEND=0", "OK", 1000, 0);	
	printf("---Start Transmitting--- \n");
	Ql_SendAT2(buf, "SEND OK", 10000, read_len + 1);
	printf("---Transmitting Completed!--- \n");
	Ql_SendAT2("AT+QICLOSE=0", "OK", 1000, 0);	
	bzero(buf, BIGNUMBER);																								//关闭虚拟串口
    printf("< end >\n");
	return 0;
}

int Upload_Image_TCP3(char *image_path)
{
	FILE               *from_file;	//need send to server
	char               buf[MAXLINE]; 
	int                read_len; 
 	char		       str[10];
	char               at_command_tmp[100]; 

	if((from_file = open(image_path, O_RDONLY)) < 0)							//打开要读取的文件
	{
		printf("Can't open the file \n");
		exit(1);
	}
 
	Ql_SendAT3("AT+QIOPEN=1,0,\"TCP\",\"139.199.10.173\",5000,0,1", "OK", 1000, 1);	
	Ql_SendAT3("AT+QISTATE=1,0", "OK", 1000, 1);	

	printf("---Start Transmitting--- \n");
	while ( (read_len = read(from_file, buf, MAXLINE)) > 0 )							//读取文件知道读完
	{
		printf("Read Length = %d\n", read_len);
		sprintf(str,"%d",read_len);														//将int型整数转为char型		
		sprintf(at_command_tmp, "AT+QISEND=0,%s", str);					                //合成发送命令
		Ql_SendAT3(at_command_tmp, "OK", 1000, 1);	
		Ql_SendAT2(buf, "SEND OK", 5000, read_len);										//添加发送的数据
		bzero(at_command_tmp, 100);														//清空数组
		bzero(buf, MAXLINE);
	}
	printf("---Transmitting Completed!--- \n");

	Ql_SendAT3("AT+QICLOSE=0", "OK", 1000, 1);	
	bzero(buf, MAXLINE);		
	close(from_file);																	//关闭文件
   	// close(smd_fd);																		//关闭虚拟串口
    printf("< end >\n");
	return 0;
}

int GPS_Config(void)
{
	Ql_SendAT3("AT+QGPSCFG=\"outport\",\"uartdebug\"", "OK", 1000, 1);	
	Ql_SendAT3("AT+QGPS=1", "OK", 1000, 1);
	while( strlen(GPS_Raw_Data) < 20 )
	{
		memset(GPS_Raw_Data, '\0', sizeof(GPS_Raw_Data));
		Ql_SendAT3("AT+QGPSLOC=2", "OK", 2000, 1);
		printf("GPS_Raw_Data: %s\n", GPS_Raw_Data);
	}
	printf("GPS Is Ready\n");
}

int HTTP_Config(void)
{
	Ql_SendAT3("AT+QHTTPCFG=\"contextid\",1", "OK", 1000, 1);
	Ql_SendAT3("AT+QHTTPCFG=\"contenttype\",1", "OK", 1000, 1);
	Ql_SendAT3("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1", "OK", 1000, 1);
	Ql_SendAT3("AT+QIACT=1", "OK", 1000, 1);
	Ql_SendAT3("AT+QHTTPURL=33,80", "OK", 1000, 1);
	Ql_SendAT2("http://ljq.ink/upload_image_info/", "OK", 1000, 33);
	printf("HTTP Is Ready\n");
}

int Upload_Image_Info(int class)
{
	//Raw Data Process
	char time[10];
    char latitude[12];
    char longitude[12];
    char data[100]; // 定义一个字符数组来存储输出字符串
	char str[20];

    const char *cleaned_str = GPS_Raw_Data + strspn(GPS_Raw_Data, " \t\r\n");

    int ret = sscanf(cleaned_str, "+QGPSLOC: %[^,],%[^,],%[^,]", time, latitude, longitude);
	printf("sscanf returned %d\n", ret);
    // 对时间进行处理
    char hour[3];
    char minute[3];
    char second[3];
    sscanf(time, "%2s%2s%2s", hour, minute, second);
    char new_time[10];
    sprintf(new_time, "%s-%s-%s", hour, minute, second);

    // 对经纬度进行处理
    double lat_degree, lat_minute, lon_degree, lon_minute;
    sscanf(latitude, "%2lf%lf", &lat_degree, &lat_minute);
    sscanf(longitude, "%3lf%lf", &lon_degree, &lon_minute);
    double new_latitude = lat_degree + lat_minute;
    double new_longitude = lon_degree + lon_minute;

    printf("Latitude: %lf\n", new_latitude);
    printf("Longitude: %lf\n", new_longitude);
	memset(GPS_Raw_Data, '\0', sizeof(GPS_Raw_Data));

    sprintf(data, "{\"longitude\":\"%lf\",\"latitude\":\"%lf\",\"result\":\"%d\"}", new_longitude, new_latitude, class);
	Ql_SendAT3("AT+QHTTPPOST=62,80,80", "OK", 1000, 1);
	Ql_SendAT2(data, "SEND OK", 5000, 62);										//添加发送的数据
}

int Ql_SendAT(char* atCmd, char* finalRsp, long timeout_ms)
{
    int iRet;
    int iLen;
    fd_set fds;
    int rdLen;
#define lenToRead 100
    char strAT[100];
    char strFinalRsp[100];
    char strResponse[100];
    struct timeval timeout = {0, 0};
    boolean bRcvFinalRsp = FALSE;

    memset(strAT, 0x0, sizeof(strAT));
    iLen = sizeof(atCmd);
    strncpy(strAT, atCmd, iLen);

    sprintf(strFinalRsp, "\r\n%s", finalRsp);
    
	timeout.tv_sec  = timeout_ms / 1000;
	timeout.tv_usec = timeout_ms % 1000;
    
    
    // Add <cr><lf> if needed
    iLen = strlen(atCmd);
    if ((atCmd[iLen-1] != '\r') && (atCmd[iLen-1] != '\n'))
    {
		iLen = sprintf(strAT, "%s\r\n", atCmd); 
		strAT[iLen] = 0;
    }

    // Send AT
    iRet = UartSend(smd_fd, strAT, iLen);
    printf(">>Send AT: \"%s\", iRet=%d\n", atCmd, iRet);
    
    // Wait for the response
	while (1)
	{
		FD_ZERO(&fds); 
		FD_SET(smd_fd, &fds); 

		// printf("timeout.tv_sec=%d, timeout.tv_usec: %d \n", (int)timeout.tv_sec, (int)timeout.tv_usec);
		switch (select(smd_fd + 1, &fds, NULL, NULL, &timeout))
		//switch (select(smd_fd + 1, &fds, NULL, NULL, NULL))	// block mode
		{
		case -1: 
			printf("< select error >\n");
			return -1;

		case 0:
			printf("< time out >\n");
			return 1; 

		default: 
			if (FD_ISSET(smd_fd, &fds)) 
			{
				do {
					memset(strResponse, 0x0, sizeof(strResponse));
					rdLen = read(smd_fd, strResponse, lenToRead);
					// printf(">>Read response/urc, len=%d, content:\n%s\n", rdLen, strResponse);
					//printf("rcv:%s", strResponse);
					//printf("final rsp:%s", strFinalRsp);
					if ((rdLen > 0) && strstr(strResponse, strFinalRsp))
					{
					    if (strstr(strResponse, strFinalRsp)     // final OK response
					       || strstr(strResponse, "+CME ERROR:") // +CME ERROR
					       || strstr(strResponse, "+CMS ERROR:") // +CMS ERROR
					       || strstr(strResponse, "ERROR"))      // Unknown ERROR
					    {
					        //printf("\n< match >\n");
					        bRcvFinalRsp = TRUE;
					    }else{
					        printf("\n< not final rsp >\n");
					    }
					}
				} while ((rdLen > 0) && (lenToRead == rdLen));
			}else{
				printf("FD is missed\n");
			}
			break;
		}

		// Found the final response , return back
		if (bRcvFinalRsp)
		{
		    break;
		}	
   	}
   	return 0;
}

int Ql_SendAT2(char* atCmd, char* finalRsp, long timeout_ms, int should_Add)//should_Add是判断是否末尾要加入\r\n，如果不添加则还代表atCmd数据的长度
{
    int iRet;
    int iLen;
    fd_set fds;
    int rdLen;
#define lenToRead 100
    char strAT[100];
    char strFinalRsp[100];
    char strResponse[100];
    struct timeval timeout = {0, 0};
    boolean bRcvFinalRsp = FALSE;
 
    sprintf(strFinalRsp, "\r\n%s", finalRsp);
    
	timeout.tv_sec  = timeout_ms / 1000;
	timeout.tv_usec = timeout_ms % 1000;
    
    // Send AT
    if( should_Add != 0 )
	{
		// iRet = UartSend(smd_fd, strAT, iLen);
		iRet = write(smd_fd, atCmd, should_Add);
		printf("Data Length = %d\n", should_Add);
		printf(">>QT2 Send Data: \"%s\", iRet=%d\n", atCmd, iRet);
	}
	else
	{
		memset(strAT, 0x0, sizeof(strAT));
		iLen = sizeof(atCmd);
		strncpy(strAT, atCmd, iLen);
		iLen = strlen(atCmd);
		if ((atCmd[iLen-1] != '\r') && (atCmd[iLen-1] != '\n'))
		{
			iLen = sprintf(strAT, "%s\r\n", atCmd); 
			strAT[iLen] = 0;
		}
		iRet = write(smd_fd, strAT, iLen);
		printf(">>QT2 Send AT: \"%s\", iRet=%d\n", atCmd, iRet);
		// iRet = UartSend(smd_fd, strAT, iLen);
	}
    // Wait for the response
	while (1)
	{
		FD_ZERO(&fds); 
		FD_SET(smd_fd, &fds); 
 
		//printf("timeout.tv_sec=%d, timeout.tv_usec: %d \n", (int)timeout.tv_sec, (int)timeout.tv_usec);
		switch (select(smd_fd + 1, &fds, NULL, NULL, &timeout))
		//switch (select(smd_fd + 1, &fds, NULL, NULL, NULL))	// block mode
		{
		case -1: 
			printf("< select error >\n");
			return -1;
 
		case 0:
			printf("< time out >\n");
			return 1; 
 
		default: 
			if (FD_ISSET(smd_fd, &fds)) 
			{
				do {
					memset(strResponse, 0x0, sizeof(strResponse));
					rdLen = read(smd_fd, strResponse, lenToRead);
					printf(">>Read response/urc, len=%d, content:\n%s\n", rdLen, strResponse);
					printf("rcv:%s", strResponse);
					printf("final rsp:%s", strFinalRsp);
					if ((rdLen > 0) && strstr(strResponse, strFinalRsp))
					{
					    if (strstr(strResponse, strFinalRsp)     // final OK response
					       || strstr(strResponse, "+CME ERROR:") // +CME ERROR
					       || strstr(strResponse, "+CMS ERROR:") // +CMS ERROR
					       || strstr(strResponse, "ERROR"))      // Unknown ERROR
					    {
					        //printf("\n< match >\n");
					        bRcvFinalRsp = TRUE;
					    }else{
					        printf("\n< not final rsp >\n");
					    }
					}
				} while ((rdLen > 0) && (lenToRead == rdLen));
			}else{
				printf("FD is missed\n");
			}
			break;
		}
 
		// Found the final response , return back
		if (bRcvFinalRsp)
		{
		    break;
		}	
   	}
   	return 0;
}

int Ql_SendAT3(char* atCmd, char* finalRsp, long timeout_ms, int should_Add)
{
    int iRet;
    int iLen;
    fd_set fds;
    int rdLen;
#define lenToRead 100
    char strAT[100];
    char strFinalRsp[100];
    char strResponse[100];
    struct timeval timeout = {0, 0};
    boolean bRcvFinalRsp = FALSE;

    memset(strAT, 0x0, sizeof(strAT));
    iLen = sizeof(atCmd);
    strncpy(strAT, atCmd, iLen);

    sprintf(strFinalRsp, "\r\n%s", finalRsp);
    
	timeout.tv_sec  = timeout_ms / 1000;
	timeout.tv_usec = timeout_ms % 1000;

	if( should_Add != 0 )
	{
		// Add <cr><lf> if needed
		iLen = strlen(atCmd);
		if ((atCmd[iLen-1] != '\r') && (atCmd[iLen-1] != '\n'))
		{
			iLen = sprintf(strAT, "%s\r\n", atCmd); 
			strAT[iLen] = 0;
		}
		iRet = UartSend(smd_fd, strAT, iLen);
    	printf(">>QT3 AT Send: \"%s\", iRet=%d\n", atCmd, iRet);
	}
	else
	{
		// Send Data
		iRet = UartSend(smd_fd, atCmd, iLen);
		printf(">>QT3 Data Send: \"%s\", iRet=%d\n", atCmd, iRet);
	}
    // Wait for the response
	while (1)
	{
		FD_ZERO(&fds); 
		FD_SET(smd_fd, &fds); 

		// printf("timeout.tv_sec=%d, timeout.tv_usec: %d \n", (int)timeout.tv_sec, (int)timeout.tv_usec);
		switch (select(smd_fd + 1, &fds, NULL, NULL, &timeout))
		//switch (select(smd_fd + 1, &fds, NULL, NULL, NULL))	// block mode
		{
		case -1: 
			printf("< select error >\n");
			return -1;

		case 0:
			printf("< time out >\n");
			return 1; 

		default: 
			if (FD_ISSET(smd_fd, &fds)) 
			{
				do {
					memset(strResponse, 0x0, sizeof(strResponse));
					rdLen = read(smd_fd, strResponse, lenToRead);
					if (strcmp(atCmd, "AT+QGPSLOC=2") == 0)// GPS需要有信息返回
					{
						strncat(GPS_Raw_Data, strResponse, sizeof(GPS_Raw_Data) - strlen(GPS_Raw_Data) - 1);

						// strcpy(GPS_Raw_Data, strResponse);
						// printf("%s\n", GPS_Raw_Data);
						// strncpy(GPS_Raw_Data, strResponse, sizeof(GPS_Raw_Data) - 1);
						// GPS_Raw_Data[sizeof(GPS_Raw_Data) - 1] = '\0';  // 确保字符串以空字符结尾
					}
					printf(">>Read response/urc, len=%d, content:\n%s\n", rdLen, strResponse);
					// printf("rcv:%s", strResponse);
					// printf("final rsp:%s", strFinalRsp);
					if ((rdLen > 0) && strstr(strResponse, strFinalRsp))
					{
					    if (strstr(strResponse, strFinalRsp)     // final OK response
					       || strstr(strResponse, "+CME ERROR:") // +CME ERROR
					       || strstr(strResponse, "+CMS ERROR:") // +CMS ERROR
					       || strstr(strResponse, "ERROR"))      // Unknown ERROR
					    {
					        //printf("\n< match >\n");
					        bRcvFinalRsp = TRUE;
					    }else{
					        printf("\n< not final rsp >\n");
					    }
					}
				} while ((rdLen > 0) && (lenToRead == rdLen));
			}else{
				printf("FD is missed\n");
			}
			break;
		}

		// Found the final response , return back
		if (bRcvFinalRsp)
		{
		    break;
		}	
   	}
   	return 0;
}
