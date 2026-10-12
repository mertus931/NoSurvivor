#include "projectile.h"

Projectile::Projectile( FPoint _position, FPoint _direction, int _damage)
{
	position = _position;
	direction = _direction;
	damage = _damage;
}

void Projectile::Render( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT, Point cam, Renderer * renderer ) const
{
	const FRect projectileRect { position.x - cam.x, position.y - cam.y, this->size.x, this->size.y };
	SDL_SetRenderDrawColor(renderer, 255,0,0,255);
	SDL_RenderFillRectF(renderer, &projectileRect);
}

void Projectile::Move( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT )
{
	position.x += direction.x * speed;
	position.y += direction.y * speed;
}

bool Projectile::IsTooFar( Point cam, Point windowsSize) const
{
	return (
		position.x - cam.x < 0 ||
		position.x - cam.x > windowsSize.x ||
		position.y - cam.y < 0 ||
		position.y - cam.y > windowsSize.y);
}

const FPoint & Projectile::getPosition()
{
	return position;
}

const FPoint & Projectile::getSize()
{
	return size;
}
