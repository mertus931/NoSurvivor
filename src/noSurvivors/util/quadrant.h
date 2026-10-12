#ifndef SDL_BASEGAME_QUADRANT_H
#define SDL_BASEGAME_QUADRANT_H

#include <set>
#include "global.h"
#include "recthelper.h"

template<typename T, size_t quadXSize = 16, size_t quadYSize = 16>
class Quadrant {
	Array<std::set<T *>, quadXSize * quadYSize> quadrants {};
	Point borders {};

	int genIndex(const FPoint &pos) {
		int x = pos.x / borders.x;
		int y = pos.y / borders.y;
		if(x < 0 || x > 16 || y < 0 || y > 16)
			return -1;
		int index = std::min(15, x) + std::min(15, y) * 16;
		return index;
	}

	Vector<int> genAllIndices(const FPoint &pos, const FPoint &size) {
		Vector<int> ret {};
		int index = genIndex(pos);
		if(index >= 0)
			ret.push_back(index);

		FPoint p = pos + 16.0_right;
		FPoint last = pos + size;
		while(p.y < last.y) {
			while(p.x < last.x) {
				index = genIndex(pos);
				if(index >= 0)
					ret.push_back(index);
				p.x += 16;
			}
			p.x = pos.x;
			p.y += 16;
		}

		index = genIndex(last);
		if(index >= 0)
			ret.push_back(index);
		return ret;
	}

public:
	explicit Quadrant(const Point & size) {
		borders.x = size.x / quadXSize;
		borders.y = size.y / quadYSize;
		quadrants.fill(std::set<T *> {});
	}

	void insert(T *elem, const FPoint &pos, const FPoint &size) {
		for(int i : genAllIndices(pos, size)) {
			quadrants[i].insert(elem);
		}
	}

	void insert(T *elem, const Rect &rect) {
		insert(elem, Point { rect.x, rect.y}, Point {rect.w, rect.h});
	}

	std::set<T *> get(const FPoint &pos, const FPoint &size) {
		std::set<T *> ret {};

		for(int i : genAllIndices(pos, size)) {
			auto s = quadrants[i];
			ret.merge(s);
		}

		return ret;
	}

	std::set<T *> get(const Rect &rect) {
		return get(Point { rect.x, rect.y}, Point {rect.w, rect.h});
	}

	const Array<std::set<T *>, quadXSize * quadYSize> &getAll() {
		return quadrants;
	}
};

#endif //SDL_BASEGAME_QUADRANT_H
