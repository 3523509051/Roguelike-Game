CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g
SRC_DIR = src
TEST_DIR = test

# 所有源文件
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/Map.cpp $(SRC_DIR)/Entity.cpp \
       $(SRC_DIR)/Player.cpp $(SRC_DIR)/Monster.cpp $(SRC_DIR)/Item.cpp \
       $(SRC_DIR)/Combat.cpp $(SRC_DIR)/Render.cpp $(SRC_DIR)/Input.cpp \
       $(SRC_DIR)/Save.cpp $(SRC_DIR)/Game.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = dungeon.exe

# 默认目标：构建游戏
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译规则
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# 测试目标
test_map: $(SRC_DIR)/Map.o
	$(CXX) $(CXXFLAGS) -o test_map.exe $(TEST_DIR)/test_map.cpp $(SRC_DIR)/Map.o

test_combat: $(SRC_DIR)/Entity.o $(SRC_DIR)/Player.o $(SRC_DIR)/Monster.o $(SRC_DIR)/Combat.o $(SRC_DIR)/Item.o
	$(CXX) $(CXXFLAGS) -o test_combat.exe $(TEST_DIR)/test_combat.cpp \
		$(SRC_DIR)/Entity.o $(SRC_DIR)/Player.o $(SRC_DIR)/Monster.o \
		$(SRC_DIR)/Combat.o $(SRC_DIR)/Item.o

test_render: $(SRC_DIR)/Map.o $(SRC_DIR)/Entity.o $(SRC_DIR)/Player.o \
             $(SRC_DIR)/Monster.o $(SRC_DIR)/Item.o $(SRC_DIR)/Render.o \
             $(SRC_DIR)/Input.o $(SRC_DIR)/Save.o
	$(CXX) $(CXXFLAGS) -o test_render.exe $(TEST_DIR)/test_render.cpp \
		$(SRC_DIR)/Map.o $(SRC_DIR)/Entity.o $(SRC_DIR)/Player.o \
		$(SRC_DIR)/Monster.o $(SRC_DIR)/Item.o $(SRC_DIR)/Render.o \
		$(SRC_DIR)/Input.o $(SRC_DIR)/Save.o

.PHONY: clean
clean:
	del /Q src\*.o 2>nul || true
	del /Q *.exe 2>nul || true
