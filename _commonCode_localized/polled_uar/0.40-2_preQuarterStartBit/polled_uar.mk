# polled_uar depends on tcnter

# The following variables are expected to be defined 
#  in the calling makefile(s)
# POLLED_UAR_LIB - Path to polled_uar.mk/c including filename, 
#  excluding extension
# COMDIR
#
# PUAR_INLINE (optional, NOT a CFLAG) to inline functions...
#
#  CFLAGS += -D'...
#  BIT_TCNT	- Number of TCNTS per bit...
#             If not defined, then you must either manually set
#             bitTcnt[puarNum] or run puar_calibrate 
#             THIS AFFECTS ALL PUARS _AND_ PUATS!
#             e.g. CFLAGS+=-D'BIT_TCNT=26'
#
#	NUMPUARS - Number of polled_uars (Required if > 1)
#
#
# CONSIDERATIONS:
#
# *** These could have defaults... ***
# TCNTER_SOURCE_VAR - e.g. TCNT0
#   might take some work if 
#   TCNTER_SOURCE_VAR is a variable which needs to be externed
#   TCNTER_SOURCE_EXTERNED = TRUE to add externing...
# TCNTER_SOURCE_MAX   - e.g. OCR0A
#   also, could be a constant value...
#
# OPTIONAL (change BOTH if either)
# tcnter_source_t - e.g. uint8_t, uint16_t
# tcnter_compare_t - e.g. int16_t, int32_t
#
# TCNTER_STUFF_IN_MAIN  - probably should be default...
#                        prevents running tcnter_update from uar_update()
#								 also tcnter_init from uar_init()
#                        don't forget to call them in the main while loop!
# CFLAGS += -D'PU_PC_DEBUG=TRUE' -> PC Debugging printf's...

#only include polled_uar once...
ifneq ($(POLLED_UAR_INCLUDED),true)
POLLED_UAR_INCLUDED = true

CFLAGS += -D'_POLLED_UAR_INCLUDED_=$(POLLED_UAR_INCLUDED)' 
CFLAGS += -D'_POLLED_UAR_HEADER_="$(POLLED_UAR_LIB).h"' 

ifndef TCNTER_LIB
VER_TCNTER = 0.10
TCNTER_LIB = $(COMDIR)/tcnter/$(VER_TCNTER)/tcnter
include $(TCNTER_LIB).mk
endif

ifeq ($(PUAR_INLINE), TRUE)
CFLAGS += -D'_PUAR_INLINE_=TRUE'
# Extract the directory we're in and add this to COM_HEADERS...
COM_HEADERS += $(dir $(POLLED_UAR_LIB))
else
CFLAGS += -D'_PUAR_INLINE_=FALSE'
#Add this library's source code to SRC
SRC += $(POLLED_UAR_LIB).c
endif

endif
