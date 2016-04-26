import sys

# Healer Bot

# Literal Triggers
triggers = {
    # Login/Connecting
    "Do you want color? (Y/N) ->" : "N",
    "Your selection?" : "p",
    "By what name do you wish to be known?" : sys.argv[1],
    "What is your character's name?" : sys.argv[1],
    "Password:" : sys.argv[2],
    "Do you wish to connect anyway (Y/N)" : "Y",
    "[Hit Return to continue]" : "",
    "Welcome to Crimson Skies" : "start",
    "Copyover recovery complete." : "start",

    # General
    "You are hungry" : "say I sure am hungry.",
    "You are thirsty" : "drink",
    " tells you '" : "reply I'm sorry, I am a bot that is currently being used for load testing.",
    "Reconnecting. Type replay to see missed tells." : "start",
    "In your dreams, or what?" : "wake",
    "Nah... You feel too relaxed..." : "wake",
    "You can not sleep while flying, you must land first." : "land",
    "waiting to be read" : "catchup",
}

#RegEx Triggers
regexTriggers = {
    #"(\w+), right here." : "> %1 say Hello %1, I am a bot written in Python, nice to meet you!",
    "(\w+), nearby to the (\w+)." : "say i see %1 to the %2",
    "(\w+) is bleeding to death." : "> %1 You look awful.;cast 'heal' %1"
}

# Aliases
alias = {
    "start" : "prompt <%hhp/%Hmhp %mm/%Mmm %vmv/%Vmmv [%r] (%e)>;say greetings my friends!"
}

