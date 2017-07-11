#!/bin/bash
#----------------------------------------
#
# Purpose: routines used by build.sh to set up the environment

# The log file $LOG should go in the build directory. This can be
# reassigned after the command line argumens have been parsed. Also
# that dir can be clean out if the -c option is given.
# Don't call this function until after the above is done.
function log
{
    echo "$@" | tee -a $LOG
}

function run
{
    log "============================================================"
    log "$@"
    if [ $verbose ]; then
        "$@" 2>&1 | tee -a $LOG
    else
        "$@" 2>&1 >> $LOG
    fi
    rc=${PIPESTATUS[0]}
    if [[ $rc != 0 && -z $ignore_failures ]]; then
        log 
        log "Error $rc :-("
        log "See $build_root/Testing/Temporary/LastTest.log for detailed test log"
        log "See $LOG for detailed build log"
        exit $rc
    fi
    return $rc
}

# helper function: Print True Or False
function ptof {
    if [[ -z $1 ]]; then printf "False";
    else               printf "True";
    fi
}


abspath()
{
    if [ -d "$1" ]; then
        (cd "$1"; pwd)
    elif [ -f "$1" ]; then
        if [[ $1 == */* ]]; then
            echo "$(cd "${1%/*}"; pwd)/${1##*/}"
        else
            echo "$(pwd)/$1"
        fi
    else
        # It doesn't exist...
        if [[ ${1:0:1} == "/" ]]; then
            printf "$1"
        elif [[ ${1:0:2} == "./" ]]; then
            printf "$(pwd)/"${1:2}
        elif [[ ${1:0:2} == "~/" ]]; then
            printf "$(HOME)/"${1:2}
        elif [[ ${1:0:3} == "../" ]]; then
            printf "$(cd ..;pwd)/"${1:3}
        else
            printf $(pwd)/${1}
        fi
    fi
}


#----------------------------------------
# Determine number of cores for compiling.
#----------------------------------------
case `uname` in
    Linux)
        last_core_index=`cat /proc/cpuinfo | grep "processor" | awk '{print $3}' | tail -1`
        ((num_cores=last_core_index+1))
        hostname=$(hostname -s)
        ;;
    Darwin)
        num_cores=`sysctl -n hw.ncpu`
        hostname=$(hostname -s)
        ;;
    SunOS)
        num_cores=`psrinfo | wc -l`
        hostname=$(uname -n)
        ;;
    MINGW32_NT-6.1)
        hostname=$(uname -n)
        ;;     
    *)
        num_cores=1
esac

if ((num_cores<8)); then
    num_threads=$num_cores
elif ((num_cores<16)); then
    num_threads=$((num_cores - 2))
else
    num_threads=$((num_cores*3/4))
fi

repo=$(abspath $(dirname "$0"))

user_install_prefix=$HOME/.local
system_install_prefix=/usr/local

python_exe=`which python`

system_python_install="/usr/local"
user_python_install="$HOME/.local"

function hashit
{
    case `uname` in
        Linux)
            echo $1 | shasum
            ;;
        Darwin)
            echo $1 | shasum 
            ;;
        SunOS)
            echo $1 | shasum
            ;;
    esac
}


function get_repo_state
{
    git_hash=`cd $1; git rev-parse HEAD`
    git_tag=`cd $1; git name-rev --tags --name-only $git_hash`
    git_tag=${git_tag%^0}
    git_branch=`cd $1;git branch --no-color 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/' | sed -e 's/(detached from \(.*\))/\1/'`
    
    if [ "$git_tag" != "undefined" ]; then
        printf $git_tag
    elif [ -n "$git_branch" ]; then
        printf $git_branch
    else
        printf ${git_hash:0:7}
    fi
}

build_root=$repo/build/$hostname-$(get_repo_state $repo)
