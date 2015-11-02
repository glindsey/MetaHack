MetaHack
========

A graphical roguelike engine which uses SFML for graphics and sound, and Lua for scripting.

MetaHack is compiled under Visual Studio 2015. Right now, the code is in serious flux; although I try to make sure it always builds successfully before committing, I guarantee that there are still plenty of runtime errors present. Also, at the moment, the engine is extremely incomplete, with most functionality still unimplemented.


Interface
---------

Unlike traditional roguelikes, MetaHack has an "object-oriented" bent to its interface; you select one or more items you want to interact with first, and then choose the command to apply to them.


### Movement

Movement is done via the number pad arrow keys. Eventually there will be "quick move" commands similar to NetHack's `g` command, but for now this is not yet implemented.

Waiting is done with the period `.` key just as in NetHack. No wait-on-space is implemented.


### Inventory (Items)

The Inventory area is always present on the right-hand side of the screen, and displays either your player's inventory, or the inventory of the tile you are on. Use `Tab` to switch between these two.

Items in inventory slots are selected/deselected by pressing a letter `a-z` (or a shifted letter `A-Z` if more than 26 items are present).

If an item is a container, after selecting it you can use the right bracket `]` to view its contents. This can be done for as many nested containers as you like. To go back up a level, use the left bracket `[`.

If an inventory slot contains two or more of the same item (such as "5 gold coins"), after selecting it you can choose how many are selected with the `-` and `+` keys. This allows you to use a command on only a portion of the quantity -- to drop only some gold, for instance. Only one slot can be selected in this case. *(For example, you can't drop "5 gold coins" and "a red gem" at once, you have to do them separately for now. Hopefully this can be changed in the future.)*

The `=` key also adds to the number selected, in case you prefer to use the `-`/`=` keys on the main keyboard instead of the `-`/`+` keys on the keypad.


### Commands

Once items have been selected, a command is issued using `CTRL` plus a letter key.

*(Eventually the goal is to allow items to be selected using the mouse, and then have a command menu from which the command can be chosen.)*

Some commands do request a "target" item after the command has been entered, such as "store item". In that case, you would choose the items to store, press `CTRL-S`, and then choose the container to store them in.

The following commands are currently in place, although some are not yet fully implemented:

`CTRL-D` - Drop item(s)
`CTRL-P` - Pick up item(s)
`CTRL-Q` - Quaff (drink) item(s)
`CTRL-S` - Store item(s) in another item
`CTRL-T` - Take item out of container
`CTRL-W` - Wield item


### View Mode

Hitting `/` puts the display into "view mode". In this mode, the arrow keys move the cursor around, and the inventory window will display what is there. Hitting `/` again will return you to "game mode".


### Other Keys / Actions

Hitting the backtick/tilde key brings up the debug console, which at the moment does absolutely nothing. Eventually it will let you execute Lua functions directly.

For those accustomed to NetHack, the comma `,` key is equivalent to `CTRL-P`. You just have to remember to select the items to pick up *first*!

`Esc` will immediately exit the game. No time for "are you sure?" niceties right now.

`CTRL-minus` and `CTRL-plus` (or `CTRL-equals`) zoom the graphics in and out. `CTRL-0` (zero) should return to standard zoom.

The mouse wheel also zooms the screen in/out.


Metadata
--------
All of the information regarding map tiles, items, enemies, et cetera., is stored in the `resources` subdirectory, as groups of XML, PNG, and Lua script files. So far there isn't a ton of information there yet, but as I add features to the engine they will become more complex.

Hopefully one day I will be able to document those files a bit more thoroughly here, but for now if you are interested you'll have to look through the source code I'm afraid.


Known Bugs
----------

* Resizing the window is definitely broken.
* There are no enemies whatsoever yet.


License
-------

This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
