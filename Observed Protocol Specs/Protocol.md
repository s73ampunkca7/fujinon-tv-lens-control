Fujinon B4 Lens Protocol
===

# Introduction


This document describes the Observed Specifications of the lens control Protocol used by B4 Broadcast lenes.

## Test Setup

The test-setup uses a Logic analyzer between a Blackmagic Design Ursa Broadcast G1 and a Fujinon XA17x7.6 BRM-M58B
A Fujinon A13x4.5 BERM-S48 is used as a Second Lens reference.
The use of a Canon lens for aditional cross manufacturer Reference is planned.

# Observations

## Physical Specs

## Used Plugs

|Camera side: | HR10-10R-12S|
|-|-|
|Lens side: |HR10-10P-12P|

## Pinout

|Pin| 	Function| 	Direction |	Description|
|-|-|-|-|
1 |	RET SW |	lens to camera |	Connected to RET button on lens; GND when pressed, otherwise open|
2 	|VTR SW |	lens to camera| 	Connected to VTR or REC button on lens; GND when pressed, otherwise open|
3| 	GND| 	camera to lens |	Ground|
4 |	IRIS ENF AUTO| 	camera to lens |	Momentary auto iris, off: 0V, on: 5V|
5 	|IRIS CONT| 	camera to lens |	Voltage indicative of requested iris position, f/2.8: 6.2V, f/16: 3.4V|
6| 	+12V |	camera to lens |	Power supply|
7 |	IRIS FOLLOW |	lens to camera |	Voltage indicative of current iris position, f/2.8: 6.2V, f/16: 3.4V|
8 	|IRIS A/R |	camera to lens| 	Iris auto: 0V, remote: 5V|
9| 	EXT ANS |	lens to camera| 	Connected to GND when extender is engaged, otherwise open|
10| 	ZOOM FOLLOW |	lens to camera| 	Voltage indicative of current zoom position, Wide: 2V, Tele: 7V|
11| 	RxD |	lens to camera| 	Receive serial data |
12 |	TxD| 	camera to lens |	Transmit serial data |

## Data Format

| Spec | Value | Comment |
| -------- | -------- | -------- |
| Boudrate | 78.400 Boud | Unusual value |
|Data Length|8 Bit||
|Stop Bits|1 Bit||
|Parity|NONE||
|Polarity|Iverted|like RS232|
|Bit Order|LSB First||
|Voltage|0-5V||

## Protocol Sequence 

All Data Packages follow the same Structure

|Data Length|Command|Data|Checksum|
|-|-|-|-|
|1 byte|1 byte|0-15 byte|1 byte|

### Data Length

The Data length describes how many bytes are expected for Data. It ranges from 0x00 to 0x0F for 0 to 15 bytes.

### Command

The Command byte is described in the Command list Section.

### Data

If 2 or more bytes of Data are send, the Data bytes are send in order of top to Bottom

### Checksum

The checksum consists of a single byte and is calculated as follows.

The Data Length, Command and Data Bytes get added together in a single Byte. The intermediete result gets Subtracted from 0x100. The result is the Checksum.

#### Example of checksum calculation

|Data Length|Command|Data|Sum|Substraction|Checksum|
|-|-|-|-|-|-|
|0x02|0x20|0x80, 0x80|0x22|0x100-0x22|0xDE|

#### Verification of Checksum

To verify a Data Packet, all received bytes, including the Checksum get added up in a single byte. If the result is 0x00, the checksum test is passed.

### Communication related operation

>[!WARNING]
>Everything that follows is incomplete.
>See FUJINON L10 Protocol Specification for rough overview.


1. The Host (Camera) leads the communication, the lens responds to the Cameras commands. No Commands can be send by the lens first.
2. The lens always responds with the same Command byte, optionally followed by the requested Data as an ACK signal.

## Command List

### Commands Overview

|Function Code|Function Name|Data Length|Data description|Response Data Length|Response Data Description|Additional Comment|
|-|-|-|-|-|-|-|
|0x01|Connect|0|Connection request|0|Connection response||
|0x01|Reset|0x01|reset request|0x01|reset response|Command taken from L10 Specs, not yet observed in actual communication|
|0x05|UNKNOWN|0x00||0x00||Maybe some sort of end of init signal?!|
|0x10|Manufacturer Name Request|0x00||0x00-0x0F|Manufatcurer name|Data Length probably ranges from 0x00 to 0x0F, only 0x07 was observed yet|
|0x11|Lens name request|0x00||0x00-0x0F|Lens name|
|0x12|2nd Lens name request|0x00||0x00-0x0F|Only gets requested if response data length of 0x11 is 0x0F|
|0x13|Open Fno|0x00|request for open Fno|0x02|Open Fno|Ursa Broadcast (G1) Sends this command first instead of 0x01|
|0x23|UNKNOWN|2||0||Probably same as 0x20 in the original L10 Protocol, used to set Iris value|
|0x42|Switch2 Control|1|Set Switch Values|0||Probably same as L10 Protocol|
|0x44|Switch4 Control|1|Set Switch Values|0||Probably same as L10 Protocol|
|0x53|Switch3 request|0|Get Switch Values|1||Probably same as L10 Protocol|
|0x60|Multiple data|0|Request multiple data thats set with 0x70|0x01-0x07|Multiple data response||
|0x61|UNKNOWN|0||variable||Probably continuation of 0x60?? Repeadedly asked by camera, Must contain Iris, focus and Zoom values, and Probably some Switch positions|
|0x62|UNKNOWN|0||variable||Continuation of 0x61|
|0x70|Set Multiple Data|0x01-0x04|Set data to be requested with 0x60|0x01-0x04|Respond with available data types||
|0x71|UNKNOWN|Variable||variable|Responds with same data, some lenses dont respond with all bytes |Probably an extention of 0x70, like 0x61|
|0x72|UNKNOWN|Variable||variable||Probably continuation of 0x71|

### Detailed Command description

#### 0x01 
See L10 Protocol

#### 0x05
Currently unknown.
Gets send by the Camera only once near the beginning of the communication after requesting some initial Data. Maybe some sort of end of Init signal. Lens responds with empty ACK.

#### 0x10
Request for Manufacturer Name.
See 0x11. There seems to be no second half request.

#### 0x11, 0x12
Request of Lens name.
ASCII Encoded. if 0x11 gets a 0x0F long response, camera requests 0x12.

#### 0x13
Open Iris Value.
0x0000 to 0xFFFF, 0x10000 is F 1.0
0x1000 per Iris.
$Fno=2^{(8*(1-Data/0x10000)}$
$Data=0x10000*1-Log_2(Fno)/8$

#### 0x14, 0x15 0x16
Distance data Format.
Not yet observed, Probably same as L10 Protocol

#### 0x20, 0x21, 0x22
