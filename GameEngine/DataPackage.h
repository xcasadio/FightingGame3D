#pragma once

#include <iostream>

namespace GameEngine
{
	/**
	 * Classe abstraite de serialisation
	 */
	class ISerializable
	{
	public:
		/**
		 * Permet de charger un fichier de serialisation
		 */
		virtual bool LoadSerialized(const char *fileName_) = 0;

		/**
		 * Permet de sauvegarder un fichier de serialisation
		 */
		virtual bool SaveSerialized(const char *fileName_) = 0;
	};

	/**
	 * Permet de sauvegarder/charger des données dans un fichier
	 */
	template <class T>
	class cDataPackage
	{
	public:
		/**
		 *
		 */
		cDataPackage();

		/**
		 *
		 */
		~cDataPackage();

		/**
		 * Alloue de la memoire pour num fois T
		 * @param num nombre d'element T à allouer
		 * @return Un pointeur sur la memoire allouée ou NULL si echec
		 */
		T *Create(unsigned long num);

		/**
		 *	Libere la memoire
		 */
		void Free();

		/**
		 * Sauvegarde dans le fichier fileName_
		 * @param fileName_ le nom du fichier à sauvegarder
		 * @return true si reussi sinon false
		 */
		bool Save(const char *fileName_);

		/**
		 * Sauvegarde dans le fichier fileName_. Ecrase le fichier si il existe.
		 * @param fileName_ le nom du fichier à sauvegarder
		 * @return NULL si echec sinon un pointeur sur la memoire allouée
		 */
		T *Load(const char *fileName_);

		/**
		 * @return le nombre d'element
		 */
		unsigned long GetSize() const;

	protected:
		T *m_Buf;
		unsigned long m_Num;
	};

#include "DataPackage.inl"

} // namespace GameEngine
