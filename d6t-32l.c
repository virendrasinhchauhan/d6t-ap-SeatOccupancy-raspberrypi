/*
 * MIT License
 * Copyright (c) 2019, 2018 - present OMRON Corporation
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

 /* includes */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <time.h>
#include <linux/i2c.h> //add

/* defines */
#define D6T_ADDR 0x0A  // for I2C 7bit address
#define D6T_CMD 0x4D  // for D6T-32L-01A, compensated output.

#define N_ROW 32
#define N_PIXEL (32 * 32)

#define N_READ ((N_PIXEL + 1) * 2 + 1)
uint8_t rbuf[N_READ];

#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

/***** Setting Parameter *****/
#define comparingNumInc 8 // x200 ms   (example) 8 -> 1.6 sec
#define comparingNumDec 8  // x200 ms   (example) 8 -> 1.6 sec
#define threshHoldInc 18 //  /10 degC   (example) 18 -> 1.8 degC
#define threshHoldDec 12 //  /12 degC   (example) 10 -> 1.2 degC
#define moveAveTimes 4 //   Max 10 (example) 4
#define personPix 10 //   (example) 10
bool  enablePix[1024] = {
			false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,
			false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
			false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
};
/****************************/
uint8_t rbuf[N_READ];
int16_t pix_data[1024] = { 0 };
int16_t seqData[1024][10] = { 0 };
int16_t seqAveData[1024][10] = { 0 }; //add
bool  occuPix[1024] = { 0 };
uint16_t  occuPixNum = 0;  // add
uint8_t  resultOccupancy = 0;
uint16_t  totalCount = 0;
uint16_t printCount = 0;

/** JUDGE_occupancy: judge occupancy*/
bool judge_seatOccupancy(void) {
	int i = 0;
	int j = 0;
	int k = 0;
	bool minusFlag = false;
	int32_t sumData = 0; //add
	uint32_t sumDataU = 0; //add
	uint16_t tempData = 0;
	for (i = 0; i < 1024; i++) {
		for (j = 0; j < 9; j++) {
			seqData[i][9 - j] = seqData[i][8 - j];
			seqAveData[i][9 - j] = seqAveData[i][8 - j]; //add
		}
		seqData[i][0] = pix_data[i];
		for (k = 0; k < moveAveTimes; k++) {
			sumData = sumData + seqData[i][k];
		}
		if (sumData > 0) {
			sumDataU = sumData;
			minusFlag = false;
		}
		else {
			sumDataU = -sumData;
			minusFlag = true;
		}
		tempData = (uint16_t)(((uint32_t)sumDataU) / ((uint32_t)moveAveTimes));
		if (minusFlag == false) {
			seqAveData[i][0] = (int16_t)tempData;
		}
		else {
			seqAveData[i][0] = -((int16_t)tempData);
		}		
		sumData = 0;
	}
	if (totalCount <= comparingNumInc) {
		totalCount++;
	}
	if (totalCount > comparingNumInc) {
		for (i = 0; i < 1024; i++) {
			if (enablePix[i] == true) {
				if (occuPix[i] == false) {
					if ((int16_t)(seqAveData[i][0] - seqAveData[i][comparingNumInc]) > (int16_t)threshHoldInc) { // change to seqAveData
						occuPix[i] = true;
					}
				}
				else {
					if ((int16_t)(seqAveData[i][comparingNumDec] - seqAveData[i][0]) > (int16_t)threshHoldDec) { // change to seqAveData
						occuPix[i] = false;
					}
				}
			}
		}
		occuPixNum = 0;
		for (i = 0; i < 1024; i++) {
			if (occuPix[i] == true) {
				occuPixNum = occuPixNum + 1;
			}
		}
		if (occuPixNum > personPix - 1) {
			resultOccupancy = 1;
		}
		else {
			resultOccupancy = 0;
		}
	}
	return true;
}

/* I2C functions */
/** <!-- i2c_read_reg8 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_read_reg8(uint8_t devAddr, uint8_t regAddr,
	uint8_t *data, int length
) {
	int fd = open(I2CDEV, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
		return 21;
	}
	int err = 0;
	do {
		struct i2c_msg messages[] = {
			{ devAddr, 0, 1, &regAddr },
			{ devAddr, I2C_M_RD, length, data },
		};
		struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };
		if (ioctl(fd, I2C_RDWR, &ioctl_data) != 2) {
			fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
		}

	} while (false);
	close(fd); //change
	return err;
}

uint8_t calc_crc(uint8_t data) {
	int index;
	uint8_t temp;
	for (index = 0; index < 8; index++) {
		temp = data;
		data <<= 1;
		if (temp & 0x80) { data ^= 0x07; }
	}
	return data;
}

/** <!-- D6T_checkPEC {{{ 1--> D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n) {
	int i;
	uint8_t crc = calc_crc((D6T_ADDR << 1) | 1);  // I2C Read address (8bit)
	for (i = 0; i < n; i++) {
		crc = calc_crc(buf[i] ^ crc);
	}
	bool ret = crc != buf[n];
	if (ret) {
		fprintf(stderr,
			"PEC check failed: %02X(cal)-%02X(get)\n", crc, buf[n]);
	}
	return ret;
}

/** <!-- conv8us_s16_le {{{1 --> convert a 16bit data from the byte stream.
 */
int16_t conv8us_s16_le(uint8_t* buf, int n) {
	int ret;
	ret = buf[n];
	ret += buf[n + 1] << 8;
	return (int16_t)ret;   // and convert negative.
}

void delay(int msec) {
	struct timespec ts = { .tv_sec = msec / 1000,
						  .tv_nsec = (msec % 1000) * 1000000 };
	nanosleep(&ts, NULL);
}

/** <!-- main - Thermal sensor {{{1 -->
 * 1. read sensor.
 * 2. output results, format is: [degC]
 */
int main() {
	int i, j;
	while (1) {
		memset(rbuf, 0, N_READ);
		for (i = 0; i < 10; i++) {
			uint32_t ret = i2c_read_reg8(D6T_ADDR, D6T_CMD, rbuf, N_READ);
			if (ret == 0) {
				break;
			}
			else if (ret == 23) {  // write error
				delay(60);
			}
			else if (ret == 24) {  // read error
				delay(3000);
			}
		}
		if (i >= 10) {
			fprintf(stderr, "Failed to read/write: %s\n", strerror(errno));
		}

		if (D6T_checkPEC(rbuf, N_READ - 1)) {
		}

		// 1st data is PTAT measurement (: Proportional To Absolute Temperature)
		int16_t itemp = conv8us_s16_le(rbuf, 0);
		printf("PTAT: %6.1f[degC], ", itemp / 10.0); //change

		// loop temperature pixels of each thrmopiles measurements
		for (i = 0, j = 2; i < N_PIXEL; i++, j += 2) {
			//for (i = 0, j = 2; i < 64; i++, j += 2) {
			itemp = conv8us_s16_le(rbuf, j);
			pix_data[i] = itemp; //add
			printf("%4.1f", itemp / 10.0);  // print PTAT & Temperature
			if ((i % N_ROW) == N_ROW - 1) {
				printf(",");  // wrap text at ROW end.  //change
			}
			else {
				printf(",");   // print delimiter
			}
		}
		judge_seatOccupancy();
		printf(" PixNum: %d", occuPixNum);
		printf(" Occupancy: %d\n", resultOccupancy);
		delay(200);
	}
}
// vi: ft=c:fdm=marker:et:sw=4:tw=80
