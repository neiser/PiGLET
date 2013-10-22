#!/bin/bash
ROOTDIR="/" # dont forget trailing slash
OPTIONS=(-avH --delete
    --exclude '/dev'
    --exclude '/proc'
    --exclude '/sys'
    --exclude '/tmp'
    --exclude '/run'
    --exclude '/mnt'
    --exclude '/media'
    --exclude '/lost+found'
    --exclude '/var/cache/apt/archives'
    --exclude '/boot'
    --exclude '/var/log'
)

rsync "${OPTIONS[@]}" /* a2onlinedisplay-01:/ &
rsync "${OPTIONS[@]}" /* a2onlinedisplay-02:/ &
wait
rsync "${OPTIONS[@]}" /* a2onlinedisplay-03:/ &
rsync "${OPTIONS[@]}" /* a2onlinedisplay-04:/ &
wait
rsync "${OPTIONS[@]}" /* a2onlinedisplay-05:/ &
rsync "${OPTIONS[@]}" /* a2onlinedisplay-06:/ &
wait
rsync "${OPTIONS[@]}" /* a2onlinedisplay-07:/ &
rsync "${OPTIONS[@]}" /* a2onlinedisplay-08:/ &
wait
rsync "${OPTIONS[@]}" /* a2onlinedisplay-09:/ &
rsync "${OPTIONS[@]}" /* a2onlinedisplay-10:/ &
rsync "${OPTIONS[@]}" /* a2onlinedisplay-11:/ &
wait
echo "DONE!"
