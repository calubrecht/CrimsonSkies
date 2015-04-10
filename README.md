Crimson Skies
=============

Crimson Skies is an open source MUD based on ROM 2.4b6/QuickMUD.  All new changes
will be documented in the change log. 

Current Crimson Skies compiles and runs out of the box on:

  - Ubuntu 14.04
  - Rasbian for use with the RaspberryPi

## Project Goals:

  - To provide a ROM code base that includes creative additions that keep the same
    feel of what made ROM great.
  - To provide modern tools and hooks into ROM to allow it to integrate with services
    not imagined in the early 90's.  One feature will be a client based area build that
    will allow for dragging and dropping over rooms on a map (I had originally completed
    a program that did this back in 2000 and I will start from that concept).
  - I may use ROM as a base to port the code to a modern language like C# that will run on
    both Linux (through Mono) and Windows and implement an easy to use modern database. 
  - I will be using both Nano on the Linux side and Visual Studio on the Windows side to
    program for this (you can use anything you want if you decide to edit).  Visual Studio 2013 
    allows some pretty cool intellisense which makes coding in C in this project way friendly 
    than back in the day when I was using solely pico.  The solution currently doesn't compile 
    under Windows however.
  - I will likely be working on alternate tools for building areas via a GUI where you can
    drag and drop rooms to draw maps quickly and then fill in the text info.  I may work on
    a mud client that functions like a traditional but also integrates additional input methods
    like voice recognition and joystick support (e.g. you would be able to use a standard game
    controller to move and map commands to the different buttons and they communicate via a head
    set).

## Resources

Unfortunately over the years the best resources have been lost to time.  I will try to
provide new documentation and archive old documentation as I can find it.  The best 
old resource was the rom mailing list but alas it does not exist at it's old locations
which were at:

  - http://www.the-infinite.org/lists/romlist
  - rom-request@rom.org

## License 

I have no specific license for my code at this time, but as this is a ROM deritivate it must 
adhear to the licenses set before it (specifically the ROM, Merc and Diku licenses).  

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