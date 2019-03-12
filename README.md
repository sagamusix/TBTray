Traktouch - make Traktor useable with a touch screen
====================================================

What problem does this solve?
--------------------------------

Native Instruments' Traktor is a wonderful DJing software and I love it to bits.
And my Windows laptop has a touch screen, so I was looking forward to operating
Traktor's knobs and buttons by touching them in addition to using my DJ controller.

Unfortunately Traktor does not support touch screens so far, so I can only go through
Windows' mouse emulation, and that's no good for several reasons:

 1. When I touch something and keep my finger where it is, Windows eventually emulates
    a right click instead of clicking and dragging and my touch is lost.
 2. Even if I get Windows to emulate click-and-drag, sliders and knobs don't respond
    as expected - they quickly move all the way to one end of their scale.
 3. Scrolling the track list is super fiddly without a mouse wheel.

Traktouch solves all these problems for you:

 1. Buttons, knobs and faders respond to touch as you would intuitively expect.
 2. Dragging two fingers across the Traktor window will scroll the track list, 
    no matter where you touch. There is also some acceleration built into this,
	so quick scroll movements will take you VERY far.


How to use Traktouch
-----------------------

 1. Unzip traktouch.exe and traktouch.dll into your Traktor install directory, 
    so they're in the same folder as Traktor.exe.
 2. Start Traktor by running traktouch.exe instead of Traktor.exe. 
    Traktor will start and be magically changed :)

That's all. Traktouch does _not_ modify Traktor.exe in any way, it just modifies the
running Traktor instance in memory. If you want an unmodified Traktor you can always
run Traktor.exe as usual.

Myself, I simply changed Traktor's start menu shortcut to launch traktouch.exe.


How does it work?
--------------------

The short version is that Traktouch intercepts some of the communication between
Windows and Traktor, modifying touch-based mouse events coming down from Windows
and monitoring some of Traktor's reactions to those events. It also changes some
details of how Windows turns touch interaction into mouse events on Traktor's behalf.

The longer version is that Traktouch:
 * Tells Windows to disable right-click emulation for Traktor's main window.
 * Disables all special touch gestures except panning (two-finger dragging),
   again only for Traktor's window.
 * Intercepts Traktor's attempts to move the mouse cursor while dragging a knobs
   or slider, and internally stores an offset value to _pretend_ the mouse cursor
   got moved.
 * Turns pan gestures into fake mouse wheel events, with acceleration.
 * Plus some other minor event munging to make slider/knob control smooth and reliable.

For the very long version, the source code is at https://github.com/dop3j0e/traktouch :)


Can I safely use Traktouch during my live gig?
-------------------------------------------------

Time will tell.

This program is very new, and while the methods I'm using to insert code into Traktor
are nothing new and I made an effort to keep the code as simple and unintrusive as
possible, I _am_ inserting custom code into Traktor after all, and things might break
in unexpected places.

You're well advised to give this program a few thorough test runs using your usual
DJing routine before you hit the club on a Friday night. If it doesn't crash during
the test runs, I'm fairly confident it won't crash during the gig.


How are you going to support this tool?
------------------------------------------

As time allows.

I wrote this program during my free time to scratch an itch. I cannot and will not
guarantee correct operation in all situations, and I cannot provide support in my
free time - you're on your own. That said, it's been working reliably for me so far,
and if you're running into problems, I'll happily take issues on GitHub, I just cannot
guarantee fast resolution.


Does Traktouch support manipulating multiple controls at the same time?
--------------------------------------------------------------------------

No, it's still just a mouse emulation and therefore only allows touching one control at
a time. I don't think real multi-touch can be bolted onto Traktor from the side, Native
Instruments would need to implement support for it in Traktor itself.


Will there be a Mac version of this?
---------------------------------------

Not from me - but you're welcome to steal ideas from Traktouch if you'd like to make a
Mac equivalent.


I like to tweak stuff, do you have any configuration settings?
-----------------------------------------------------------------

If the default behavior of Traktouch isn't right for you, there are some things you can
configure by creating a file called traktouch.ini and placing it in the same directory
as traktouch.exe. Here's an example file with default settings:

<pre>
; Settings for mouse wheel emulation
[Scroll]

; Basic scale factor for scroll movements. Increase to scroll faster, decrease to scroll slower.
Scale=6

; Below a certain scroll speed, no acceleration is applied. Zero gives you immediate acceleration,
; a very large value like 1000 will give you no acceleration at all.
AccelDeadZone=3
; Once acceleration is applied, this specifies how much faster you go. Minimum value is 100,
; less than 100 will actually make scrolling slower if you go faster ;)
AccelExponent=200

; Track list scrolling is achieved by generating mouse wheel events at a virtual mouse
; position inside the track list. The following two settings define the offset of that
; position, measured in pixels from the bottom right corner of Traktor's window.
; If scrolling does not work for you, you can try tweaking these values to match your Traktor layout.
TrackListOffsetX=20
TrackListOffsetY=80

; Settings for touch emulation
[Touch]

; Experiments indicate that touching the Stripe doesn't work as expected unless Traktouch delays the
; emulated click events for a short time. This delay causes a bit of latency between a touch and
; Traktor's reaction. If you want to get rid of that latency, or touch interaction is behaving
; weirdly for you, and you don't need the Stripe, you can turn off the delay by setting this to 0.
DeferButtons=2
</pre>


Acknowledgements
----------------

Traktouch would not be possible without the [mhook] library by Marton Anka, which it uses to 
intercept some of the Windows API calls.

The first implementations took a lot of ideas from Fabian Giesen's [kkapture]. The implementation
has diverged significantly from that first draft, but without kkapture I wouldn't have known where
to start.

[mhook]: http://codefromthe70s.org/mhook24.aspx
[kkapture]: https://github.com/rygorous/kkapture
