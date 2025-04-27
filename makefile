# CXX = g++
CXX = distrobox-enter debian -- g++
CXXFLAGS = -lGL -lGLU -lglut -std=c++14

TARGET = projetDjivan.out

SRCS = $(wildcard Eleve*.cpp) $(wildcard Eleve*.h) $(OBJ_LIB)
# SRC_LIB = G2DKey.cpp G2DMouse.cpp G2DX.cpp G2DColor.cpp G2DMain.cpp G2DPPM.cpp V2.cpp
SRC_LIB = G2DColor.cpp  G2DKey.cpp  G2DMain.cpp  G2DMouse.cpp  G2DPNGreader.cpp  G2DX.cpp  lodepng.cpp  V2.cpp
SRC_H_LIB = $(SRC_LIB:.cpp=.h)  # Conversion des fichiers .cpp en .h
OBJ_LIB = $(SRC_LIB:.cpp=.o)  # Conversion des fichiers .cpp en .o

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) -o $(TARGET)

library.o: $(SRC_LIB) $(SRC_H_LIB)
	$(CXX) $(CXXFLAGS) -c $(SRC_LIB)

clean:
	rm -f $(TARGET) *.o *.out


# cd /var/home/dd/MEGA/Documents/School/ESIEE/S2/EL-3034/TD/
