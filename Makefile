# Option Cortex specific library

LIBLINKNAME=libowire.so
LIBLINKSOURCEDIR=lib/general/link/CORTEX
LIBLINKSOURCE_OBJ= cortexll.o cortexses.o
LIBLINKSOURCE=$(LIBLINKSOURCE_OBJ:%=$(LIBLINKSOURCEDIR)/%)

LIBOWSNAME=libows.a

all: $(LIBOWSNAME) $(LIBLINKNAME)


$(LIBLINKNAME): $(LIBLINKSOURCE)
	$(CC) -shared -fPIC -o $@ $^

%.o: %.c
	$(CC) -Iinclude -I. -Icommon/  $(CFLAGS) -Wall -fPIC -shared -c $< -o $@

# Dallas-sdk common/ objects
# Add here files from common directory
DALLAS_SDK_COMMON_SOURCE_PATH=common
DALLAS_SDK_COMMON_SOURCE_OBJ=crcutil.o owerr.o findtype.o atod26.o temp28.o swt29.o
DALLAS_SDK_COMMON_OBJ=$(DALLAS_SDK_COMMON_SOURCE_OBJ:%=$(DALLAS_SDK_COMMON_SOURCE_PATH)/%)

# Dallas-sdk lib/general/shared/ objects
# Add here files from shared directory
DALLAS_SDK_SHARED_SOURCE_PATH=lib/general/shared
DALLAS_SDK_SHARED_SOURCE_OBJ=ownet.o owtran.o
DALLAS_SDK_SHARED_OBJ=$(DALLAS_SDK_SHARED_SOURCE_OBJ:%=$(DALLAS_SDK_SHARED_SOURCE_PATH)/%)

OBJS=$(DALLAS_SDK_COMMON_OBJ) $(DALLAS_SDK_SHARED_OBJ)

$(info COMMON_OBJ >>> $(DALLAS_SDK_COMMON_OBJ))
$(info SHARED_OBJ >>> $(DALLAS_SDK_SHARED_OBJ))

$(LIBOWSNAME): $(OBJS)
	rm -f $@
	$(AR) -rs $@ $(OBJS)

clean:
	rm -f $(LIBOWSNAME) $(LIBLINKNAME) $(DALLAS_SDK_COMMON_SOURCE_PATH)/*.o $(DALLAS_SDK_SHARED_SOURCE_PATH)/*.o
