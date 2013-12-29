arduino-projects
================

This repository contains all my projects experiments on Arduino.

All projects are created on [Eclipse](http://www.eclipse.org/) with an [Arduino plugin](http://www.baeyens.it/eclipse/), which I find easier to use when coding more than a dozen lines of C++ as errors are more easily found and fixed through Eclipse.
Hence every project can be directly compiled under the same environment but will need some adaptations to be compiled by the usual Arduino IDE. These adaptations are always the same for every project:

1. change the main project .CPP file extension into .INO
2. in the new .INO file, remove the first include line #include "ProjectName.h"

If a project uses one my own libraries, it can be found on my github arduino-libraries repository.

For my electronics circuits design, I use 2 useful applications, they are not free but they are quite cheap and very easy (and quick) to use:

- for diagrams, I use [sPlan 7.0](http://www.abacom-online.de/uk/html/splan.html) 
- for stripboards design and wiring, I use [LochMaster 4.0](http://www.abacom-online.de/uk/html/lochmaster.html) for the stripboard circuit. 

If you don't want to buy sPlan or LochMaster, you can still open these files with free viewers for [sPlan](http://www.abacom-online.de/updates/sPlan70_Viewer.exe) and [LochMaster](http://www.abacom-online.de/updates/LochMaster40_Viewer.zip).
