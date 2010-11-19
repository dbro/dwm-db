dwm-db
======

Starting from (dwm)[http://dwm.suckless.org/] version 5.8.2, this fork has
the following changes:

* movestack patch allows shuffling windows around, similar to xmonad
* xft patch supports nicer fonts
* statuscolors patch enables colors in status bar text
* TODO: only show tag names (1,2,...9) when they have windows in them, or are visible
* TODO: use a different color for the active window title

statusbar.sh
------------

A shell script which provides information to the dwm status bar.

* CPU, Memory, Swap usage
* Network throughput and wireless signal strength (if detected)
* Battery remaining (if detected)
* MPC music playing
* Date and clock

dwm-start
---------

A script that starts the statusbar.sh script and then starts dwm. This is
what my /usr/share/xsessions/dwm.desktop runs.

Installation
------------
Follow the instructions for installing dwm. Note the colors are specified
differently in config.h, and are off-by-one when referring to them in
statusbar.sh.

copy statusbar.sh into the default path, eg /usr/local/bin

make; sudo make install


Dan Brown, 2010
