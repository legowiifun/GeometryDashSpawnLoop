/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

class $modify(EditorPauseLayer) {
	void createSpawnTrigger(bool isSpawnTriggered, bool isMultiTriggered, int group, 
		int targetGroupID, float spawnTriggerDelay, bool spawnOrdered, CCPoint p) {
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
		}
	}
	void onCreateLoop(cocos2d::CCObject * sender) {
		auto disable = Mod::get()->getSettingValue<bool>("disable");
		// if the mod is disabled
		if (disable) {
			EditorPauseLayer::onCreateLoop(sender);
			return;
		}
		// otherwise...
		// get the editor UI and the next free group ID
		EditorUI* ui = m_editorLayer->m_editorUI;
		int id = m_editorLayer->getNextFreeGroupID(CCArray::create());

		// get the minimum x coordinate of the loop, and the y coordinate of that object
		float minX=0;
		float y = 0;
		bool flag = false;
		// also add each trigger to the next free group, and set them to spawn-triggered and multi-triggered
		for (int i = 0; i < ui->getSelectedObjects()->count(); i++) {
			if (static_cast<GameObject*>(ui->getSelectedObjects()->objectAtIndex(i))->m_classType==GameObjectClassType::Effect) {
				EffectGameObject* obj = static_cast<EffectGameObject*>(ui->getSelectedObjects()->objectAtIndex(i));
				obj->m_isSpawnTriggered = true;
				obj->m_isMultiTriggered = true;
				obj->addToGroup(id);
				if (!flag || obj->getPositionX() < minX) {
					minX=obj->getPositionX();
					y = obj->getPositionY();
					flag = true;
				}
			}
		}

		CCPoint p;
		
		// set up the first spawn trigger
		p.setPoint(minX, y + 60);
		createSpawnTrigger(true, true, id, id, Mod::get()->getSettingValue<float>("delay"), true, p);

		// set up the second spawn trigger
		p.setPoint(minX, y + 90);
		createSpawnTrigger(false, false, 0, id, 0, true, p);
	}
};