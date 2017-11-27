CXXFLAGS=-Wall -Wpedantic -std=c++1y
SRCS=main.o
TARGET=hikaliumd

$(TARGET) : $(SRCS) Makefile
	g++ -o $(TARGET) $(SRCS)

run : $(TARGET)
	./$(TARGET)

clean:
	-rm *.o
	-rm $(TARGET)

