Core Flight Executive Open Source Release Readme

cFE Release 6.4.2 

Date: 
July 14, 2015

Introduction:
 The Core Flight Executive is a portable, platform independent embedded system 
 framework developed by NASA Goddard Space Flight Center. This framework is 
 used as the basis for the flight software for satellite data systems and 
 instruments, but can be used on other embedded systems. 
 The Core Flight Executive is written in C and depends on another software
 library called the Operating System Abstraction Layer (OSAL). The OSAL
 is available at http://sourceforge.net/projects/osal/ 
 and github.com/nasa/osal

 This software is licensed under the NASA Open Source Agreement. 
 http://ti.arc.nasa.gov/opensource/nosa

 The Core Flight Executive consists of the following subsystems:
  - Executive Services - initializes and controls applications
  - Software Bus - A publish and subscribe messaging system based
                   on CCSDS command and telemetry packets
  - Time Services - Manages system time
  - Event Services - Event reporting and logging services for applications
  - Table Services - Data/parameter load and update services for applications  

 The Core Flight Executive is intended to host a suite of applications
 and libraries. The applications and libraries are not included in this 
 distribution. A sample library and sample application are included to 
 help verify that the build and runtime are configured correctly. 
 
Software Included:
 Core Flight Executive ( cFE ) 6.4.2
 Platform Support Package 
 Core Flight System Mission Build system 

 Applications:
    Sample Library -- an example of a CFS library
    Sample App -- an example of a CFS application
    CI_LAB -- A test application to send CCSDS Command packets to 
             the system over UDP/IP
    TO_LAB -- A test application to subscribe to and send CCSDS 
              telemetry packets over UDP/IP
    SCH_LAB -- A test application to schedule housekeeping 
               packet collection.

 Simple Command and Telemetry utilities

    The command and telemetry utilities included in the cFS-GroundSystem under the 
    cfe/tools directory provide a basic ground system for desktop testing via UDP/IP 
    connections to ci_lab and to_lab. The utilities use python 2.x and the QT 4.x GUI 
    libraries. The forms were designed in the QT4 designer program. The python bindings 
    used are the PyQT4 bindings. On Ubuntu 12.04 LTS for example: use the following 
    command to install the software needed to run the utilities:
    $ sudo apt-get install python-qt4 pyqt4-dev-tools
    
    cmdUtil -- A simple command line utility to format and send CCSDS command over
               UDP/IP to the CI_LAB application. Located in cfe/tools/cmdUtil.
               Before using cmdGui, run "make" in this directory to build the 
               cmdUtil binary.

    cmdGUI --  A simple python/QT4 based utility that allows you to define and 
               send commands using the cmdUtil program. 
               cmdGui is started by running:
               $ python ./CommandSystem.py

    tlmGUI -- A simple python/QT4 based utility that allows you to define and
               display telemetry received from the TO_LAB application.
               tlmUtil is started by running: 
               $ python ./TelemetrySystem.py 
 
Software Required:

 Operating System Abstraction Layer 4.1.1 or higher
    Can be obtained at http://sourceforge.net/projects/osal
     or github.com/nasa/osal

 Supported Build and Runtime Environment:

 Build Environment Supported:
  This software is built on CentOS/RHEL 6.x x86 ( 32 bit ) 
 
  It should be possible to build on other recent linux distributions
  ( Debian, Ubuntu, SUSE, etc ). It should also build and run
  as a 32 bit application on a 64 bit system.  
  This release was compiled on CentOS 6.5-x86, CentOS-7.0-x64, 
  Ubuntu 12.04 LTS x86, Ubuntu 14.04 LTS x64. 
  Note: On Ubuntu 14.04, the cFE unit tests do not work due to 
        differences in the GCC compiler. 

  It should be possible to build on Windows and Mac OS X, but is
    not currently tested. The platform support packages (psp) for
    mac-osx and pc-cygwin are out of date and will probably not 
    work. 

Runtime Targets Supported:
   The "out of the box" targets in this distribution include:
     1. 32 bit x86 Linux ( CentOS 6.x )
     2. Motorola MCP750 PowerPC vxWorks 6.4

Other targets: 
    Other targets are included, but may take additional work to
    run. They are included as examples of other target 
    environments.
    1. mcf5235-rtems - This is for the Axiom MCF5235 Coldfire board running
                       RTEMS 4.10. It requires a static loader component for the
                       OS abstraction layer. The static loader is currently
                       not available as open source, so this target is not
                       considered complete. RTEMS 4.11 will have a dynamic
                       loader which will be supported by a future release
                       of the OS Abstraction Layer, completing the RTEMS support
                       for the cFE.
          
                       Once RTEMS 4.11 is released, the goal is to support
                       an RTEMS simulator platform such as SPARC/sis or 
                       quemu.

    2. mac-osx and pc-cygwin - These targets are included for examples. They
                       will most likely be removed in the future. 
                       The pc-linux target is highly recommended. If you 
                       have a windows or mac computer it is relatively easy to 
                       set up a free virtual machine environment with 32 bit 
                       Cent OS 5.x.

Quick start: How to build and run the cFS software on Linux:

cFS has been built and run on Ubuntu 13.10 x64 and CentOS 6.5 i386 however,
the following instructions below assume you are in a CentOS/RHEL terminal. 
The cFE distribution file is in a directory called "Projects".

Unpack the cFE distribution

  [cfe@localhost ~]$ cd Projects
  [cfe@localhost Projects]$ tar -zxf cFE-6.4.1-OSS-release.tar.gz
  [cfe@localhost Projects]$ cd cFE-6.4.1-OSS-release

  Unpack the OSAL distribution ( obtain from sources above )
  Assuming the file is in your Projects directory
  ( example: /home/acudmore/Projects/osal-4.1.1-release.tar.gz)

  [cfe@localhost cFE-6.4.1-OSS-release]$ tar -zxf ../Projects/osal-4.1.1-release.tar.gz
  [cfe@localhost cFE-6.4.1-OSS-release]$ mv osal-4.1.1-release osal

  Build the cFE source code

  NOTE: In the first command, make sure you enter a '.' then a space 
        then './setvars.sh' !!!
        ( see the bash command "source" )

  [cfe@localhost cFE-6.4.1-OSS-release]$ . ./setvars.sh
  [cfe@localhost cFE-6.4.1-OSS-release]$ cd build
  [cfe@localhost build]$ cd cpu1 
   ( Note: cpu1 is configured for the pc-linux target ) 
  [cfe@localhost pc-linux]$ make config
   ( Note: this copies all of the needed configuration make and header files ) 
  [cfe@localhost pc-linux]$ make     
   ( Note: this builds the OSAL, cFE, and apps ) 

  When the build completes, the cFE core executable that runs on on CentOS is i
  in the exe directory. It can be run by doing the following:
   
  [cfe@localhost cpu1]$ cd exe
  [cfe@localhost exe]$ ./core-linux.bin 

  At this point, the linux version of the cFE will start running. Note that the 
  OSAL uses POSIX message queues to implement the inter-task communication 
  queues.
  
  The cFE by default needs a larger "msg_max" parameter in linux to run. There 
  are two solutions to this problem:
    1. increase the /proc/sys/fs/mqueue/msg_max parameter 
    2. run the cFE core as root
       [cfe@localhost exe]$ su
       ( type in root password )
       [cfe@localhost exe]# ./core-linux.bin

  Root/sudo is also required to enable the linux real time scheduler for
  more accurate priorty control. 

  With the cFE starting, it should initialize, then read the cFE startup 
  script and load the library and applications.  The following apps/libraries 
  are integrated and running:
  
       SCH_LAB - simple software scheduler
       CI_LAB - UDP command packet receiver
       TO_LAB - UDP telemetry packet transmitter
       SAMPLE_LIB - Sample library
       SAMPLE_APP - Sample "Hello World" application   
 
  New applications can be added by editing the cfe_es_startup.scr file.
  Stop the cFE and all applications by hitting control-c in the terminal.
  
  NOTE: After quitting the cFE with a Control-C signal, it MUST be started 
  next time with a Poweron Reset ( --reset PO ).

  When the cFE and CFS applications are running, you can start the 
  CommandSystem.py and TelemetrySystem.py and try some simple commanding.

-----------
Commands and Telemetry:

Before trying to use this, make sure you have python 2.x, QT4, and PyQt4
installed. Note: The QT4 library is only needed to add new buttons and/or
graphical features to the GUI.

Commands:
   Commands can be sent using the scripts in the 
   /cfe/tools/cFS-GroundSystem/Subsystems/cmdUtil directory or python GUI in 
   the /cfe/tools/cFS-GroundSystem directory.

   To try out the pre-defined commands with the python gui, follow the 
   instructions contained in /cfe/tools/cFS-GroundSystem/Guide-GroundSystem.txt
   
   make sure the command utility is compiled and the executable permissions
   are set:
   
   $ cd $CFS_MISSION/cfe/tools/cFS-GroundSystem/Subsystems/cmdUtil
   $ make
   $ chmod u+x cmdUtil
   
   Run the python gui:
   
   $ cd ../../cFS-GroundSystem
   $ python GroundSystem.py
   
   Note: the executable permissions may need to be set on the GroundSystem.py 
   script.

   From the gui, open the command system by clicking the "Start Command System" button.
   This will bring up a display page to bring up the command display pages for the various
   applications.  The "Send" button will send the command to the cFS system or open an
   additional page for entering command parameters.  After telemetry is enabled you should 
   see feedback from the commands in the cFS terminal. 
  
   To enable telemetry, be sure to open the command system by clicking the 
   "Start Command System" button and hit the "Enable Tlm" button for the 
   TO Enable Tlm (localhost) command. 

   The commands are actually shell commands that use the CmdUtil binary. 

   To execute a command from the command line do the following:

   ( in "cmdUtil" directory ) 
   $ ./to-enable-tlm.sh
   
   Note: make sure the executable permissions are set on the script file(s).
   
   The above command will send the Telemetry Output LAB applications's 
   TO Eanble Tlm (localhost)command followed by a Telemetry Output LAB application 
   No Operation (NOOP) command. 

   The  python command page was created using the QT designer (QT4 ) application.
   The python scripts use the PyQt4 interfaces between Python and QT4. These libraries are 
   available for both CentOS 6.x and more recent Ubuntu based distributions.
   
   There are several "unused" Subsystem/Pages on the Command System Main Page for adding 
   additional Subsystems/Pages.

   To add a new Subsystem/Page to the python gui, follow the instructions contained in 
   /cfe/tools/cFS-GroundSystem/Guide-GroundSystem.txt for "ADDING NEW FLIGHT SOFTWARE 
   APPLICATION TO GROUND SYSTEM COMMAND GUI" 
      
   If all the "unused" pages have been populated in the Command System Main Page and a new
   entry needs to be added:

   1. Run the Qt Designer application by clicking Applications->Programming->Qt4 Designer
   2. In the Qt4 Deigner program, on the bottom left of the screen, click Open and open the 
      "CommandDialog.ui" file under the CFS/commands directory.
   3. Add an Entry/Push Button by copying and pasting one of the existing buttons or dragging 
      and dropping a Push Button from the Widget Box located on the left hand side of the 
      Qt Designer (Note: Widget Box window may need to be expanded).
   4. Make note of the the new button's name displayed in the Object Inspector window i.e 
      "pushButton_16".  This name will be used in updating the CommandDialog.py script 
      file.
   4. Double click on the new button to change the name to describe the new command
   5. Save the "CommandSystemDialog.ui" file and exit the Qt4 Designer program.
   6. From the /cfe/tools/cmdGui directory run:
      
      "pyuic4 -o CommandSystemDialog.py CommandSystemDialog.ui"   
   
-----------

Telemetry:
   Telemetry is sent from the running cFS as CCSDS telemetry packets encapsulated in 
UDP/IP packets. The "enable telemetry" command tells the TO_LAB application to start 
sending packets to a UDP port on the "localhost" IP. 

Start the telemetry system by clicking the "Start Telemetry System" button on the
main window of the cFS Ground System
  
Note: the executable permissions may need to be set on the TelemetrySystem.py 
script.  

"Display Page" Buttons are available for individual telemetry pages. 
New pages can be added by adding to the "telemetry-pages.txt" text file. 
The format is not very forgiving, and must be:
Descripton,     Name of python class,     Telemetry Message ID,  packet
definition file

Also, a corresponding telemetry packet definition file must be added. For
example: 

   cfe-es-hk-tlm.txt is for the cFE Executive Services housekeeping packet.
 
That file contains the information needed to decode and display each data
field of the packet. This is comma delimited text, and has no error checking. 
The fields are:

   1. Field Descripton -- This is put on the page
   2. Byte offset in packet -- This is the starting byte for the data field
   3. Length of data field in packet -- For integer types it is usually 1, 2, or 4
                              For strings, it is a fixed length like 16
   4. Python type  -- Python type for the field
                             B = unsigned byte
                             I = unsigned 32 bit int
                             H = unsigned Hex
                             S = String
Floating point types are not supported yet, but are planned. See all of the types 
here:

http://docs.python.org/2/library/struct.html#format-characters

   5. Display type -- This is how the data is displayed on the page
                    Dec = Decimal
                    Hex = Hexidecimal
                    Enm = Enumerated Type ( see next four fields )
                    Str = String

   6. Enumerated type 0 -- This string is displayed if the display type is Enm
      and the value is 0
                                              
   7. Enumerated type 1 -- This string is displayed if the display type is Enm
      and the value is 1

   8. Enumerated type 2 -- This string is displayed if the display type is Enm
      and the value is 2

   9. Enumerated type 3 -- This string is displayed if the display type is Enm
      and the value is 3

Where to find more info:
  There is much more information that is beyond the scope of a readme file.

  The current documents can be found in the following directories:
  cFE-6.4.1-OSS-release/docs
  cFE-6.4.1-OSS-release/osal/docs ( once the OSAL is installed )
  cFE-6.4.1-OSS-release/cfe/docs

  cFE-6.4.1-OSS-release/cfe/docs/doxygen/index.html -- a good place to start
         for the cFE

EOF
