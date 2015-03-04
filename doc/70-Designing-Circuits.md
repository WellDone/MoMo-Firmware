# Working with Printed Circuit Boards
The MoMo framework includes a number of tools that simplify the process of designing and assembling printed circuit boards.  These tools lets you automatically generated Bill of Materials documents, price your board at prototype and production quantities and generate all of the required production files for getting the board fabricated and assembled.  All of these products are built from your board layout file containing attributes describing all the required metadata.  This section details how to use the tools and how to design your boards to take advantage of the automatic MoMo PCB build system.

## Getting Started
There are a number of steps that take you from having a PCB layout to actually having that board assembled and in your hands (or those of your customers).  

- **(Supported)** You need to match all of the parts you chose with a component database and generate a Bill of Materials (BOM) document that you can use to buy all of the components for your board.
- **(Supported)** You may want to have multiple assembly variants of the same board, so you'll need a way to make appropriate BOMs and assembly drawings for each variant.
- **(Supported)** You need to generate gerbers and drill files from your board to send to the pcb fab for the board to get made.
- **(Supported)** You need to generate assembly drawings so that you can get the board populated with components from your BOM.
- **(Supported)** You need to figure out how much it will cost for your board in many different quantities and where you should buy your parts.
- (Support planned) You may want to generate a 3D model of your board to use to design the enclosure and make sure everything fits.
- (Support planned) You may need to generate pick and place instructions for automated assembly.

Each of these steps takes time and is error prone, contributing to the cost of your design by increasing the engineering time required to make it.  Included in `pymomo` is a suite of tools that automate each of these steps, reducing errors and saving time.  

In order to make this possible, you need to use a compatible ECAD program and you need to annotate your board with a minimum of metadata so that our automated tools know how to generate all of the required files from generic templates.  The way this annotation takes place depends on what ECAD system you use and is described for each supported program in one of the sections below.

### Required Board Metadata
Each board file is required to specify the following attributes, which are used at various points in the build process to generate output files like assembly drawings, BOMs and Gerbers. The specific way you need to set the attributes varies by ECAD program and is documented in the section on each supported program.

- Height: (number) The height (y dimension) of the board in the units specified in Dim_Units
- Width: (number) The width (x dimension) of the board in the units specified in Dim_Units
- Dim_Units: (text) The units for Width and Height (e.g. inches, mm)
- Company: (text) A string identifying the company or individual that made this board
- Revision: (text) A string specifying the version or revision number of this board.  This can be an arbitary string and is not forced to conform to any specific format.
- Fab_Template: (text) A string specifying what kind of board this is (two layer, 4 layer, etc).  It must correspond to the name of a template defined for your ECAD program, see the section on your ECAD program for the predefined templates.

### Required Component Attributes
In order to produced useful BOMs and quote board prices, we need to be able to match each component on the board with an online database of physical parts that could be purchased from a distributor.  This can be done in two ways.  Either you can supply a manufacturer and manufacturer's part number for a component, or you can supply a distributor's name, like Digikey or Arrow and their part number for the component.  These are the only attributes that are required per part.  

The rest of the information needed for generating BOMs can be looked up but if you would like to override it with your own values, you can define the following optional attributes that will be used instead of what is found in the database:

- Description: (text) a brief description of the component such as "PIC 24F16KA101 8-bit microprocessor"
- Footprint: (text) a text identifier for the type of package that the component comes such as SOT-23 or SMD 0402

Some standard packages are automatically inferred from information provided by the ECAD program and more will be added in the future.  If you don't like the wording of the package in the BOM, you can override it by using the FOOTPRINT attribute.  Not all online databases provide description information so it can be useful to set that attribute as well.

### When Are Identifiers Considered Identical?
A lot of the time there could be ambiguity over whether two words are the same.  For example, is "Digi-Key" the same distributor as "DIGIKEY"?  It's likely that the user intended for them to be the same, but they are not identical strings, making it difficult for a program to determine the user's intent.  In `pymomo.pcb` a consistent policy is applied to determine whether two text strings like manufacturer names or part numbers are identical.  First, all spaces, '_' and '-' characters are stripped out of both strings and they are converted to lowercase.  Then, these canonicalized strings are compared to see if they are identical.  So, "Digi-Key" matches "DIGIKEY" which also matches "DIGI KEY" but does not match "Digi,key".

### Supported Component Databases
Currently the only supported component database that can be used is [Octopart](http://octopart.com).  It is easy to add other databases and support is planned for Mouser's API as well. This means that to use the BOM matching features you currently need an Octopart API key (there's a free tier!).  `pymomo.pcb` looks for the key as an environment variable named `$OCTOPART_KEY`.  All results from any component database is cached for the maximum amount of time permitted by the TOS of the serivce to reduce latency and the total number of requests the program makes.  This caching process is transparent to the user and results returned from the cache are indistinguishable from those returned directly from the database.  

## Interacting with PCB Board Files
All of the objects that deal with circuit boards are defined in `pymomo.pcb` and accessible using the `momo` tool.  This section walks through some common tasks using the momo controller pcb as an example:

```shell
> ls $MOMOPATH/pcb/momo_modular/controller
controller.brd	controller.sch
```

### Matching Parts with an Online Database
```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd match_status
True
```

`match_status` returns whether or not all of the parts on this board could be matched to an online database based on their embedded metadata.  You can get more details about the status of each part by using ```match_details```

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd match_details
Q1: Matched to a unique item
Q3: Matched to a unique item
Q2: Matched to a unique item
U5: Matched to a unique item
U4: Matched to a unique item
U1: Matched to a unique item
U2: Matched to a unique item
JP1: Matched to a unique item
C13: Matched to a unique item
JP2: Matched to a unique item
JP5: Matched to a unique item
JP4: Matched to a unique item
C15: Matched to a unique item
R13: Matched to a unique item
XT1: Matched to a unique item
C9: Matched to a unique item
C3: Matched to a unique item
C5: Matched to a unique item
C4: Matched to a unique item
R4: Matched to a unique item
C20: Matched to a unique item
R7: Matched to a unique item
R1: Matched to a unique item
D2: Matched to a unique item
```

This tells us that all of the parts were matched to exactly one physical part in the online database, indicating that there was no ambiguity in the match process and the board is ready for BOM generation.

### Looking up a Part 
You can look up any part on a board to see who sells it and how much it costs.  For example lets see what part U5 on the controller board does:
```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd find U5
Part U5
 Manufacturer: MICRON
 MPN: M25PX80-VMN6TP
 Distributor: Digikey
 Distributor PN: M25PX80-VMN6TPCT-ND
 Package: SOP08 Package (0 pins, 8 pads)
 Location: (55.9816 millimeter, 19.6088 millimeter) rotated 90 degrees
 Description: M25PX80 1MB Serial Flash
```

It looks like this part is a flash memory chip.  Now let's see how much it costs and who sells it:

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd lookup U5
Manufacturer: Micron
MPN: M25PX80-VMN6TP
Description: NOR Flash Serial-SPI 2.5V/3.3V 8Mbit 1M x 8bit 8ns 8-Pin SO N T/R
Offers
  Seller: Verical
    Packaging: None
    Stock: 7500
    Minimum Order: 2500
    Breaks
      2500: 0.60000
  Seller: Digi-Key
    Packaging: Cut Tape
    Stock: 18208
    Minimum Order: 1
    Breaks
      1: 0.86000
      10: 0.78100
      25: 0.77120
      50: 0.76680
      100: 0.68540
      250: 0.67968
      500: 0.66960
      1000: 0.64548
  Seller: Digi-Key
    Packaging: Custom Reel
    Stock: 18208
    Minimum Order: 1
    Breaks
      1: 0.86000
      10: 0.78100
      25: 0.77120
      50: 0.76680
      100: 0.68540
      250: 0.67968
      500: 0.66960
      1000: 0.64548

 <output truncated>
```

The `lookup` function returns all of the sellers for a particular part known to the chosen online database.  

### Generating a BOM
Once a board has all of the required metadata, it can be used to automatically generate a BOM.  Currently, `pymomo` only supports generating excel style (.xlsx) BOMs since these are standard but support is planned for template based generation of BOMs in other formats.  First, let's make sure the BOM lines look right:

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd bom_lines
6 10K Resistor: ['R1', 'R2', 'R3', 'R5', 'R9', 'R18']
2 1 ohm Resistor: ['R4', 'R20']
5 100K Resistor: ['R6', 'R10', 'R11', 'R13', 'R19']
7 510K Resistor: ['R7', 'R8', 'R12', 'R14', 'R15', 'R16', 'R17']
8 100 nF Capacitor: ['C1', 'C2', 'C5', 'C10', 'C11', 'C12', 'C16', 'C17']
1 33 pF Capacitor: ['C3']
1 10 pF Capacitor: ['C4']
4 1 uF Capacitor: ['C6', 'C9', 'C19', 'C21']
2 18 pF Capacitor: ['C13', 'C14']
2 10 uF Capacitor: ['C15', 'C18']
1 22 pF Capacitor: ['C20']
2 Diode: ['D2', 'D3']
1 XC6504 Linear Regulator 2.8V: ['U1']
1 PIC24FJ64GA306 Microcontroller: ['U2']
1 RN4020 Bluetooth 4.0 Module: ['U4']
1 M25PX80 1MB Serial Flash: ['U5']
1 Quartz Crystal (Radial Can): ['XT1']
1 6 Pin Vertical Male Header: ['JP1']
1 6-pin female R/A header: ['JP2']
1 MicroUSB-B Receptacle : ['JP4']
1 R/A Shrouded 2 pin connector: ['JP5']
1 N-Channel MOSFET 2N7002P: ['Q1']
2 P Channel MOSFET: ['Q2', 'Q4']
1 PNP Transistor: ['Q3']
```

The `bom_lines` function returns a list of strings for each BOM line with the line's description and, quantity and reference designators.  If everything seems fine, you can export a BOM using:
```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd export_bom $OUTPUT_FILE --format excel
```

`$OUTPUT_FILE` should be the path to the excel file that you want to generate.  If there are any errors or warnings in the process of generating the BOM they are noted in the excel file by default to make sure that you don't accidentally get a bad BOM.

### Checking For Errors
The program keeps track of any missing required attributes that it finds as well as common mistakes and can report these to you using the `get_errors` and `get_warnings` functions, which both return a list of strings.

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd get_errors
ERROR on attribute/part company: Attribute was empty or not defined
ERROR on attribute/part units: Attribute was empty or not defined
```

Here the system is telling us that we forgot to define a company and dim_units attribute on our board. In generated BOMs, the spots where the information is needed will be filled with obvious placeholder values until we fix this.  The program does its best to work around any missing information but always makes a note of it so that you know what it's doing.

### Pricing Parts and Boards
If you want to figure out how much a certain part costs at various quantities, you can use the `price` function.  For example, to figure out how much part U5 costs for 1 unit from Digikey, you can do:

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd require_distributor Digikey price U5 1
$0.86 from Digi-Key as Custom Reel
```

More information on setting requirements about what types of sellers can be used to generate prices is discussed in the section on Creating a Price Model.  We can just as easily figure out how many 1000 parts would cost (including any saving due to price breaks).

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd require_distributor Digikey price U5 1000
$0.65 from Digi-Key as Custom Reel
```

To figure out how many an entire board costs, you can use the `detailed_prices` function.  For example, let's say that we want to buy enough parts for 10 boards from Digikey with 10% excess to account for assembly losses.  Since it's a small number of units, we don't want to consider any Digi-Reel custom reels.  It's most clear to specify the price requirements one at a time, we start by opening a momo shell and specifying our requirements:

```shell
> momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd
(CircuitBoard) require_distributor Digikey
(CircuitBoard) exclude_packaging "Custom Reel"
(CircuitBoard) detailed_prices 10 10
P Channel MOSFET: $4.32 from Digi-Key as Cut Tape
1 ohm Resistor: $0.64 from Digi-Key as Cut Tape
Diode: $3.76 from Digi-Key as Cut Tape
10 pF Capacitor: $0.13 from Digi-Key as Cut Tape
510K Resistor: $1.06 from Digi-Key as Cut Tape
RN4020 Bluetooth 4.0 Module: $88.40 from Digi-Key as Tray
10K Resistor: $0.92 from Digi-Key as Cut Tape
10 uF Capacitor: $3.42 from Digi-Key as Cut Tape
18 pF Capacitor: $0.48 from Digi-Key as Cut Tape
XC6504 Linear Regulator 2.8V: $3.91 from Digi-Key as Cut Tape
100 nF Capacitor: $2.02 from Digi-Key as Cut Tape
6-pin female R/A header: $7.26 from Digi-Key as Bulk
MicroUSB-B Receptacle : $4.33 from Digi-Key as Cut Tape
1 uF Capacitor: $2.64 from Digi-Key as Cut Tape
PIC24FJ64GA306 Microcontroller: $32.20 from Digi-Key as Tray
Quartz Crystal (Radial Can): $1.75 from Digi-Key as Bulk
PNP Transistor: $0.95 from Digi-Key as Cut Tape
33 pF Capacitor: $0.13 from Digi-Key as Cut Tape
N-Channel MOSFET 2N7002P: $1.35 from Digi-Key as Cut Tape
22 pF Capacitor: $0.13 from Digi-Key as Cut Tape
6 Pin Vertical Male Header: $2.05 from Digi-Key as Bag
M25PX80 1MB Serial Flash: $7.81 from Digi-Key as Cut Tape
R/A Shrouded 2 pin connector: $1.53 from Digi-Key as Bulk
100K Resistor: $0.76 from Digi-Key as Cut Tape
(CircuitBoard) quit
```

If we don't care about the details of how much each part costs but just want the total price, we can use the `total_price` function

```shell
momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd total_price 100 0
total: 1290.11
unit: 12.90
```

### Creating a Price Model
Price a board is a little complicated.  Each part could come from several distributors and each distributor has a finite amount of stock.  Parts come in various kinds of packaging as well like Reels, Cut Tape and Trays.  Part of pricing a board is setting up preferences for which distributors to buy from and what kinds of packaging to look for or avoid.  The set of requirements you have for sellers to be considered valid is called the Price Model by `pymomo.pcb`.  You can build it up little by little using the following functions:

```
exclude_distributor
require_distributor
exclude_package
require_package
require_stock
```

The `require_*` and `exclude_*` functions respectively whitelist and blacklist certain distributors and packaging types.  For example if you call:

```
(CircuitBoard) exclude_distributor Mouser
(CircuitBoard) require_package "Cut Tape"
```

Then you are saying that you only want to look at prices that come in Cut Tape and are not from Mouser.  You can also choose to only show prices where there is enough stock to cover your entire order from a single distributor using the `require_stock` function.  If you make a mistake, you can clear the current Price Model and start over using `clear_pricemodel`. The Price Model is a property of the CircuitBoard context that created it and will apply to all future pricing requests on that CircuitBoard until it is cleared.  

### Generating Production Files for Fabrication and Assembly
Once you have your board file tagged with all of the required metadata as described in the section on Supported ECAD Programs, you can use `pymomo.pcb` to automatically generate production files (Gerbers, Excellon Drill Files, Assembly Drawings and BOMs) for fabrication and assembly.  To do so, just create a CircuitBoard object and call `generate_production` passing the name of the directory you would like to create to contain all of the fabrication and assembly files.  For example,

```shell
$ momo pcb CircuitBoard $MOMOPATH/pcb/momo_modular/controller/controller.brd generate_production ~/controller
$ ls ~/controller/fabrication/
README.txt	controller.crm	controller.plc	controller.stc
controller.cmp	controller.drd	controller.sol	controller.sts
$ ls ~/controller/assembly/
BOM_controller.xlsx	controller.ps
```

The created production files include:

- GERBER format files for each layer on your board
- A README.txt that contains descriptions for each gerber, your company, part name, revision, etc.
- A BOM for your board
- An assembly drawing
- A zip file containing all of the fabrication (<part name>_fab.zip) related files for uploading to your board house.

### Templates Used for Production Files
In order to generate production files, you need to embed information into your board about the layers that you want generated.  For example, is your board a standard 2 layer design with top and bottom copper and soldermask and a top silkscreen or do you want a 4 layer board?  This is done by embedding a fab_template attribute into your board as described in the section on your particular ECAD program.  The template name is used to look up how many Gerber layers to create and what logical layers to include in each gerber file.

## Supported ECAD Programs
The `pymomo.pcb` PCB build system is completely agnostic to where the information it needs comes from, so it does not depend per-se on what ECAD program you use to design your circuit.  However, it needs a way to extract the information from your board files.  We have built adapters that work automatically with the following popular ECAD programs.  If you use another program you could consider writing an adapter module to make it work with `pymomo.pcb`.  Pull Requests are always appreciated.

### EAGLE CAD
Eagle is a very popular schematic capture and layout program and is fully supported by `pymomo.pcb`.  The build system works with EAGLE .brd board files and to be suitable you need to annotate the boards using EAGLE attributes as described in this section.  You also need to be using a new enough version of EAGLE that uses their XML file format.  

#### Specifying Global Board Information
You need to set the following 'Global Attributes' on your Eagle Board (not the schematic, the board itself).  You can set an attribute using the menu item Edit -> Global Attributes.  The attributes are described in the above section on required attributes.  This section just lists the names that each attribute should have (all attributes in Eagle are stored as uppercase strings).

- HEIGHT: Board Height
- WIDTH: Board Width
- DIM_UNITS: Height/width units
- COMPANY: Your company or name
- REVISION: An identifying specifying the revision of this board.
- FAB_TEMPLATE: The template describing the layers on your board (i.e. two_layer, 4_layer, etc).

#### Specifying Multiple Assembly Variants
If your board has multiple assembly variants, you need to specify what they are by defining a Global Attribute (see previous section) that contains the name of each variant.  The names can be any string you want that forms a valid EAGLE attribute identifier. Each variant should be defined by creating an attribute whose name corresponds to that name of the attribute and whose value is 'ASSY-VARIANT'.  Remember that assembly variant names are not case sensitive.  If you only have one assembly variant nothing needs to be done.

For example, to define a board with two assembly variants (solar and battery), define two global attributes as follows:

```
SOLAR: ASSY-VARIANT
BATTERY: ASSY-VARIANT
```

You can specify assembly-variant specific part information by appending the assembly variant name to a part's attribute using a hyphen, so if you want to make a part that is populated in the SOLAR variant but not in BATTERY (e.g. the solar panel), you could define two attributes on that part as follows:

```
POPULATE-SOLAR: YES
POPULATE-BATTERY: NO
```

This part will be automatically included as a populated part when you deal with the SOLAR variant but will be listed as an unpopulated part in the BATTERY variant.  You can also specify default values that apply at all variants except when overridden by specifying the attribute without a variant extension.  So the above example could also be written as:

```
POPULATE: YES
POPULATE-BATTERY: NO
```

The default attribute applied to all assembly variants would be to populate the part but, only for the battery variant, it would be overriden to not populate the part.  The following per-component metadata can be overwritten in this way:
- VALUE
- POPULATE
- DIST
- DIST-PN
- DIGIKEY-PN
- MANU
- MANU-PN
- DESCRIPTION

#### Specifying Component Metadata
In order to support BOM generation and the creation of placement files and assembly drawings, each component on the eagle board file must have a minimum out of metadata that allows it to be matched with an online component database and the rest of the information looked up.  Specifically, you have to provide either a distributor's name and part number or a manufacturer's name and part number.  These are specified using Eagle attributes attached to each component *on the board file*.  The list of valid attributes is:

- DIST - a distributor's name that carries this part
- DIST-PN - the distributor's part number for this part
- DIGIKEY-PN - a shortcut that sets DIST=Digikey and DIST-PN to this value
- MANU - The manufacturer
- MPN - The manufacturer's part number
- POPULATE - [yes | no], whether this part should be populated.  All parts are assumed to be populated unless a POPULATE=no attribute is specified.
- DESCRIPTION - a short textual description
- FOOTPRINT - the footprint for this part (i.e. SOT-23, SMD 0402, Through Hole, etc)

You can override some of these values per assembly variant as described in the previous section.

#### Supported Fabrication Templates
Currently, only two layer boards are supported without extra configuration by specifying 'two_layer' in the FAB_TEMPLATE global attribute.  This will create a board with the following gerber layers (composed of the indicated EAGLE layers):

- Top Soldermask: ["tStop"]
- Top Silkscreen: ["Dimension", "tPlace", "tNames"]
- Top Solderpaste: ["tCream"]
- Top Copper: ["Top", "Pads", "Vias"]
- Bottom Copper: ["Bottom", "Pads", "Vias"]
- Bottom Soldermask: ["bStop"]

The assembly drawing is in PS format since that is what EAGLE supports exporting and it includes the following layers:
["tPlace", "tNames", "tDocu", "Document", "Reference","Dimension"]

You can create your own eagle templates and use them in your boards by editing the eagle.json file in the config directory of pymomo.

#### Finding Your Installed Eagle Version
If you are not using Mac OS X you need to add the EAGLE executable to your $PATH in order for `pymomo.pcb` to be able to find it and use it for generating fabrication files.  You don't need to do this for BOM generation since that is done by parsing the EAGLE board files directly. 

If you are using Mac OS X and you installed Eagle to the standard location using the EAGLE installer (/Applications/EAGLE-X.Y.Z) then you don't need to do anything.  Pymomo will find eagle for you and automatically use the latest version.  If you want to force pymomo to use a particular version then you can add that version to your $PATH and pymomo will preferentially use what it finds in the path over what it finds looking on its own.