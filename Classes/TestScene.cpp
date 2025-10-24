#include "TestScene.h"
#include "Enemy_Metall.h" // ----- METALL CODE ADDED -----
// #include "ReplaceEndSceneHelper.h" // Removed

USING_NS_CC;

cocos2d::Scene* TestScene::createScene()
{
	return TestScene::create();
}

bool TestScene::init()
{
	if (Scene::initWithPhysics())
		createMyPhysicsWorld();
	else return false;

	// soundManager->StopAllSounds(); // Removed

	// Main Scene (Ruined Street Map) Layer (2304x224)
	wlayer = LayerColor::create(Color4B(0, 148, 255, 255));	// Skyblue Color
	wlayer->setContentSize(Size(2304, 224));				// Main Map Sprite Size
	this->addChild(wlayer);


	// ----- METALL CODE ADDED START -----
	// Health Bar Initializing
	playerHealth = 28.0f; // Default : 28

	EmptyHealthBar = Sprite::create("Megaman/UI/HealthBar_Empty.png");
	HealthBar = Sprite::create("Megaman/UI/HealthBar_Divide.png");	// Size = (11 x 56), Health = 28

	EmptyHealthBar->getTexture()->setAliasTexParameters();
	HealthBar->getTexture()->setAliasTexParameters();

	EmptyHealthBar->setPosition(15, 178);
	HealthBar->setPosition(15, 182);

	this->addChild(EmptyHealthBar, 40);
	this->addChild(HealthBar, 41);
	// ----- METALL CODE ADDED END -----

	isInited = false;
	atkAnimate = nullptr; // Initialize atkAnimate

	// Init enum states
	moveInputState = MS_INIT;
	attackInputState = DEFAULT_POSE;
	moveBeforeState = MS_PLACEHOLDER;
	attackBeforeState = ATK_PLACEHOLDER;
	chargeState = COMMON_BULLET; // Added back


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Load Tile Map (Tiled)

	tmap = TMXTiledMap::create("MainStage_Tileset/ruined_street_tileset.tmx");
	background = tmap->getLayer("Background");
	background->getTexture()->setAliasTexParameters();

	// Init Background Parallax
	auto skyImage = Sprite::create("MainStage_Tileset/Ruined_Street_Background.png");
	skyImage->setPosition(0, 0);
	skyImage->setAnchorPoint(Vec2(0.0f, 0.0f));
	skyImage->getTexture()->setAliasTexParameters();

	auto parallax = ParallaxNode::create();
	parallax->addChild(skyImage, 1, Vec2(0.86f, 1.0f), Vec2::ZERO);
	parallax->addChild(tmap, 2, Vec2(1.0f, 1.0f), Vec2::ZERO);

	wlayer->addChild(parallax, 0);

	// Init Player's Spawn Point
	auto spawnObjects = tmap->getObjectGroup("SpawnPoint");

	ValueMap spawnPoint = spawnObjects->getObject("SpawnPoint");
	Value objectval = Value(spawnPoint);

	float x = spawnPoint["x"].asFloat();
	float y = spawnPoint["y"].asFloat();

	spawnPosition = Vec2(x, y + 150.0f);


	// Init Map Collider (Ground)
	auto colliderObjects = tmap->getObjectGroup("Collider");
	auto colliders = colliderObjects->getObjects();
	Vector<Sprite*> _colliderSprite;

	for (int i = 0; i < colliders.size(); i++) {
		auto size = Size(colliders.data()[i].asValueMap()["width"].asFloat(), colliders.data()[i].asValueMap()["height"].asFloat());
		auto pos = Vec2(colliders.data()[i].asValueMap()["x"].asFloat(), colliders.data()[i].asValueMap()["y"].asFloat());

		auto backCollider = PhysicsBody::createBox(size, PHYSICSBODY_MATERIAL_DEFAULT);
		backCollider->setDynamic(false);
		backCollider->setCategoryBitmask(Utils::CreateMask(core::CategoryBits::PLATFORM));

		// ----- METALL CODE ADDED START -----
		// Restored original collision masks
		backCollider->setCollisionBitmask(Utils::CreateMask(core::CategoryBits::PLAYER
			, core::CategoryBits::ENEMY
			, core::CategoryBits::ENEMY_PROJECTILE));

		backCollider->setContactTestBitmask(Utils::CreateMask(core::CategoryBits::PLAYER_PROJECTILE
			, core::CategoryBits::ENEMY_PROJECTILE));  // 콜리전 이벤트
		// ----- METALL CODE ADDED END -----

		backCollider->setTag(core::TagIndex::PLATFORM);

		auto setcol = Sprite::create();
		setcol->setContentSize(size);
		setcol->setPosition(pos);
		setcol->setAnchorPoint(Vec2(0, 0));
		setcol->setPhysicsBody(backCollider);

		_colliderSprite.pushBack(setcol);
		wlayer->addChild(_colliderSprite.at(i));
	}

	// ----- METALL CODE ADDED START -----
	// Init Ground's DeadEnd (for Enemies)
	auto deadEndObjects = tmap->getObjectGroup("DeadEnd");
	auto deadEnds = deadEndObjects->getObjects();

	Vector<Sprite*> _deadEndSprite;

	for (int i = 0; i < deadEnds.size(); i++) {

		auto size = Size(deadEnds.data()[i].asValueMap()["width"].asFloat(), deadEnds.data()[i].asValueMap()["height"].asFloat());
		auto pos = Vec2(deadEnds.data()[i].asValueMap()["x"].asFloat(), deadEnds.data()[i].asValueMap()["y"].asFloat());

		auto deadEndCollider = PhysicsBody::createBox(size, PHYSICSBODY_MATERIAL_DEFAULT);
		deadEndCollider->setDynamic(false);
		deadEndCollider->setCategoryBitmask(Utils::CreateMask(core::CategoryBits::DEADEND));

		deadEndCollider->setCollisionBitmask(0);

		deadEndCollider->setContactTestBitmask(Utils::CreateMask(core::CategoryBits::ENEMY));

		deadEndCollider->setTag(core::TagIndex::DEADEND);

		auto settingdeadEnd = Sprite::create();
		settingdeadEnd->setContentSize(size);
		settingdeadEnd->setPosition(pos);
		settingdeadEnd->setAnchorPoint(Vec2(0, 0));
		settingdeadEnd->setPhysicsBody(deadEndCollider);

		_deadEndSprite.pushBack(settingdeadEnd);

		wlayer->addChild(_deadEndSprite.at(i));
	}
	// ----- METALL CODE ADDED END -----


	// All enemy, trigger, and boss colliders removed
	// ...

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Character Parent
	charLayer = Sprite::create("Placeholder.png");
	charLayer->setScale(1.0f);
	charLayer->setPosition(spawnPosition);
	charLayer->setAnchorPoint(Vec2(0.5, 0.25));
	charLayer->getTexture()->setAliasTexParameters();

	// Movement Sprite
	character = Sprite::create("init_teleport.png");
	character->setAnchorPoint(Vec2::ZERO);
	character->getTexture()->setAliasTexParameters();

	// Attack Sprite
	attackChar = Sprite::create("test_megaman.png");
	attackChar->setAnchorPoint(Vec2::ZERO);
	attackChar->getTexture()->setAliasTexParameters();

	// Player's Physics
	auto material1 = PhysicsMaterial(1.0f, 0.0f, 0.0f);
	characterBody = PhysicsBody::createBox(Size(32, 40), material1, Vec2(0.5f, 0.5f));

	characterBody->setMass(1.0f);
	characterBody->setRotationEnable(false);
	characterBody->setCategoryBitmask(Utils::CreateMask(core::CategoryBits::PLAYER));

	characterBody->setCollisionBitmask(Utils::CreateMask(core::CategoryBits::PLATFORM, core::CategoryBits::BOUNDARY)); // FIXED: Replaced | with ,

	// ----- METALL CODE ADDED START -----
	// Restored contact test mask for enemy collision
	characterBody->setContactTestBitmask(Utils::CreateMask(core::CategoryBits::ENEMY
		, core::CategoryBits::ENEMY_PROJECTILE
		, core::CategoryBits::ENEMY_AWAKE_SENSOR));
	// ----- METALL CODE ADDED END -----

	characterBody->setTag(core::TagIndex::PLAYER);
	characterBody->setGravityEnable(false); // Start disabled for spawn anim

	// Init bools
	fall = false;
	movingLeft = false;
	movingRight = false;
	movingUp = false;
	movingSliding = false;
	inputShoot = false;
	isInited = true;
	isChangeMoveSprite = false;

	// ----- METALL CODE ADDED START -----
	CollideEnemy = false;
	CollideEnemyBullet = false;
	isStuck = false;
	startStuckTimer = false;
	stuckTime = 0;
	contactingDamage = 0;
	// ----- METALL CODE ADDED END -----

	atkChargeTime = 0;
	x1Ready = false;
	x2Ready = false;

	// Init Player's Shooting Bullet Position
	shootPosition = Vec2(characterBody->getPosition().x - (atk_stop_pos[0][0] + 3), characterBody->getPosition().y + atk_stop_pos[0][1]);

	charLayer->setPhysicsBody(characterBody);
	charLayer->addChild(character);
	charLayer->addChild(attackChar);

	attackChar->setVisible(false);

	wlayer->addChild(charLayer, 3);

	// ----- METALL CODE ADDED START -----
	// Init Enemies's Array size
	metalls.reserve(10);

	// Spawn Enemies 1 (Metall)
	auto metallSpawnObjects = tmap->getObjectGroup("MetallSpawn");
	auto metallSpawnPoints = metallSpawnObjects->getObjects();
	metallsNum = metallSpawnPoints.size();

	for (int i = 0; i < metallsNum; i++) {
		auto pos = Vec2(metallSpawnPoints.data()[i].asValueMap()["x"].asFloat(), metallSpawnPoints.data()[i].asValueMap()["y"].asFloat());

		auto metall{ Enemy_Metall::create(wlayer, pos.x, pos.y) };
		metall->setPlayerPos(characterBody);
		metalls.emplace(i, metall);

		wlayer->addChild(metalls[i]);
	}
	log("metalls Size : %d", metalls.size());
	// ----- METALL CODE ADDED END -----

	// All enemy spawning removed
	// ...

	////////////////////////////////////////////////////////

	// Init Physics Contact(Collision) Event
	auto contact = EventListenerPhysicsContact::create();

	// Added back bullet collision logic for particles
	contact->onContactBegin = [this](PhysicsContact& contact) -> bool {
		auto nodeA = contact.getShapeA()->getBody()->getTag();
		auto nodeB = contact.getShapeB()->getBody()->getTag();

		// ----- METALL CODE ADDED START -----
		// Check for bullet (A) hitting platform (B) or Metall (B)
		if ((nodeA == core::TagIndex::COMMON_BULLET || nodeA == core::TagIndex::CHARGE_X1 || nodeA == core::TagIndex::CHARGE_X2) && (nodeB == core::TagIndex::PLATFORM || nodeB == core::TagIndex::METALL))
		{
			if (contact.getShapeA()->getBody()->getNode() == nullptr) return true;

			auto posX = contact.getContactData()->points->x;
			auto posY = contact.getShapeA()->getBody()->getPosition().y;
			auto world = wlayer->convertToNodeSpace(Vec2(posX, posY));

			if (nodeA == core::TagIndex::COMMON_BULLET) {
				contact.getShapeA()->getBody()->getNode()->removeFromParentAndCleanup(true);
				bulletParticleAnimState(COMMON_BULLET, world.x, world.y);

				if (nodeB == core::TagIndex::METALL) {
					if ((static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isAwake)) {
						static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isHit = true;
						static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->metallHealth -= 1.0f;
					}
					// else soundManager->PlayTinkEffect(soundManager->metallTinkPath); // Sound removed
				}
			}
			else if (nodeA == core::TagIndex::CHARGE_X1) {
				float targetHealth = 1;
				if (nodeB == core::TagIndex::METALL) {
					if ((static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isAwake)) {
						static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isHit = true;
						static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->metallHealth -= 2.0f;
					}
					// else soundManager->PlayTinkEffect(soundManager->metallTinkPath); // Sound removed
					targetHealth = static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->metallHealth;
				}

				if (targetHealth > 0) { // Only destroy bullet if enemy not killed
					contact.getShapeA()->getBody()->getNode()->removeFromParentAndCleanup(true);
					bulletParticleAnimState(CHARGE_X1, world.x, world.y);
				}
			}
			else if (nodeA == core::TagIndex::CHARGE_X2) {
				float targetHealth = 1;
				if (nodeB == core::TagIndex::METALL) {
					if ((static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isAwake)) {
						static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isHit = true;
						static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->metallHealth -= 4.0f;
					}
					// else soundManager->PlayTinkEffect(soundManager->metallTinkPath); // Sound removed
					targetHealth = static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->metallHealth;
				}

				if (targetHealth > 0) { // Only destroy bullet if enemy not killed
					contact.getShapeA()->getBody()->getNode()->removeFromParentAndCleanup(true);
					bulletParticleAnimState(CHARGE_X2, world.x, world.y);
				}
			}
		}
		// Check for platform (A) or Metall (A) hitting bullet (B)
		else if ((nodeA == core::TagIndex::PLATFORM || nodeA == core::TagIndex::METALL) && (nodeB == core::TagIndex::COMMON_BULLET || nodeB == core::TagIndex::CHARGE_X1 || nodeB == core::TagIndex::CHARGE_X2))
		{
			if (contact.getShapeB()->getBody()->getNode() == nullptr) return true;

			auto posX = contact.getContactData()->points->x;
			auto posY = contact.getShapeB()->getBody()->getPosition().y;
			auto world = wlayer->convertToNodeSpace(Vec2(posX, posY));

			if (nodeB == core::TagIndex::COMMON_BULLET) {
				contact.getShapeB()->getBody()->getNode()->removeFromParentAndCleanup(true);
				bulletParticleAnimState(COMMON_BULLET, world.x, world.y);

				if (nodeA == core::TagIndex::METALL) {
					if ((static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isAwake)) {
						static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isHit = true;
						static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->metallHealth -= 1.0f;
					}
					// else soundManager->PlayTinkEffect(soundManager->metallTinkPath); // Sound removed
				}
			}
			else if (nodeB == core::TagIndex::CHARGE_X1) {
				float targetHealth = 1;
				if (nodeA == core::TagIndex::METALL) {
					if ((static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isAwake)) {
						static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isHit = true;
						static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->metallHealth -= 2.0f;
					}
					// else soundManager->PlayTinkEffect(soundManager->metallTinkPath); // Sound removed
					targetHealth = static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->metallHealth;
				}

				if (targetHealth > 0) {
					contact.getShapeB()->getBody()->getNode()->removeFromParentAndCleanup(true);
					bulletParticleAnimState(CHARGE_X1, world.x, world.y);
				}
			}
			else if (nodeB == core::TagIndex::CHARGE_X2) {
				float targetHealth = 1;
				if (nodeA == core::TagIndex::METALL) {
					if ((static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isAwake)) {
						static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isHit = true;
						static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->metallHealth -= 4.0f;
					}
					// else soundManager->PlayTinkEffect(soundManager->metallTinkPath); // Sound removed
					targetHealth = static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->metallHealth;
				}

				if (targetHealth > 0) {
					contact.getShapeB()->getBody()->getNode()->removeFromParentAndCleanup(true);
					bulletParticleAnimState(CHARGE_X2, world.x, world.y);
				}
			}
		}

		// Metall bullet hitting platform
		if (nodeA == core::TagIndex::METALL_BULLET && nodeB == core::TagIndex::PLATFORM) {
			if (contact.getShapeA()->getBody()->getNode() == nullptr) return true;
			contact.getShapeA()->getBody()->getNode()->removeFromParentAndCleanup(true);
			auto posX = contact.getContactData()->points->x;
			auto posY = contact.getShapeA()->getBody()->getPosition().y;
			auto world = wlayer->convertToNodeSpace(Vec2(posX, posY));
			bulletParticleAnimState(COMMON_BULLET, world.x, world.y);
		}
		if (nodeB == core::TagIndex::METALL_BULLET && nodeA == core::TagIndex::PLATFORM) {
			if (contact.getShapeB()->getBody()->getNode() == nullptr) return true;
			contact.getShapeB()->getBody()->getNode()->removeFromParentAndCleanup(true);
			auto posX = contact.getContactData()->points->x;
			auto posY = contact.getShapeB()->getBody()->getPosition().y;
			auto world = wlayer->convertToNodeSpace(Vec2(posX, posY));
			bulletParticleAnimState(COMMON_BULLET, world.x, world.y);
		}

		// Metall sensor logic
		if (nodeA == core::TagIndex::AWAKE_SENSOR) {
			auto a = static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent()->getParent());
			a->innerSensor = true;
		}
		else if (nodeB == core::TagIndex::AWAKE_SENSOR) {
			auto b = static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent()->getParent());
			b->innerSensor = true;
		}

		// Metall DeadEnd logic
		if (nodeA == core::TagIndex::DEADEND && nodeB == core::TagIndex::METALL) {
			log("Contact DeadEnd");
			static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->isDeadEnd = true;
			static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent())->contactDeadEnd();
		}
		if (nodeB == core::TagIndex::DEADEND && nodeA == core::TagIndex::METALL) {
			log("Contact DeadEnd");
			static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->isDeadEnd = true;
			static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent())->contactDeadEnd();
		}

		// Player collision logic
		if (nodeA == core::TagIndex::PLAYER && (nodeB == core::TagIndex::METALL || nodeB == core::TagIndex::METALL_BULLET))
		{
			switch (nodeB)
			{
			case core::TagIndex::METALL:
				CollideEnemy = true;
				contactingDamage = core::EnemyDamage::METALL_DMG;
				break;
			case core::TagIndex::METALL_BULLET:
				CollideEnemyBullet = true;
				contactingDamage = core::EnemyDamage::METALL_BULLET_DMG;
				break;
			}
		}
		if (nodeB == core::TagIndex::PLAYER && (nodeA == core::TagIndex::METALL || nodeA == core::TagIndex::METALL_BULLET))
		{
			switch (nodeA)
			{
			case core::TagIndex::METALL:
				CollideEnemy = true;
				contactingDamage = core::EnemyDamage::METALL_DMG;
				break;
			case core::TagIndex::METALL_BULLET:
				CollideEnemyBullet = true;
				contactingDamage = core::EnemyDamage::METALL_BULLET_DMG;
				break;
			}
		}
		// ----- METALL CODE ADDED END -----

		return true;
		};

	// Added back bullet cleanup logic for screen boundary
	contact->onContactSeparate = [this](PhysicsContact& contact) -> bool {
		auto nodeA = contact.getShapeA()->getBody()->getTag();
		auto nodeB = contact.getShapeB()->getBody()->getTag();

		// ----- METALL CODE ADDED START -----
		// Added METALL_BULLET to boundary check
		if ((nodeA == core::TagIndex::COMMON_BULLET || nodeA == core::TagIndex::CHARGE_X1 || nodeA == core::TagIndex::CHARGE_X2 || nodeA == core::TagIndex::METALL_BULLET)
			&& nodeB == core::TagIndex::BOUNDARY && contact.getShapeA()->getBody()->getNode() != nullptr)
		{
			contact.getShapeA()->getBody()->getNode()->removeFromParentAndCleanup(true);
		}
		else if ((nodeB == core::TagIndex::COMMON_BULLET || nodeB == core::TagIndex::CHARGE_X1 || nodeB == core::TagIndex::CHARGE_X2 || nodeB == core::TagIndex::METALL_BULLET)
			&& nodeA == core::TagIndex::BOUNDARY && contact.getShapeB()->getBody()->getNode() != nullptr)
		{
			contact.getShapeB()->getBody()->getNode()->removeFromParentAndCleanup(true);
		}

		// Sensor logic
		if (nodeA == core::TagIndex::AWAKE_SENSOR) {
			auto a = static_cast<Enemy_Metall*>(contact.getShapeA()->getBody()->getNode()->getParent()->getParent());
			a->innerSensor = false;
		}
		else if (nodeB == core::TagIndex::AWAKE_SENSOR) {
			auto b = static_cast<Enemy_Metall*>(contact.getShapeB()->getBody()->getNode()->getParent()->getParent());
			b->innerSensor = false;
		}

		// Player collision logic
		if (nodeA == core::TagIndex::PLAYER && (nodeB == core::TagIndex::METALL || nodeB == core::TagIndex::METALL_BULLET))
		{
			if (nodeB == core::TagIndex::METALL) {
				CollideEnemy = false;
				contactingDamage = 0;
			}
			else if (nodeB == core::TagIndex::METALL_BULLET) {
				CollideEnemyBullet = false;
				contactingDamage = 0;
			}
		}
		if (nodeB == core::TagIndex::PLAYER && (nodeA == core::TagIndex::METALL || nodeA == core::TagIndex::METALL_BULLET))
		{
			if (nodeA == core::TagIndex::METALL) {
				CollideEnemy = false;
				contactingDamage = 0;
			}
			else if (nodeA == core::TagIndex::METALL_BULLET) {
				CollideEnemyBullet = false;
				contactingDamage = 0;
			}
		}
		// ----- METALL CODE ADDED END -----

		return true;
		};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(contact, wlayer);

	////////////////////////////////////////////////////////

	// Player's Charging Effect Shader // Added back
	GLchar* fragmentSource = (GLchar*)String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename("ChargeMode_Shader.fsh").c_str())->getCString();
	glProg = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragmentSource);
	glProgramState = GLProgramState::getOrCreateWithGLProgram(glProg);

	glProgramState->setUniformInt("chargeMode", 0);
	attackChar->setGLProgramState(glProgramState);


	// Sound preload/play removed
	// ...

	attackAnimState(DEFAULT_POSE); // Initialize atkAnimate

	this->scheduleUpdate();

	return true;
}

void TestScene::onEnter()
{
	Scene::onEnter();

	_listener = EventListenerKeyboard::create();
	_listener->onKeyPressed = CC_CALLBACK_2(TestScene::onKeyPressed, this);
	_listener->onKeyReleased = CC_CALLBACK_2(TestScene::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

	readyLogoAnim(); // Triggers spawn anim
}

void TestScene::onExit()
{
	_eventDispatcher->removeEventListener(_listener);

	Scene::onExit();
}

void TestScene::update(float dt)
{
	// ----- METALL CODE ADDED START -----
	HealthBar->setTextureRect(Rect(0, (2 * (28 - playerHealth)), 11, (56 - (2 * (28 - playerHealth)))));
	HealthBar->setPosition(15, 182 + (playerHealth - 28));
	// ----- METALL CODE ADDED END -----

	if (hideLogo && !readyGame) {
		// This logic handles the end of the spawn animation
		changeTime += dt;
		if (changeTime >= .02f && characterBody->getVelocity().y < 0.0001f && characterBody->getVelocity().y > -0.0001f) {
			moveAnimState(moveInputState);
			attackAnimState(attackInputState);
			characterBody->setCollisionBitmask(Utils::CreateMask(core::CategoryBits::PLATFORM
				, core::CategoryBits::BOUNDARY));
			changeTime = 0;
		}
	}
	else if (hideLogo && readyGame) {

		// Player death logic // REMOVED
		// ...

		// ----- METALL CODE ADDED START -----
		// Stuck logic
		if (CollideEnemy && !CollideEnemyBullet && !startStuckTimer || (!CollideEnemy && CollideEnemyBullet && !startStuckTimer)) {
			moveInputState = MS_HURT;
			moveAnimState(moveInputState);
			characterBody->setVelocity(Vec2(-1 * movingDir * 10, characterBody->getVelocity().y + 10));
		}

		if (startStuckTimer) {
			stuckTime += dt;

			if (fmod(stuckTime, 0.1f) < 0.05f) {
				character->setOpacity(100);
				attackChar->setOpacity(100);
			}
			else {
				character->setOpacity(255);
				attackChar->setOpacity(255);
			}

			if (stuckTime >= 1.5f) {
				stuckTime = 0;
				startStuckTimer = false;
				character->setOpacity(255);
				attackChar->setOpacity(255);
			}
		}

		if (isStuck) {
			startStuckTimer = true;
		}
		// ----- METALL CODE ADDED END -----

		// Charge logic // Added back
		if (inputShoot) {
			atkChargeTime += dt;
			if (0.85f < atkChargeTime && atkChargeTime <= 2.0f) {
				if (chargeState != CHARGE_X1) {
					chargeState = CHARGE_X1;
					log("Charge_X1 Ready");

					if (!x1Ready) {
						// soundManager->PlayAttackEffect(soundManager->startChargePath); // Sound removed
						x1Ready = true;
					}
				}

				if (fmod(atkChargeTime, 0.15f) < 0.1f)
					glProgramState->setUniformInt("chargeMode", 1);
				else glProgramState->setUniformInt("chargeMode", 0);
			}

			if (2.0f < atkChargeTime) {
				if (chargeState != CHARGE_X2) {
					chargeState = CHARGE_X2;
					log("Charge_X2 Ready");

					if (!x2Ready) {
						// soundManager->PlayAttackEffect(soundManager->chargingLoopPath); // Sound removed
						x2Ready = true;
					}
				}

				if (fmod(atkChargeTime, 0.1f) < 0.05f)
					glProgramState->setUniformInt("chargeMode", 2);
				else glProgramState->setUniformInt("chargeMode", 3);
			}
		}

		// Logic to switch back to move sprite after shooting
		if (!inputShoot && !isChangeMoveSprite) {
			changeTime += dt;

			if (!inputShoot && (changeTime > 0.18f)) {
				turnOnMoveSprite();
				changeTime = 0;
				isChangeMoveSprite = true;
			}
		}

		// Cutscene logic // REMOVED
		// ...

		// ----- METALL CODE ADDED START -----
		if (!isStuck) { // Added check
			// ----- METALL CODE ADDED END -----

				// --- Start Player Movement Logic ---

				// Ground Check
			if (characterBody->getVelocity().y < 0.0001f && characterBody->getVelocity().y > -0.0001f)
				fall = false;
			else fall = true;

			// Jump
			if (movingUp && !movingSliding) {
				jumpChargeTime += dt;
				if (jumpChargeTime > 0.2f) {
					isJumping = false;
					movingUp = false;
				}
			}

			if (movingUp && !fall && jumpChargeTime < 0.2f) {
				characterBody->setVelocity(Vec2(0, 50));
				isJumping = true;
				moveInputState = MS_JUMP;
				attackInputState = JUMP_SHOOT;
				attackAnimState(attackInputState);
				moveAnimState(moveInputState);
			}

			if (isJumping == true && jumpChargeTime < 0.2f) {
				characterBody->setVelocity(Vec2(0, 110));
			}

			if (fall && inputShoot) {
				turnOffMoveSprite();
			}

			// Fall
			if (fall && characterBody->getVelocity().y < -20.0f) {
				moveInputState = MS_FALL;
				attackInputState = FALL_SHOOT;
				characterBody->setVelocity(Vec2(characterBody->getVelocity().x, (characterBody->getVelocity().y) * 1.05f));
				attackAnimState(attackInputState);
				moveAnimState(moveInputState);
				movingSliding = false;
			}

			// Left Move
			if (movingLeft && !movingRight) {
				if (movingDir == -1 && !character->isFlippedX()) {
					flippedCharacter(true);
					movingSliding = false;
				}
				else characterBody->setVelocity(Vec2(-50, characterBody->getVelocity().y));

				if (!movingUp && !fall && !movingSliding) {
					moveInputState = MS_RUN;
					attackInputState = RUN_SHOOT;
					attackAnimState(attackInputState);
					moveAnimState(moveInputState);
					if (inputShoot)
						turnOffMoveSprite();
				}
			}

			// Right Move
			if (movingRight && !movingLeft) {
				if (movingDir == 1 && character->isFlippedX()) {
					flippedCharacter(false);
					movingSliding = false;
				}
				else characterBody->setVelocity(Vec2(50, characterBody->getVelocity().y));

				if (!movingUp && !fall & !movingSliding) {
					moveInputState = MS_RUN;
					attackInputState = RUN_SHOOT;
					attackAnimState(attackInputState);
					moveAnimState(moveInputState);
					if (inputShoot)
						turnOffMoveSprite();
				}
			}

			// Stop
			if ((!movingLeft && !movingRight && !movingSliding) || (movingLeft && movingRight)) {
				characterBody->setVelocity(Vec2(0, characterBody->getVelocity().y));
				movingSliding = false;
				if (!movingUp && !fall) {
					moveInputState = MS_STOP;
					moveAnimState(moveInputState);
					attackInputState = DEFAULT_POSE;
					attackAnimState(attackInputState);

					if (inputShoot) {
						attackInputState = STOP_SHOOT;
						turnOffMoveSprite();
						attackAnimState(attackInputState);
					}
				}
			}

			// Sliding
			if (movingSliding && !movingUp && !fall) {
				if (character->isFlippedX())
					characterBody->setVelocity(Vec2(-85, characterBody->getVelocity().y));
				else characterBody->setVelocity(Vec2(85, characterBody->getVelocity().y));

				if (!movingUp && !fall) {
					moveInputState = MS_SLIDING;
					moveAnimState(moveInputState);
				}
			}

			// --- End Player Movement Logic ---


			// Shooting Position by each frames (Kept for attack anim alignment)
			if (atkAnimate) { // Safety check
				auto frameNum = (atkAnimate->getCurrentFrameIndex());
				auto nodePosition = wlayer->convertToNodeSpace(characterBody->getPosition());

				if (character->isFlippedX()) {
					if (attackInputState == STOP_SHOOT || attackInputState == DEFAULT_POSE) {
						shootPosition = Vec2(nodePosition.x - (atk_stop_pos[0][0] + 3), characterBody->getPosition().y + atk_stop_pos[0][1]);
					}
					else if (attackInputState == JUMP_SHOOT) {
						shootPosition = Vec2(nodePosition.x - (atk_jump_pos[frameNum][0] + 3), nodePosition.y + atk_jump_pos[frameNum][1]);
					}
					else if (attackInputState == FALL_SHOOT) {
						shootPosition = Vec2(nodePosition.x - (atk_fall_pos[frameNum][0] + 3), nodePosition.y + atk_fall_pos[frameNum][1]);
					}
					else if (attackInputState == RUN_SHOOT) {
						shootPosition = Vec2(nodePosition.x - (atk_run_pos[frameNum][0] + 3), nodePosition.y + atk_run_pos[frameNum][1]);
					}
				}
				else {
					if (attackInputState == STOP_SHOOT || attackInputState == DEFAULT_POSE) {
						shootPosition = Vec2(nodePosition.x + (atk_stop_pos[0][0] + 3), nodePosition.y + atk_stop_pos[0][1]);
					}
					else if (attackInputState == JUMP_SHOOT) {
						shootPosition = Vec2(nodePosition.x + (atk_jump_pos[frameNum][0] + 3), nodePosition.y + atk_jump_pos[frameNum][1]);
					}
					else if (attackInputState == FALL_SHOOT) {
						shootPosition = Vec2(nodePosition.x + (atk_fall_pos[frameNum][0] + 3), nodePosition.y + atk_fall_pos[frameNum][1]);
					}
					else if (attackInputState == RUN_SHOOT) {
						shootPosition = Vec2(nodePosition.x + (atk_run_pos[frameNum][0] + 3), nodePosition.y + atk_run_pos[frameNum][1]);
					}
				}
			}

			// ----- METALL CODE ADDED START -----
		} // end if(!isStuck)
		// ----- METALL CODE ADDED END -----

		// Follow Camera Setting
		// Simplified camera logic
		auto follow = Follow::create(charLayer, Rect(0, 0, 2304, 224)); // FIXED: Set to full map width
		wlayer->runAction(follow);

		// ----- METALL CODE ADDED START -----
		// Update Enemies
		for (int i = 0; i < metallsNum; i++) {
			metalls[i]->update(dt);
		}
		// ----- METALL CODE ADDED END -----
	}

	// Restart logic // REMOVED
	// ...
}

void TestScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	if (hideLogo && readyGame) { // Removed boss dead check
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_SPACE:
			if (!readySlide) {
				if (jumpChargeTime <= 0.3f) {
					movingUp = true;
					movingSliding = false;
				}
			}
			else if (readySlide && !fall && !inputShoot) {
				movingSliding = true;
			}
			break;

		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			readySlide = true;
			break;

		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			movingLeft = true;
			movingDir = -1;
			if (movingDir != -1)
				movingSliding = false;

			break;

		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			movingRight = true;
			movingDir = 1;
			if (movingDir != 1)
				movingSliding = false;

			break;

		case EventKeyboard::KeyCode::KEY_X:
			// ----- METALL CODE ADDED START -----
			if (!isStuck) { // Added check
				// ----- METALL CODE ADDED END -----
					// Updated to create bullet
				inputShoot = true;
				if (!movingSliding)
					createBullet(COMMON_BULLET);
				// ----- METALL CODE ADDED START -----
			}
			// ----- METALL CODE ADDED END -----
			break;
		}
	}
}

void TestScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	if (hideLogo && readyGame) { // Removed teleport check
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_SPACE:
			movingUp = false;
			isJumping = false;
			jumpChargeTime = 0;
			break;

		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			readySlide = false;
			break;

		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			movingLeft = false;
			moveInputState = MS_STOP;
			break;

		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			movingRight = false;
			moveInputState = MS_STOP;
			break;

		case EventKeyboard::KeyCode::KEY_X:
			// Updated to release charged shot and reset
			if (chargeState != COMMON_BULLET && !isStuck) { // Added !isStuck
				createBullet(chargeState);
			}
			glProgramState->setUniformInt("chargeMode", 0);
			chargeState = COMMON_BULLET;
			inputShoot = false;
			isChangeMoveSprite = false;
			changeTime = 0;
			atkChargeTime = 0;
			x1Ready = false;
			x2Ready = false;
			break;
		}
	}
}

void TestScene::moveAnimState(_moveState state)
{
	int allSheetNum;
	std::string sPath;
	std::string sName;
	float frameDelay = 0.075f;

	if (state != moveBeforeState) {

		if (isInited)
			character->stopAllActions();

		switch (state)
		{
		case MS_INIT: // Spawn animation
			allSheetNum = 6;
			sPath = "Megaman/Animation/Megaman_Teleport_Init.plist";
			sName = "teleport_";
			frameDelay = 0.05f;
			break;

			// MS_TELEPORT removed

		case MS_STOP:
			allSheetNum = 70;
			sPath = "Megaman/Animation/Megaman_Idle.plist";
			sName = "idle_";
			frameDelay = 0.12f;
			break;
		case MS_RUN:
			allSheetNum = 10;
			sPath = "Megaman/Animation/Megaman_Run.plist";
			sName = "run_";
			frameDelay = 0.07f;
			break;
		case MS_JUMP:
			allSheetNum = 5;
			sPath = "Megaman/Animation/Megaman_Jump.plist";
			sName = "jump_";
			frameDelay = 0.05f;
			break;
		case MS_SLIDING:
			allSheetNum = 3;
			sPath = "Megaman/Animation/Megaman_Sliding.plist";
			sName = "sliding_";
			break;
		case MS_FALL:
			allSheetNum = 5;
			sPath = "Megaman/Animation/Megaman_Fall.plist";
			sName = "fall_";
			frameDelay = 0.1f;
			break;
		case MS_HURT: // Animation exists, but won't be called
			allSheetNum = 5;
			sPath = "Megaman/Animation/Megaman_Hurt.plist";
			sName = "hurt_";
			frameDelay = 0.05f;
			break;
		default:
			// Do nothing if state is not recognized
			return;
		}

		/////////////////////////////////////////////////////////////////////////////////

		auto cache = SpriteFrameCache::getInstance();
		cache->addSpriteFramesWithFile(sPath);

		Vector<SpriteFrame*> animFrames;

		for (int i = 0; i < allSheetNum; i++) {
			std::string _frames = StringUtils::format("%s%02d.png", sName.c_str(), i);
			SpriteFrame* frame = cache->getSpriteFrameByName(_frames);
			if (!frame) continue; // Safety check
			frame->getTexture()->setAliasTexParameters();
			animFrames.pushBack(frame);
		}

		if (animFrames.empty()) return; // Don't crash if frames are missing

		auto animation = Animation::createWithSpriteFrames(animFrames, frameDelay);
		auto animate = Animate::create(animation);

		if (state == MS_STOP || state == MS_RUN) {
			auto runRep = RepeatForever::create(animate);
			character->runAction(runRep);
		}
		else if (state == MS_INIT) {
			// This is the spawn-in animation
			auto callback = CallFunc::create(this, callfunc_selector(TestScene::setReadyGame));
			auto seq = Sequence::create(animate->reverse(), callback, nullptr);
			character->runAction(seq);
		}
		// ----- METALL CODE ADDED START -----
		else if (state == MS_HURT) {
			isStuck = true;
			turnOnMoveSprite();
			playerHealth -= (float)contactingDamage;
			auto callback = CallFunc::create(this, callfunc_selector(TestScene::setOffStuck));
			auto seq = Sequence::create(animate, DelayTime::create(0.1f), callback, nullptr);
			character->runAction(seq);
		}
		// ----- METALL CODE ADDED END -----
		else {
			character->runAction(animate);
		}

		// Sound effects removed
		// ...
	}

	moveBeforeState = state;
}

void TestScene::attackAnimState(_attackState state)
{
	int allSheetNum;
	std::string sPath;
	std::string sName;
	float frameDelay;

	if (state != attackBeforeState || state == STOP_SHOOT) {
		if (isInited)
			attackChar->stopAllActions();

		// Handle DEFAULT_POSE to initialize atkAnimate safely
		if (state == DEFAULT_POSE) {
			if (!atkAnimate) { // Only initialize if it's null
				allSheetNum = 3;
				sPath = "Megaman/Animation/Megaman_Shoot.plist";
				sName = "shoot_";
				frameDelay = 0.0333f;

				auto cache = SpriteFrameCache::getInstance();
				cache->addSpriteFramesWithFile(sPath);
				Vector<SpriteFrame*> animFrames;
				for (int i = 0; i < allSheetNum; i++) {
					std::string _frames = StringUtils::format("%s%02d.png", sName.c_str(), i);
					SpriteFrame* frame = cache->getSpriteFrameByName(_frames);
					if (!frame) continue;
					frame->getTexture()->setAliasTexParameters();
					animFrames.pushBack(frame);
				}
				if (animFrames.empty()) return;

				auto atkAnimation = Animation::createWithSpriteFrames(animFrames, frameDelay);
				atkAnimate = Animate::create(atkAnimation);
				atkAnimate->retain();
			}
			attackBeforeState = state;
			return; // Don't run any action
		}


		switch (state)
		{
			// DEFAULT_POSE is handled above
		case STOP_SHOOT:
			allSheetNum = 3;
			sPath = "Megaman/Animation/Megaman_Shoot.plist";
			sName = "shoot_";
			frameDelay = 0.0333f;
			break;

		case RUN_SHOOT:
			allSheetNum = 10;
			sPath = "Megaman/Animation/Megaman_Run_Shoot.plist";
			sName = "run_shoot_";
			frameDelay = 0.07f;
			break;
		case JUMP_SHOOT:
			allSheetNum = 5;
			sPath = "Megaman/Animation/Megaman_Jump_Shoot.plist";
			sName = "jump_shoot_";
			frameDelay = 0.05f;
			break;
		case FALL_SHOOT:
			allSheetNum = 5;
			sPath = "Megaman/Animation/Megaman_Fall_Shoot.plist";
			sName = "fall_shoot_";
			frameDelay = 0.1f;
			break;
		default:
			return;
		}

		auto cache = SpriteFrameCache::getInstance();
		cache->addSpriteFramesWithFile(sPath);

		Vector<SpriteFrame*> animFrames;

		for (int i = 0; i < allSheetNum; i++) {
			std::string _frames = StringUtils::format("%s%02d.png", sName.c_str(), i);
			SpriteFrame* frame = cache->getSpriteFrameByName(_frames);
			if (!frame) continue;
			frame->getTexture()->setAliasTexParameters();
			animFrames.pushBack(frame);
		}

		if (animFrames.empty()) return;

		auto atkAnimation = Animation::createWithSpriteFrames(animFrames, frameDelay);
		// Stop and release previous animation before replacing
		if (atkAnimate) {
			atkAnimate->stop();
			atkAnimate->release();
		}
		atkAnimate = Animate::create(atkAnimation);
		atkAnimate->retain();


		if (state == STOP_SHOOT) {
			auto callback = CallFunc::create(this, callfunc_selector(TestScene::turnOnMoveSprite));
			auto seq = Sequence::create(atkAnimate, callback, nullptr);
			attackChar->runAction(seq);
		}
		else if (state == RUN_SHOOT) {
			auto runRep = RepeatForever::create(atkAnimate);
			attackChar->runAction(runRep);
		}
		else if (state == JUMP_SHOOT || state == FALL_SHOOT) {
			attackChar->runAction(atkAnimate);
		}

		attackBeforeState = state;
	}
}

// bulletParticleAnimState // Added back
void TestScene::bulletParticleAnimState(_bullets state, float x, float y)
{
	int allSheetNum;
	std::string sPath;
	std::string sName;
	float frameDelay = 0.05f;
	float particleScale = 1.0f;

	switch (state)
	{
	case COMMON_BULLET:
		allSheetNum = 4;
		sPath = "Megaman/Particle/Particle_Explosion_Small.plist";
		sName = "explosion_small_";
		particleScale = 0.9f;
		break;

	case CHARGE_X1:
		allSheetNum = 5;
		sPath = "Megaman/Particle/Particle_Charge_Affect.plist";
		sName = "charge_affect_";
		particleScale = 0.5f;
		break;
	case CHARGE_X2:
		allSheetNum = 5;
		sPath = "Megaman/Particle/Particle_Charge_Affect.plist";
		sName = "charge_affect_";
		break;
	}

	// Ã¦Ââ€ ÃÂ¦ÂÂÃ¨Â¡Â¤ ÃÂ¥Â¼ Ã¬ÂÂ©Ã¬ÂÂ©ÃÂ¥ÂËÃÂ©Â° ÃÂ£Â¬Ã«Â¡ÂÅ plist ÃÂÆÂÄÃ¬ÂÂ¼ Ã«Â¶ÂÂÃ«ÂÂ¤Ã¬ÂËÂ¤ÃªÂ¸Â°
	auto cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;

	for (int i = 0; i < allSheetNum; i++) {

		// StringUtils::format => Ã¬Â§ÂÃ¬Â ÂÂÃÂ£Â¬ ÃÂ£ÂÂÃ¬ÂÂÂÃ¬ÂÂ¸ÂÃ«Â¡ÂÅ Ã«Â¬Â¸Ã¬Â ÂÂÃ¬Â ÂÂ Ã¬ÂÂÂÃ¬ÂÂ Â
		std::string _frames = StringUtils::format("%s%02d.png", sName.c_str(), i);

		// Ã¬ÂÂÂÃ¬ÂÂ ÂÃÂ£Â¬ Ã«Â¬Â¸Ã¬Â ÂÂÃ¬Â ÂÂ Ã¬ÂÂÃ¬ÂÂ©ÃÂ¥ÂÃÂ£Â¬Ã¬ÂÂ¸Â plist Ã«ÂÂ´Ã«Â¶ÂÂÃ¬ÂÂ SpriteFrame Ã¬Â ÂÂÃ«ÂBCÂ´Ã«Â¥Â¼ ÃªÂ°ÂÂÃ¬Â ÂÂ¤Ã¬ÂËÂ¤ÃªÂ¸Â°
		SpriteFrame* frame = cache->getSpriteFrameByName(_frames);
		if (!frame) continue;
		frame->getTexture()->setAliasTexParameters();
		// Ã¬Â Â Ã«Â³Â ÃÂ£Â¬ SpriteFrameÃ¬Â ÂÂ Ã¬ÂÂ½Ã¬ÂÂ

		animFrames.pushBack(frame);
	}
	if (animFrames.empty()) return;

	// plist ÃªÂ¸Â°Ã«Â°ÂÂÃ¬ÂÂ¼Â· Ã«Â§ÂÅÃ«ÂÂ  SpriteFrame Ã¬Â ÂÂÃ«Â³Â´Ã«Â¥Â¼ ÃÂ£ÂÂÃ«Â¡Â©ÃÂ£Â¬Ã¬ÂÂ¸Â Ã¬ÂÂ¸Ã¬ÂÂ¤Ã­ÂÂ°Ã¬ÂÂ¤ Ã¬ÂÂÂÃ¬ÂÂ Â
	auto animation = Animation::createWithSpriteFrames(animFrames, frameDelay);
	//auto atkAnimation = Animation::createWithSpriteFrames(animFrames, frameDelay);
	auto animate = Animate::create(animation);

	auto particleBullet = Sprite::create();
	particleBullet->setPosition(x, y);
	//log("%f, %f", x, y);
	//particleBullet->setAnchorPoint(Vec2(0.75f, 0.5f));
	particleBullet->setScale(particleScale);
	auto seq = Sequence::create(animate, RemoveSelf::create(), nullptr);
	wlayer->addChild(particleBullet);

	particleBullet->runAction(seq);
}


void TestScene::readyLogoAnim()
{
	// Instead of showing the "READY" logo,
	// just immediately call hideReadyLogo() to trigger the spawn anim.
	hideReadyLogo();
}

// getPlayerWorldPos // REMOVED
// ...

void TestScene::createMyPhysicsWorld()
{
	auto visibleSize = _director->getVisibleSize();
	Vect gravity = Vect(0.0f, -300.0f);
	m_world = this->getPhysicsWorld();
	// m_world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); // Uncomment for debugging
	m_world->setGravity(gravity);
	m_world->setSpeed(2.0f);
	m_world->setSubsteps(60);

	auto body = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 1);
	body->setCategoryBitmask(Utils::CreateMask(core::CategoryBits::BOUNDARY));
	// ----- METALL CODE ADDED START -----
	// Restored collisions
	body->setCollisionBitmask(Utils::CreateMask(core::CategoryBits::PLAYER
		, core::CategoryBits::ENEMY
		, core::CategoryBits::ENEMY_PROJECTILE));
	body->setContactTestBitmask(Utils::CreateMask(core::CategoryBits::PLAYER_PROJECTILE
		, core::CategoryBits::ENEMY_PROJECTILE));
	// ----- METALL CODE ADDED END -----
	body->setTag(core::TagIndex::BOUNDARY);

	auto edgeNode = Node::create();
	edgeNode->setPosition(Vec2(visibleSize) * 0.5f);
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);
}

void TestScene::turnOnMoveSprite()
{
	if (!(character->isVisible())) {
		character->setVisible(true);
		attackChar->setVisible(false);
	}
}

void TestScene::turnOffMoveSprite()
{
	if (character->isVisible()) {
		character->setVisible(false);
		attackChar->setVisible(true);
	}
}

void TestScene::setReadyGame()
{
	readyGame = true;
	moveInputState = MS_STOP;
}

void TestScene::hideReadyLogo()
{
	// readyLogoSprite->setVisible(false); // Sprite removed
	characterBody->setGravityEnable(true);
	// soundManager->PlayMovementEffect(soundManager->warpDownPath); // Sound removed
	hideLogo = true;

	// Manually trigger the spawn animation
	moveInputState = MS_INIT;
	moveAnimState(moveInputState);
}

// ----- METALL CODE ADDED START -----
void TestScene::setOffStuck()
{
	isStuck = false;
}
// ----- METALL CODE ADDED END -----

// playTeleportSound // REMOVED
// ...
// playerDie // REMOVED
// ...
// pauseGame // REMOVED
// ...
// resumeGame // REMOVED
// ...
// restartGame // REMOVED
// ...

void TestScene::flippedCharacter(bool state)
{
	character->setFlippedX(state);
	attackChar->setFlippedX(state);
}

// startBossStage // REMOVED
// ...
// endGame // REMOVED
// ...
// setTeleportVelocity // REMOVED
// ...
// replaceEndScene // REMOVED
// ...

// createBullet // Added back
void TestScene::createBullet(_bullets state)
{
	int allSheetNum;
	std::string sPath;
	std::string sName;
	Size size;
	float frameDelay = 0.05f; // Default for charge shots

	switch (state)
	{
	case COMMON_BULLET:
		allSheetNum = 3;
		sPath = "Megaman/Weapon/Weapon_Bullet.png";
		sName = "shooting_";
		size = Size(10.0f, 9.0f);
		break;

	case CHARGE_X1:
		allSheetNum = 4;
		sPath = "Megaman/Weapon/Charge_X1_Bullet.plist";
		sName = "charge_x1_bullet_";
		size = Size(22.0f, 15.0f);
		break;

	case CHARGE_X2:
		allSheetNum = 6;
		sPath = "Megaman/Weapon/Charge_X2_Bullet.plist";
		sName = "charge_x2_bullet_";
		size = Size(33.0f, 27.0f);
		break;
	default:
		return; // Should not happen
	}

	// Ã­ÂÂÃ­ÂÂ Ã¬ÂÂÂÃ¬ÂÂ Â
	auto pBullet = new Sprite();
	auto bulletPBody = PhysicsBody::createBox(size, PHYSICSBODY_MATERIAL_DEFAULT);
	bulletPBody->setGravityEnable(false);

	if (state == COMMON_BULLET) {
		pBullet = Sprite::create(sPath);
		if (!pBullet) return;
		pBullet->setPosition(shootPosition);
		bulletPBody->setTag(core::TagIndex::COMMON_BULLET);
		// soundManager->PlayAttackEffect(soundManager->bulletPath); // Sound removed
	}
	else if (state == CHARGE_X1 || state == CHARGE_X2) {
		// Ã¦Ââ€ ÃÂ¦ÂÂÃ¨Â¡Â¤ ÃÂ¥Â¼ Ã¬ÂÂ©Ã¬ÂÂ©ÃÂ¥ÂËÃÂ©Â° ÃÂ£Â¬Ã«Â¡ÂÅ plist ÃÂÆÂÄÃ¬ÂÂ¼ Ã«Â¶ÂÂÃ«ÂÂ¤Ã¬ÂËÂ¤ÃªÂ¸Â°
		auto cache = SpriteFrameCache::getInstance();
		cache->addSpriteFramesWithFile(sPath);

		Vector<SpriteFrame*> animFrames;

		for (int i = 0; i < allSheetNum; i++) {
			std::string _frames = StringUtils::format("%s%02d.png", sName.c_str(), i);
			SpriteFrame* frame = cache->getSpriteFrameByName(_frames);
			if (!frame) continue;
			frame->getTexture()->setAliasTexParameters();
			animFrames.pushBack(frame);
		}
		if (animFrames.empty()) return;

		// plist ÃªÂ¸Â°Ã«Â°ÂÂÃ¬ÂÂ¼Â· Ã«Â§ÂÅÃ«ÂÂ  SpriteFrame Ã¬Â ÂÂÃ«Â³Â´Ã«Â¥Â¼ ÃÂ£ÂÂÃ«Â¡Â©ÃÂ£Â¬Ã¬ÂÂ¸Â Ã¬ÂÂ¸Ã¬ÂÂ¤Ã­ÂÂ°Ã¬ÂÂ¤ Ã¬ÂÂÂÃ¬ÂÂ Â
		auto Animation = Animation::createWithSpriteFrames(animFrames, frameDelay);
		auto bulletAnim = Animate::create(Animation);
		bulletAnim->retain();

		pBullet = Sprite::create("Placeholder.png");
		if (!pBullet) return;
		pBullet->setPosition(shootPosition);

		if (state == CHARGE_X1) {
			bulletPBody->setTag(core::TagIndex::CHARGE_X1);
			// soundManager->PlayAttackEffect(soundManager->chargeX1Path); // Sound removed
		}
		else if (state == CHARGE_X2) {
			bulletPBody->setTag(core::TagIndex::CHARGE_X2);
			// soundManager->PlayAttackEffect(soundManager->chargeX2Path); // Sound removed
		}

		pBullet->runAction(bulletAnim);
	}

	bulletPBody->setCategoryBitmask(Utils::CreateMask(core::CategoryBits::PLAYER_PROJECTILE));
	bulletPBody->setCollisionBitmask(0);

	// ----- METALL CODE ADDED START -----
	// Updated bitmask to check for boundary, platforms, and enemies
	bulletPBody->setContactTestBitmask(Utils::CreateMask(core::CategoryBits::BOUNDARY
		, core::CategoryBits::PLATFORM
		, core::CategoryBits::ENEMY));
	// ----- METALL CODE ADDED END -----

	pBullet->setPhysicsBody(bulletPBody);

	wlayer->addChild(pBullet);

	// Ã¬ÂÂ¡Ã¬ÂÂ Â Ã¬Â ÂÂÃªÂµÂ­
	if (character->isFlippedX()) {
		pBullet->setFlippedX(true);
		bulletPBody->setVelocity(Vec2(-175.0f, 0.0f));
	}

	else {
		pBullet->setFlippedX(false);
		bulletPBody->setVelocity(Vec2(175.0f, 0.0f));
	}
}

