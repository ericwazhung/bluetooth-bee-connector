#Stolen from tcnter 0.20 from polled_uar 0.10-6 from hfmTest3
# TARGET gets put in the _BUILD directory... not sure how I feel about that
TARGET = btConnector

MY_SRC = main.c



COMREL = ../../../..
COMDIR = $(COMREL)/_commonCode



CFLAGS += -D'BTBC_PC_DEBUG=TRUE'
VER_BTBCONNECTOR = 0.10
BTBCONNECTOR_LIB = $(COMDIR)/btbConnector/$(VER_BTBCONNECTOR)/btbConnector
include $(BTBCONNECTOR_LIB).mk



include $(COMDIR)/_make/reallyCommon.mk

