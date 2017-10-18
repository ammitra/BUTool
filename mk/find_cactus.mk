CACTUS_ROOT?=/opt/cactus
CXXFLAGS+=-isystem $(CACTUS_ROOT)/include
LDFLAGS+=-L$(CACTUS_ROOT)/lib
LDFLAGS+=-Wl,-rpath=$(CACTUS_ROOT)/lib
