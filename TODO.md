To Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add command so anyone can go loner.
  - Create a branch that is bare bones (one area, skills and spells removed, etc so that you can 
    start with a working code base that literally has to be built from the ground up with the 
    exception of the plumping for socket server, pfile handling, area loading, that are necessary
    for it to run)
  - Add Charisma stat (CHA)
  - Fighting Style (Offensive/Normal/Defensive)
  - VNUM conversion from sh_int to int to allow for more than 32,000 vnums.
  - do_reset and aedit_reset collision
  - Statistics.
  - Add bank, determine monetary type to bank (consider getting rid of silver)
  - Add throwing
  - Add ranged spells (fireball)
  - Sector to display color on exits or another mechansim which is togglable (green for forest, blue for water, etc.)
  - Number of connections per boot saved on count as well as most online ever.
  - Add profile command and/or way for user to store some OOC data like email.
  - have do_dump put files in the log directory.
  - Use Alander's random number generator which is the system, remove Furey's code but stash it somewhere for posterity.
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
  - double exp mode
  - double gold mode
  - questing
  - Add red oak village at the base of the mountains for the dwarves.
  - Clan halls for Midgaard, New Thalos, Red Oak and the to be named elven city.
  - Assassin class
  - Ranger class
  - Surge spell for certain classes, allows them to cast at heightened
    level for a much higher mana consumption.  
  - Ocean, thread them together, create coastlines for intial continents.
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
  - Add settings, use OLC to set them.
  - Add game stats that are persisted.  
  - Make a web site (ASP.Net), make the API for it generic so game data can easily be hooked into.
  - Make a character a ghost for 5 ticks after they die, ghosts can't be attacked and use less movement
    to move so a character can get to their corpse easier.
  - Make the guild command a one stop command for all guild actions (putting people in clans, listing
    clans, listing clan recruiters, etc.).
  - Alchemist Class, mage and/or cleric reclass.
  - Spell that allows a mage reclass to lower someone's casting level.
  - Fix creation bug where a partial lookup can cause a group in another group not to be loaded 
    depending on the order they're loaded in (consider making an exact match lookup and making 
    users type in the whole skill/group they want in creation.
  - Ice continent
  - outfit after reclass
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
  - Guild rooms are busted, the logic check fell apart once we added new classes.  To fix, add
    a guild room bit, then if a user tries to move into it then check their guild numbers.  The
    bug I created was because reclasses shared guild rooms with their base class.  This will also
    stop having to loop over all classes and all their guilds every room movement, which after we
    add 20+ classes will be needless overhead (when only currently 8 rooms are guild rooms in the
    game out of 3000+ rooms).
  - rwhere command to find room by keyword and return room name/vnum.
  - Count number of items in the pit + their actual count.
  - Command to globally purge the pits.
  - Look at the auto saving code and make sure it's working right.
  - Add banish faerie fire spell to healers.
  - Check looting
