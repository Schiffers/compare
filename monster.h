/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2015  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_MONSTER_H_9F5EEFE64314418CA7DA41D1B9409DD0
#define FS_MONSTER_H_9F5EEFE64314418CA7DA41D1B9409DD0

#include "tile.h"
#include "monsters.h"
#include "luascript.h"
#include "spawn.h"

class Creature;
class Game;
class Spawn;

typedef std::unordered_set<Creature*> CreatureHashSet;
typedef std::list<Creature*> CreatureList;

enum TargetSearchType_t {
	TARGETSEARCH_DEFAULT,
	TARGETSEARCH_RANDOM,
	TARGETSEARCH_ATTACKRANGE,
	TARGETSEARCH_NEAREST,
};

class Monster final : public Creature
{
	public:
		std::string name, nameDescription, altStrDesc;
		static Monster* createMonster(const std::string& name);
		static int32_t despawnRange;
		static int32_t despawnRadius;

		explicit Monster(MonsterType* mtype);
		~Monster();

		// non-copyable
		Monster(const Monster&) = delete;
		Monster& operator=(const Monster&) = delete;

		Monster* getMonster() final {
			return this;
		}
		const Monster* getMonster() const final {
			return this;
		}

		void setID() final {
			if (id == 0) {
				id = monsterAutoID++;
			}
		}

		void removeList() final;
		void addList() final;

		// aqui // monsterName
		const std::string& getName() const final {
			if (!name.empty()) {
				return name;
			}
			return mType->name;
		}
		const std::string& getNameDescription() const final {
			if (nameDescription.empty()) {
				return mType->nameDescription;
			}
			return nameDescription;
		}
		std::string getDescription(int32_t) const final {
			if (!altStrDesc.empty()) {
				return altStrDesc + '.';
			}
			return strDescription + '.';
		}
		// fim
		CreatureType_t getType() const final {
			return CREATURETYPE_MONSTER;
		}

		const Position& getMasterPos() const {
			return masterPos;
		}
		// aqui // setSpawnPos
		void setMasterPos(Position pos) {
			masterPos = pos;
		}
		
		// fim
		RaceType_t getRace() const final {
			return mType->race;
		}
		int32_t getArmor() const final {
			return mType->armor;
		}
		int32_t getDefense() const final {
			return mType->defense;
		}
		bool isPushable() const final {
			return mType->pushable && baseSpeed != 0;
		}
		bool isAttackable() const final {
			return mType->isAttackable;
		}

		bool canPushItems() const {
			return mType->canPushItems;
		}
		bool canPushCreatures() const {
			return mType->canPushCreatures;
		}
		bool isHostile() const {
			return mType->isHostile;
		}
		// aqui // passive/prey etc
		bool isPassive() const {
			return mType->isPassive;
		}

		bool isPreyable() const {
			return mType->isPreyable;
		}

		bool isPet() const {
			return mType->isPet;
		}
		// fim
		bool canSee(const Position& pos) const final;
		bool canSeeInvisibility() const final {
			return isImmune(CONDITION_INVISIBLE);
		}
		uint32_t getManaCost() const {
			return mType->manaCost;
		}
		// aqui // spawn
		void setSpawn(Spawn* _spawn) {
			spawn = _spawn;
		}
		// fim 
		void onAttackedCreatureDisappear(bool isLogout) final;

		bool canWalkOnFieldType(CombatType_t combatType) const;

		void onCreatureAppear(Creature* creature, bool isLogin) final;
		void onRemoveCreature(Creature* creature, bool isLogout) final;
		void onCreatureMove(Creature* creature, const Tile* newTile, const Position& newPos, const Tile* oldTile, const Position& oldPos, bool teleport) final;
		void onCreatureSay(Creature* creature, SpeakClasses type, const std::string& text) final;

		void drainHealth(Creature* attacker, int32_t damage) final;
		void changeHealth(int32_t healthChange, bool sendHealthChange = true) final;
		void onWalk() final;
		bool getNextStep(Direction& dir, uint32_t& flags) final;
		void onFollowCreatureComplete(const Creature* creature) final;

		void onThink(uint32_t interval) final;

		bool challengeCreature(Creature* creature) final;
		bool convinceCreature(Creature* creature) final;

		void setNormalCreatureLight() final;
		bool getCombatValues(int32_t& min, int32_t& max) final;

		void doAttacking(uint32_t interval) final;
		bool hasExtraSwing() final {
			return extraMeleeAttack;
		}

		bool searchTarget(TargetSearchType_t searchType = TARGETSEARCH_DEFAULT);
		bool selectTarget(Creature* creature);

		const CreatureList& getTargetList() const {
			return targetList;
		}
		const CreatureHashSet& getFriendList() const {
			return friendList;
		}

		bool isTarget(const Creature* creature) const;
		bool isFleeing() const {
			return !isSummon() && getHealth() <= mType->runAwayHealth;
		}

		bool getDistanceStep(const Position& targetPos, Direction& dir, bool flee = false);
		bool isTargetNearby() const {
			return stepDuration >= 1;
		}

		bool isRandomSteping() const {
			return randomSteping;
		}
		void setIgnoreFieldDamage(bool ignore) {
			ignoreFieldDamage = ignore;
		}
		bool getIgnoreFieldDamage() const {
			return ignoreFieldDamage;
		}
		// aqui // setName
		void setName(std::string n) {
			name = n;
		}
		void setDescription(std::string desc) {
			nameDescription = desc;
		}
		void setStrDescription(std::string altdesc) {
			altStrDesc = altdesc;
		}
		// fim 

		BlockType_t blockHit(Creature* attacker, CombatType_t combatType, int32_t& damage,
		                     bool checkDefense = false, bool checkArmor = false, bool field = false);

		static uint32_t monsterAutoID;

	private:
		CreatureHashSet friendList;
		CreatureList targetList;

		std::string strDescription;

		MonsterType* mType;
		Spawn* spawn = nullptr;

		int64_t lastMeleeAttack = 0;
		int64_t lastDamage = 0;
		uint32_t attackTicks = 0;
		uint32_t targetTicks = 0;
		uint32_t targetChangeTicks = 0;
		uint32_t defenseTicks = 0;
		uint32_t yellTicks = 0;
		int32_t minCombatValue = 0;
		int32_t maxCombatValue = 0;
		int32_t targetChangeCooldown = 0;
		int32_t stepDuration = 0;

		Position masterPos;

		bool isIdle = true;
		bool extraMeleeAttack = true;

		bool isMasterInRange = false;
		bool randomSteping = false;
		bool ignoreFieldDamage = false;

		void onCreatureEnter(Creature* creature);
		void onCreatureLeave(Creature* creature);
		void onCreatureFound(Creature* creature, bool pushFront = false);

		void updateLookDirection();

		void addFriend(Creature* creature);
		void removeFriend(Creature* creature);
		void addTarget(Creature* creature, bool pushFront = false);
		void removeTarget(Creature* creature);

		void updateTargetList();
		void clearTargetList();
		void clearFriendList();

		void death(Creature* _lastHitCreature) final;
		Item* getCorpse(Creature* _lastHitCreature, Creature* mostDamageCreature) final;

		void setIdle(bool _idle);
		void updateIdleStatus();
		bool getIdleStatus() const {
			return isIdle;
		}

		void onAddCondition(ConditionType_t type) final;
		void onEndCondition(ConditionType_t type) final;
		void onCreatureConvinced(const Creature* convincer, const Creature* creature) final;

		bool canUseAttack(const Position& pos, const Creature* target) const;
		bool canUseSpell(const Position& pos, const Position& targetPos,
		                 const spellBlock_t& sb, uint32_t interval, bool& inRange, bool& resetTicks);
		bool getRandomStep(const Position& creaturePos, Direction& dir) const;
		bool getDanceStep(const Position& creaturePos, Direction& dir,
		                  bool keepAttack = true, bool keepDistance = true);
		bool isInSpawnRange(const Position& pos) const;
		bool canWalkTo(Position pos, Direction dir) const;

		static bool pushItem(Item* item);
		static void pushItems(Tile* tile);
		static bool pushCreature(Creature* creature);
		static void pushCreatures(Tile* tile);

		void onThinkTarget(uint32_t interval);
		void onThinkYell(uint32_t interval);
		void onThinkDefense(uint32_t interval);

		bool isFriend(const Creature* creature) const;
		bool isOpponent(const Creature* creature) const;

		uint64_t getLostExperience() const final {
			return skillLoss ? mType->experience : 0;
		}
		uint16_t getLookCorpse() const final {
			return mType->lookcorpse;
		}
		void dropLoot(Container* corpse, Creature* _lastHitCreature) final;
		uint32_t getDamageImmunities() const final {
			return mType->damageImmunities;
		}
		uint32_t getConditionImmunities() const final {
			return mType->conditionImmunities;
		}
		void getPathSearchParams(const Creature* creature, FindPathParams& fpp) const final;
		bool useCacheMap() const final {
			return !randomSteping;
		}

		friend class LuaScriptInterface;
};

#endif
