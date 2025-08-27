# fujinon-tv-lens-control (and probably other B4 Lenses too)

> [!NOTE]
> Discaimer:  
This is work in progress. Any information could be faulty or wrong. It's not finished yet

> [!NOTE]
> This is a fork of https://github.com/yasdfgr/fujinon-tv-lens-control

## Purpose

This is a collection of knowledge about the control interface of B4 tv lenses.  

This interface could be found on lenses from Canon and Fujinion. Canon Lenses also Provide a Digital interface to communicate with the Camera, it might use a Different Protocol, i havent tested it yet.

If you searching around you find only some few contributions in a bunch of different platforms. This is the result of my own research about this topic.

I'm focusing on Fujinion lenses in the further description.

## Contribiution
If you want to help completing the Reverse engeneering of this Protocol and have a Lens that we haven't yet dumped the commands, please consider running the "Check Commands" Program and donating the results.

## Goal
1. The goal of this Project is to reverse engeneer the Protocol used on the Hirose 12 Pin Plug between Camera and Lens.

2. Build a device that connects between any (full servo) ENG B4 Lens and the Camera to allow full digital Control over any Analog full servo Lens.

## Planned Features

### Core Features

1. Output analog voltages via external plugs for Zoom and Focus to univerally control any broadcast lens
2. Output analog voltage via 12pin Hirose to control Iris on any broadcast lens
3. Feedback of Iris zoom and Focus Position via digital Protocol 
4. Input for Analog Voltages to accept local control for Zoom and Focus control.
5. Storage for Multiple Lens data files, Lens selectable via DIP Switch
6. Option to upload additional Lens files via USB

### Optional Features
1. Output the known Fujinon L10 Protocol to Digitally control older Digipower lenses via the 10pin hirose Plug
2. Output the Tilta Protocol to support additional Motors for non full servo Lenses
3. Accept the Tilta Protocol to accept local control for zoom and focus via Tilta demands.
4. A-B Feature for setting a min/max for zoom and focus values to get finer local control
5. Support for Blackmagic zoom and focus demands (if someone decides to reverse engeneer their protocol, some bird told me its just usb with a special flavor)


## Foreword
During my Research i found the following:
The B4 mount and its connections were originally Standardized by ARIB.
The Standard  can be found  [here](https://www.arib.or.jp/english/std_tr/broadcasting/bta-s-1005.html).  
The Standard also includes different configurations for the commonly used Hirose 12pin plug.
Both with analog and Digital communication options.
The Digital Protocol described in the Standard does not match any communication or pinout ive seen used in real life, it seems it was abandoned at some point.
A newer Standard by the ARIB  [here](https://www.arib.or.jp/english/html/overview/doc/8-TR-B37v1_1-E2.pdf) describes the currently used Signal interface on a Hardware Level, but does not describe the digital Protocol used on Pin 11 and 12 of the connector.



### Fujinon RS232

During the Research i found a Protocol used by Fujinon to control their Lenses via an external RS232 Port.
This port uses a fully featured RS232 Connection on an Hirose 10pin Plug.
(See Documents in the "UART Protocol Docs" folder.
I also found a Demo Tool to control the lens with this Protocol:
<https://imagelabs.com/imaging-system-components/security/fujinon-lens-controller-software/>


 

## Credits

This research is done with a couple of very helpful tools:  

+ PicoScope 2204A (a USB oscilloscope) with corresponding software: <https://www.picotech.com/downloads>
+ PicoScope serial decoding: <https://www.picotech.com/library/oscilloscopes/rs-232-serial-protocol-decoding>
+ Pulseview, the signal and logic analyzer software of sigrok project: <https://sigrok.org/wiki/Downloads>
+ csv2vcd, a tool to convert PicoScope output to Pulseview: <https://github.com/feecat/csv2vcd>
+ USBProg v4.0, USB programmer and serial device (no more available), working with 3.3V and 5.0V UART signal levels
+ YAT (yet another terminal), a very good terminal application, especially for binary protocols: <https://sourceforge.net/projects/y-a-terminal/>

## Links

### Know-How

#### RS232

+ <https://www.best-microcontroller-projects.com/how-rs232-works.html>
+ <https://www.youtube.com/watch?v=sTHckUyxwp8>  
+ <https://www.youtube.com/watch?v=JuvWbRhhpdI>  

### Collection of links to similar contributions:  

#### BMD Forum

+ B4 lens communication protocol: <https://forum.blackmagicdesign.com/viewtopic.php?p=343280>
+ https://forum.blackmagicdesign.com/viewtopic.php?t=165607

#### Reddit

+ B4 tv lens digital control protocol: <https://www.reddit.com/r/videography/comments/1av053z/b4_tv_lens_digital_control_protocol/>

#### Arduino Forum, Blackmagic Forum


+ https://www.reddit.com/r/VIDEOENGINEERING/comments/14dpj38/b4_lens_database/

+ https://en.wikipedia.org/wiki/B4-mount

+ https://www.arib.or.jp/english/std_tr/broadcasting/desc/bta-s-1005.html
+ https://www.arib.or.jp/english/std_tr/broadcasting/bta-s-1005.html


+ https://forum.arduino.cc/t/inverted-serial-signals/12020/8

+ https://en.wikipedia.org/wiki/9-Pin_Protocol
+ https://ftp.zx.net.nz/pub/archive/ftp.sgi.com/sgi/video/rld/vidpage/s9pin.html

+ https://en.wikipedia.org/wiki/B4-mount


+ https://www.esbroadcast.com/product/used-fujinon-ha18x7-6berd-s48-hd-eng-lens/
+ https://forum.blackmagicdesign.com/viewtopic.php?f=2&t=164165&p=866679&hilit=serial+lens+control#p866679
+ https://forum.blackmagicdesign.com/viewtopic.php?f=4&t=134929&p=730986&hilit=serial+lens+control#p730986
+ https://forum.blackmagicdesign.com/viewtopic.php?f=4&t=84205
+ https://forum.blackmagicdesign.com/viewtopic.php?t=165607
+ https://forum.blackmagicdesign.com/viewtopic.php?f=2&t=171757&p=904192&hilit=fujinon+digital+analog#p904192
+ https://forum.blackmagicdesign.com/viewtopic.php?f=2&t=155504&p=952285&hilit=serial+lens+control#p952285
+ https://forum.blackmagicdesign.com/viewtopic.php?f=4&t=21610

+ https://support.cyanview.com/docs/Integrations/Lens/B4Lens

+ https://www.canon-europe.com/broadcast/tv_lenses/remote_control_lenses/

+ https://www.canon-europe.com/pro/broadcast/eng-efp-pro/

+ https://forum.blackmagicdesign.com/viewtopic.php?f=4&t=75905

+ https://www.dyxum.com/dforum/emount-electronic-protocol-reverse-engineering_topic119522.html
+ https://www.dpreview.com/forums/thread/3872069

## other Solutions
+ https://vimeo.com/738206774
+ https://www.linkedin.com/posts/levitezer_levitezer-b4-control-box-demonstration-activity-6963089260321099776-IrMd/

+ https://wiki.skaarhoj.com/books/device-core-articles/page/eth-b4-link-lens-control







