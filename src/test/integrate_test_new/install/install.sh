#!/bin/sh
#usage: sh install.sh $1
#such as:
######intall hudson environment
#sh intall.sh hudson 
#####intall press environment
#sh install.sh press
script=hudson
echo $#
if [ $# -ge 1 ]
then
	script=$1
	if [ ! -f ./install_${script}.py ]
	then
		echo "no install file: install_${script}"
		exit 0
	fi
fi
echo  install file install_${script}

####get svn new version
#python install_${script}.py -o download
#sh update_install.sh
#####
python install_${script}.py -o install

#####change monitor


#####
python  install_${script}.py -o start -m pm
python install_${script}.py -o start -m pn
python install_${script}.py -o start -m monitor
