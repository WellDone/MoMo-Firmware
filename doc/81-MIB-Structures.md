## MIB Struct and Command Map Formats
This section describes the required ROM structures that must be present inside of MIB application modules in order for them to be properly loaded and executed.  Application developers do not need to implement these structures themselves since the MIB compiler automatically generates the correct structures from high-level .mib file definitions.  This section is primarily to document the format of these structures in case the pic12_executive needs to be modified and for general information.

### MIB Struct
Each 8 bit enhanced midrange PIC application module must have a MIB identification structure located in the last 16 words of ROM of the first 2k ROM page.  This structure contains required information for loading the modules as well as jump tables for locating the commands and interfaces that this module supports.

**16 word structure with the following format**

Any values that are not prefaced by GOTO should be encoded as retlw instructions

```
------------------------------
| Hardware Type	  			 |
------------------------------
| Major API Version 	  	 |
------------------------------
| Minor API Version 		 |
------------------------------
| Module Name or Identifier	 |
| 							 |
| 6 bytes of ASCII text 	 |
| or a 6 byte GUID		 	 |
|							 |
|							 |
------------------------------
| Major Version Number 		 |
------------------------------
| Minor Version Number 		 |
------------------------------
| Patch Version Number 		 |
------------------------------
| Application Checksum 		 |
------------------------------
| MIB Magic Number 			 |
------------------------------
| GOTO Config Function 		 |
------------------------------
| Reserved, must be retlw 0	 |
------------------------------
```

#### Version Numbering
Versions should be in X.Y.Z format with X, Y and Z between 0 and 255 inclusive.  They should correspond to semantic versioning in the sense of major, minor patch.

### Required Configuration Function
Inside the MIB Block there are GOTO configuration function.  This function must be located in the first ROM page (address < 2k) and it must take a single byte as an argument in the W register.  The valid arguments are 0-3 inclusive and in response to being called, the configuration function must load the FSR0 indirect register with the full address of the command, interface or configuration variable maps.  The loading must happen with the FSR0H MSB set to indicate access to ROM so that the command map and interface maps may be read simply using the INDF0 register.  The arguments that the function must respond to are:

- 0: Command Map
- 1: Interface Map
- 2: Configuration Variable Map
- 3: Reserved, function must be do nothing except (possibly) return an arbitrary value in W.

### Command Map Format
The command map is a list of 4 word entries where each entry has the following format.  The last entry contains a sentinel value so there must always be at least one entry.

```
------------------------------
| Command ID (16 bits) 		 |
| 							 |
------------------------------
| Command Location PCLATH	 |
------------------------------
| Command Location Low byte	 |
------------------------------
```

The sentinel value has the third byte, PCLATH, equal to 0xFF since there is no 8-bit enhanced midrange PIC with 64 kb of ROM.

### Interface Map Format
Interfaces are 4 byte numbers that define high level collections of MIB endpoints that pertain to specific high-level functionality like a sensor api for pulling data or a communication API for sending data remotely.  The format for the interface map is that each interface that an application implements must be listed as a series of retlw instructions in little endien format followed by a 4 word sentinel value that consists of 4 retlw 0xFF commands.

The format for each entry is:

```
------------------------------
| Interface ID bits 0..7 	 |
------------------------------
| Interface ID bits 8..15	 |
------------------------------
| Interface ID bits 16..23	 |
------------------------------
| Interface ID bits 24..31	 |
------------------------------
```

The format for the sentinel value that ends the list is shown below.  This is the same sentinel value that ends the command map list.

```
------------------------------
| retlw 0xFF			 	 |
------------------------------
| retlw 0xFF			 	 |
------------------------------
| retlw 0xFF			 	 |
------------------------------
| retlw 0xFF			 	 |
------------------------------
```
