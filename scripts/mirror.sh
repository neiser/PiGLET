#!/bin/bash
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

shopt -s dotglob

rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-01:$1/ &
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-02:/ &
wait
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-03:$1/ &
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-04:$1/ &
wait
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-05:$1/ &
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-06:$1/ &
wait
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-07:$1/ &
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-08:$1/ &
wait
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-09:$1/ &
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-10:$1/ &
rsync "${OPTIONS[@]}" $1/* a2onlinedisplay-11:$1/ &
wait
echo "DONE!"
