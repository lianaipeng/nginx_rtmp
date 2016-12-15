#!/bin/bash 

# ./stopproc.sh start > send.log &

STREAM=123hello
FILE=$0
COUNT=1

function _other()
{
   echo "ERROR $0 start|state|stop [process_name]"
}

function _kill()
{
    echo "kill------ $STREAM"
    PROCESS1=`ps -ef | grep $STREAM | grep -v grep | grep -v stopproc | awk '{ print $2}'`
    for i in $PROCESS1
    do
        echo "Kill the $STREAM process [ $i ]"
        kill -9 $i >/dev/null 2>&1 & 
    done
    echo "kill------ end"
}

function _send()
{
    while((1))
    do
        COUNT=$((COUNT+1))
        echo "send------ COUNT:$COUNT"
        SLEEP=$(($RANDOM%120))
        #ffmpeg -re -i "/home/MOMO/downloads/sources/x264long.flv" -vcodec copy -acodec aac -strict -2 -f flv  rtmp://192.168.104.168:1935/myapp/$STREAM >/dev/null  2>&1  &
        ffmpeg -re -i "/home/MOMO/downloads/sources/x264long.flv" -vcodec copy -acodec aac -strict -2 -f flv  rtmp://101.201.70.93:1935/myapp/$STREAM >/dev/null  2>&1  &
        echo "I will sleep $SLEEP second."
        sleep $SLEEP
        
        _kill 
    done
}

function _state()
{
    ps -ef | grep $FILE | grep -v grep 
    ps -ef | grep $STREAM | grep -v grep | grep -v stopproc
}

function _stop()
{
    _kill
    # 父进程杀死后 子进程将会变成孤儿进程
    
    echo "stop------"
    PROCESS=`ps -ef | grep $FILE | grep -v grep  | awk '{ print $2}'`
    for i in $PROCESS
    do
        echo "Kill the $FILE process [ $i ]"
        kill -9 $i >/dev/null 2>&1 & 
    done
    echo "stop------ end"
}


if [ $# -lt 1 ] 
then
    _other
    exit 1
fi
   
case $1 in
start)
    echo "##### start "
    _send
    ;;
state)
    echo "##### state"
    _state
    ;;
stop)
    echo "##### stop"  
    _stop
    ;;
*)
    _other
    exit 1;
    ;;
esac
