# placeholder

dirName = `basename $(PWD)`
currentDate = `date +%D | sed 's/\//-/g'`
fileName = ../$(dirName)-$(currentDate).tar.gz

HDR = $(wildcard src/*.hpp)
ALL_SRC = $(wildcard src/*.cpp)
BLACKLIST = src/calculate_stats_us_binpacking.cpp src/EvaluateMapping.cpp src/GeneticAlgorithm.cpp src/SimulatedAnnealing.cpp
SRC = $(filter-out $(BLACKLIST), $(ALL_SRC))
#HDR = code/*.cpp 
OBJ = ${SRC:.cpp=.o}
LINK = -lcrypto
EXE = main.exe
CPP = g++
CPPFLAGS = -O3 -g -Iincl/
STRIP = strip
.PHONY = clean export clean-all backup
TEMP = $(shell ls | grep -v "makefile" | grep -v "createdgraphs.log")

all: ${EXE}

#${OBJ}: ${SRC}
#		${CPP} ${CPPFLAGS} -c ${SRC} -o ${OBJ}

$(EXE): ${OBJ}
#		${CPP} ${CPPFLAGS} -o ${EXE} ${OBJ}
		${CPP} ${CPPFLAGS} ${OBJ} ${LINK} -o ${EXE}
		${STRIP} ${EXE}

%.o: %.cpp
		$(CPP) -c $(CPPFLAGS) $< -o $@

# Make modifiers
clean:
		rm -rf ${OBJ} ${EXE}
		-rm ${TEMP} #'-' at the start of the line forces make to ignore errors from this command

clean-all:
		rm -rf ${OBJ} ${EXE} input/* output/*

export:
		rm -rf ${OBJ} ${EXE}
		tar --exclude=*git* --exclude=*input* --exclude=*output* --exclude=.git* -czvf SDC.tar.gz ../SDC/*
backup:
	tar --exclude=*git* --exclude=*input* --exclude=*output* --exclude=.git* -czvf $(fileName) $(PWD)/*
