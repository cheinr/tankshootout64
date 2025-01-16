V=1
SOURCE_DIR=src
BUILD_DIR=build

TOOLS_DIR=libdragon/tools
TOOLS_MKSPRITE=$(N64_INST)/bin/mksprite

IMAGES_DIR=$(SOURCE_DIR)/images
IMAGE_FILES=$(wildcard $(IMAGES_DIR)/*.png)

SPRITE_DIR=$(BUILD_DIR)/filesystem
SPRITE_FILES=$(subst $(IMAGES_DIR),$(SPRITE_DIR),$(IMAGE_FILES:.png=.sprite))

include $(N64_INST)/include/n64.mk

all: tankshootout64.z64
.PHONY: all

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SPRITE_DIR): $(BUILD_DIR)
	mkdir -p $(SPRITE_DIR)

$(SPRITE_FILES): $(IMAGE_FILES) $(SPRITE_DIR)
	@echo "SPRITE_FILES: $(SPRITE_FILES)"
	@echo "	[MKSPRITE] Processing image" $(notdir $(subst sprite,png,$@))
	$(TOOLS_MKSPRITE) 16 63 10 $(IMAGES_DIR)/$(notdir $(subst sprite,png,$@)) $@

$(BUILD_DIR)/tankshootout64.dfs: $(SPRITE_FILES) $(wildcard $(BUILD_DIR)/filesystem/*)

OBJS = $(BUILD_DIR)/main.o

tankshootout64.z64: N64_ROM_TITLE="Tank64 Demo"
tankshootout64.z64: N64_HEADERPATH=boot/libdragon/boot/bin/ipl3_compat.z64
tankshootout64.z64: $(SPRITE_FILES) $(BUILD_DIR)/tankshootout64.dfs

$(BUILD_DIR)/tankshootout64.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
