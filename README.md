TBTray - Native Win32 Thunderbird tray icon
===========================================

After the x-th reincarnation of the MinimizeToTray add-on for Thunderbird broke in Thunderbird 68,
it seems like it becomes more and more difficult, if not impossible, to solve the issue at hand -
keeping Thunderbird minimized in the notification area when closing or minimizing it -
using Web Extensions.

I know that [BirdTray](https://github.com/gyunaev/birdtray) exists, and it's even cross-platform.
However, it tries to solve way more problems than I have and uses Qt, so it's not
quite as light-weight as I think a background process should be.

So I decided to fork a program a friend of mine wrote - [traktouch](https://github.com/dop3j0e/traktouch),
as it solves a very similar problem. I could have written it from scratch, but
this way I didn't have to write most of the boilerplate code.

How does it work?
-----------------

TBTray intercepts some window messages sent to Thunderbird, rejecting window
minimize and close events and instead hiding the window and creating a tray icon.

To do this, TBTray checks for the presence of the Thunderbird main window, and if
it finds the window, injects a library into the Thunderbird process to hook into
the message queue. TBTray keeps running in the background, in case you want to
restart Thunderbird at some point.

Halp, how do I quit Thunderbird?
--------------------------------

Through the File menu, or the context menu of the tray icon.

Is there any sort of configuration?
-----------------------------------

This program does not come with any options, because it is just supposed to fix
exactly the problem I had. Please fork the repository if you want the program to
behave differently. 

Autostart
---------

To automatically start TBTray on Windows startup, run `register.cmd`.
To uninstall, run `unregister.cmd`. 