#include "libSkeleton.h"


SkeletonRepresentation::SkeletonRepresentation(double _version):
m_version(_version)
{
}


SkeletonRepresentation::~SkeletonRepresentation(void)
{
}


/************************************************************************/
/* Transformation of skeleton using Rotation and translation            */
/************************************************************************/
SkeletonRepresentation SkeletonRepresentation::transformRT(Eigen::Matrix3f R, Eigen::Vector3f pos)
{

	SkeletonRepresentation res = *this;
	SkeletonRepresentation tmp = *this;

	// transpose rotation
	Eigen::Matrix3f R1 = R.transpose();

	// translate skeleton
	tmp.head = head - pos;

	tmp.leftAnkle = leftAnkle - pos;
	tmp.leftElbow = leftElbow - pos;
	tmp.leftFoot = leftFoot- pos;
	tmp.leftHand = leftHand- pos;
	tmp.leftHip = leftHip- pos;
	tmp.leftKnee = leftKnee- pos;
	tmp.leftShoulder = leftShoulder- pos;
	tmp.leftWrist = leftWrist- pos;

	tmp.rightAnkle = rightAnkle - pos;
	tmp.rightElbow = rightElbow- pos;
	tmp.rightFoot = rightFoot- pos;
	tmp.rightHand = rightHand- pos;
	tmp.rightHip = rightHip- pos;
	tmp.rightKnee = rightKnee- pos;
	tmp.rightShoulder = rightShoulder- pos;
	tmp.rightWrist = rightWrist- pos;

	tmp.shoulderCenter = shoulderCenter - pos;
	tmp.hipCenter = hipCenter - pos;
	tmp.spine = spine - pos;

	// Rotate skeleton
	res.head = R1 * tmp.head;

	res.leftAnkle = R1 * tmp.leftAnkle;	
	res.leftElbow = R1 * tmp.leftElbow;
	res.leftFoot = R1 * tmp.leftFoot;
	res.leftHand = R1 * tmp.leftHand;
	res.leftHip = R1 * tmp.leftHip;
	res.leftKnee = R1 * tmp.leftKnee;
	res.leftShoulder = R1 * tmp.leftShoulder;
	res.leftWrist = R1 * tmp.leftWrist;

	res.rightAnkle = R1 * tmp.rightAnkle;	
	res.rightElbow = R1 * tmp.rightElbow;
	res.rightFoot = R1 * tmp.rightFoot;
	res.rightHand = R1 * tmp.rightHand;
	res.rightHip = R1 * tmp.rightHip;
	res.rightKnee = R1 * tmp.rightKnee;
	res.rightShoulder = R1 * tmp.rightShoulder;
	res.rightWrist = R1 * tmp.rightWrist;

	res.shoulderCenter = R1 * tmp.shoulderCenter;
	res.hipCenter = R1 * tmp.hipCenter;
	res.spine = R1 * tmp.spine;
	
	return res;

}


void SkeletonRepresentation::Serialize( Json::Value& root )
{
	// serialize primitives
	root["version"] = m_version;
	root["frameNumber"] = m_frameNumber;
	for (size_t i = 0; i < 3; i++) 
	{
		root["joint.head"].append(head[i]);
		root["joint.leftShoulder"].append(leftShoulder[i]);
	}
}


// Note that at this point we are only taking the skeleton with index 0 and discarding all the other data!!!!!!!
void SkeletonRepresentation::Deserialize( Json::Value& root )
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

			this->id = (*it).get("id", index).asUInt();
			this->head = jsonGetPoint3((*it).get("head", Json::nullValue));

			this->leftShoulder = jsonGetPoint3((*it).get("leftShoulder", Json::nullValue));
			this->rightShoulder = jsonGetPoint3((*it).get("rightShoulder", Json::nullValue));
			this->leftElbow = jsonGetPoint3((*it).get("leftElbow", Json::nullValue));
			this->rightElbow = jsonGetPoint3((*it).get("rightElbow", Json::nullValue));
			this->leftWrist = jsonGetPoint3((*it).get("leftWrist", Json::nullValue));
			this->rightWrist = jsonGetPoint3((*it).get("rightWrist", Json::nullValue));

			this->leftHip = jsonGetPoint3((*it).get("leftHip", Json::nullValue));
			this->rightHip = jsonGetPoint3((*it).get("rightHip", Json::nullValue));
			this->leftKnee = jsonGetPoint3((*it).get("leftKnee", Json::nullValue));
			this->rightKnee = jsonGetPoint3((*it).get("rightKnee", Json::nullValue));
			this->leftAnkle = jsonGetPoint3((*it).get("leftAnkle", Json::nullValue));
			this->rightAnkle = jsonGetPoint3((*it).get("rightAnkle", Json::nullValue));

			this->leftFoot = jsonGetPoint3((*it).get("leftFoot", Json::nullValue));
			this->rightFoot = jsonGetPoint3((*it).get("rightFoot", Json::nullValue));
			this->leftHand = jsonGetPoint3((*it).get("leftHand", Json::nullValue));
			this->rightHand = jsonGetPoint3((*it).get("rightHand", Json::nullValue));

			this->hipCenter = jsonGetPoint3((*it).get("hipCenter", Json::nullValue));
			this->shoulderCenter = jsonGetPoint3((*it).get("shoulderCenter", Json::nullValue));
			this->spine = jsonGetPoint3((*it).get("spine", Json::nullValue));

			index++;
		}
	}

}



/** 
* Returns Point3 structure for give input json array
*/
Eigen::Vector3f SkeletonRepresentation::jsonGetPoint3(const Json::Value &jsonValue)
{
	Eigen::Vector3f point;

	if (jsonValue.isNull()) return Eigen::Vector3f();

	if (jsonValue.isArray() && jsonValue.size() == 3)
	{
		Json::ValueIterator it = jsonValue.begin();
		point[0] = static_cast<float>((*it).asDouble());
		it++;
		point[1] = static_cast<float>((*it).asDouble());
		it++;
		point[2] = static_cast<float>((*it).asDouble());
	}
	else
		;//throw Exception("Error in PoseSet::jsonGetPoint3(): value size = %d", jsonValue.size());

	return point;
}



SkeletonRepresentationSequence::SkeletonRepresentationSequence()
{

}


void SkeletonRepresentationSequence::Serialize( Json::Value& root )
{

}

/** 
* Deserialize JSON formatted skeleton sequence
*/
void SkeletonRepresentationSequence::Deserialize(Json::Value& root)
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
			SkeletonRepresentation skeletonFrame;
			skeletonFrame.Deserialize(*it);

			m_v.push_back(skeletonFrame);
			index++;
		}
	}
}