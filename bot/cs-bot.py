import getpass
import sys
import telnetlib
import time
import os
import re
import string
from threading import Thread

##############################################################################
#                                                                            #
#       Script:  Mud Bot                                                     #
#   Written by:  Blake Pell (blakepell@hotmail.com, http://www.blakepell.com #
# Last Updated:  10/1/2015                                                   #
#                                                                            #
#                This was written specifically for Crimson Skies (CS-Mud)    #
#                which is a DikuMud/Merc/Rom derivative.  It should give a   #
#                idea of how to write a mud bot and should be easily tweaked #
#                to work on other muds (or anything telnet/raw tcp based. My #
#                goal is to use this for load testing.                       #
#                                                                            #
#                Mud Bot Supports:                                           #
#                  - Simple aliases                                          #
#                  - Literal Triggers                                        #
#                  - RegEx Triggers                                          #
#                                                                            #
##############################################################################

HOST = "localhost"
PORT = 4000
USER = ""
PASS = ""
VERBOSE = True
KEEP_ALIVE_LENGTH = 40

# Print function where the verbose can be toggled on or off.
def _print(buf):
    if VERBOSE == True:
        print buf
    return

# Send function, allows for multiple commands with the ; splitting them up, this
# will also check aliases.  If the alias is split up into multiple commands we
# will recurse to get them all.
def _send(buf):
    for cmd in buf.split(";"):    
        cmd = _alias(cmd)

        if ";" in cmd:
            # There are more commands in this cmd that need processed
            _send(cmd)
        else:
            # This is a single command, send it.
            tn.write(cmd + "\n")
            _print("[Sent: " + cmd + "]")
    return

# See if the command is an alias, if so, swap the alias in, if not, return
# what was sent in back out.
def _alias(buf):
    for key in alias:
        if key == buf:
            return alias[key]

    return buf

# This is the main thread that will read all TCP incoming data and then process it
# for screen printing (if enabled), triggers and special data.
def _tcpReaderThread():
    buf = ""

    while True:
        # Keep contactinating to the buffer until we hit a marker, then we'll interpret.
        buf = buf + tn.read_very_eager()

        # A full line was sent or it's the special case password that doesn't send a \n
        if ("\n" in buf or "Password:" in buf):
            _print(buf)

            # Check our literal triggers, split on lines, then process if there are multiple
            # lines that came through
            for key in triggers:
                for line in buf.splitlines():
                    if key in line:
                        _print("[Trigger]")
                        _send(triggers[key])

            # Check our regex triggers, split on lines, then process if there are multiple
            # lines that came through
            for key in regexTriggers:
                for line in buf.splitlines():
                    regex = re.compile(key, re.IGNORECASE)
                    x = 0
                    cmd = regexTriggers[key]
                    found = False
                    for match in regex.finditer(line):                        
                        found = True
                        _print("[RegEx Trigger]")

                        # I see this as hacky.. not sure why we can't get the count in the group
                        # this will work for now.
                        for x in (1, 2, 3, 4, 5, 6, 7, 8, 9):
                            var = "%%%d" % x
                            try:
                                cmd = string.replace(cmd, var, match.group(x))
                            except:
                                # If we have to put something here... I suppose we'll set the ceiling :p
                                x = x - 1

                    # Only send if we found a trigger
                    if found:
                        _send(cmd)

            # We've done all we want with this buffer, clear it
            buf = ""

        time.sleep(1)

# Thread that will send an empty line to keep the connection alive and then will sleep
# for an amount of seconds.
def _keepAliveThread():
    while True:
        # By putting the send after the timer it will wait for seconds after login to
        # run for the first time.
        time.sleep(KEEP_ALIVE_LENGTH)
        _print("KEEP ALIVE")
        _send("")

# Literal Triggers
triggers = {
            # Login/Connecting
            "Do you want color? (Y/N) ->" : "N",
            "By what name do you wish to be known?" : USER,
            "Password:" : PASS,
            "Do you wish to connect anyway (Y/N)" : "Y",
            "[Hit Return to continue]" : "",
            "Welcome to Crimson Skies" : "start",
            
            "You are hungry" : "say i sure am hungry",
            "You are thirsty" : "drink",
            " tells you '" : "reply I'm sorry, I am a bot that is currently being used for load testing.",
            "Reconnecting. Type replay to see missed tells." : "start",
            "In your dreams, or what?" : "wake",
            "You can not sleep while flying, you must land first." : "land",
            "is DEAD!!" : "recall;n;sleep",
            "waiting to be read" : "catchup"
           }

#RegEx Triggers
regexTriggers = {
    #"(\w+), right here." : "> %1 say Hello %1, I am a bot written in Python, nice to meet you!",
    "(\w+), nearby to the (\w+)." : "say i see %1 to the %2"
}

alias = {
            "start" : "prompt <<%hhp/%Hmhp %mm/%Mmm %vmv/%Vmmv [%r] (%e)>>",
            "mudschool" : "wake;recall;u;n;n;w;n;n;w;d;s;kill mon;blade"
        }

##############################################################################
#  Main Logic Loop
##############################################################################

# Setup the telnet library and connect
tn = telnetlib.Telnet(HOST, PORT, timeout = 2)

# Start the TCP thread to read incoming data and do triggers and all
# that fun stuff.
tcpThread = Thread(target = _tcpReaderThread)
tcpThread.daemon = True
tcpThread.start()

keepAliveThread = Thread(target = _keepAliveThread)
keepAliveThread.daemon = True
keepAliveThread.start()

# Main input loop.  This will allow us to enter input if we actually need to.
while True:
    userInput = ""

    # This should allow the raw_input to work both from the terminal and
    # if it's running in the background (where no input would ever be
    # entered.
    if os.getpgrp() == os.tcgetpgrp(sys.stdout.fileno()):
        userInput = raw_input("")
        _send(str(userInput))

    time.sleep(1)

    # Special handling for quit or logout so we can quit this thread.
    if str(userInput) == "quit" or str(userInput) == "logout":
        sys.exit()

