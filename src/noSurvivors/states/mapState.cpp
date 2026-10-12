#include "../noSurvivors.h"
#include "noSurvivors/util/quadrant.h"

void MapState::RenderBackground( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT)
{
	//const auto & [sizeX, sizeY] = GetGame().GetWindowSize();
	Rect src {0, 0, 16, 16};
	Rect dest = {0, 0, 16, 16};

	Point copyCam = cam % (100 * 16);

	for(int y = 0; y < 100; y++)
	{
		for(int x = 0; x < 100; x++)
		{
			const int index = background[x + y * 100];
			src.x = index % 8 * 16;
			src.y = index / 8 * 16;
			for(auto &offset : mapOffsets) {
				dest.x = x * 16 - copyCam.x - offset.x;
				dest.y = y * 16 - copyCam.y - offset.y;
				SDL_RenderCopy(renderer, tileSet, &src, &dest);
			}
		}
	}
}

void MapState::RenderTimer( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT )
{
	u32 elapsedTime = msSinceStart - stateTime;
	int minutesSinceStart = std::floor(elapsedTime / (60*1000));
	int secondsSinceStart = (elapsedTime / 1000) % 60;

	const char * leadingZeroMinutes = minutesSinceStart < 10 ? "0" : "";
	const char * leadingZeroSeconds = secondsSinceStart < 10 ? "0" : "";

	string text = format("{}{}:{}{}\n", leadingZeroMinutes, minutesSinceStart, leadingZeroSeconds, secondsSinceStart);

	static Point p{ 0, 0 };
	static int colorIndex = 9;
	static bool isDarkOutline = true;

	const Color outlineColor = isDarkOutline ? Color{ 0, 0, 0 } : Color{ 255, 255, 255 };

	if (blendedText != nullptr)
		SDL_DestroyTexture(blendedText);

	Surface* surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), white, PlayingGameState::GetGame().GetWindowSize().x);
	blendedText = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	u32 fmt;
	int access;
	SDL_QueryTexture(blendedText, &fmt, &access, &blendedTextSize.x, &blendedTextSize.y);

	SDL_SetTextureColorMod( blendedText, outlineColor.r, outlineColor.g, outlineColor.b);

	p = {(PlayingGameState::GetGame().GetWindowSize().x / 2) - (blendedTextSize.x / 2), 20};
	for( const Point & pd : shadowOffsets )
	{
		const Rect dst_rect = Rect{ p.x + pd.x, p.y + pd.y, blendedTextSize.x, blendedTextSize.y };
		SDL_RenderCopy( renderer, blendedText, EntireRect, &dst_rect );
	}

	const Color color = sor::hsnr64::Palette[colorIndex];
	SDL_SetTextureColorMod( blendedText, color.r, color.g, color.b );
	const Rect dst_rect = { p.x, p.y, blendedTextSize.x, blendedTextSize.y };
	SDL_RenderCopy( renderer, blendedText, EntireRect, &dst_rect );
}

void MapState::RenderHealthbar(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT) {
	float healthbarWidth = 50.0;
	float healthbarHeight = 5.0;

	float playerHalf = pc.position.x - cam.x + pc.width / 2 ;
	float healthbarX = playerHalf - healthbarWidth / 2;
	float fillIn = (healthbarWidth / pc.maxHealth) * pc.health;

	FRect maxHealthRect = {healthbarX - 1, pc.position.y - cam.y + pc.height + 5 -1, healthbarWidth + 2, healthbarHeight +2};
	FRect actualHealthRect = {healthbarX, pc.position.y - cam.y + pc.height + 5, fillIn, healthbarHeight};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRectF(renderer, &maxHealthRect);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRectF(renderer, &actualHealthRect);
}

void MapState::SpawnEnemies( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT) {
	u32 elapsedTime = msSinceStart - stateTime;
	int minutesSinceStart = std::floor(elapsedTime / (60*1000));

	for (auto const& [enemyType, currentCount] : currentEnemyCountByType) {
		int currentCountVal = currentCount;

		for (int i = 0; i < countEnemyTypePerMinute[minutesSinceStart][enemyType] - currentCountVal; ++i) {
			int random_angle = std::rand() % 359;
			int radius = sqrt(pow(game.GetWindowSize().x, 2) + pow(game.GetWindowSize().y, 2)) + 10;
			int randRadius = std::rand() % 50 - 50;
			float x = pc.position.x + ((radius + randRadius) * cos(random_angle));
			float y = pc.position.y + ((radius + randRadius) * sin(random_angle));
			enemies.push_back(Enemy {enemyType, {x, y}});
			currentEnemyCountByType[enemyType]++;
		}
	}
}

MapState::MapState(Game& game_, Renderer* render_) : PlayingGameState(game_, render_)
{
	std::fill_n(background, 100 * 100, 9);
	std::fill_n(background + 100 * 33, 300, 6);
	for(int i = 0; i < 100; i++)
	{
		background[i * 100 + 44] = 6;
		background[i * 100 + 45] = 6;
		background[i * 100 + 46] = 6;
	}
}

void MapState::Init()
{
	if(!tileSet)
	{
		tileSet = IMG_LoadTexture(renderer, BasePath "asset/graphic/maps/map1/Tileset.png");
	}

	if( !font )
	{
		font = TTF_OpenFont( BasePath "asset/font/RobotoSlab-Bold.ttf", 24 );
		TTF_SetFontHinting( font, TTF_HINTING_LIGHT );
		if( !font )
			print( stderr, "TTF_OpenFont failed: {}\n", TTF_GetError() );
	}

	if (assets.empty()) {
		SDL_Texture * bee = IMG_LoadTexture(renderer, BasePath "asset/graphic/Enemy/bee.png");
		SDL_Texture * wasp = IMG_LoadTexture(renderer, BasePath "asset/graphic/Enemy/wasp.png");
		SDL_Texture * hornet = IMG_LoadTexture(renderer, BasePath "asset/graphic/Enemy/hornet.png");
		SDL_Texture * dudeMonster = IMG_LoadTexture(renderer, BasePath "asset/graphic/Characters/Dude_Monster/Dude_Monster_Walk_6.png");

		assets = {
			{characterType::PC,     dudeMonster},
			{characterType::BEE,    bee},
			{characterType::WASP,   wasp},
			{characterType::HORNET, hornet},
		};
	}

	//enemies.emplace_back();
}

void MapState::UnInit()
{

}

void MapState::Input()
{
	// Is not supressed during ImGui input
	const u8 * key_array = SDL_GetKeyboardState( nullptr );

	FPoint movement = {0,0};
	if( key_array[SDL_SCANCODE_W] || key_array[SDL_SCANCODE_UP] )
	{
		movement.y += -1.0;
	}
	if( key_array[SDL_SCANCODE_A] || key_array[SDL_SCANCODE_LEFT] )
	{
		movement.x += -1.0;
	}
	if( key_array[SDL_SCANCODE_S] || key_array[SDL_SCANCODE_DOWN] )
	{
		movement.y += 1.0;
	}
	if( key_array[SDL_SCANCODE_D] || key_array[SDL_SCANCODE_RIGHT] )
	{
		movement.x += 1.0;
	}

	pc.Move(movement);
}

bool MapState::HandleEvent(const Event& event)
{
	return false;
}

void MapState::Update(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT)
{
	SpawnEnemies(framesSinceStart, msSinceStart, deltaT);
	if(pc.invincible > 0) {
		pc.invincible--;
	}

	const Point & winSize = game.GetWindowSize();
	pc.position.x += pc.velocity.x;
	pc.position.y += pc.velocity.y;
	cam.x = pc.position.x - winSize.x / 2 + 16;
	cam.y = pc.position.y - winSize.y / 2 + 16;

	for (Weapon * weapon : pc.weapons) {
		if (!weapon->IsOnCooldown()) {
			weapon->Attack(this);
		}
	}


	Quadrant<Entity> quadrants {winSize};

	quadrants.insert(&pc, {pc.position.x - cam.x, pc.position.y - cam.y}, {pc.width, pc.height});

	int shitLoop = 0;
	for(auto &enemy : enemies) {
		float x = enemy.position.x - cam.x;
		float y = enemy.position.y - cam.y;

		FPoint pcCenter = {pc.position.x + pc.width/2, pc.position.y + pc.height/2};

		enemy.Move(enemy.CalculateDirectionToTarget( pcCenter ));

		FPoint targetVector = pc.position - enemy.position;
		float diffToTarget = sqrt(pow(targetVector.x, 2) + pow(targetVector.y, 2));
		float deathZone = sqrt(pow(GetGame().GetWindowSize().x, 2) + pow(GetGame().GetWindowSize().y, 2));
		if ( diffToTarget > deathZone + 100) {
			enemies.erase(enemies.begin() + shitLoop);
			currentEnemyCountByType[enemy.GetEnemyType()]--;
		} else {
			if(x >= 0 && x <= winSize.x && y >= 0 && y <= winSize.y) {
				quadrants.insert(&enemy, {x, y}, {enemy.width, enemy.height});
			}
			shitLoop++;
		}
	}

	int stupidLoopIndex = 0;
	for (Projectile & projectile : projectiles) {
		bool despawnProjectile = false;
		projectile.Move(framesSinceStart, msSinceStart, deltaT);

		if (projectile.IsTooFar( cam, GetGame().GetWindowSize())) {
			despawnProjectile = true;
		}

		quadrants.insert(&projectile, projectile.getPosition() - FPoint{(float)cam.x, (float)cam.y}, projectile.getSize());

		FPoint pos = projectile.getPosition() - FPoint{(float)cam.x, (float)cam.y};
		for (Entity * entity : quadrants.get(pos, projectile.getSize())) {
			if(entity->GetType() == 1) {
				auto enemy = (Enemy *) entity;

				FRect projectileRect = FRect{projectile.getPosition().x - cam.x, projectile.getPosition().y - cam.y, projectile.getSize().x, projectile.getSize().y};
				FRect enemyRect = enemy->CalculateHitbox(cam);

				if (SDL_HasIntersectionF( &projectileRect, &enemyRect)) {
					enemy->isHit = true;
					enemy->DoDamage( projectile.damage );
					despawnProjectile = true;
					break;
				}
			}
		}

		if (despawnProjectile)
			projectiles.erase(projectiles.begin()+stupidLoopIndex);
		else
			stupidLoopIndex++;
	}

	for(std::set<Entity *> quadrant : quadrants.getAll()) {
		for(auto i = quadrant.begin(); i != quadrant.end(); i++) {
			for(auto j = i; ++j != quadrant.end();) {
				FRect playerRect = pc.CalculateHitbox(cam);
				if((*i)->GetType() == 1) {
					auto enemyI = (Enemy *)(*i);
					if((*j)->GetType() == 1) {
						auto enemyJ = (Enemy *)(*j);
						enemyI->Collide(enemyJ, cam);
					}
					if((*j)->GetType() == 0) {
						pc.Collide(enemyI, cam);
					}
				} else if((*i)->GetType() == 0) {
					if((*j)->GetType() == 1) {
						auto enemy = (Enemy *)(*j);
						pc.Collide(enemy, cam);
					}
				}
			}
		}
	}

	for(auto &enemy : enemies) {
		enemy.position.x += enemy.velocity.x;
		enemy.position.y += enemy.velocity.y;
	}

	if(pc.health <= 0) {
		restart(msSinceStart);
	}
}

void MapState::Render(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT)
{
	RenderBackground(framesSinceStart, msSinceStart, deltaT);

	for (Projectile & projectile : projectiles) {
		projectile.Render(framesSinceStart, msSinceStart, deltaT, cam, renderer);
	}

	{
		Point spriteSheetSize;
		const FRect playerRect { pc.position.x - cam.x, pc.position.y - cam.y, pc.width, pc.height };
		SDL_QueryTexture( assets[PC], NULL, NULL, &spriteSheetSize.x, &spriteSheetSize.y);

		int frameDuration = 8;
		int frameCount = 5;

		if (pc.isMoving) {
			if(pc.animationStep % frameDuration == 0)
			{
				pc.animationFrame++;
			}
			pc.animationStep++;
			if(pc.animationFrame == frameCount)
			{
				pc.animationFrame = 0;
			}
		}

		Rect srcRect = {(spriteSheetSize.x/6) * pc.animationFrame, 0, spriteSheetSize.x/6, spriteSheetSize.y};
		if (pc.looksRight) {
			SDL_RenderCopyExF(renderer, assets[PC], &srcRect, &playerRect, 0, NULL, SDL_FLIP_HORIZONTAL);
		} else {
			SDL_RenderCopyF( renderer, assets[PC], &srcRect, &playerRect);
		}

		if (GetGame().debugEnemyHitboxes) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			FRect hitbox = pc.CalculateHitbox(cam);
			SDL_RenderDrawRectF(renderer, &hitbox);
		}
	}

	{
		int stupidIndex = 0;
		for (Enemy & enemy : enemies) {
			const FRect enemyRect { enemy.position.x - cam.x, enemy.position.y - cam.y, (float)enemy.width, (float)enemy.height };
			SDL_Texture * enemyTexture = assets[enemy.GetEnemyType()];
			SDL_SetTextureColorMod(enemyTexture, 255, 255, 255);
			if (enemy.isHit) {
				SDL_SetTextureColorMod(enemyTexture, 255, 0, 0);
				stupidIndex++;
			}
			else if (enemy.health <= 0) {
				enemies.erase(enemies.begin() + stupidIndex);
				currentEnemyCountByType[enemy.GetEnemyType()]--;
			}
			else {
				stupidIndex++;
			}
			enemy.isHit = false;

			if (enemy.looksRight) {
				SDL_RenderCopyExF(renderer, assets[enemy.GetEnemyType()], NULL, &enemyRect, 0, NULL, SDL_FLIP_HORIZONTAL);
			} else {
				SDL_RenderCopyF( renderer, assets[enemy.GetEnemyType()], NULL, &enemyRect);
			}
			if (GetGame().debugEnemyHitboxes) {
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				FRect hitbox = enemy.CalculateHitbox(cam);
				SDL_RenderDrawRectF(renderer, &hitbox);
			}
			if (GetGame().debugEnemyBumpingHitboxes) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
				FRect bumpingHitbox = enemy.CalculateBumpingHitbox(cam);
				SDL_RenderDrawRectF(renderer, &bumpingHitbox);
			}
		}
	}
	RenderTimer(framesSinceStart, msSinceStart, deltaT);
	RenderHealthbar(framesSinceStart, msSinceStart, deltaT);
}

void MapState::restart(const u32 msSinceStart)
{
	currentEnemyCountByType = {
		{characterType::BEE,    0},
		{characterType::WASP,   0},
		{characterType::HORNET, 0},
	};

	pc = {};
	enemies = {};
	projectiles = {};
	stateTime = msSinceStart;
}