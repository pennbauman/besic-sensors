// BESI-C Sensor Board Revision 3.0 Driver
//   https://github.com/besi-c/besic-sensors
//   Yudel Martinez <yam3nv@virginia.edu>
//   Penn Bauman <pcb8gb@virginia.edu>
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
	#ifdef DEBUG_
	printf("veml7700\n");
	#endif

	if (gpioInitialise() < 0){
		return -1;
	}
	int fd = i2cOpen(1,VEML7700_DEVICE_ID,0);

	if (fd < 0){
		return -1;
	}

	i2cWriteWordData(fd,0x00,VEML7700_CONFIG_SETTINGS);
	i2cWriteWordData(fd,0x03,VEML7700_POWER_SETTINGS);

	uint16_t data = i2cReadWordData(fd,0x04);

	float f = convert_bin_to_lux(data);

	#ifdef DEBUG_
	printf("%x\n%d\n",data,data);
	printf("%.2f\n",f);
	#endif

	i2cClose(fd);
	return f;
}

float hs3002(){
	#ifdef DEBUG_
	printf("hs3002\n");
	#endif

	if (gpioInitialise() < 0){
		return -1;
	}
	int fd = i2cOpen(1,HS300x_DEVICE_ID,0);

	if (fd < 0){
		return -1;
	}

	i2cWriteByte(fd,0);

	delay(50);

	int data = i2cReadByte(fd);

	data = endian(data);

	float f = convert_bin_to_hum(data);

	#ifdef DEBUG_
	printf("%x\n%d\n",data,data);
	printf("%.2f\n",f);
	#endif

	i2cClose(fd);
	return f;
}

float tmp117(){
	#ifdef DEBUG_
	printf("tmp117\n");
	#endif

	if (gpioInitialise() < 0){
		return -1;
	}
	int fd = i2cOpen(1,TMP117_DEVICE_ID,0);

	if (fd < 0){
		return -1;
	}

	i2cWriteWordData(fd,0x01,TMP117_CONFIG_SETTINGS); // had the id again lol

	uint16_t data = i2cReadWordData(fd,0x00);

	data = endian(data);

	float f = convert_bin_to_tmp(data);

	#ifdef DEBUG_
	printf("%x\n%d\n",data,data);
	printf("%.2f\n",f);
	#endif

	i2cClose(fd);
	return f;
}

uint32_t array_to_int(uint8_t*value){
	uint32_t ret = (value[0] << 16) | (value[1]<<8) | value[2];
	return ret;
}

float ms5607(){
	#ifdef DEBUG_
	printf("ms5607\n");
	#endif

	if (gpioInitialise() < 0){
		return -1;
	}
	int fd = i2cOpen(1,MS5607_DEVICE_ID,0);

	if (fd < 0){
		return -1;
	}

	uint8_t cmd = 0;
	uint16_t C1,C2,C3,C4,C5,C6 = 5;
	uint8_t D1_raw[3],D2_raw[3];

	cmd = 0xA2;
	i2cWriteByte(fd,cmd);
	delay(10);
	i2cReadDevice(fd,(char*)&C1,2);
	cmd = 0xA4;
	i2cWriteByte(fd,cmd);
	delay(10);
	i2cReadDevice(fd,(char*)&C2,2);
	cmd = 0xA6;
	i2cWriteByte(fd,cmd);
	delay(10);
	i2cReadDevice(fd,(char*)&C3,2);
	cmd = 0xA8;
	i2cWriteByte(fd,cmd);
	delay(10);
	i2cReadDevice(fd,(char*)&C4,2);
	cmd = 0xAA;
	i2cWriteByte(fd,cmd);
	delay(10);
	i2cReadDevice(fd,(char*)&C5,2);
	cmd = 0xAC;
	i2cWriteByte(fd,cmd);
	delay(10);
	i2cReadDevice(fd,(char*)&C6,2);

	C1 = endian(C1);
	C2 = endian(C2);
	C3 = endian(C3);
	C4 = endian(C4);
	C5 = endian(C5);
	C6 = endian(C6);

	#ifdef DEBUG_
	printf("1:%u\n2:%u\n3:%u\n4:%u\n5:%u\n6:%u\n",C1,C2,C3,C4,C5,C6);
	#endif

	i2cWriteByte(fd,0x48);
	delay(10);
	i2cWriteByte(fd,0x00);
	delay(10);
	i2cReadDevice(fd,(char*)&D1_raw,3);

	i2cWriteByte(fd,0x58);
	delay(10);
	i2cWriteByte(fd,0x00);
	delay(10);
	i2cReadDevice(fd,(char*)&D2_raw,3);

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

	i2cClose(fd);
	return P;
}


void readData(besic_data *d) {
	struct timeval time;
	gettimeofday(&time, NULL);
	d->timestamp = time.tv_sec*1000LL + time.tv_usec/1000LL;
	d->lux = veml7700();
	d->tmp = tmp117();
	d->prs = ms5607();
	d->hum = hs3002() * 100;
}
