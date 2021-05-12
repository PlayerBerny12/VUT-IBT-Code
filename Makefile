# ************************************************************************** #
# **   Project: Aplikace pro řízený přístup                               ** #
# **            ke vzdáleným dokumentům pro GNU/Linux                     ** #
# **   University: VUT FIT Brno                                           ** #
# **   Authors: Jan Bernard                                               ** #
# **   Created: 17.11.2020                                                ** #
# **   Module: makefile                                                   ** #
# ************************************************************************** #
.PHONY: run_unit_tests build build_unit_tests doxygen clean

# C compiler variables
C = gcc
CFLAGS =-std=gnu17 -Wall -Wextra -pedantic

# C++ compiler variables
CPP = g++
CPPFLAGS =-std=c++17 -Wall -Wextra -pedantic

# Object files directory
OBJDIR = obj

# Source and object files 
SRCFILES = api.cpp database.cpp file.cpp notification.cpp utils.cpp
OBJFILES = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCFILES))

# Source and object files for tests
SRCTESTFILES = tests_ping.cpp tests_auth.cpp tests_file.cpp
OBJTESTFILES = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCTESTFILES))

# Names of executables
TARGET = vdu-app
TARGETFUSE = vdu-app-fuse
TARGETTESTS = vdu-app-tests

all: build build_fuse build_unit_tests

run_unit_tests: build_unit_tests
	./$(TARGETTESTS)

run_behave_tests:
	python3 -m behave tests/behave

build: $(OBJFILES)
	$(CPP) $(CPPFLAGS) -o $(TARGET) $^ src/string.h src/main.cpp -lcurl -lsqlite3 -lcrypto \
	`pkg-config --libs --cflags libnotify` \
	`pkg-config --libs --cflags libsecret-1`

# arguments for different setup
#	-DVDU_DEV		= setup for dev enviroment (using placeholders for specific http header values etc)
#	-DVDU_HTTP_3	= setup cURL to use HTTP 3 
$(OBJDIR)/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) -DVDU_DEV -c -o $@ $< \
	`pkg-config --libs --cflags libnotify` \
	`pkg-config --libs --cflags libsecret-1`

build_fuse:
	$(C) $(CFLAGS) -o $(TARGETFUSE) src/fuse/main.c `pkg-config --cflags --libs fuse3`

build_unit_tests: $(OBJTESTFILES) $(OBJFILES)
	$(CPP) $(CPPFLAGS) -pthread -o $(TARGETTESTS) $^ src/string.h tests/unit/tests.cpp -lgtest -lpthread -lcurl -lsqlite3 -lcrypto \
	`pkg-config --libs --cflags libnotify` \
	`pkg-config --libs --cflags libsecret-1`

$(OBJDIR)/%.o: tests/unit/%.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< \
	`pkg-config --libs --cflags libnotify` \
	`pkg-config --libs --cflags libsecret-1`

doxygen:
	doxygen Doxyfile

clean:
	rm -f $(TARGET) $(TARGETFUSE) $(TARGETTESTS)
	rm -f $(OBJDIR)/*.o
	rm -rf docs/generated_docs_html