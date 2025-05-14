/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

 /**
  * Brings cocos2d and all Geode namespaces to the current scope.
  */
using namespace geode::prelude;

enum loopType {
	robtop, optimized, boilerplate
};

class $modify(BSLPauseLayer, EditorPauseLayer) {
	/*
	* Create a spawn trigger with various properties
	*/
	void createSpawnTrigger(bool isSpawnTriggered, bool isMultiTriggered, int group, 
		int targetGroupID, float spawnTriggerDelay, bool spawnOrdered, CCPoint p, bool isTouchTriggered) {
		GameObject* obj = m_editorLayer->createObject(1268, p, false);
		if (obj->m_objectID==1268) {
			SpawnTriggerGameObject* spwn = static_cast<SpawnTriggerGameObject*>(obj);
			spwn->m_isSpawnTriggered = isSpawnTriggered;
			spwn->m_isMultiTriggered = isMultiTriggered;
			if (group != 0) {
				spwn->addToGroup(group);
			}
			spwn->m_targetGroupID = targetGroupID;
			spwn->m_spawnDelay = spawnTriggerDelay;
			spwn->m_spawnOrdered = spawnOrdered;
			spwn->m_isTouchTriggered = isTouchTriggered;
			spwn->m_shouldPreview = isTouchTriggered;
		}
	}
	/*
	* Create a stop trigger with various properties
	*/
	void createStopTrigger(int targetGroupID, CCPoint p) {
		GameObject* obj = m_editorLayer->createObject(1616, p, false);
		if (obj->m_objectID == 1616) {
			EffectGameObject* stop = static_cast<EffectGameObject*>(obj);
			stop->m_targetGroupID = targetGroupID;
			stop->m_isTouchTriggered = true;
			stop->m_shouldPreview = true;
		}
	}
	/*
	* Create a spawn loop
	* can create any type of spawn loop
	*/
	void loopCreate(cocos2d::CCObject* sender, loopType lt) {
		// if you want the traditional robtop spawn loop
		if (lt == loopType::robtop) {
			EditorPauseLayer::onCreateLoop(sender);
			return;
		}
		// otherwise...
		// get the editor UI and the next free group ID
		EditorUI* ui = m_editorLayer->m_editorUI;
		int id = m_editorLayer->getNextFreeGroupID(CCArray::create());

		// get the minimum x coordinate of the loop, and the y coordinate of that object
		float minX = 0;
		float y = 0;
		bool flagMin = false;
		bool flagMax = false;
		float maxX = 0;
		float delay = 0;
		bool lastObjTrigger = false;
		// also add each trigger to the next free group, and set them to spawn-triggered and multi-triggered
		for (int i = 0; i < ui->getSelectedObjects()->count(); i++) {
			GameObject* obj = static_cast<GameObject*>(ui->getSelectedObjects()->objectAtIndex(i));
			if (obj->m_classType == GameObjectClassType::Effect) {
				EffectGameObject* effectObj = static_cast<EffectGameObject*>(obj);
				effectObj->m_isSpawnTriggered = true;
				effectObj->m_isMultiTriggered = true;
				effectObj->addToGroup(id);
			}
			// get the minimum x coordinate
			// and the highest y coordinate at that position
			if (!flagMin || obj->getPositionX() < minX) {
				minX = obj->getPositionX();
				y = obj->getPositionY();
				flagMin = true;
			}
			else if (obj->getPositionX() == minX) {
				if (obj->getPositionY() > y) {
					y = obj->getPositionY();
				}
			}
			// get the maximum x coordinate
			if (!flagMax || obj->getPositionX() > maxX) {
				maxX = obj->getPositionX();
				flagMax = true;
				if (obj->m_classType == GameObjectClassType::Effect) {
					if (obj->m_objectID == 1006) {
						EffectGameObject* effectObj = static_cast<EffectGameObject*>(obj);
						delay = effectObj->m_fadeInDuration;
						delay += effectObj->m_holdDuration;
						delay += effectObj->m_fadeOutDuration;
					}
					else {
						delay = static_cast<EffectGameObject*>(obj)->m_duration;
					}
				}
			}
		}

		// calculate the delay
		// 1 second is 10*30 + 5*2 + 3*0.5 + 1*0.1=311.6
		// 1 unit is 0.0032 seconds
		// 5/1558 seconds for more accuracy
		// spawn triggers only go up to 4-digit accuracy
		delay += (0.0032 * (maxX - minX));
		CCPoint p;

		// set up the first spawn trigger
		p.setPoint(minX, y + 60);
		createSpawnTrigger(true, true, id, id, delay, true, p, false);

		// set up the second spawn trigger
		bool boilerplate = (lt == loopType::boilerplate);
		p.setPoint(minX, y + 90);
		if (!boilerplate) {
			createSpawnTrigger(false, false, 0, id, 0, true, p, false);
		}
		else {
			int id2 = m_editorLayer->getNextFreeGroupID(CCArray::create());
			createSpawnTrigger(true, true, id2, id, 0, true, p, false);
			// set up the third spawn trigger
			// for initialization
			p.setPoint(minX, y + 120);
			createSpawnTrigger(false, false, 0, id2, 0, false, p, true);
			// set up a color trigger
			// to showcase the delay
			p.setPoint(minX, y - 30);
			GameObject* obj = m_editorLayer->createObject(899, p, false);
			if (obj->m_objectID == 899) {
				EffectGameObject* col = static_cast<EffectGameObject*>(obj);
				col->m_duration = delay;
				col->m_isTouchTriggered = true;
				col->m_shouldPreview = false;
			}
			// set up the stop triggers
			p.setPoint(minX - 30, y + 90);
			createStopTrigger(id, p);
			p.setPoint(minX - 30, y + 120);
			createStopTrigger(id2, p);
		}
	}
	
	// used for button callbacks
	void optLoop(cocos2d::CCObject* sender) {
		loopCreate(sender, loopType::optimized);
	}
	void bpLoop(cocos2d::CCObject* sender) {
		loopCreate(sender, loopType::boilerplate);
	}

	// HOOKS: 
	// create a spawn loop
	void onCreateLoop(cocos2d::CCObject* sender) {
		bool buttonMode = (Mod::get()->getSettingValue<std::string>("loopVersion")) == "robtop spawn loop"
			|| (Mod::get()->getSettingValue<std::string>("loopVersion")) == "separate buttons";
		if (buttonMode) {
			loopCreate(sender, loopType::robtop);
		}
		else if ((Mod::get()->getSettingValue<std::string>("loopVersion")) == "minimal spawn loop") {
			loopCreate(sender, loopType::optimized);
		}
		else {
			loopCreate(sender, loopType::boilerplate);
		}
	}

	// initialize the editorPauseLayer
	bool init(LevelEditorLayer* layer) {
		if (!EditorPauseLayer::init(layer)) {
			return false;
		}
		// if you want separate buttons, add them
		if ((Mod::get()->getSettingValue<std::string>("loopVersion")) == "separate buttons") {
			ButtonSprite* spr1 = ButtonSprite::create("minimal spawn loop", 30, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.3f);
			CCMenuItemSpriteExtra* btn1 = CCMenuItemSpriteExtra::create(
				spr1, this, menu_selector(BSLPauseLayer::optLoop)
			);
			btn1->setID("minimal_spawn_loop_button"_spr);

			ButtonSprite* spr2 = ButtonSprite::create("boilerplate spawn loop", 30, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.3f);
			CCMenuItemSpriteExtra* btn2 = CCMenuItemSpriteExtra::create(
				spr2, this, menu_selector(BSLPauseLayer::bpLoop)
			);
			btn2->setID("boilerplate_spawn_loop_button"_spr);

			CCNode* menu = getChildByID("small-actions-menu");
			CCNode* afterNode = static_cast<CCNode*>(menu->getChildren()->objectAtIndex(0));
			menu->insertBefore(btn1, afterNode);
			menu->insertBefore(btn2, afterNode);
			menu->updateLayout();
		}
		return true;
	}
};
