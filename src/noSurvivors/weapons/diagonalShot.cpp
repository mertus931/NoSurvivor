#include "../Entities/weapon.h"
#include "noSurvivors/noSurvivors.h"

void DiagonalShot::Attack(MapState * state)
{
	FPoint topLeftOfChar {(state->pc.position.x), (state->pc.position.y)};
	FPoint topRightOfChar {(state->pc.position.x + state->pc.width), (state->pc.position.y)};
	FPoint bottomLeftOfChar {(state->pc.position.x), (state->pc.position.y) + state->pc.height};
	FPoint bottomRightOfChar {(state->pc.position.x) + state->pc.width, (state->pc.position.y) + state->pc.height};

	Projectile projectile1 {topLeftOfChar, FPoint{-1, -1}, damage};
	Projectile projectile2 {topRightOfChar, FPoint{1, -1}, damage};
	Projectile projectile3 {bottomLeftOfChar, FPoint{-1, 1}, damage};
	Projectile projectile4 {bottomRightOfChar, FPoint{1, 1}, damage};
	state->projectiles.push_back(projectile1);
	state->projectiles.push_back(projectile2);
	state->projectiles.push_back(projectile3);
	state->projectiles.push_back(projectile4);
}

bool DiagonalShot::IsOnCooldown()
{
	if (currentCoolDown<coolDown) {
		currentCoolDown++;
		return true;
	}
	currentCoolDown = 0;
	return false;
}
