-- Lua script run when a map is generated.
-- Eventually this script will handle ALL map generation; right now it just
-- populates an already-created map with some test items.
--
-- Globals used:
--		current_map_id - ID of the map currently being generated

print("Current map ID is " .. current_map_id)

if current_map_id ~= 0 then
    local start_x, start_y = map_get_start_coords(current_map_id)
    print("Map is ID " .. tostring(current_map_id) .. " and has start coords (" .. tostring(start_x) .. ", " .. tostring(start_y) .. ")")

    local start_floor_id = map_get_floor(current_map_id, start_x, start_y)
    print("Start floor is ID " .. tostring(start_floor_id))

    local player_id = thing_get_player()
    local player_x, player_y = thing_get_coords(player_id)
    print("Player is ID " .. tostring(player_id) .. " and is located at (" .. tostring(player_x) .. ", " .. tostring(player_y) .. ")")

    local player_type = thing_get_type(player_id)
    print("Player type is " .. tostring(player_type) .. " which corresponds to global " .. tostring(_G[player_type]))

    print("Trying to move player to starting position...")
    local move_success = thing_move_into(player_id, start_floor_id)

    print("Result = " .. tostring(move_success))

	print("TODO: Populate map with stuff.")
end