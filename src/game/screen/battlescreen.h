#pragma once

#include <stack>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include <alligator/game/iscreen.h>
#include <alligator/input/input.h>
#include <alligator/camera/camera.h>
#include <alligator/util/matrix.h>

#include "../battle/entity.h"
#include "../battle/hud.h"
#include "../assets.h"


class LD35;

enum HudState : int
{
	ChooseAbility = 0,
	ChooseEnemy,
};

enum BattleStatus : int
{
	PlayerTurn,
	EnemyTurn,
	DecideTurn,
	InfoTurn,
	EndBattle
};


class BattleScreen : public IScreen
{
public:
	BattleScreen(LD35* g);

	virtual ~BattleScreen();

	// IScreen interface
	void show() override;
	void update(double delta) override;
	void render() override;
	void hide() override;

	void setEnemyTurn()
	{
		m_battleStatus = EnemyTurn;
		m_lastx = m_lasty = 0;
	}

	void message( BattleStatus returningStatus, std::string txt1, std::string txt2 = "", std::string txt3 = ""  )
	{
		m_infomsg1 = txt1;
		m_infomsg2 = txt2;
		m_infomsg3 = txt3;
		m_battleStatus = InfoTurn;
		m_storedStatus = returningStatus;
	}

	int consumeExperience()
	{
		int ret = partyExperience;
		partyExperience = 0;
		return ret;
	}

private:
	LD35* m_game;
	PlayerHud m_hud;
	EnemyHud m_enemyHud;
	HudState m_hudState = ChooseAbility;
	BattleStatus m_battleStatus = PlayerTurn;
	std::string m_infomsg1;
	std::string m_infomsg2;
	std::string m_infomsg3;
	BattleStatus m_storedStatus;
	int partyExperience = 0;

	std::stack<std::string> notifs;
	bool m_infoForNotif = false;

	int m_lastx, m_lasty;

};
