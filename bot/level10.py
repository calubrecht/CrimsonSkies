import sys

# Through Mud School Training, level 5 to 10

# Literal Triggers
triggers = {
    # Login/Connecting
    "Do you want color? (Y/N) ->" : "N",
    "By what name do you wish to be known?" : sys.argv[1],
    "Password:" : sys.argv[2],
    "Do you wish to connect anyway (Y/N)" : "Y",
    "[Hit Return to continue]" : "",
    "Welcome to Crimson Skies" : "start",

    # General
    "You are hungry" : "say i am very hungry.",
    "You are thirsty" : "drink",
    " tells you '" : "reply I'm sorry, I am a bot that is currently being used for load testing.",
    "Reconnecting. Type replay to see missed tells." : "start",
    "In your dreams, or what?" : "wake",
    "Nah... You feel too relaxed..." : "wake",
    "You can not sleep while flying, you must land first." : "land",
    "waiting to be read" : "catchup",
    "is DEAD" : "recall;n;>lae spells;sleep;#WAIT 180;wake;s;s;s;e;e;e;e;e;e;e;n;n;n;n;n;kill bard",
    "They aren't here." : "recall;n;>lae spells;sleep;#WAIT 180;wake;s;s;s;e;e;e;e;e;e;e;n;n;n;n;n;kill bard"
}

#RegEx Triggers
regexTriggers = {
    #"(\w+), right here." : "> %1 say Hello %1, I am a bot written in Python, nice to meet you!",
    "(\w+) is bleeding to death." : "> %1 You look awful.;cast 'heal' %1",
    "A bard leaves (\w+)" : "%1;kill bard"
}

# Aliases
alias = {
    "start" : "prompt <%hhp/%Hmhp %mm/%Mmm %vmv/%Vmmv [%r] (%e)>;recall;n;s;s;s;e;e;e;e;e;e;e;n;n;n;n;n;kill bard"
}

