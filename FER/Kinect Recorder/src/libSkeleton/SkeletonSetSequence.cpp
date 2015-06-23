#include "libSkeleton.h"
#include "SkeletonSetSequence.h"

#include <fstream>


/** 
* Default constructor
*/
SkeletonSetSequence::SkeletonSetSequence()
{

}



SkeletonSetSequence::~SkeletonSetSequence()
{
	m_v.clear();
}

/** 
* Load skeleton sequence from a file. Returns 0 if ok, returns -1 if error.
*/
int SkeletonSetSequence::load(std::string filename)
{
	std::ifstream f;
	f.open(filename);
	if (!f)
	{
		throw Exception("Cannot open file for reading: %s", filename.c_str());
		return -1;
	}
	std::istream& is = f;

	// start loading skeleton sets frame by frame
	SkeletonSet skeletonSet;
	while (skeletonSet.load(is, io::POSE_IO_NOFLAGS))
	{
		this->m_v.push_back(skeletonSet);
	}

	this->version = skeletonSet.getVersion();

	f.close();

	return 0;
}

/** 
* Write skeleton sequence to a file
*/
int SkeletonSetSequence::save(std::string filename, io::SkeletonIOFlags flags)
{
	std::ofstream f = std::ofstream(filename);
	if (!f)
	{
		throw Exception("Cannot open file for writing: %s", filename.c_str());
		return -1;
	}
	std::ostream& os = f;

	for (size_t i = 0; i < size(); i++)
	{
		this->m_v[i].save(os, flags);
	}

	f.close();

	return 0;
}


/** 
* Serialize a set of skeletons
*/
void SkeletonSetSequence::Serialize( Json::Value& root )
{
#ifdef DEBUG_JSON
	std::cout << "SkeletonSequence::Serialize(): starting serialization" << std::endl;
	CStopWatch timer;
	timer.startTimer();
#endif

	// serialize sequence of skeleton sets
	root["version"] = this->version;
	root["frameCount"] = this->size();

	Json::Value skeletonSequence;
	for (size_t i = 0; i < this->size(); i++)
	{
		Json::Value skeletonSet;

		// skeletonSet
		m_v[i].Serialize(skeletonSet);
		skeletonSequence.append(skeletonSet);

	}
	root["skeletonSequence"] = skeletonSequence;

#ifdef DEBUG_JSON
	timer.stopTimer();
	std::cout << "SkeletonSequence::Serialize(): serialization completed in " << timer.getElapsedTime() << " seconds." << std::endl;
#endif

}


/** 
* Deserialize set of skeletons
*/
void SkeletonSetSequence::Deserialize(Json::Value& root)
{
#ifdef DEBUG_JSON
	std::cout << "SkeletonSequence::Deserialize(): starting de-serialization" << std::endl;
	CStopWatch timer;
	timer.startTimer();
#endif

	this->version = root.get("version", 0).asFloat();
	size_t frameCount = root.get("frameCount", 0).asUInt();

	Json::Value skeletonSequence = root.get("skeletonSequence", Json::nullValue);

	if (frameCount == 0 || skeletonSequence.isNull())
	{
		root = Json::nullValue;
		return;
	}
	else
	{
		size_t index = 0;
		for (Json::ValueIterator it = skeletonSequence.begin(); it != skeletonSequence.end(); it++)
		{
			if ((*it).isNull()) continue;

			// get pose set for current frame
			SkeletonSet skeletonFrame;
			skeletonFrame.Deserialize(*it);

			m_v.push_back(skeletonFrame);
			index++;
		}
	}


#ifdef DEBUG_JSON
	timer.stopTimer();
	std::cout << "SkeletonSequence::Deserialize(): de-serialization completed in " << timer.getElapsedTime() << " seconds." << std::endl;
#endif

}