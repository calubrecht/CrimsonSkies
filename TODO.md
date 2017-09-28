wTo Do List
==========

This is my to do list, mostly will be things big and small as I think about them.

  - Add custom recall field for each clan in the tables struct.
  - Add luck stat
  - Anti-magic areas
  - Add more items to be sold in Midgaard for 15-20 levels.
  - do_reset and aedit_reset collision
  - Add throwing
  - Add languages
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
  - Clan halls for Midgaard, New Thalos, Red Oak and the to be named elven city.
  - Surge spell for certain classes, allows them to cast at heightened
    level for a much higher mana consumption.  
  - Dropped items in ocean sink (for fun, add them to a lost and found pit in the immortals area so they save).
  - Ability to catch a cold if in the cold weather/tundra.
  - Ability to be affected random affects, having more energy, feeling weak, etc.
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
  - make blind fighting work in more skills.
  - Update butcher to show what is being butchered, don't let someone butcher something of the same race (it's creepy).
  - Update help file and make a level -1 entries 0 or 1 (this will ensure their keywords are displayed with the entry)
  - There is a bug where I saw two janitors in a room, repeatedly picking things up but never taking
    them out of the room, the log was littered with null in_room from each take in obj_from_char (and spec_janitor).  Find and fix.
  - Make do_worth show global worth of all players logged in for immortals and/or make a world
    stats command that shows.
  - Research why all items have 0 condition.
  - Look at capitalization in names (e.g. disallow it, make it with capitalize).
  - Create command to reset a users password while they maybe offline
  - Make a function that will make a player visible with ACT messages from all invis states then implement it where that code is repeated.
  - Function that makes someone invisible with no act messages.
  - Function to count an item type in a room, one for area (can be used to quickly make sure an item doesn't already exist in a room, like a campfire, etc).
  - Make sha256_crypt cross platform compatible (endian stuff)
  - Prune note spools, currently old notes don't load but they are kept in the file.
  - Task scheduler in game, to schedule any number of commands by minute, hour, day, time, etc.
  - Auto quit option to try to auto quit when a player goes link dead.
  - Current online time
  - Prepare herb skill for rangers.
  - noob command to toggle on and off noob tips or additional info prompts (which can be created).
  - Setting to turn on/off player looting
  - Max character length for note as const or setting.
  - Consider moving condition into it's own function.
  - Move shared fight skills into a file (leave combat system in fight.c).
  - Clipboard command to allow a character to save notes.
  - Update name generator to find the maximum size of a name that's available and log it on startup.
  - fix (An Imm) when tells sometimes appearing when it shouldn't, copyover fixes.
  - locate creature (similiar to locate object).
  - Ability to search note body's like subjects.
  - Delete expired notes by calling the save function, either on load or reboot.
  - Update locate object to additionally show the area the object is in.
  - Healer spell to create a healing crystal that can be given out, cool down on it
  - Move the code to clear all flags on disenchant or fade into it's own helpler, have those spells call that common code for weapons.
  - Bug saving helps.. if a new help is created with OLC and saved it will wipe the current area out.
  - Wiznet, option to log also (research this)
  - Revolt - Make it work against charmies but only if both the ch and master are clanned. (will need to look into is_safe).
  - Get rid of ban_permit, keep plr_permit.
  - Bans to check both d->host and d->ip_address
  - Reformat socket command, show both d->host and d->ip_address
  - Preserve group after copyover
  - Critical strike
  - Sliding saves bonus based on wisdom
  - Sliding magic damage bonus based on int
  - Extra chance attack on init based on dex (higher the dex, the higher the percent)
  - Sliding damage bonus based on strength for physical attacks (higher the strength, higher the bonus)
  - Pscionist spell to temporarly wipe facial recognition of who a player is.
  - Help file update for notes to display supported spools
  - Command to list the spools available, with properties to denote immortal only
  - rename player offline method.
  - bamfin/bamfout ->poofin/poofout (to match the commands used in the game)
  - Documentation, ugg.
  - No magic area flag
  - Fix formatting string removing line breaks.
  - Find out why strings are saving funny (this was fixed in notes)  
  - All coloring on login menu to come from settings so no mismatched colors
  - Update formatting on the stat command, use the grid code.
  - Update ranged spells to support defense spells also (they only support offensive right now).
  - Updated ranged spells to have a custom message for when the spell flies through another room.
  - Update ranged spells to show the direction the spell came from.
  - Don't allow picking up of buried items, Buried items possibly interfere with burying next item.
  - Remove mass invis gsn, add invis flag when it's cast.
  - Psion - Temporal Fracture
  - Create function of all things to be removed in combat/damage.
  - Slippery Mind for psion
  - Remove armor class (AC) from the hit check, reduce damage from it though
  - Room affects
  - pk wholist
  - jab skill for rogues that lowers dex
  - damage adjustment based on int (up or down) on damage casting spells.
  - ArenaKills added to pcdata
  - nobility
  - Make guardian angel spell mob disappear after the effect is gone from the mob.
  - swims in message (floats in).
  - add hitroll_bonus, damroll_bonus to class structure
  - Secondary class stat
  - barb, mud coat skill.
  - enhance pain spell (more damage taken)
  - psionict's chance to avoid psionic damage damage
  - assassin class (strangle, assassinate, flash bomb)
  - assassin surprise attack (moves a person into the next room and attacks in a single command)
  - Merits, make only "choosable" one's availble from creation, update the look to search for both or filter those out.
  - Previous HP to affect your reclass HP
  - Allow logout even in pk lag if the player is in a clan owned room of the clan they are in
  - Priest spell (with cooldown) to prevent a person from entering a room for a tick
  - Area flag that denotes a clan/kingdom hall.
  - Hometown selection in creation
  - Drunk randoms from smaug
  - Drunk in score or affects
  - Whisper command
  - Smoke/fog lowers in quality every tick.
  - Make glance also show magic status if detect magic is up.
  - %t for time in prompt.
  - Sheathed weapon
  - Ability to enter a background history about your character
  - whoisic (shows description, phyiscal appearnce and background).
  - key chain
