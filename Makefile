V=1
SOURCE_DIR=src
BUILD_DIR=build

TOOLS_DIR=libdragon/tools
TOOLS_MKSPRITE=$(N64_INST)/bin/mksprite

IMAGES_DIR=$(SOURCE_DIR)/images
IMAGE_FILES=$(wildcard $(IMAGES_DIR)/*.png)

IMAGE_FILES=$(wildcard $(IMAGES_DIR)/*tank/*/*.png)

SPRITE_DIR=$(BUILD_DIR)/filesystem
SPRITE_FILES=$(subst $(subst /,_,$(IMAGES_DIR)/),$(SPRITE_DIR)/,$(subst /,_,$(IMAGE_FILES:.png=.sprite)))

ROM_TITLE="Tank Shootout 64"

include $(N64_INST)/include/n64.mk

all: emu-compat n64-compat
.PHONY: all

emu-compat: tankshootout64.emu-compat.z64
n64-compat: tankshootout64.z64

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SPRITE_DIR): $(BUILD_DIR)
	mkdir -p $(SPRITE_DIR)

$(SPRITE_FILES): $(IMAGE_FILES) $(SPRITE_DIR)
	@echo "SPRITE_FILES: $(IMAGE_FILES)"
	@echo "target: $@"
	@echo "source: $(subst $(SPRITE_DIR),$(IMAGES_DIR),$(subst sprite,png,$(subst _,/,$@)))"
	@echo "	[MKSPRITE] Processing image" $(notdir $(subst sprite,png,$@))
	$(TOOLS_MKSPRITE) 16 1 1 $(subst $(SPRITE_DIR),$(IMAGES_DIR),$(subst sprite,png,$(subst _,/,$@))) $@


$(BUILD_DIR)/tankshootout64.dfs: $(SPRITE_FILES) $(wildcard $(BUILD_DIR)/filesystem/*)
$(BUILD_DIR)/tankshootout64.emu-compat.dfs: $(SPRITE_FILES) $(wildcard $(BUILD_DIR)/filesystem/*)

OBJS = $(BUILD_DIR)/main.o \
	$(BUILD_DIR)/physics.o \
	$(BUILD_DIR)/tank.o \
	$(BUILD_DIR)/fps.o

tankshootout64.z64: N64_ROM_TITLE=$(ROM_TITLE)
tankshootout64.z64: N64_HEADERPATH=boot/ipl3_compat.z64
tankshootout64.z64: $(BUILD_DIR)/tankshootout64.dfs

tankshootout64.emu-compat.z64: N64_ROM_TITLE=$(ROM_TITLE)
tankshootout64.emu-compat.z64: N64_HEADERPATH=boot/libdragon/boot/bin/ipl3_compat.z64
tankshootout64.emu-compat.z64: $(BUILD_DIR)/tankshootout64.emu-compat.dfs

$(BUILD_DIR)/tankshootout64.elf: $(OBJS)
$(BUILD_DIR)/tankshootout64.emu-compat.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
