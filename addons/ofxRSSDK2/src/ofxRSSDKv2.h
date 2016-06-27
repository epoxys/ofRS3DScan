#ifndef __OFX_RSSDK_H__
#define __OFX_RSSDK_H__
#ifdef _DEBUG
#pragma comment(lib, "libpxcmd_d.lib")
#else
#pragma comment(lib, "libpxcmd.lib")
#endif
#include <memory>
#include "ofMain.h"
#include "pxcsensemanager.h"
#include "pxcprojection.h"
#include "pxcblobmodule.h"
#include "pxcfacemodule.h"
#include "pxcfaceconfiguration.h"
#include "pxc3dscan.h"
#include "pxcimage.h"

using namespace std;

//ADDED------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------

enum FaceStatusDetected
{
	FaceStatusDetected_Detected,
	FaceStatusDetected_NotDetected
};

enum FaceStatusXAxis
{
	FaceStatusXAxis_InRange,
	FaceStatusXAxis_TooFarLeft,
	FaceStatusXAxis_TooFarRight
};

enum FaceStatusYAxis
{
	FaceStatusYAxis_InRange,
	FaceStatusYAxis_TooFarUp,
	FaceStatusYAxis_TooFarDown
};

enum FaceStatusZAxis
{
	FaceStatusZAxis_InRange,
	FaceStatusZAxis_TooClose,
	FaceStatusZAxis_TooFar
};

enum FaceStatusYaw
{
	FaceStatusYaw_InRange,
	FaceStatusYaw_TooFarLeft,
	FaceStatusYaw_TooFarRight,
};

enum FaceStatusPitch
{
	FaceStatusPitch_InRange,
	FaceStatusPitch_TooFarUp,
	FaceStatusPitch_TooFarDown,
};
enum ObjectRange
{
	Object_InRange,
	Object_TooClose,
	Object_TooFar
};
enum ObjectStructure 
{
	Object_SufficientStructure,
	Object_InsufficientStructure
};
enum ObjectTracking 
{
	Object_Tracking,
	Object_LostTracking
};

//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------

namespace ofxRSSDK
{
	class RSDevice;
	typedef unique_ptr<RSDevice> RSDevicePtr;
	typedef shared_ptr<RSDevice> RSDeviceRef;

	enum DepthRes
	{
		R200_SD,	// 480x360
		R200_VGA,	// 628x468
		F200_VGA,	// 640x480 
		QVGA		// 320x240
	};

	enum RGBRes
	{
		VGA,
		HD720,
		HD1080
	};

	enum AlignMode
	{
		ALIGN_FRAME,
		ALIGN_UVS_ONLY
	};

	enum CloudRes
	{
		FULL_RES=1,
		HALF_RES=2,
		Q_RES=4
	};

	class RSDevice
	{
	protected:
		RSDevice();
	public:
		~RSDevice();
		static RSDevicePtr createUniquePtr() { return RSDevicePtr(new RSDevice()); }
		static RSDeviceRef createSharedPtr() { return RSDeviceRef(new RSDevice()); }
		bool init();
		bool initRgb(const RGBRes& pSize, const float& pFPS);
		bool initDepth(const DepthRes& pSize, const float& pFPS, bool pAsColor);

		//ADDED STUFF------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------


		bool initScan();

		FaceStatusDetected FaceScan_StatusDetected();
		FaceStatusXAxis FaceScan_StatusXAxis();
		FaceStatusYAxis FaceScan_StatusYAxis();
		FaceStatusZAxis FaceScan_StatusZAxis();
		FaceStatusYaw FaceScan_StatusYaw();
		FaceStatusPitch FaceScan_StatusPitch();

		ObjectRange Scan_StatusRange();
		ObjectStructure Scan_StatusStructure();
		ObjectTracking Scan_StatusTracking();

		PXC3DScan::AlertData scanGetAlertData();


		bool scanAcquire();
		void scanRelease();
		bool scanUpdate();

		void scanStart();
		void scanCancel();

		bool isScanActive();

		void saveScan();

		PXCImage *getColorImage();

		ofPixels		rgbFrame,
						depthFrameColor;
		ofShortPixels depthFrame;

		bool		getColor,
					getDepth,
					getDepthColor,
					getPCloud;

		uint16_t	*rawDepth;
		

		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------

		void enableAlignedImages(bool pState = true, AlignMode pMode = AlignMode::ALIGN_UVS_ONLY) { mShouldAlign = pState; mAlignMode = pMode; }
		void enablePointCloud(CloudRes pCloudRes, float pMinDepth, float pMaxDepth) { mCloudRes=pCloudRes; mShouldGetPointCloud=true; mPointCloudRange = ofVec2f(pMinDepth,pMaxDepth);}
		bool enableFaceTracking(bool pUseDepth);
		bool enableBlobTracking();

		void setPointCloudRange(float pMin, float pMax);

		bool start();
		bool update();
		bool stop();

		const ofPixels&	getRgbFrame();
		const ofShortPixels&	getDepthFrame();
		const ofPixels&	getDepth8uFrame();
		const ofPixels&	getColorMappedToDepthFrame();
		const ofPixels&	getDepthMappedToColorFrame();
		vector<ofVec3f> getPointCloud();
		//Nomenclature Notes:
		//	"Space" denotes a 3d coordinate
		//	"Image" denotes an image space point ((0, width), (0,height), (image depth))
		//	"Coords" denotes texture space (U,V) coordinates
		//  "Frame" denotes a full Surface

		//get a camera space point from a depth image point
		const ofPoint		getDepthSpacePoint(float pImageX, float pImageY, float pImageZ);
		const ofPoint		getDepthSpacePoint(int pImageX, int pImageY, uint16_t pImageZ);
		const ofPoint		getDepthSpacePoint(ofPoint pImageCoords);

		//get a ofColor object from a depth image point
		const ofColor		getColorFromDepthImage(float pImageX, float pImageY, float pImageZ);
		const ofColor		getColorFromDepthImage(int pImageX, int pImageY, uint16_t pImageZ);
		const ofColor		getColorFromDepthImage(ofPoint pImageCoords);

		//get a ofColor object from a depth camera space point
		const ofColor		getColorFromDepthSpace(float pCameraX, float pCameraY, float pCameraZ);
		const ofColor		getColorFromDepthSpace(ofPoint pCameraPoint);

		//get ofColor space UVs from a depth image point
		const ofVec2f		getColorCoordsFromDepthImage(float pImageX, float pImageY, float pImageZ);
		const ofVec2f		getColorCoordsFromDepthImage(int pImageX, int pImageY, uint16_t pImageZ);
		const ofVec2f		getColorCoordsFromDepthImage(ofPoint pImageCoords);

		//get ofColor space UVs from a depth space point
		const ofVec2f		getColorCoordsFromDepthSpace(float pCameraX, float pCameraY, float pCameraZ);
		const ofVec2f		getColorCoordsFromDepthSpace(ofPoint pCameraPoint);

		const ofVec2f&	getDepthSize() { return mDepthSize;  }
		const int		getDepthWidth() { return mDepthSize.x;  }
		const int		getDepthHeight() { return mDepthSize.y; }

		const ofVec2f&	getRgbSize() { return mRgbSize; }
		const int		getRgbWidth() { return mRgbSize.x; }
		const int		getRgbHeight() { return mRgbSize.y; }

		//ADDED------------------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------------------

		
		//ADDED------------------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------------------

	private:
		void			updatePointCloud();
		void			updateFaces();
		void			updateBlobs();

		bool			mIsInit,
						mIsRunning,
						mHasRgb,
						mHasDepth,
						mShouldAlign,
						mShouldGetDepthAsColor,
						mShouldGetPointCloud,
						mShouldGetFaces,
						mShouldGetBlobs;

		AlignMode		mAlignMode;
		CloudRes		mCloudRes;

		ofVec2f			mPointCloudRange;
		ofVec2f			mDepthSize;
		ofVec2f			mRgbSize;
		ofPixels		mRgbFrame;
		ofPixels		mDepth8uFrame;
		ofPixels		mColorToDepthFrame;
		ofPixels		mDepthToColorFrame;
		ofShortPixels		mDepthFrame;

		PXCSenseManager		*mSenseMgr;
		PXCProjection		*mCoordinateMapper;
		PXCCapture::Sample	*mCurrentSample;

		PXCBlobModule		*mBlobTracker;
		PXCFaceModule		*mFaceTracker;

		vector<PXCPoint3DF32>	mInPoints3D;
		vector<PXCPoint3DF32>	mOutPoints3D;
		vector<PXCPointF32>		mOutPoints2D;
		vector<ofVec3f>			mPointCloud;
		uint16_t				*mRawDepth;
	};
};
#endif
