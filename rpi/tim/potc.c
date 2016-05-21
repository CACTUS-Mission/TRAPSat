#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>


/*
** File Definitions
*/
#define MAX_IN_FILENAME_LEN (15) /* 000_t_00000.csv */
#define MAX_OUT_FILENAME_LEN (21) /* 000_t_00000_conv.csv */
#define RAW_DATA_BUF_LEN (16) /* FF, FF, FF, FF,  */
#define DATA_SET_PER_FILE (4)

/*
** ADS1115/Thermistor definitions 
*/
#define SERIES_RES        (300000)  /* ohms */
#define THERM_NOM_RES     (10000)   /* ohms */
#define THERM_NOM_TEMP    (25)      /* degrees C */
#define THERM_NOM_BCOEF   (3892)    /* K */
#define ADS_GAIN          (4096)    /* voltage gain */
#define KELVIN_OFFSET     (273.15)  /* Celsius */


/*
** Function Prototypes
*/
float conv_word_volt(uint16_t in_word);
int conv_adc_temp(char* input_file, char* output_file);



int main(int argc, char* argv[])
{

    printf("TRAPSat RockSat-X Post-Operations Temperature Converter!\n");

    char output_file[32];
    char input_file[32];

    memset(output_file, '\0', 32);
    memset(input_file, '\0', 32);


    switch (argc)
    {
        case 2: //printf("Found one argument:\n");
                //printf("Arg 0: %s\n", argv[0]);
                //printf("Arg 1: %s\n", argv[1]);
                //printf("Using Arg 1 as input file: %s\n", argv[1]);
                strncpy(input_file, argv[1], MAX_IN_FILENAME_LEN);                
                snprintf(output_file, MAX_OUT_FILENAME_LEN, "%.11s_conv.csv", argv[1]);
                //printf("Using Arg 1 (conv) as output file: %s\n", output_file);
                printf("Source file for temps: [%s]\n", input_file);
                printf("Defaulting Destination file for temps: [%s]\n", output_file);
                break;

        case 3: // printf("Found two arguments:\n");
                // printf("Arg 0: %s\n", argv[0]);
                // printf("Using Arg 1 as input file: %s\n", argv[1]);
                // printf("Using Arg 2 as output file: %s\n", argv[2]);
                strncpy(input_file, argv[1], MAX_IN_FILENAME_LEN);
                strncpy(output_file, argv[2], MAX_OUT_FILENAME_LEN);
                printf("ADC Source Data File: [%s]\n", input_file);
                printf("Temperature Data Destination File: [%s]\n", output_file);
                break;
        default:/* Default Constant */
                fprintf(stderr, "No input/output files provided.\n");
                fprintf(stderr, "Usage 1: ~ $ %s <input_file.csv>\n", argv[0]);
                fprintf(stderr, "Usage 2: ~ $ %s <input_file.csv> <output_file.csv>\n", argv[0]);
                exit(1);
                break;
    }
    
    conv_adc_temp(input_file, output_file);

    return(0);
}

/*
** This function opens a ADC Register (Temperature CSV) file
** and converts the values from a 16-bit ADC reading to a 
** new CSV file with temperature values in Celsius.
*/
int conv_adc_temp(char* input_file, char* output_file)
{
    /*
    ** Debug, Remove later
    */
    //printf("\nint conv_adc_temp(char*, char*)\n");
    //printf("Input File (raw ADC word data): %s\n", input_file);
    //printf("Output File (temps in deg C): %s\n", output_file);
    printf("\n");


    /*
    ** File IO variables
    */
    int in_fd = 0;                     /* 16-Bit ADC Registers CSV File Descriptor */
    int out_fd = 0;                    /* Temperature CSV File Descriptor */

    char in_fn[MAX_IN_FILENAME_LEN+1];    /* Input Filename Buffer */
    char out_fn[MAX_OUT_FILENAME_LEN+1];   /* Output Filename Buffer */

    mode_t in_mode = S_IRUSR | S_IRGRP | S_IROTH;          /* r--r--r-- (444) */
    mode_t out_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; /* rw-r--r-- (644) */

    /*
    ** Data Conversion variables
    */
    int set_index = 0;

    uint8_t raw_byte_data[DATA_SET_PER_FILE][2];
    uint8_t comma_space[2] = {0x00, 0x00};           /* This buff will check the validity of file */
    
    uint16_t word_data[DATA_SET_PER_FILE] = {0x0000, 0x0000, 0x0000, 0x0000};
    float volt_data[DATA_SET_PER_FILE] = {0.00, 0.00, 0.00, 0.00};
    float temp_data[DATA_SET_PER_FILE] = {0.00, 0.00, 0.00, 0.00};

    float therm_res = 0.0;

    /*
    ** Clear Buffers
    */
    memset(in_fn, '\0', MAX_IN_FILENAME_LEN+1);
    memset(out_fn, '\0', MAX_OUT_FILENAME_LEN+1);

    /*
    ** Get arguments
    */
    snprintf(in_fn, MAX_IN_FILENAME_LEN+1, "%s", input_file);
    snprintf(out_fn, MAX_OUT_FILENAME_LEN+1, "%s", output_file);

    /*
    ** Open Input File
    */
    if( (in_fd = open(in_fn, O_RDONLY, in_mode)) < 0)
    {
        fprintf(stderr, "Couldn't open input file [%s].\n", in_fn);
        exit(1);
    }

    /*
    ** Read in values 1 word at a time
    */
    for(set_index = 0; set_index < DATA_SET_PER_FILE; set_index++)
    {
        /*
        ** Collect temperature data
        */
        if(set_index == 0)
        {
            read(in_fd, raw_byte_data[set_index], 1);
            fprintf(stderr, "Skipping one byte in first temp value!\n");
            printf("Raw Byte Read: \n");
            printf("Byte 0 (MSB): [%.2X]\n", raw_byte_data[set_index][0]); 
        }
        else
        {
            read(in_fd, raw_byte_data[set_index], 2);

            printf("Raw Two Bytes Read: \n");
            printf("Byte 0 (MSB): [%.2X]\n", raw_byte_data[set_index][0]);
            printf("Byte 1 (LSB): [%.2X]\n", raw_byte_data[set_index][1]);
                    
        }

        printf("Hint: [0x7F] often means the thermistor was D/C'd.\n");
        printf("\n");


        /*
        ** Check comma and space 
        */
        //lseek(in_fd, 2, SEEK_CUR);
        read(in_fd, comma_space, 2);

        

        if((comma_space[0] != 0x2C) || (comma_space[1] != 0x20))
        {
            fprintf(stderr, "Failed comma/space validation check.\n");
            fprintf(stderr, "Raw Two Bytes Read: \n");
            fprintf(stderr, "Byte 0 (Expecting Comma [2C]): [%.2X]\n", comma_space[0]);
            fprintf(stderr, "Byte 1 (Expecting Space [20]): [%.2X]\n", comma_space[1]);
        }
        else
        {
            /*
            ** Pack the raw two bytes into a word (prep for calculations)
            */
            //printf("raw_byte_data[%d][0] = [%.2X]; raw_byte_data[%d][1] = [%.2X]\n", set_index, raw_byte_data[set_index][0], set_index, raw_byte_data[set_index][1]);
            
            word_data[set_index] = (uint16_t) ((uint16_t)(raw_byte_data[set_index][0] << 8) + (uint16_t)raw_byte_data[set_index][1]);

            printf("ADC Word Reg [%d]: [%.4X]\n", set_index, word_data[set_index]);
        }
    }
    
    /*
    ** Close input file
    */
    close(in_fd);

    /*
    ** Convert all values
    */
    for(set_index = 0; set_index < DATA_SET_PER_FILE; set_index++)
    {
        /*
        ** Convert 16-bits to floating point voltage value
        */
        volt_data[set_index] = word_data[set_index] * 4.096 / 32767.0;

        printf("Thermistor %d Voltage = %f Volts\n", set_index, volt_data[set_index]);


        /*
        ** Calculate thermistor's resistance
        */
        therm_res = SERIES_RES / (((ADS_GAIN - 1) / volt_data[set_index]) - 1);
        printf("Thermistor %d Calculated Resistance = %.6f Ohms\n", set_index, therm_res);


        /*
        ** Convert thermistor resistance to temperature
        ** using steinhart formula found at:
        ** https://learn.adafruit.com/themistor/using-a-thermistor
        */
        temp_data[set_index] = therm_res / THERM_NOM_RES;
        temp_data[set_index] = (float) log( (double)temp_data[set_index] );
        temp_data[set_index] = temp_data[set_index] / THERM_NOM_BCOEF;
        temp_data[set_index] = temp_data[set_index] + (1.0 / (THERM_NOM_TEMP + KELVIN_OFFSET));
        temp_data[set_index] = 1.0 / temp_data[set_index];
        temp_data[set_index] = temp_data[set_index] - KELVIN_OFFSET;   /* convert to C */

        printf("Thermistor %d Calculated Temperature = %f degrees Celsius\n\n", set_index, temp_data[set_index]);
        
    }

    /*
    ** Open output file
    */
    if( (out_fd = open(out_fn, O_WRONLY | O_CREAT | O_TRUNC, out_mode)) < 0)
    {
        fprintf(stderr, "Couldn't open output file [%s].\n", out_fn);
        exit(1);
    }

    /*
    ** Write values
    */
    for(set_index = 0; set_index < DATA_SET_PER_FILE; set_index++)
    {
        //printf("Attempting to write Temp Data %d to file.\n", set_index);

        //printf("Size of temp data [sizeof(float)]: %lu\n", sizeof(temp_data[set_index]));

        write(out_fd, (uint8_t *) &temp_data[set_index], sizeof(temp_data[set_index]));

        //printf("Thermistor %d = [%.8X] or %f deg C written to file.\n", set_index, (uint32_t) temp_data[set_index], temp_data[set_index]);
        
        //printf("Thermistor %d =  %f deg C written to file.\n", set_index, temp_data[set_index]);

        /*
        ** Add comma and space for fomatting
        */
        comma_space[0] = 0x2C;
        comma_space[1] = 0x20;
        write(out_fd, comma_space, 2);
    }


    /*
    ** Close output file
    */
    close(out_fd);

    return(0);
}


