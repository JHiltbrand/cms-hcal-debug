# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: analyze --data --conditions auto:run2_data -s RAW2DIGI --geometry DB:Extended --era Run2_2017 --customise Debug/HcalDebug/customize.analyze_l1t_tp --customise Debug/HcalDebug/customize.analyze_raw_tp --customise Debug/HcalDebug/customize.analyze_reemul_tp --customise Debug/HcalDebug/customize.compare_l1t_reemul_tp --customise Debug/HcalDebug/customize.use_data_reemul_tp --filein /store/data/Run2017C/HcalNZS/RAW/v1/000/299/844/00000/AE36B18A-5271-E711-A223-02163E013895.root,/store/data/Run2017C/HcalNZS/RAW/v1/000/299/844/00000/46B78BA1-5271-E711-8820-02163E01A60E.root -n -1 --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

GT ='124X_dataRun3_HLT_v4'

process = cms.Process('PLOT', eras.Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/work/m/mkrohn/HCALtrigger/2022_MWGR/TestingPFA1Prime/DebugStatements/CMSSW_12_5_0_pre4/src/Debug/HcalDebug/DataIsolatedBunches/Run357611/10e44e0e-7a03-4872-a787-493ae6b70bb4.root',
        'file:/afs/cern.ch/work/m/mkrohn/HCALtrigger/2022_MWGR/TestingPFA1Prime/DebugStatements/CMSSW_12_5_0_pre4/src/Debug/HcalDebug/DataIsolatedBunches/Run357611/fc79ec7f-ca39-426e-ab64-458f0972b55b.root',
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')
#process.HcalTPGCoderULUT.applyFixPCC = cms.bool(False)

process.options = cms.untracked.PSet(
    FailPath = cms.untracked.vstring(),
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    SkipEvent = cms.untracked.vstring(),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(1)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    makeTriggerResults = cms.obsolete.untracked.bool,
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(1),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('analyze nevts:-1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('analyze_RAW2DIGI.root'),
    outputCommands = process.RECOSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
override = ''
process.GlobalTag = GlobalTag(process.GlobalTag, GT, override)
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.endjob_step,process.RECOSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from Debug.HcalDebug.customize
from Debug.HcalDebug.customize import analyze_l1t_tp,analyze_raw_tp,analyze_reemul_tp,compare_l1t_reemul_tp,use_data_reemul_tp 

#call to customisation function analyze_l1t_tp imported from Debug.HcalDebug.customize
process = analyze_l1t_tp(process)

#call to customisation function analyze_raw_tp imported from Debug.HcalDebug.customize
process = analyze_raw_tp(process)

#call to customisation function analyze_reemul_tp imported from Debug.HcalDebug.customize
process = analyze_reemul_tp(process)

#call to customisation function compare_l1t_reemul_tp imported from Debug.HcalDebug.customize
process = compare_l1t_reemul_tp(process)

#call to customisation function use_data_reemul_tp imported from Debug.HcalDebug.customize
process = use_data_reemul_tp(process)

# End of customisation functions


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
