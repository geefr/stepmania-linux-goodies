# Stepmania Linux goodies

This repo contains several little goodies for use with Stepmania on Linux

## Polling issues
If you're reading this you're most likely interested in fixing your polling rate issues. Head over to the [wiki](https://github.com/geefr/stepmania-linux-goodies/wiki/So-You-Think-You-Have-Polling-Issues) and have a read.
If the wiki information doesn't make sense give me a bell or raise an issue here to track your problems.

The release list contains some pre-built kernels that can increase your polling rate, but please read the wiki first.
In most cases/going forward you shouldn't need to swap out your kernel as Linux mainline includes polling options for
all kinds of input devices.

If you're looking to verify polling outside stepmania, see [evhz](https://github.com/geefr/evhz) and the [relevant wiki page](https://github.com/geefr/stepmania-linux-goodies/wiki/evhz-usage)

## arduino-sextetstreamreader
This folder contains stuff to help drive cab lights using an arduino.
See the [sextetstream readme](https://github.com/stepmania/stepmania/blob/master/src/arch/Lights/LightsDriver_SextetStream.md) for more information on this setup
Note: At least initially I only wrote the arduino firmware to support my single-pad setup.

## arduino-fsr
The Arduino code for my FSR-modded pad.
This has a couple of neat features, but it's incomplete. The pygui is barely a proof of concept.

If you're looking for pad code best to look elsewhere, or message me first if you want to use this for something.

# Licencing
In short everything is licenced as per the Linux kernel itself (GPL 2)

If that's a problem for your usage then get in touch, and in most cases i'll re-licence the needed parts to something more permissive

