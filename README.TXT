An interface for connecting a serial device to a computer via the
"BlueTooth Bee." Handles things like automatically configuring the BTB,
resetting it when necessary, etc. Written for an AVR, there is a PC-based
test-program under _commonCode-localized/btbConnector/...
It should be relatively easily ported to any microcontroller.

Not sure whether the difficulties I encountered were due to my Operating
System, or the BTB, but after the BTB serial-port was closed, the BTB had
to be reset in order for the computer to re-open it. Among other things.

This is as of Mac OS X 10.5.8.


--------

This particular implementation connects a Wacom Digitizer via bluetooth.

It's been *quite some time* since I last worked on this, so I can't vouch
for its functionality... I had to do some modifications to make it git-able
But I don't think I created any new bugs in doing-so, especially for the
non-AVR/Project-specific code.
(One place to check would be polled_uar/...)

--------

Nearly all the work here is mine, except where otherwise noted. You're free
to make use of it however you please, unless you represent an organization
or otherwise plan to profit from my work. In which case, you must contact
me first. I'm always appreciative of beer, and cash!
ericwazhung [at] gmaildotcom
