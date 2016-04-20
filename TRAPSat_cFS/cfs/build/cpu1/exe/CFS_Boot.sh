#!/bin/bash

### BEGIN INIT INFO
# Provides:          CFS_Boot
# Required-Start:    $all
# Required-Stop:     $all
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Starts the CFS
# Description:       script to start the CFS on boot with the 
#                    daemon command. To setup the script type 
#                    "sudo ./CFS_Boot.sh setup" 
#
### END INIT INFO

# Change the next 3 lines to suit where you install your script and what you want to call it
DIR=/home/pi/TRAPSat/TRAPSat_cFS/cfs/build/cpu1/exe
DAEMON=$DIR/core-linux.bin
DAEMON_NAME=CFS_Boot

# Change the next 3 lines to suit log location  
LOG=$DIR/ram/logs
OUT=$LOG/$DAEMON_NAME.out
ERR=$LOG/$DAEMON_NAME.err
DBG=$LOG/$DAEMON_NAME.dbg

#checks for log dir
if [ ! -d "$LOG" ]; then
   mkdir -p "$LOG"
fi

# This next line determines what user the script runs as.
# Root generally not recommended but necessary if you are using the Raspberry Pi GPIO from Python.
DAEMON_USER=root

# The process ID and priority of the script when it runs is stored here:
  PIDFILE=/var/run/$DAEMON_NAME.pid

. /lib/lsb/init-functions

do_start () {
    # fix daemon group write error -- maybe this should be in start? not sure
    sudo chmod g-w $DAEMON

    # Increment exe/ram/logs/reboot.txt count
    log_daemon_msg "Incrementing reboot file"
    printf "%03d" $(expr $(cat $LOG/reboot.txt) + 1) > $LOG/reboot.txt
    log_end_msg $?

    # Go to execution dir and start
    log_daemon_msg "Starting system $DAEMON_NAME daemon"
    cd $DIR
    (sudo ./core-linux.bin 1>>$OUT 2>>$ERR)&

    # Send Process ID to PID File
    sudo echo $! > $PIDFILE
    log_end_msg $?

    echo "cFS Starting with PID $(cat $PIDFILE)"
}

do_stop () {
    log_daemon_msg "Stopping system $DAEMON_NAME daemon"
    sudo kill $(cat $PIDFILE)
    log_end_msg $?
}

do_reset_boot_count () {
    log_daemon_msg "Clearing reboot file to 000"
    printf "000" > $LOG/reboot.txt
    log_end_msg $?
}

do_del_logs () {
    log_daemon_msg "Deleting logs files"
    sudo rm -f $DIR/ram/logs/CFS_Boot.*
    log_end_msg $?
}

do_del_data () {
    log_daemon_msg "Deleting Images"
    sudo rm -f $DIR/ram/images/*.jpg
    log_end_msg $?

    log_daemon_msg "Deleting Temperature Files"
    sudo rm -f $DIR/ram/temps/*.csv
    log_end_msg $?
}

set(){
    log_daemon_msg "Setting up $DAEMON_NAME"
    sudo cp CFS_Boot.sh /etc/init.d/CFS_Boot.sh
    cd /etc/init.d/
    sudo insserv $DAEMON_NAME.sh
    log_end_msg $?
}

case "$1" in

    start|stop|del_data|reset_boot_count|del_logs)
        do_${1}  
        ;;

    restart|reload|force-reload)
        do_stop
        do_start
        ;;

    setup)
        set
        ;;

    status)
        status_of_proc "$DAEMON_NAME" "$DAEMON" && exit 0 || exit $?
        ;;

    *)
        echo "Usage: /etc/init.d/$DAEMON_NAME {start|stop|setup|restart|status|del_data|reset_boot_count|del_logs}"
<<<<<<< HEAD
=======

>>>>>>> 6cb622b7b83d95d135fa7ec0c62141aa170c8c02
        exit 1
        ;;

esac
exit 0

## Old Required-Start:    $local_fs $time $network $syslog
## Old Required-Stop:     $local_fs $time $network $syslog

#do_start
    #typeset -i index=$(cat $LOG/reboot.txt)
    #read index < $LOG/reboot.txt
    #start-stop-daemon --stop --pidfile $PIDFILE --retry 10
    #daemon --pidfile=$PIDFILE --user $DAEMON_USER --foreground --stdout=$OUT --stderr=$ERR -- $DAEMON
    #declare -i tmp=index+1
    #printf "%03d" $tmp > $LOG/reboot.txt
