class UnknownType
{
	constructor(_type)
	{
		type = _type
	}

	function _tostring()
	{
		return "UnknownType(" + type + ")"
	}

	type = null
}

class Mob
{
	constructor(_index)
	{
		index = _index
	}

	function _get(key)
	{
		return ::GetField_Mob(index, key)
	}

	function _tostring()
	{
		return "Mob(" + index + ")"
	}

	index = null
}
