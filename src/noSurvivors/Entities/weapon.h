#pragma once

class Weapon;
class DiagonalShot;

class MapState;

class Weapon
{
public:
	virtual void Attack(MapState * state) = 0;
	virtual bool IsOnCooldown() = 0;
	int damage = 0;
	int cooldown = 0;
};

class DiagonalShot final : public Weapon
{
public:
	void Attack(MapState * state) override;
	bool IsOnCooldown() override;
	int damage = 5;
	int currentCoolDown = 0;
	int coolDown = 50;
};