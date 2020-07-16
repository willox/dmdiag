#include "internal_types.h"
#include "State.h"

namespace dm
{

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


Value* Mob::GetField(Ref<String> name)
{
	uint32_t count = variables.count;
	Variable* vars = variables.elements.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return &x.value;
		}
	}

	Value* initial = GetInitialField(name);
	if (initial != nullptr)
	{
		return initial;
	}

	Value* global = GetGlobalField(name);
	if (global != nullptr)
	{
		return global;
	}

	return nullptr;
}

Value* Mob::GetInitialField(Ref<String> name)
{
	MobType* pMobType = mob_type.get();
	if (pMobType == nullptr)
		return nullptr;

	ObjPath* pObjPath = pMobType->obj_path.get();
	if (pObjPath == nullptr)
		return nullptr;

	Misc* pInitialVars = pObjPath->initial_vars.get();
	if (pInitialVars == nullptr)
		return nullptr;

	uint32_t count = pInitialVars->initial_variable_table.count();
	InitialVariable* vars = pInitialVars->initial_variable_table.variables.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return &x.value;
		}
	}

	return nullptr;
}

Value* Mob::GetGlobalField(Ref<String> name)
{
	MobType* pMobType = mob_type.get();
	if (pMobType == nullptr)
		return nullptr;

	ObjPath* pObjPath = pMobType->obj_path.get();
	if (pObjPath == nullptr)
		return nullptr;

	Misc* pVarDeclarations = pObjPath->var_declarations.get();
	if (pVarDeclarations == nullptr)
		return nullptr;

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
			return x.value.get();
		}
	}

	return nullptr;
}

}

