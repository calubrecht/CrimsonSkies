Crimson Skies
=============

Crimson Skies is an open source MUD based on ROM 2.4b6 with some features included from other Diku family muds such as Smaug and ROT.  

All new changes will be documented in the change log (The last few weeks of changes have been in the reclass branch where the reclass system and the first reclass enchantors are being coded). 

Currently Crimson Skies compiles and runs out of the box on:

  - Ubuntu 14.04
  - Rasbian for use with the RaspberryPi
  - Windows 7, Windows 8 & Windows 8.1 (compiles with Visual Studio 2013)

Crimson Skies will likely also compile on Debian with little if any changes (as Raspbian is a deriative of Debian).

The current development server which is playable and will reflect the latest commits here is at (it will start 
eloving from stock ROM'ish while I work through initial changes):

  - 66.244.102.112 port 4000

This isn't advertised with Mud Connector or anywhere else, just here for anyone interested while I work on it.  The first things 
I will be working on is cleaning up formatting, removing some small stuff I didn't think fit an RP mud (like the Jukebox) and 
then I'll focus on new classes, reclasses, reclass reclasses, spells and skills.  I will be altering the underpinnings before 
making many area additions.  

## Project Goals:

  - To provide a ROM code base that includes creative additions that keep the same
    feel of what made ROM great (I will be changing and re-organizing things though) and will probably
    remove or drastically change some areas that don't fit imo (like the smurf village).
  - To document the old functions with descriptions and hopefully well document new functions.
  - Remove legacy OS support and support the major OS's of today (Linux via Ubuntu/Raspbian/Debian, 
    Windows via Windows 7/8/8.1 and likely 2008R2 Server+ and possibly OSX though my OSX test laptop is
    pretty old and I'm not getting a new one for cost reasons).
  - To provide modern tools and hooks into ROM to allow it to integrate with services
    not imagined in the early 90's.  One feature will be a client based area build that
    will allow for dragging and dropping over rooms on a map (I had originally completed
    a program that did this back in 2000 and I will start from that concept).
  - I will be using both Nano on the Linux side and Visual Studio on the Windows side to
    program for this (you can use anything you want if you decide to edit).  Visual Studio 2013 
    allows some pretty cool intellisense which makes coding in C in this project way friendlier
    than back in the day when I was using solely pico.  The solution will compile and is fully
    debugable with Visual Studio which means you can run it and when it crashes you'll be sitting
    on the line of code that caused it when everything or you can easily step into, over and out
    of code.  
  - I will likely be working on alternate tools for building areas via a GUI where you can
    drag and drop rooms to draw maps quickly and then fill in the text info.  I may work on
    a mud client that functions like a traditional but also integrates additional input methods
    like voice recognition and joystick support (e.g. you would be able to use a standard game
    controller to move and map commands to the different buttons and they communicate via a head
    set).  Gammon forums has a Windows GUI program to create ROM and Smaug area files.  It is located at
    http://www.gammon.com.au/downloads.htm.  When I re-write mine I will be including a drag and drop editor
    to graphically create the room via a map (which will then make it easier to export map files in different
    formats).
  - To provide a branch that is ROM stripped down to the socket server that can be repurposed for other things.
  - To provide a base branch that has the areas and code stripped down to one area (and any code alterations to 
    remove vnum usage that uses vnums in other areas, this will allow for people starting a mud from scractch 
    building their own areas.)

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

## Thank you

I want to send a special thank you to Tony Allen who runs Dark and Shattered Lands (DSL) at
http://www.dsl-mud.org .  I coded (as Rhien) on his mud from 1999-2003 and it was there
that I really learned C and to this day I credit it with helping me along in the programming
profession.  I highly suggest checking out his MUD if you have time.  This mud code base contains no code
that was written for DSL.
