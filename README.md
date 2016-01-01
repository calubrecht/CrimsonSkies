Crimson Skies Mud (CS-Mud)
==========================

Crimson Skies is an open source MUD starting from ROM 2.4b6.  Useful features from other open source code bases
(like Smaug) will be included as well as what will be a lot of custom code.  The first version release is nearing
completion.  Classes, groups, skills and spells are loading from files and all have OLC built in so you can maintain
all of this from the game (adding skills and spells to classes is far easier than in stock ROM code).  

The current development server which is stable and playable and will reflect the latest commits here is at (you can 
telnet or use a mud client like ZMud, CMud or Mudlet, I still use Zmud 4.62 from like 1996 on Windows 10):

  - 66.244.102.112 port 4000

All new changes will be documented in the commit history and I will move all of those into a detailed change lot when I
official go to version 1.0 (version 1 will be official when I finish 2 more reclasses so each base class has a reclass
and when I finish resets of ocean life in the oceans).  The next version on the roadmap will include 4 more reclasses 
and new continent that will begin to build new areas that aren't from the stock Diku/Merc/Rom.  You can check my TODO
file for short and long term things I'm going to work on (and the list is way larger, I just haven't added it all, 
coding on this is a labor of love, I'm glad to be back to it).

Currently Crimson Skies compiles and runs out of the box on:

  - Ubuntu 14.04
  - Rasbian for use with the RaspberryPi
  - Windows 7, Windows 8, Windows 8.1, Windows 10 (compiles with Visual Studio 2013 & 2015, for your initial 
    compile/run in Visual Studio you may need to follow the instructions in this blog post, 
    they are brief: http://www.blakepell.com/how-to-change-working-directory-in-visual-studio-for-a-cc-app)

Crimson Skies will likely also compile on Debian with little if any changes (as Raspbian is a deriative of Debian).

If there are any coders and/or builders (esp builders) who are interested in contributing to an open source mud feel
free to send me a message and get on board.  I am standing up a building port for this reason.  Great muds are a 
combination of the code and the content.  You can't have just one or the other.  Although CS-Mud is newish to
open source/GitHub it has been under private development since 1998 (as my hobby).

## Project Goals:

  - To provide a code base that continues to evolve without losing the Rom feel and balance (whereas many these days 
    have stopped or don't share their code).
  - To provide a ROM code base that includes a world that is setup for pkill or roleplaying (e.g. the things like the
    smurf village have been changed or removed).
  - Server side LUA to suppliment and/or replace standard mob progs (safe, feature rich and little
    chance of memory leaks, yes, yes and yes).
  - To document the old functions with descriptions and hopefully well document new functions.
  - Remove unused code (for instance, skill SLOTS and the pgsn pointer weren't being used so I
    removed them while implementing OLC).
  - To provide modern tools and hooks into ROM to allow it to integrate with services
    not imagined in the early 90's.  One feature will be a client based area build that
    will allow for dragging and dropping over rooms on a map (I had originally completed
    a program that did this back in 2000 and I will start from that concept).
  - I will be using both Nano on the Linux side and Visual Studio on the Windows side to
    program for this (you can use anything you want if you decide to edit).  Visual Studio 2015 
    allows some pretty cool intellisense which makes coding in C in this project way friendlier
    than back in the day when I was using solely pico.  The solution will compile and is fully
    debugable with Visual Studio which means you can run it and when it crashes you'll be sitting
    on the line of code that caused it when everything or you can easily step into, over and out
    of code (without having to use something like gdb).  
  - I will likely be working on alternate tools for building areas via a GUI where you can
    drag and drop rooms to draw maps quickly and then fill in the text info.  I may work on
    a mud client that functions like a traditional but also integrates additional input methods
    like voice recognition and joystick support (e.g. you would be able to use a standard game
    controller to move and map commands to the different buttons and they communicate via a head
    set).  Gammon forums has a Windows GUI program to create ROM and Smaug area files.  It is located at
    http://www.gammon.com.au/downloads.htm.  When I re-write mine I will be including a drag and drop editor
    to graphically create the room via a map (which will then make it easier to export map files in different
    formats).

## Resources

Unfortunately over the years the best resources have been lost to time.  I will try to
provide new documentation and archive old documentation as I can find it.  The best 
old resource was the rom mailing list but alas it does not exist at it's old locations
which were at:

  - http://www.the-infinite.org/lists/romlist
  - rom-request@rom.org

Here are some related resources that are thanksfully still active.

  - Mud Bytes - http://www.mudbytes.net/
  - Gammon Forums (Smaug & ROM) - http://www.gammon.com.au/forum/
  - Erwin Andreasen's page (though out dated is important, if only for his sharing of copyover): http://www.andreasen.org/mud.shtml

## License 

I have no specific license for my code at this time, but as this is a ROM/Merc/Diku deritivate it must 
adhear to the licenses set before it from those code bases.  

## Contact Me

If you wish to contact me you can contact me through my web-site and it will route it to
my current preferred email:

 - http://www.blakepell.com/Contact.aspx
