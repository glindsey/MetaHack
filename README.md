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

Once items have been selected, a command is issued using `CTRL` plus a letter key. Extended commands are issued using `CTRL` plus `ALT` plus a letter key.

*(Eventually the goal is to allow items to be selected using the mouse, and then have a command menu from which the command can be chosen.)*

Some commands do request a "target" item after the command has been entered, such as "store item". In that case, you would choose the items to store, press `CTRL-S`, and then choose the container to store them in.

The following commands are currently in place, although some (most?) are not yet fully implemented. ~~Strikeout~~ indicates commands that may be planned but are currently not recognized at all.

* `CTRL-A` - Attack a direction. Although attacking is usually automatic for hostile creatures, this forces an attack on something non-hostile, or something that isn't a creature (like a door).
* ~~`CTRL-B` - Nothing yet; maybe beg for your life? That would be interesting.~~
* `CTRL-C` - Close item(s). Also used to close doors/grates/etc.
* `CTRL-D` - Drop item(s).
* `CTRL-E` - Eat item(s).
* `CTRL-F` - Fill item. Used to transfer a liquid/powder from one container to another.
* `CTRL-G` - Get (pick up) an item.
* `CTRL-H` - Hurl (throw) an item.
* `CTRL-I` - Inscribe (write) on an item.
* ~~`CTRL-J` - Jump in a direction.~~
* ~~`CTRL-K` - Kick in a direction.~~
* ~~`CTRL-L` - Nothing yet; maybe look around (similar to "search" in Nethack).~~
* `CTRL-M` - Mix two liquids/powders together.
* ~~`CTRL-N` - Nothing yet.~~
* `CTRL-O` - Open item(s). Also used to open doors/grates/etc.
* `CTRL-P` - Put item(s) in another item. This is also used to load ammunition into a fireable weapon.
* `CTRL-Q` - Quaff (drink) from item(s).
* `CTRL-R` - Read item(s).
* `CTRL-S` - Shoot a fireable item. This will normally shoot the item you have equipped, if it is a firearm. If you select an item first, it will attempt to wield it and then shoot it.
* `CTRL-T` - Take item(s) out of a container.
* `CTRL-U` - Use an item. For items with actions that don't really fit into other command verbs.
* ~~`CTRL-V` - Nothing yet.~~
* `CTRL-W` - Wield an item.
* ~~`CTRL-X` - Nothing yet.~~ 
* ~~`CTRL-Y` - Nothing yet.~~ 
* ~~`CTRL-Z`- Nothing yet.~~ 
* ~~`CTRL-ALT-S` - Save the game.~~
* ~~`CTRL-ALT-T` - Talk in a direction. Possibly talk to an item at some point, I dunno.~~
* `CTRL-ALT-Q` - Quit the game, immediately, abandoning your character. Will eventually have an "are you sure" prompt.

This list is definitely not finished and is subject to change.

### View Mode

Hitting `/` puts the display into "view mode". In this mode, the arrow keys move the cursor around, and the inventory window will display what is there. Hitting `/` again will return you to "game mode".


### Other Keys / Actions

Hitting the backtick/tilde key brings up the debug console, which at the moment does absolutely nothing. Eventually it will let you execute Lua functions directly.

Hitting `ESC` when in a "choose an item" / "choose a direction" prompt will abort an action.

For those accustomed to NetHack, the comma `,` key is equivalent to `CTRL-P`. You just have to remember to select the items to pick up *first*!

`CTRL-ALT-Q` will immediately exit the game. No time for "are you sure?" niceties right now.

`CTRL-minus` and `CTRL-plus` (or `CTRL-equals`) zoom the graphics in and out. `CTRL-0` (zero) should return to standard zoom.

The mouse wheel also zooms the screen in/out.


Metadata
--------
All of the information regarding map tiles, items, enemies, et cetera., is stored in the `resources` subdirectory, as groups of XML, PNG, and Lua script files. So far there isn't a ton of information there yet, but as I add features to the engine they will become more complex.

Hopefully one day I will be able to document those files a bit more thoroughly here, but for now if you are interested you'll have to look through the source code I'm afraid.


Known Bugs / Issues
-------------------

* Resizing the window is definitely broken.
* There are no enemies whatsoever yet.


License
-------

This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
