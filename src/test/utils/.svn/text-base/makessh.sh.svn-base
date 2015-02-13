#!/bin/sh
for machine_name in `cat $1`
do
#	scp makesshauth.sh $machine_name:~/
 	ssh -o StrictHostKeyChecking=no $machine_name "mkdir -p ~/.ssh & rm ~/.ssh/authorized_keys"
 	ssh -o StrictHostKeyChecking=no $machine_name "ssh-keygen -t rsa"
	scp -o StrictHostKeyChecking=no $machine_name:~/.ssh/id_rsa.pub ~/.ssh/tmp.pub
	cat ~/.ssh/tmp.pub >>~/.ssh/authorized_keys
done

chmod 644 ~/.ssh/authorized_keys
chmod 700 ~/.ssh
chmod 711 /home/work
for machine_name in `cat $1`
do
	echo " 4.==> .ssh/authorized_keys ~/.ssh/auth.tmp "
	scp -o StrictHostKeyChecking=no ~/.ssh/authorized_keys $machine_name:~/.ssh/auth.tmp
	ssh -o StrictHostKeyChecking=no $machine_name "chmod 700 .ssh;cd .ssh;cat auth.tmp >>authorized_keys;chmod 644 authorized_keys; chmod 711 /home/work"
done
