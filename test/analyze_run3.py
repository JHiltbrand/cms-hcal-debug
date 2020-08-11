# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: analyze --data --conditions 101X_dataRun2_Prompt_v11 -s RAW2DIGI --geometry DB:Extended --era Run2_2018 --customise Debug/HcalDebug/customize.compare_raw_reemul_tp --customise Debug/HcalDebug/customize.compare_reemul_reco_sev9 --customise_commands process.TFileService.fileName=cms.string('analyze_325520.root') --filein=filelist:filelist_325520_ZeroBias.txt --python_filename=analyze_325520.py --no_exec -n -1 --no_output
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('RAW2DIGI',eras.Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load("SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff") 
#process.load("EventFilter.HcalRawToDigi.HcalRawToDigi_cfi")
process.load("RecoLocalCalo.Configuration.hcalLocalReco_cff") 
process.load('CondCore.CondDB.CondDB_cfi')     


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_10_6_0/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-DIGI-RAW/106X_upgrade2021_realistic_v4-v1/10000/EF39EED5-3FED-7B4E-AAD2-0A33A9416A0D.root',
        '/store/relval/CMSSW_10_6_0/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-DIGI-RAW/106X_upgrade2021_realistic_v4-v1/10000/EB5DE19B-827A-C140-B813-1426AD296FCC.root',
        '/store/relval/CMSSW_10_6_0/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-DIGI-RAW/106X_upgrade2021_realistic_v4-v1/10000/E3C5FF84-AD89-F743-BC1E-02E039B91EDF.root',
        '/store/relval/CMSSW_10_6_0/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-DIGI-RAW/106X_upgrade2021_realistic_v4-v1/10000/E0857173-72F5-6347-9B22-F8A42FB4321F.root'
#        '/store/data/Run2018D/ZeroBias/RAW/v1/000/325/170/00000/FF9E45DF-DC15-E749-8E0C-0EE9A37361CD.root',
#        '/store/data/Run2018D/ZeroBias/RAW/v1/000/325/170/00000/FCF7B55F-2E54-7B4C-852C-559C2729B181.root',
#        '/store/data/Run2018D/ZeroBias/RAW/v1/000/325/170/00000/FD51C341-E991-A547-82DD-46B9646B5622.root',
#        '/store/data/Run2018D/ZeroBias/RAW/v1/000/325/170/00000/F83D72CC-CB79-2340-AB87-B1DDC124D6FE.root'
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('analyze nevts:-1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.simHcalTriggerPrimitiveDigis.upgrade = cms.bool(True)

#process.HcalTPGCoderULUT.upgrade = cms.bool(True)
#process.CaloTPGTranscoder.upgrade = cms.bool(True)

#process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag(
#    cms.InputTag("mix", "HBHEUpgradeDigiCollection", "DIGI2RAW"),
#    cms.InputTag("mix", "HFUpgradeDigiCollection", "DIGI2RAW"))
    # process.digitisation_step.remove(process.simHcalTriggerPrimitiveDigis)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2021_realistic_v4','')
#'101X_dataRun2_Prompt_v11', '')

process.dump = cms.EDAnalyzer("EventContentAnalyzer") 

process.startjob = cms.Path( 
    process.hcalDigis*
#    process.simHcalTriggerPrimitiveDigis*
#  process.RawToDigi* 
  process.hfprereco*
  process.hfreco*
  process.hbheprereco
#  process.dump
)                                                                                                                                                                                               
       

# Path and EndPath definitions
#process.raw2digi_step = cms.Path(process.RawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)

# Schedule definition
process.schedule = cms.Schedule(process.startjob,process.endjob_step)      
#process.schedule = cms.Schedule(process.raw2digi_step,process.endjob_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from Debug.HcalDebug.customize
from Debug.HcalDebug.customize import use_data_reemul_tp,compare_reemul_reco_sev9 

process =  use_data_reemul_tp(process)

#call to customisation function compare_raw_reemul_tp imported from Debug.HcalDebug.customize
#process = compare_raw_reemul_tp(process)

#call to customisation function compare_reemul_reco_sev9 imported from Debug.HcalDebug.customize
process = compare_reemul_reco_sev9(process)
#process = analyze_emul_tp(process)

# End of customisation functions

# Customisation from command line

process.TFileService.fileName=cms.string('analyze.root')
# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
