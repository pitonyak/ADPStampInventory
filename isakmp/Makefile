CC:=gcc
CXX:=g++
CFLAGS:=-O3 -fPIC
#
# Do NOT sanitize alignment or there will be lots of errors while accessing pcap data that is
# is not aligned. 
# If you use the sanitizer, then you must install the sanitizer module and the code will
# run slower. Good for debugging, bad for large runtimes. 
# see https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
#
# -O3		More optimizations than -O2
# -fPIC 	emit position-independent code.
# -Werror 	Make all warnings errors
# -Wall 	Turn on all warnings (well almost all)
# -Wextra	Turn on a few more not covered with -Wall (see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
#
# -fsanitize=address 	Enable the AddressSanitizer (fast memory error detector)
# -fsanitize=undefined	UndefinedBehaviorSanitizer
# -fno-sanitize=alignment	Of there is a complaint while accessing types at improper boundaries. Only a problem because -fsantize prior.
#
CXXFLAGS:=-O3 -fPIC -std=c++17
#CXXFLAGS:=-O3 -fPIC -std=c++17 -Werror -Wall -Wextra -fsanitize=undefined,address -fno-sanitize=alignment -g
#CXXFLAGS:=-O3 -fPIC -std=c++17 -Werror -Wall -Wextra 
INCLUDE:=-I/work1/home/emeneker/local_build/install/libpcap/1.9.1/include
LIBS:=-lpcap

# So clean is always considered out of date.
.PHONY=clean

# Add .d to Make's recognized suffixes.
SUFFIXES += .d

#We don't need to clean up when we're making these targets
NODEPS:=clean

# Find all the C++ files in the ./ directory
# Remove leading ./
SOURCES:=$(shell find . -name "*.cpp")
SOURCES:=$(patsubst ./%,%,$(SOURCES))
#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst %.cpp,%.d,$(SOURCES))
OBJFILES:=$(patsubst %.cpp,%.o,$(SOURCES))
EXEFILES:=isakmp_vendor

# I can avoid this by listing the DEPFILES as part of all.
#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards

    all: $(DEPFILES) $(OBJFILES) $(EXEFILES) 
    -include $(DEPFILES)
endif

#$(info SOURCES: $(SOURCES))
$(info DEPFILES: $(DEPFILES))
$(info OBJFILES: $(OBJFILES))
$(info EXEFILES: $(EXEFILES))

#This is the rule for creating the dependency files
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst %.cpp,%.o,$<)' $< -MF $@

#This rule does the compilation
%.o: %.cpp %.d %.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

isakmp_vendor: main.o ip_address_pairs.o MurmurHash3.o
	$(CXX) $(CXXFLAGS) main.o ip_address_pairs.o MurmurHash3.o -o isakmp_vendor $(LDFLAGS) $(LIBS)

clean:
	rm -f $(DEPFILES) $(OBJFILES) $(EXEFILES)
