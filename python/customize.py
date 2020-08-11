import FWCore.ParameterSet.Config as cms
#import FWCore.ParameterSet.VarParsing as VarParsing
#options = VarParsing.VarParsing ('standard')
#options.register('hltName', 
#                 'HLT', 
#                 VarParsing.VarParsing.multiplicity.singleton, 
#                 VarParsing.VarParsing.varType.string, 
#                 "HLT menu to use for trigger matching"
#)

def add_fileservice(process):
    process.TFileService = cms.Service("TFileService",
                                       closeFileFast=cms.untracked.bool(True),
                                       fileName=cms.string('analyze.root'))


def add_path(process):
    if not hasattr(process, 'tpCheck'):
        process.tpCheck = cms.Path()
        process.schedule.append(process.tpCheck)


def add_reemul(process):
    process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')
    process.simHcalTriggerPrimitiveDigis.RunZS = cms.bool(True)
    process.simHcalTriggerPrimitiveDigis.ZS_threshold = cms.uint32(0)
    add_path(process)
    try:
        process.tpCheck.index(process.simHcalTriggerPrimitiveDigis)
    except ValueError:
        process.tpCheck *= process.simHcalTriggerPrimitiveDigis


def add_l1t(process):
    process.load("EventFilter.L1TXRawToDigi.caloLayer1Stage2Digis_cfi")
    add_path(process)
    try:
        process.tpCheck.index(process.l1tCaloLayer1Digis)
    except ValueError:
        process.tpCheck *= process.l1tCaloLayer1Digis

def analyze_tp(process, name, tag1):
#def analyze_tp(process, options, name, tag1):
    process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')
    add_fileservice(process)
    add_path(process)

    setattr(process, name, cms.EDAnalyzer("AnalyzeTP",
                                          triggerPrimitives=cms.InputTag(tag1, "", ""),
                                          # vertices
                                          vtxToken=cms.untracked.VInputTag("offlinePrimaryVertices","","RECO"),
                                          doReco = cms.bool(True),
                                          maxVtx = cms.uint32(100),
                                          threshold = cms.untracked.double(0.5))
                                          # apply trigger
#                                          bits = cms.InputTag("TriggerResults","",options.hltName))
    )
    process.tpCheck *= getattr(process, name)
    return process


def analyze_l1t_tp(process):
    add_l1t(process)
    return analyze_tp(process, 'analyzeL1T', 'l1tCaloLayer1Digis')


#def analyze_raw_tp(process, options):
#    return analyze_tp(process, options, 'analyzeRaw', 'hcalDigis')

def analyze_raw_tp(process):
    return analyze_tp(process, 'analyzeRaw', 'hcalDigis')


def analyze_emul_tp(process):
    return analyze_tp(process, 'analyzeEmul', 'simHcalTriggerPrimitiveDigis')


def analyze_reemul_tp(process):
    add_reemul(process)
    return analyze_tp(process, 'analyzeReemul', 'simHcalTriggerPrimitiveDigis')


def compare_tp(process, name, tag1, tag2):
    add_fileservice(process)
    add_path(process)
    setattr(process, name, cms.EDAnalyzer("CompareTP",
                                          swapIphi=cms.bool(False),
                                          triggerPrimitives=cms.InputTag(tag1, "", process.name_()),
                                          emulTriggerPrimitives=cms.InputTag(tag2, "", process.name_())),
)
    process.tpCheck *= getattr(process, name)
    return process


def compare_raw_reemul_tp(process):
    add_reemul(process)
    return compare_tp(process, 'compare', 'hcalDigis', 'simHcalTriggerPrimitiveDigis')


def compare_l1t_reemul_tp(process):
    add_l1t(process)
    add_reemul(process)
    return compare_tp(process, 'compareL1T', 'l1tCaloLayer1Digis', 'simHcalTriggerPrimitiveDigis')


def compare_tp_reco(process, name, tag_tp, tag_df, sev):
    process.load("RecoLocalCalo.HcalRecAlgos.hcalRecAlgoESProd_cfi")
    add_fileservice(process)
    add_path(process)
    setattr(process, name, cms.EDAnalyzer("HcalCompareUpgradeChains",
                                          triggerPrimitives=cms.InputTag(tag_tp),
                                          recHits=cms.VInputTag('hbheprereco', 'hfreco'),
                                          dataFrames=cms.VInputTag(cms.InputTag(tag_df), cms.InputTag(tag_df)),
                                          swapIphi=cms.bool(False),
                                          maxSeverity=cms.int32(sev)
                                          ))
    process.tpCheck *= getattr(process, name)
    return process


def compare_raw_reco_sev9(process):
    return compare_tp_reco(process, 'compareRawRecoSeverity9', 'hcalDigis', 'hcalDigis', 9)


def compare_reemul_reco_sev9(process):
    return compare_tp_reco(process, 'compareReemulRecoSeverity9', 'simHcalTriggerPrimitiveDigis', 'hcalDigis', 9)


def compare_raw_reco_sev9999(process):
    return compare_tp_reco(process, 'compareRawRecoSeverity9999', 'hcalDigis', 'hcalDigis', 9999)


def use_data_reemul_tp(process):
    add_reemul(process)
    process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag('hcalDigis', 'hcalDigis')
    process.simHcalTriggerPrimitiveDigis.inputUpgradeLabel = cms.VInputTag('hcalDigis', 'hcalDigis')
    return process


def use_linear_luts(process):
    process.load('CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi')
    process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')
    process.CaloTPGTranscoder.linearLUTs = cms.bool(True)
    process.HcalTPGCoderULUT.linearLUTs = cms.bool(True)
    return process
