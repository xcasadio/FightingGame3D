#pragma once

#include <iostream>
#include "../Config/Export.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Permet de sauvegarder/charger des données dans un fichier
	 */
	template <class T>
	class BinarySerialization
	{
	public:
		/**
		 *
		 */
		BinarySerialization();

		/**
		 *
		 */
		~BinarySerialization();

		/**
		 * Alloue de la memoire pour num fois T
		 * @param num nombre d'element T à allouer
		 * @return Un pointeur sur la memoire allouée ou NULL si echec
		 */
		T* Create(unsigned long num);

		/**
		 *	Libere la memoire
		 */
		void Free();

		/**
		 * Sauvegarde dans le fichier fileName_
		 * @param fileName_ le nom du fichier à sauvegarder
		 * @return true si reussi sinon false
		 */
		bool Save(const char* fileName_);

		/**
		 * Sauvegarde dans le fichier fileName_. Ecrase le fichier si il existe.
		 * @param fileName_ le nom du fichier à sauvegarder
		 * @return NULL si echec sinon un pointeur sur la memoire allouée
		 */
		T* Load(const char* fileName_);

		/**
		 * @return le nombre d'element
		 */
		unsigned long GetSize() const;

	protected:
		T* m_Buf;
		unsigned long m_Num;
	};

#include "BinarySerialization.inl"

} // namespace GameEngine


#include "../DebugEngine/DebugNewOff.h"
