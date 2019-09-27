INC_DIR:=./include
SRC_DIR:=./src
EXE_DIR:=./bin

TARGET:=$(EXE_DIR)/main
OBJS:= $(wildcard $(SRC_DIR)/*.cc)

CXX:=g++
CXXFLAGS:=-g -Wall -std=c++11

LIBS:= -lgflags -lpthread -llua -ldl

$(TARGET):$(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf $(TARGET)
