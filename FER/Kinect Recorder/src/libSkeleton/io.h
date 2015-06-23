#pragma once


namespace io
{

	// I/O flags for writing into files
	enum SkeletonIOFlags { 
		POSE_IO_NOFLAGS = 0x01, 
		POSE_IO_BINARY = 0x02, 
		POSE_IO_JOINT_DETECTION_SCORES = 0x20, 
		POSE_IO_ROTATIONS = 0x10,
		POSE_IO_TRANSFORM = 0x08
	};

	bool readDouble (std::istream &is, double &d, io::SkeletonIOFlags pf);
	void writeDouble (std::ostream &os, const double &d, io::SkeletonIOFlags pf);
	bool readFloat (std::istream &is, float &f, io::SkeletonIOFlags pf);
	void writeFloat (std::ostream &os, const float &f, io::SkeletonIOFlags pf);
	bool readInt (std::istream &is, int &i, io::SkeletonIOFlags pf);
	void writeInt (std::ostream &os, const int &i, io::SkeletonIOFlags pf);
	bool readLong (std::istream &is, long &i, io::SkeletonIOFlags pf);
	void writeLong (std::ostream &os, const long &i, io::SkeletonIOFlags pf);
	void writeDelimiter (std::ostream &os, io::SkeletonIOFlags pf);
	
};