#include "libSkeleton.h"
#include "io.h"
#include <iostream>
#include <iomanip>


bool io::readDouble (std::istream &os, double &d, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.read ((char *) &d, sizeof (d));
	else
		os >> d;

	if (os.eof()) return false;
	else return true;
}

void io::writeDouble (std::ostream &os, const double &d, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.write ((char *) &d, sizeof (d));
	else
	{
		os << std::setprecision (10);
		os << d << " ";
	}
}

bool io::readFloat (std::istream &os, float &f, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.read ((char *) &f, sizeof (f));
	else
		os >> f;

	if (os.eof()) return false;
	else return true;

}

void io::writeFloat (std::ostream &os, const float &f, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.write ((char *) &f, sizeof (f));
	else
		os << f << " ";
}

bool io::readInt (std::istream &os, int &i, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.read ((char *) &i, sizeof (i));
	else
		os >> i;

	if (os.eof()) return false;
	else return true;
}

void io::writeInt (std::ostream &os, const int &i, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.write ((char *) &i, sizeof (i));
	else
		os << i << " ";
}

bool io::readLong (std::istream &os, long &i, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.read ((char *) &i, sizeof (i));
	else
		os >> i;

	if (os.eof()) return false;
	else return true;
}

void io::writeLong (std::ostream &os, const long &i, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		os.write ((char *) &i, sizeof (i));
	else
		os << i << " ";
}

void io::writeDelimiter (std::ostream &os, io::SkeletonIOFlags pf)
{
	if (pf & io::POSE_IO_BINARY)
		;
	else
		os << std::endl;
}

