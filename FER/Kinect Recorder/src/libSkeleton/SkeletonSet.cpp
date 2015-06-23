#include "libSkeleton.h"
#include "io.h"



SkeletonSet::Header::Header():
	m_version(SKELETON_FILE_VERSION),
	m_kinectVersion(2.0),
	m_frameNumber(0),
	m_timeStampSeconds(0),
	m_timeStampMicroSeconds(0),
	m_timeStamp(0.0)
{

}

/**
 *	Copy constructor for Header
 */
SkeletonSet::Header::Header(const Header &header)
{
	this->m_frameNumber = header.m_frameNumber;
	this->m_kinectVersion = header.m_kinectVersion;
	this->m_matCameraToWorld = header.m_matCameraToWorld;
	this->m_timeStamp =  header.m_timeStamp;
	this->m_timeStampMicroSeconds = header.m_timeStampMicroSeconds;
	this->m_timeStampSeconds = header.m_timeStampSeconds;
	this->m_version = header.m_version;

}

SkeletonSet::SkeletonSet(float version)
{
	// set default header information
	m_header.m_version = version;

}



SkeletonSet::~SkeletonSet()
{
	m_v.clear();
}



SkeletonSet::Header SkeletonSet::getHeader() const
{
	return m_header;
}

void SkeletonSet::setHeader(const SkeletonSet::Header &header)
{
	m_header = header;	// We can do this for simple structure
}

void SkeletonSet::save (std::ostream &os, io::SkeletonIOFlags pf) const
{
	int nPoses = this->size ();

	// get number of tracked poses
	int nTrackedPoses = 0;
	for (int i = 0; i < nPoses; i ++) if (at(i).isTracked()) nTrackedPoses++;

	// version
	io::writeFloat (os, m_header.m_version, pf);
	// frame number
	io::writeLong (os, m_header.m_frameNumber, pf);
	// time stamp in seconds from start
	io::writeDouble (os, m_header.m_timeStamp, pf);
	// unix time stamp
	io::writeLong(os, m_header.m_timeStampSeconds, pf);
	io::writeLong(os, m_header.m_timeStampMicroSeconds, pf);
	// write number of poses
	io::writeInt (os, nTrackedPoses, pf);
	io::writeDelimiter (os, pf);

	// only store body transform when there is pose detected
	if (nTrackedPoses > 0)
	{
		if ( pf & io::POSE_IO_TRANSFORM)
		{
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
				writeFloat(os, m_header.m_matCameraToWorld(i,j), pf);
		}
		io::writeDelimiter (os, pf);
	}

	// go through all poses
	for (int i = 0; i < nPoses; i ++)
	{
		// ignore non-tracked poses
		if (!at(i).isTracked()) continue;

		//        int poseID = 0;
		io::writeInt (os, at(i).m_id, pf);

		at(i).save (os, pf);
		io::writeDelimiter (os, pf);
	}
}


/**
* Load pose file
*/
bool SkeletonSet::load (std::istream &is, io::SkeletonIOFlags pf)
{
	float storedVersion = 0.0f;
	int nPoses = 0;
	
	// read version number
	bool status = io::readFloat (is, storedVersion, pf);
	if (!status) return false;
	m_header.m_version = storedVersion;

	// TODO: show warning for old versions
	//if (storedVersion != SKELETON_FILE_VERSION)
	//	std::cout << "PoseSet::load(): Outdated file version (v" << storedVersion << ")" << std::endl;
	//	throw Exception ("Unknown pose file version.");

	// read frame number or increase automatically if not present
	if ( storedVersion > 2.0f) 
		io::readLong (is, m_header.m_frameNumber, pf);
	else
		m_header.m_frameNumber++;

	// read time stamp in double
	io::readDouble (is, m_header.m_timeStamp, pf);
	// read Unix time stamps for version 2.0
	long foo1, foo2;
	if ( storedVersion > 2.0f) {
		io::readLong (is, foo1, pf);
		io::readLong (is, foo2, pf);
	}

	// do we need to read the transform???

	// read number of poses (skeletons) stored in the file
	io::readInt (is, nPoses, pf);
	// resize this vector to appropriate number of skeletons
	resize (nPoses);

	// go through each pose/skeleton and read joint and other data
	for (int i = 0; i < nPoses; i ++)
	{
		int poseID;
		io::readInt (is, poseID, pf);
		//        if (poseID < 0 || poseID >= size ())
		//            throw Exception ("Unknown pose file version.");
		at(i).load (is, pf, storedVersion);
		at(i).m_id = poseID;
	}

	return true;
}

/** 
* Serialize a set of skeletons
*/
void SkeletonSet::Serialize( Json::Value& root )
{
	root["version"] = this->getVersion();
	root["kinectVersion"] = this->getKinectVersion();

	root["frameNumber"] = this->getFrameNumber();
	root["timeStamp"] = this->getTimeStampdDouble();
	
	// get Unix time stamp
	long seconds, micros;
	this->getTimeStamp(seconds, micros);
	root["realTimeStampSeconds"] = seconds;
	root["realTimeStampMicros"] = micros;

	root["skeletonCount"] = size();

	Json::Value skeletonArray;
	for (size_t i = 0; i < size(); i++)
	{
		Json::Value jsonSkeleton;
		this->at(i).Serialize(jsonSkeleton);
		skeletonArray.append(jsonSkeleton);

	}
	// add to the root
	root["skeletons"] = skeletonArray;

}


/** 
* Deserialize set of skeletons
*/
void SkeletonSet::Deserialize(Json::Value& root)
{
	// de-serialize primitives
	this->setVersion(root.get("version", 1.0).asFloat());
	this->setKinectVersion(root.get("kinectVersion", 1.0).asFloat());

	this->setFrameNumber(root.get("frameNumber",0).asUInt());
	this->setTimeStampDouble(root.get("timeStamp",0).asDouble());

	long seconds, micros;
	seconds = static_cast<long>(root.get("realTimeStampSeconds", 0).asUInt64());
	micros = static_cast<long>(root.get("realTimeStampMicros", 0).asUInt64());
	this->setTimeStamp(seconds, micros);

	// get size of array attached
	size_t skeletonCount = root.get("skeletonCount",0).asUInt();
	m_v.assign(skeletonCount, Skeleton());

	Json::Value skeletonArray = root.get("skeletons", Json::nullValue);

	if (!skeletonArray.isNull())
	{
		size_t index = 0;
		for (Json::ValueIterator it = skeletonArray.begin(); it != skeletonArray.end(); it++)
		{
			if (index > 1) break;	/// here, we quit after reading the zero index

			if ((*it).isNull()) continue;

			Json::Value jsonSkeleton = (*it);
			m_v.at(index).Deserialize(jsonSkeleton);

			index++;
		}
	}

}