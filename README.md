MetaHack
========

A graphical roguelike engine which uses SFML for graphics and sound, and Lua for scripting. It is called "MetaHack" because the goal is to make a "hack"-style game that is easy to hack (mod) via Lua scripts.

MetaHack is compiled using the following:

* Visual Studio 2015

* Boost 1.59

* LuaJIT 2.0

* SFML 2.3.1 (built from scratch under VS2015)

Right now, the code is in serious flux; although I try to make sure it always builds successfully before committing, I guarantee that there are still plenty of runtime errors present. Also, at the moment, the engine is extremely incomplete, with most functionality still unimplemented.


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

Once items have been selected, a command is issued using `CTRL` plus a letter key. Extended commands are issued using `CTRL` plus `ALT` plus a letter key. (Or they will be. No extended commands are implemented yet.)

*(Eventually the goal is to allow items to be selected using the mouse, and then have a command menu from which the command can be chosen.)*

Some commands do request a "target" item after the command has been entered, such as "store item". In that case, you would choose the items to store, press `CTRL-S`, and then choose the container to store them in.

The following commands are currently in place, although ~~some~~ most are not yet fully implemented. ~~Strikeout~~ indicates commands that may be planned but are currently not recognized at all.

* `.` (period) - Wait. This is the only command that does not use the CTRL key.
* `CTRL-A` - Attire/adorn yourself with an item (e.g. wear it). Using this with @ (yourself) will attempt to remove everything you are wearing; using it with an already worn item will attempt to remove it. 
* ~~`CTRL-B` - Nothing yet; maybe beg for your life? That would be interesting.~~
* `CTRL-C` - Close item(s). Also used to close doors/grates/etc.
* `CTRL-D` - Drop item(s).
* `CTRL-E` - Eat item(s).
* `CTRL-F` - Fill item. Used to transfer a liquid/powder from one container to another.
* `CTRL-G` - Get (pick up) an item. For convenience to those familiar with NetHack, `,` (comma) is a synonym for this command.
* `CTRL-H` - Hurl (throw) an item.
* `CTRL-I` - Inscribe (write) on an item.
* ~~`CTRL-J` - Jump in a direction.~~
* ~~`CTRL-K` - Kick in a direction.~~
* ~~`CTRL-L` - Nothing yet; maybe look around (similar to "search" in Nethack), or lock/unlock.~~
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
* `CTRL-W` - Wield or unwield an item. Using this with @ (yourself) unwields whatever you are wielding; wielding your already wielded object will also unwield it.
* `CTRL-X` - eXplicitly attack a direction. Although attacking is usually automatic for hostile creatures, this forces an attack on something non-hostile, or something that isn't a creature (like a door). If used in the direction of a group of items, it will automatically attack the largest item there.
* ~~`CTRL-Y` - Nothing yet.~~ 
* ~~`CTRL-Z`- Nothing yet.~~ 
* ~~`CTRL-ALT-C` - Construct an item out of a collection of items.~~
* ~~`CTRL-ALT-D` - Destruct an item into its constituent parts.~~
* ~~`CTRL-ALT-S` - Save the game.~~
* ~~`CTRL-ALT-T` - Talk in a direction. Possibly talk to an item at some point, I dunno.~~
* `CTRL-ALT-Q` - Quit the game, immediately, abandoning your character. Will eventually have an "are you sure" prompt.

This list is definitely not finished and is subject to change.

### View Mode

Hitting `/` puts the display into "view mode". In this mode, the arrow keys move the cursor around, and the inventory window will display what is there. Hitting `/` again will return you to "game mode".


### Other Keys / Actions

Hitting the backtick/tilde key brings up the debug console, which will let you execute Lua functions directly. Since I have not implemented scrollback in the message log yet, Lua functions print to the console window instead of the message log (as they will eventually).

Hitting `ESC` when in a "choose an item" / "choose a direction" prompt will abort an action.

As noted above, the comma `,` key is equivalent to `CTRL-G`. You just have to remember to select the items to get *first*!

`CTRL-ALT-Q` will immediately exit the game. No time for "are you sure?" niceties right now.

`CTRL-minus` and `CTRL-plus` (or `CTRL-equals`) zoom the graphics in and out. `CTRL-0` (zero) should return to standard zoom.

The mouse wheel also zooms the screen in/out.


Metadata
--------
All of the information regarding map tiles, items, enemies, et cetera., is stored in the `resources` subdirectory, as Lua script files and associated PNGs (when necessary). So far there isn't a ton of information there yet, but as I add features to the engine they will become more complex.

Hopefully one day I will be able to document those files a bit more thoroughly here, but for now if you are interested you'll have to look through the source code I'm afraid.


Known Bugs / Issues
-------------------

* Library and include directories in the project are absolute at the moment, instead of being relative to the project.

* Resizing the window is ~~definitely~~ maybe broken? It seems to be working correctly again, but I swear it was broken before.

* There is a grand total of one enemy so far, and you can't attack it.

* Picking up/dropping a set number of an item isn't working; something I did recently must have broken it.

TODO List
---------
The TODO list is currently a text file called "Big Todo List.txt". It can be found in the repository.

FAQOWBIALAT (Frequently Asked Questions, Or Would Be If Anyone Looked At This)
------------------------------------------------------------------------------
* Q: Why is development of this so slow and erratic?

    * A: I have a full-time job and a family. Finding time to work on an open-source project such as this is difficult at best, and impossible at worst. Still, I try to work on it when I can.

License
-------
This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
