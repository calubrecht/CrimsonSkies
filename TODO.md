To Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add command so anyone can go loner.
  - Create a branch that is bare bones (one area, skills and spells removed, etc so that you can 
    start with a working code base that literally has to be built from the ground up with the 
    exception of the plumping for socket server, pfile handling, area loading, that are necessary
    for it to run)
  - Make blink in comm.c immortal only.
  - OLC chopped off characters in name in area name after asave area.
  - _WIN32 waits for input when someone connects but doesn't enter anything, if something is sent to
    the buffer the entire mud hangs until that person enters something.  This behavior isn't happening
    on Linux.
  - d->ANSI is getting blown away on copyover causing send_to_desc to not color code things properly.
  - Add Charisma stat (CHA)
  - Fighting Style (Offensive/Normal/Defensive)
  - Add pray command that will allow a mortal to send a message for any immortals logged in to see.
  - Better format sockets command.
  - change all doors to use MAX_DOOR instead of hard coded integer
  - Remove the new notes/boards and revert to the old school note system.  Like it much better.
  - Remove the title table, let all players set their own title and only set the initial one for them.
  - Command to list all rooms/vnums, objects/vnums and/or mobs/vnums in an area to assist in OLC.
  - VNUM conversion from sh_int to int to allow for more than 32,000 vnums.
