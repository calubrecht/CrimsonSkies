To Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add command so anyone can go loner.
  - Add Charisma stat (CHA)
  - Anti-magic areas
  - Add more items to be sold in Midgaard for 15-20 levels.
  - Fighting Style (Offensive/Normal/Defensive)
  - do_reset and aedit_reset collision
  - Add bank, determine monetary type to bank (consider getting rid of silver)
  - Add throwing
  - Add languages
  - Add ranged spells (fireball)
  - Sector to display color on exits or another mechansim which is togglable (green for forest, blue for water, etc.)
  - Number of connections per boot saved on count as well as most online ever.
  - Add profile command and/or way for user to store some OOC data like email.
  - have do_dump put files in the log directory.
  - Helper methods to do common tasks, remove all items from a player, remove all pets from a player, etc.
  - bool char_in_room to determine if a character is already in a room.
  - Enchantor spell - 'bind object', makes it so the object can only be used by the enchantor, this
    spell doesn't translate to other players (e.g. you can't bind another players soul to an object,
    just yours).  Enchantors aren't much as fighters, they will be able to make sure their gear even
    if looted can't be used by others if they so choose).  (maybe overpowered)
  - Update score with telnetga,compact mode, prompt, combine items, brief
  - Code to search through items in bag or pit.
  - Note cleanup, note forward bug
  - questing (Vasigo's questing code)
  - Add red oak village in the center of the Arcanis continent.
  - Clan halls for Midgaard, New Thalos, Red Oak and the to be named elven city.
  - Assassin class
  - Surge spell for certain classes, allows them to cast at heightened
    level for a much higher mana consumption.  
  - Dropped items in ocean sink (for fun, add them to a lost and found pit in the immortals area so they save).
  - Ability to catch a cold if in the cold weather/tundra.
  - Ability to be affected random affects, having more energy, feeling weak, etc.
  - Load class multipliers from file (uncomment out the code in db.c).  Make pc_race_table not a const so it can be updated.
  - Save pc_race_table out to disk and load it in, add OLC for it.
  - gedit - delete a group (test what happens when a player logs in with said group then).
  - Make the game not crash if a player with an invalid class logs in.
  - Retrain command to restart as a base class (consider allowing to reclass back to base class).
  - Set autogold by default on creation
  - Crafting system, intertwine it with my nature code (that I need to dig up and add)
  - Make a web site (ASP.Net), make the API for it generic so game data can easily be hooked into.
  - Make the guild command a one stop command for all guild actions (putting people in clans, listing
    clans, listing clan recruiters, etc.).
  - Alchemist Class, mage and/or cleric reclass.
  - Spell that allows a mage reclass to lower someone's casting level.
  - Fix creation bug where a partial lookup can cause a group in another group not to be loaded 
    depending on the order they're loaded in (consider making an exact match lookup and making 
    users type in the whole skill/group they want in creation.
  - add another affect field that is in the smaug style with xis_set
  - add enabled field on class that allows the class to be enabled/not, this
    will affect whether people can actively reclass or take a class.  It will
    allow a class to exist without it being generally available.
  - Fix the smaug extended bitvector stuff.  The declarations need to be moved up higher
    in merc.h so they are declared before EXT_BV is used in places like CHAR_DATA.
  - Barbarian class - 4th attack, warcry, acute vision, bash bonus, no magic, spell vulnerable
  - Bladesinger reposition skill.
  - When examining a pit/shelves/container, make it clear what the character can and can't wear.
  - rwhere command to find room by keyword and return room name/vnum.
  - Look at the auto saving code and make sure it's working right.
  - Check looting
  - Add guild room names and area names to class command.
  - Update do_ofind to have two columns like olc olist
  - Update do_mfind to have two columns like olc mlist
  - Add class/race multipliers to OLC.
  - Implement MCCP
  - duress skill that allows player to do more damage if they are towards death.
  - Add a field to type item type field of whether it can be set via OLC, then add OLC support
    to check that field before setting it.  This will allow for the creation of items with a
    type via code that you may not want people creating via OLC (like a healer's bind).
  - Move base string functions into a new strings.c file (separate from the olc strings in olc_string).
  - Update get_random_room in some manner to allow for getting a random room in an area, a continent or the world.
  - different versions of teleport, area, continent, world
  - pfile saving fixed on windows, now fix every other instance that uses rename when saving (OLC, pits, stats, all of them)
  - Try to port Nick Gammon's windows service code.  
  - Try to port Tyche's copyover code from Murk++ for a Windows copyover.
  - Implement and debug Moosehead macro's
  - make blind fighting work in more skills.
  - Update butcher to show what is being butchered, don't let someone butcher something of the same race (it's creepy).
  - Update help file and make a level -1 entries 0 or 1 (this will ensure their keywords are displayed with the entry)
  - There is a bug where I saw two janitors in a room, repeatedly picking things up but never taking
    them out of the room, the log was littered with null in_room from each take in obj_from_char (and spec_janitor).  Find and fix.
  - Make do_worth show global worth of all players logged in for immortals and/or make a world
    stats command that shows.
  - Update the interp table with a flag to indicate whether a command is a skill.
    Then, update the commands command to group regular commands and then skills (then don't show skills if a user doesn't have them).
  - Research why all items have 0 condition.
  - Look at capitalization in names (e.g. disallow it, make it with capitalize).
  - Create command to reset a users password while they maybe offline
  - Make a function that will make a player visible with ACT messages from all invis states then implement it where that code is repeated.
  - Function to count an item type in a room, one for area (can be used to quickly make sure an item doesn't already exist in a room, like a campfire, etc).
  - Make sha256_crypt cross platform compatible (endian stuff)
  - Prune note spools, currently old notes don't load but they are kept in the file.
  - Task scheduler in game, to schedule any number of commands by minute, hour, day, time, etc.
  - Auto quit option to try to auto quit when a player goes link dead.
  - Current online time
  - When you follow someone indicate what direction you are following them.
  - Prepare herb skill for rangers.
  - noob command to toggle on and off noob tips or additional info prompts (which can be created).
  - Setting to turn on/off player looting
  - Max character length for note as const or setting.
  - Consider moving condition into it's own function.
  - Move shared fight skills into a file (leave combat system in fight.c).
  - Clipboard command to allow a character to save notes.
  - Move all login menu functions into login-menu.c
  - Update name generator to find the maximum size of a name that's available and log it on startup.
  - Reformat random names on the main screen to be the same length and columns as whose online.
  - Quest check, to make sure that all quest items that need to exist do exist (or put null checks on those items
    when their loaded and report them if they fail / don't crash).
  - fix (An Imm) when tells sometimes appearing when it shouldn't, copyover fixes.
  - look in portal
  - snoopinfo command or imm who that shows more detail on the who list.
  - migrate skill_lookup calls to gsn_ checks for performance, especially in heavy hit spells like cancel and dispel
  - Escape skill that will allow Rogue's one no fail flee (but can't be used again for a few ticks).  
