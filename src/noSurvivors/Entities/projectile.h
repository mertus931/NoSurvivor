#pragma once

#include "global.h"
#include "movableCharacter.h"

class Projectile;

class Projectile : public Entity {
	FPoint position {};
	FPoint direction {};
	FPoint size = FPoint{7,7};
	int speed = 4;

public:
	int damage = 0;
	Projectile(FPoint position, FPoint direction, int damage);
	void Render(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT, Point cam, Renderer * renderer) const;
	void Move(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT);
	bool IsTooFar(Point cam, Point windowSize) const;
	bool IsColliding(Enemy * entity) const;

	const FPoint &getPosition();
	const FPoint &getSize();

	int GetType() override { return 2; }
};
