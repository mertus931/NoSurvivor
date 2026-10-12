#pragma once

#include "global.h"
#include "weapon.h"
#include "entity.h"
#include <memory>

class MovableCharacter;
class PlayerCharacter;

enum characterType {PC, BEE, WASP, HORNET};

class MovableCharacter : public Entity
{
public:
	virtual ~MovableCharacter() = default;
	FPoint position {0,0};
	FPoint velocity {0,0};
	FRect hitboxOffset {};
	FRect bumpingHitboxOffset {};
	float width = 0;
	float height = 0;
	int health = 0;
	int maxHealth = 0;
	float speed = 0.0;
	bool isHit = false;
	bool looksRight = true;
	int animationStep = 0;
	int animationFrame = 0;
	MovableCharacter() = default;

	virtual void Move(FPoint direction) = 0;
	virtual void Collide(MovableCharacter * colliding, Point cam) = 0;
	virtual FRect CalculateHitbox(Point cam) = 0;
	virtual FRect CalculateBumpingHitbox(Point cam) = 0;
};

class PlayerCharacter final : public MovableCharacter
{
private:
	float speed = 3;
public:
	FPoint position {1280/2,960/2};
	FPoint velocity {0,0};
	FRect hitboxOffset {10, 20, 80, 80};
	FRect bumpingHitboxOffset {};
	float height = 100;
	float width = 100;
	int health = 50;
	int maxHealth = 50;
	int invincible = 0;
	int animationStep = 0;
	int animationFrame = 0;
	bool isMoving = false;

	Vector<Weapon *> weapons;

	PlayerCharacter();
	void Move(FPoint direction) override;
	void Collide(MovableCharacter * colliding, Point cam) override;
	FRect CalculateHitbox(Point cam) override;
	FRect CalculateBumpingHitbox(Point cam) override;

	int GetType() override { return 0; }
};


class Enemy final : public MovableCharacter
{
private:
	float speed = 3.0;
	characterType type;
public:
	FPoint position {500,500};
	FPoint velocity {0,0};
	FRect hitboxOffset {};
	FRect bumpingHitboxOffset {};
	float height = 32;
	float width = 32;
	int health = 5;
	int maxHealth = 5;
	int damage;
	int looksRight = true;
	int animationStep = 0;
	int animationFrame = 0;

	Enemy( characterType type, FPoint position);

	void Move(FPoint direction) override;
	void Collide(MovableCharacter * colliding, Point cam) override;
	FPoint CalculateDirectionToTarget( FPoint target);
	FRect CalculateHitbox(Point cam) override;
	FRect CalculateBumpingHitbox(Point cam) override;
	void DoDamage(int dmgNumber);

	characterType GetEnemyType();

	int GetType() override { return 1; }
};

