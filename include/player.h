#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf3d_entity.h"
#include "projectile.h"
#include "gf3d_ui.h"

typedef enum
{
	PLAYER_JACK,
	PLAYER_ROGUE,
	PLAYER_KNIGHT
}PlayerClass;

typedef struct
{
	char *modelName;
	char *modelAnim;
	char *specialAttack;
	char *specialOrb;
	char *no_special;
	int active;
	int health;
	float speed;
	int damagePrimary;
	int damageSecondary;
	int reloadTime;
	int user_last_click;
	char *primaryFire;
	char *secondaryFire;
	int KO_Count;
	int jackProficiency;
	int tankProficiency;
	int thiefProficiency;
	int currentProficiency;
	int special;
	ProjectileType primaryType;
	ProjectileType secondaryType;
	ProjectileType specialType;
	PlayerClass playerClass;
	UI *specialMeter;
}PlayerData;

void player_think(Entity *self);
void player_die(Entity *self);
Entity *player_spawn(Vector3D position, const char *modelName, PlayerClass playerClass);
void proficiency_system(Entity *self);

#endif