CXXFLAGS=-Wall -Wpedantic -std=c++1y
SRCS=main.cpp
TARGET=hikaliumd

$(TARGET) : $(SRCS) Makefile
	g++ $(CXXFLAGS) -o $(TARGET) $(SRCS)

run : $(TARGET)
	./$(TARGET)

clean:
	-rm *.o
	-rm $(TARGET)

