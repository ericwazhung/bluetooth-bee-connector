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


-----------------
As Of 11-27-15:
The official code-site for this project has moved to github
Below is the front-page info from the original google-code page.
TODO: Merge this with the above...

BlueTooth?-Bee-Connector

An interface for seamlessly connecting a serial peripheral device to a computer via the BlueToothBee ( http://www.seeedstudio.com/wiki/Bluetooth_Bee )

Handles things like automatically configuring the BTB, resetting it when necessary, etc. Written for an AVR, there is a PC-based test-program, and it should be easily ported to any microcontroller.

This allows you to use your serial-device's normal RS-232 driver over bluetooth without having to send special commands, or otherwise hi-jack the regular-ol' driver.

Not sure whether the difficulties I encountered were due to my Operating System, or the BTB, but after the BTB serial-port was closed, the BTB had to be reset in order for the computer to re-open it. Among several other things.

It should be compatible with any OS, since the btb-connector circuitry exists on the BTB-side and its entire intent is to create an invisible interface as though the RS232-peripheral is connected straight to an RS-232 port.

This was designed to install a Wacom "Penabled" digitizer in a Mac PowerBook? G4, running OS X 10.5.8, as described here. 
