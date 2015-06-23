#include "libSkeleton.h"


SkeletonRepresentationJSON::SkeletonRepresentationJSON(double _version)
{
	
}


SkeletonRepresentationJSON::~SkeletonRepresentationJSON(void)
{
}

/** 
* Returns JSON array from input Vector structure
*/
Json::Value SkeletonRepresentationJSON::jsonAddPoint3(Eigen::Vector3f point)
{
	Json::Value jsonVector;
	jsonVector.append(point.x);
	jsonVector.append(point.y);
	jsonVector.append(point.z);

	return jsonVector;
}

/**
* Serialize single skeleton pose into JSON string
*/
void SkeletonRepresentationJSON::Serialize( Json::Value& skeleton )
{
	skeleton["id"] = this->id;
	skeleton["version"] = this->m_version;	// this is new... should this be under set?

	// head
	skeleton["head"] = jsonAddPoint3(this->head());
	if (this->m_version > 1)
	{
		skeleton["neck"] = jsonAddPoint3(this->neck());
	}

	// upper body
	skeleton["leftShoulder"] = jsonAddPoint3(this->leftShoulder());
	skeleton["rightShoulder"] = jsonAddPoint3(this->rightShoulder());
	skeleton["leftElbow"] = jsonAddPoint3(this->leftElbow());
	skeleton["rightElbow"] = jsonAddPoint3(this->rightElbow());
	skeleton["leftWrist"] = jsonAddPoint3(this->leftWrist());
	skeleton["rightWrist"] = jsonAddPoint3(this->rightWrist());
	skeleton["leftHand"] = jsonAddPoint3(this->leftHand());
	skeleton["rightHand"] = jsonAddPoint3(this->rightHand());

	if (this->m_version > 1)
	{
		// additional joints for hand detection
		skeleton["leftHandTip"] = jsonAddPoint3(this->leftHandTip());
		skeleton["rightHandTip"] = jsonAddPoint3(this->rightHandTip());
		skeleton["leftThumb"] = jsonAddPoint3(this->leftThumb());
		skeleton["rightThumb"] = jsonAddPoint3(this->rightThumb());
	}

	// lower body
	skeleton["leftHip"] = jsonAddPoint3(this->leftHip());
	skeleton["rightHip"] = jsonAddPoint3(this->rightHip());
	skeleton["leftKnee"] = jsonAddPoint3(this->leftKnee());
	skeleton["rightKnee"] = jsonAddPoint3(this->rightKnee());
	skeleton["leftAnkle"] = jsonAddPoint3(this->leftAnkle());
	skeleton["rightAnkle"] = jsonAddPoint3(this->rightAnkle());
	skeleton["leftFoot"] = jsonAddPoint3(this->leftFoot());
	skeleton["rightFoot"] = jsonAddPoint3(this->rightFoot());

	// for historical reasons, this is named differently... should we rename???
	if (this->m_version == 1)
	{
		skeleton["hipCenter"] = jsonAddPoint3(this->spineBase());
		skeleton["shoulderCenter"] = jsonAddPoint3(this->spineMid());
		skeleton["spine"] = jsonAddPoint3(this->spineShoulder());
	}
	else
	{
		skeleton["spineBase"] = jsonAddPoint3(this->spineBase());
		skeleton["spineMid"] = jsonAddPoint3(this->spineMid());
		skeleton["spineShoulder"] = jsonAddPoint3(this->spineShoulder());
	}


	// TODO: should we store hand position state/orientation/confidence level????

}


/**
* Deserialize JSON string into the skeleton
*/
void SkeletonRepresentationJSON::Deserialize( Json::Value& root )
{

	this->id = root.get("id", this->id).asUInt();
	this->m_version = root.get("version", 1.0f).asFloat();			// by default use version 1
	// get head
	this->head(jsonGetPoint3(root.get("head", Json::nullValue)));
	if (this->m_version > 1.0f)
	{
		this->neck(jsonGetPoint3(root.get("neck", Json::nullValue)));
	}

	// upper body
	this->leftShoulder(jsonGetPoint3(root.get("leftShoulder", Json::nullValue)));
	this->rightShoulder(jsonGetPoint3(root.get("rightShoulder", Json::nullValue)));
	this->leftElbow(jsonGetPoint3(root.get("leftElbow", Json::nullValue)));
	this->rightElbow(jsonGetPoint3(root.get("rightElbow", Json::nullValue)));
	this->leftWrist(jsonGetPoint3(root.get("leftWrist", Json::nullValue)));
	this->rightWrist(jsonGetPoint3(root.get("rightWrist", Json::nullValue)));
	this->leftHand(jsonGetPoint3(root.get("leftHand", Json::nullValue)));
	this->rightHand(jsonGetPoint3(root.get("rightHand", Json::nullValue)));

	if (this->m_version > 1.0f)
	{
		this->leftHandTip(jsonGetPoint3(root.get("leftHandTip", Json::nullValue)));
		this->rightHandTip(jsonGetPoint3(root.get("rightHandTip", Json::nullValue)));
		this->leftThumb(jsonGetPoint3(root.get("leftThumb", Json::nullValue)));
		this->rightThumb(jsonGetPoint3(root.get("rightThumb", Json::nullValue)));
	}

	// lower body
	this->leftHip(jsonGetPoint3(root.get("leftHip", Json::nullValue)));
	this->rightHip(jsonGetPoint3(root.get("rightHip", Json::nullValue)));
	this->leftKnee(jsonGetPoint3(root.get("leftKnee", Json::nullValue)));
	this->rightKnee(jsonGetPoint3(root.get("rightKnee", Json::nullValue)));
	this->leftAnkle(jsonGetPoint3(root.get("leftAnkle", Json::nullValue)));
	this->rightAnkle(jsonGetPoint3(root.get("rightAnkle", Json::nullValue)));
	this->leftFoot(jsonGetPoint3(root.get("leftFoot", Json::nullValue)));
	this->rightFoot(jsonGetPoint3(root.get("rightFoot", Json::nullValue)));

	// old hips
	if (this->m_version == 1.0f)
	{
		this->spineBase(jsonGetPoint3(root.get("hipCenter", Json::nullValue)));
		this->spineShoulder(jsonGetPoint3(root.get("shoulderCenter", Json::nullValue)));
		this->spineMid(jsonGetPoint3(root.get("spine", Json::nullValue)));
	}
	else
	{
		this->spineBase(jsonGetPoint3(root.get("spineBase", Json::nullValue)));
		this->spineShoulder(jsonGetPoint3(root.get("spineShoulder", Json::nullValue)));
		this->spineMid(jsonGetPoint3(root.get("spineMid", Json::nullValue)));
	}

	// TODO: should we calculate the neck or just leav it out????

}



/** 
* Returns Point3 structure for give input json array
*/
Eigen::Vector3f SkeletonRepresentationJSON::jsonGetPoint3(const Json::Value &jsonValue)
{
	Eigen::Vector3f point = Eigen::Vector3f::Zero();

	if (jsonValue.isNull()) return Eigen::Vector3f();

	if (jsonValue.isArray() && jsonValue.size() == 3)
	{
		Json::ValueIterator it = jsonValue.begin();
		point[0] = static_cast<float>((*it).asFloat());
		it++;
		point[1] = static_cast<float>((*it).asFloat());
		it++;
		point[2] = static_cast<float>((*it).asFloat());
	}
	else
		return point;//throw Exception("Error in PoseSet::jsonGetPoint3(): value size = %d", jsonValue.size());

	return point;
}



SkeletonRepresentationSetJSON::SkeletonRepresentationSetJSON()
{

}

/** 
* Serialize a set of skeletons
*/
void SkeletonRepresentationSetJSON::Serialize( Json::Value& root )
{
	root["version"] = this->getVersion();
	root["kinectVersion"] = this->getKinectVersion();

	root["frameNumber"] = this->getFrameNumber();
	root["timeStamp"] = this->getTimeStampdDouble();
	// get unix time stamp
	long seconds, micros;
	this->getTimeStamp(seconds, micros);
	root["realTimeStampSeconds"] = seconds;
	root["realTimeStampMicros"] = micros;

	root["skeletonCount"] = size();

	Json::Value skeletonArray;
	for (size_t i = 0; i < size(); i++)
	{

		skeletonArray.append(this->at(i).Serialize());

	}
	root["skeletons"] = skeletonArray;

}


/** 
* Deserialize set of skeletons
*/
void SkeletonRepresentationSetJSON::Deserialize(Json::Value& root)
{
	// de-serialize primitives
	m_version = root.get("version",0).asDouble();
	m_frameNumber = root.get("frameNumber",0).asInt();
	m_timeStamp = root.get("timeStamp",0).asDouble();

	size_t skeletonCount = root.get("skeletonCount",0).asUInt();

	// we only use the first skeleton (index = 0) from the data
	//m_v.assign(skeletonCount, PoseRepresentationPoints());

	Json::Value skeletonArray = root.get("skeletons", Json::nullValue);

	if (!skeletonArray.isNull())
	{
		size_t index = 0;
		for (Json::ValueIterator it = skeletonArray.begin(); it != skeletonArray.end(); it++)
		{
			if (index > 1) break;	/// here, we quit after reading the zero index

			if ((*it).isNull()) continue;


			index++;
		}
	}

}

SkeletonRepresentationSequenceJSON::SkeletonRepresentationSequenceJSON()
{

}


void SkeletonRepresentationSequenceJSON::Serialize( Json::Value& root )
{

}

/** 
* Deserialize JSON formatted skeleton sequence
*/
void SkeletonRepresentationSequenceJSON::Deserialize(Json::Value& root)
{
	this->version = root.get("version", 0).asDouble();
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
			SkeletonRepresentationJSON skeletonFrame;
			skeletonFrame.Deserialize(*it);

			push_back(skeletonFrame);
			index++;
		}
	}
}