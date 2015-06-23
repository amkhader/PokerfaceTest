#pragma once
/********************************************************************
	created:	2014/10/01
	filename: 	SkeletonSet.h
	file path:	libSkeleton\
	file base:	SkeletonRepresentation
	file ext:	h
	author:		Gregorij Kurillo
	
	purpose:	Skeleton set representation
	comment:
*********************************************************************/

#include "Skeleton.h"


/** 
* Set of several skeletons
*/
class SkeletonSet: public IJsonSerializable
{
public:
	SkeletonSet(float version = SKELETON_FILE_VERSION);
	~SkeletonSet();

	struct Header {
		Header();
		Header(const Header &header);
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW;	/// must add this for the alignment issue of EIGEN 

		friend class SkeletonSet;
	
	protected:
		long m_timeStampSeconds;		///< time in seconds
		long m_timeStampMicroSeconds;	///< time in microseconds
		long m_frameNumber;				///< current frame number
		double m_timeStamp;				///< Time stamp in double format
		float m_version;				///< skeleton format version
		float m_kinectVersion;			///< Kinect version used to record
		
		Eigen::MatrixX4f m_matCameraToWorld;	///< 4x4 transform used to convert from camera to world coordinate system
	};


	// vector-like behavior
	size_t size() const { return m_v.size(); }
	void push_back(const Skeleton &ps) { m_v.push_back(ps); }
	Skeleton &operator [] (size_t n) { return (m_v[n]); }
	const Skeleton &operator [] (size_t n) const { return (m_v[n]); }
	Skeleton & at(size_t n) { return m_v.at(n); }
	const Skeleton & at(size_t n) const { return m_v.at(n); }
	void resize(size_t n) { m_v.resize(n, Skeleton(m_header.m_version)); }
	void reserve(size_t n) { m_v.reserve(n); }
	void clear() { m_v.clear(); }

	// Load and save...
	void save (std::ostream &os, io:: SkeletonIOFlags pf) const;
	bool load (std::istream &is, io :: SkeletonIOFlags pf);

	// functions
	void setTimeStamp(long seconds, long microseconds) { m_header.m_timeStampSeconds = seconds; m_header.m_timeStampMicroSeconds = microseconds; }
	void getTimeStamp(long &seconds, long &microseconds) const { seconds = m_header.m_timeStampSeconds; microseconds = m_header.m_timeStampMicroSeconds; }

	double getTimeStampdDouble() const { return m_header.m_timeStamp; }
	void setTimeStampDouble(double t) { m_header.m_timeStamp = t; }

	void setFrameNumber(long frame) { m_header.m_frameNumber = frame; }
	long getFrameNumber() const { return m_header.m_frameNumber; }

	void setVersion(float ver) { m_header.m_version = ver; }
	float getVersion() const { return m_header.m_version; }

	void setKinectVersion(float ver) { m_header.m_kinectVersion = ver; }
	float getKinectVersion() const { return m_header.m_kinectVersion; }

	void setCameraToWorldTransform(const Eigen::Matrix4f &trans) { m_header.m_matCameraToWorld = trans; }
	Eigen::Matrix4f getCameraToWorldTranform() { return m_header.m_matCameraToWorld; }

	Header getHeader() const;
	void setHeader(const Header &header);


private:
	std::vector<Skeleton> m_v;	///< Vector of skeletons
	SkeletonSet::Header m_header;


public:
	virtual void Serialize( Json::Value& root );
	virtual void Deserialize( Json::Value& root);

private:

};

