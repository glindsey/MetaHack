-- Lua script run when a map is generated.
-- Eventually this script will handle ALL map generation; right now it just
-- populates an already-created map with some test items.
--
-- Globals used:
--		current_map_id - ID of the map currently being generated

print("Current map ID is " .. current_map_id)

if current_map_id ~= 0 then
    local start_x, start_y = map_get_start_coords(current_map_id)
    local start_contents_id = map_get_tile_contents(current_map_id, start_x, start_y)
    local player_id = get_player()

    -- TESTING CODE: Create a lighting orb held in player inventory.
    print("Creating lighting orb...")
    local orb_id = entity_create(player_id, "LightOrb")

    -- TESTING CODE: Create a sconce immediately north of the player.
    print("Creating sconce...");
    local sconce_id = entity_create(map_get_tile_contents(current_map_id, start_x, start_y - 1), "Sconce")

    -- TESTING CODE: Create a rock immediately south of the player.
    print("Creating rock...");
    local rock_id = entity_create(map_get_tile_contents(current_map_id, start_x, start_y + 1), "Rock")

    -- TESTING CODE: Create a sack immediately east of the player.
    print("Creating sack...");
    local sack_id = entity_create(map_get_tile_contents(current_map_id, start_x + 1, start_y), "SackCloth")

    -- TESTING CODE: Create five gold coins west of the player.
    print("Creating 5 coins...");
    local coins_id = entity_create(map_get_tile_contents(current_map_id, start_x - 1, start_y), "CoinGold", 5)

    -- TESTING CODE: Create ten gold coins northwest of the player.
    print("Creating 10 coins...");
    local coins2_id = entity_create(map_get_tile_contents(current_map_id, start_x - 1, start_y - 1), "CoinGold", 10)

    -- TESTING CODE: Create a rock lichen northeast of the player.
    --print("Creating rock lichen...");
    --local lichen_id = entity_create(map_get_tile_contents(current_map_id, start_x + 1, start_y - 1), "RockLichen")

    -- TESTING CODE: Create a test marcher northeast of the player.
    print("Creating marcher robot...");
    local marcher_id = entity_create(map_get_tile_contents(current_map_id, start_x + 1, start_y - 1), "Marcher")

    -- TESTING CODE: Create a dagger southeast of the player.
    print("Creating dagger...");
    local dagger_id = entity_create(map_get_tile_contents(current_map_id, start_x + 1, start_y + 1), "Dagger")

end