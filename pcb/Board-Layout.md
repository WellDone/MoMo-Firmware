# Circuit Board Layout Guidelines
This document specifies rules and best practices for laying out and routing MoMo circuit boards.  It should be followed for all MoMo boards.  Additions to this document will be made as new guidelines are needed or board specifications change.

## Feature Sizes
- All silkscreen label text should be size 32 mils in EAGLE.  This is large enough to be legible but not so large as to make placement too difficult.
- Minimum trace/space is 6 mil.  When possible, route with larger trace/spaces but that only really makes sense if you can route the entire board with larger traces.  Power containing traces should be sized according to power demands but at least 12 mils.

## Fixed Locations
Screw holes are required on the short edge of each MoMo module for proper assemble.  The holes should be .132" in diameter and spaced .128" in from the edge of the boards.  The should be spaced 1.1" apart from each other.  There will be a nylon standoff around the screw hole, meaning that the exclusion are around the screw hole is: 0.25" in diameter.  The two sets of holes should be 3.744" away from eachother center to center.  Given a 2" long module, that gives 0.128" away from the module edge.

## Miscellaneous

- All capacitors unless required should be 0402 from Eagle's RCL library C_US 0402K (Kemet for reflow soldering)
- All resistors should be MLCC 0603 standard 1% chip resistors unless other specs are required.