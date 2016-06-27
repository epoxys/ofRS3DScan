#include <exception>
#include "ofxRSSDKv2.h"
#include "ofMain.h"

//ADDED------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
struct scanData {

	PXCSenseManager *sManager;
	PXC3DScan *dScan;

	bool frameAquired;

	PXCImage *colorImage;
	PXCImage::ImageData colorImageData;

	PXCImage *AcquiredPreviewImage;

	PXC3DScan::Configuration dScanConfig;

	bool dScanInProgress;

};

const char *alertMap[] =
{
	"ALERT_IN_RANGE",
	"ALERT_TOO_CLOSE",
	"ALERT_TOO_FAR",
	"ALERT_TRACKING",
	"ALERT_LOST_TRACKING",
	"ALERT_SUFFICIENT_STRUCTURE",
	"ALERT_INSUFFICIENT_STRUCTURE",
	"ALERT_FACE_DETECTED",
	"ALERT_FACE_NOT_DETECTED",
	"ALERT_FACE_X_IN_RANGE",
	"ALERT_FACE_X_TOO_FAR_LEFT",
	"ALERT_FACE_X_TOO_FAR_RIGHT",
	"ALERT_FACE_Y_IN_RANGE",
	"ALERT_FACE_Y_TOO_FAR_UP",
	"ALERT_FACE_Y_TOO_FAR_DOWN",
	"ALERT_FACE_Z_IN_RANGE",
	"ALERT_FACE_Z_TOO_CLOSE",
	"ALERT_FACE_Z_TOO_FAR",
	"ALERT_FACE_YAW_IN_RANGE",
	"ALERT_FACE_YAW_TOO_FAR_LEFT",
	"ALERT_FACE_YAW_TOO_FAR_RIGHT",
	"ALERT_FACE_PITCH_IN_RANGE",
	"ALERT_FACE_PITCH_TOO_FAR_UP",
	"ALERT_FACE_PITCH_TOO_FAR_DOWN"
};

class MyAlertHandler : public PXC3DScan::AlertHandler
{
public:
	MyAlertHandler()
	{
		Reset();
	}
	void Reset()
	{
		ZeroMemory(&Data, sizeof(Data));
		mStatusDetected = (FaceStatusDetected)0;
		mStatusXAxis = (FaceStatusXAxis)0;
		mStatusYAxis = (FaceStatusYAxis)0;
		mStatusZAxis = (FaceStatusZAxis)0;
		mStatusPitch = (FaceStatusPitch)0;
		mStatusYaw = (FaceStatusYaw)0;

		//added------------------
		mStatusRange = (ObjectRange)0;
		mStatusTracking = (ObjectTracking)0;
		mStatusStructure = (ObjectStructure)0;

	}
	virtual void PXCAPI OnAlert(const PXC3DScan::AlertData& data)
	{
		Data = data;
		int labelIndex = data.label;
		if (labelIndex >= 0 && labelIndex < ARRAYSIZE(alertMap))
		{
			ofLog(OF_LOG_NOTICE, "Alert " + ofToString(alertMap[labelIndex]));
		}
		switch (data.label)
		{
		case PXC3DScan::ALERT_FACE_DETECTED: { mStatusDetected = FaceStatusDetected_Detected; } break;
		case PXC3DScan::ALERT_FACE_NOT_DETECTED: { mStatusDetected = FaceStatusDetected_NotDetected; } break;

		case PXC3DScan::ALERT_FACE_X_IN_RANGE: { mStatusXAxis = FaceStatusXAxis_InRange; } break;
		case PXC3DScan::ALERT_FACE_X_TOO_FAR_LEFT: { mStatusXAxis = FaceStatusXAxis_TooFarLeft; } break;
		case PXC3DScan::ALERT_FACE_X_TOO_FAR_RIGHT: { mStatusXAxis = FaceStatusXAxis_TooFarRight; } break;

		case PXC3DScan::ALERT_FACE_Y_IN_RANGE: { mStatusYAxis = FaceStatusYAxis_InRange; } break;
		case PXC3DScan::ALERT_FACE_Y_TOO_FAR_UP: { mStatusYAxis = FaceStatusYAxis_TooFarUp; } break;
		case PXC3DScan::ALERT_FACE_Y_TOO_FAR_DOWN: {mStatusYAxis = FaceStatusYAxis_TooFarDown; } break;

		case PXC3DScan::ALERT_FACE_Z_IN_RANGE: { mStatusZAxis = FaceStatusZAxis_InRange; } break;
		case PXC3DScan::ALERT_FACE_Z_TOO_CLOSE: { mStatusZAxis = FaceStatusZAxis_TooClose; } break;
		case PXC3DScan::ALERT_FACE_Z_TOO_FAR: { mStatusZAxis = FaceStatusZAxis_TooFar; } break;

		case PXC3DScan::ALERT_FACE_YAW_IN_RANGE: { mStatusYaw = FaceStatusYaw_InRange; } break;
		case PXC3DScan::ALERT_FACE_YAW_TOO_FAR_LEFT: { mStatusYaw = FaceStatusYaw_TooFarLeft; } break;
		case PXC3DScan::ALERT_FACE_YAW_TOO_FAR_RIGHT: { mStatusYaw = FaceStatusYaw_TooFarRight; } break;

		case PXC3DScan::ALERT_FACE_PITCH_IN_RANGE: { mStatusPitch = FaceStatusPitch_InRange; } break;
		case PXC3DScan::ALERT_FACE_PITCH_TOO_FAR_UP: { mStatusPitch = FaceStatusPitch_TooFarUp; } break;
		case PXC3DScan::ALERT_FACE_PITCH_TOO_FAR_DOWN: { mStatusPitch = FaceStatusPitch_TooFarDown; } break;

		//ADDED--------------------------------------------------- essential for object scanning -----------
		
		case PXC3DScan::ALERT_IN_RANGE: { mStatusRange = Object_InRange;  } break;
		case PXC3DScan::ALERT_TOO_CLOSE: { mStatusRange = Object_TooClose; } break;
		case PXC3DScan::ALERT_TOO_FAR: { mStatusRange = Object_TooFar; } break;

		case PXC3DScan::ALERT_TRACKING: { mStatusTracking = Object_Tracking; } break;
		case PXC3DScan::ALERT_LOST_TRACKING: { mStatusTracking = Object_LostTracking; } break;
		
		case PXC3DScan::ALERT_SUFFICIENT_STRUCTURE: { mStatusStructure = Object_SufficientStructure; } break;
		case PXC3DScan::ALERT_INSUFFICIENT_STRUCTURE: { mStatusStructure = Object_InsufficientStructure; } break;

		}
	}
	PXC3DScan::AlertData Data;

	FaceStatusDetected GetStatusDetected() { return mStatusDetected; }
	FaceStatusXAxis GetStatusXAxis() { return mStatusXAxis; }
	FaceStatusYAxis GetStatusYAxis() { return mStatusYAxis; }
	FaceStatusZAxis GetStatusZAxis() { return mStatusZAxis; }
	FaceStatusYaw GetStatusYaw() { return mStatusYaw; }
	FaceStatusPitch GetStatusPitch() { return mStatusPitch; }

	//added-------------------------
	ObjectRange GetStatusRange() { return mStatusRange; }
	ObjectTracking GetStatusTracking() { return mStatusTracking; }
	ObjectStructure GetStatusStructure() { return mStatusStructure; }

private:

	FaceStatusDetected mStatusDetected;
	FaceStatusXAxis mStatusXAxis;
	FaceStatusYAxis mStatusYAxis;
	FaceStatusZAxis mStatusZAxis;
	FaceStatusYaw mStatusYaw;
	FaceStatusPitch mStatusPitch;

	//added---------------------------
	ObjectRange mStatusRange;
	ObjectTracking mStatusTracking;
	ObjectStructure mStatusStructure;
};

MyAlertHandler gAlertHandler;


static scanData scanGlob = {};



//ADDED------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------

namespace ofxRSSDK
{
	RSDevice::~RSDevice(){}
	RSDevice::RSDevice()
	{
		mIsInit = false;
		mIsRunning = false;
		mHasRgb = false;
		mHasDepth = false;
		mShouldAlign = false;
		mShouldGetDepthAsColor = false;
		mShouldGetPointCloud = false;
		mShouldGetFaces = false;
		mShouldGetBlobs = false;
		mPointCloudRange = ofVec2f(0,3000);
		mCloudRes = CloudRes::FULL_RES;

		
	}

#pragma region Init
	bool RSDevice::init()
	{
		mSenseMgr = PXCSenseManager::CreateInstance();
		if (mSenseMgr)
			mIsInit = true;

		return mIsInit;
	}

	bool RSDevice::initRgb(const RGBRes& pSize, const float& pFPS)
	{
		pxcStatus cStatus;
		if (mSenseMgr)
		{
			switch (pSize)
			{
			case RGBRes::VGA:
				mRgbSize = ofVec2f(640, 480);
				break;
			case RGBRes::HD720:
				mRgbSize = ofVec2f(1280, 720);
				break;
			case RGBRes::HD1080:
				mRgbSize = ofVec2f(1920, 1080);
				break;
			}
			cStatus = mSenseMgr->EnableStream(PXCCapture::STREAM_TYPE_COLOR, mRgbSize.x, mRgbSize.y, pFPS);
			if (cStatus >= PXC_STATUS_NO_ERROR)
			{
				mHasRgb = true;
				mRgbFrame.allocate(mRgbSize.x, mRgbSize.y,ofPixelFormat::OF_PIXELS_BGRA);
			}
		}

		return mHasRgb;
	}

	bool RSDevice::initDepth(const DepthRes& pSize, const float& pFPS, bool pAsColor)
	{
		pxcStatus cStatus;
		if (mSenseMgr)
		{
			switch (pSize)
			{
			case DepthRes::R200_SD:
				mDepthSize = ofVec2f(480,360);
				break;
			case DepthRes::R200_VGA:
				mDepthSize = ofVec2f(628,468);
				break;
			case DepthRes::F200_VGA:
				mDepthSize = ofVec2f(640,480);
				break;
			case DepthRes::QVGA:
				mDepthSize = ofVec2f(320,240);
				break;
			}
			cStatus = mSenseMgr->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, mDepthSize.x, mDepthSize.y, pFPS);
			if (cStatus >= PXC_STATUS_NO_ERROR)
			{
				mHasDepth = true;
				mShouldGetDepthAsColor = pAsColor;
				mDepthFrame.allocate(mDepthSize.x, mDepthSize.y,1);
				mDepth8uFrame.allocate(mDepthSize.x, mDepthSize.y, ofPixelFormat::OF_PIXELS_RGBA);
				mRawDepth = new uint16_t[(int)mDepthSize.x*(int)mDepthSize.y];
			}
		}
		return mHasDepth;
	}
#pragma endregion

	//ADDED------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------

	FaceStatusDetected RSDevice::FaceScan_StatusDetected() { return gAlertHandler.GetStatusDetected(); }
	FaceStatusXAxis RSDevice::FaceScan_StatusXAxis() { return gAlertHandler.GetStatusXAxis(); }
	FaceStatusYAxis RSDevice::FaceScan_StatusYAxis() { return gAlertHandler.GetStatusYAxis(); }
	FaceStatusZAxis RSDevice::FaceScan_StatusZAxis() { return gAlertHandler.GetStatusZAxis(); }
	FaceStatusYaw RSDevice::FaceScan_StatusYaw() { return gAlertHandler.GetStatusYaw(); }
	FaceStatusPitch RSDevice::FaceScan_StatusPitch() { return gAlertHandler.GetStatusPitch(); }

	//added------------------------------------------------------------------------------------------
	ObjectRange RSDevice::Scan_StatusRange() { return gAlertHandler.GetStatusRange(); }
	ObjectStructure RSDevice::Scan_StatusStructure() { return gAlertHandler.GetStatusStructure(); }
	ObjectTracking RSDevice::Scan_StatusTracking() { return gAlertHandler.GetStatusTracking(); }


	bool RSDevice::initScan() {

		gAlertHandler.Reset();

		PXCSenseManager *sm = PXCSenseManager::CreateInstance();
		pxcStatus status;

		status = sm->Enable3DScan();
		ofLog(OF_LOG_NOTICE, "INIT_DEBUG (pxcstatus enum): " + ofToString(status));


		PXC3DScan *fs = sm->Query3DScan();

		status = sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 1280, 720, 30.0f);
		ofLog(OF_LOG_NOTICE, "INIT_DEBUG (pxcstatus enum): " + ofToString(status));

		status = sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480, 30.0f);
		ofLog(OF_LOG_NOTICE, "INIT_DEBUG (pxcstatus enum): " + ofToString(status));

		status = sm->Init();
		ofLog(OF_LOG_NOTICE, "INIT_DEBUG (pxcstatus enum): " + ofToString(status));

		//for some reason texture building does not work?
		scanGlob.dScanConfig.startScan = false;
		scanGlob.dScanConfig.mode = PXC3DScan::ScanningMode::FACE;
		scanGlob.dScanConfig.options = PXC3DScan::ReconstructionOption::LANDMARKS;
		scanGlob.dScanConfig.maxTriangles = 0;
		

		status = fs->SetConfiguration(scanGlob.dScanConfig);
		ofLog(OF_LOG_NOTICE, "INIT_MODULE_DEBUG (pxcstatus enum): " + ofToString(status));


		fs->Subscribe(&gAlertHandler);

		scanGlob.dScan = fs;
		scanGlob.sManager = sm;

		
		if (status == 0) {
			
			rgbFrame.allocate(1280, 720, OF_PIXELS_BGRA);
			depthFrame.allocate(640, 480,1);
			depthFrameColor.allocate(640, 480, ofPixelFormat::OF_PIXELS_RGBA);

			rawDepth = new uint16_t[(int)640*(int)480];

			getColor = true;
			getDepth = true;
			getDepthColor = true;
			getPCloud = false;

			return true;

		}
		else {

			return false;
		}

	}


	PXC3DScan::AlertData RSDevice::scanGetAlertData() {
		return gAlertHandler.Data;
	}

	bool RSDevice::scanAcquire() {

		assert(!scanGlob.frameAquired);
		pxcStatus acquireStatus = scanGlob.sManager->AcquireFrame(false,0);

		scanGlob.dScanInProgress = scanGlob.dScan->IsScanning() != 0;

		scanGlob.frameAquired = acquireStatus >= pxcStatus::PXC_STATUS_NO_ERROR;

		//ofLog(OF_LOG_NOTICE, "try to acquire frame" + ofToString(acquireStatus));

		return scanGlob.frameAquired;

	}

	void RSDevice::scanRelease() {

		assert(scanGlob.frameAquired);

		if (scanGlob.AcquiredPreviewImage) {

			scanGlob.AcquiredPreviewImage->Release();
			scanGlob.AcquiredPreviewImage = NULL;
			
		}

		scanGlob.sManager->ReleaseFrame();
		scanGlob.frameAquired = false;

	}

	PXCImage *RSDevice::getColorImage() {

		assert(scanGlob.frameAquired);
		PXCCapture::Sample *sample = scanGlob.sManager->QuerySample();
		return sample->color;

	} //this is not used yet?

	void RSDevice::scanStart() {
		
		scanGlob.dScanConfig.startScan = true;
		scanGlob.dScanConfig.mode = PXC3DScan::ScanningMode::FACE;
		scanGlob.dScanConfig.options = PXC3DScan::ReconstructionOption::LANDMARKS;
		scanGlob.dScanConfig.maxTriangles = 0;

		scanGlob.dScan->SetConfiguration(scanGlob.dScanConfig);

	}

	void RSDevice::scanCancel() {

		scanGlob.dScanConfig.startScan = false;
		scanGlob.dScan->SetConfiguration(scanGlob.dScanConfig);

	}

	bool RSDevice::isScanActive() {

		return scanGlob.dScanInProgress;

	}

	void RSDevice::saveScan() {
		wchar_t filename[256];
		swprintf(filename, 256, L"%hs", "C:\\WERKMAP_TOM\\OF_9.3\\of_v0.9.3_vs_release\\apps\\myApps\\realsense3DScan\\bin\\data\\test.obj");
		scanGlob.dScan->Reconstruct(PXC3DScan::FileFormat::OBJ, filename);

	}

	bool RSDevice::scanUpdate() {
		
		if (scanGlob.sManager) {

			if (scanAcquire()) { //Dont forget to call scanRelease on all structure paths after acquiring!

				if (scanGlob.dScan->IsScanning()) {
					//is scanning, for now we are not going to try to get
					//color / depth / point cloud.
					ofLogNotice("SCANNING");


					
					scanRelease();
					return true;
				}
				else {
					ofLogNotice("IDLE");
					PXCCapture::Sample *currentSample = scanGlob.sManager->QuerySample();

					pxcStatus aStatus;

					if (!currentSample) {
						scanRelease();
						return false;
					}

					if (getColor) {
					//-- COLOR TO OFPIXELS --------------------------------------------------------------		
					//-----------------------------------------------------------------------------------
							if (!currentSample->color) {
								scanRelease();
								return false;
							}
							PXCImage *cColorImage = currentSample->color;
							PXCImage::ImageData colorData;

							aStatus = cColorImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &colorData);

							if (aStatus < PXC_STATUS_NO_ERROR) {

								cColorImage->ReleaseAccess(&colorData);
								scanRelease();
								return false;

							}

							rgbFrame.setFromExternalPixels(reinterpret_cast<uint8_t *>(colorData.planes[0]), 1280, 720, 4);
							cColorImage->ReleaseAccess(&colorData);
					
					}

					//--- DEPTH TO OFPIXELS ----------------------------------------------------------
					//-----------------------------------------------------------------------------------
					if (getDepth) {

						if (!currentSample->depth) {
							scanRelease();
							return false;
						}
						PXCImage *depthImage = currentSample->depth;
						PXCImage::ImageData depthData;

						aStatus = depthImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &depthData);

						if (aStatus < PXC_STATUS_NO_ERROR) {

							depthImage->ReleaseAccess(&depthData);
							scanRelease();
							return false;

						}
						
						depthFrame.setFromExternalPixels(reinterpret_cast<uint16_t *>(depthData.planes[0]), 640, 480, 1);
						memcpy(rawDepth, reinterpret_cast<uint16_t *>(depthData.planes[0]), (size_t)((int)640*(int)480 * sizeof(uint16_t)));
						depthImage->ReleaseAccess(&depthData);

						if (getDepthColor) {

							PXCImage::ImageData depthDataColor;

							aStatus = depthImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &depthDataColor);

							if (aStatus < PXC_STATUS_NO_ERROR) {

								depthImage->ReleaseAccess(&depthDataColor);
								scanRelease();
								return false;

							}

							depthFrameColor.setFromExternalPixels(reinterpret_cast<uint8_t *>(depthDataColor.planes[0]), 640, 480, 4);
							depthImage->ReleaseAccess(&depthDataColor);

						}
						
					}
										
					scanRelease();
					return true;
				}
				
			}
			else {

				//frame acquirement failed
				return false;

			}

		}
		else {

			//senseManager not available
			return false;

		}
		

	}

	//-----------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------

	void RSDevice::setPointCloudRange(float pMin=100.0f, float pMax=1500.0f)
	{
		mPointCloudRange = ofVec2f(pMin,pMax);
	}

	bool RSDevice::start()
	{
		pxcStatus cStatus = mSenseMgr->Init();
		if (cStatus >= PXC_STATUS_NO_ERROR)
		{
			mCoordinateMapper = mSenseMgr->QueryCaptureManager()->QueryDevice()->CreateProjection();
			if (mShouldAlign)
			{
				mColorToDepthFrame.allocate(mRgbSize.x, mRgbSize.y, ofPixelFormat::OF_PIXELS_RGBA);
				mDepthToColorFrame.allocate(mRgbSize.x, mRgbSize.y, ofPixelFormat::OF_PIXELS_RGBA);
			}
			mIsRunning = true;
			return true;
		}
		return false;
	}

	bool RSDevice::update()
	{
		pxcStatus cStatus;
		if (mSenseMgr)
		{
			cStatus = mSenseMgr->AcquireFrame(false,0);
			if (cStatus < PXC_STATUS_NO_ERROR)
				return false;
			PXCCapture::Sample *mCurrentSample = mSenseMgr->QuerySample();
			if (!mCurrentSample)
				return false;
			if (mHasRgb)
			{
				if (!mCurrentSample->color)
					return false;
				PXCImage *cColorImage = mCurrentSample->color;
				PXCImage::ImageData cColorData;
				cStatus = cColorImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &cColorData);
				if (cStatus < PXC_STATUS_NO_ERROR)
				{
					cColorImage->ReleaseAccess(&cColorData);
					return false;
				}
				mRgbFrame.setFromExternalPixels(reinterpret_cast<uint8_t *>(cColorData.planes[0]), mRgbSize.x, mRgbSize.y,4);

				cColorImage->ReleaseAccess(&cColorData);
				if (!mHasDepth)
				{	
					mSenseMgr->ReleaseFrame();
					return true;
				}
			}
			if (mHasDepth)
			{
				if (!mCurrentSample->depth)
					return false;
				PXCImage *cDepthImage = mCurrentSample->depth;
				PXCImage::ImageData cDepthData;
				cStatus = cDepthImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &cDepthData);
				
				if (cStatus < PXC_STATUS_NO_ERROR)
				{
					cDepthImage->ReleaseAccess(&cDepthData);
					return false;
				}
				mDepthFrame.setFromExternalPixels(reinterpret_cast<uint16_t *>(cDepthData.planes[0]), mDepthSize.x, mDepthSize.y, 1);
				memcpy(mRawDepth, reinterpret_cast<uint16_t *>(cDepthData.planes[0]), (size_t)((int)mDepthSize.x*(int)mDepthSize.y*sizeof(uint16_t)));			
				cDepthImage->ReleaseAccess(&cDepthData);

				if (mShouldGetDepthAsColor)
				{
					PXCImage::ImageData cDepth8uData;
					cStatus = cDepthImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &cDepth8uData);
					if (cStatus < PXC_STATUS_NO_ERROR)
					{
						cDepthImage->ReleaseAccess(&cDepth8uData);
						return false;
					}
					mDepth8uFrame.setFromExternalPixels(reinterpret_cast<uint8_t *>(cDepth8uData.planes[0]), mDepthSize.x, mDepthSize.y, 4);
					cDepthImage->ReleaseAccess(&cDepth8uData);
				}

				if(mShouldGetPointCloud)
				{
					updatePointCloud();
				}

				if (!mHasRgb)
				{
					mSenseMgr->ReleaseFrame();
					return true;
				}
			}

			if (mHasDepth&&mHasRgb&&mShouldAlign&&mAlignMode==AlignMode::ALIGN_FRAME)
			{
				PXCImage *cMappedColor = mCoordinateMapper->CreateColorImageMappedToDepth(mCurrentSample->depth, mCurrentSample->color);
				PXCImage *cMappedDepth = mCoordinateMapper->CreateDepthImageMappedToColor(mCurrentSample->color, mCurrentSample->depth);

				if (!cMappedColor || !cMappedDepth)
					return false;

				PXCImage::ImageData cMappedColorData;
				if (cMappedColor->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &cMappedColorData) >= PXC_STATUS_NO_ERROR)
				{
					mColorToDepthFrame.setFromExternalPixels(reinterpret_cast<uint8_t *>(cMappedColorData.planes[0]), mRgbSize.x, mRgbSize.y, 4);
					cMappedColor->ReleaseAccess(&cMappedColorData);
				}

				PXCImage::ImageData cMappedDepthData;
				if (cMappedDepth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &cMappedDepthData) >= PXC_STATUS_NO_ERROR)
				{
					mDepthToColorFrame.setFromExternalPixels(reinterpret_cast<uint8_t *>(cMappedDepthData.planes[0]), mRgbSize.x, mRgbSize.y, 4);
					cMappedDepth->ReleaseAccess(&cMappedDepthData);
				}

				try
				{
					cMappedColor->Release();
					cMappedDepth->Release();
				}
				catch (const exception &e)
				{
					ofLog(ofLogLevel::OF_LOG_WARNING, "Release check error: ");
					ofLog(ofLogLevel::OF_LOG_WARNING, e.what());
				}
			}
			mSenseMgr->ReleaseFrame();
			return true;
		}
		return false;
	}

	bool RSDevice::stop()
	{
		if (mSenseMgr)
		{
			mCoordinateMapper->Release();
			mSenseMgr->Close();
			if(mShouldGetBlobs)
			{
				if(mBlobTracker)
					mBlobTracker->Release();
			}
			if(mShouldGetFaces)
			{
				if(mFaceTracker)
					mFaceTracker->Release();
			}
			return true;
		}
		delete [] mRawDepth;
		return false;
	}

#pragma region Enable
		bool RSDevice::enableFaceTracking(bool pUseDepth)
		{
			if(mSenseMgr)
			{
				if(mSenseMgr->EnableFace()>=PXC_STATUS_NO_ERROR)
				{
					mFaceTracker = mSenseMgr->QueryFace();
					if(mFaceTracker)
					{
						PXCFaceConfiguration *config = mFaceTracker->CreateActiveConfiguration();
						switch(pUseDepth)
						{
						case true:
							config->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
							break;
						case false:
							config->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR);
							break;
						}
						config->ApplyChanges();
						config->Release();
						mShouldGetFaces = true;
					}
					else
						mShouldGetFaces = false;
					return mShouldGetFaces;
				}
				return false;
			}
			return false;
		}

		bool RSDevice::enableBlobTracking()
		{
			if(mSenseMgr)
			{
				if(mSenseMgr->EnableBlob()>=PXC_STATUS_NO_ERROR)
				{
					mBlobTracker = mSenseMgr->QueryBlob();
					if(mBlobTracker)
						mShouldGetBlobs = true;
					else
						mShouldGetBlobs = false;
					return mShouldGetBlobs;
				}
				return false;
			}
			return false;

		}
#pragma endregion

#pragma region Update
	void RSDevice::updatePointCloud()
	{
		int width = (int)mDepthSize.x;
		int height = (int)mDepthSize.y;
		int step = (int)mCloudRes;
		mPointCloud.clear();
		vector<PXCPoint3DF32> depthPoints, worldPoints;
		for (int dy = 0; dy < height;dy+=step)
		{
			for (int dx = 0; dx < width; dx+=step)
			{
				PXCPoint3DF32 cPoint;
				cPoint.x = dx; cPoint.y = dy; cPoint.z = (float)mRawDepth[dy*width + dx];
				if(cPoint.z>mPointCloudRange.x&&cPoint.z<mPointCloudRange.y)
					depthPoints.push_back(cPoint);
			}
		}

		worldPoints.resize(depthPoints.size());
		mCoordinateMapper->ProjectDepthToCamera(depthPoints.size(), &depthPoints[0], &worldPoints[0]);

		for (int i = 0; i < depthPoints.size();++i)
		{
			PXCPoint3DF32 p = worldPoints[i];
			mPointCloud.push_back(ofVec3f(p.x, p.y, p.z));
		}
	}
#pragma endregion

#pragma region Getters
	const ofPixels& RSDevice::getRgbFrame()
	{
		return rgbFrame;
	}

	const ofShortPixels& RSDevice::getDepthFrame()
	{
		return mDepthFrame;
	}

	const ofPixels& RSDevice::getDepth8uFrame()
	{
		return depthFrameColor;
	}

	const ofPixels& RSDevice::getColorMappedToDepthFrame()
	{
		return mColorToDepthFrame;
	}

	const ofPixels& RSDevice::getDepthMappedToColorFrame()
	{
		return mDepthToColorFrame;
	}

	vector<ofVec3f> RSDevice::getPointCloud()
	{
		return mPointCloud;
	}

	//Nomenclature Notes:
	//	"Space" denotes a 3d coordinate
	//	"Image" denotes an image space point ((0, width), (0,height), (image depth))
	//	"Coords" denotes texture space (U,V) coordinates
	//  "Frame" denotes a full Surface

	//get a camera space point from a depth image point
	const ofPoint RSDevice::getDepthSpacePoint(float pImageX, float pImageY, float pImageZ)
	{
		if (mCoordinateMapper)
		{
			PXCPoint3DF32 cPoint;
			cPoint.x = pImageX;
			cPoint.y = pImageY;
			cPoint.z = pImageZ;

			mInPoints3D.clear();
			mInPoints3D.push_back(cPoint);
			mOutPoints3D.clear();
			mOutPoints3D.resize(2);
			mCoordinateMapper->ProjectDepthToCamera(1, &mInPoints3D[0], &mOutPoints3D[0]);
			return ofPoint(mOutPoints3D[0].x, mOutPoints3D[0].y, mOutPoints3D[0].z);
		}
		return ofPoint(0);
	}

	const ofPoint RSDevice::getDepthSpacePoint(int pImageX, int pImageY, uint16_t pImageZ)
	{
		return getDepthSpacePoint(static_cast<float>(pImageX), static_cast<float>(pImageY), static_cast<float>(pImageZ));
	}

	const ofPoint RSDevice::getDepthSpacePoint(ofPoint pImageCoords)
	{
		return getDepthSpacePoint(pImageCoords.x, pImageCoords.y, pImageCoords.z);
	}

	//get a Color object from a depth image point
	const ofColor RSDevice::getColorFromDepthImage(float pImageX, float pImageY, float pImageZ)
	{
		if (mCoordinateMapper)
		{
			PXCPoint3DF32 cPoint;
			cPoint.x = pImageX;
			cPoint.y = pImageY;
			cPoint.z = pImageZ;
			PXCPoint3DF32 *cInPoint = new PXCPoint3DF32[1];
			cInPoint[0] = cPoint;
			PXCPointF32 *cOutPoints = new PXCPointF32[1];
			mCoordinateMapper->MapDepthToColor(1, cInPoint, cOutPoints);

			float cColorX = cOutPoints[0].x;
			float cColorY = cOutPoints[0].y;

			delete cInPoint;
			delete cOutPoints;
			if (cColorX >= 0 && cColorX < mRgbSize.x&&cColorY >= 0 && cColorY < mRgbSize.y)
			{
				return mRgbFrame.getColor(cColorX, cColorY);
			}
		}
		return ofColor::black;
	}

	const ofColor RSDevice::getColorFromDepthImage(int pImageX, int pImageY, uint16_t pImageZ)
	{
		if (mCoordinateMapper)
			return getColorFromDepthImage(static_cast<float>(pImageX),static_cast<float>(pImageY),static_cast<float>(pImageZ));
		return ofColor::black;
	}

	const ofColor RSDevice::getColorFromDepthImage(ofPoint pImageCoords)
	{
		if (mCoordinateMapper)
			return getColorFromDepthImage(pImageCoords.x, pImageCoords.y, pImageCoords.z);
		return ofColor::black;
	}


		//get a ofColor object from a depth camera space point
	const ofColor RSDevice::getColorFromDepthSpace(float pCameraX, float pCameraY, float pCameraZ)
	{
		if (mCoordinateMapper)
		{
			PXCPoint3DF32 cPoint;
			cPoint.x = pCameraX; cPoint.y = pCameraY; cPoint.z = pCameraZ;

			mInPoints3D.clear();
			mInPoints3D.push_back(cPoint);
			mOutPoints2D.clear();
			mOutPoints2D.resize(2);
			mCoordinateMapper->ProjectCameraToColor(1, &mInPoints3D[0], &mOutPoints2D[0]);

			int imageX = static_cast<int>(mOutPoints2D[0].x);
			int imageY = static_cast<int>(mOutPoints2D[0].y);
			if( (imageX>=0&&imageX<mRgbSize.x)  &&(imageY>=0&&imageY<mRgbSize.y))
				return mRgbFrame.getColor(imageX, imageY);
			return ofColor::black;
		}
		return ofColor::black;
	}

	const ofColor RSDevice::getColorFromDepthSpace(ofPoint pCameraPoint)
	{
		if (mCoordinateMapper)
			return getColorFromDepthSpace(pCameraPoint.x, pCameraPoint.y, pCameraPoint.z);
		return ofColor::black;
	}

		//get ofColor space UVs from a depth image point
	const ofVec2f RSDevice::getColorCoordsFromDepthImage(float pImageX, float pImageY, float pImageZ)
	{
		if (mCoordinateMapper)
		{
			PXCPoint3DF32 cPoint;
			cPoint.x = pImageX;
			cPoint.y = pImageY;
			cPoint.z = pImageZ;

			PXCPoint3DF32 *cInPoint = new PXCPoint3DF32[1];
			cInPoint[0] = cPoint;
			PXCPointF32 *cOutPoints = new PXCPointF32[1];
			mCoordinateMapper->MapDepthToColor(1, cInPoint, cOutPoints);

			float cColorX = cOutPoints[0].x;
			float cColorY = cOutPoints[0].y;

			delete cInPoint;
			delete cOutPoints;
			return ofVec2f(cColorX / (float)mRgbSize.x, cColorY / (float)mRgbSize.y);
		}
		return ofVec2f(0);
	}

	const ofVec2f RSDevice::getColorCoordsFromDepthImage(int pImageX, int pImageY, uint16_t pImageZ)
	{
		return getColorCoordsFromDepthImage(static_cast<float>(pImageX), static_cast<float>(pImageY), static_cast<float>(pImageZ));
	}

	const ofVec2f RSDevice::getColorCoordsFromDepthImage(ofPoint pImageCoords)
	{
		return getColorCoordsFromDepthImage(pImageCoords.x, pImageCoords.y, pImageCoords.z);
	}

		//get ofColor space UVs from a depth space point
	const ofVec2f RSDevice::getColorCoordsFromDepthSpace(float pCameraX, float pCameraY, float pCameraZ)
	{
		if (mCoordinateMapper)
		{
			PXCPoint3DF32 cPoint;
			cPoint.x = pCameraX; cPoint.y = pCameraY; cPoint.z = pCameraZ;

			PXCPoint3DF32 *cInPoint = new PXCPoint3DF32[1];
			cInPoint[0] = cPoint;
			PXCPointF32 *cOutPoint = new PXCPointF32[1];
			mCoordinateMapper->ProjectCameraToColor(1, cInPoint, cOutPoint);

			ofVec2f cRetPt(cOutPoint[0].x / static_cast<float>(mRgbSize.x), cOutPoint[0].y / static_cast<float>(mRgbSize.y));
			delete cInPoint;
			delete cOutPoint;
			return cRetPt;
		}
		return ofVec2f(0);
	}

	const ofVec2f RSDevice::getColorCoordsFromDepthSpace(ofPoint pCameraPoint)
	{
		return getColorCoordsFromDepthSpace(pCameraPoint.x, pCameraPoint.y, pCameraPoint.z);
	}
}
#pragma endregion