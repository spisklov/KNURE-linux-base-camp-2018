key_err="$1 is not a valid key"
target="task1.sh"
target_dir="/usr/local/bin"
current_dir=$(pwd)

while [ -n "$1" ]
do
  case "$1" in
    -h|--help) echo HELP PAGE
    exit 0 ;;
    *) echo $key_err >&2
    exit 1 ;;
  esac
  shift
done

if [ -d $target_dir ]
then
  sudo mkdir -p $target_dir >&2
  if [ $? -ne 0 ]
  then
    exit 1
  fi 
fi

sudo cp $target $target_dir
cd $target_dir
sudo chown root $target
sudo chmod 755 $target
alias task1.sh='/usr/local/bin/task1.sh'
