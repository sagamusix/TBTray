TBTray - Native Win32 Thunderbird tray icon
===========================================

After the x-th reincarnation of the MinimizeToTray add-on for Thunderbird broke in Thunderbird 68,
it seems like it becomes more and more difficult, if not impossible, to solve the issue at hand -
keeping Thunderbird minimized in the notification area when closing or minimizing it.

I know that [BirdTray](https://github.com/gyunaev/birdtray) exists, and it's even cross-platform.
However, it tries to solve way more problems than I have and uses Qt, so it's not
quite as light-weight as I think a background process should be.

So I decided to fork a program a friend of mine wrote - [traktouch](https://github.com/dop3j0e/traktouch),
as it solves a very similar problem. I could have written it from scratch, but
this way I didn't have to write most of the boilerplate code.

How does it work?
-----------------

The short version is that TBTray intercepts some of the communication between
Windows and Thunderbird, rejecting window minimize and close events and instead
hiding the window and creating a tray icon.

Autostart
---------

To automatically start TBTray on Windows startup, run `register.cmd`.
To uninstall, run `unregister.cmd`. 