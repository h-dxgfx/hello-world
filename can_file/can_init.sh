#!/bin/bash

sudo ip link set down can0
sudo ip link set can0 type can bitrate 250000
sudo ip link set can0 up
