import FWCore.ParameterSet.Config as cms

from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.StandardSequences.Eras import eras

processName = "ANATPS"
process = cms.Process(processName, eras.__ERA__)

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
                                   fileName=cms.string('analyze_tps.root'))

# LUTGenerationMode = False => use L1TriggerObjects (for data)
# LUTGenerationMode = True => use L1TriggerObjects (for MC; default)
process.HcalTPGCoderULUT.LUTGenerationMode = cms.bool(__GENLUTS__)
process.simHcalTriggerPrimitiveDigis.inputLabel        = cms.VInputTag("__DIGISTAG__", "__DIGISTAG__")
process.simHcalTriggerPrimitiveDigis.inputUpgradeLabel = cms.VInputTag("__UPGRADEDIGISTAG1__", "__UPGRADEDIGISTAG2__")

process.hcalDigis.silent = cms.untracked.bool(False)

process.analyzeTPs = cms.EDAnalyzer("AnalyzeTPs",
                                 packedTriggerPrimitives=cms.InputTag("__PACKEDTPTAG__", "", __PACKEDTPPROCESSNAME__),
                                 reemulTriggerPrimitives=cms.InputTag("simHcalTriggerPrimitiveDigis", "", processName),
                                 )

process.p = cms.Path(
    process.hcalDigis *
    process.simHcalTriggerPrimitiveDigis *
    process.analyzeTPs)
