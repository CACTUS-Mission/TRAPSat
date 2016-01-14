/*
** parallel_io_slave.c
**
** Currently for use with Raspberry Pi Compute Module
** 
** Created by: K Moore
*/

#include <stdio.h>
#include <stdlib.h>  /* system() */
#include <stdint.h>

/**
 * Here is an example of a generated template by the doxydoxygen package for sublime
 *
 * @brief      { function_description }
 *
 * @param      data   { parameter_description }
 * @param[in]  bit_n  { parameter_description }
 *
 * @return     { description_of_the_return_value }
 */

uint8_t bit_test(uint8_t* data, uint8_t bit_n);
uint8_t gpio_write(uint8_t gpio_pin, uint8_t value);
uint8_t gpio_read(uint8_t gpio_pin);
uint8_t parallel_write(uint8_t* msg, size_t msg_size);

/*
** Parallel Data Read GPIO Pins
*/
const uint8_t GPIO_DATA[0, 1, 2, 3, 4, 5, 6, 7];

/*
** Parallel Read Strobe GPIO Pin
*/
const uint8_t GPIO_READ_STROBE = 8;

/*
** Main Testing Block
*/
int main(int argc, char* argv[])
{
	
	while(1)
	{
		if(gpio_read(GPIO_READ_STROBE))
		{
			uint8_t data[2] = "Aa";
			uint8_t result = 0;
			result = parallel_write(data, sizeof(data));
			printf("Parallel Write() Result => %d\n", result);
		}
		else
		{
			continue;
		}
	}

	printf("Exiting now.")

	return 0;
}


/*
** uint8_t bit_test(uint8_t* data, uint8_t bit_n)
** 
** Purpose
** ============================================================
** This function tests bit_n of data to be HIGH or LOW
** 
** Input Arguments
** ============================================================
** uint8_t* data
** This arguement expects the address to an array of 8-bit 
** unsigned integers 
**
** uint8_t bit_n
** This should be an integer between 
**
** Return Value
** ============================================================
** uint8_t
** 0 if GPIO read to be low
** 1 if GPIO read to be HIGH
** -1 if error
** 
*/

/**
 *	And here's another for this function.
 *
 * @brief      { function_description }
 *
 * @param      data   { parameter_description }
 * @param[in]  bit_n  { parameter_description }
 *
 * @return     { description_of_the_return_value }
 */
uint8_t bit_test(uint8_t* data, uint8_t bit_n)
{
	if( (sizeof(data) * 8) <= bit_n )
	{
		fprintf(stderr, "[ERROR] bit_test(): bit_n out of range\n");
		return -1;
	}

	return ( *data & (1 << bit_n) );
}

/*
** uint8_t gpio_write(uint8_t gpio_pin, uint8_t value);
** 
** Purpose
** ============================================================
** The purpose of this function is to write to a user-specified 
** GPIO pin.
** 
** Input Arguments
** ============================================================
** uint8_t gpio_pin
** This should be the GPIO pin of the Raspberry Pi CM
**
** uint8_t value
** This should be 0 or 1 (as in Low or High)
**
** Return Value
** ============================================================
** uint8_t
** On error => -1 
**
*/
uint8_t gpio_write(uint8_t gpio_pin, uint8_t value)
{
	/*
	** value should be 0 or 1
	** Mask all non-zero numbers as HIGH
	*/
	if( value != 0)
	{
		value = 1;
	}

	/*
	** Change this to check if GPIO pins in range
	*/
	if( gpio_pin < 0 || gpio_pin > 45)
	{
		return -1;
	}

	/*
	** This write technique might be able to be optimized a bit..
	*/
	uint8_t result = 0;
	char buffer[38];
	memset(buffer, '\0', sizeof(buffer));

	/*
	** Concatonate system call for write
	*/
	if( (result = sprintf(buffer, "echo %d > /sys/class/gpio/gpio%d/value", gpio_pin, value)) < 0 )
	{
		fprintf(stderr, "[ERROR] gpio_write(): sprintf() error\n");
		return -1;
	}
	
	result = system(buffer);

	return result;
}

/*
** uint8_t gpio_read(uint8_t gpio_pin);
** 
** Purpose
** ============================================================
** The purpose of this function is to read to a user-specified 
** GPIO pin.
** 
** Input Arguments
** ============================================================
** uint8_t gpio_pin
** This should be the GPIO pin of the Raspberry Pi CM
**
**
** Return Value
** ============================================================
** uint8_t
** On error => -1 
**
*/
uint8_t gpio_read(uint8_t gpio_pin)
{
	uint8_t result = 0;
	char path_buffer[32];
	memset(path_buffer, '\0', sizeof(path_buffer));
	FILE *gpio_pin_fp;
	int ch;

	/*
	** Create file path to GPIO file
	*/
	if( (result = sprintf(path_buffer, "/sys/class/gpio/gpio%d/value", gpio_pin, value)) < 0 )
	{
		fprintf(stderr, "[ERROR] gpio_read(): sprintf() error\n");
		return -1;
	}

	/*
	** Attempt to open file.
	*/
	if( (gpio_pin_fp = fopen(path_buffer,'r')) == NULL )
	{
		fprintf(stderr, "[ERROR] gpio_read(): couldn't open gpio file")
		return -1
	}

	/*
	** Read first character of file 
	*/
	ch = getc(gpio_pin_fp);
	fclose(gpio_pin_fp);

	/*
	** Check if 1 or 0
	*/
	if(atoi(ch) == 1)
	{
		result = 1;
	}
	else if(atoi(ch) == 0)
	{
		result = 0;
	}

	return result;
}

/*
** uint8_t parallel_write(uint8_t* msg, size_t msg_size);
** 
** Purpose
** ============================================================
** This function is designed to take an array of 8-bit values
** 
** Input Arguments
** ============================================================
** uint8_t* msg
**  - should be the address of an array of type unsigned char
** size_t msg_size
**  - should be the size (in bytes) of the first argument (msg)
**
** Return Value
** ============================================================
** uint8_t
** On input arguement size error => -1 
** On success => Number of Bits Written (should be 8 * msg_size)
*/
uint8_t parallel_write(uint8_t* msg, size_t msg_size)
{
	/*
	** Check input for erroneous data
	*/
	if( sizeof(msg) != sizeof(msg_size) )
	{
		if( sizeof(msg) > msg_size )
		{
			printf("\t[DEBUG] parallel_write(): sizeof(msg) => %lu.\n", sizeof(msg));
			printf("\t[DEBUG] parallel_write(): msg_size => %lu.\n", msg_size);
			fprintf(stderr, "[WARNING] parallel_write_16(): argument size too large\n");
			fprintf(stderr, "[WARNING] parallel_write_16(): attempting to continue\n");
			fprintf(stderr, "[WARNING] parallel_write_16(): data will be truncated\n");
		}	
		else
		{
			printf("\t[DEBUG] parallel_write(): sizeof(msg) => %lu.\n", sizeof(msg));
			printf("\t[DEBUG] parallel_write(): msg_size => %lu.\n", msg_size);
			fprintf(stderr, "[ERROR] parallel_write_16(): arguement size too small\n");
			fprintf(stderr, "[ERROR] parallel_write_16(): cannot continue; exit now\n");
			return -1;
		}
	}

	/*
	** Translate Bytes to Bits and Write
	*/
	uint8_t index = 0;
	uint8_t bit_index = 0;
	uint8_t byte_index = 0;

	/*
	** Loop through each parallel bit
	*/
	for( index = 0; index < (msg_size * 8); index++ )
	{
		bit_index = (uint8_t)(index % 8);  /* Indexing 0 though 7 of single byte */
		byte_index = (uint8_t)(index / 8);  /* Indexing 0 through [msg_size] of [msg] */

		printf("[DEBUG] BIT_TEST(%c, %d) => %d\n",
			 msg[byte_index], bit_index,
			 bit_test(&msg[byte_index], bit_index));

		/*
		** Write translated bit to GPIO
		*/
		if( bit_test(&msg[byte_index], bit_index)) )
		{
			gpio_write(GPIO_DATA[bit_index], 1);
		}
		else
		{
			gpio_write(GPIO_DATA[bit_index], 0);
		}
	}

	return index;
}