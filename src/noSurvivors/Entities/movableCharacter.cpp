#include "movableCharacter.h"
#include "global.h"
#include "recthelper.h"
#include "weapon.h"

PlayerCharacter::PlayerCharacter()
{
	auto * diagonalShot = new DiagonalShot();
	weapons.push_back(diagonalShot);
}

void PlayerCharacter::Move( FPoint direction )
{
	FPoint noMovement = {0.0, 0.0};
	if (direction.x == noMovement.x && direction.y == noMovement.y)
	{
		isMoving = false;
	} else {
		if (direction.x == 1)
		{
			looksRight = false;
		}
		else if (direction.x == -1)
		{
			looksRight = true;
		}
		isMoving = true;
	}

	velocity.x = speed * direction.x;
	velocity.y = speed * direction.y;
}

void PlayerCharacter::Collide(MovableCharacter * collidingObject, Point cam)
{
	auto colliding = (Enemy*) collidingObject;
	FRect enemyHitbox = colliding->CalculateHitbox(cam);
	FRect playerHitbox = CalculateHitbox(cam);
	if(SDL_HasIntersectionF(&enemyHitbox, &playerHitbox) && invincible == 0) {
		health -= colliding->damage;
		invincible = 5;
	}
}

FRect PlayerCharacter::CalculateHitbox(Point cam) {
	return {(position.x + hitboxOffset.x) - cam.x,(position.y + hitboxOffset.y) - cam.y,hitboxOffset.w,hitboxOffset.h};
}

FRect PlayerCharacter::CalculateBumpingHitbox(Point cam) {
	return {(position.x + bumpingHitboxOffset.x) - cam.x,(position.y + bumpingHitboxOffset.y)- cam.y,bumpingHitboxOffset.w,bumpingHitboxOffset.h};
}

Enemy::Enemy( characterType type, FPoint position )
{
	switch(type){
		case BEE:
			this->type = BEE;

			this->maxHealth = 5;
			this->health = this->maxHealth;
			this->damage = 2;
			this->speed = 2;
			this->width = 50;
			this->height = 50;
			this->hitboxOffset = {0,0,width,height};
			this->bumpingHitboxOffset = {15,15,width-30,height-30};
			break;
		case WASP:
			this->type = WASP;

			this->maxHealth = 10;
			this->health = this->maxHealth;
			this->damage = 4;
			this->speed = 3;
			this->width = 70;
			this->height = 70;
			this->hitboxOffset = {0,30,width,height-30};
			this->bumpingHitboxOffset = {20,45,width - 40,height-60};
			break;
		case HORNET:
			this->type = HORNET;

			this->maxHealth = 500;
			this->health = this->maxHealth;
			this->damage = 20;
			this->speed = 100.0;
			this->width = 100;
			this->height = 100;
			this->hitboxOffset = {0,0,width,height};
			this->bumpingHitboxOffset = {0,0,width,height};
			break;
		default:
			this->health = 5;
			this->damage = 1;
			this->speed = 3.0;
			break;
	}
	this->position.x = position.x;
	this->position.y = position.y;
}

FPoint Enemy::CalculateDirectionToTarget( FPoint target )
{
	FPoint targetVector = target - position;

	float targetVectorLength = sqrt(pow(targetVector.x, 2) + pow(targetVector.y, 2));

	float addToRoundX = targetVector.x > 0 ? 0.5 : -0.5;
	float addToRoundY = targetVector.y > 0 ? 0.5 : -0.5;

	FPoint normalizedTargetVector {
		normalizedTargetVector.x = (int)((targetVector.x / targetVectorLength) +addToRoundX),
		normalizedTargetVector.y = (int)((targetVector.y / targetVectorLength) +addToRoundY)
	};

	if (normalizedTargetVector.x > 0) {
		looksRight = true;
	} else if (normalizedTargetVector.x < 0) {
		looksRight = false;
	}
	return FPoint {normalizedTargetVector.x, normalizedTargetVector.y};
}

void Enemy::Move( FPoint direction )
{
	velocity.x = speed * direction.x;
	velocity.y = speed * direction.y;
}

void Enemy::Collide(MovableCharacter * collidingObject, Point cam)
{
	auto colliding = (Enemy*) collidingObject;
	FRect overlap {};
	FRect thisRect = CalculateBumpingHitbox(cam);
	FRect collidingObjectRect = collidingObject->CalculateBumpingHitbox(cam);

	if(SDL_IntersectFRect(&thisRect, &collidingObjectRect, &overlap)) {
		if (overlap.w < overlap.h) {
			float direction = (position.x < colliding->position.x) ? 1.0 : -1.0;
			float distance = overlap.w / 2.0;
			position.x -= distance * direction;
			colliding->position.x += distance * direction;

			float relativeVelocity = colliding->velocity.x - velocity.x;
			float elasticity = 0;
			velocity.x += (2.0 * relativeVelocity * colliding->width) / (width + colliding->width) * elasticity;
			colliding->velocity.x -= (2.0 * relativeVelocity * width) / (width + colliding->width) * elasticity;
		} else {
			float direction = (position.y < colliding->position.y) ? 1.0 : -1.0;
			float distance = overlap.h / 2.0;
			position.y -= distance * direction;
			colliding->position.y += distance * direction;

			float relativeVelocity = colliding->velocity.y - velocity.y;
			float elasticity = 0;
			velocity.y += (2.0 * relativeVelocity * colliding->height) / (height + colliding->height) * elasticity;
			colliding->velocity.y -= (2.0 * relativeVelocity * height) / (height + colliding->height) * elasticity;
		}
	}
}

void Enemy::DoDamage( int dmgNumber )
{
	health -= dmgNumber;
}

FRect Enemy::CalculateHitbox(Point cam) {
	return {(position.x + hitboxOffset.x) - cam.x,(position.y + hitboxOffset.y) - cam.y,hitboxOffset.w,hitboxOffset.h};
}

FRect Enemy::CalculateBumpingHitbox(Point cam) {
	return {(position.x + bumpingHitboxOffset.x) - cam.x ,(position.y + bumpingHitboxOffset.y) - cam.y,bumpingHitboxOffset.w,bumpingHitboxOffset.h};
}

characterType Enemy::GetEnemyType() {
	return this->type;
}