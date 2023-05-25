// -*- C++ -*-
//
// Package:    HcalDebug
// Class:      AnalyzeCT
// 
/**\class AnalyzeCT AnalyzeCT.cc HcalDebug/CompareChans/src/AnalyzeCT.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  matthias wolf
//         Created:  Fri Nov 27 11:21:58 CET 2015
// $Id$
//
//


// system include files
#include <memory>
#include <unordered_map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/L1TCalorimeter/interface/CaloTower.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TTree.h"
//
// class declaration
//

class AnalyzeCT : public edm::one::EDAnalyzer<> {
   public:
      explicit AnalyzeCT(const edm::ParameterSet&);
      ~AnalyzeCT();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void analyze(const edm::Event&, const edm::EventSetup&);

      // ----------member data ---------------------------
      edm::InputTag towers_;
      double threshold_;

      int event_;

      TTree *cts_;
      int ieta_;
      int iphi_;
      double et_;
      int fg_;
};

AnalyzeCT::AnalyzeCT(const edm::ParameterSet& config) :
   towers_(config.getParameter<edm::InputTag>("caloTowers")),
   threshold_(config.getUntrackedParameter<double>("threshold", 0.))
{
   edm::Service<TFileService> fs;

   consumes<l1t::CaloTowerBxCollection>(towers_);

   cts_ = fs->make<TTree>("cts", "L1T Calo Towers");
   cts_->Branch("event", &event_);
   cts_->Branch("ieta", &ieta_);
   cts_->Branch("iphi", &iphi_);
   cts_->Branch("et", &et_);
   cts_->Branch("fg", &fg_);
}

AnalyzeCT::~AnalyzeCT() {}

void
AnalyzeCT::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
   using namespace edm;

   event_ = event.id().event();

   Handle<l1t::CaloTowerBxCollection> towers;
   if (!event.getByLabel(towers_, towers)) {
      LogError("AnalyzeCT") <<
         "Can't find calo tower collection with tag '" <<
         towers_ << "'" << std::endl;
      return;
   }

   for (const auto& tower: *towers) {
      ieta_ = tower.hwEta();
      iphi_ = tower.hwPhi();
      et_ = tower.etHad();
      fg_ = tower.hwQual() & 0x4;
      cts_->Fill();
   }
}

void
AnalyzeCT::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(AnalyzeCT);
