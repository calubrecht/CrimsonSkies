import sys

# Healer Bot

# Literal Triggers
triggers = {
    # Login/Connecting
    "Do you want color? (Y/N) ->" : "N",
    "By what name do you wish to be known?" : sys.argv[1],
    "Password:" : sys.argv[2],
    "Do you wish to connect anyway (Y/N)" : "Y",
    "[Hit Return to continue]" : "",
    "Welcome to Crimson Skies" : "start",
    "Copyover recovery complete." : "start",

    # General
    "You are hungry" : "cast 'nurishment'",
    "You are thirsty" : "cast 'nurishment'",
    " tells you '" : "reply I'm sorry, I am a bot that is currently being used for load testing.",
    "Reconnecting. Type replay to see missed tells." : "start",
    "In your dreams, or what?" : "wake",
    "Nah... You feel too relaxed..." : "wake",
    "You can not sleep while flying, you must land first." : "land",
    "waiting to be read" : "catchup",
    "return home" : "recall;n"
}

#RegEx Triggers
regexTriggers = {
    #"(\w+), right here." : "> %1 say Hello %1, I am a bot written in Python, nice to meet you!",
    "(\w+), nearby to the (\w+)." : "say i see %1 to the %2",
    "(\w+) has arrived." : "> %1 If you need a spell I can provide, directly speak to me and say it's name.;examine %1",
    "(\w+) says \(to You\) '(\w+)'" : "> %1 My blessings be with you;cast '%2' %1",
    "(\w+) says \(to You\) '(\w+) (\w+)'" : "> %1 My blessings be with you;cast '%2 %3' %1",
    "(\w+) says \(to You\) 'spells'" : "> %1 My blessings be with you;cast 'armor' %1;cast 'shield' %1;cast 'bless' %1;cast 'frenzy' %1;cast 'sanc' %1;cast 'life boost' %1;cast 'enhanced recovery' %1;c 'heal' %1",
    "(\w+) has a few scratches." : "cast 'heal' %1",
    "(\w+) has some small wounds and bruises." : "cast 'heal' %1",
    "(\w+) has quite a few wounds." : "cast 'heal' %1",
    "(\w+) has some big nasty wounds and scratches." : "cast 'heal' %1",
    "(\w+) looks pretty hurt." : "cast 'heal' %1",
    "(\w+) is in awful condition." : "> %1 You look awful.;cast 'heal' %1",
    "(\w+) is bleeding to death." : "> %1 You look awful.;cast 'heal' %1"
}

# Aliases
alias = {
    "start" : "prompt <%hhp/%Hmhp %mm/%Mmm %vmv/%Vmmv [%r] (%e)>",
    "mudschool" : "wake;recall;u;n;n;w;n;n;w;d;s;kill mon;blade"
}

