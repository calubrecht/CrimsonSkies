To Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add command so anyone can go loner.
  - Create a branch that is bare bones (one area, skills and spells removed, etc so that you can 
    start with a working code base that literally has to be built from the ground up with the 
    exception of the plumping for socket server, pfile handling, area loading, that are necessary
    for it to run)
  - OLC chopped off characters in name in area name after asave area.
  - Add Charisma stat (CHA)
  - Fighting Style (Offensive/Normal/Defensive)
  - Better format sockets command.
  - VNUM conversion from sh_int to int to allow for more than 32,000 vnums.
  - do_reset and aedit_reset collision
  - Statistics.
  - Add underwater room_flag, water breathing spell, damage for underwater w/o
  - Add bank, determine monetary type to bank (consider getting rid of silver)
  - Add throwing
  - Add ranged spells (fireball)
  - Dual wield
  - Pit saving
  - Sector to display color on exits or another mechansim which is togglable (green for forest, blue for water, etc.)
  - Number of connections per boot saved on count as well as most online ever.
  - Cleanup display of wiznet on/off options
  - Add profile command and/or way for user to store some OOC data like email.
  - have do_dump put files in the log directory.
  - Use Alander's random number generator which is the system, remove Furey's code but stash it somewhere for posterity.
  - No recall or word of recall for a tick or two after pkill battle.  
  - No recall or word of recall from battle at all.
  - WIN32 doesn't save after the first session, the rename command is failing in the save_char_obj
  - Add tester flag to set a mortal as a tester. (have show damage and save throws in battle, etc).
  - Have tester and immortals have no lag on using the save command.
  - Implement dynamic classes that aren't hard coded in the tables with skills listed that they have.
  - Helper methods to do common tasks, remove all items from a player, remove all affects from a player, remove all pets from a player, etc.
  - bool char_in_room to determine if a character is already in a room.
  - Test reclassing
  - group obj/separate_obj from smaug for more efficient object storing.
  - Enchantor spell - 'bind object', makes it so the object can only be used by the enchantor, this
    spell doesn't translate to other players (e.g. you can't bind another players soul to an object,
    just yours).  Enchantors aren't much as fighters, they will be able to make sure their gear even
    if looted can't be used by others if they so choose).  (maybe overpowered)
  - Update locate for wizard mark
  - enhanced recall skill, open to all classes that only makes recall 25% of movement.
  - Update score with telnetga,compact mode, prompt, combine items, brief
  - Code to search through items in bag or pit.
  - Note cleanup, note forward bug, note mem leak testing
