#pragma once
#include "serializer.h"

#include <Eigen/Geometry>

/*********************************************************************************/
/* This class reflects Stepan's PoseRepresentation class using Eigen::Vector     */
/*********************************************************************************/
class SkeletonRepresentation : public IJsonSerializable
{
public:
	SkeletonRepresentation(double version = 1.0);
	~SkeletonRepresentation(void);

	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);

	double m_version;		///< version
	double m_kinectVersion;
	unsigned int m_frameNumber;
	double m_timeStamp;
	int id;

	/* Joint positions */
	Eigen::Vector3f head;
	Eigen::Vector3f neck;

	// upper body
	Eigen::Vector3f leftShoulder, rightShoulder;
	Eigen::Vector3f leftElbow, rightElbow;
	Eigen::Vector3f leftWrist, rightWrist;
	Eigen::Vector3f leftHand, rightHand;

	// lower body
	Eigen::Vector3f leftHip, rightHip;
	Eigen::Vector3f leftKnee, rightKnee;
	Eigen::Vector3f leftAnkle, rightAnkle;
	Eigen::Vector3f leftFoot, rightFoot;

	// additional joints that were not in Stepan's original skeleton
	Eigen::Vector3f spineBase;		//spineBase		(hipCenter)
	Eigen::Vector3f spineShoulder;	//spineShoulder	(shoulderCenter)
	Eigen::Vector3f spineMid;			//SpineMid		(spine)

	// additional hand joints
	Eigen::Vector3f leftHandTip, rightHandTip;
	Eigen::Vector3f leftThumb, rightThumb;


	/// Transform skeleton for given Rotation and Translation
	SkeletonRepresentation transformRT(Eigen::Matrix3f R, Eigen::Vector3f t);

private:
	  Eigen::Vector3f jsonGetPoint3(const Json::Value &jsonValue);

};



class SkeletonRepresentationSequence : public IJsonSerializable
{
public:
	SkeletonRepresentationSequence();

	// vector-like behavior
	size_t size() const { return m_v.size(); }
	void push_back(const SkeletonRepresentation &ps) { m_v.push_back(ps); }
	SkeletonRepresentation &operator [] (size_t n) { return (m_v[n]); }
	const SkeletonRepresentation &operator [] (size_t n) const { return (m_v[n]); }
	SkeletonRepresentation & at(size_t n) { return m_v.at(n); }
	const SkeletonRepresentation & at(size_t n) const { return m_v.at(n); }
	void resize(size_t n) { m_v.resize(n); }
	void reserve(size_t n) { m_v.reserve(n); }
	void clear() { m_v.clear(); }

	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);

	// member variables
	double version;

private:
	std::vector<SkeletonRepresentation> m_v;

};

