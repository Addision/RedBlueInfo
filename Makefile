
CC = gcc
CXX = g++
CFLAGS = -g -DMAP 
LDFLAGS = -lrt -lhtmlcxx -lcurl

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %cpp, %o, $(SRCS))
INCS = -I/usr/local/include/htmlcxx/html -I/usr/include/curl

TARGET = Remind
all: $(TARGET)

$(TARGET) : $(OBJS)
		$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

%.o : %.cpp
		$(CXX) $(CFLAGS) -c $^ -o $@ $(INCS)	

.PHONY : clean

clean :
		rm -f $(TARGET)
		find ./ -name "*.o" -exec rm -rf {} \;

