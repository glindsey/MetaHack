-- Default Lua script read when ThingManager is first initialized.

function trace(string)
	print(debug.traceback())

	--local n = 0
	--local debuginfo = debug.getinfo(n)
	--while debuginfo ~= nil do
	--  print("Stack Level " .. n)
	--  print("  source = " .. debuginfo.source)
	--  print("  short_src = " .. debuginfo.short_src)
	--  print("  linedefined = " .. debuginfo.linedefined)
	--  print("  what = " .. debuginfo.what)
	--  print("  name = " .. (debuginfo.name or "<none>"))
	--  print("  namewhat = " .. debuginfo.namewhat)
	--  print("  nups = " .. debuginfo.nups)
	--  n = n + 1
	--  debuginfo = debug.getinfo(n)
	--end
end

-- inheritsFrom courtesy of lua-users.org wiki
function inheritsFrom(baseClass, className)
	local new_class = {}
	new_class.type = className

	-- @todo Register class name with the ThingManager
	--thingManager_register(className)
	
	-- new_class:create() is disabled because we don't actually create
	-- instances of these classes; instead they operate on C++ Thing instances.
	
	--local class_mt = { __index = new_class }
	--function new_class:create()
	--	local newinst = {}
	--	setmetatable(newinst, class_mt)
	--	return newinst
	--end
	
	if nil ~= baseClass then
		setmetatable(new_class, { __index = baseClass })
	end
	
	function new_class:class()
		return new_class
	end
	
	function new_class:superClass()
		return baseClass
	end
	
	function new_class:isa(theClass)
		local b_isa = false
		local cur_class = new_class
		while (nil ~= cur_class) and (false == b_isa) do
			if cur_class == theClass then
				b_isa = true
			else
				cur_class = cur_class:superClass()
			end
		end
		return b_isa
	end
	
	return new_class
end

print("*** Default Lua script executed.")