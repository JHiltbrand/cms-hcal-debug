"""
Template for analyzing local runs. Can be configured
with script one_run.py
"""
import FWCore.ParameterSet.Config as cms

from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.StandardSequences.Eras import eras

RUN = '357885'
GT = '124X_dataRun3_HLT_v4'

process = cms.Process('PLOT', eras.Run3)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
# syntax for override of a single condition
#override = 'HcalElectronicsMap_2018_v3.0_data,HcalElectronicsMapRcd,frontier://FrontierProd/CMS_CONDITIONS'
override = ''
process.GlobalTag = GlobalTag(process.GlobalTag, GT, override)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:root://cms-xrd-global.cern.ch//store/data/Run2022D/HcalNZS/RAW/v1/000/357/885/00000/248fbab6-964d-4c09-925a-f6ea42b6da8f.root',
#       'file:/eos/cms/tier0/store/data/Commissioning2022/MinimumBias/RAW/v1/000/350/491/00000/2264eece-d860-4048-9dc2-30a5106632b4.root',
    ),
#    skipEvents = cms.untracked.uint32(8500),
    secondaryFileNames = cms.untracked.vstring()
)

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load("SimCalorimetry.Configuration.hcalDigiSequence_cff")
process.load('CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi')
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')

process.TFileService = cms.Service("TFileService",
                                   closeFileFast=cms.untracked.bool(True),
                                   #fileName=cms.string('analyze_' + RUN + '_1event.root'))
                                   fileName=cms.string('analyze_' + RUN + '.root'))

# LUTGenerationMode = False => use L1TriggerObjects (for data)
# LUTGenerationMode = True => use L1TriggerObjects (for MC; default)
#process.HcalTPGCoderULUT.LUTGenerationMode = cms.bool(True)
process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag("hcalDigis", "hcalDigis")
process.simHcalTriggerPrimitiveDigis.inputUpgradeLabel = cms.VInputTag("hcalDigis", "hcalDigis")

process.hcalDigis.silent = cms.untracked.bool(False)



process.analyzeRAW = cms.EDAnalyzer("AnalyzeTP",
                                    triggerPrimitives=cms.InputTag("hcalDigis", "", ""),
                                     # vertices
                                    vtxToken=cms.untracked.VInputTag("offlinePrimaryVertices","","RECO"),
                                    doReco = cms.bool(False),
                                    maxVtx = cms.uint32(100),
                                    threshold = cms.untracked.double(0.5)

)
process.analyzeSIM = cms.EDAnalyzer("AnalyzeTP",
                                    triggerPrimitives=cms.InputTag("simHcalTriggerPrimitiveDigis", "", ""),
                                     # vertices
                                    vtxToken=cms.untracked.VInputTag("offlinePrimaryVertices","","RECO"),
                                    doReco = cms.bool(False),
                                    maxVtx = cms.uint32(100),
                                    threshold = cms.untracked.double(0.5)
)

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
    process.analyzeRAW *
    process.analyzeSIM *
    process.compare)
