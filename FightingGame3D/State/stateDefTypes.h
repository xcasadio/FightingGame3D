#ifndef _STATEDEFTYPE_H_
#define _STATEDEFTYPE_H_

#include <d3dx9.h>

/**
 * definit les stateDef obligatoire et commun
 * Les memes constantes sont utilisées pour definir les animId
 * STATE_DEF_IDLE = animId pour le state idle
 */
enum
{
	STATE_DEF_GHOST = -1, // etat verifié a chaque frame
	STATE_DEF_IDLE = 0,
	STATE_DEF_WALK_FORWARD = 50,
	STATE_DEF_WALK_BACKWARD = 100,
	STATE_DEF_IDLE_TO_CROUCH = 150,
	STATE_DEF_CROUCH = 200,
	STATE_DEF_CROUCH_TO_IDLE = 250,
	STATE_DEF_IDLE_TO_AIR = 300,
	STATE_DEF_IDLE_TO_AIR_FORWARD = 301,
	STATE_DEF_IDLE_TO_AIR_BACKWARD = 302,
	STATE_DEF_AIR_BACKWARD = 350,
	STATE_DEF_AIR_IDLE = 400,
	STATE_DEF_AIR_FORWARD = 450,
	STATE_DEF_AIR_TO_IDLE = 500,

	STATE_DEF_GUARD_STAND = 5000,
	STATE_DEF_GUARD_CROUCH = 5050,
	STATE_DEF_GUARD_AIR = 5100,

	STATE_DEF_HIT_STAND = 6000,
	STATE_DEF_HIT_CROUCH = 6050,
	STATE_DEF_HIT_AIR = 6100,
};

/**
 * Type de state pour les StateDef
 */
typedef enum eStateType
{
	STATE_TYPE_STAND = 0,
	STATE_TYPE_CROUCH,
	STATE_TYPE_AIR,
	STATE_TYPE_LIE_DOWN,
	STATE_TYPE_UNCHANGED,
} eStateType;

/**
 * Type de mouvement pour les StateDef
 */
typedef enum eMoveType
{
	MOVE_TYPE_ATTACK = 0,
	MOVE_TYPE_IDLE,
	MOVE_TYPE_HIT,
	MOVE_TYPE_UNCHANGED,
} eMoveType;

/**
 * Type de physique pour les StateDef
 */
typedef enum ePhysics
{
	PHYSIC_TYPE_STAND = 0,
	PHYSIC_TYPE_CROUCH,
	PHYSIC_TYPE_AIR,
	PHYSIC_TYPE_NONE,
	PHYSIC_TYPE_UNCHANGED,
} ePhysics;

/**
 * Utiliser pour la logique des triggers
 * Tous les operateurs possibles
 */
typedef enum eOperator
{
	OPERATOR_UNDEFINED	= 0,
	OPERATOR_EQUAL		= 1,
	OPERATOR_DIFFERENT	= 2,
	OPERATOR_SUP		= 4,
	OPERATOR_INF		= 8,
	OPERATOR_OR			= 16,
	OPERATOR_AND		= 32,
	OPERATOR_SUP_EQUAL	= 64,
	OPERATOR_INF_EQUAL	= 128,
} eOperator;

/**
 * Utiliser pour analyse les expressions des triggers
 * Debut des mots clé logique
 */
#define OPERATOR_STRING "()[]!=&|<>"

/**
 * Type de Controleur de state
 */
typedef enum eStateControllerType
{
	STATE_CONTROLLER_TYPE_CHANGE_ANIM,
	STATE_CONTROLLER_TYPE_CHANGE_STATE,
	STATE_CONTROLLER_TYPE_CTRL_SET,
	STATE_CONTROLLER_TYPE_HIT_DEF,
	STATE_CONTROLLER_TYPE_MOVE_PLAYER,
	STATE_CONTROLLER_TYPE_MOVE_PLAYER_BY_TIME,
	STATE_CONTROLLER_TYPE_MOVE_FORWARD,
	STATE_CONTROLLER_TYPE_MOVE_BACKWARD,
	STATE_CONTROLLER_TYPE_MOVE_STRAFE_RIGHT,
	STATE_CONTROLLER_TYPE_MOVE_STRAFE_LEFT,
} eStateControllerType;

/**
 * Les mots clés reconnu par les triggers
 */
#define TRIGGER_KEY_WORD_COMMAND_NAME '"'

#define TRIGGER_KEY_WORD_A "A"
#define TRIGGER_KEY_WORD_C "C"
#define TRIGGER_KEY_WORD_S "S"

#define TRIGGER_KEY_WORD_ANIMID "animid"
#define TRIGGER_KEY_WORD_ANIM_TIME "animtime"
#define TRIGGER_KEY_WORD_CTRL "ctrl"
#define TRIGGER_KEY_WORD_COMMAND "command"
#define TRIGGER_KEY_WORD_END_ANIM_REACH "endanimreach"
#define TRIGGER_KEY_WORD_HIT "hit"
#define TRIGGER_KEY_WORD_IS_HIT "ishit"
#define TRIGGER_KEY_WORD_MOVE_CONTACT "movecontact"
#define TRIGGER_KEY_WORD_POS_X "posx"
#define TRIGGER_KEY_WORD_POS_Y "posy"
#define TRIGGER_KEY_WORD_POS_Z "posz"
//#define TRIGGER_KEY_WORD_SET_VEL "velset"
#define TRIGGER_KEY_WORD_STATE_TYPE "statetype"
#define TRIGGER_KEY_WORD_STATE_NO "stateno"
#define TRIGGER_KEY_WORD_VEL_X "velx"
#define TRIGGER_KEY_WORD_VEL_Y "vely"
#define TRIGGER_KEY_WORD_VEL_Z "velz"
#define TRIGGER_KEY_WORD_JUMP_VEL_X "jumpvelx"
#define TRIGGER_KEY_WORD_JUMP_VEL_Y "jumpvely"
#define TRIGGER_KEY_WORD_JUMP_VEL_Z "jumpvelz"

typedef enum eIAKeyWord
{
	IA_KEY_WORD_DIST_BETWEEN_PLAYER,
	IA_KEY_WORD_OPPONENT_IS_ATTACKING,
} eIAKeyWord;

/**
 *
 */
#define COMMAND_TIME_INFINITE -1



typedef enum eHitHeight
{
	HIT_LOW,
	HIT_MIDDLE,
	HIT_HIGH,
} eHitHeight;

typedef enum eStateControllerValueType
{
	STATE_CONTROLLER_VALUE_TYPE_VEC3,
	STATE_CONTROLLER_VALUE_TYPE_FLOAT,
} eStateControllerValueType;

typedef struct sStateControllerValue
{
	eStateControllerValueType type;
	//union
	//{
	D3DXVECTOR3 vec3;
	float real;
	//}
} sStateControllerValue;



#endif // _STATEDEFTYPE_H_
