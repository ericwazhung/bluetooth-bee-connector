# btbConnector depends on cirBuff

# The following variables are expected to be defined 
#  in the calling makefile(s)
# BTBCONNECTOR_LIB - Path to polled_uar.mk/c including filename, 
#  excluding extension
# COMDIR
#
# BTBC_INLINE (optional, NOT a CFLAG)
#
# CFLAGS += -D'BTBC_PC_DEBUG=TRUE' -> PC Debugging printf's...

#only include btbConnector once...
ifneq ($(BTBCONNECTOR_INCLUDED),true)
BTBCONNECTOR_INCLUDED = true

CFLAGS += -D'_BTBCONNECTOR_INCLUDED_=$(BTBCONNECTOR_INCLUDED)' 
CFLAGS += -D'_BTBCONNECTOR_HEADER_="$(BTBCONNECTOR_LIB).h"' 

ifndef CIRBUFF_LIB
CFLAGS += -D'CIRBUFF_NO_CALLOC=TRUE'
VER_CIRBUFF = 0.99
CIRBUFF_LIB = $(COMDIR)/cirBuff/$(VER_CIRBUFF)/cirBuff
include $(CIRBUFF_LIB).mk
endif

ifeq ($(BTBC_INLINE), TRUE)
CFLAGS += -D'_BTBC_INLINE_=TRUE'
# Extract the directory we're in and add this to COM_HEADERS...
COM_HEADERS += $(dir $(BTBCONNECTOR_LIB))
else
CFLAGS += -D'_BTBC_INLINE_=FALSE'
#Add this library's source code to SRC
SRC += $(BTBCONNECTOR_LIB).c
endif

endif
