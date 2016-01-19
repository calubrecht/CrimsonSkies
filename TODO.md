To Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add command so anyone can go loner.
  - Add Charisma stat (CHA)
  - Anti-magic areas
  - Add more items to be sold in Midgaard for 15-20 levels.
  - Fighting Style (Offensive/Normal/Defensive)
  - VNUM conversion from sh_int to int to allow for more than 32,000 vnums.
  - do_reset and aedit_reset collision
  - Add bank, determine monetary type to bank (consider getting rid of silver)
  - Add throwing
  - Add languages
  - Add ranged spells (fireball)
  - Sector to display color on exits or another mechansim which is togglable (green for forest, blue for water, etc.)
  - Number of connections per boot saved on count as well as most online ever.
  - Add profile command and/or way for user to store some OOC data like email.
  - have do_dump put files in the log directory.
  - No recall or word of recall for a tick or two after pkill battle.  
  - No recall or word of recall from battle at all.
  - WIN32 doesn't save after the first session, the rename command is failing in the save_char_obj
  - Helper methods to do common tasks, remove all items from a player, remove all pets from a player, etc.
  - bool char_in_room to determine if a character is already in a room.
  - Enchantor spell - 'bind object', makes it so the object can only be used by the enchantor, this
    spell doesn't translate to other players (e.g. you can't bind another players soul to an object,
    just yours).  Enchantors aren't much as fighters, they will be able to make sure their gear even
    if looted can't be used by others if they so choose).  (maybe overpowered)
  - Update score with telnetga,compact mode, prompt, combine items, brief
  - Code to search through items in bag or pit.
  - Note cleanup, note forward bug, note mem leak testing
  - gain_exp tweaks
  - main menu
  - questing
  - Add red oak village at the base of the mountains for the dwarves.
  - Clan halls for Midgaard, New Thalos, Red Oak and the to be named elven city.
  - Assassin class
  - Ranger class
  - Surge spell for certain classes, allows them to cast at heightened
    level for a much higher mana consumption.  
  - Dropped items in ocean sink (for fun, add them to a lost and found pit in the immortals area).
  - Ability to catch a cold if in the cold weather/tundra.
  - Ability to be affected random affects, having more energy, feeling weak, etc.
  - Load class multipliers from file (uncomment out the code in db.c).  Make pc_race_table not a const so it can be updated.
  - Save pc_race_table out to disk and load it in, add OLC for it.
  - gedit - delete a group (test what happens when a player logs in with said group then).
  - Make the game not crash if a player with an invalid class logs in.
  - Retrain command to restart as a base class
  - Set autogold by default on creation
  - Allow any player to change their recall to any number of custom bind stones for recall.
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
  - Make lore do something (like give info about the item when looked at)
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
  - auto name generator methods
  - Move base string functions into a new strings.c file (separate from the olc strings in olc_string).
  - Update get_random_room in some manner to allow for getting a random room in an area, a continent or the world.
  - different versions of teleport, area, continent, world
  - pfile saving fixed on windows, now fix every other instance that uses rename when saving (OLC, pits, stats, all of them)
  - Try to port Nick Gammon's windows service code.  
  - Try to port Tyche's copyover code from Murk++ for a Windows copyover.
  - Implement and debug Moosehead macro's
  - add improve check for swim, enhanced recall, blind fighting
  - make blind fighting work in more skills.
  - update dodge and parry, more factors.
