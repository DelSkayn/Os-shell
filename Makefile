#
# Author: Mees Delzenne s1531255
# Mick Voogt s1542125
#
COMP=gcc
BIN_NAME = shell
SRC_EXT = c
SRC_PATH = ./src
BUILD_PATH = ./bin
FLAGS = -std=c99 -Wall -Wextra -pedantic -g 
INCLUDES = -I $(SRC_PATH)/
ZIP_NAME = assignment1

SOURCES := $(wildcard $(SRC_PATH)/*.$(SRC_EXT))
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)

.PHONY: dirs
dirs:
	mkdir -p $(BUILD_PATH)
	make debug

all: dirs

debug: $(BIN_NAME)

$(BIN_NAME): $(OBJECTS)
	$(COMP) $(OBJECTS) -o $@


$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	$(COMP) $(FLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(OBJECTS:%.o=%.d)
	rm -d -f $(BUILD_PATH)
	rm -f $(BIN_NAME)

zip: 
	make clean
	rm $(ZIP_NAME).tar.gz
	mkdir $(ZIP_NAME)
	cp -rf src $(ZIP_NAME)/
	cp Makefile $(ZIP_NAME)/
	tar -czvf $(ZIP_NAME).tar.gz $(ZIP_NAME)
	rm -rf $(ZIP_NAME)
