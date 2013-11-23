# MoMo Printed Circuit Boards
This directory contains all of the MoMo PCB files as well as scripts used to convert them into production data.
All boards must use EAGLE CAD for layout in order to be compatible with our production scripts.

## PCB Layout
The MoMo build system includes a set of python based tools used to automatically generate production files, BOMS 
and cost estimates from EAGLE board files.  In order for a board to be compatible with these scripts, it
must be made to conform with the following requirements.

EAGLE Attributes are used to tag each part and board with metadata that enables the scripts to look up
information on the part using the Octopart API and to generate a BOM.  Each part should have one of following attributes:

 * _DIGIKEY-PN_ - This should contain the Digikey part number for this part.  If there is not a digikey part number, is should
be left blank or non existent
 * _MPN_ - This is the Manufacturer's part number for the part.  Either MPN or DIGIKEY-PN must be set for every part on the board that
should be populated.
 * _MANU_ - The manufacturer's name.  If MPN is used to specify the part, MANU should be specified as well.
 * _FOOTPRINT_ - If the part does not have a standard SMD footprint link 0402, 0603, etc, they FOOTPRINT should be explicitly defined so
 that it can be used to create a BOM automatically.  If this is a through hole part, the footprint should be given as "Through Hole"
 * _DESCRIPTION_ - If the part cannot have a generic description made for it (currently only works for Resistors, Capacitors and Inductors)
 then the explicit description should be given in this attribute.

###Assembly Variants
In order to save on PCB fabrication costs, sometimes a single circuit board will be made flexible so that it can be customized at assembly
time by simply change or not populating certain parts.  Each different way the same board can be assembled is called an Assembly Variant.

If a board has several assembly variants, this should be indicated by specifying certain attributes on the board.  An assembly variant can 
either populate a certain part, not populate a certain part or replace a part with a different one.  To do this, change the DIGIKEY-PN or MPN attribute for the part to contain -VARIANT where variant is the name of the assembly variant.  A part with only DIGIKEY-PN (or MPN) set will be assumed to be populated
in every assembly variant. A part with DIGIKEY-PN-VAR1 and DIGIKEY-PN-VAR2 will be populated only in Assembly Variants VAR1 and VAR2.  

###Global Attributes
Every board must define the following global attributes.  These should be defined on the Board, not the Schematic, under Edit->Global Attributes:

 * _[VARIANT]_ - for each assembly variant (if there is only one, nothing must be done) with value ASSY-VARIANT.  The attribute name is the variant name
 * _WIDTH_ - the width of the board (in inches)
 * _HEIGHT_ - the height of the board (in inches)
 * _REVISION_ - a tag specifying the revision level of the board
 * _PARTNAME_ - a name for the board that will be used to generate file names

###Required Configuration
In order to use these scripts you need to configure the location of the Eagle binary on your computer in /config/settings.local.json.  Use the template provided in:
settings.global.json
