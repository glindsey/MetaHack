-- Default Lua script read when ThingManager is first initialized.

require "resources/deepcopy"

-- Seed the RNG.
-- (TODO: Probably should improve upon this in the future.)
math.randomseed(os.time())

-- Function to compare two tables. Used to check property tables
-- to see if objects can be combined.
-- Taken from https://stackoverflow.com/questions/20325332/how-to-check-if-two-tablesobjects-have-the-same-value-in-lua
function equals(o1, o2, ignore_mt)
    if o1 == o2 then return true end
    local o1Type = type(o1)
    local o2Type = type(o2)
    if o1Type ~= o2Type then return false end
    if o1Type ~= 'table' then return false end

    if not ignore_mt then
        local mt1 = getmetatable(o1)
        if mt1 and mt1.__eq then
            --compare using built in method
            return o1 == o2
        end
    end

    local keySet = {}

    for key1, value1 in pairs(o1) do
        local value2 = o2[key1]
        if value2 == nil or equals(value1, value2, ignore_mt) == false then
            return false
        end
        keySet[key1] = true
    end

    for key2, _ in pairs(o2) do
        if not keySet[key2] then return false end
    end
    return true
end

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

-- Initial inheritsFrom code courtesy of lua-users.org wiki
function inheritsFrom(baseClass, className)
    
    local new_class = {}

    -- new_class:create() is disabled because we don't actually create
    -- instances of these classes; instead they operate on C++ Thing instances.
        
    --local class_mt = { __index = new_class }
    --function new_class:create()
    --  local newinst = {}
    --  setmetatable(newinst, class_mt)
    --  return newinst
    --end

    -- Add the default intrinsics and properties tables if they don't exist.
    new_class.intrinsics = new_class.intrinsics or {}
    new_class.defaults = new_class.defaults or {}
    
    if nil ~= baseClass then	
        -- Do a deep copy of the base class, for starters.
        new_class = table.deepcopy(baseClass)
    
        -- Inherit from the base class.
        setmetatable(new_class, { __index = baseClass })

        -- Add the other class as this class' parent.
        new_class.intrinsics.parent = baseClass.type
        
        -- Add this class to the base class' list of children.
        baseClass.children = baseClass.children or {}
        baseClass.children[className] = new_class;
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

    
    -- @todo Register class name with the ThingManager
    --thingManager_register(className)
    new_class.intrinsics.type = className  
    return new_class
end

function range(minValue, maxValue)
    local r = {}
    r.type = "range"
    r.min = minValue
    r.max = maxValue
    return r
end

function vector2(xValue, yValue)
    local v = {}
    v.type = "vector2"
    v.x = xValue
    v.y = yValue
    return v
end

-- === USEFUL GLOBALS =========================================================
Mu = 0


print("*** Default Lua script executed.")