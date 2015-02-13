#!/bin/sh

echo "ssh-keygen -t rsa"

rm -rf ~/.ssh
ssh-keygen -t rsa

cat ~/.ssh/id_rsa.pub >~/.ssh/authorized_keys
chmod g-w ~/.ssh/authorized_keys
