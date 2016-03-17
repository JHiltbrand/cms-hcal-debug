// -*- C++ -*-
//
// Package:    HcalDebug
// Class:      CompareTP
// 
/**\class CompareTP CompareTP.cc HcalDebug/CompareChans/src/CompareTP.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  matthias wolf
//         Created:  Mon Feb 29 13:39:57 CET 2016
// $Id$
//
//


// system include files
#include <memory>
#include <unordered_map>
#include <unordered_set>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"

#include "CondFormats/DataRecord/interface/HcalChannelQualityRcd.h"
#include "CondFormats/DataRecord/interface/L1CaloGeometryRecord.h"
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/L1TObjects/interface/L1CaloGeometry.h"

#include "CondFormats/L1TObjects/interface/L1RCTParameters.h"
#include "CondFormats/DataRecord/interface/L1RCTParametersRcd.h"
#include "CondFormats/L1TObjects/interface/L1CaloHcalScale.h"
#include "CondFormats/DataRecord/interface/L1CaloHcalScaleRcd.h"

#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloCollections.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalTrigTowerGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TTree.h"
//
// class declaration
//

class CompareTP : public edm::EDAnalyzer {
   public:
      explicit CompareTP(const edm::ParameterSet&);
      ~CompareTP();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void analyze(const edm::Event&, const edm::EventSetup&);

      // ----------member data ---------------------------
      edm::InputTag digis_;
      edm::InputTag edigis_;

      bool swap_iphi_;

      int event_;

      TTree *tps_;

      int tp_ieta_;
      int tp_iphi_;
      int tp_depth_;
      int tp_version_;
      int tp_soi_;
      int tp_soi_emul_;
      double tp_et_;
      double tp_et_emul_;
};

CompareTP::CompareTP(const edm::ParameterSet& config) :
   edm::EDAnalyzer(),
   digis_(config.getParameter<edm::InputTag>("triggerPrimitives")),
   edigis_(config.getParameter<edm::InputTag>("emulTriggerPrimitives")),
   swap_iphi_(config.getParameter<bool>("swapIphi"))
{
   edm::Service<TFileService> fs;

   consumes<HcalTrigPrimDigiCollection>(digis_);
   consumes<HcalTrigPrimDigiCollection>(edigis_);

   tps_ = fs->make<TTree>("tps", "Trigger primitives");
   tps_->Branch("event", &event_);
   tps_->Branch("ieta", &tp_ieta_);
   tps_->Branch("iphi", &tp_iphi_);
   tps_->Branch("depth", &tp_depth_);
   tps_->Branch("version", &tp_version_);
   tps_->Branch("soi", &tp_soi_);
   tps_->Branch("soi_emul", &tp_soi_emul_);
   tps_->Branch("et", &tp_et_);
   tps_->Branch("et_emul", &tp_et_emul_);
}

CompareTP::~CompareTP() {}

namespace std {
   template<> struct hash<HcalTrigTowerDetId> {
      size_t operator()(const HcalTrigTowerDetId& id) const {
         return hash<int>()(id);
      }
   };
}

void
CompareTP::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
   using namespace edm;

   event_ = event.id().event();

   Handle<HcalTrigPrimDigiCollection> digis;
   if (!event.getByLabel(digis_, digis)) {
      LogError("CompareTP") <<
         "Can't find hcal trigger primitive digi collection with tag '" <<
         digis_ << "'" << std::endl;
      return;
   }

   Handle<HcalTrigPrimDigiCollection> edigis;
   if (!event.getByLabel(edigis_, edigis)) {
      LogError("CompareTP") <<
         "Can't find emulated hcal trigger primitive digi collection with tag '" <<
         digis_ << "'" << std::endl;
      return;
   }

   ESHandle<CaloTPGTranscoder> decoder;
   setup.get<CaloTPGRecord>().get(decoder);

   std::unordered_set<HcalTrigTowerDetId> ids;
   typedef std::unordered_map<HcalTrigTowerDetId, HcalTriggerPrimitiveDigi> digi_map;
   digi_map ds;
   digi_map eds;

   for (const auto& digi: *digis) {
      ids.insert(digi.id());
      ds[digi.id()] = digi;
   }

   for (const auto& digi: *edigis) {
      ids.insert(digi.id());
      eds[digi.id()] = digi;
   }

   for (const auto& id: ids) {
      tp_ieta_ = id.ieta();
      tp_iphi_ = id.iphi();
      tp_depth_ = id.depth();
      tp_version_ = id.version();
      digi_map::const_iterator digi;
      if ((digi = ds.find(id)) != ds.end()) {
         tp_soi_ = digi->second.SOI_compressedEt();
         tp_et_ = decoder->hcaletValue(id, digi->second.t0());
      } else {
         tp_soi_ = 0;
         tp_et_ = 0;
      }
      auto new_id(id);
      if (swap_iphi_ and id.version() == 1 and id.ieta() > 28 and id.ieta() < 40) {
         if (id.iphi() % 4 == 1)
            new_id = HcalTrigTowerDetId(id.ieta(), (id.iphi() + 70) % 72, id.depth(), id.version());
         else
            new_id = HcalTrigTowerDetId(id.ieta(), (id.iphi() + 2) % 72 , id.depth(), id.version());
      }
      if ((digi = eds.find(new_id)) != eds.end()) {
         tp_soi_emul_ = digi->second.SOI_compressedEt();
         tp_et_emul_ = decoder->hcaletValue(id, digi->second.t0());
      } else {
         tp_soi_emul_ = 0;
         tp_et_emul_ = 0;
      }
      tps_->Fill();
   }
}

void
CompareTP::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(CompareTP);
