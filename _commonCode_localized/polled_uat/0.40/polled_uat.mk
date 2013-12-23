# polled_uat depends on tcnter

# The following variables are expected to be defined 
#  in the calling makefile(s)
# POLLED_UAT_LIB - Path to polled_uat.mk/c including filename, 
#  excluding extension
# COMDIR
#
# PUAT_INLINE (optional, NOT a CFLAG) to inline functions...
#
# CFLAGS += -D'...
# NUMPUATS	-	Number of polled_uats (Required if >1)
#
# BIT_TCNT	- Should probably no longer be used...
#             see polled_uar0.40 for better notes
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
#                        prevents running tcnter_update from uat_update()
#								 also tcnter_init from uat_init()
#                        don't forget to call them in the main while loop!
# CFLAGS += -D'PU_PC_DEBUG=TRUE' -> PC Debugging printf's...

#only include polled_uat once...
ifneq ($(POLLED_UAT_INCLUDED),true)
POLLED_UAT_INCLUDED = true

CFLAGS += -D'_POLLED_UAT_INCLUDED_=$(POLLED_UAT_INCLUDED)' 
CFLAGS += -D'_POLLED_UAT_HEADER_="$(POLLED_UAT_LIB).h"' 

ifndef TCNTER_LIB
VER_TCNTER = 0.10
TCNTER_LIB = $(COMDIR)/tcnter/$(VER_TCNTER)/tcnter
include $(TCNTER_LIB).mk
endif

ifeq ($(PUAT_INLINE), TRUE)
CFLAGS += -D'_PUAT_INLINE_=TRUE'
# Extract the directory we're in and add this to COM_HEADERS...
COM_HEADERS += $(dir $(POLLED_UAT_LIB))
else
CFLAGS += -D'_PUAT_INLINE_=FALSE'
#Add this library's source code to SRC
SRC += $(POLLED_UAT_LIB).c
endif

endif
