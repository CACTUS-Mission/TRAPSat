#include <stdio.h>
#include <stdint.h>

uint8_t bit_test(uint8_t* data, uint8_t bit_n);

uint8_t parallel_write(uint8_t* msg, size_t msg_size);

int main(int argc, char* argv[])
{
	uint8_t data[2] = "Aa";
	uint8_t result = 0;
	result = parallel_write(data, sizeof(data));
	printf("Parallel Write() Result => %d\n", result);
	return 0;
}

/*
** uint8_t bit_test(uint8_t* data, uint8_t bit_n)
**
**
*/
uint8_t bit_test(uint8_t* data, uint8_t bit_n)
{
	return (*data & (1 << bit_n));
}

/*
** uint8_t parallel_write(uint8_t* msg, size_t msg_size)
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
** Return Value (int)
** ============================================================
** On Error 
**  - Size Error => (-1)
** On Success
**  - Number of Bits Written (should be 8 * msg_size)
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
			printf("\tsizeof(msg) => %lu.\n", sizeof(msg));
			printf("\tmsg_size => %lu.\n", msg_size);
			printf("[WARNING] parallel_write_16(): argument size too large\n");
			printf("[WARNING] parallel_write_16(): attempting to continue\n");
			printf("[WARNING] parallel_write_16(): data will be truncated\n");
		}	
		else
		{
			printf("\tsizeof(msg) => %lu.\n", sizeof(msg));
			printf("\tmsg_size => %lu.\n", msg_size);
			printf("[ERROR] parallel_write_16(): arguement size too small\n");
			printf("[ERROR] parallel_write_16(): cannot continue; exit now\n");
			return -1;
		}
	}


	/*
	** Translate Bytes to Bits and Write
	*/
	uint8_t index = 0;
	uint8_t bit_index = 0;
	uint8_t byte_index = 0;

	for( index = 0; index < (msg_size * 8); index++ )
	{
		bit_index = (uint8_t)(index % 8);  /* Indexing 0 though 7 of single byte */
		byte_index = (uint8_t)(index / 8);  /* Indexing 0 through [msg_size] of [msg] */
		printf("[DEBUG] BIT_TEST(%c, %d) => %d\n",
			 msg[byte_index], bit_index,
			 bit_test(&msg[byte_index], bit_index));
	}

	return index;
}