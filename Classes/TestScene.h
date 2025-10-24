#pragma once
#include "cocos2d.h"
#include "Enemy_Metall.h" // Added back
// #include "Enemy_Bunby.h" // Removed
// #include "Enemy_Boss.h" // Removed
#include "Core.h"
#include "Utils.h"
#include <unordered_map>
#include <cmath>
// #include "RestartHelper.h"; // Removed
// #include "SoundManager.h" // Removed


enum _moveState {
	MS_PLACEHOLDER,
	MS_INIT,
	// MS_TELEPORT, // Removed
	MS_STOP,
	MS_RUN,
	MS_JUMP,
	MS_SLIDING,
	MS_FALL,
	MS_HURT // Kept for anim
};

enum _attackState {
	ATK_PLACEHOLDER,
	DEFAULT_POSE,
	STOP_SHOOT,
	RUN_SHOOT,
	JUMP_SHOOT,
	FALL_SHOOT
};

// Added back _bullets enum
enum _bullets {
	COMMON_BULLET,
	CHARGE_X1,
	CHARGE_X2
};

class TestScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(TestScene);

	void onEnter();
	void onExit();

	void update(float dt) override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	void moveAnimState(_moveState state);
	void attackAnimState(_attackState state);
	void bulletParticleAnimState(_bullets state, float x, float y); // Added back
	void readyLogoAnim(); // Kept to trigger spawn animation
	void setReadyGame(); // Kept for spawn anim callback
	void hideReadyLogo(); // Kept for spawn anim callback

	// Removed sound and boss triggers

private:
	cocos2d::LayerColor* wlayer;

	// ----- METALL CODE ADDED START -----
	// Added back Health UI
	cocos2d::Sprite* EmptyHealthBar;
	cocos2d::Sprite* HealthBar;
	float playerHealth = 28.0f;
	// ----- METALL CODE ADDED END -----

	bool isInited;
	cocos2d::PhysicsWorld* m_world;

	void createMyPhysicsWorld();

	void turnOnMoveSprite();
	void turnOffMoveSprite();

	// ----- METALL CODE ADDED START -----
	void setOffStuck();
	// ----- METALL CODE ADDED END -----

	// Removed unused callbacks
	// void playTeleportSound();
	// void playerDie();
	// void restartGame();
	// void endGame();

	void flippedCharacter(bool state);
	void createBullet(_bullets state); // Added back

	cocos2d::Sprite* charLayer;
	cocos2d::Sprite* character;
	cocos2d::Sprite* attackChar;
	cocos2d::PhysicsBody* characterBody;
	// cocos2d::Sprite* readyLogoSprite; // Removed
	cocos2d::EventListenerKeyboard* _listener;

	bool readyGame = false;
	bool hideLogo = false; // Used by spawn anim

	// ----- METALL CODE ADDED START -----
	// Added back combat/health bools
	bool CollideEnemy = false;
	bool CollideEnemyBullet = false;
	bool isStuck = false;
	bool startStuckTimer = false;
	int contactingDamage = 0;
	float stuckTime = 0;
	// ----- METALL CODE ADDED END -----

	bool isChangeMoveSprite = false;

	float changeTime = 0;
	float atkChargeTime = 0; // Added back
	float jumpChargeTime = 0;
	// Removed combat/stuck floats

	bool fall;
	bool isJumping;
	bool movingLeft;
	bool movingRight;
	bool movingUp;
	bool readySlide;
	bool movingSliding;
	bool inputShoot; // Kept to toggle between move/attack sprites
	int movingDir = 1;	// 0 : Left, 1 : Right

	float followX = 0.0f;
	float followWidth = 1024.0f;

	// Removed triggers

	bool x1Ready = false; // Added back
	bool x2Ready = false; // Added back

	_moveState moveInputState;
	_moveState moveBeforeState;

	_attackState attackInputState;
	_attackState attackBeforeState;

	_bullets chargeState; // Added back

	// Kept anim positions for sprite alignment
	float atk_stop_pos[1][2] = { 23, 2 };
	float atk_jump_pos[5][2] = { {23, 2}, {24, 9}, {23, 12}, {24, 14}, {24, 13} };
	float atk_fall_pos[5][2] = { {24, 13}, {24, 14}, {23, 12}, {24, 9}, {23, 2} };
	float atk_run_pos[10][2] = { {23, 2}, {23, 4}, {23, 4}, {23, 2}, {23, 2}, {23, 1}, {24, 1}, {24, 3}, {24, 1}, {24, 1} };

	// Removed explosionPos

	cocos2d::Vec2 shootPosition; // Kept for anim positioning

	cocos2d::Animate* atkAnimate;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cocos2d::Vec2 spawnPosition;
	cocos2d::TMXTiledMap* tmap;

	cocos2d::TMXLayer* background;
	// Removed unused layers

	// Added back shader properties
	cocos2d::GLProgram* glProg;
	cocos2d::GLProgramState* glProgramState;

	// ----- METALL CODE ADDED START -----
	std::unordered_map<size_t, Enemy_Metall*> metalls;
	int metallsNum;
	// ----- METALL CODE ADDED END -----
};

