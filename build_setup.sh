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
    log "========================================"
    log "$@"
    if [ $verbose ]; then
        "$@" 2>&1 | tee -a $LOG
    else
        "$@" 2>&1 >> $LOG
    fi
    rc=${PIPESTATUS[0]}
    if [[ $rc != 0 ]]; then
        log "Error, rc=$rc"
        if [[ $exit_on_fail == 1 ]]; then
            exit
        fi
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
    cd "$(dirname "$1")"
    printf "%s/%s\n" "$(pwd)" "$(basename "$1")"
    cd "$OLDPWD"
}

absdir()
{
    cd $1
    pwd
    cd $OLDPATH
}

#----------------------------------------
# Determine number of cores for compiling.
#----------------------------------------
case `uname` in
    Linux)
        last_core_index=`cat /proc/cpuinfo | grep "processor" | awk '{print $3}' | tail -1`
        num_cores=`nproc`
        hostname=$(hostname -s)
        ;;
    Darwin)
        num_cores=`sysctl -n hw.ncpu`
        hostname=$(hostname -s)
        ;;
    SunOS)
        num_cores=`kstat cpu_info | grep instance | wc -l`
        hostname=$(uname -n)
        ;;
    *)
        num_cores=1
esac

if ((num_cores<16)); then
    num_threads=$((num_cores/2))
else
    num_threads=$((num_cores*3/4))
fi

repo=$(absdir $(dirname $0))

user_install_prefix=$HOME/.local
system_install_prefix=/usr/local

python_exe=`which python2.7`

system_python_install="/usr/local"
user_python_install="~/.local"

git_hash=`cd $repo; git rev-parse HEAD`
git_branch=`cd $repo; git name-rev --name-only $git_hash`
git_branch=${git_branch%^0}
git_tag=`cd $repo; git name-rev --tags --name-only $git_hash`
git_tag=${git_tag%^0}

if [ "$git_tag" != "undefined" ]; then
    build_root=$repo/build-$hostname-$git_tag
else
    build_root=$repo/build-$hostname-$git_branch
fi
