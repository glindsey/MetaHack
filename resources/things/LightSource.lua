-- Definition of special functions for the LightSource object type.

LightSource = LightSource or {}

function LightSource.on_create(id)
	messageLog_add("LightSource.on_create() called, new ID = " .. id)
	return ActionResult.Success
end

function LightSource.is_usable_by(id)
	-- This SHOULD check if a creature is sentient.
	-- For now just return true.
	return true
end

function LightSource.perform_action_used_by(id)
	local name = thing_get_display_name(id)
	local is_lit = thing_get_property_flag(id, "light_lit")
	is_lit = ~is_lit
	if is_lit then
		messageLog_add("You light the " .. name .. ".")
	else
		messageLog_add("You extinguish the " .. name .. ".")
	end
	thing_set_property_flag(id, "light_lit", is_lit)
	return ActionResult.Success
end