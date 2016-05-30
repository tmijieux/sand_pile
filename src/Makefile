ARCH            := $(shell uname -s | tr a-z A-Z)

PROG	:=	sable

CFLAGS	:=	-g -O0 -std=c99 -Wall -fopenmp
ifeq ($(ARCH),DARWIN)
CFLAGS	+=	-I /opt/local/include
LDFLAGS	+=	-L /opt/local/include
LDLIBS	+=	-framework GLUT -framework OpenGL -framework OpenCL
else
LDLIBS		:= -lOpenCL -lGL -lGLU -lglut -lm -fopenmp
endif

SRC = main.o display.o sand_heap.o sand_builder.o colorer.o

.phony: default clean

default: $(PROG)

$(PROG): $(SRC)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

%.o: %.h

clean:
	$(RM) *~ *.o $(PROG)
