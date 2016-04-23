# Codec Step Response

[![Build Status](https://travis-ci.org/CSIR-RTVC/CodecStepResponse.png)](https://travis-ci.org/CSIR-RTVC/CodecStepResponse)
Description of codec configurations used to characterise codec step response

## x264
```
  x264_param_default_preset(&params, "ultrafast", "zerolatency");

  VLOG(2) << "Default level: " << params.i_level_idc;
  params.i_threads = 1;
  params.i_width = m_in.getWidth();
  params.i_height = m_in.getHeight();
  // FIXME: we use doubles (won't work for 12.5)
  params.i_fps_num = (int)m_in.getFps();
  params.i_fps_den = 1;

  // for debugging
#if 0
  params.i_log_level = X264_LOG_DEBUG;
#endif
  // disables periodic IDR frames
  params.i_keyint_max = X264_KEYINT_MAX_INFINITE;
  params.rc.i_rc_method = X264_RC_ABR ;
  params.rc.i_bitrate = m_uiTargetBitrate;

  switch (m_uiMode)
  {
    // STD
    case 0:
    {
      break;
    }
    // CBR
    case 2:
    {
      VLOG(2) << "CBR mode!";
      params.rc.i_vbv_buffer_size = m_uiTargetBitrate;
      params.rc.i_vbv_max_bitrate = m_uiTargetBitrate*m_dCbrFactor;
      params.rc.f_rate_tolerance = 1.0 ;
      break;
    }
  }
```

## openH264
```
  ISVCEncoder* m_pCodec;
  ...
  SEncParamExt param;
  m_pCodec->GetDefaultParams (&param);
  param.iUsageType = CAMERA_VIDEO_REAL_TIME;
  param.fMaxFrameRate = (uint32_t)m_in.getFps();
  param.iPicWidth = m_in.getWidth();
  param.iPicHeight = m_in.getHeight();
  param.iTargetBitrate = m_uiTargetBitrate * 1000;
  //param.bEnableDenoise = denoise;
  param.bEnableDenoise = false;
  param.iSpatialLayerNum = 1;
  // prevents IDR generation on scene change
  param.bEnableSceneChangeDetect = false;
  param.bEnableFrameSkip = false;
  for (int i = 0; i < param.iSpatialLayerNum; i++) {
      param.sSpatialLayers[i].iVideoWidth = m_in.getWidth() >> (param.iSpatialLayerNum - 1 - i);
      param.sSpatialLayers[i].iVideoHeight = m_in.getHeight() >> (param.iSpatialLayerNum - 1 - i);
      param.sSpatialLayers[i].fFrameRate = (uint32_t)m_in.getFps();
      param.sSpatialLayers[i].iSpatialBitrate = param.iTargetBitrate;
      param.sSpatialLayers[i].sSliceCfg.uiSliceMode = SM_SINGLE_SLICE;
#if 0
      param.sSpatialLayers[i].sSliceCfg.uiSliceMode = sliceMode;
      if (sliceMode == SM_DYN_SLICE) {
          param.sSpatialLayers[i].sSliceCfg.sSliceArgument.uiSliceSizeConstraint = 600;
          param.uiMaxNalSize = 1500;
      }
#endif
  }
  param.iTargetBitrate *= param.iSpatialLayerNum;
  m_pCodec->InitializeExt (&param);
  int videoFormat = videoFormatI420;
  m_pCodec->SetOption (ENCODER_OPTION_DATAFORMAT, &videoFormat);
```

## VPP

```
  ICodecv2* pCodec;
  ...

  //Set default quality to 16
  pCodec->SetParameter("quality", "16");
  pCodec->SetParameter("incolour", D_IN_COLOUR_YUV420P8);
  // This multiplies the bit size of every i-frame by 2
  pCodec->SetParameter("ipicturemultiplier", "1");
  pCodec->SetParameter("switchframeperiod", "12");

  // default rate control values
  pCodec->SetParameter("max bits per frame", "100000000");
  pCodec->SetParameter("num rate control frames", "16");
  pCodec->SetParameter("max distortion", "30000");
  pCodec->SetParameter("ipicture dmax multiper", "2");
  pCodec->SetParameter("ipicture dmax fraction", "0");
  pCodec->SetParameter("minimum intra qp", "16");
  pCodec->SetParameter("minimum inter qp", "4");
  pCodec->SetParameter("rate overshoot percent", "100");

  // always flip the image on windows to get it into the desired YUV format
  pCodec->SetParameter("flip", "1");
  pCodec->SetParameter("unrestrictedmotion", "1");
  pCodec->SetParameter("extendedpicturetype", "1");
  pCodec->SetParameter("unrestrictedmotion", "1");
  pCodec->SetParameter("advancedintracoding", "1");
  pCodec->SetParameter("alternativeintervlc", "1");
  pCodec->SetParameter("modifiedquant", "1");
  pCodec->SetParameter("autoipicture", "1");
  pCodec->SetParameter("ipicturefraction", "0");
```
### VPP-Cbr
```
  pCodec->SetParameter("mode of operation", "1");
```
### VPP-Pow
```
  pCodec->SetParameter("mode of operation", "2");
  pCodec->SetParameter("rate control model type", "1");
```
### VPP-Log
```
  pCodec->SetParameter("mode of operation", "2");
  pCodec->SetParameter("rate control model type", "2");
```
