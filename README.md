PiGLET
======

Raspberry _PI_ open_GL_ _E_pics _T_oolkit

PiGLET is a live graphing tool for
[EPICS](http://www.aps.anl.gov/epics/) PVs for the Raspberry Pi (but
compiles also on normal PCs). It uses OpenGL for drawing.


Building and Dependencies
-------------------------

To build it, you need on your system (names are Debian packages):

 * build-essential, cmake (i.e. a sane development environment)
 * Magick Wand Library and Headers (libmagickwand-dev)
 * PulseAudio Library and Headers (libpulse-dev)
 * Sndfile Library and Headers (libsndfile-dev)
 * (Free)GLUT (for PC)
 * `/opt/vc` headers (for the PI, shipped with [raspbian](http://www.raspbian.org/))
 * EPICS, preferrably in `/opt/epics`, but cmake finds it via
   EPICS_BASE environment variable (tested with 3.14.12.3 and
   3.15.0.1, for the latter see below)

Then, do a nice "out-of-source" build of the code, e.g.:

    mkdir build && cd build && cmake ..
  
Watch for errors of cmake, usually it tells you that you have missed
some dependency. Then 

    make

Grab a coffee, it takes 10mins on the Pi! In the end, run

    ./PiGLET
  
It should output something (on ssh) and then open up a blank black
empty fullscreen! See below what to do now.


How to use?
-----------

PiGLET is configured via 

    telnet localhost 1337
  
Of course, replace localhost with the host of the Pi if needed. Then
type `List` in the terminal to see the first Commands. To add a
PlotWindow, which displays an EPICS record, use:

    AddPlotWindow MyReallyCoolRecord

There is also a little EPICS IOC provided with a simple database for
playing around with `caput` and `caget`, but you probably want to edit
the hard-coded path in the `Run.sh` script and/or `source
thisEPICS.sh` in your shell.

You can also use the `PiGLETManager` in `scripts`, which issues the
telnet commands for you. Try

    ./scripts/PiGLETManager activate Test

Then, if you want to develop with us, we highly recommend using
QtCreator, although nothing Qt-related is done here.


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
