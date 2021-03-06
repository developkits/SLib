/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_APPLE)

#include "slib/media/camera.h"

#include "slib/core/platform_apple.h"
#include "slib/core/endian.h"
#include "slib/core/log.h"
#include "slib/core/safe_static.h"

#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

namespace slib
{
	class _AVFoundation_Camera;
}

@interface _AVFoundation_Camera_Callback : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
{
	@public slib::WeakRef<slib::_AVFoundation_Camera> m_camera;
}
@end

namespace slib
{

	class _AVFoundation_Camera_Static
	{
	public:
		dispatch_queue_t dispatch_queue;
		
	public:
		_AVFoundation_Camera_Static()
		{
			dispatch_queue = dispatch_queue_create("SLIB_CAMERA", DISPATCH_QUEUE_SERIAL);
		}
	};
	SLIB_SAFE_STATIC_GETTER(_AVFoundation_Camera_Static, _AVFoundation_Camera_getStatic)

	class _AVFoundation_Camera : public Camera
	{
	public:
		_AVFoundation_Camera_Callback* m_callback;
		AVCaptureSession *m_session;
		AVCaptureDevice* m_device;
		AVCaptureDeviceInput* m_input;
		AVCaptureVideoDataOutput* m_output;
		
		sl_bool m_flagRunning;
		
	public:
		_AVFoundation_Camera()
		{
			m_callback = nil;
			m_session = nil;
			m_device = nil;
			m_input = nil;
			m_output = nil;
			
			m_flagRunning = sl_false;
		}

		~_AVFoundation_Camera()
		{
			release();
		}
		
	public:
		static void logError(String error) {
			Log("Camera", error);
		}
		
		static void logError(String error, NSError* err) {
			Log("Camera", "%s: [%s]", error, [err localizedDescription]);
		}

		static Ref<_AVFoundation_Camera> _create(const CameraParam& param)
		{
			Ref<_AVFoundation_Camera> ret;
			
			_AVFoundation_Camera_Callback* callback = [[_AVFoundation_Camera_Callback alloc] init];
			AVCaptureSession *session = [[AVCaptureSession alloc] init];
			
			sl_int32 req_width = param.preferedFrameWidth;
			sl_int32 req_height = param.preferedFrameHeight;
			selectPresetForSession(session, req_width, req_height);
			
			AVCaptureDevice* device = _selectDevice(param.deviceId);
			if (device == nil) {
				logError("Camera is not found: " + param.deviceId);
				return ret;
			}
			
			NSError* error;
			AVCaptureDeviceInput* input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
			if (input == nil) {
				logError("Failed to create AVCaptureDeviceInput", error);
				return ret;
			}
			
			if (!([session canAddInput:input])) {
				logError("Can not add input to session");
				return ret;
			}
			
			AVCaptureVideoDataOutput* output = [[AVCaptureVideoDataOutput alloc] init];
			[output setAlwaysDiscardsLateVideoFrames:YES];
			dispatch_queue_t queue = _AVFoundation_Camera_getStatic()->dispatch_queue;
			[output setSampleBufferDelegate:callback queue:queue];
			
			NSMutableDictionary *settings = [[NSMutableDictionary alloc] init];
			if (BitmapFormats::getColorSpace(param.preferedFrameFormat) == ColorSpace::YUV) {
				[settings setObject:@(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey];
			} else {
				[settings setObject:@(kCVPixelFormatType_32BGRA) forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey];
			}
			[output setVideoSettings:settings];
			
			if ([session canSetSessionPreset:AVCaptureSessionPreset640x480]) {
				[session setSessionPreset:AVCaptureSessionPreset640x480];
			}
			
			[session addInput:input];
			[session addOutput:output];
			
			AVCaptureConnection *videoConnection = [output connectionWithMediaType:AVMediaTypeVideo];
			if ([videoConnection isVideoOrientationSupported]) {
				[videoConnection setVideoOrientation:AVCaptureVideoOrientationPortrait];
			}
			
			ret = new _AVFoundation_Camera();
			if (ret.isNotNull()) {
				callback->m_camera = ret;
				ret->m_callback = callback;
				ret->m_session = session;
				ret->m_device = device;
				ret->m_input = input;
				ret->m_output = output;
				ret->_init(param);
				if (param.flagAutoStart) {
					ret->start();
				}
			}
			return ret;
		}
		
		struct _PresetInfo
		{
			NSString* preset;
			sl_int32 width;
			sl_int32 height;
		};
		static void selectPresetForSession(AVCaptureSession* session, sl_int32& req_width, sl_int32& req_height)
		{
			_PresetInfo presets[3] = {
				{AVCaptureSessionPreset352x288, 352, 288},
				{AVCaptureSessionPreset640x480, 640, 480},
				{AVCaptureSessionPreset1280x720, 1280, 720}
			};
			if (req_width > 0 && req_height > 0) {
				sl_int32 min_dist = 0;
				NSString* min_preset = nil;
				sl_int32 min_width = 0;
				sl_int32 min_height = 0;
				for (sl_uint32 i = 0; i < 3; i++) {
					if ([session canSetSessionPreset:(presets[i].preset)]) {
						sl_int32 dist1 = (req_width - presets[i].width)*(req_width - presets[i].width) + (req_height - presets[i].height)*(req_height - presets[i].height);
						sl_int32 dist2 = (req_height - presets[i].width)*(req_height - presets[i].width) + (req_width - presets[i].height)*(req_width - presets[i].height);
						sl_int32 dist = SLIB_MIN(dist1, dist2);
						if (min_preset == nil || dist < min_dist) {
							min_preset = presets[i].preset;
							min_width = presets[i].width;
							min_height = presets[i].height;
							min_dist = dist;
						}
					}
				}
				if (min_preset != nil) {
					[session setSessionPreset:min_preset];
				}
				req_width = min_width;
				req_height = min_height;
			}
		}
		
		static AVCaptureDevice* _selectDevice(String deviceId)
		{
#if !defined(SLIB_PLATFORM_IS_IOS_DEVICE)
			if (deviceId == "FRONT" || deviceId == "BACK") {
				deviceId.setNull();
			}
#endif
			NSArray *devices = [AVCaptureDevice devices];
			for (AVCaptureDevice* device in devices) {
				if ([device hasMediaType:AVMediaTypeVideo]) {
					if (deviceId.isEmpty()) {
						return device;
					}
#if defined(SLIB_PLATFORM_IS_IOS_DEVICE)
					if ([device position] == AVCaptureDevicePositionFront && deviceId == "FRONT") {
						return device;
					}
					if ([device position] == AVCaptureDevicePositionBack && deviceId == "BACK") {
						return device;
					}
#endif
					String uid = Apple::getStringFromNSString([device uniqueID]);
					if (uid == deviceId) {
						return device;
					}
				}
			}
			return NULL;
		}
		
		void release()
		{
			ObjectLocker lock(this);
			stop();
			m_session = nil;
			m_callback = nil;
			m_input = nil;
			m_output = nil;
			m_device = nil;
		}

		sl_bool isOpened()
		{
			return m_session != nil;
		}

		void start()
		{
			ObjectLocker lock(this);
			AVCaptureSession* session = m_session;
			if (session == nil) {
				return;
			}
			if (m_flagRunning) {
				return;
			}
			[session startRunning];
			m_flagRunning = sl_true;
		}

		void stop()
		{
			ObjectLocker lock(this);
			AVCaptureSession* session = m_session;
			if (session == nil) {
				return;
			}
			if (!m_flagRunning) {
				return;
			}
			[session stopRunning];
			m_flagRunning = sl_false;
		}

		sl_bool isRunning()
		{
			return m_flagRunning;
		}

		void _onFrame(CMSampleBufferRef sampleBuffer) {
			
			CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
			
			if (CVPixelBufferLockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly) == kCVReturnSuccess) {
				
				sl_uint8* baseAddress = (sl_uint8*)(CVPixelBufferGetBaseAddress(imageBuffer));
				sl_uint32 width = (sl_uint32)(CVPixelBufferGetWidth(imageBuffer));
				sl_uint32 height = (sl_uint32)(CVPixelBufferGetHeight(imageBuffer));
				
				if (baseAddress) {
					
					VideoCaptureFrame frame;
					frame.image.width = width;
					frame.image.height = height;
					
					do {
						OSType type = CVPixelBufferGetPixelFormatType(imageBuffer);
						if (type == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
							if (width & 1) {
								break;
							}
							if (height & 1) {
								break;
							}
							CVPlanarPixelBufferInfo_YCbCrBiPlanar* p = reinterpret_cast<CVPlanarPixelBufferInfo_YCbCrBiPlanar*>(baseAddress);
							if (p->componentInfoY.offset == 0) {
								frame.image.format = BitmapFormat::YUV_NV12;
								frame.image.data = baseAddress + sizeof(CVPlanarPixelBufferInfo_YCbCrBiPlanar);
							} else {
								frame.image.format = BitmapFormat::YUV_NV12;
								frame.image.data = baseAddress + (sl_int32)(Endian::swap32LE(p->componentInfoY.offset));
								frame.image.pitch = Endian::swap32LE(p->componentInfoY.rowBytes);
								frame.image.data1 = baseAddress + (sl_int32)(Endian::swap32LE(p->componentInfoCbCr.offset));
								frame.image.pitch1 = Endian::swap32LE(p->componentInfoCbCr.rowBytes);
							}
						} else if (type == kCVPixelFormatType_32BGRA) {
							frame.image.format = BitmapFormat::BGRA;
							frame.image.data = baseAddress;
							frame.image.pitch = (sl_uint32)(CVPixelBufferGetBytesPerRow(imageBuffer));
						}
					} while (0);
					
					if (frame.image.format != BitmapFormat::None) {
						_onCaptureVideoFrame(&frame);
					}
				}
				
				CVPixelBufferUnlockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
				
			}
			
		}
	};


	Ref<Camera> Camera::create(const CameraParam& param)
	{
		return _AVFoundation_Camera::_create(param);
	}

	List<CameraInfo> Camera::getCamerasList()
	{
		List<CameraInfo> ret;
		NSArray *devices = [AVCaptureDevice devices];
		for (AVCaptureDevice* device in devices) {
			if ([device hasMediaType:AVMediaTypeVideo]) {
				CameraInfo info;
				info.id = Apple::getStringFromNSString([device uniqueID]);
				info.name = Apple::getStringFromNSString([device localizedName]);
				ret.add_NoLock(info);
			}
		}
		return ret;
	}

}

@implementation _AVFoundation_Camera_Callback

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
	slib::Ref<slib::_AVFoundation_Camera> camera(m_camera);
	if (camera.isNotNull()) {
		camera->_onFrame(sampleBuffer);
	}
}

@end

#endif
