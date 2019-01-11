#!/usr/bin/env sh
mkfifo "./Data/StepMania-Lights-SextetStream.out" &> /dev/null
socat "./Data/StepMania-Lights-SextetStream.out" /dev/ttyACM0,raw,echo=0,b115200
