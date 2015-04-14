To Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add command so anyone can go loner.
  - Create a branch that is bare bones (one area, skills and spells removed, etc so that you can 
    start with a working code base that literally has to be built from the ground up with the 
    exception of the plumping for socket server, pfile handling, area loading, that are necessary
    for it to run)
  - Keep lope's color but removed the other customized colors.  This can predominately be done via clients these days and something 
    is buggy with it, throwing memory allocation errors.  Also, compare this to the older versions of lope's code.  There a lot of stuff
    going on with this version that I don't find useful.  See if I can cherry pick the best of both.
  - Make blink in comm.c immortal only.
  - Windows support so the code base can be debugged through Visual Studio 2013.
  - OLC chopped off characters in name in area name after asave area.