#!/bin/bash

function noVar(){
    local var="$1"
    if test -z "$var"
    then
        [[ $( echo "1" ) ]]
        return

    elif [ "$var" == 0 2> /dev/null ]
    then
        [[ $( echo "1" ) ]]
        return

    elif [ "$var" == 0.0 2> /dev/null ]
    then
        [[ $( echo "1" ) ]]
        return
    fi
    [[ $( echo "" ) ]]
}

function WriteFile() {

if ! [ -f $1 ] ; then

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: не вдалося створити файл" >&2 
else
	echo "ERROR: can't create the file" >&2
fi

exit -1
fi

#***************func for print help 
function help(){

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "Вхідні параметри: ./task1.sh [-h|--help] [-n num] [file]" 
	echo "num - кількість файлів з системною інформацією,"
	echo "file - шлях та ім'я файлу, для запису результату;"

else
	echo "Input parameter: ./task1.sh [-h|--help] [-n num] [file]" 
	echo "num - number of files with system information,"
	echo "file - path and file name for write output;"
fi

}

#path="/home/vladyslav/Документы/output.txt"

#********Date info*************
date -R | sed 's/^/Date: /'# >> $1
echo "---- Hardware ----" >> $1


#********cpu info***************
cpu="$(grep -m 1 "model name" /proc/cpuinfo)"
echo "CPU: \"${cpu:13}\"" >> $1


#*********ram info**************
unset ramarr
ramarr=( $(sudo dmidecode -t memory | grep Size | awk '{print $2}') )
ram=0
for i in "${ramarr[@]}" ;
do 
if [[ "$i" -ne "No" ]] ; then
ram="$(expr $ram + $i)"
fi
done
echo "RAM: ${ram} MB" >> $1


#*********motherboard***********
manufact="$(dmidecode -t 2 | grep "Manufacturer")"
manufact=${manufact:15}

if (noVar $manufact) ;then
	manufact="$Unknown"
fi

prodname="$(dmidecode -t 2 | grep "Product Name")"
prodname=${prodname:15}

if (noVar $prodname) ;then
	prodname="$Unknown"

fi
serialnumb="$(dmidecode -t 2 | grep "Serial Number")"
serialnumb=${serialnumb:15}

if (noVar $serialnumb ] ;then
	serialnumb="Unknown"
fi

echo "Motherboard: \"$manufact\", \"$prodname\"" >> $1
echo "System Serial Number: $serialnumb" >> $1


echo "---- System ----" >> $1

#*************distr & kernel********
distr="$(grep "DISTRIB_DESCRIPTION" /etc/*-release)"
distr=${distr:20}

if [ -z "$distr" ] ;then
distr="Unknow"
fi

uname -r -m | sed 's/^/Kernel version: '/ >> $1

instaldate=""
dumpe2fs $(mount | grep 'on \/ ' | awk '{print $1}') | grep 'Filesystem created:' >> $instaldate
if (noVar $instaldate) ; then 
$instaldate="Unknown"
fi
echo $instaldate  >> $1

host="$(hostname -I | awk '{print $1}')"
if (noVar $host) ; then 
$host="Unknown"
fi
echo "Hostname: $host" >> $1

uptime -p | sed 's/.*up/Uptime: /' >> $1

#procnumb="$(ps ax | grep "httpd" | wc -c)"

procnumb="$(ps -A --no-headers | wc -l)"
if (noVar $procnumb) ; then
$procnumb="Unknown"
fi
echo "Processes runing: ${procnumb}" >> $1

usr="$(uptime | awk '{print $4}')"
usr=${users## }
if (noVar $usr) ; then 
$usr="Unknown"
fi
echo "User logged in: $usr" >> $1

echo "---- Network ----" >> $1

#*************network*************

1echo "$(ip -o -4 a | cut -d " " -f2,7)" >> $1

echo "----\"EOF\"----" >> $1

#********************parser

while [ -n "$1" ]
do

case "$1" in

-h) 
help
exit 0 
;;
--help)
help
exit 0
;;

-n)
shift

re='^[0-9]+$'
if ! [[ $1 =~ $re && $1 -gt 0 && $1 -le 1000 ]];	# check for integer val & var > 0
then

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: 'num' повинно бути цілим числом більшим за 1 !" >&2
else
	echo "ERROR: 'num' must be integer number higher than 1!" >&2
fi
exit 1
fi

num_files=$1
;;
*)
path=$1  					
if [ -z "$path" ] ; then 			
path="bash/task1.out"
cd ~						
shift
fi
;;

esac
shift
done



if (noVar $path) ; then
path="bash/task1.out"				
cd ~						
fi

dir_path=$(dirname $path)

if ! mkdir -p $dir_path ; then		

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: не вдалося створити папку!" >&2

else
	echo "ERROR: could not create folder!" >&2
fi

exit 3
fi

filename="${path##*/}"
filename="${filename%.*}"		
extension="${path##*.}"			

n=$(printf "%04d" 0)

cd $dir_path
oldname="$filename.$extension"
newname=$oldname

while (( 1 ))
do
if [ -f "$newname" ] ; then		
	newname="$filename-$(date +%Y%m%d)-$n.$extension"
	n="$(expr $n + 1)"
	n=$(printf "%04d" $n)
else
if [[ "$oldname" != "$newname" ]] ; then
		mv $oldname $newname 		
	fi
	break;
fi
done

#************ creating file
if ! touch $oldname; then	

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: не вдалося створити файл!" >&2

else
	echo "ERROR: could not create the file!" >&2
fi

exit 4
fi


(WriteFile $oldname)

FileList=( $(ls) ) 
len="${#FileList[*]}"
if [[ $num_files && $num_files -le $len ]] ; then
del="$(expr $len - $num_files)"
echo "Deleting $del files"
for (( i=0; i < del ; i++ ))
do
rm "${FileList[i]}"
done
fi

cd ~

exit 0

#**********The end of script
