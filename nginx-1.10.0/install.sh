#!/bin/bash 

NOCACHE_PATH=/home/MOMO/nocache/nginx-1.10.0
CACHE_PATH=/home/MOMO/servers/nginx-1.10.0

function makeinstall()
{
    echo "make..."
    make >/dev/null #2>&1
    echo "make install..."
    make install >/dev/null #2>&1
}
function nocache_config()
{
    echo "configure..."
    ./configure --with-debug  --prefix=$NOCACHE_PATH --add-module=../nginx-rtmp-module-1.1.10 >/dev/null #2>&1
}

function cache_config()
{
    echo "configure..."
    ./configure --with-debug  --prefix=$CACHE_PATH --add-module=../nginx-rtmp-module-1.1.10 >/dev/null #2>&1
}

function help_info()
{    
    echo "##### ERROR try:$0 [nocache|cache]"
}

if [ $# == 0 ] 
then
    makeinstall
elif [ $# == 1 ]
then 
    case $1 in
    "nocache")
        echo "##### install in $NOCACHE_PATH"
        nocache_config
        makeinstall
        ;;
    "cache")
        echo "##### install in $CACHE_PATH"
        cache_config
        makeinstall
        ;;
    "all")
        echo "##### install in $NOCACHE_PATH"
        nocache_config
        makeinstall
        echo "##### install in $CACHE_PATH"
        cache_config
        makeinstall
        ;;
    *)
        help_info
        ;;
    esac
else
    help_info
fi
