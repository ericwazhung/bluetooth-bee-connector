#Stolen from polled_uar 0.10-6 from hfmTest3
# TARGET gets put in the _BUILD directory... not sure how I feel about that
TARGET = tcnter_Test

MY_SRC = main.c



COMREL = ../../../..
COMDIR = $(COMREL)/_commonCode


# These are global variables in--and values specific to--main.c
# To override the AVR stuff...
CFLAGS += -D'TCNTER_SOURCE_VAR=timerCounter'
CFLAGS += -D'TCNTER_SOURCE_EXTERNED=TRUE'
CFLAGS += -D'TCNTER_SOURCE_OVERFLOW_VAL=8'

# This *should* ONLY enable debug-printing,
# But it seems to be doing more than that...
CFLAGS += -D'PU_PC_DEBUG=TRUE'

# This is nice because it doesn't require a version number...
# But, when compiling, it places the intermediate files (.d, .o)
#  outside the build directory (one level higher, due to "../")
# Smarter, maybe, would be to extract the version number from the current
#  working-directory, and use my normal method
#  of $(COMDIR)/tcnter/$(VER_TCNTER)/tcnter
TCNTER_LIB = ../tcnter
include $(TCNTER_LIB).mk



include $(COMDIR)/_make/reallyCommon.mk


