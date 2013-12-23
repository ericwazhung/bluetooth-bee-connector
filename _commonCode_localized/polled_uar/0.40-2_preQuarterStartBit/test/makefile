#Stolen from hfmTest3
# TARGET gets put in the _BUILD directory... not sure how I feel about that
TARGET = polled_uar_Test

MY_SRC = main.c



COMREL = ../../../..
COMDIR = $(COMREL)/_commonCode


# These are global variables in--and values specific to--main.c
# To override the AVR stuff...
#extern uint8_t puar_readInput(uint8_t puarNum)
#CFLAGS += -D'getbit(...)=(lineVal)'
CFLAGS += -D'TCNTER_SOURCE_VAR=(timerCounter)'
CFLAGS += -D'TCNTER_SOURCE_EXTERNED=TRUE'
CFLAGS += -D'TCNTER_SOURCE_MAX=(9)'
CFLAGS += -D'BIT_TCNT=6'
CFLAGS += -D'TCNTER_STUFF_IN_MAIN=TRUE'

CFLAGS += -D'NUMPUARS=2'

# This *should* ONLY enable debug-printing,
# But it seems to be doing more than that...
CFLAGS += -D'PU_PC_DEBUG=TRUE'

# This is nice because it doesn't require a version number...
# But, when compiling, it places the intermediate files (.d, .o)
#  outside the build directory (one level higher, due to "../")
# Smarter, maybe, would be to extract the version number from the current
#  working-directory, and use my normal method
#  of $(COMDIR)/polled_uar/$(VER_POLLED_UAR)/polled_uar
POLLED_UAR_LIB = ../polled_uar
include $(POLLED_UAR_LIB).mk



include $(COMDIR)/_make/reallyCommon.mk


