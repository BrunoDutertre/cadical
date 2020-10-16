# Use colors if '<stdin>' is connected to a terminal.

color_echo_options=""

# BD: these code don't work for all terminals
#   BAD="\033[1;31m"	# bright red
#   HILITE="\033[32m"	# normal green
#   GOOD="\033[1;32m"	# bright green
#   HIDE="\033[34m"	# cyan
#   BOLD="\033[1m"      # bold color
#   NORMAL="\033[0m"	# normal color
#
# use tput instead below.

if [ -t 1 ]
then
  BAD=`tput bold; tput setaf 1`	 # bright red
  HILITE=`tput setaf 2`	         # normal green
  GOOD=`tput bold; tput setaf 2` # bright green
  HIDE=`tput setaf 6`	         # cyan
  BOLD=`tput bold`               # bold
  NORMAL=`tput sgr0`	         # normal

  if [ x"`echo -e 2>/dev/null`" = x ]
  then
    color_echo_options=" -e"
  fi
else
  BAD=""
  HILITE=""
  GOOD=""
  HIDE=""
  BOLD=""
  NORMAL=""
fi

cecho () {
  echo$color_echo_options $*
}
