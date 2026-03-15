CXX ?= g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I../myaotron

test: test_detection_logic
	./test_detection_logic

test_detection_logic: test_detection_logic.cpp ../myaotron/detection_logic.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f test_detection_logic

.PHONY: test clean
