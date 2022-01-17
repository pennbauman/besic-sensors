#include "device.h"

#define TMP117_DEVICE_ID 0x48
#define TMP117_CONFIG_SETTINGS 0x0028
#define TMP117_TEMP_RESOLUTION 0.0078125

#define HS300x_DEVICE_ID 0x44

#define VEML7700_CONFIG_SETTINGS 0x0000 //0b000 00 0 0000 00 00 0 0 
#define VEML7700_POWER_SETTINGS 0x0000
#define VEML7700_DEVICE_ID 0x10
#define VEML7700_RESOLUTION 0.0288

#define MS5607_DEVICE_ID 0x77

//#define DEBUG_

int fd;

char ADDR [18];

void mac_addr(){
	FILE *f = fopen("/sys/class/net/wlan0/address","r");
	fgets(ADDR,18,f);
	fclose(f);
}

void delay(int ms){
	usleep((1000*ms));
}

float convert_bin_to_tmp(uint16_t data){
	float d = (float) data;
	return d*TMP117_TEMP_RESOLUTION;
}

float convert_bin_to_hum(uint16_t data){
	float d = (float) data;
	float num = d/((float)((int)0b100000000000000 - 1));
	return num;
}

float convert_bin_to_lux(uint16_t data){
	float d = (float) data;
	return d*VEML7700_RESOLUTION;
}

uint16_t endian(uint16_t data){
	return ((data&0xff00)>>8) | ((data&0x00ff)<<8);
}

float veml7700(){
	fd = wiringPiI2CSetup(VEML7700_DEVICE_ID);

	if(fd == -1){
		return -1;
	}

	wiringPiI2CWriteReg16(fd,0x00,VEML7700_CONFIG_SETTINGS);
	wiringPiI2CWriteReg16(fd,0x03,VEML7700_POWER_SETTINGS);

	uint16_t data = wiringPiI2CReadReg16(fd,0x04);

	float f = convert_bin_to_lux(data);

	#ifdef DEBUG_
	printf("%x\n%d\n",data,data);
	printf("%.2f\n",f);
	#endif

	return f;
}

float hs3002(){
	fd = wiringPiI2CSetup(HS300x_DEVICE_ID);

	if(fd == -1){
		return -1;
	}

	wiringPiI2CWrite(fd,0);

	delay(50);

	int data = wiringPiI2CRead(fd);

	data = endian(data);

	float f = convert_bin_to_hum(data);

	#ifdef DEBUG_
	printf("%x\n%d\n",data,data);
	printf("%.2f\n",f);
	#endif

	return f;
}

float tmp117(){
	fd = wiringPiI2CSetup(TMP117_DEVICE_ID);

	if (fd == -1){
		return -1;
	}

	wiringPiI2CWriteReg16(fd,0x01,TMP117_CONFIG_SETTINGS); // had the id again lol

	uint16_t data = wiringPiI2CReadReg16(fd,0x00);

	data = endian(data);

	float f = convert_bin_to_tmp(data);

	#ifdef DEBUG_
	printf("%x\n%d\n",data,data);
	printf("%.2f\n",f);
	#endif

	return f;
}

uint32_t array_to_int(uint8_t*value){
	uint32_t ret = (value[0] << 16) | (value[1]<<8) | value[2];
	return ret;
}

float ms5607(){
	fd = wiringPiI2CSetup(0x77);

	if (fd == -1){
		return -1;
	}

	uint8_t cmd = 0;
	uint16_t C1,C2,C3,C4,C5,C6 = 5;
	uint8_t D1_raw[3], D2_raw[3];

	cmd = 0xA2;
	write(fd,&cmd,1);
	delay(10);
	read(fd,&C1,2);
	cmd = 0xA4;
	write(fd,&cmd,1);
	delay(10);
	read(fd,&C2,2);
	cmd = 0xA6;
	write(fd,&cmd,1);
	delay(10);
	read(fd,&C3,2);
	cmd = 0xA8;
	write(fd,&cmd,1);
	delay(10);
	read(fd,&C4,2);
	cmd = 0xAA;
	write(fd,&cmd,1);
	delay(10);
	read(fd,&C5,2);
	cmd = 0xAC;
	write(fd,&cmd,1);
	delay(10);
	read(fd,&C6,2);

	C1 = endian(C1);
	C2 = endian(C2);
	C3 = endian(C3);
	C4 = endian(C4);
	C5 = endian(C5);
	C6 = endian(C6);

	#ifdef DEBUG_
	printf("1:%u\n2:%u\n3:%u\n4:%u\n5:%u\n6:%u\n",C1,C2,C3,C4,C5,C6);
	#endif

	wiringPiI2CWrite(fd,0x48);
	delay(10);
	wiringPiI2CWrite(fd,0x00);
	delay(10);
	read(fd,&D1_raw,3);

	wiringPiI2CWrite(fd,0x58);
	delay(10);
	wiringPiI2CWrite(fd,0x00);
	delay(10);
	read(fd,&D2_raw,3);

	uint32_t D1 = array_to_int(D1_raw);
	uint32_t D2 = array_to_int(D2_raw);

	float C1_ = (float) C1;
	float C2_ = (float) C2;
	float C3_ = (float) C3;
	float C4_ = (float) C4;
	float C5_ = (float) C5;
	float C6_ = (float) C6;
	float D1_ = (float) D1;
	float D2_ = (float) D2;


	float dT = D2_ - (C5_*256);
	float TEMP = 2000+(dT*(C6_/(8388608)));

	float OFF = (C2_*131072) + ((C4_+dT)/(64));
	float SENS = (C1_*65536) + ((C3_*dT)/(127));

	float P = (((D1_)*(SENS/(2097152)))-OFF)/(32768);
	P = P/100;
	TEMP = TEMP/100;

	#ifdef DEBUG_
	printf("D1:%0.2f\n",D1_);
	printf("D2:%0.2f\n",D2_);
	printf("dT:%0.2f\n",dT);
	printf("TEMP:%0.2f\n",TEMP);
	printf("OFF:%0.2f\n",OFF);
	printf("SENS:%0.2f\n",SENS);
	printf("P:%0.2f\n",P);
	#endif

	return P;
}

/*
int main(){
	printf("\n~~~~~~ MAC ADDR ~~~~~~\n");
	mac_addr();
	printf("\nMAC: %s\n",ADDR);
	printf("\n~~~~~~ VEML7700 ~~~~~~\n");
	float LUX = veml7700();
	printf("\nLUX: %10.2f\n",LUX);
	printf("\n~~~~~~  HS3002  ~~~~~~\n");
	float HUM = hs3002() * 100;
	printf("\nHUM: %10.2f%%\n",HUM);
	printf("\n~~~~~~  TMP117  ~~~~~~\n");
	float TMP = tmp117();
	printf("\nTMP: %10.2f°C\n",TMP,176);
	printf("\n~~~~~~  MS5607  ~~~~~~\n");
	float PRS = ms5607();
	printf("\nPRS: %10.2f mBar\n\n",PRS);
	return 0;
} // */
