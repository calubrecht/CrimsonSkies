# CS-Mud 1.0 Documentation

### Setup for Raspbian Linux

This tutorial will assume that you have your Raspbian Linux server setup with the ability to SSH in.

#### Prerequisites

1. Raspbian OS for the Raspberry Pi (a derivative of Debian) 
2. Update apt-get (**sudo apt-get update**)
3. Git (**sudo apt-get install git**)
4. gcc compiler (**sudo apt-get install gcc**)
5. cshell (**sudo apt-get csh**)
6. make (**sudo apt-get install make**)

#### Setup Steps

1. Login to your server with an SSH account.
1. Clone the git repository into the directory of yours choice (git clone https://github.com/blakepell/CrimsonSkies cs-mud)
1. Change directory to the "src" directory in the repository you just cloned.  If you are using my naming conventions it would be "cd cs-mud" followed by "cd src".
1. Execute the "make" command to build the code.
1. After the build succeeds navigate to the area directory "cd ../area"
1. Set the startup file to have execute permissions "chmod +x startup"
1. Start the mud for the first time "./startup &" (You will see the process number next to it when successful)
    1. The mud should have created a log file in the ../log directory (it starts and 1000 and counts up each time the mud boots)
    2. The first time the mud boots a few warnings will be in the log files because there are no settings yet, this is Ok.
    3. If you see "The game is ready to rock on port 4000." in your log file the mud started successfully.
1. Test a connection to your mud locally, execute "telnet localhost 4000" or "telnet 127.0.0.1 4000" and you should be greeted with the login menu.
    1. If you are hosting in the cloud, port 4000 may not be open and you will need to open it (for example, it was open when I used a physical server at my house, on an Azure VM I had to create an open an end point that allowed connections to pass through).
    2. From the login menu, create a new character (this will be a level 1 character and we will edit your pfile to make you an immortal or game admin)
    3. After creating, quit the mud and type "nano ../player/-your player name-".  Format is crucial in this file so just change the field values.  Change Level to 60 and Security to 10.  (Control-X to exit and save from Nano)
    4. telnet again to the game (telnet localhost 4000) and login again, you are an immortal at the maximum level (type 'wiz' to see all your new immortal commands)

[Back to Table of Contents](index.md)
