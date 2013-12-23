#Stolen from polled_uar 0.10-5
# TARGET gets put in the _BUILD directory... not sure how I feel about that
TARGET = polled_uat_Test

MY_SRC = main.c



COMREL = ../../../..
COMDIR = $(COMREL)/_commonCode


# These are global variables in--and values specific to--main.c
# To override the AVR stuff...
#extern void puat_writeOutput(uint8_t puatNum, uint8_t value);
#CFLAGS += -D'puat_writeOutput(a,b)=(printf("writeOutput[%d]: %d\n", a, b))'
CFLAGS += -D'TCNTER_SOURCE_VAR=(timerCounter)'
CFLAGS += -D'TCNTER_SOURCE_EXTERNED=TRUE'
CFLAGS += -D'TCNTER_SOURCE_MAX=(9)'
CFLAGS += -D'BIT_TCNT=6'

CFLAGS += -D'NUMPUATS=2'

# This *should* ONLY enable debug-printing,
# But it seems to be doing more than that...
CFLAGS += -D'PU_PC_DEBUG=TRUE'

# This is nice because it doesn't require a version number...
# But, when compiling, it places the intermediate files (.d, .o)
#  outside the build directory (one level higher, due to "../")
# Smarter, maybe, would be to extract the version number from the current
#  working-directory, and use my normal method
#  of $(COMDIR)/polled_uat/$(VER_POLLED_UAT)/polled_uat
POLLED_UAT_LIB = ../polled_uat
include $(POLLED_UAT_LIB).mk



include $(COMDIR)/_make/reallyCommon.mk


