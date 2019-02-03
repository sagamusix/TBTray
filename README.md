Traktouch - improve Traktor touchscreen usability
=================================================

1. What problem does this solve?
--------------------------------

Native Instruments' Traktor is a wonderful DJing software and I love it to bits.
My new Windows laptop has a touch screen, so I was looking forward to operating
Traktor's knobs and buttons by touching them in addition to using my DJ controller.
Unfortunately, so far Traktor does not support touch screens, so all I get is 
Windows' mouse emulation, which brings about two problems:

 1. When I touch a knob and don't move it right away, Windows thinks I want to 
    perform a right click and my touch is lost - I have to lift my finger and try
	again.
 2. Even if I move my finger right away and don't create a right click, knobs and
    sliders don't respond as expected: I move my finger away from the touch position 
	by a bit, and the the slider instantly moves all the way into that direction.
 
Both of these problems are fixed by Traktouch. I can touch a knob and not move my 
finger for as long as I want, and when I move it, the knob moves as expected and 
stops when I stop moving my finger.


2. How to use Traktouch
-----------------------

 1. Unzip traktouch.exe and traktouch.dll into your Traktor install directory, 
    so they're in the same folder as Traktor.exe.
 2. Start Traktor by running traktouch.exe instead of Traktor.exe. 
    Traktor will start and be magically fixed :)

That's all. Traktouch does _not_ modify Traktor.exe in any way; if you want an 
unmodified Traktor you can always run Traktor.exe as usual. Myself, I simply changed 
Traktor's start menu shortcut to launch traktouch.exe.

There is another way of running: If Traktor is already running when you start 
traktouch.exe, the running instance of Traktor will be modified. But launching
Traktor through Traktouch is one less program to start :)


3. How does it work?
--------------------

Windows programs have some control over the way Windows turns touch interactions
into mouse events. Traktor does not natively use these options, so Traktouch inserts 
some extra code into the loaded Traktor process that will take care of this.

Also, Traktor implements its knobs and sliders in the assumption that you're using a 
mouse which is a relative input device: Whenever you move the mouse while holding the
button down on, say, a slider, the cursor moves by a few pixels, and Traktor moves the
slider by that distance and moves the cursor back to the location where you first
clicked the slider. This may sound weird, but it's necessary to make sure you get
infinite movement on the knobs - otherwise the mouse cursor would eventually get stuck
at the edge of the screen and you couldn't move your knob any further.

(You don't see this because Traktor turns off the mouse cursor while it's doing this,
but you'll notice that when you release the mouse button, the cursor reappears at the
same place where you pressed the button.)

The downside to this technique is that you can move a mouse cursor around the screen
under software control, but not the user's finger ;) So every time Traktor moves the
cursor back to the original point, your finger stays put and Traktor will think that 
the mouse moved, so it moves the slider/knob and puts the cursor back, but your 
finger is still where it's at so Traktor thinks you moved it yet again, and so on.

Traktouch fixes this by intercepting Traktor's attempts to move the mouse cursor and
storing a correction value internally that it applies to all mouse position reported
back to Traktor. In a way, it emulates moving the user's finger so that Traktor's
approach to infinite mouse movement works with touch interaction too.


4. Can I safely use Traktouch during my live gig?
-------------------------------------------------

Time will tell.

This program is very new, and while the methods I'm using to insert code into Traktor
are nothing new and I made an effort to keep the code as simple and unintrusive as
possible, I _am_ inserting custom code into Traktor after all, and things might break
in unexpected places.

You're well advised to give this program a few thorough test runs using your usual
DJing routine before you hit the club on a Friday night. If it doesn't crash during
the test runs, I'm fairly confident it won't crash during the gig.


5. How are you going to support this tool?
------------------------------------------

As time allows.

I wrote this program during my free time to scratch an itch. I cannot and will not
guarantee correct operation in all situations, and I cannot provide support in my
free time - you're on your own. That said, it's been working reliably for me so far,
and if you're running into problems, I'll happily take issues on GitHub, I just cannot
guarantee fast resolution.


6. The knobs and sliders are super small and hard to hit, can you fix that too?
-------------------------------------------------------------------------------

I have a few ideas but they'll be hard to implement; I don't know whether or not
I'll have the time and motivation for that. Maybe there will be a version 2.0 if
that itch becomes really itchy for me :)


7. Will there be a Mac version of this?
---------------------------------------

Certainly not, unless someone else writes it.
