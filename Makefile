CXX = g++										# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD -pthread -std=c++11

OBJECTS1 = main.o TCB.o server.o segment.o proxy.o
EXEC1 = pa2-358s17

OBJECTS = ${OBJECTS1} 				# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1}				# all executables

.PHONY : all clean

all : ${EXECS}									# build all executables

#############################################################

${EXEC1} : ${OBJECTS1}							# link step 1st executable
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

-include ${DEPENDS}

clean :											# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} ImplType
