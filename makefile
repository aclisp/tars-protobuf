
#-----------------------------------------------------------------------

APP       := ProtoBufDemo
TARGET    := MyProtoBufApp
CONFIG    := 
STRIP_FLAG:= N

INCLUDE   += 
LIB       += -lprotobuf

LOCAL_SRC += flight.pb.cc

#-----------------------------------------------------------------------

include /usr/local/tars/cpp/makefile/makefile.tars

#-----------------------------------------------------------------------
