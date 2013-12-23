# cirBuff doesn't depend on other libraries

# The following variables are expected to be defined in the calling makefile(s)
# CIRBUFF_LIB
# TCOMDIR

#only include midi_out once...
ifneq ($(CIRBUFF_INCLUDED),true)

CIRBUFF_INCLUDED = true
CFLAGS +=  -D'_CIRBUFF_INCLUDED_=$(CIRBUFF_INCLUDED)'
CFLAGS += -D'_CIRBUFF_HEADER_="$(CIRBUFF_LIB).h"'

#Add this library's source code to SRC
SRC += $(CIRBUFF_LIB).c

endif
