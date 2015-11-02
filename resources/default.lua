-- Default Lua script read when ThingManager is first initialized.
local temp = ActionResult.FailureCircularReference.value
print("Testing enum import: ActionResult.FailureCircularReference = " , temp)
print("*** Default Lua script executed.")

function trace(string)
	--print(debug.traceback())

	local n = 0
	local debuginfo = debug.getinfo(n)
	while debuginfo ~= nil do
	  print("Stack Level " .. n)
	  print("  source = " .. debuginfo.source)
	  print("  short_src = " .. debuginfo.short_src)
	  print("  linedefined = " .. debuginfo.linedefined)
	  print("  what = " .. debuginfo.what)
	  print("  name = " .. (debuginfo.name or "<none>"))
	  print("  namewhat = " .. debuginfo.namewhat)
	  print("  nups = " .. debuginfo.nups)
	  n = n + 1
	  debuginfo = debug.getinfo(n)
	end
end