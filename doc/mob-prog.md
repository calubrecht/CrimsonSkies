# CS-Mud 1.4.8 Documentation

### MobProgs

MobProgs or Mobile Programs are a way to add behaviors to mobs without actually writing C code that needs to be compiled into the code base.  MobProgs are snippits of a custom language that are stored in the area file and attached to a mob.  These are not as full featured as something like Lua can provide but can all you to create basic behaviors to tailors your creatures.

[MobProg Examples](mob-prog-examples.md)

Here's a simple example of a basic mob prog that could be setup to trigger when a character enters an area:

~~~~
#6575
mob zecho $I shouts '{YLet it be know, $n has entered the dueling grounds!{x'
~
~~~~

[Back to Table of Contents](index.md)
