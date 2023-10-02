"""
Template for analyzing local runs. Can be configured
with script one_run.py
"""
import FWCore.ParameterSet.Config as cms

from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.StandardSequences.Eras import eras

RUN = '373559'
GT = '132X_dataRun3_HLT_v2'

process = cms.Process('PLOT', eras.Run3)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
# syntax for override of a single condition
#override = 'HcalElectronicsMap_2018_v3.0_data,HcalElectronicsMapRcd,frontier://FrontierProd/CMS_CONDITIONS'
override = ''
process.GlobalTag = GlobalTag(process.GlobalTag, GT, override)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1000))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/eos/cms/tier0/store/data/Run2023F/ZeroBias/RAW/v1/000/373/784/00000/7d118679-a8e4-46e6-a9f1-8ac69bead65c.root',
    ),
)

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load("SimCalorimetry.Configuration.hcalDigiSequence_cff")
process.load('CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi')
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')


CONDDIR="Debug/HcalDebug/test/conditions"

process.es_prefer = cms.ESPrefer('HcalTextCalibrations','es_ascii')
process.es_ascii = cms.ESSource('HcalTextCalibrations',
   input = cms.VPSet(
      cms.PSet(
         object = cms.string('ChannelQuality'),
         file   = cms.FileInPath(CONDDIR+'/ChannelQuality/HcalChannelQuality_2023_v1.2_data.txt')
      ),
      cms.PSet(
         object = cms.string('Pedestals'),
	 file   = cms.FileInPath(CONDDIR+'/Pedestals/Pedestals_Run374000_50ZDCchannels.txt')
      ),	
      cms.PSet(
         object = cms.string('EffectivePedestals'),
	 file   = cms.FileInPath(CONDDIR+'/EffectivePedestals/HcalPedestals_2023_v1.4_data_effective_ZDCpedestals.txt')
      ),	
      cms.PSet(
         object = cms.string('Gains'),
	 file   = cms.FileInPath(CONDDIR+'/Gains/Gains_Run374000_50ZDCchannels_ZDCgains_v2.txt')
      ),
      cms.PSet(
         object = cms.string('RespCorrs'),
	 file   = cms.FileInPath(CONDDIR+'/RespCorrs/HcalRespCorrs_2023_v3.0_data.txt')
      ),
      cms.PSet(
        object = cms.string('ElectronicsMap'),
	file   = cms.FileInPath(CONDDIR+'/ElectronicsMap/HcalElectronicsMap_2023_v1.0_data.txt')
      ),
      cms.PSet(
        object = cms.string('TPParameters'),
	file   = cms.FileInPath(CONDDIR+'/TPParameters/TPParameters_Run374000.txt')
      ),
      cms.PSet(
        object = cms.string('TPChannelParameters'),
	file   = cms.FileInPath(CONDDIR+'/TPChannelParameters/TPChannelParameters_Run374000_50ZDCchannels_PFA1p.txt')
      ),
      cms.PSet(
        object = cms.string('LUTCorrs'),
	file   = cms.FileInPath(CONDDIR+'/LUTCorrs/LUTCorrs_Run374000_50ZDCchannels.txt')
      ),
      cms.PSet(
        object = cms.string('QIEData'),
	file   = cms.FileInPath(CONDDIR+'/QIEData/QIEData_Run374000_50ZDCchannels.txt')
      ),
      cms.PSet(
        object = cms.string('QIETypes'),
	file   = cms.FileInPath(CONDDIR+'/QIETypes/QIETypes_Run374000_50ZDCchannels.txt')
      ),
      cms.PSet(
        object = cms.string('LutMetadata'),
	file   = cms.FileInPath(CONDDIR+'/LutMetadata/LutMetadata_Run374000_50ZDCchannels.txt')
      ),
      cms.PSet(
        object = cms.string('TimeCorrs'),
	file   = cms.FileInPath(CONDDIR+'/TimeCorrs/TimeCorrs_Run374000_50ZDCchannels.txt')
      ),
   )
)


process.TFileService = cms.Service("TFileService",
                                   closeFileFast=cms.untracked.bool(True),
                                   fileName=cms.string('analyze_' + RUN + '.root'))

process.hcalDigis.saveQIE10DataNSamples = cms.untracked.vint32( 6)
process.hcalDigis.saveQIE10DataTags = cms.untracked.vstring( "MYDATA" )

process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag("hcalDigis", "hcalDigis:ZDC")
process.simHcalTriggerPrimitiveDigis.inputUpgradeLabel = cms.VInputTag("hcalDigis", "hcalDigis:ZDC")
#process.HcalTPGCoderULUT.read_XML_LUTs = cms.bool(True)
#process.HcalTPGCoderULUT.inputLUTs = cms.FileInPath("/afs/cern.ch/work/m/mkrohn/HCALtrigger/2022_MWGR/TestingPFA1Prime/DebugStatements/CMSSW_12_5_X_2022-08-10-1100/src/Debug/HcalDebug/Run3Aug2022_MaskingIphi51.xml" )

process.hcalDigis.silent = cms.untracked.bool(False)

process.compare = cms.EDAnalyzer("CompareTP",
                                 triggerPrimitives=cms.InputTag("hcalDigis"),
                                 emulTriggerPrimitives=cms.InputTag("simHcalTriggerPrimitiveDigis"),
                                 #swapIphi=cms.bool(True),
                                 swapIphi=cms.bool(False),
				 printSwaps=cms.untracked.bool(False),
                                 swapIeta=cms.bool(False))

process.p = cms.Path(
    process.hcalDigis *
    process.simHcalTriggerPrimitiveDigis *
    process.compare)
