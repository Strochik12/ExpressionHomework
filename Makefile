CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++20 -w -O2

SRCTESTS = tests.cpp Expression.cpp
SRC = main.cpp Expression.cpp

OBJTESTS = $(SRCTESTS:.cpp=.o) 
OBJ = $(SRC:.cpp=.o)

TARGETTESTS = tests.exe
TARGET = differentiator



$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGETTESTS): $(OBJTESTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: test
test: $(TARGETTESTS)
	./$(TARGETTESTS)

# Команда для удаления скомпилированных файлов
clean:
	rm -f *.o $(TARGET) $(TARGETTESTS) $(OBJS) $(OBJTESTS)