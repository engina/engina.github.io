#!/bin/bash

##################################################
#               Little QoS script
#
# Author : Engin AYDOGAN
# Purpose: Avoid enourmous latency times when
#          uploading big files.
# How    : It marks big packets ( they are being
#          used by big file uploads ) and gives
#          them a limited bandwidth and low prioritry
# Wish   : An inverse deleting of iptables rules
#          ( Delete each rule appended )
#          Delete queue discs
##################################################

# Upload bandwidth for big file uploads ( in kilobits )
upl=32

# Upload bandwidth for interactive communication
# ( in kilobits )
uph=64

# Device which will create queue discs on
dev=eth1

#######################
# Do not edit below unless you know what you're doing
#######################

# Delete anything which can interfere ( is this necessary? )
iptables -F -t mangle > /dev/null 2> /dev/null
tc qdisc del dev $dev root > /dev/null 2> /dev/null
if [ "$1" == "stop" ]
        then
        echo "iptables and QoS rules are deleted"
        exit 0
fi
# Mark the packets

# Mark the packets with the size range of 0-500 bytes with 3
iptables -t mangle -A OUTPUT -m length --length 0:500 -j MARK --set-mark 3

# Mark the packets with the size range of 500-10000 bytes with 4
# Actually it is non sense to make the range 500-10000 since MTU is
# usually 1500 on eth0, but somehow if we do it 500:1500 it does not
# work in kernel 2.6.2, it seems there is something broken.

iptables -t mangle -A OUTPUT -m length --length 500:10000 -j MARK --set-mark 4

# If packets are for local traffic mark them with 5
iptables -t mangle -A OUTPUT -d 192.168.1.1/255.255.255.0 -j MARK --set-mark 5

# Create queue disc
tc qdisc add dev $dev root handle 10: cbq bandwidth 10Mbit avpkt 1000 mpu 64

# Bandwidth with high prioritry
tc class add dev $dev parent 10:0 classid 10:1 cbq bandwidth 10Mbit \
    rate "$uph"Kbit allot 1514 prio 1 maxburst 10 avpkt 100 isolated \
    weight 1Mbit
# Bandwidth with low prioritry
tc class add dev $dev parent 10:0 classid 10:2 cbq bandwidth 10Mbit \
    rate "$upl"Kbit allot 1514 prio 8 maxburst 2 avpkt 1500 bounded \
    weight 1Mbit

# Bind marked packets to classes.

tc filter add dev $dev parent 10:0 protocol ip handle 3 fw flowid 10:1
tc filter add dev $dev parent 10:0 protocol ip handle 4 fw flowid 10:2

echo "I've attempted to insert necessary iptables and QoS rules"

