This is a debug menu for GTA III, VC and SA.

The idea is that other plugins load this dll and add variables
and commands to it so they can be edited from within the game.

It is currently compatible with 1.0 US executables.

How to install
==============
Put debugmenu.dll for your game into the game's root directory.
Plugins that make use of it will load it so DON'T rename the file.


How to control
==============
- Ctrl-M toggles the debug menu
- Arrow Up/Down moves the selection
- Enter/Backspace enters/exists a submenu
- Arrow Left/Right changes values/executes commands
- the mouse is pretty intuitive

How to use (for programmers)
============================
Include debugmenu_public.h in your plugin, define 'DebugMenuAPI gDebugMenuAPI'
globally somewhere and load the debugmenu.dll by calling 'bool DebugMenuLoad(void)',
which loads all function pointers and returns whether the library could
be loaded.
NB: it is NOT recommended to do this from DllMain. Hook an early function and
do it from there!

Integer variables can be added to the menu with functions of this type:

MenuEntry *DebugMenuAdd(const char *path, const char *name, INT *ptr, TriggerFunc triggerFunc, INT step, INT lowerBound, INT upperBound, const char **strings)

where INT stands for all common integer types.
Path is a path in the menu in the form "menu|submenu1|submenu2", name is
the name of the entry you want to add.
Ptr is a pointer to the variable, triggerFunc is a function that is called
when the value was changed (can be NULL).
Step, lowerBound and upperBound control how the variable's value is changed.
Strings is an array of strings that are displayed instead of the numbers (for enums).
DebugMenuAddVarBool8/16/32 is provided for convenience, but just calls the respective INT function.

MenuEntry *DebugMenuAdd(const char *path, const char *name, FLOAT *ptr, TriggerFunc triggerFunc, FLOAT step, FLOAT lowerBound, FLOAT upperBound)
Similarly adds floats and doubles but you can't display strings with them.

MenuEntry *DebugMenuAddCmd(const char *path, const char *name, TriggerFunc triggerFunc)
registers a function that is called from the menu, no values are changed.

void DebugMenuEntrySetWrap(MenuEntry *e, bool wrap)
sets whether values should wrap around at the ends. (when e == NULL, this does nothing)

void DebugMenuEntrySetStrings(MenuEntry *e, const char **strings)
changes the strings for a variable. (when e == NULL, this does nothing)


TODO:
- API versioning? I'm not sure the API is stable yet...
- work on mouse handling
- think of more things TODO
