CC=gcc
#CC=C:/cygwin64/bin/i686-w64-mingw32-gcc.exe
# CFLAGS=-c -Wall -Wextra -Werror -std=c++17
#CFLAGS=-c -Wextra -Werror -std=c++1z -ltbb -lpthread
CFLAGS=-c -g -std=c++17 -Wextra -Werror  -ltbb -lpthread
LDFLAGS=
SOURCES=\
main.cpp search_server.cpp \
string_processing.cpp \
request_queue.cpp \
read_input_functions.cpp \
remove_duplicates.cpp \
document.cpp\
process_queries.cpp\
test_example_functions.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main
RM = del /Q /F
#RM = rm -f

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -lstdc++ -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -lstdc++ -o $@
#$(CC) $(CFLAGS) $< -lstdc++ -o $@

clean:
	@echo "clean project"
	-$(RM) main.exe *.o
	@echo "clean completed"

# $@ — имя .o-файла
# $< — имя .cpp-файла