CC = gcc
CFLAGS = -Iinclude
SRC = ./app/main.c ./src/hello.c ./src/mybase.c ./src/mydatatype.c ./src/mylog.c ./src/mystring.c

# 生成build目录下的目标文件路径（保留源文件的目录结构）
BUILD_DIR = ./build
OBJ = $(patsubst ./%,$(BUILD_DIR)/%,$(SRC:.c=.o))
TARGET = ./bin/main

all: $(TARGET)

# 创建build目录和bin目录
$(shell mkdir -p $(BUILD_DIR)/app $(BUILD_DIR)/src bin)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# 编译规则：将每个源文件编译到build目录对应的位置
$(BUILD_DIR)/%.o: ./%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/app/* $(BUILD_DIR)/src/* $(TARGET)