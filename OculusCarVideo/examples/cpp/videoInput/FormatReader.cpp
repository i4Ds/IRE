#include <Mfapi.h>


#include "FormatReader.h"
#include "Strsafe.h"

LPCWSTR GetGUIDNameConstNew(const GUID& guid);
HRESULT GetGUIDNameNew(const GUID& guid, WCHAR **ppwsz);

HRESULT LogAttributeValueByIndexNew(IMFAttributes *pAttr, DWORD index);
HRESULT SpecialCaseAttributeValueNew(GUID guid, const PROPVARIANT& var, MediaType &out);

unsigned int *GetParametr(GUID guid, MediaType &out)
{
	if(guid == MF_MT_YUV_MATRIX) 
		return &(out.MF_MT_YUV_MATRIX);

	if(guid == MF_MT_VIDEO_LIGHTING) 
		return &(out.MF_MT_VIDEO_LIGHTING);
		
	if(guid == MF_MT_DEFAULT_STRIDE) 
		return &(out.MF_MT_DEFAULT_STRIDE);
		
	if(guid == MF_MT_VIDEO_CHROMA_SITING) 
		return &(out.MF_MT_VIDEO_CHROMA_SITING);
	
	if(guid == MF_MT_VIDEO_NOMINAL_RANGE) 
		return &(out.MF_MT_VIDEO_NOMINAL_RANGE);

	if(guid == MF_MT_ALL_SAMPLES_INDEPENDENT) 
		return &(out.MF_MT_ALL_SAMPLES_INDEPENDENT);
	
	if(guid == MF_MT_FIXED_SIZE_SAMPLES) 
		return &(out.MF_MT_FIXED_SIZE_SAMPLES);

	if(guid == MF_MT_SAMPLE_SIZE) 
		return &(out.MF_MT_SAMPLE_SIZE);

	if(guid == MF_MT_VIDEO_PRIMARIES) 
		return &(out.MF_MT_VIDEO_PRIMARIES);

	if(guid == MF_MT_INTERLACE_MODE) 
		return &(out.MF_MT_INTERLACE_MODE);

	return NULL;
}

HRESULT LogAttributeValueByIndexNew(IMFAttributes *pAttr, DWORD index, MediaType &out)
{
    WCHAR *pGuidName = NULL;
    WCHAR *pGuidValName = NULL;

    GUID guid = { 0 };

    PROPVARIANT var;
    PropVariantInit(&var);

    HRESULT hr = pAttr->GetItemByIndex(index, &guid, &var);

    if (FAILED(hr))
    {
        goto done;
    }

    hr = GetGUIDNameNew(guid, &pGuidName);

    if (FAILED(hr))
    {
        goto done;
    }

    hr = SpecialCaseAttributeValueNew(guid, var, out);

	unsigned int *p;

    if (FAILED(hr))
    {
        goto done;
    }
    if (hr == S_FALSE)
    {
        switch (var.vt)
        {
        case VT_UI4:

			p = GetParametr(guid, out);

			if(p) 
			{
				*p = var.ulVal;
			}

            break;

        case VT_UI8:
          
            break;

        case VT_R8:
          
            break;

        case VT_CLSID:
			if(guid == MF_MT_AM_FORMAT_TYPE)
			{
				hr = GetGUIDNameNew(*var.puuid, &pGuidValName);

				if (SUCCEEDED(hr))
				{
					out.MF_MT_AM_FORMAT_TYPE = MF_MT_AM_FORMAT_TYPE;

					out.pMF_MT_AM_FORMAT_TYPEName = pGuidValName;

					pGuidValName = NULL;
				}
			}

			if(guid == MF_MT_MAJOR_TYPE)
			{
				hr = GetGUIDNameNew(*var.puuid, &pGuidValName);

				if (SUCCEEDED(hr))
				{
					out.MF_MT_MAJOR_TYPE = MF_MT_MAJOR_TYPE;

					out.pMF_MT_MAJOR_TYPEName = pGuidValName;

					pGuidValName = NULL;
				}
			}

			if(guid == MF_MT_SUBTYPE)
			{
				hr = GetGUIDNameNew(*var.puuid, &pGuidValName);

				if (SUCCEEDED(hr))
				{
					out.MF_MT_SUBTYPE = MF_MT_SUBTYPE;

					out.pMF_MT_SUBTYPEName = pGuidValName;

					pGuidValName = NULL;
				}
			}

            break;

        case VT_LPWSTR:

            break;

        case VT_VECTOR | VT_UI1:
          
            break;

        case VT_UNKNOWN:
           
            break;

        default:
          
            break;
        }
    }

done:
    CoTaskMemFree(pGuidName);
    CoTaskMemFree(pGuidValName);
    PropVariantClear(&var);
    return hr;
}

HRESULT GetGUIDNameNew(const GUID& guid, WCHAR **ppwsz)
{
    HRESULT hr = S_OK;
    WCHAR *pName = NULL;

    LPCWSTR pcwsz = GetGUIDNameConstNew(guid);
    if (pcwsz)
    {
        size_t cchLength = 0;
    
        hr = StringCchLength(pcwsz, STRSAFE_MAX_CCH, &cchLength);
        if (FAILED(hr))
        {
            goto done;
        }
        
        pName = (WCHAR*)CoTaskMemAlloc((cchLength + 1) * sizeof(WCHAR));

        if (pName == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto done;
        }

        hr = StringCchCopy(pName, cchLength + 1, pcwsz);
        if (FAILED(hr))
        {
            goto done;
        }
    }
    else
    {
        hr = StringFromCLSID(guid, &pName);
    }

done:
    if (FAILED(hr))
    {
        *ppwsz = NULL;
        CoTaskMemFree(pName);
    }
    else
    {
        *ppwsz = pName;
    }
    return hr;
}

void LogUINT32AsUINT64New(const PROPVARIANT& var, UINT32 &uHigh, UINT32 &uLow)
{
    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &uHigh, &uLow);

}

float OffsetToFloatNew(const MFOffset& offset)
{
    return offset.value + (static_cast<float>(offset.fract) / 65536.0f);
}

HRESULT LogVideoAreaNew(const PROPVARIANT& var)
{
    if (var.caub.cElems < sizeof(MFVideoArea))
    {
		return S_OK;
    }

    MFVideoArea *pArea = (MFVideoArea*)var.caub.pElems;

    return S_OK;
}

HRESULT SpecialCaseAttributeValueNew(GUID guid, const PROPVARIANT& var, MediaType &out)
{
	if (guid == MF_MT_FRAME_SIZE)
	{
		UINT32 uHigh = 0, uLow = 0;

		LogUINT32AsUINT64New(var, uHigh, uLow);
		
		out.width = uHigh;
		
		out.height = uLow;

		out.MF_MT_FRAME_SIZE = out.width * out.height;
	}
	else
	if (guid == MF_MT_FRAME_RATE)
	{
		UINT32 uHigh = 0, uLow = 0;

		LogUINT32AsUINT64New(var, uHigh, uLow);
		
		out.MF_MT_FRAME_RATE = uHigh;
		
		out.MF_MT_FRAME_RATE_low = uLow;
	}
	else
	if (guid == MF_MT_FRAME_RATE_RANGE_MAX)
	{
		UINT32 uHigh = 0, uLow = 0;

		LogUINT32AsUINT64New(var, uHigh, uLow);
		
		out.MF_MT_FRAME_RATE_RANGE_MAX = uHigh;
		
		out.MF_MT_FRAME_RATE_RANGE_MAX_low = uLow;
	}
	else
	if (guid == MF_MT_FRAME_RATE_RANGE_MIN)
	{
		UINT32 uHigh = 0, uLow = 0;

		LogUINT32AsUINT64New(var, uHigh, uLow);
		
		out.MF_MT_FRAME_RATE_RANGE_MIN = uHigh;
		
		out.MF_MT_FRAME_RATE_RANGE_MIN_low = uLow;
	}
	else
	if (guid == MF_MT_PIXEL_ASPECT_RATIO)
	{
		UINT32 uHigh = 0, uLow = 0;

		LogUINT32AsUINT64New(var, uHigh, uLow);
		
		out.MF_MT_PIXEL_ASPECT_RATIO = uHigh;
		
		out.MF_MT_PIXEL_ASPECT_RATIO_low = uLow;
	}	     
    else
    {
        return S_FALSE;
    }
    return S_OK;
}

#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return L#val
#endif

LPCWSTR GetGUIDNameConstNew(const GUID& guid)
{
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
    IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
    IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
    IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
    IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
    IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
    IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
    IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
    IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
    IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
    IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
    IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
    IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
    IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
    IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT); 
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC); 
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);
    
    // Media types

    IF_EQUAL_RETURN(guid, MFMediaType_Audio);
    IF_EQUAL_RETURN(guid, MFMediaType_Video);
    IF_EQUAL_RETURN(guid, MFMediaType_Protected);
    IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
    IF_EQUAL_RETURN(guid, MFMediaType_Script);
    IF_EQUAL_RETURN(guid, MFMediaType_Image);
    IF_EQUAL_RETURN(guid, MFMediaType_HTML);
    IF_EQUAL_RETURN(guid, MFMediaType_Binary);
    IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

    IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
    IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
    IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
    IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
    IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
    IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
    IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

    IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
    IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
    IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

    return NULL;
}

FormatReader::FormatReader(void)
{
}

MediaType FormatReader::Read(IMFMediaType *pType)
{
	UINT32 count = 0;

	HRESULT hr = S_OK;

	MediaType out;

	hr = pType->LockStore();

	if (FAILED(hr))
    {
        return out;
    }

    hr = pType->GetCount(&count);

    if (FAILED(hr))
    {
        return out;
    }
	
    for (UINT32 i = 0; i < count; i++)
    {
        hr = LogAttributeValueByIndexNew(pType, i, out);

        if (FAILED(hr))
        {
            break;
        }
    }

	hr = pType->UnlockStore();

	if (FAILED(hr))
    {
        return out;
    }

    return out;
}

FormatReader::~FormatReader(void)
{
}
