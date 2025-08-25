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


 
## Pinout 12-pin B4 lens connector

This connector is a Hirose


|         | old style analog only (C)                                    | with serial digital control (B)                                                     |             |
| :----:  | ---------------                                           | -----------------------------------                                             | ----------- |
| **Pin**     | **Function**                                          | **Function**                                                                    | **Direction**   |
| 1       | RET SW (Return Video Select) (OFF: open ON: 0V)           | RET SW (Return Video Select) (OFF: open ON: 0V)                                 | Lens -> Cam |
| 2       | VTR SW (VTR Start/Stop) (NO ACTION: open START/STOP: 0V)  | VTR SW (VTR Start/Stop) (NO ACTION: open START/STOP: 0V)                        | Lens -> Cam |
| 3       | GND (0V)                                                  | GND (0V)                                                                        | Cam -> Lens |
| 4       | Iris Servo (OFF: 0V ON: 5V)                               | IRIS ENF AUTO (OFF: 0V ON: 5V)                                                  | Cam -> Lens |
| 5       | IRIS CONT SIG (Iris Control Signal)                       | IRIS CONT SIG (Iris Control Signal) (F2.8: 6,2V F16: 3,4V CLOSE: 2,5V)          | Cam -> Lens |
| 6       | POWER (+12V)                                              | POWER (+12V)                                                                    | Cam -> Lens |
| 7       | IRIS POSITION (F2.8: 6,2V F16: 3,4V CLOSE: 2,5V)          | IRIS POSITION (F2.8: 6,2V F16: 3,4V CLOSE: 2,5V)                                | Lens -> Cam |
| 8       | IRIS MODE CONTROL (AUTO: 0V REMOTE: 5V)                   | IRIS MODE CONTROL (AUTO: 0V REMOTE: 5V) AUTO=lens itself REMOTE=from camera     | Cam -> Lens |
| 9       | EXT SIG (Extender Signal) (OUT: open IN: 0V)              | EXT SIG (Extender Signal) (OUT: open IN: 0V)                                    | Lens -> Cam |
| 10      | ZOOM POSITION (WIDE: 2V TELE: 7V)                         | ZOOM POSITION (WIDE: 2V TELE: 7V)                                               | Lens -> Cam |
| 11      | FOCUS POSITION (NEAR: 2V INFINITY: 7V)                    | TXD (Transmit) (FALSE: 0V TRUE: 5V)                                             | Lens -> Cam |
| 12      | --                                                        | RXD (Receive)  (FALSE: 0V TRUE: 5V)                                             | Cam -> Lens |

## Serial Communication Parameters for TX/RX Pin


The communication uses a unusual communication rate at 78400 [baud].
The level is TTL, 0 .. 5V.
The logic levels are inverted!

+ TTL = 0 = 5V
+ TTL = 1 = 0V

If you only want to read the communication you can simply connect the serial line form camera or lens to an ordinary RS232 interface (e.g. MAX232). RS232 by itself uses inverted levels. This is perfect in this case. Normally the levels are between -12V .. +12V but it works also with the 0 .. 5V TTL levels.  
> [!WARNING]
> Avoid connecting the TX pin to the camera or to the lens!

For reference, normal (not inverted) RS232 levels are:

+ TTL = 0 = 0V --> RS232 = 0 = +12V
+ TTL = 1 = 5V --> RS232 = 1 = -12V

The uart parameters between camera and lens are:

|Parameter    | Value|
|---------    | -----|
|Speed [baud] | 78400 |
|Databits     | 8|
|Paritybit    | none |
|Stopbits     | 1|

short: `78400 8N1`

## Protocol Frame

A quick look with a Logic analyzer showed quite a lot of similarities between the Protocol Standardized by Fujinon for the 10pin plug and the Protocol used on the 12 pin Plug.

### Telegram structure

|byte 0|byte 1| byte ..| last byte|
|--    |--    |--      |--        |
|\<length>|\<cmd>|\<data 0..14>|\<crc>|

`<length> <cmd> <data 0..14> <crc>`

\<length>: length of all data bytes (value 0x00..0x0F / 0..15 -> max. 15 data bytes),  following after the mandatory \<cmd> byte  
\<cmd>: command / function  
\<data 0..n>: optional bytes, could be missing  
\<crc>: cyclic redundanc ceck / checksum  

#### CRC

sum up all bytes, beginning with the length byte  
calculate: 0x0100 - sum, take lower byte -> this is the crc

#### Example
minimal telegram  
`0x00 0x53 0xXX`  

telegramm with 10 data bytes  
`0x0A 0x53 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0xXX`

## Communication

The camera is polling any information from the lens. The lens waits for commands from the camera and responds accordingly. The lens always answers with the same command code which was sent by the camera.
> [!NOTE]
> Discaimer:
THIS INFORMATION IS TAKEN FROM THE FUJINON SPEC AND DOES NOT FULLY MATCH THE ACTUAL COMMANDS ON THE 12PIN CONNECTOR BETWEEN CAMERA AND LENS.

|  | | CAM TO LENS | | | LENS TO CAM | | |
|-|-|-|-|-|-|-|-|
|Function code |Function code name | Data length | Data | Function description | Data length | Data |Function description |
|-|-|-|-|-|-|-|-|
|0x01| Connect |0| - | Connection request |0 | - | Connection response |
|0x01| Connect | 1| By sending a command having data (data length: 0) to the lens, the host makes a request for connection, and the lens responds to the host. By sending a command having data (data length: 1) to the lens, the host makes a forcible resetting of the lens and the lens tells the host that the lens is being reset. Contents of all the data are 0H. However, the lens without reset function responds by sending a command having data (data length: 0) when receiving a command having data (data length: 1). Please contact us, Fujinon to ask if your lens has a reset function.| Lens reset request |1| | Response at the time of resetting |
|0x11| Lens name 1| 0| - | Request for the first half of the lensname |0..15 |The name of a lens may be sent at up to 80 ASCII characters. The host also makes a request for 0x12 if the 0x11 response data is 15 bytes long.  | Response to the first half of the lens name |
|0x12| Lens name 2| 0| - | Request for the second half of the lens name |0..15 |The name of a lens may be sent at up to 80 ASCII characters. The host also makes a request for 0x12 if the 0x11 response data is 15 bytes long.  | Response to the second half of the lens name |
|0x13| Open F No. |0| - | Request for open-F No. |2| 10000H is regarded as F1.0 with 1000H per iris FNo. = 2^(8*(1-Data/10000H)) Data = 10000H * (1-log2(FNo.)/8)  Data can be used up to FFFFH. | Response to open-F No.|
|0x14| Tele-end focal length |0| - | Request for tele-end focal length |2| | Response to tele-end focal length |
|0x15| Wide-end focal length |0| - | Request for wide-end focal length |2| | Response to wide-end focal length |
|0x16| MOD |0 | - | Request for MOD |2| | Response to MOD |
|0x20| Iris control |2|The variable range is represented as 0000H through FFFFH. 0000H is the close end and FFFFH the open end. | Iris control |0| - | ACKNOWLEDGE|
|0x21| Zoom control |2|The variable range is represented as 0000H through FFFFH. 0000H is the wide end and FFFFH the tele-end. | Zoom control |0| - | ACKNOWLEDGE| 
|0x22| Focus control| 2| The variable range is represented as 0000H through FFFFH. 0000H is the MOD and FFFFH the infinite distance  | Focus control |0| - | ACKNOWLEDGE |
|0x30| Iris position |0| - | Request for iris position |2|The variable range is represented as 0000H through FFFFH. 0000H is the close end and FFFFH the open end.| Response to iris position |
|0x31| Zoom position |0| - | Request for zoom position |2| The variable range is represented as 0000H through FFFFH. 0000H is the wide end and FFFFH the tele-end.  | Response to zoom position |
|0x32| Focus position |0| - | Request for focus position |2| The variable range is represented as 0000H through FFFFH. 0000H is the MOD and FFFFH the infinite distance  |Response to focus position |
|0x42| Switch 2 control |1| | Switch 2 control |0| - | ACKNOWLEDGE |
|0x43| Switch 3 control |1| | Switch 3 control |0| - | ACKNOWLEDGE |
|0x44| Switch 4 control |1| | Switch 4 control |0| - |ACKNOWLEDGE |
|0x52| Switch 2 position |0| - | Request for switch 2 position |1| | Response to switch 2 position |
|0x53| Switch 3 position |0| - | Request for switch 3 position |1| | Response to switch 3 position |
|0x54| Switch 4 position |0| - | Request for switch 4 position |1| | Response to switch 4 position |
|0x60| Multiple data |0| - | Request for multiple data |1..7 | |Response to multiple data 
|0x70| Multiple data setting |1..4| | Request for setting of multiple data |1..4| | Response to setting of multiple data

## ACTUAL Protocol used on the 12Pin Plug

The camera (Ursa Broadcast g1) initilizes the communication with repeaded 0x13 commands, until the lens responds.
Then the camera sends a 0x01 command.
After the init, the camera sends requests for names, and some unknown commands.

The commands 0x53, 0x61 and 0x62 are repeadedly send by the camera to get the current state of the lens

|Command Byte|Command name|Data Length|Response Data length|comment|
|---|---|---|---|---|
|0x01|Init|0|0|same as Documented Spec|
|0x10|Manufacturer Name|0|Observed 0x7, probably up to 0x0F|Probably same specs as 0x11|
|0x11|Lens Name 1|0|0-0xF|Same as Documented|
|0x12|Lens Name 2|0|0-0xF|Same as Documented|
|0x13|Open iris Value|0|2|Same as Documented, actually used for init by URSA Broadcast|
|0x71|Unknown|6|6|Lens responds with exact same command and data|
|0x72|unknown|4|3|Lens responds with same command and data, minus the last databyte|
|0x61|unknown|0|0xC|Repeadedly asked by camera, Must contain Iris, focus and Zoom values, and Probably some Switch positions|
|0x62|Unknown|0|3|Probably continuation of 0x61|
|0x44|unknown|1|0|Probably sets some Switch|
|0x42|See original Protocol|1|0|Probably same as in Original Protocol|
|0x53|See original Protocol|||Probably same as Original|
|0x05|Unknown|0|0|Lens responds with same Comand and data. maybe some end of init signal|
|0x23|Unknown|2|0|probably "Set IRIS Position"|


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





