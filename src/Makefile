ARCH    := $(shell uname -s | tr a-z A-Z)

PROG	:=	sable

CFLAGS	:=	-g -O0 -std=gnu99 -Wall -fopenmp
ifeq ($(ARCH),DARWIN)
CFLAGS	+=	-I /opt/local/include
LDFLAGS	+=	-L /opt/local/include
LDLIBS	+=	-framework GLUT -framework OpenGL -framework OpenCL
else
LDLIBS		:= -lOpenCL -lGL -lGLU -lglut -lm -fopenmp
endif

SRC=$(wildcard *.c)
SRC+=$(wildcard util/*.c)
CL_SOURCE=$(wildcard *.cl)
OBJ=$(SRC:.c=.o)
CFLAGS+=-DKERNEL_FILE=\"$(CL_SOURCE)\"

.phony: default clean

default: $(PROG)

$(PROG): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

%.o: %.h

clean:
	$(RM) *~ *.o util/*.o $(PROG)


