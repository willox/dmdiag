#pragma once

#include "internal_types.h"
#include "Value.h"

namespace dm
{
// TODO: remove
void SetupMobFieldGetters();

struct MobTable
{
	VPtr<VPtr<Mob>> elements;
	uint32_t length;
};
static_assert(sizeof(MobTable) == 8);

struct MobTypeTable
{
	VPtr<MobType> elements;
	uint32_t count;
};
static_assert(sizeof(MobTypeTable) == 8);

struct MobType
{
	Ref<ObjPath> obj_path;
	char unk_0[0xC];
};
static_assert(sizeof(MobType) == 16);

struct MobFields
{
	enum class Flags : uint32_t
	{
		Opacity = 0x01,
		Density = 0x02,
		Visibility = 0x04, // What is atom.visibility?
		GenderFlags = 0xC0, // A 2-bit index to `Gender` enum
		MouseDropZone = 0x100,
		MouseOpacity = 0x3000,
		AnimateMovement = 0xC000,
		Override = 0x40000,
	};

	char unk_0[0x1c];
	Ref<String> name;
	Ref<String> desc;
	Ref<String> suffix;
	Ref<String> screen_loc;
	Ref<String> text;
	uint32_t icon;
	Ref<String> icon_state;
	uint32_t unk_2;
	uint32_t unk_3;
	uint32_t unk_4;
	uint32_t unk_5;
	Flags flags; // todo
	uint8_t unk_7;
	uint8_t invisibility;
	uint8_t infra_luminosity;
	uint8_t luminosity;
	uint32_t unk_8;
	uint32_t unk_9;
	uint32_t unk_10;
	float layer;
	Ref<String> maptext;
	uint16_t maptext_x;
	uint16_t maptext_y;
	uint16_t maptext_width; // (special behaviour)
	uint16_t maptext_height; // (special behaviour)
	Value mouse_over_pointer;
	Value mouse_drag_pointer;
	Value mouse_drop_pointer;
	char unk_11[0x34];
	Ref<String> render_source;
	Ref<String> render_target;
	uint16_t vis_flags;

	// TODO: transform is in here somewhere
	// TODO: size unknown
};

struct Mob
{
	enum class Flags : uint16_t
	{
		SeeInfrared,
	};

	Value loc;
	char unk_0_0[0x8];
	uint16_t bound_x;
	uint16_t bound_y;
	uint16_t bound_width;
	uint16_t bound_height;
	float step_x;
	float step_y;
	float step_size;
	VPtr<uint32_t> unk_1;
	VariableTable variables;
	uint8_t dir;
	uint8_t unk_2_0;
	uint16_t pixel_x;
	uint16_t pixel_y;
	uint16_t pixel_w;
	uint16_t pixel_z;
	VPtr<uint32_t> unk_3;
	VPtr<uint32_t> unk_4;
	VPtr<uint32_t> unk_5;
	char unk_6[0x8];
	Ref<MobFields> fields;
	char unk_7[0x3C];
	Ref<String> key;
	Ref<String> ckey;
	Ref<ObjPath> path;
	Ref<MobType> mob_type;
	char unk_8[0x8];
	VPtr<uint32_t> unk_9;
	char unk_10[0x4];
	uint16_t client_index;
	uint16_t unk_11;
	uint16_t sight;
	Flags flags;
	uint8_t see_in_dark;
	uint8_t see_invisible;
	uint8_t unk_14;
	uint8_t unk_15;

	static Value getType(Ref<Mob> ref);
	static Value getVerbs(Ref<Mob> ref);
	static Value getLoc(Ref<Mob> ref);
	static Value getX(Ref<Mob> ref);
	static Value getY(Ref<Mob> ref);
	static Value getZ(Ref<Mob> ref);
	static Value getDir(Ref<Mob> ref);
	static Value getSight(Ref<Mob> ref);
	static Value getClient(Ref<Mob> ref);
	static Value getKey(Ref<Mob> ref);
	static Value getCKey(Ref<Mob> ref);
	static Value getGroup(Ref<Mob> ref);
	static Value getContents(Ref<Mob> ref);
	static Value getTag(Ref<Mob> ref); 
	static Value getVars(Ref<Mob> ref);
	static Value getOverlays(Ref<Mob> ref);
	static Value getUnderlays(Ref<Mob> ref);
	static Value getParentType(Ref<Mob> ref);
	static Value getSeeInDark(Ref<Mob> ref);
	static Value getSeeInvisible(Ref<Mob> ref);
	static Value getSeeInfrared(Ref<Mob> ref);
	static Value getPixelX(Ref<Mob> ref);
	static Value getPixelY(Ref<Mob> ref);
	static Value getPixelW(Ref<Mob> ref);
	static Value getPixelStepSize(Ref<Mob> ref);
	static Value getPixelZ(Ref<Mob> ref);
	static Value getLocs(Ref<Mob> ref);
	static Value getStepX(Ref<Mob> ref);
	static Value getStepY(Ref<Mob> ref);
	static Value getStepSize(Ref<Mob> ref);
	static Value getBoundX(Ref<Mob> ref);
	static Value getBoundY(Ref<Mob> ref);
	static Value getBoundWidth(Ref<Mob> ref);
	static Value getBoundHeight(Ref<Mob> ref);
	static Value getGlideSize(Ref<Mob> ref);
	static Value getVisContents(Ref<Mob> ref);
	static Value getVisLocs(Ref<Mob> ref);
	static Value getFilters(Ref<Mob> ref);
	static Value getTransform(Ref<Mob> ref);
	static Value getAlpha(Ref<Mob> ref);
	static Value getColor(Ref<Mob> ref);
	static Value getBlendMode(Ref<Mob> ref);
	static Value getAppearance(Ref<Mob> ref);
	static Value getPlane(Ref<Mob> ref);
	static Value getAppearanceFlags(Ref<Mob> ref);
	static Value getName(Ref<Mob> ref);
	static Value getDesc(Ref<Mob> ref);
	static Value getSuffix(Ref<Mob> ref);
	static Value getScreenLoc(Ref<Mob> ref);
	static Value getText(Ref<Mob> ref);
	static Value getIcon(Ref<Mob> ref);
	static Value getIconState(Ref<Mob> ref);
	static Value getDensity(Ref<Mob> ref);
	static Value getOpacity(Ref<Mob> ref);
	static Value getGender(Ref<Mob> ref);
	static Value getMouseDropZone(Ref<Mob> ref);
	static Value getAnimateMovement(Ref<Mob> ref);
	static Value getMouseOpacity(Ref<Mob> ref);
	static Value getOverride(Ref<Mob> ref);
	static Value getInvisibility(Ref<Mob> ref);
	static Value getInfraLuminosity(Ref<Mob> ref);
	static Value getLuminosity(Ref<Mob> ref);
	static Value getLayer(Ref<Mob> ref);
	static Value getMapText(Ref<Mob> ref);
	static Value getMapTextX(Ref<Mob> ref);
	static Value getMapTextY(Ref<Mob> ref);
	static Value getMapTextWidth(Ref<Mob> ref);
	static Value getMapTextHeight(Ref<Mob> ref);
	static Value getMouseOverPointer(Ref<Mob> ref);
	static Value getMouseDragPointer(Ref<Mob> ref);
	static Value getMouseDropPointer(Ref<Mob> ref);
	static Value getRenderSource(Ref<Mob> ref);
	static Value getRenderTarget(Ref<Mob> ref);
	static Value getVisFlags(Ref<Mob> ref);

	static std::optional<Value> GetField(Ref<Mob> ref, Ref<String> name);
	static std::optional<Value> GetInitialField(Ref<Mob> ref, Ref<String> name);
	static std::optional<Value> GetGlobalField(Ref<Mob> ref, Ref<String> name);
};
static_assert(sizeof(Mob) == 188);

}