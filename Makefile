CC := gcc
TARGET := bot_net

LD_FLAGS :=
CC_FLAGS := -MMD

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

-include $(OBJFILES:.o=.d)