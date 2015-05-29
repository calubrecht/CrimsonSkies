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
  - Add Charisma stat (CHA)
  - Fighting Style (Offensive/Normal/Defensive)
  - Add pray command that will allow a mortal to send a message for any immortals logged in to see.
  - Better format sockets command.
  - Remove the new notes/boards and revert to the old school note system.  Like it much better.
  - VNUM conversion from sh_int to int to allow for more than 32,000 vnums.
  - do_reset and aedit_reset collision
  - Statistics.
  - Add underwater room_flag, water breathing spell, damage for underwater w/o
  - Add bank, determine monetary type to bank (consider getting rid of silver)
  - Add throwing
  - Add ranged spells (fireball)
  - Dual wield
  - Wear flags
  - Sector to display color on exits or another mechansim which is togglable (green for forest, blue for water, etc.)
  - Number of connections per boot saved on count as well as most online ever.
  - Cleanup display of wiznet on/off options
  - Add profile command and/or way for user to store some OOC data like email.
  - have do_dump put files in the log directory.
  - Use Alander's random number generator which is the system, remove Furey's code but stash it somewhere for posterity.
  - No recall or word of recall for a tick or two after pkill battle.  
  - No recall or word of recall from battle at all.
  - WIN32 doesn't save after the first session, the rename command is failing in the save_char_obj
  - Add tester flag to set a mortal as a tester.
  - Have tester and immortals have no lag on using the save command.
  - Add reclass command and then create first reclass (enchantor).
  - Implement dynamic classes that aren't hard coded in the tables with skills listed that they have.
  - Helper methods to do common tasks, remove all items from a player, remove all affects from a player, remove all pets from a player, etc.
  - bool char_in_room to determine if a character is already in a room.
  - Add enchantor spells, test reclassing better.