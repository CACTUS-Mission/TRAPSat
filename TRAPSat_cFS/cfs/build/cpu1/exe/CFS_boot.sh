#!/bin/bash

### BEGIN INIT INFO
# Provides:          CFS
# Required-Start:    $local_fs $time $network $syslog
# Required-Stop:     $local_fs $time $network $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Starts the CFS
# Description:       place file in "/etc/init.d". 
#                    use inssver command to install
#                    into rc?.d files 
### END INIT INFO

# Change the next 3 lines to suit where you install your script and what you want to call it
DIR=/home/pi/TRAPSat/TRAPSat_cFS/cfs/build/cpu1/exe
DAEMON=$DIR/core-linux.bin
DAEMON_NAME=CFS

# Change the next 3 lines to suit log location  
LOG=/home/pi/logs
OUT=$LOG/cfs.out
ERR=$LOG/cfs.err

#checks for log dir
if [ ! -d "$LOG"  ]; then
   mkdir -p "$LOG"
fi

# This next line determines what user the script runs as.
# Root generally not recommended but necessary if you are using the Raspberry Pi GPIO from Python.
DAEMON_USER=root

# The process ID and priority of the script when it runs is stored here:
PIDFILE=/var/run/$DAEMON_NAME.pid
NI=10

. /lib/lsb/init-functions

do_start () {
    log_daemon_msg "Starting system $DAEMON_NAME daemon"
    echo
    nice -n$NI daemon --pidfile=$PIDFILE --user $DAEMON_USER --foreground --stdout=$OUT --stderr=$ERR -- $DAEMON 
    #log_end_msg $?
}

do_stop () {
    log_daemon_msg "Stopping system $DAEMON_NAME daemon"
    start-stop-daemon --stop --pidfile $PIDFILE --retry 10
    #log_end_msg $?
}

case "$1" in

    start|stop)
        do_${1}  
        ;;

    restart|reload|force-reload)
        do_stop
        do_start
        ;;

    status)
        status_of_proc "$DAEMON_NAME" "$DAEMON" && exit 0 || exit $?
        ;;

    *)
        echo "Usage: /etc/init.d/$DAEMON_NAME {start|stop|restart|status}"
        exit 1
        ;;

esac
exit 0
