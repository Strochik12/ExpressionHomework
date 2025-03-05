CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++20 -w -O2

SRCTESTS = tests.cpp Expression.cpp

# Список объектных файлов
OBJTESTS = $(SRCTESTS:.cpp=.o) 

# Имя итогового исполняемого файла
TARGETTESTS = tests.exe

all: $(TARGETTESTS)

# Правило для сборки исполняемого файла
$(TARGETTESTS): $(OBJTESTS)
	$(CXX) $^ -o $@

# Правило для компиляции .cpp в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: test
test: $(TARGETTESTS)
	./$(TARGETTESTS)

# Команда для удаления скомпилированных файлов
clean:
	rm -f *.o $(TARGETTESTS)