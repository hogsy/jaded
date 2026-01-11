#if 0
#ifdef ACTIVE_EDITORS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dshow.h>
#include <atlbase.h>
#include <qedit.h>
#include "Mpeglib/mpg_export.h"

class SmartString
{
public:
	SmartString():str(NULL) {}

	SmartString(char* pStr):str(NULL)
	{
		if (pStr)
		{
			int size = strlen(pStr);
			str = new char[size+1];
			strcpy(str, pStr);
		}
	}

	SmartString(SmartString& sStr)
	{
		SetString(sStr.GetBuffer());
	}

	~SmartString()
	{
		if (str)
			delete[] str;
	}

	SmartString& operator =(char* pStr)
	{
		SetString(pStr);
		return *this;
	}

	SmartString& operator =(SmartString& sStr)
	{
		SetString(sStr.GetBuffer());
		return *this;
	}

	char* GetBuffer() {return str;}

protected:
	void SetString(char *pStr)
	{
		if (str)
			delete[] str;

		if (!pStr)
		{
			str = NULL;
		}
		else
		{
			int size = strlen(pStr);
			str = new char[size + 1];
			strcpy(str, pStr);
		}

	}


	char* str;
};

struct DeviceInfo
{
	DeviceInfo():next(NULL), deviceId(-1)
	{
	} 
	~DeviceInfo()
	{
		if (next)
			delete next;
	}


	SmartString friendlyName;
	int			deviceId;	
	DeviceInfo* next;
};


class ARFrameGrabber  
{
public:
	VIDEOINFOHEADER Vih;
	ARFrameGrabber();
	virtual ~ARFrameGrabber();

	void Init(int deviceId);
	void BindFilter(int deviceId, IBaseFilter **pFilter);
	void GrabFrame(long* size, long** pBuffer);
	void GrabFrame();
	void Grab32BitFrame();


	long  GetBufferSize() {return bufferSize;}
	long* GetBuffer() {return pBuffer;}
	
	void SetFlippedImage(bool flag) {flipImage = flag;}

	void DisplayProperties();
	void EnumDevices(DeviceInfo *head);

protected:
	CComPtr<IGraphBuilder> pGraph;
	CComPtr<IBaseFilter> pDeviceFilter;
	CComPtr<IMediaControl> pMediaControl;
	CComPtr<IBaseFilter> pSampleGrabberFilter;
	CComPtr<ISampleGrabber> pSampleGrabber;
	CComPtr<IPin> pGrabberInput;
	CComPtr<IPin> pGrabberOutput;
	CComPtr<IPin> pCameraOutput;
	CComPtr<IMediaEvent> pMediaEvent;
	CComPtr<IBaseFilter> pNullFilter;
	CComPtr<IPin> pNullInputPin;

	void FlipImage(long* pBuf);

private:
	void ReportError(char *msg);


	bool flipImage;
	long bufferSize;
	long *pBuffer;

};




ARFrameGrabber *grabber;

ARFrameGrabber::ARFrameGrabber():pBuffer(NULL), bufferSize(0), flipImage(false)
{

}

ARFrameGrabber::~ARFrameGrabber()
{
	pMediaControl->Stop();
/*	pMediaControl->Release();
	pMediaEvent->Release();
	pSampleGrabber->Release();*/
}

HRESULT hrinit = S_OK;

void ARFrameGrabber::Init(int deviceId)
{
	HRESULT hr = S_OK;
	hrinit = S_OK;

	// Create the Filter Graph Manager.
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **)&pGraph);

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, 
    IID_IBaseFilter, (LPVOID *)&pSampleGrabberFilter);

	hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pMediaControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **) &pMediaEvent);

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*) &pNullFilter);

	hr = pGraph->AddFilter(pNullFilter, L"NullRenderer");

	hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
	
	AM_MEDIA_TYPE   mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB32;
	mt.formattype = FORMAT_VideoInfo; 
	hr = pSampleGrabber->SetMediaType(&mt);

	pGraph->AddFilter(pSampleGrabberFilter, L"Grabber");

	// Bind Device Filter.  We know the device because the id was passed in
	BindFilter(deviceId, &pDeviceFilter);
	pGraph->AddFilter(pDeviceFilter, NULL);

	CComPtr<IEnumPins> pEnum;
	pDeviceFilter->EnumPins(&pEnum);
	
	hr = pEnum->Reset();
	hr = pEnum->Next(1, &pCameraOutput, NULL); 


	pEnum = NULL; 
	pSampleGrabberFilter->EnumPins(&pEnum);
	pEnum->Reset();
	hr = pEnum->Next(1, &pGrabberInput, NULL); 

	pEnum = NULL;
	pSampleGrabberFilter->EnumPins(&pEnum);
	pEnum->Reset();
	pEnum->Skip(1);
	hr = pEnum->Next(1, &pGrabberOutput, NULL); 


	pEnum = NULL;
	pNullFilter->EnumPins(&pEnum);
	pEnum->Reset();
	hr = pEnum->Next(1, &pNullInputPin, NULL);

	
	hr = pGraph->Connect(pCameraOutput, pGrabberInput);

	hr = pGraph->Connect(pGrabberOutput, pNullInputPin);

//	hr = pGraph->Render(pGrabberOutput);


	if (FAILED(hr))
	{
		switch(hr)
		{
		case VFW_S_NOPREVIEWPIN :
			break;
		case E_FAIL :
			break;
		case E_INVALIDARG :
			break;
		case E_POINTER :
			break;
		}
		hrinit = hr ;
		return;
	}

	pSampleGrabber->SetBufferSamples(TRUE);
	pSampleGrabber->SetOneShot(TRUE);

//	AM_MEDIA_TYPE mt;
	hr = pSampleGrabber->GetConnectedMediaType(&mt);
	if (!FAILED(hr))
		memcpy(&Vih,mt.pbFormat,sizeof(VIDEOINFOHEADER));

	

}

void ARFrameGrabber::GrabFrame(long* size, long** pBuffer)
{
	if (!size)
		return;

	// don't want to leak mem, pBuffer must be NULL
	if (!pBuffer || *pBuffer)
		return;

	long evCode;


	pMediaControl->Run();
	pMediaEvent->WaitForCompletion(INFINITE, &evCode);
	pSampleGrabber->GetCurrentBuffer(size, NULL);
	if (*size)
	{
		*pBuffer = new long[*size];
	}

	pSampleGrabber->GetCurrentBuffer(size, *pBuffer);
}

void ARFrameGrabber::GrabFrame()
{
	long evCode;
	long size = 0;

	pMediaControl->Run();
	pMediaEvent->WaitForCompletion(INFINITE, &evCode);
	pSampleGrabber->GetCurrentBuffer(&size, NULL);

	// if buffer is not the same size as before, create a new one
	if (size != bufferSize)
	{
		if (pBuffer)
			delete[] pBuffer;

		bufferSize = size;

		pBuffer = new long[bufferSize];
	}

	pSampleGrabber->GetCurrentBuffer(&size, pBuffer);
	if (flipImage)
		FlipImage(pBuffer);
}

void ARFrameGrabber::FlipImage(long* pBuf)
{
	DWORD *ptr = (DWORD*)pBuf;
	int pixelCount = bufferSize/4;

 
	if (!pBuf)
		return;

	for (int index = 0; index < pixelCount/2; index++)
	{
		ptr[index] = ptr[index] ^ ptr[pixelCount - index - 1];
		ptr[pixelCount - index - 1] = ptr[index] ^ ptr[pixelCount - index - 1];
		ptr[index] = ptr[index] ^ ptr[pixelCount - index - 1];
	}

}


void ARFrameGrabber::Grab32BitFrame()
{
	long evCode;
	long size = 0;
	long* pData;
	unsigned char* pTemp;
	unsigned char* ptr;

	pMediaControl->Run();
	pMediaEvent->WaitForCompletion(INFINITE, &evCode);
	pSampleGrabber->GetCurrentBuffer(&size, NULL);

	if (size != bufferSize)
	{
		if (pBuffer)
			delete[] pBuffer;

		bufferSize = size/3*4;  // add space for padding

		pBuffer = new long[bufferSize];
	}

	pData= (long*) new unsigned char[size];
	pSampleGrabber->GetCurrentBuffer(&size, pData);
	ptr = ((unsigned char*)pBuffer) + bufferSize - 1;
	pTemp = (unsigned char*) pData;


	// do the padding
	for (int index = 0; index < size/3; index++)
	{
		unsigned char r = *(pTemp++);
		unsigned char g = *(pTemp++);
		unsigned char b = *(pTemp++);

		*(ptr--) = 0;
		*(ptr--) = b;
		*(ptr--) = g;
		*(ptr--) = r;
	}
	delete[] pData;
}

void ARFrameGrabber::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return;
	
    // enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
	{
//		ErrMsg("Error Creating Device Enumerator");
		return;
	}

    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
								&pEm, 0);
    if (hr != NOERROR) 
	{
//		ErrMsg("Sorry, you have no video capture hardware");
		return;
    }

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
	int index = 0;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
    {
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
    }
}


void ARFrameGrabber::DisplayProperties()
{
	CComPtr<ISpecifyPropertyPages> pPages;

	HRESULT hr = pCameraOutput->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pPages);
	if (SUCCEEDED(hr))
	{
		PIN_INFO PinInfo;
		pCameraOutput->QueryPinInfo(&PinInfo);

		CAUUID caGUID;
		pPages->GetPages(&caGUID);

		OleCreatePropertyFrame(
			NULL,
			0,
			0,
			L"Property Sheet",
			1,
			(IUnknown **)&(pCameraOutput.p),
			caGUID.cElems,
			caGUID.pElems,
			0,
			0,
			NULL);
		CoTaskMemFree(caGUID.pElems);
		PinInfo.pFilter->Release();
	}
}



void ARFrameGrabber::EnumDevices(DeviceInfo *head)
{
	if (!head)
		return;

	DeviceInfo *ptr = head;
	int id = 0;
	
    // enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
//    ICreateDevEnum *pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	
    if (hr != NOERROR)
	{
		//ErrMsg("Error Creating Device Enumerator");
		return;
	}



    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
								&pEm, 0);

    if (hr != NOERROR) 
	{
		//ErrMsg("Sorry, you have no video capture hardware");
		return;
    }

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				char str[2048];
				
				if (ptr->deviceId != -1)
				{
					ptr->next = new DeviceInfo();
					ptr = ptr->next;
				}


				ptr->deviceId = id++;
				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				
				ptr->friendlyName = str;

				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
    }
}


void ARFrameGrabber::ReportError(char *msg)
{
	MessageBox(NULL, msg, "ARFrameGrabber Error", MB_ICONSTOP);
}

extern"C" {
static MPG_IOStruct *p_IOSave = NULL;
int  WEBCAM_INIT(MPG_IOStruct *p_IO)
{
	CoInitialize(NULL);

	p_IOSave = p_IO;

	grabber = new ARFrameGrabber();

	for (int Counter = 0;Counter < 4; Counter ++)
	{
		grabber->Init(Counter);
		if (hrinit != S_OK)
		{
			delete grabber;
			grabber = new ARFrameGrabber();
		} else
			Counter = 10000; 
	}

	if (hrinit != S_OK)
	{
		MessageBox(NULL, "no video capture found !!", NULL, MB_ICONSTOP) ;
		return 0;
	}
	
	grabber->DisplayProperties();
	grabber->GrabFrame();
	grabber->GetBuffer();
	grabber->GrabFrame();
	grabber->GrabFrame();
	grabber->GrabFrame();

	p_IO->SizeX = grabber->Vih.bmiHeader.biWidth;
	p_IO->SizeY = grabber->Vih.bmiHeader.biHeight;
	p_IO->ulBufferResult = (unsigned long*)grabber->GetBuffer();
	p_IO->fFrameRate = 60;
	p_IO->ulNumberOfFrames = 256;
	p_IO->ulCurrentFrame = 1;


	CoUninitialize();

	return 1;
}
int  WEBCAM_FRAME(void)
{
	grabber->GrabFrame();
	if (p_IOSave) {p_IOSave->ulCurrentFrame ++;p_IOSave->ulCurrentFrame &=255;};
	return 1;
}
void WEBCAM_GOTO(int ulNum)
{
}
int  WEBCAM_END(void)
{
	p_IOSave = NULL;
	delete grabber;
	return 0;
}

}
#endif
#endif