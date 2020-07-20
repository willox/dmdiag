#include "Mob.h"
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

template<>
Mob* Ref<Mob>::get()
{
	return current_state->GetMob(index);
}

template<>
MobType* Ref<MobType>::get()
{
	return current_state->GetMobType(index);
};

template<>
MobFields* Ref<MobFields>::get()
{
	return current_state->GetMobFields(index);
}

Value Mob::getType(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{pMob->mob_type} : Value::Null;
}

Value Mob::getVerbs(Ref<Mob> ref)
{
	return {DataType::LIST_MOB_VERBS, ref.index};
}

Value Mob::getLoc(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{pMob->loc} : Value::Null;
}

Value Mob::getX(Ref<Mob> ref)
{
	Mob* pMob = ref.get();

	if (pMob == nullptr)
		return Value::Null;

	if (!pMob || pMob->loc.type != DataType::TURF)
		return {0.f};

	// TODO: Annoying logic
	return Value::Null;
}

Value Mob::getY(Ref<Mob> ref)
{
	Mob* pMob = ref.get();

	if (pMob == nullptr)
		return Value::Null;

	if (!pMob || pMob->loc.type != DataType::TURF)
		return {0.f};

	// TODO: Annoying logic
	return Value::Null;
}

Value Mob::getZ(Ref<Mob> ref)
{
	Mob* pMob = ref.get();

	if (pMob == nullptr)
		return Value::Null;

	if (!pMob || pMob->loc.type != DataType::TURF)
		return {0.f};

	// TODO: Annoying logic
	return Value::Null;
}

Value Mob::getDir(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->dir)} : Value::Null;
}

Value Mob::getSight(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->sight)} : Value::Null;
}

Value Mob::getClient(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{DataType::CLIENT, pMob->client_index} : Value::Null;
}

Value Mob::getKey(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{pMob->key} : Value::Null;
}

Value Mob::getCKey(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{pMob->ckey} : Value::Null;
}

Value Mob::getGroup(Ref<Mob> ref)
{
	// TODO: Check
	return {DataType::LIST_GROUP, ref.index};
}

Value Mob::getContents(Ref<Mob> ref)
{
	return {DataType::LIST_MOB_CONTENTS, ref.index};
}

// TODO: Just really annoying
Value Mob::getTag(Ref<Mob> ref)
{
	return Value::Null;
}

Value Mob::getVars(Ref<Mob> ref)
{	
	return {DataType::LIST_MOB_VARS, ref.index};
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getOverlays(Ref<Mob> ref)
{
	return {DataType::LIST_MOB_OVERLAYS, ref.index};
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getUnderlays(Ref<Mob> ref)
{
	return {DataType::LIST_MOB_UNDERLAYS, ref.index};
}

Value Mob::getParentType(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	if (pMob == nullptr)
		return Value::Null;

	ObjPath* current = pMob->path.get();
	if (current == nullptr)
		return Value::Null;

	auto type = current_state->FindMobType(current->parent);
	return {type};
}

Value Mob::getSeeInDark(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->see_in_dark)} : Value::Null;
}

Value Mob::getSeeInvisible(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->see_invisible)} : Value::Null;
}

Value Mob::getSeeInfrared(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	if (pMob == nullptr)
		return Value::Null;

	return {((uint16_t)pMob->flags & (uint16_t)Flags::SeeInfrared) ? 1.f : 0.f};
}

Value Mob::getPixelX(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->pixel_x)} : Value::Null;
}

Value Mob::getPixelY(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->pixel_y)} : Value::Null;
}

Value Mob::getPixelW(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->pixel_w)} : Value::Null;
}

// TODO: harder
Value Mob::getPixelStepSize(Ref<Mob> ref)
{
	return Value::Null;
}

Value Mob::getPixelZ(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->pixel_z)} : Value::Null;
}

// TODO: annoying
Value Mob::getLocs(Ref<Mob> ref)
{
	return Value::Null;
}

Value Mob::getStepX(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->step_x)} : Value::Null;
}

Value Mob::getStepY(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->step_y)} : Value::Null;
}

Value Mob::getStepSize(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->step_size)} : Value::Null;
}

Value Mob::getBoundX(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->bound_x)} : Value::Null;
}

Value Mob::getBoundY(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->bound_y)} : Value::Null;
}

Value Mob::getBoundWidth(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->bound_width)} : Value::Null;
}

Value Mob::getBoundHeight(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	return pMob ? Value{static_cast<float>(pMob->bound_height)} : Value::Null;
}

// TODO: harder
Value Mob::getGlideSize(Ref<Mob> ref)
{
	return Value::Null;
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getVisContents(Ref<Mob> ref)
{
	return {DataType::LIST_MOB_VIS_CONTENTS, ref.index};
}

// TODO: Needs the Mob index (move out of Mob class?)
Value Mob::getVisLocs(Ref<Mob> ref)
{
	return {DataType::LIST_MOB_VIS_LOCS, ref.index};
}

// TODO: All of these 
Value Mob::getFilters(Ref<Mob> ref) { return Value::Null; }
Value Mob::getTransform(Ref<Mob> ref) { return Value::Null; }
Value Mob::getAlpha(Ref<Mob> ref) { return Value::Null; }
Value Mob::getColor(Ref<Mob> ref) { return Value::Null; }
Value Mob::getBlendMode(Ref<Mob> ref) { return Value::Null; }
Value Mob::getAppearance(Ref<Mob> ref) { return Value::Null; }
Value Mob::getPlane(Ref<Mob> ref) { return Value::Null; }
Value Mob::getAppearanceFlags(Ref<Mob> ref) { return Value::Null; }

Value Mob::getName(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->name} : Value::Null;
}

Value Mob::getDesc(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->desc} : Value::Null;
}

Value Mob::getSuffix(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->suffix} : Value::Null;
}

Value Mob::getScreenLoc(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->screen_loc} : Value::Null;
}

Value Mob::getText(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->text} : Value::Null;
}

// TODO
Value Mob::getIcon(Ref<Mob> ref)
{
	return Value::Null;
}

Value Mob::getIconState(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->icon_state} : Value::Null;
}

// TODO
Value Mob::getDensity(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getOpacity(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getGender(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getMouseDropZone(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getAnimateMovement(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getMouseOpacity(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getOverride(Ref<Mob> ref)
{
	return {0.f};
}

Value Mob::getInvisibility(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{static_cast<float>(f->invisibility)} : Value::Null;
}

Value Mob::getInfraLuminosity(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{static_cast<float>(f->infra_luminosity)} : Value::Null;
}

Value Mob::getLuminosity(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{static_cast<float>(f->luminosity)} : Value::Null;
}

Value Mob::getLayer(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->layer} : Value::Null;
}

Value Mob::getMapText(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->maptext} : Value::Null;
}

Value Mob::getMapTextX(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{static_cast<float>(f->maptext_x)} : Value::Null;
}

Value Mob::getMapTextY(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{static_cast<float>(f->maptext_y)} : Value::Null;
}

// TODO
Value Mob::getMapTextWidth(Ref<Mob> ref)
{
	return {0.f};
}

// TODO
Value Mob::getMapTextHeight(Ref<Mob> ref)
{
	return {0.f};
}

Value Mob::getMouseOverPointer(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? f->mouse_over_pointer : Value::Null;
}

Value Mob::getMouseDragPointer(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? f->mouse_drag_pointer : Value::Null;
}

Value Mob::getMouseDropPointer(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? f->mouse_drop_pointer : Value::Null;
}

Value Mob::getRenderSource(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->render_source}: Value::Null;
}

Value Mob::getRenderTarget(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{f->render_target}: Value::Null;
}

Value Mob::getVisFlags(Ref<Mob> ref)
{
	Mob* pMob = ref.get();
	MobFields* f = pMob ? pMob->fields.get() : nullptr;
	return f ? Value{static_cast<float>(f->vis_flags)}: Value::Null;
}


std::optional<Value> Mob::GetField(Ref<Mob> ref, Ref<String> name)
{
	// Check built-in fields first
	auto getter = getters.find(name.index);
	if (getter != getters.end())
	{
		return (*getter->second)(ref);
	}

	Mob* pMob = ref.get();
	if (pMob == nullptr)
		return std::nullopt;

	uint32_t count = pMob->variables.count;
	Variable* vars = pMob->variables.elements.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return x.value;
		}
	}

	auto initial = GetInitialField(ref, name);
	if (initial)
	{
		return initial;
	}

	auto global = GetGlobalField(ref, name);
	if (global)
	{
		return global;
	}

	return std::nullopt;
}

std::optional<Value> Mob::GetInitialField(Ref<Mob> ref, Ref<String> name)
{
	Mob* pMob = ref.get();
	if (pMob == nullptr)
		return std::nullopt;

	MobType* pMobType = pMob->mob_type.get();
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

std::optional<Value> Mob::GetGlobalField(Ref<Mob> ref, Ref<String> name)
{
	Mob* pMob = ref.get();
	if (pMob == nullptr)
		return std::nullopt;

	MobType* pMobType = pMob->mob_type.get();
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

