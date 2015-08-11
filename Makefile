CC			= g++
CPPFLAGS	= -std=c++11 -Wall `pkg-config --cflags gtkmm-3.0 opencv`
LDFLAGS		= `pkg-config --libs gtkmm-3.0 opencv`
SRCS		= main.cpp colorwindow.cpp
OBJS		= $(SRCS:.cpp=.o)
PROG		= main.out

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CPPFLAGS) $< -c -o $@

clean:
	rm $(OBJS) $(PROG)
