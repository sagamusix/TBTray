TBTray 2 - Native Win32 Thunderbird tray icon
=============================================

After the x-th reincarnation of the MinimizeToTray add-on for Thunderbird broke
in Thunderbird 68, it seems like it becomes more and more difficult, if not
impossible, to solve the issue at hand - keeping Thunderbird minimized in the
notification area when closing or minimizing it - using Web Extensions.

I know that [BirdTray](https://github.com/gyunaev/birdtray) exists, and it's
even cross-platform. However, it tries to solve way more problems than I have
and uses Qt (no offense, I really like the framework), so it's not quite as
light-weight as I think a background process should be.

Thunderbird 78 also finally comes with its own tray icon, but it
[only works when minimizing the window](https://bugzilla.mozilla.org/show_bug.cgi?id=1666638),
not when closing it. TBTray 0.10 used to provide its own tray icon, but TBTray
2.0 instead makes use of Thunderbird's own tray icon, and merely intercepts the
requests to close the window and turns them into requests to minimize the window
instead.

So I decided to fork a program a friend of mine wrote - [traktouch](https://github.com/dop3j0e/traktouch),
as it solves a very similar problem. I could have written it from scratch, but
this way I didn't have to write most of the boilerplate code.

Installation
------------

Note: Requires _Microsoft Visual C++ 2015-2019 Redistributable_, with same bitness
([x86](https://aka.ms/vs/16/release/VC_redist.x86.exe), [x64](https://aka.ms/vs/16/release/VC_redist.x64.exe))
as your Thunderbird.

0. Make sure that you are using Thunderbird 78 or later.
1. Download the [latest TBTray release](https://github.com/sagamusix/TBTray/releases).
2. Extract the archive anywhere you want, `%localappdata%\TBTray` would be a
   good place for instance.
   I would not recommend to put it in the same folder as Thunderbird, although
   it should be possible in theory.
3. Figure out whether you run a 32-bit or 64-bit version of Thunderbird.
   If you are not sure, check whether Thunderbird is installed in
   `Program Files` (64-bit on a 64-bit system, 32-bit on a 32-bit system) or
   `Program Files (x86)` (32-bit on a 64-bit system).
4. Launch the TBTray executable in the folder that **matches your Thunderbird
   bitness**, i.e. if you run a 32-bit Thunderbird, then run `TBTray.exe` in the
   32-bit folder of TBTray.
5. To automatically start TBTray on Windows startup, run `register.cmd` in the
   folder that **matches your Thunderbird bitness**, i.e. if you run a 32-bit
   Thunderbird, then run `register.cmd` in the 32-bit folder of TBTray. 
   If you ever decide to move the executable to a different location, you will
   need to run `register.cmd` again.
6. To completely uninstall, run `unregister.cmd` and delete the extracted files. 

How does it work?
-----------------

TBTray intercepts some window messages sent to Thunderbird, rejecting window
close events and instead minimizing the window, causing Thunderbird to make use
of its tray icon.

To do this, TBTray checks for the presence of the Thunderbird main window, and if
it finds the window, injects a library into the Thunderbird process to hook into
the message queue. TBTray keeps running in the background, in case you want to
restart Thunderbird at some point.

It doesn't work for me!
-----------------------

The most likely cause is that you have `mail.tabs.drawInTitlebar` set to `true`
(which is the default value) - setting it to `false` through the
[config editor](https://support.mozilla.org/en-US/kb/config-editor) should solve
that problem.

Alternatively, Alt + F4 still works intended - you can still keep using that
shortcut to send Thunderbird to the notification area if you are used it.

Note: If you want to get this fixed, consider submitting a pull request - I do
not have the time required to debug and fix a feature I am not using.

Halp, how do I quit Thunderbird?
--------------------------------

Through the File menu.

Is there any sort of configuration?
-----------------------------------

This program does not come with any options, because it is just supposed to fix
exactly the problem I had. Please fork the repository if you want the program to
behave differently. 
