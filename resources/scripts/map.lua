-- Lua script run when a map is generated.
-- Eventually this script will handle ALL map generation; right now it just
-- populates an already-created map with some test items.
--
-- Globals used:
--		current_map_id - ID of the map currently being generated

print("Current map ID is " .. current_map_id)

if current_map_id ~= 0 then
	print("TODO: Populate map with stuff.")
end