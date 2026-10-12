#ifndef SDL_BASEGAME_RECTHELPEREXTRA_H
#define SDL_BASEGAME_RECTHELPEREXTRA_H
#include "recthelper.h"

constexpr Rect fromPoints(const Point &pos, const Point &size) {
	return toXY(pos) + toWH(size);
}

constexpr Rect fromPoints(const Point &pos, int w, int h) {
	return toXY(pos) + toWH(Point {w, h});
}

constexpr Rect fromPoints(int x, int y, const Point &size) {
	return toXY(Point {x, y}) + toWH(size);
}

constexpr FRect fromFPoints(const FPoint &pos, const FPoint &size) {
	return toXY(pos) + toWH(size);
}

constexpr FRect fromFPoints(const FPoint &pos, float w, float h) {
	return toXY(pos) + toWH(FPoint {w, h});
}

constexpr FRect fromFPoints(float x, float y, const FPoint &size) {
	return toXY(FPoint {x, y}) + toWH(size);
}

#endif //SDL_BASEGAME_RECTHELPEREXTRA_H
