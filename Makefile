V=1
SOURCE_DIR=src
BUILD_DIR=build

TOOLS_DIR=libdragon/tools
TOOLS_MKSPRITE=$(N64_INST)/bin/mksprite

IMAGES_DIR=$(SOURCE_DIR)/images
IMAGE_FILES=$(wildcard $(IMAGES_DIR)/*.png)
TANK_BODY_IMAGE_FILES=$(wildcard $(IMAGES_DIR)/redtankbody*.png)
TANK_BARREL_IMAGE_FILES=$(wildcard $(IMAGES_DIR)/redtankbarrel*.png)

SPRITE_DIR=$(BUILD_DIR)/filesystem
TANK_BODY_SPRITE_FILES=$(subst $(IMAGES_DIR),$(SPRITE_DIR),$(TANK_BODY_IMAGE_FILES:.png=.sprite))
TANK_BARREL_SPRITE_FILES=$(subst $(IMAGES_DIR),$(SPRITE_DIR),$(TANK_BARREL_IMAGE_FILES:.png=.sprite))

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

$(TANK_BODY_SPRITE_FILES): $(TANK_BODY_IMAGE_FILES) $(SPRITE_DIR)
	@echo "TANK_BODY_SPRITE_FILES: $(TANK_BODY_SPRITE_FILES)"
	@echo "	[MKSPRITE] Processing image" $(notdir $(subst sprite,png,$@))
	$(TOOLS_MKSPRITE) 16 18 10 $(IMAGES_DIR)/$(notdir $(subst sprite,png,$@)) $@

$(TANK_BARREL_SPRITE_FILES): $(TANK_BARREL_IMAGE_FILES) $(SPRITE_DIR)
	@echo "TANK_BARREL_SPRITE_FILES: $(TANK_BARREL_IMAGE_FILES)"
	@echo "	[MKSPRITE] Processing image" $(notdir $(subst sprite,png,$@))
	$(TOOLS_MKSPRITE) 16 9 10 $(IMAGES_DIR)/$(notdir $(subst sprite,png,$@)) $@


$(BUILD_DIR)/tankshootout64.dfs: $(TANK_BODY_SPRITE_FILES) $(TANK_BARREL_SPRITE_FILES) $(wildcard $(BUILD_DIR)/filesystem/*)
$(BUILD_DIR)/tankshootout64.emu-compat.dfs: $(TANK_BODY_SPRITE_FILES) $(TANK_BARREL_SPRITE_FILES) $(wildcard $(BUILD_DIR)/filesystem/*)

OBJS = $(BUILD_DIR)/main.o \
	$(BUILD_DIR)/physics.o \
	$(BUILD_DIR)/tank.o \
	$(BUILD_DIR)/fps.o

tankshootout64.z64: N64_ROM_TITLE=$(ROM_TITLE)
tankshootout64.z64: N64_HEADERPATH=boot/ipl3_compat.z64
tankshootout64.z64: $(TANK_BODY_SPRITE_FILES) $(TANK_BARREL_SPRITE_FILES) $(BUILD_DIR)/tankshootout64.dfs

tankshootout64.emu-compat.z64: N64_ROM_TITLE=$(ROM_TITLE)
tankshootout64.emu-compat.z64: N64_HEADERPATH=boot/libdragon/boot/bin/ipl3_compat.z64
tankshootout64.emu-compat.z64: $(TANK_BODY_SPRITE_FILES) $(TANK_BARREL_SPRITE_FILES) $(BUILD_DIR)/tankshootout64.emu-compat.dfs

$(BUILD_DIR)/tankshootout64.elf: $(OBJS)
$(BUILD_DIR)/tankshootout64.emu-compat.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
