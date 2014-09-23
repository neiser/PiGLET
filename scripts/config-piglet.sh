#!/bin/bash

FILE=$HOME/piglet-cfg/$HOSTNAME
PORT=1337
WAIT=2

echo "Feeding config..."

test -f "${FILE}" || { echo "${FILE} not found"; exit 1; }

sleep $WAIT

echo "."
until nc localhost $PORT < "${FILE}"
do
	echo "."
	sleep $WAIT
done

echo "OK"
