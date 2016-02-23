#include "Direction.h"

// === STATIC INSTANCES =======================================================
Direction const Direction::None;
Direction const Direction::Self{ 0, 0 };
Direction const Direction::North{ 0, -1 };
Direction const Direction::Northeast{ 1, -1 };
Direction const Direction::East{ 1, 0 };
Direction const Direction::Southeast{ 1, 1 };
Direction const Direction::South{ 0, 1 };
Direction const Direction::Southwest{ -1, 1 };
Direction const Direction::West{ -1, 0 };
Direction const Direction::Northwest{ -1, -1 };
Direction const Direction::Up{ 0, 0, -1 };
Direction const Direction::Down{ 0, 0, 1 };