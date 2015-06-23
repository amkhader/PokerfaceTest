#pragma once
/********************************************************************
	created:	2014/09/30
	filename: 	SkeletonRepresentation.h
	file path:	libSkeleton\
	file base:	SkeletonRepresentation
	file ext:	h
	author:		Gregorij Kurillo
	
	purpose:	Skeleton representation
	comment:
*********************************************************************/

#include "serializer.h"
#include <Eigen/Eigen>
#include <Eigen/StdVector>
#include "io.h"

/** 
* Single skeleton representation
*/
class Skeleton: public IJsonSerializable
{
public:
	Skeleton(float version = 1.0f);
	~Skeleton(void);


	enum JointType	// This matches Kinect 2!
	{
		Joint_SpineBase	= 0,
		Joint_SpineMid	= 1,
		Joint_Neck	= 2,
		Joint_Head	= 3,
		Joint_ShoulderLeft	= 4,
		Joint_ElbowLeft	= 5,
		Joint_WristLeft	= 6,
		Joint_HandLeft	= 7,
		Joint_ShoulderRight	= 8,
		Joint_ElbowRight	= 9,
		Joint_WristRight	= 10,
		Joint_HandRight	= 11,
		Joint_HipLeft	= 12,
		Joint_KneeLeft	= 13,
		Joint_AnkleLeft	= 14,
		Joint_FootLeft	= 15,
		Joint_HipRight	= 16,
		Joint_KneeRight	= 17,
		Joint_AnkleRight	= 18,
		Joint_FootRight	= 19,
		Joint_SpineShoulder	= 20,
		Joint_HandTipLeft	= 21,
		Joint_ThumbLeft	= 22,
		Joint_HandTipRight	= 23,
		Joint_ThumbRight	= 24,
		Joint_Count	= ( Joint_ThumbRight + 1 ) 
	} ;


	// set tracking
	bool isTracked() const { return m_tracked; }
	void setTracked(bool status) { m_tracked = status; }

	// set/get skeleton ID
	void setId(int id) {m_id = id;}
	int getId() const {return m_id;}

	// set/get version
	void setVersion(float ver) { m_version = ver; }
	float getVersion() const {return m_version;}

	// get number of joints
	size_t getJointCount() const { return m_numberOfJoints; }

	// set joint score
	void setJointScore(JointType index, float score);
	float getJointScore(JointType index);

	// set orientation by number. TODO: make named calls?
	void setJointQuaternion(JointType index, const Eigen::Vector4f &q);
	void setJointQuaternion(JointType index, float x, float y, float z, float w);
	Eigen::Vector4f getJointQuaternion(JointType index);

	// set projected skeleton by number
	void setJoint2D(JointType index, const Eigen::Vector2f &position);
	void setJoint2D(JointType index, float x, float y);
	Eigen::Vector2f getJoint2D(JointType index) const;

	// assign joint by number
	void setJoint(JointType index, const Eigen::Vector3f &position);
	void setJoint(JointType index, float x, float y, float z);
	Eigen::Vector3f getJoint(JointType index);

	// head and neck
	void head(Eigen::Vector3f position);
	Eigen::Vector3f head();

	void neck(Eigen::Vector3f position);
	Eigen::Vector3f neck();

	// upper extremities
	void leftShoulder(Eigen::Vector3f position);
	Eigen::Vector3f leftShoulder();
	
	void leftElbow(Eigen::Vector3f position);
	Eigen::Vector3f leftElbow();

	void leftWrist(Eigen::Vector3f position);
	Eigen::Vector3f leftWrist();

	void leftHand(Eigen::Vector3f position);
	Eigen::Vector3f leftHand();

	void rightShoulder(Eigen::Vector3f position);
	Eigen::Vector3f rightShoulder();

	void rightElbow(Eigen::Vector3f position);
	Eigen::Vector3f rightElbow();

	void rightWrist(Eigen::Vector3f position);
	Eigen::Vector3f rightWrist();

	void rightHand(Eigen::Vector3f position);
	Eigen::Vector3f rightHand();

	// lower body
	void leftHip(Eigen::Vector3f position);
	Eigen::Vector3f leftHip();

	void leftKnee(Eigen::Vector3f position);
	Eigen::Vector3f leftKnee();

	void leftAnkle(Eigen::Vector3f position);
	Eigen::Vector3f leftAnkle();

	void leftFoot(Eigen::Vector3f position);
	Eigen::Vector3f leftFoot();

	void rightHip(Eigen::Vector3f position);
	Eigen::Vector3f rightHip();

	void rightKnee(Eigen::Vector3f position);
	Eigen::Vector3f rightKnee();

	void rightAnkle(Eigen::Vector3f position);
	Eigen::Vector3f rightAnkle();

	void rightFoot(Eigen::Vector3f position);
	Eigen::Vector3f rightFoot();
	
	// spine
	void spineBase(Eigen::Vector3f position); // (hipCenter)
	Eigen::Vector3f spineBase();

	void spineShoulder(Eigen::Vector3f position); // (shoulderCenter)
	Eigen::Vector3f spineShoulder();

	void spineMid(Eigen::Vector3f position); //(spine)
	Eigen::Vector3f spineMid();

	// additional hand joints
	void leftHandTip(Eigen::Vector3f position);
	Eigen::Vector3f leftHandTip();

	void leftThumb(Eigen::Vector3f position);
	Eigen::Vector3f leftThumb();

	void rightHandTip(Eigen::Vector3f position);
	Eigen::Vector3f rightHandTip();

	void rightThumb(Eigen::Vector3f position);
	Eigen::Vector3f rightThumb();

	/// Transform skeleton for given Rotation and Translation
	Skeleton transformRT(Eigen::Matrix3f R, Eigen::Vector3f t);


protected:
	friend class SkeletonSet;
		void save (std::ostream &os, io::SkeletonIOFlags pf) const;
		void load (std::istream &is, io::SkeletonIOFlags pf, float headerVersion);


private:
	std::vector<Eigen::Vector3f> m_skeleton;	///< Collection of joint locations
	std::vector<float> m_detectionScore;		///< Detection score, 0.0 - joint invisible, 0.5 - joint infered, 1.0 - joint visible
	std::vector<Eigen::Vector4f, Eigen::aligned_allocator<Eigen::Vector4f>> m_skeletonOrientations;		// must use special allocator!!!
	std::vector<Eigen::Vector2f> m_skeletonProjected;

	size_t m_numberOfJoints;					///< Number of joints in the structure
	bool m_tracked;								///< skeleton is being tracked
	float m_version;		///< version
	float m_kinectVersion;
	int m_id;


public:
	/// De-/Serialization from/to JSON
	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);

private:
	Eigen::Vector3f jsonGetPoint3(const Json::Value &jsonValue);
	Json::Value jsonAddPoint3(Eigen::Vector3f point);

};

