import FWCore.ParameterSet.Config as cms

process = cms.Process("FILTER")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "__FILE__",
    ),
)

# Output module - keep only a specific collection
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("filtered.root"),
    outputCommands = cms.untracked.vstring(
        "drop *",                   # Drop everything by default
        "keep *_TriggerResults_*_HLT",
        "keep *_simHcalUnsuppressedDigis_*_HLT"   # Keep only this collection (change name accordingly)
    )
)

# EndPath (tells CMSSW to save the output)
process.e = cms.EndPath(process.output)
