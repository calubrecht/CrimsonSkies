import getpass
import sys
import telnetlib
import time
import re
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
#                to work on other muds (or anything telnet/raw tcp based.    #
#                                                                            #
##############################################################################

HOST = "localhost"
PORT = 4000
USER = ""
PASS = ""
VERBOSE = True

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

            # Check our triggers
            for key in triggers:
                if key in buf:
                    _print("[Trigger]")
                    _send(triggers[key])

            # We've done all we want with this buffer, clear it
            buf = ""

        time.sleep(1)

# Literal Triggers
triggers = {
            "Do you want color? (Y/N) ->" : "N",
            "By what name do you wish to be known?" : USER,
            "Password:" : PASS,
            "Do you wish to connect anyway (Y/N)" : "Y",
            "You are hungry" : "say i sure am hungry",
            "You are thirsty" : "drink",
            "[Hit Return to continue]" : "",
            " tells you '" : "reply I'm sorry, I am a bot that is currently being used for load testing.",
            "Reconnecting. Type replay to see missed tells." : "mudschool",
            "In your dreams, or what?" : "wake",
            "You can not sleep while flying, you must land first." : "land",
            "Welcome to Crimson Skies" : "mudschool",
            "is DEAD!!" : "recall;n;sleep",
            "^.+, right here." : "say someone is here.",
           }

alias = {
            "start" : "scan",
            "mudschool" : "wake;recall;u;n;n;w;n;n;w;d;s;kill mon;blade"
        }

# Setup the telnet library and connect
tn = telnetlib.Telnet(HOST, PORT, timeout = 2)

##############################################################################
#  Main Logic Loop
##############################################################################

# Start the TCP thread to read incoming data and do triggers and all
# that fun stuff.
thread = Thread(target = _tcpReaderThread)
thread.daemon = True
thread.start()

# Main input loop.  This will allow us to enter input if we actually need to.
while True:
    userInput = ""
    userInput = raw_input("")
    _send(str(userInput))
    time.sleep(1)

    # Special handling for quit or logout so we can quit this thread.
    if str(userInput) == "quit" or str(userInput) == "logout":
        sys.exit()

