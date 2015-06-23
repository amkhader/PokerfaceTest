#include "libSkeleton.h"
#include <iostream>


Skeleton::Skeleton(float _version):
m_version(_version),
m_numberOfJoints(Joint_Count),
m_tracked(false),
m_id(0)
{

	// initialize skeleton representation
	m_skeleton.resize(m_numberOfJoints, Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	m_detectionScore.resize(m_numberOfJoints, 0.0f);
	m_skeletonOrientations.resize(m_numberOfJoints, Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
	m_skeletonProjected.resize(m_numberOfJoints, Eigen::Vector2f(0.0f, 0.0f));


}

Skeleton::~Skeleton(void)
{
	m_skeleton.clear();
	m_detectionScore.clear();
	m_skeletonOrientations.clear();
	m_skeletonProjected.clear();

	m_numberOfJoints = 0;
}


void Skeleton::setJoint(JointType index, const Eigen::Vector3f &position)
{
	if (index >= static_cast<int>(m_skeleton.size())) return;
	m_skeleton.at(index) = position;
}

void Skeleton::setJoint(JointType index, float x, float y, float z)
{
	if (index >= static_cast<int>(m_skeleton.size())) return;
	m_skeleton.at(index) = Eigen::Vector3f(x, y, z);
}

Eigen::Vector3f Skeleton::getJoint(JointType index)
{
	if (index >= static_cast<int>(m_skeleton.size())) return Eigen::Vector3f::Zero();
	else return m_skeleton.at(index);
}

void Skeleton::setJointScore(JointType index, float score)
{
	if (index >= static_cast<int>(m_detectionScore.size())) return;
	m_detectionScore.at(index) = score;
}

float Skeleton::getJointScore(JointType index)
{
	if (index >= static_cast<int>(m_detectionScore.size())) return 0.0f;
	else return m_detectionScore.at(index);
}

/**
 *	Set joint quaternion
 */
void Skeleton::setJointQuaternion(JointType index, const Eigen::Vector4f &q)
{
	if (index >= static_cast<int>(m_skeletonOrientations.size())) return;
	m_skeletonOrientations.at(index) = q;
}

/**
 *	Set joint quaternion by components
 */
void Skeleton::setJointQuaternion(JointType index, float x, float y, float z, float w)
{
	if (index >= static_cast<int>(m_skeletonOrientations.size())) return;
	m_skeletonOrientations.at(index) = Eigen::Vector4f(x, y, z, w);
}

/**
 *	Get joint quaternion
 */
Eigen::Vector4f Skeleton::getJointQuaternion(JointType index)
{
	if (index >= static_cast<int>(m_skeletonOrientations.size())) return Eigen::Vector4f::Zero();
	else return m_skeletonOrientations.at(index);
}

/**
 *	Set skeleton to image projection
 */
void Skeleton::setJoint2D(JointType index, const Eigen::Vector2f &position)
{
	if (index >= static_cast<int>(m_skeletonProjected.size())) return;
	m_skeletonProjected.at(index) = position;
}

/**
 *	Set skeleton to image projection
 */
void Skeleton::setJoint2D(JointType index, float x, float y)
{
	if (index >= static_cast<int>(m_skeletonProjected.size())) return;
	m_skeletonProjected.at(index) = Eigen::Vector2f(x, y);
}

/**
 *	get skeleton image projection
 */
Eigen::Vector2f Skeleton::getJoint2D(JointType index) const
{
	if (index >= static_cast<int>(m_skeletonProjected.size())) return Eigen::Vector2f::Zero();
	else return m_skeletonProjected.at(index);
}



void Skeleton::head(Eigen::Vector3f position) { m_skeleton.at(Joint_Head) = position; }
Eigen::Vector3f Skeleton::head() { return m_skeleton.at(Joint_Head); }

void Skeleton::neck(Eigen::Vector3f position) { m_skeleton.at(Joint_Neck) = position; }
Eigen::Vector3f Skeleton::neck() { return m_skeleton.at(Joint_Neck); }

// upper extremities
void Skeleton::leftShoulder(Eigen::Vector3f position) { m_skeleton.at(Joint_ShoulderLeft) = position; }
Eigen::Vector3f Skeleton::leftShoulder() { return m_skeleton.at(Joint_ShoulderLeft); }

void Skeleton::leftElbow(Eigen::Vector3f position) { m_skeleton.at(Joint_ElbowLeft) = position; }
Eigen::Vector3f Skeleton::leftElbow() { return m_skeleton.at(Joint_ElbowLeft); }

void Skeleton::leftWrist(Eigen::Vector3f position) { m_skeleton.at(Joint_WristLeft) = position; }
Eigen::Vector3f Skeleton::leftWrist() { return m_skeleton.at(Joint_WristLeft); }

void Skeleton::leftHand(Eigen::Vector3f position) { m_skeleton.at(Joint_HandLeft) = position; }
Eigen::Vector3f Skeleton::leftHand() { return m_skeleton.at(Joint_HandLeft); }

void Skeleton::rightShoulder(Eigen::Vector3f position) { m_skeleton.at(Joint_ShoulderRight) = position; }
Eigen::Vector3f Skeleton::rightShoulder() { return m_skeleton.at(Joint_ShoulderRight); }

void Skeleton::rightElbow(Eigen::Vector3f position) { m_skeleton.at(Joint_ElbowRight) = position; }
Eigen::Vector3f Skeleton::rightElbow() { return m_skeleton.at(Joint_ElbowRight); }

void Skeleton::rightWrist(Eigen::Vector3f position) { m_skeleton.at(Joint_WristRight) = position; }
Eigen::Vector3f Skeleton::rightWrist() { return m_skeleton.at(Joint_WristRight); }

void Skeleton::rightHand(Eigen::Vector3f position) { m_skeleton.at(Joint_HandRight) = position; }
Eigen::Vector3f Skeleton::rightHand() { return m_skeleton.at(Joint_HandRight); }

// lower body
void Skeleton::leftHip(Eigen::Vector3f position) { m_skeleton.at(Joint_HipLeft) = position; }
Eigen::Vector3f Skeleton::leftHip() { return m_skeleton.at(Joint_HipLeft); }

void Skeleton::leftKnee(Eigen::Vector3f position) { m_skeleton.at(Joint_KneeLeft) = position; }
Eigen::Vector3f Skeleton::leftKnee() { return m_skeleton.at(Joint_KneeLeft); }

void Skeleton::leftAnkle(Eigen::Vector3f position) { m_skeleton.at(Joint_AnkleLeft) = position; }
Eigen::Vector3f Skeleton::leftAnkle() { return m_skeleton.at(Joint_AnkleLeft); }

void Skeleton::leftFoot(Eigen::Vector3f position) { m_skeleton.at(Joint_FootLeft) = position; }
Eigen::Vector3f Skeleton::leftFoot() { return m_skeleton.at(Joint_FootLeft); }

void Skeleton::rightHip(Eigen::Vector3f position) { m_skeleton.at(Joint_HipRight) = position; }
Eigen::Vector3f Skeleton::rightHip() { return m_skeleton.at(Joint_HipRight); }

void Skeleton::rightKnee(Eigen::Vector3f position) { m_skeleton.at(Joint_KneeRight) = position; }
Eigen::Vector3f Skeleton::rightKnee() { return m_skeleton.at(Joint_KneeRight); }

void Skeleton::rightAnkle(Eigen::Vector3f position) { m_skeleton.at(Joint_AnkleRight) = position; }
Eigen::Vector3f Skeleton::rightAnkle() { return m_skeleton.at(Joint_AnkleRight); }

void Skeleton::rightFoot(Eigen::Vector3f position) { m_skeleton.at(Joint_FootRight) = position; }
Eigen::Vector3f Skeleton::rightFoot() { return m_skeleton.at(Joint_FootRight); }

// spine
void Skeleton::spineBase(Eigen::Vector3f position) { m_skeleton.at(Joint_SpineBase) = position; } // (hipCenter)
Eigen::Vector3f Skeleton::spineBase() { return m_skeleton.at(Joint_SpineBase); }

void Skeleton::spineShoulder(Eigen::Vector3f position) { m_skeleton.at(Joint_SpineShoulder) = position; } // (shoulderCenter)
Eigen::Vector3f Skeleton::spineShoulder() { return m_skeleton.at(Joint_SpineShoulder); }

void Skeleton::spineMid(Eigen::Vector3f position) { m_skeleton.at(Joint_SpineMid) = position; } //(spine)
Eigen::Vector3f Skeleton::spineMid() { return m_skeleton.at(Joint_SpineMid); }

// additional hand joints
void Skeleton::leftHandTip(Eigen::Vector3f position) { m_skeleton.at(Joint_HandTipLeft) = position; }
Eigen::Vector3f Skeleton::leftHandTip() { return m_skeleton.at(Joint_HandTipLeft); }

void Skeleton::leftThumb(Eigen::Vector3f position) { m_skeleton.at(Joint_ThumbLeft) = position; }
Eigen::Vector3f Skeleton::leftThumb() { return m_skeleton.at(Joint_ThumbLeft); }

void Skeleton::rightHandTip(Eigen::Vector3f position) { m_skeleton.at(Joint_HandTipRight) = position; }
Eigen::Vector3f Skeleton::rightHandTip() { return m_skeleton.at(Joint_HandTipRight); }

void Skeleton::rightThumb(Eigen::Vector3f position) { m_skeleton.at(Joint_ThumbRight) = position; }
Eigen::Vector3f Skeleton::rightThumb() { return m_skeleton.at(Joint_ThumbRight); }


/************************************************************************/
/* Transformation of skeleton using Rotation and translation            */
/************************************************************************/
Skeleton Skeleton::transformRT(Eigen::Matrix3f R, Eigen::Vector3f pos)
{

	Skeleton res = *this;
	Skeleton tmp = *this;

	// transpose rotation
	Eigen::Matrix3f R1 = R.transpose();

	// rotate / translate the skeleton
	for (size_t i = 0; i < m_numberOfJoints; i++)
	{
		tmp.m_skeleton[i] = R1 * (res.m_skeleton[i] - pos);
	}
	
	return res;

}

/**
 *	Skeleton saving function. Note that we will not back-support previous data format in writing.
 */
void Skeleton::save (std::ostream &os, io::SkeletonIOFlags pf) const
{
	// write io flags
	 writeInt (os, int (pf), pf);

	 // write delimiter ???
	 writeDelimiter (os, pf);

	// write skeleton representation version
	float version = m_version;
	writeFloat (os, version, pf);

	// write all the joint locations
	for (size_t i = 0; i < m_skeleton.size(); i ++)
	{
		writeFloat (os, m_skeleton.at(i).x(), pf);
		writeFloat (os, m_skeleton.at(i).y(), pf);
		writeFloat (os, m_skeleton.at(i).z(), pf);
	}

	writeDelimiter (os, pf);

	// write confidence
	if (pf & io::POSE_IO_JOINT_DETECTION_SCORES)
	{
		writeFloat (os, version, pf);
		for (size_t i = 0; i < m_detectionScore.size(); i ++)
		{
			writeFloat (os, m_detectionScore.at(i), pf);
		}

		writeDelimiter (os, pf);
	}

	// write quaternions
	/*float data3 [] = { headRotation.x,         headRotation.y,          headRotation.z,				headRotation.w,         
		leftShoulderRotation.x,  leftShoulderRotation.y,  leftShoulderRotation.z,		leftShoulderRotation.w, 
		rightShoulderRotation.x, rightShoulderRotation.y, rightShoulderRotation.z,		rightShoulderRotation.w,
		leftElbowRotation.x,     leftElbowRotation.y,     leftElbowRotation.z,			leftElbowRotation.w,    
		rightElbowRotation.x,    rightElbowRotation.y,    rightElbowRotation.z,			rightElbowRotation.w,   
		leftWristRotation.x,     leftWristRotation.y,     leftWristRotation.z,			leftWristRotation.w,    
		rightWristRotation.x,    rightWristRotation.y,    rightWristRotation.z,			rightWristRotation.w,   
		leftHipRotation.x,       leftHipRotation.y,       leftHipRotation.z,			leftHipRotation.w,      
		rightHipRotation.x,      rightHipRotation.y,      rightHipRotation.z,			rightHipRotation.w,     
		leftKneeRotation.x,      leftKneeRotation.y,      leftKneeRotation.z,			leftKneeRotation.w,     
		rightKneeRotation.x,     rightKneeRotation.y,     rightKneeRotation.z,			rightKneeRotation.w,    
		leftAnkleRotation.x,     leftAnkleRotation.y,     leftAnkleRotation.z,			leftAnkleRotation.w,    
		rightAnkleRotation.x,    rightAnkleRotation.y,    rightAnkleRotation.z,			rightAnkleRotation.w,   
		leftFootRotation.x,      leftFootRotation.y,      leftFootRotation.z,			leftFootRotation.w,     
		rightFootRotation.x,     rightFootRotation.y,     rightFootRotation.z,			rightFootRotation.w,    
		leftHandRotation.x,      leftHandRotation.y,      leftHandRotation.z,			leftHandRotation.w,     
		rightHandRotation.x,     rightHandRotation.y,     rightHandRotation.z,			rightHandRotation.w,    
		shoulderCenterRotation.x,	 shoulderCenterRotation.y,	shoulderCenterRotation.z,	shoulderCenterRotation.w,
		hipCenterRotation.x,	 hipCenterRotation.y,	 hipCenterRotation.z,			hipCenterRotation.w,	 
		spineRotation.x,		 spineRotation.y,		spineRotation.z,				spineRotation.w,
	};

	if (pf & POSE_IO_ROTATIONS)
	{
		writeFloat (os, version, pf);
		for (int i = 0; i < sizeof (data3) / sizeof (*data3); i ++)
			writeFloat (os, data3 [i], pf);

		writeDelimiter (os, pf);
	}*/

}

void Skeleton::load (std::istream &is, io::SkeletonIOFlags pf, float headerVersion)
{

}


/** 
* Returns JSON array from input Vector structure
*/
Json::Value Skeleton::jsonAddPoint3(Eigen::Vector3f point)
{
	Json::Value jsonVector;
	jsonVector.append(point.x());
	jsonVector.append(point.y());
	jsonVector.append(point.z());

	return jsonVector;
}

/**
* Serialize single skeleton pose into JSON string
*/
void Skeleton::Serialize( Json::Value& skeleton )
{
	skeleton["id"] = this->m_id;
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
void Skeleton::Deserialize( Json::Value& root )
{
	this->m_id = root.get("id", this->m_id).asUInt();
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
Eigen::Vector3f Skeleton::jsonGetPoint3(const Json::Value &jsonValue)
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


