V=1
SOURCE_DIR=src
BUILD_DIR=build

TOOLS_DIR=libdragon/tools
TOOLS_MKSPRITE=$(N64_INST)/bin/mksprite

SPRITES_DIR=$(SOURCE_DIR)/filesystem
SPRITE_FILES=$(subst images,filesystem,$(IMAGE_FILES:.png=.sprite))

IMAGES_DIR=$(SOURCE_DIR)/images
IMAGE_FILES=$(wildcard $(IMAGES_DIR)/*.png)

include $(N64_INST)/include/n64.mk

all: hello.z64
.PHONY: all



$(SPRITE_FILES): $(IMAGE_FILES)
	@echo "	[MKSPRITE] Processing image" $(notdir $(subst sprite,png,$@))
	$(TOOLS_MKSPRITE) 16 1 2 $(IMAGES_DIR)/$(notdir $(subst sprite,png,$@)) $@

$(BUILD_DIR)/hello.dfs: $(wildcard $(SOURCE_DIR)/filesystem/*)

OBJS = $(BUILD_DIR)/main.o

hello.z64: N64_ROM_TITLE="Tank64 Demo"
hello.z64: $(SPRITE_FILES) $(BUILD_DIR)/hello.dfs

$(BUILD_DIR)/hello.elf: $(OBJS)

clean:
	rm -f $(BUILD_DIR)/* *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
