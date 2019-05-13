#!/bin/bash

clear
echo "Hello"
read -p "Please Enter Your Names:" name
echo "$name" >> names.txt
clear
echo -e "Hellow $name\nYour names as been added to the list."
echo "=========================="
cat names.txt

echo "========================="
echo "Goodbye $name"
sleep 2
