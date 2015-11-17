-- Lua script run when a map is generated.
-- Eventually this script will handle ALL map generation; right now it just
-- populates an already-created map with some test items.
--
-- Globals used:
--		current_map_id - ID of the map currently being generated

print("Current map ID is " .. current_map_id)

if current_map_id ~= 0 then
    local start_x, start_y = map_get_start_coords(current_map_id)
    local start_floor_id = map_get_floor(current_map_id, start_x, start_y)
    local player_id = thing_get_player()

    print("Trying to move player to starting position...")
    local move_success = thing_move_into(player_id, start_floor_id)
    print("Success = " .. tostring(move_success))

    -- TESTING CODE: Create a lighting orb held in player inventory.
    print("Creating lighting orb...")
    local orb_id = thing_create(player_id, "LightOrb")

    -- TESTING CODE: Create a sconce immediately north of the player.
    print("Creating sconce...");
    local sconce_id = thing_create(map_get_floor(current_map_id, start_x, start_y - 1), "Sconce")

    -- TESTING CODE: Create a rock immediately south of the player.
    print("Creating rock...");
    local rock_id = thing_create(map_get_floor(current_map_id, start_x, start_y + 1), "Rock")

    -- TESTING CODE: Create a sack immediately east of the player.
    print("Creating sack...");
    local sack_id = thing_create(map_get_floor(current_map_id, start_x + 1, start_y), "SackCloth")

    -- TESTING CODE: Create a rock lichen northeast of the player.
    print("Creating rock lichen...");
    local lichen_id = thing_create(map_get_floor(current_map_id, start_x + 1, start_y - 1), "RockLichen")

    -- TESTING CODE: Create a dagger southeast of the player.
    print("Creating dagger...");
    local dagger_id = thing_create(map_get_floor(current_map_id, start_x + 1, start_y + 1), "Dagger")

end