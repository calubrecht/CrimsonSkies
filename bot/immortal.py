import sys

# Immortal Bot

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
    " tells you '" : "reply I apologize, but I am currently away.",
    "Reconnecting. Type replay to see missed tells." : "start",
    "In your dreams, or what?" : "wake",
    "Nah... You feel too relaxed..." : "wake",
    "You can not sleep while flying, you must land first." : "land",
    "waiting to be read" : "catchup",
}

#RegEx Triggers
regexTriggers = {
    #"(\w+), right here." : "> %1 say Hello %1, I am a bot written in Python, nice to meet you!",
    "--> (\w+)'(\w+) gained level (\w+)" : "load obj 1200;at %1'%2 give vial %1'%2;",
    "--> (\w+) gained level (\w+)" : "load obj 1200;at %1 give vial %1"
}

# Aliases
alias = {
    "start" : "prompt <%hhp/%Hmhp %mm/%Mmm %vmv/%Vmmv [%r] (%e)>"
}

