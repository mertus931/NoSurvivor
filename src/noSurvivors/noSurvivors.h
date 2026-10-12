#pragma once

#include <global.h>

#include <gamebase.h>
#include "util/rectHelperExtra.h"
#include <autocast_ptr.h>
#include <tiles.h>
#include <map>
#include "Entities/movableCharacter.h"
#include "noSurvivors/Entities/projectile.h"

class NoSurvivors final : public Game
{
public:
	NoSurvivors();
	bool debugEnemyHitboxes = false;
	bool debugEnemyBumpingHitboxes = false;
	bool debugQuadrants = false;

	bool HandleEvent( const Event & event) override;
};

class PlayingGameState : public GameState
{
public:
	[[nodiscard]] NoSurvivors & GetGame() const override { return dynamic_cast<NoSurvivors &>( GameState::GetGame() ); }

	// ctor
	using GameState::GameState;
};

class IntroState : public PlayingGameState
{
protected:
	Font    * font              = nullptr;
	Texture * image             = nullptr;
	Music   * music             = nullptr;
	Chunk   * sound             = nullptr;
	Texture * blendedText       = nullptr;
	Point     blendedTextSize   = { 0, 0 };
	u8        textmode          = 1;

	static constexpr const Color white { 255, 255, 255, SDL_ALPHA_OPAQUE };
	static constexpr const Color black {   0,   0,   0, SDL_ALPHA_OPAQUE };

	static constexpr const Array<Point,8> shadowOffsets = {
		/*
		Point { -1, -1 },
		Point { +1, -1 },
		Point { -1, +1 },
		Point { +1, +1 },
		Point { +0, +2 },
		Point { +2, +0 },
		Point { +0, -2 },
		Point { -2, +0 },
		*/
		1_up   + 1_left,
		1_up   + 1_right,
		1_down + 1_left,
		1_down + 1_right,
		2_right,
		2_left,
		2_up,
		2_down,
	};

public:
	// ctor
	using PlayingGameState::PlayingGameState;

	void Init() override;
	void UnInit() override;

	bool HandleEvent( const Event & event ) override;
	void Update( const u32 frame, const u32 totalMSec, const float deltaT ) override;
	void Render( const u32 frame, const u32 totalMSec, const float deltaT ) override;

	[[nodiscard]] Color GetClearColor() const override { return Color { 255, 255, 255, SDL_ALPHA_OPAQUE }; }
};

class MapState : public PlayingGameState
{
	Texture * tileSet           = nullptr;
	Font    * font              = nullptr;
	int background[100 * 100]{};
	Texture * blendedText       = nullptr;
	Point     blendedTextSize   = { 0, 0 };

	static constexpr const Color white { 255, 255, 255, SDL_ALPHA_OPAQUE };
	static constexpr const Color black {   0,   0,   0, SDL_ALPHA_OPAQUE };

	static constexpr const Array<Point,8> shadowOffsets = {
		/*
		Point { -1, -1 },
		Point { +1, -1 },
		Point { -1, +1 },
		Point { +1, +1 },
		Point { +0, +2 },
		Point { +2, +0 },
		Point { +0, -2 },
		Point { -2, +0 },
		*/
		1_up   + 1_left,
		1_up   + 1_right,
		1_down + 1_left,
		1_down + 1_right,
		2_right,
		2_left,
		2_up,
		2_down,
	};

	u32 stateTime = 0;

	std::map<characterType, SDL_Texture*> assets;

	std::map<int, std::map<characterType, int>> countEnemyTypePerMinute {
		{0, {
		    {characterType::BEE, 30},
		    {characterType::WASP, 0},
		    {characterType::HORNET, 0},
	    }},
		{1, {
			{characterType::BEE, 20},
		    {characterType::WASP, 10},
		    {characterType::HORNET, 0},
		}},
		{2, {
		    {characterType::BEE, 30},
		    {characterType::WASP, 20},
		    {characterType::HORNET, 0},
	    }},
		{3, {
		    {characterType::BEE, 40},
		    {characterType::WASP, 30},
		    {characterType::HORNET, 0},
	    }},
		{4, {
		    {characterType::BEE, 5},
		    {characterType::WASP, 40},
		    {characterType::HORNET, 0},
	    }},
		{5, {
		    {characterType::BEE, 0},
		    {characterType::WASP, 0},
		    {characterType::HORNET, 1},
	    }},
		{6, {
		    {characterType::BEE, 0},
		    {characterType::WASP, 0},
		    {characterType::HORNET, 1},
	    }},
	};

	std::map<characterType, int> currentEnemyCountByType {
		{characterType::BEE,    0},
		{characterType::WASP,   0},
		{characterType::HORNET, 0},
	};

protected:
	virtual void RenderBackground( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT);
	virtual void RenderTimer(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT);
	virtual void RenderHealthbar(const u32 framesSinceStart, const u32 msSinceStart, const float deltaT);
	virtual void SpawnEnemies( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT);
	virtual void restart(const u32 msSinceStart);

	static constexpr const Array<Point,9> mapOffsets {
		Point {0, 0},
		16_right * 100,
		16_left * 100,
		16_down * 100,
		16_up * 100,
		(16_right + 16_down) * 100,
		(16_left + 16_down) * 100,
		(16_right + 16_up) * 100,
		(16_left + 16_up) * 100
	};

public:
	MapState( Game & game_, Renderer * render_ );
	Point cam {};
	PlayerCharacter pc {};
	Vector<Enemy> enemies {};
	Vector<Projectile> projectiles {};

	void Init() override;
	void UnInit() override;
	void Input() override;
	bool HandleEvent( const Event & event ) override;
	void Update( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT ) override;
	void Render( const u32 framesSinceStart, const u32 msSinceStart, const float deltaT ) override;
};