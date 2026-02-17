CXX = g++

CXXFLAGS = -Wall

TARGET = traffic_sim

all: $(TARGET)

$(TARGET): src/traffic_sim.cpp
	$(CXX) $(CXXFLAGS) src/traffic_sim.cpp -o $(TARGET)

clean:
	rm -f $(TARGET)