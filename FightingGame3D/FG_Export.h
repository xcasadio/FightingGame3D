#pragma once


#ifdef FIGHTING_GAME_EXPORTS
#   define FIGHTING_GAME_EXPORT __declspec(dllexport)
#else
#   define FIGHTING_GAME_EXPORT __declspec(dllimport)
#endif
