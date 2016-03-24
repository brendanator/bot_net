EXE = bot_net

CC = gcc
FLAGS = -Wall -Wextra -g
CC_FLAGS = $(FLAGS) -MMD
LD_FLAGS = $(FLAGS)

SRC_DIRECTORY = src
SRC_FILES = $(wildcard $(SRC_DIRECTORY)/*.c)
OBJ_DIRECTORY = obj
OBJ_FILES = $(addprefix $(OBJ_DIRECTORY)/,$(notdir $(SRC_FILES:.c=.o)))
TST_DIRECTORY = test
TST_FILES = $(wildcard $(TST_DIRECTORY)/*.c)
TST_OBJ_FILES = $(addprefix $(OBJ_DIRECTORY)/,$(notdir $(TST_FILES:.c=.o)))

.PHONY: all clean test $(TST_FILES:.c=)

all: $(OBJ_DIRECTORY) $(EXE)

$(EXE): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) -o $@ $^

$(OBJ_DIRECTORY):
	mkdir $(OBJ_DIRECTORY)

$(OBJ_DIRECTORY)/%.o: $(SRC_DIRECTORY)/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

$(OBJ_DIRECTORY)/%.o: $(TST_DIRECTORY)/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

$(notdir $(TST_FILES:.c=)): $(filter-out obj/main.o, $(OBJ_FILES))
	-@ $(CC) $(LD_FLAGS) -o $(TST_DIRECTORY)/$@ $(TST_DIRECTORY)/$@.c $^
	-@ $(TST_DIRECTORY)/$@

test: $(notdir $(TST_FILES:.c=))

clean:
	-rm -rf $(OBJ_DIRECTORY) $(EXE)

-include $(OBJ_FILES:.o=.d)
-include $(TST_OBJ_FILES:.o=.d)