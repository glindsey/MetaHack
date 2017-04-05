-- Definition of special functions for the Marcher object type.

function Marcher_process(id)
    print("DEBUG: Marcher_process() called for object " .. id .. ".")
    return true, LuaType.Boolean
end