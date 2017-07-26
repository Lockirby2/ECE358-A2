CXX = g++										# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD -pthread -std=c++11

OBJECTS1 = main.o TCB.o UDPServer.o segment.o message.o checkSumUtil.o
OBJECTS2 = clientMain.o UDPClient.o

EXEC1 = pa2-358s17
EXEC2 = testClient

OBJECTS = ${OBJECTS1} ${OBJECTS2} 				# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}				# all executables

.PHONY : all clean

all : ${EXECS}									# build all executables

#############################################################

${EXEC1} : ${OBJECTS1}							# link step 1st executable
	${CXX} ${CXXFLAGS} $^ -o $@

${EXEC2} : ${OBJECTS2}							# link step 2st executable
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

-include ${DEPENDS}

clean :											# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} ImplType
