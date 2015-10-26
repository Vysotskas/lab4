#!/bin/sh

insmod fibonacci.ko || exit

rm -f /dev/fibonacci_input
rm -f /dev/fibonacci_result

mknod /dev/fibonacci_input c $(awk '$2=="fibonacci_input" {print $1}' /proc/devices) 0
mknod /dev/fibonacci_result c $(awk '$2=="fibonacci_result" {print $1}' /proc/devices) 0

chmod 666 /dev/fibonacci_input
chmod 444 /dev/fibonacci_result
