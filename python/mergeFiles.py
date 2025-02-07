import FWCore.ParameterSet.Config as cms

process = cms.Process("MERGE")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "FILE1",
        "FILE2",
    ),
)

# Output module - keep only a specific collection
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("merged.root"),
    outputCommands = cms.untracked.vstring(
        "keep *",
    )
)

# EndPath (tells CMSSW to save the output)
process.e = cms.EndPath(process.output)
