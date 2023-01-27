#include <iostream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>

#include <vector>

#include "Utilities/Serializable.h"

using namespace std;
using namespace GameEngine;

class A : public ISerializable
{
public:
	float f;
	vector<int> n;

	int Read( std::istream &is )
	{
		int octetRead = is.tellg();

		is.read( (char *) &f, sizeof(float) );

		std::vector<int>::size_type size;

		is.read( (char *) &size, sizeof( std::vector<int>::size_type ) );

		for ( size_t i = 0; i < size; i++ )
		{
			int x;
			is.read( (char *) &x, sizeof(int) );
			n.push_back( x );
		}

		octetRead = (int) is.tellg() - octetRead;

		return octetRead;
	}

	/**
	 *
	 */
	std::ostream &operator >> ( std::ostream &os )
	{
		os.write( (char *) &f, sizeof(float) );

		std::vector<int>::size_type size = n.size();
		os.write( (char *) &size, sizeof( std::vector<int>::size_type) );

		for ( size_t i = 0; i < n.size(); i++ )
		{
			int x = n.at(i);
			os.write( (char *) &x, sizeof(int) );
		}

		return os;
	}
};

int main( int argc, char *argv[] )
{
	A a;

	a.f = 12300.2f;

	a.n.push_back(1);
	a.n.push_back(2);
	a.n.push_back(3);
	a.n.push_back(4);

	std::fstream fs( "data.txt", std::ios::binary | std::ios::out );

	a >> fs;

	fs.close();

	A aa;
	aa.LoadSerialized( "data.txt" );

	std::cout << aa.f << std::endl;

	for ( int i = 0; i < aa.n.size(); i++ )
	{
		std::cout << aa.n.at(i) << " ";
	}

	std::cout << std::endl;

	getchar();
	
	return EXIT_SUCCESS;
}