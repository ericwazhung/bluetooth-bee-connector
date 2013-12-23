# tcnter (doesn't even) depends only on bithandling, which isn't a library..

# The following variables are expected to be defined 
#  in the calling makefile(s)
# TCNTER_LIB - Path to tcnter.mk/c including filename, 
#  excluding extension
# COMDIR
#
#	TCNTER_INLINE (optional, NOT a CFLAG)
#
# CONSIDERATIONS:
#
# TCNTER_SOURCE_VAR - e.g. TCNT0
#   might take some work if 
#   TCNTER_SOURCE_VAR is a variable which needs to be externed
#   TCNTER_SOURCE_EXTERNED = TRUE to add externing...
# TCNTER_SOURCE_OVERFLOW_VAL   - e.g. OCR0A
#   also, could be a constant value...
#   (if TCNT is 8-bit and overflows on its own, this should be 256)
#
# OPTIONAL (change BOTH if either)
# tcnt_source_t - e.g. uint8_t, uint16_t
# tcnt_compare_t - e.g. int16_t, int32_t

#only include tcnter once...
ifneq ($(TCNTER_INCLUDED),true)
TCNTER_INCLUDED = true

CFLAGS += -D'_TCNTER_INCLUDED_=$(TCNTER_INCLUDED)' 
CFLAGS += -D'_TCNTER_HEADER_="$(TCNTER_LIB).h"' 

ifeq ($(TCNTER_INLINE), TRUE)
CFLAGS += -D'_TCNTER_INLINE_=TRUE'
# Extract the directory we're in and add this to COM_HEADERS...
COM_HEADERS += $(dir $(TCNTER_LIB))
else
CFLAGS += -D'_TCNTER_INLINE_=FALSE'
#Add this library's source code to SRC
SRC += $(TCNTER_LIB).c
endif

endif
