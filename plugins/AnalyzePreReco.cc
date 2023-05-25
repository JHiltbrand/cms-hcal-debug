// -*- C++ -*-
//
// Package:    HcalDebug
// Class:      AnalyzePreReco
// 
/**\class AnalyzePreReco AnalyzePreReco.cc HcalDebug/CompareChans/src/AnalyzePreReco.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Matthias Wolf
//         Created:  Wed Jun 29 10:35:39 CEST 2016
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

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TTree.h"
//
// class declaration
//

class AnalyzePreReco : public edm::one::EDAnalyzer<> {
   public:
      explicit AnalyzePreReco(const edm::ParameterSet&);
      ~AnalyzePreReco();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void analyze(const edm::Event&, const edm::EventSetup&);

      // ----------member data ---------------------------
      edm::InputTag prehits_;
      double threshold_;

      int event_;

      TTree *prh_;
      int ieta_;
      int iphi_;
      int depth_;
      int hfdepth_;
      int adc_;
      float charge_;
      float energy_;
      float time_rising_;
      float time_falling_;
};

AnalyzePreReco::AnalyzePreReco(const edm::ParameterSet& config) :
   prehits_(config.getParameter<edm::InputTag>("preRecHits")),
   threshold_(config.getUntrackedParameter<double>("threshold", 0.))
{
   edm::Service<TFileService> fs;

   consumes<HFPreRecHitCollection>(prehits_);

   prh_ = fs->make<TTree>("prh", "HF PreRecHits");
   prh_->Branch("event", &event_);
   prh_->Branch("ieta", &ieta_);
   prh_->Branch("iphi", &iphi_);
   prh_->Branch("depth", &depth_);
   prh_->Branch("hfdepth", &hfdepth_);
   prh_->Branch("adc", &adc_);
   prh_->Branch("charge", &charge_);
   prh_->Branch("energy", &energy_);
   prh_->Branch("time_rising", &time_rising_);
   prh_->Branch("time_falling", &time_falling_);
}

AnalyzePreReco::~AnalyzePreReco() {}

void
AnalyzePreReco::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
   using namespace edm;

   event_ = event.id().event();

   Handle<HFPreRecHitCollection> prehits;
   if (!event.getByLabel(prehits_, prehits)) {
      LogError("AnalyzePreReco") <<
         "Can't find calo tower collection with tag '" <<
         prehits_ << "'" << std::endl;
      return;
   }

   for (const auto& prehit: *prehits) {
      ieta_ = prehit.id().ieta();
      iphi_ = prehit.id().iphi();
      depth_ = prehit.id().depth();
      hfdepth_ = prehit.id().hfdepth();
      for (const auto i: {0, 1}) {
         auto info = prehit.getHFQIE10Info(i);
         if (!info)
            continue;
         adc_ = info->soi();
         charge_ = info->charge();
         energy_ = info->energy();
         time_rising_ = info->timeRising();
         time_falling_ = info->timeFalling();
         prh_->Fill();
      }
   }
}

void
AnalyzePreReco::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(AnalyzePreReco);
