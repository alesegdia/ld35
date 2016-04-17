#pragma once

#include <allegro5/allegro5.h>

#include <alligator/game/iscreen.h>
#include <alligator/input/input.h>
#include <alligator/camera/camera.h>

#include "../map/mapgen.h"
#include "../map/maprenderer.h"
#include "../map/mapentity.h"


class LD35;

class MapScreen : public IScreen {
public:
	MapScreen(LD35* g);

	virtual ~MapScreen();

	// IScreen interface
	void show() override;
	void update(double delta) override;
	void render() override;
	void hide() override;

	void scroll(Vec2f new_cam_pos, Camera& cam);

	void map( const Matrix2Di& map );

	int getTileFrontPlayer();

	Matrix2Di::SharedPtr map();

	Vec2i findFirstWalkable();

	Vec2i randomFreeTile();

	void infoMessage(std::string line1, std::string line2 = "", std::string line3 = "", std::string line4 = "")
	{
		m_infomessage1 = line1;
		m_infomessage2 = line2;
		m_infomessage3 = line3;
		m_infomessage4 = line4;
		m_isInfo = true;
	}

	void nextLevel();

private:
	LD35* m_game;
	MapRenderer::SharedPtr m_mapRenderer;
	PlayerPawnMap::SharedPtr m_player;
	Matrix2Di::SharedPtr m_map;

	std::vector<Vec2i> m_freeTiles;

	bool m_isInfo = false;

	std::string m_infomessage1;
	std::string m_infomessage2;
	std::string m_infomessage3;
	std::string m_infomessage4;

	Vec2i m_prevPlayerPos;


};


