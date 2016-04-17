#include "mapscreen.h"
#include "../ld35game.h"
#include "../assets.h"

MapScreen::MapScreen(LD35 *g)
	: m_game(g)
{
	nextLevel(1);
}

MapScreen::~MapScreen()
{

}

void MapScreen::show()
{
	Assets::instance->battle->pause();
	if( !Assets::instance->music->playing() )
	{
		Assets::instance->music->rewind();
		Assets::instance->music->play();
	}

	if( m_game->m_player->levelingNotification.isPresent )
	{
		infoMessage( m_game->m_player->levelingNotification.text1,
					 m_game->m_player->levelingNotification.text2,
					 m_game->m_player->levelingNotification.text3,
					 m_game->m_player->levelingNotification.text4 );
		Assets::instance->levelupSfx->play();
		m_game->m_player->levelingNotification.isPresent = false;
	}
}

void MapScreen::update(double delta)
{
	if( Input::IsKeyJustPressed(ALLEGRO_KEY_F5) ) m_game->advanceFloor();
	if( Input::IsKeyJustPressed(ALLEGRO_KEY_F6) ) m_game->endGame();

	if( m_isInfo )
	{
		if( Input::IsKeyJustPressed(ALLEGRO_KEY_ENTER) )
		{
			m_isInfo = false;
			Vec2i front_tile_coords;
			int front_tile = getTileFrontPlayer(&front_tile_coords);
			if( front_tile == 2 )
			{
				m_game->advanceFloor();
			}
			// RED TILE
			else if( front_tile == 3 )
			{
				m_game->m_player->unlock(Fire);
				m_map->set(front_tile_coords.x(), front_tile_coords.y(), 0);
			}
			// GREEN TILE
			else if( front_tile == 4 )
			{
				m_game->m_player->unlock(Gaia);
				m_map->set(front_tile_coords.x(), front_tile_coords.y(), 0);
			}
			// BLUE TILE
			else if( front_tile == 5 )
			{
				m_game->m_player->unlock(Water);
				m_map->set(front_tile_coords.x(), front_tile_coords.y(), 0);
			}

		}
		if( Input::IsKeyJustPressed(ALLEGRO_KEY_BACKSPACE) )
		{
			m_isInfo = false;
		}
	}
	else
	{
		m_player->update(delta);
		if( m_player->tile() != m_prevPlayerPos )
		{
			// try new battle!
			int r = RNG::rng->nextInt(30);
			if( r < 1 )
			{
				m_game->setScreen(m_game->m_battleScreen);
			}
		}
		m_prevPlayerPos = m_player->tile();
		scroll(m_player->position(), m_game->m_camera1);
		if( Input::IsKeyJustPressed(ALLEGRO_KEY_ENTER) )
		{
			if( getTileFrontPlayer() == 2 )
			{
				infoMessage("ENTER: next floor", "BACK: cancel", "yo dawg", "that was ez");
			}
			else if( getTileFrontPlayer() == 3 )
			{
				infoMessage("Your skin burns!", "You learnt a new", "fire ability!", "");
			}
			else if( getTileFrontPlayer() == 4 )
			{
				infoMessage("You shine!", "You learnt a new", "gaia ability!", "");
			}
			else if( getTileFrontPlayer() == 5 )
			{
				infoMessage("Feeling light!", "You learnt a new", "water ability!", "");
			}

		}
	}
}

void MapScreen::scroll(Vec2f new_cam_pos, Camera& cam)
{
	new_cam_pos = new_cam_pos + Vec2f(-16 - 16, -16 - 16);
	cam.position(Vec2f(round(new_cam_pos.x()), round(new_cam_pos.y())));
}

void MapScreen::map(const Matrix2Di &map)
{
	if( m_map == nullptr )
	{
		m_map.reset(new Matrix2Di(map));
	}
	else
	{
		*m_map = map;
	}
	m_freeTiles.clear();

	for( int x = 0; x < m_map->cols(); x++ )
	{
		for( int y = 0; y < m_map->rows(); y++ )
		{
			if( m_map->get(x, y) == 0 )
			{
				m_freeTiles.push_back({x, y});
			}
		}
	}

	Vec2i end = this->randomFreeTile();
	m_map->set(end.x(), end.y(), 2);

	end = this->randomFreeTile();
	m_map->set(end.x(), end.y(), 3);
	end = this->randomFreeTile();
	m_map->set(end.x(), end.y(), 4);
	end = this->randomFreeTile();
	m_map->set(end.x(), end.y(), 5);
}

int MapScreen::getTileFrontPlayer(Vec2i* out)
{
	Vec2i tocheck(m_player->position().x() / 16, m_player->position().y() / 16);
	std::cout << tocheck << std::endl;
	if( m_player->facing() == 'u' )
	{
		tocheck = tocheck + Vec2i( 0, -1 );
	}
	else if( m_player->facing() == 'd' )
	{
		tocheck = tocheck + Vec2i( 0, 1 );
	}
	else if( m_player->facing() == 'l' )
	{
		tocheck = tocheck + Vec2i( -1, 0 );
	}
	else if( m_player->facing() == 'r' )
	{
		tocheck = tocheck + Vec2i( 1, 0 );
	}
	if( out != nullptr )
	{
		out->set(tocheck.x(), tocheck.y());
	}
	std::cout << tocheck << std::endl;
	std::cout << m_map->get(tocheck.x(), tocheck.y()) << std::endl << "======" << std::endl;
	return m_map->get(tocheck.x(), tocheck.y());
}

Matrix2Di::SharedPtr MapScreen::map()
{
	return m_map;
}

Vec2i MapScreen::findFirstWalkable()
{
	for( int x = 0; x < m_map->cols(); x++ )
	{
		for( int y = 0; y < m_map->rows(); y++ )
		{
			if( m_map->get(x, y) == 0 )
			{
				return Vec2i(x, y);
			}
		}
	}
	return Vec2i(-1, -1);
}

Vec2i MapScreen::randomFreeTile()
{
	int index = RNG::rng->nextInt(m_freeTiles.size() - 1);
	Vec2i ret = m_freeTiles[index];

	m_freeTiles.erase(m_freeTiles.begin() + index);

	return ret;
}

void MapScreen::nextLevel(int level)
{
	map(RandomWalkerGenerator().generate(10 * level, 10 * level));

	std::vector<ALLEGRO_BITMAP*> tiles;
	tiles.push_back(Assets::instance->tilesetSheet->getFrame(1, 0));
	tiles.push_back(Assets::instance->tilesetSheet->getFrame(2, 0));
	tiles.push_back(Assets::instance->tilesetSheet->getFrame(1, 1));

	tiles.push_back(Assets::instance->tilesetSheet->getFrame(0, 2));
	tiles.push_back(Assets::instance->tilesetSheet->getFrame(0, 0));
	tiles.push_back(Assets::instance->tilesetSheet->getFrame(0, 1));

	m_mapRenderer = std::make_shared<TileMapRenderer>(map(), tiles );
	Vec2i fw = findFirstWalkable();
	std::cout << fw << std::endl;
	Vec2f player_pos( fw.x() * 16, fw.y() * 16 );
	m_player.reset(new PlayerPawnMap(player_pos, map()));
}

void MapScreen::render()
{
	al_clear_to_color(al_map_rgb(0,0,0));
	al_set_target_bitmap(al_get_backbuffer(m_game->display()));

	m_game->m_camera1.bind();
	m_mapRenderer->render();
	m_player->render();

	m_game->m_camera2.bind();

	if( m_isInfo )
	{
		al_draw_filled_rectangle(0, 44, 80, 80, al_map_rgb(172, 50, 50));
		int off = 46;
		al_draw_text(m_game->m_font, al_map_rgb(255, 255, 255), 4, off, 0, m_infomessage1.c_str());
		al_draw_text(m_game->m_font, al_map_rgb(255, 255, 255), 4, off + 8, 0, m_infomessage2.c_str());
		al_draw_text(m_game->m_font, al_map_rgb(255, 255, 255), 4, off + 16, 0, m_infomessage3.c_str());
		al_draw_text(m_game->m_font, al_map_rgb(255, 255, 255), 4, off + 24, 0, m_infomessage4.c_str());

	}

	al_draw_text(m_game->m_font, al_map_rgb(255, 255, 255), 2, 2, 0, std::to_string(m_game->m_player->level()).c_str());
	al_draw_filled_rectangle(14, 5, 78, 7, al_map_rgb(172, 50, 50));

	al_draw_filled_rectangle(14, 5, 14 + m_game->m_player->expPercentage() * (78 - 14), 7, al_map_rgb(217, 87, 99));

}

void MapScreen::hide()
{

}
