#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

int main(){
	int fd, i; // initialize file descriptor and counter
	uint8_t msb, lsb; //variable for concatenation
	uint16_t data; //data to be printed
	char config = 0x68; //configuration variable. use internal vref and internal clock
	char buffer[100] = {0}; //data buffer
	fd = wiringPiSPISetup(1, 8000000); //get the file descriptor of the device at CS=1 and SPI frequency = 8MHZ. device max is 10MHz. 
	if(fd < 0){
		printf("SPI error %d\n", fd);
		return 0;
	}
	wiringPiSPIDataRW(1, &config, 1); //write configuration byte to SPI
	config = 0x0; //turn off averaging
	write(fd, &config, 1); //send command to SPI bus
//	printf("%d\n", config);
//	wiringPiSPIDataRW(1, buffer, 64);
	while(1){
		buffer[0] = 0xF8; //conversion byte. 0x86 is channel 0 only, 0xF8 is scan all.
		write(fd, buffer, 1);//send conversion byte to SPI bus. conversion is initialized
		usleep(1); //wait for the device to do conversion. set the CS1 to high
		read(fd, buffer, 32); //read 32 bytes from the SPI bus
	//	read(fd, &buffer[1], 1);
		for(i = 0; i < 32; i++){
			msb = buffer[i * 2]; //get the most significant byte
			lsb = buffer[i * 2 + 1]; //get the least significant byte
			data = ((uint16_t)msb << 8) | lsb; //cast the msb and shift 8 byte to the left. OR it with the least significant byte to produce 16 bit data.
//			data = data << 4; //ignore 4 most significant bit.
			data = data >> 2; //get rid of 2 least significant bit
			printf("%5d ", data); //final value.
		}
		printf("\n");
	}
/*

	for(i=0 ; i< 50; i++){
		printf("%d %d\n", buffer[2*i], buffer[2*i+1]);

	}
*/
}
