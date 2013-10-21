PiGLET
======

Raspberry _PI_ open_GL_ _E_pics _T_oolkit

PiGLET is a live graphing tool for EPICS [1] PVs for the
Raspberry Pi. It uses OpenGL for drawing.

[1] http://www.aps.anl.gov/epics/

Dependencies
------------

 * cmake
 * libmagickwand-dev
 * libpulse-dev
 * GLUT (for PC)
 * /opt/vc headers (for the PI)
 * EPICS (tested with 3.14.12.3 and 3.15.0.1, for the latter see below)

Troubleshooting Quirks
----------------------

 * In **EPICS 3.15.01** (not EPICS 3.14.x), the PERL library is not
   correctly named (extraneous . in filename). See
   [Known Problems](http://www.aps.anl.gov/epics/base/R3-15/0-docs/KnownProblems.html).
   Thus in order to correctly run the PERL scripts below `scripts/`
   you must apply this
   [Patch](http://www.aps.anl.gov/epics/base/R3-15/0-docs/CONFIG-Unix.patch)
   **before** building.

 * If you experience weird linker problems during development (e.g.
   after a `git pull`), run cmake again to update the files.
