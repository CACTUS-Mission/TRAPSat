#! /bin/bash

executable='./PostOpTempConv'

cur_dir=`pwd`
format='/[0-9][0-9][0-9]_t_[0-9][0-9][0-9][0-9][0-9].csv'

filebank=$cur_dir$format

# echo $cur_dir

#for filecheck in $filebank
#do
#    if [ `expr match "$filecheck" '\]'` ]
#        then
#        echo "Found improper character, assuming no csv files in dir"
#        exit
#    fi
#done;


for filepath in $filebank
do
    #start_ind=`expr match "$filename" '*/*.csv'`
    #echo $start_ind

    echo $filepath

    filename=${filepath:(-15)}

    if [ `expr match "$filename" '[0-9][0-9][0-9]_t_[0-9][0-9][0-9][0-9][0-9].csv'` ]
        then
        echo $filename

        $executable $filename
    fi
    
    
done;
