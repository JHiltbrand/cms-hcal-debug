import FWCore.ParameterSet.Config as cms

from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.StandardSequences.Eras import eras

process = cms.Process('ANATPS', eras.__ERA__)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
# syntax for override of a single condition
#override = 'HcalElectronicsMap_2018_v3.0_data,HcalElectronicsMapRcd,frontier://FrontierProd/CMS_CONDITIONS'
process.GlobalTag = GlobalTag(process.GlobalTag, "__GLOBALTAG__", '__OVERRIDE__')

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("__FILE__"),
    secondaryFileNames = cms.untracked.vstring()
)

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load("SimCalorimetry.Configuration.hcalDigiSequence_cff")
process.load('CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi')
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')

process.TFileService = cms.Service("TFileService",
                                   closeFileFast=cms.untracked.bool(True),
                                   fileName=cms.string('compare_tp.root'))

# LUTGenerationMode = False => use L1TriggerObjects (for data)
# LUTGenerationMode = True => use L1TriggerObjects (for MC; default)
process.HcalTPGCoderULUT.LUTGenerationMode = cms.bool(False)
process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag("hcalDigis", "hcalDigis")
process.simHcalTriggerPrimitiveDigis.inputUpgradeLabel = cms.VInputTag("hcalDigis", "hcalDigis")

process.hcalDigis.silent = cms.untracked.bool(False)

process.analyzeTPs = cms.EDAnalyzer("analyzeTPs",
                                 triggerPrimitives=cms.InputTag("hcalDigis"),
                                 emulTriggerPrimitives=cms.InputTag("simHcalTriggerPrimitiveDigis"),
                                 swapIphi=cms.bool(False),
				                 printSwaps=cms.untracked.bool(False),
                                 swapIeta=cms.bool(False))

process.p = cms.Path(
    process.hcalDigis *
    process.simHcalTriggerPrimitiveDigis *
    process.compare)
