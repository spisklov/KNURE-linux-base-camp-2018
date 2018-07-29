#!/bin/bash
filenum=(0 0 0 0)
num_err="files count must be greater then 1"
key_err="$1 is not a valid key"
num=1
numex=0
path=~/bash/task1.out
dir=~/bash
file=task1.out
help=0
date=$(date '+%Y%m%d')

function create { # $1 = $newname, $2 = $date, $3 = $file
  newname=$1
  local dat=$2
  local fil=$3
#echo "creating $newname"
  touch $newname
  print_all_info >$newname
  filenum[3]=$(( ${filenum[3]} + 1 ))
  for (( i=2; i>=0; i-- ))
  do
    filenum[$i]=$(( ${filenum[$i]} + (${filenum[$(( $i + 1 ))]} / 10) ))
  done
  filenum[3]=$(( ${filenum[3]} % 10 ))
  newname="$fil-$dat-${filenum[0]}${filenum[1]}${filenum[2]}${filenum[3]}"
}

function findfreename {
  while [ -e $newname ]
  do
#echo "$newname is already taken"
    filenum[3]=$(( ${filenum[3]} + 1 ))
    for (( i=2; i>=0; i-- ))
    do
      filenum[$i]=$(( ${filenum[$i]} + (${filenum[$(( $i + 1 ))]} / 10) ))
    done
    filenum[3]=$(( ${filenum[3]} % 10 ))
    newname="$file-$date-${filenum[0]}${filenum[1]}${filenum[2]}${filenum[3]}"
  done
#echo "$newname is free"
}

function cleardir {
#echo "clearing $dir"
  local direct=$1
  if [ $(ls $direct | wc -w) -ne 0 ]
  then
    rm $direct/*
  fi
}

function createfiles { 
#echo "creating $num new files"
  cleardir $dir
  filenum=(0 0 0 0)
  for (( j=0; j<$num; j++ ))
  do
    newname="$file-$date-${filenum[0]}${filenum[1]}${filenum[2]}${filenum[3]}"
    create $newname $date $file
  done
}

function preparedir {
  dir=$1
#echo "preparing $dir"
  if ! [ -d $dir ]
  then
    mkdir -p $dir
  fi

  if ! [ -d $dir ]
  then
    exit 2
  fi

  cd $dir
}

function print_help
{
  echo "$0 [-h|--help] [-n num] [file]"
  echo -e "\t-n sets number of files with results which should be kept"
  echo -e "\t-h|--help prints this help"
}

function parseargs {
  local filex=0
  while [ -n "$1" ]
  do
#echo "$1 read"
    case "$1" in
      -h|--help) print_help "$@"
      exit 0 ;;
      -n) shift
      if [ "$1" -lt 1 ]
      then
        echo $num_err >&2
        exit 1 
      fi
      num=$1
      numex=1
#echo "-n key read, value $num"
      ;;
      *)
      
      if [ $filex -eq 1 ]
      then 
        echo $key_err >&2
        exit 3
      fi
      filex=1
      path=$1 
#echo "path: $path"
;;
    esac
  shift
  done

#echo "specified path: $path"
  file=$(basename $path)
  dir=$(dirname $path)
#echo "file: $file   dir: $dir"
}

function print_cpu {
  cpuline=$(lscpu | grep "Model name")
  echo ${cpuline/#"Model name"/"CPU"}
}

function print_mem {
  mem=$(( $(cat /proc/meminfo | grep "MemTotal" | awk '{print $2}') / 1024 ))
  echo "RAM: $mem MB"
}

function print_mboard {
  mboard="Motherboard: \"$(sudo dmidecode -s baseboard-manufacturer)\", \"$(sudo dmidecode -s baseboard-product-name)\""
  echo $mboard
}

function print_serial_num {
  serialnum="System Serial Number: $(sudo dmidecode -s system-serial-number)"
  echo $serialnum
}

function print_hardware {
  echo ----Hardware----
  print_cpu
  print_mem
  print_mboard
  print_serial_num
}

function print_distrib {
  distrib=$(cat /etc/*-release | grep DISTRIB_DESCRIPTION)
  echo "OS Distribution: ${distrib##*=}"
}

function print_ker_ver {
  echo "Kernel version: $(uname -r)"
}

function print_install_date {
  created=$(sudo tune2fs -l $(df / | tail -1 | cut -f1 -d' ') | grep created)
  echo ${created/#"Filesystem created"/"Installation date"}
}

function print_hostname {
  echo "Hostname: $(cat /proc/sys/kernel/hostname)"
}

function print_uptime {
  uptime=$(cat /proc/uptime)
  upsecs=${uptime%%.*}  
  upmins=$(( $upsecs / 60 ))
  uphours=$(( $upmins / 60 ))
  upmins=$(( $upmins % 60 ))
  printf "Uptime: %d:%02d\n" $uphours $upmins
}

function print_proc_count {
  proc_count=$(ps -e | wc -l)
  echo "Processes running: $proc_count"
}

function print_users_count {
    users_count=$(w -hs | wc -l)
  echo "Users logged in: $users_count"
}

function print_system_info {
  echo ----System----
  print_distrib
  print_ker_ver
  print_install_date
  print_hostname
  print_uptime
  print_proc_count
  print_users_count
}

function print_network {
  echo ----Network----
  list=$(ip a | grep "inet " | awk '{print $NF ": ", $2, "\n"}')
  counter=1
  for word in $list
  do
    if [ $(( $counter % 2 )) -eq 0 ]
    then
    echo "$word"
    else
    echo -n "$word "
    fi
    counter=$(( $counter + 1 ))
  done
}

function print_all_info {
  echo "Date: $(date)"
  print_hardware
  print_system_info
  print_network  
  echo "----EOF----"
}

function print_info_to_files {
  if [ $numex -eq 1 ] 
  then 
    createfiles $date $file $dir
  else 
    if [ -e $file ]
    then
      newname="$file-$date-${filenum[0]}${filenum[1]}${filenum[2]}${filenum[3]}"
      findfreename $newname
      mv $file $newname
    fi
    touch "$file"
    print_all_info >$file
  fi
}

parseargs $*
preparedir $dir
print_info_to_files

