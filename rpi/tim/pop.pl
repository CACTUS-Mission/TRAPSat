#! /usr/bin/perl
####################################################################
#
# Post Operations Parser
#
# TRAPSat RockSat-X 2016
#
#####################################################################
use strict; use warnings;

#
# subroutine proto's
#
sub readnshift;

#
# Data File Defs
#
use constant IMAGE_FILENAME_LEN => 14; # 000_0_0000.jpg
use constant TEMPS_FILENAME_LEN => 15; # 000_t_00000.csv
use constant LOGS_FILENAME_LEN => 12; # CFS_Boot.out
use constant LOGS_FILENAME => "CFS_Boot.txt"; # non-binary file extension

#
# Flag used before each index flag
#
use constant GEN_FILE_FLAG => "\xFF";

#
# Index flags:
#
use constant IMAGE_START_FLAG => "\xF2";
use constant IMAGE_STOP_FLAG => "\xF3";
use constant TEMPS_START_FLAG => "\xF4";
use constant TEMPS_STOP_FLAG => "\xF5";
use constant LOGS_START_FLAG => "\xF6";
use constant LOGS_STOP_FLAG => "\xF7";

#
# Main State Table
# 
use constant NO_DATA_FILE_FOUND => 0;
use constant IMAGE_FILE_FOUND => 1;
use constant TEMPS_FILE_FOUND => 2;
use constant LOGS_FILE_FOUND => 3;
my $main_state = NO_DATA_FILE_FOUND;

#
# New File State Table
# 
use constant NO_DATA_FILE_OPEN => 0;
use constant IMAGE_FILE_OPEN => 1;
use constant TEMPS_FILE_OPEN => 2;
use constant LOGS_FILE_OPEN => 3;
my $new_file_state = NO_DATA_FILE_OPEN;


if (not defined($ARGV[0]))
{
    print "\n\tExpected raw data as command line argument. Exiting.\n";
    exit;
}
#print "$ARGV[0]\n";
my $raw_data_filename = $ARGV[0]; # "test.data"; #  #
open(RAW_DATA_FILE, "<", $raw_data_filename) or die "Failed opening $raw_data_filename!";

my $new_data_filename = "parsed.txt";
open(NEW_DATA_FILE, ">>", $new_data_filename) or die "Failed opening $new_data_filename!";

my $image_filename;
my $temps_filename;
my $logs_filename;

our $filename_byte_count = 0;

my $total_bytes = 0;

my @raw_data_buff = ("\x00", "\x00");

while(1)
{
    #printf("Main State: %d\n", $main_state);
    readnshift();
    
    if($main_state == NO_DATA_FILE_FOUND) 
    {
        #
        # Check for image flags
        #
        if( ($raw_data_buff[0] eq GEN_FILE_FLAG) and ($raw_data_buff[1] eq IMAGE_START_FLAG) )
        {
            #printf("Found %02X %02X\n", ord GEN_FILE_FLAG, ord IMAGE_START_FLAG);
            #printf("Found Image Start Flag!\n");
            $main_state = 1;
        }
    
        #
        # Check for temp flags
        #
        elsif( ($raw_data_buff[0] eq GEN_FILE_FLAG) and ($raw_data_buff[1] eq TEMPS_START_FLAG) )
        {
            #printf("Found %02X %02X\n", ord GEN_FILE_FLAG, ord TEMPS_START_FLAG);
            #printf("Found Temp Start Flag!\n");
            $main_state = 2;
        }
    
        #
        # Check for log flags
        #
        elsif( ($raw_data_buff[0] eq GEN_FILE_FLAG) and ($raw_data_buff[1] eq LOGS_START_FLAG) )
        {
            printf("Found %02X %02X\n", ord GEN_FILE_FLAG, ord LOGS_START_FLAG);
            printf("Found Logs Start Flag!\n");
            $main_state = 3;
        }
        else
        {
            #
            # Store other data in new file
            #
            printf(NEW_DATA_FILE "%c", ord $raw_data_buff[1]);
        }
    }

    elsif($main_state == IMAGE_FILE_FOUND)
    {
        if(($filename_byte_count <= IMAGE_FILENAME_LEN) and ($new_file_state == NO_DATA_FILE_OPEN))
        {
            #
            # Get Temperature File Name
            #
            $image_filename = $image_filename.$raw_data_buff[1];

            $filename_byte_count++;
        }
        elsif(($filename_byte_count <= IMAGE_FILENAME_LEN+1) and ($new_file_state == NO_DATA_FILE_OPEN))
        {
            #
            # Ignore Single byte after filename
            #
            $filename_byte_count++;
        }
        else
        {   
            #
            # Open data file for writing
            #
            #printf("About to open filename [%s]\n", $image_filename);

            #
            # Set New File State
            #
            $new_file_state = IMAGE_FILE_OPEN;

            open(IMAGE_DATA_FILE, ">>", $image_filename) or die "Failed opening $image_filename!";

            #
            # Check for end flag
            #
            if( ($raw_data_buff[0] eq GEN_FILE_FLAG) and ($raw_data_buff[1] eq IMAGE_STOP_FLAG) )
            {
                #printf("Found IMAGE_STOP_FLAG!\n");

                #
                # We have now already written 1 character too far.
                # backtrack and write a new line over it.
                #
                seek(IMAGE_DATA_FILE, -1, 1);
                printf(IMAGE_DATA_FILE "\n");

                #printf("Image File Length: [%d] Bytes\n", tell(IMAGE_DATA_FILE));
                #printf("Total Bytes Read: [%d] Bytes\n", $total_bytes);

                #
                # Close the file
                #
                close(IMAGE_DATA_FILE);

                printf("Completed parsing image file: [ %s ]\n", $image_filename);

                #
                # Reset states, flags, and counts
                #
                $main_state = NO_DATA_FILE_FOUND;
                $new_file_state = NO_DATA_FILE_OPEN;
                $total_bytes = 0;
                $image_filename = "";


                #
                # Exit now, continue later
                #
                next;
            }

            #
            # We have the filename, now write data to file
            #
            $filename_byte_count = 0;

            #printf("Writing image data to file. [%c]\n", ord $raw_data_buff[1]);
            printf(IMAGE_DATA_FILE "%c", ord $raw_data_buff[1]);
        }
    }

    elsif($main_state == TEMPS_FILE_FOUND)
    {
        if(($filename_byte_count <= TEMPS_FILENAME_LEN) and ($new_file_state == NO_DATA_FILE_OPEN))
        {
            #
            # Get Temperature File Name
            #
            $temps_filename = $temps_filename.$raw_data_buff[1];

            $filename_byte_count++;
        }
        elsif(($filename_byte_count <= TEMPS_FILENAME_LEN+2) and ($new_file_state == NO_DATA_FILE_OPEN))
        {
            #
            # Ignore Single byte after filename
            #
            $filename_byte_count++;
        }
        else
        {   
            #printf("About to open filename [%s]\n", $temps_filename);
            #die;

            #
            # Set New File State
            #
            $new_file_state = TEMPS_FILE_OPEN;

            #
            # Open data file for writing
            #
            open(TEMPS_DATA_FILE, ">>", $temps_filename) or die "Failed opening $temps_filename!";

            #
            # Check for end flag
            #
            #if(ord @raw_data_buff[0,1] == ord(chr(GEN_FILE_FLAG).chr(TEMPS_STOP_FLAG)))
            if( ($raw_data_buff[0] eq GEN_FILE_FLAG) and ($raw_data_buff[1] eq TEMPS_STOP_FLAG) )
            {
                #printf("Found TEMP_STOP_FLAG!\n");

                #
                # We have now already written 1 character too far.
                # backtrack and write a new line over it.
                #
                seek(TEMPS_DATA_FILE, -1, 1);
                printf(TEMPS_DATA_FILE ";");

                #printf("Temp File Length: [%d] Bytes\n", tell(TEMPS_DATA_FILE));
                #printf("Total Read: [%d] Bytes\n", $total_bytes);

                #
                # Close the file
                #
                close(TEMPS_DATA_FILE);

                printf("Completed parsing temps file: [ %s ]\n", $temps_filename);

                #
                # Reset states, flags, and counts
                #
                $main_state = NO_DATA_FILE_FOUND;
                $new_file_state = NO_DATA_FILE_OPEN;
                $total_bytes = 0;
                $temps_filename = "";


                #
                # Exit for now, continue later
                #
                next;
            }


            #
            # We have the filename, now write data to file
            #
            $filename_byte_count = 0;

            #printf("Writing temps data to file. [%c]\n", ord $raw_data_buff[1]);
            printf(TEMPS_DATA_FILE "%c", ord $raw_data_buff[1]);
        }


    }

    elsif($main_state == LOGS_FILE_FOUND)
    {
        if(($filename_byte_count <= LOGS_FILENAME_LEN) and ($new_file_state == NO_DATA_FILE_OPEN))
        {
            #
            # Get Temperature File Name
            #
            $logs_filename = $logs_filename.$raw_data_buff[1];

            $filename_byte_count++;
        }
        elsif(($filename_byte_count <= LOGS_FILENAME_LEN+10) and ($new_file_state == NO_DATA_FILE_OPEN))
        {
            #
            # Ignore Single byte after filename
            #
            $filename_byte_count++;
        }
        else
        {   
            #
            # Open data file for writing
            #

            printf("Received log filename [%s]\n", $logs_filename);
            #die;

            #
            # Set New File State
            #
            $new_file_state = LOGS_FILE_OPEN;

            #open(LOGS_DATA_FILE, ">>", "CFS_Boot.txt") or die "Failed opening logs [$logs_filename]!";
            open(LOGS_DATA_FILE, ">>", LOGS_FILENAME) or die "Failed opening CFS_Boot.txt]!";

            #
            # Check for end flag
            #
            #if(ord @raw_data_buff[0,1] == ord(chr(GEN_FILE_FLAG).chr(IMAGE_STOP_FLAG)))
            if( ($raw_data_buff[0] eq GEN_FILE_FLAG) and ($raw_data_buff[1] eq LOGS_STOP_FLAG) )
            {
                printf("Found LOGS_STOP_FLAG!\n");

                #
                # We have now already written 1 character too far.
                # backtrack and write a new line over it.
                #
                seek(LOGS_DATA_FILE, -1, 1);
                printf(LOGS_DATA_FILE "\n");

                printf("Logs File Length: [%d] Bytes\n", tell(LOGS_DATA_FILE));
                printf("Total Bytes Read: [%d] Bytes\n", $total_bytes);

                #
                # Close the file
                #
                close(LOGS_DATA_FILE);

                #
                # Reset states, flags, and counts
                #
                $main_state = NO_DATA_FILE_FOUND;
                $new_file_state = NO_DATA_FILE_OPEN;
                $total_bytes = 0;
                $logs_filename = "";


                #
                # Exit now, continue later
                #
                printf("Finished parsing log!\n");
                die;
            }

            #
            # We have the filename, now write data to file
            #
            $filename_byte_count = 0;

            #printf("Writing log data to file. [%c]\n", ord $raw_data_buff[1]);
            printf(LOGS_DATA_FILE "%c", ord $raw_data_buff[1]);
        }
    }

    else
    {
        printf("Inside main state %d.\n", $main_state);
        die;
    }

    $total_bytes++;
}

#
# End of Program
#


########################################################################################

#
# Begin Function Definintions
#

sub readnshift
{
    my $raw_byte = "\x00";
    read(RAW_DATA_FILE, $raw_byte, 1) or die "Failed to read byte from file: $raw_data_filename.";
    
    #printf("Raw Byte: %02X\n", ord $raw_byte);
    #printf("Character: %c\n", ord $raw_byte);

    #
    # Shift raw data into our 'window' to view current stream
    #
    $raw_data_buff[0] = $raw_data_buff[1];
    $raw_data_buff[1] = $raw_byte;
}


#
# End of File
#
