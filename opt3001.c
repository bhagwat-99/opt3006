#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

//sensor i2c address is 0x44
#define SLAVE_ADDR 0X44

//calculate power of integer
int ipow(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

//lux value calculation
float calculate_lux(__uint16_t raw_data)
{
    __uint8_t   exponent = raw_data >> 12;
    __uint16_t  mantisa = raw_data & 0x0fff;

    float lux_value = 0.01 * ipow(2,exponent) * mantisa ;
    return lux_value;

}

void main()
{
// Create I2C bus
	int file;
	char *bus = "/dev/apalis-i2c1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, HTS221 I2C address is 0x5F(95)
	ioctl(file, I2C_SLAVE, SLAVE_ADDR);

    unsigned char reg[1] = {0};
    unsigned char data[2] = {0};
    __uint16_t reg_data;

    //reading 2 bytes from 0x00
    reg[0]=0x00 | 0x80; // to read multiple address in one command need to or with 0x80 to auto increment address
    write(file, reg, 1);
    if(read(file, data, 2) != 2)
	{
		printf("Erorr : Input/output Erorr \n");
	}
    else
    {
        //conversion from 8 bit to 16 bit value
        reg_data = (data[1] * 256 + data[0]);

    }

    float lux = calculate_lux(reg_data);
    printf("Lux = %f",lux);

}
