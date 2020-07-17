#include "internal_types.h"
#include "State.h"
#include <map>
#include <utility>

namespace dm
{

// TODO: This global has to GOOO
using GetterFn = decltype(&Mob::getType);
std::map<uint32_t, GetterFn> getters;

void SetupMobFieldGetters()
{
	std::pair<const char*, GetterFn> vars[] = {
		{ "type", &Mob::getType },
		{ "verbs", &Mob::getVerbs },
		{ "loc", &Mob::getLoc },
		{ "x", &Mob::getX },
		{ "y", &Mob::getY },
		{ "z", &Mob::getZ },
		{ "dir", &Mob::getDir },
		{ "sight", &Mob::getSight },
		{ "client", &Mob::getClient },
		{ "key", &Mob::getKey },
		{ "ckey", &Mob::getCKey },
		{ "group", &Mob::getGroup },
		{ "contents", &Mob::getContents },
		{ "tag", &Mob::getTag },
		{ "vars", &Mob::getVars },
		{ "overlays", &Mob::getOverlays },
		{ "underlays", &Mob::getUnderlays },
		{ "parent_type", &Mob::getParentType },
		{ "see_in_dark", &Mob::getSeeInDark },
		{ "see_invisible", &Mob::getSeeInvisible },
		{ "see_infrared", &Mob::getSeeInfrared },
		{ "pixel_x", &Mob::getPixelX },
		{ "pixel_y", &Mob::getPixelY },
		{ "pixel_w", &Mob::getPixelW },
		{ "pixel_step_size", &Mob::getPixelStepSize },
		{ "pixel_z", &Mob::getPixelZ },
		{ "locs", &Mob::getLocs },
		{ "step_x", &Mob::getStepX },
		{ "step_y", &Mob::getStepY },
		{ "step_size", &Mob::getStepSize },
		{ "bound_x", &Mob::getBoundX },
		{ "bound_y", &Mob::getBoundY },
		{ "bound_width", &Mob::getBoundWidth },
		{ "bound_height", &Mob::getBoundHeight },
		{ "glide_size", &Mob::getGlideSize },
		{ "vis_contents", &Mob::getVisContents },
		{ "vis_locs", &Mob::getVisLocs },
		{ "filters", &Mob::getFilters },
		{ "transform", &Mob::getTransform },
		{ "alpha", &Mob::getAlpha },
		{ "color", &Mob::getColor },
		{ "blend_mode", &Mob::getBlendMode },
		{ "appearance", &Mob::getAppearance },
		{ "plane", &Mob::getPlane },
		{ "appearance_flags", &Mob::getAppearanceFlags },
		{ "name", &Mob::getName },
		{ "desc", &Mob::getDesc },
		{ "suffix", &Mob::getSuffix },
		{ "screen_loc", &Mob::getScreenLoc },
		{ "text", &Mob::getText },
		{ "icon", &Mob::getIcon },
		{ "icon_state", &Mob::getIconState },
		{ "density", &Mob::getDensity },
		{ "opacity", &Mob::getOpacity },
		{ "gender", &Mob::getGender },
		{ "mouse_drop_zone", &Mob::getMouseDropZone },
		{ "animate_movement", &Mob::getAnimateMovement },
		{ "mouse_opacity", &Mob::getMouseOpacity },
		{ "override", &Mob::getOverride },
		{ "invisibility", &Mob::getInvisibility },
		{ "infra_luminosity", &Mob::getInfraLuminosity },
		{ "luminosity", &Mob::getLuminosity },
		{ "layer", &Mob::getLayer },
		{ "maptext", &Mob::getMapText },
		{ "maptext_x", &Mob::getMapTextX },
		{ "maptext_y", &Mob::getMapTextY },
		{ "maptext_width", &Mob::getMapTextWidth },
		{ "maptext_height", &Mob::getMapTextHeight },
		{ "mouse_over_pointer", &Mob::getMouseOverPointer },
		{ "mouse_drag_pointer", &Mob::getMouseDragPointer },
		{ "mouse_drop_pointer", &Mob::getMouseDropPointer },
		{ "render_source", &Mob::getRenderSource },
		{ "render_target", &Mob::getRenderTarget },
		{ "vis_flags", &Mob::getVisFlags },
	};

	for (auto& pair : vars)
	{
		// TODO: string might not exist :(
		getters[current_state->GetStringRef(pair.first)->index] = pair.second;
	}
}

Value Mob::getType()
{
	return Value{mob_type};
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getVerbs()
{
	return Value::Null;
}

Value Mob::getLoc()
{
	return loc;
}

Value Mob::getX()
{
	if (loc.type != DataType::TURF)
		return {0.f};

	// TODO: Annoying logic
	return Value::Null;
}

Value Mob::getY()
{
	if (loc.type != DataType::TURF)
		return {0.f};

	// TODO: Annoying logic
	return Value::Null;
}

Value Mob::getZ()
{
	if (loc.type != DataType::TURF)
		return {0.f};

	// TODO: Annoying logic
	return Value::Null;
}

Value Mob::getDir()
{
	return {static_cast<float>(dir)};
}

Value Mob::getSight()
{
	return {static_cast<float>(sight)};
}

Value Mob::getClient()
{
	return {DataType::CLIENT, client_index};
}

Value Mob::getKey()
{
	return {key};
}

Value Mob::getCKey()
{
	return {ckey};
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getGroup()
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getContents()
{
	return Value::Null;
}

// TODO: Just really annoying
Value Mob::getTag()
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getVars()
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getOverlays()
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getUnderlays()
{
	return Value::Null;
}

Value Mob::getParentType()
{
	ObjPath* current = path.get();
	if (current == nullptr)
		return Value::Null;

	auto type = current_state->FindMobType(current->parent);
	return {type};
}

Value Mob::getSeeInDark()
{
	return {static_cast<float>(see_in_dark)};
}

Value Mob::getSeeInvisible()
{
	return {static_cast<float>(see_invisible)};
}

Value Mob::getSeeInfrared()
{
	return {((uint16_t)flags & (uint16_t)Flags::SeeInfrared) ? 1.f : 0.f};
}

Value Mob::getPixelX()
{
	return {static_cast<float>(pixel_x)};
}

Value Mob::getPixelY()
{
	return {static_cast<float>(pixel_z)};
}

Value Mob::getPixelW()
{
	return {static_cast<float>(pixel_w)};
}

// TODO: harder
Value Mob::getPixelStepSize()
{
	return Value::Null;
}

Value Mob::getPixelZ()
{
	return {static_cast<float>(pixel_z)};
}

// TODO: annoying
Value Mob::getLocs()
{
	return Value::Null;
}

Value Mob::getStepX()
{
	return {static_cast<float>(step_x)};
}

Value Mob::getStepY()
{
	return {static_cast<float>(step_x)};
}

Value Mob::getStepSize()
{
	return {static_cast<float>(step_size)};
}

Value Mob::getBoundX()
{
	return {static_cast<float>(bound_x)};
}

Value Mob::getBoundY()
{
	return {static_cast<float>(bound_y)};
}

Value Mob::getBoundWidth()
{
	return {static_cast<float>(bound_width)};
}

Value Mob::getBoundHeight()
{
	return {static_cast<float>(bound_height)};
}

// TODO: harder
Value Mob::getGlideSize()
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getVisContents()
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getVisLocs()
{
	return Value::Null;
}

// TODO: All of these 
Value Mob::getFilters() { return Value::Null; }
Value Mob::getTransform() { return Value::Null; }
Value Mob::getAlpha() { return Value::Null; }
Value Mob::getColor() { return Value::Null; }
Value Mob::getBlendMode() { return Value::Null; }
Value Mob::getAppearance() { return Value::Null; }
Value Mob::getPlane() { return Value::Null; }
Value Mob::getAppearanceFlags() { return Value::Null; }

Value Mob::getName()
{
	MobFields* f = fields.get();
	return f ? Value{f->name} : Value::Null;
}

Value Mob::getDesc()
{
	MobFields* f = fields.get();
	return f ? Value{f->desc} : Value::Null;
}

Value Mob::getSuffix()
{
	MobFields* f = fields.get();
	return f ? Value{f->suffix} : Value::Null;
}

Value Mob::getScreenLoc()
{
	MobFields* f = fields.get();
	return f ? Value{f->screen_loc} : Value::Null;
}

Value Mob::getText()
{
	MobFields* f = fields.get();
	return f ? Value{f->text} : Value::Null;
}

// TODO
Value Mob::getIcon()
{
	return Value::Null;
}

Value Mob::getIconState()
{
	MobFields* f = fields.get();
	return f ? Value{f->icon_state} : Value::Null;
}

// TODO
Value Mob::getDensity()
{
	return {0.f};
}

// TODO
Value Mob::getOpacity()
{
	return {0.f};
}

// TODO
Value Mob::getGender()
{
	return {0.f};
}

// TODO
Value Mob::getMouseDropZone()
{
	return {0.f};
}

// TODO
Value Mob::getAnimateMovement()
{
	return {0.f};
}

// TODO
Value Mob::getMouseOpacity()
{
	return {0.f};
}

// TODO
Value Mob::getOverride()
{
	return {0.f};
}

Value Mob::getInvisibility()
{
	MobFields* f = fields.get();
	return f ? Value{static_cast<float>(f->invisibility)} : Value::Null;
}

Value Mob::getInfraLuminosity()
{
	MobFields* f = fields.get();
	return f ? Value{static_cast<float>(f->infra_luminosity)} : Value::Null;
}

Value Mob::getLuminosity()
{
	MobFields* f = fields.get();
	return f ? Value{static_cast<float>(f->luminosity)} : Value::Null;
}

Value Mob::getLayer()
{
	MobFields* f = fields.get();
	return f ? Value{f->layer} : Value::Null;
}

Value Mob::getMapText()
{
	MobFields* f = fields.get();
	return f ? Value{f->maptext} : Value::Null;
}

Value Mob::getMapTextX()
{
	MobFields* f = fields.get();
	return f ? Value{static_cast<float>(f->maptext_x)} : Value::Null;
}

Value Mob::getMapTextY()
{
	MobFields* f = fields.get();
	return f ? Value{static_cast<float>(f->maptext_y)} : Value::Null;
}

// TODO
Value Mob::getMapTextWidth()
{
	return {0.f};
}

// TODO
Value Mob::getMapTextHeight()
{
	return {0.f};
}

Value Mob::getMouseOverPointer()
{
	MobFields* f = fields.get();
	return f ? f->mouse_over_pointer : Value::Null;
}

Value Mob::getMouseDragPointer()
{
	MobFields* f = fields.get();
	return f ? f->mouse_drag_pointer : Value::Null;
}

Value Mob::getMouseDropPointer()
{
	MobFields* f = fields.get();
	return f ? f->mouse_drop_pointer : Value::Null;
}

Value Mob::getRenderSource()
{
	MobFields* f = fields.get();
	return f ? Value{f->render_source} : Value::Null;
}

Value Mob::getRenderTarget()
{
	MobFields* f = fields.get();
	return f ? Value{f->render_target} : Value::Null;
}

Value Mob::getVisFlags()
{
	MobFields* f = fields.get();
	return f ? Value{static_cast<float>(f->vis_flags)} : Value::Null;
}


std::optional<Value> Mob::GetField(Ref<String> name)
{
	// Check built-in fields first
	auto getter = getters.find(name.index);
	if (getter != getters.end())
	{
		return (this->*getter->second)();
	}

	uint32_t count = variables.count;
	Variable* vars = variables.elements.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return x.value;
		}
	}

	auto initial = GetInitialField(name);
	if (initial)
	{
		return initial;
	}

	auto global = GetGlobalField(name);
	if (global)
	{
		return global;
	}

	return std::nullopt;
}

std::optional<Value> Mob::GetInitialField(Ref<String> name)
{
	MobType* pMobType = mob_type.get();
	if (pMobType == nullptr)
		return std::nullopt;

	ObjPath* pObjPath = pMobType->obj_path.get();
	if (pObjPath == nullptr)
		return std::nullopt;

	Misc* pInitialVars = pObjPath->initial_vars.get();
	if (pInitialVars == nullptr)
		return std::nullopt;

	uint32_t count = pInitialVars->initial_variable_table.count();
	InitialVariable* vars = pInitialVars->initial_variable_table.variables.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return x.value;
		}
	}

	return std::nullopt;
}

std::optional<Value> Mob::GetGlobalField(Ref<String> name)
{
	MobType* pMobType = mob_type.get();
	if (pMobType == nullptr)
		return std::nullopt;

	ObjPath* pObjPath = pMobType->obj_path.get();
	if (pObjPath == nullptr)
		return std::nullopt;

	Misc* pVarDeclarations = pObjPath->var_declarations.get();
	if (pVarDeclarations == nullptr)
		return std::nullopt;

	uint32_t count = pVarDeclarations->var_declaration_table.count();
	VarDeclaration* vars = pVarDeclarations->var_declaration_table.variables.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (!x.isGlobal())
			continue;

		// Dumb match against str pointers, is ok
		if (x.name.string() == name.string())
		{
			return *x.value.get();
		}
	}

	return std::nullopt;
}

}

