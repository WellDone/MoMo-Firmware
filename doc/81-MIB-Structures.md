## MIB Struct and Command Map Formats
This section describes the required ROM structures that must be present inside of MIB application modules in order for them to be properly loaded and executed.  Application developers do not need to implement these structures themselves since the MIB compiler automatically generates the correct structures from high-level .mib file definitions.  This section is primarily to document the format of these structures in case the pic12_executive needs to be modified and for general information.

### MIB Struct
Each 8 bit application module must have a MIB identification structure located in the last 16 bytes of ROM of the first 2k ROM page.  This structure contains required information for loading the modules as well as jump tables for locating the commands and interfaces that this module supports.

** 16 word structure with the following format **
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
| GOTO command map  		 |
------------------------------
| GOTO interface map		 |
------------------------------
```

#### Version Numbering
Versions should be in X.Y.Z format with X, Y and Z between 0 and 255 inclusive.  They should correspond to semantic versioning in the sense of major, minor patch.

### Command Map and Interface Map Location Functions
Inside the MIB Block there are GOTO command map and GOTO interface map functions.  These functions must be located in the first ROM page (address < 2k) and they must load the full address of the command or interface map, respectively into the FSR0 register.  The loading must happen with the FSR0H MSB set to indicate access to ROM so that the command map and interface maps may be read simply using the INDF0 register.

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
