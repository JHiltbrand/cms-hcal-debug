// -*- C++ -*-
//
// Package:    HcalDebug
// Class:      AnalyzeTP
// 
/**\class AnalyzeTP AnalyzeTP.cc HcalDebug/CompareChans/src/AnalyzeTP.cc

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
#include <string>
#include <unordered_set>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
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

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"


#include "TSystem.h"
#include "TROOT.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TTree.h"
//
// class declaration
//



class AnalyzeTP : public edm::one::EDAnalyzer<> {
   public:
      explicit AnalyzeTP(const edm::ParameterSet&);
      ~AnalyzeTP();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void analyze(const edm::Event&, const edm::EventSetup&);

      // ----------member data ---------------------------
      edm::InputTag digis_;
      //double threshold_;

      //
      bool doReco_;
      unsigned int maxVtx_;      
      std::vector<edm::InputTag> vtxToken_;
      
      double threshold_;
      edm::ESGetToken<CaloTPGTranscoder, CaloTPGRecord> tok_hcalCoder_;
      edm::ESGetToken<HcalTrigTowerGeometry, CaloGeometryRecord> tok_hcalTrigTowerGeometry_;
      
      //
  //      edm::EDGetTokenT<edm::TriggerResults> triggerBits_;
  //      bool zerobiasTrigger_;     

      int run_;
      int lumi_;
      int event_;

      TH1D *saturation_;
      TH1D *delta_;
      TH1D *lsb_;
      TH1D *nlsb_;
      TH1D *nnlsb_;
      TH1D *nnnlsb_;
      TH1D *nnnnlsb_;

      TTree *match_;
      int m_ieta_;
      int m_iphi_;
      double old_et_;
      double new_et_;
      int new_count_;
      int old_fg0_;
      int old_fg1_;
      int new_fg0_;
      int new_fg1_;

      TTree *tps_;

      int tp_ieta_;
      int tp_iphi_;
      int tp_depth_;
      int tp_version_;
      int tp_soi_;
      int tp_fg0_;
      int tp_fg1_;
      double tp_et_;

  //int ev_nvtx_;
  
      TTree *ev_;
      double ev_tp_v0_et_;
      double ev_tp_v1_et_;
      int ev_ntp_hb_;
      int ev_ntp_he_;
      int ev_ntp_hf_;
      int ev_ntp_hb_thr1_;
      int ev_ntp_he_thr1_;
      int ev_ntp_hf_thr1_;
      int ev_ntp_hb_thr5_;
      int ev_ntp_he_thr5_;
      int ev_ntp_hf_thr5_;

      int ev_nvtx_;

};

AnalyzeTP::AnalyzeTP(const edm::ParameterSet& config):
  digis_(config.getParameter<edm::InputTag>("triggerPrimitives")),
  doReco_(config.getParameter<bool>("doReco")),
  maxVtx_(config.getParameter<unsigned int>("maxVtx")),
  vtxToken_(config.getUntrackedParameter<std::vector<edm::InputTag>>("vtxToken")),
  threshold_(config.getUntrackedParameter<double>("threshold", 0.0)),
  tok_hcalCoder_(esConsumes<CaloTPGTranscoder, CaloTPGRecord>()),
  tok_hcalTrigTowerGeometry_(esConsumes<HcalTrigTowerGeometry, CaloGeometryRecord>())
  //  triggerBits_( consumes<edm::TriggerResults>(config.getParameter<edm::InputTag>("bits")) )
{
   edm::Service<TFileService> fs;

   consumes<HcalTrigPrimDigiCollection>(digis_);
   
   //
   if (doReco_) consumes<reco::VertexCollection>(vtxToken_[0]);

   saturation_ = fs->make<TH1D>("saturation", "", 42, 0.5, 42.5);
   delta_ = fs->make<TH1D>("delta", "", 42, 0.5, 42.5);
   lsb_ = fs->make<TH1D>("lsb", "", 42, 0.5, 42.5);
   nlsb_ = fs->make<TH1D>("nlsb", "", 42, 0.5, 42.5);
   nnlsb_ = fs->make<TH1D>("nnlsb", "", 42, 0.5, 42.5);
   nnnlsb_ = fs->make<TH1D>("nnnlsb", "", 42, 0.5, 42.5);
   nnnnlsb_ = fs->make<TH1D>("nnnnlsb", "", 42, 0.5, 42.5);

   tps_ = fs->make<TTree>("tps", "Trigger primitives");
   tps_->Branch("run", &run_);
   tps_->Branch("lumi", &lumi_);
   tps_->Branch("event", &event_);
   tps_->Branch("ieta", &tp_ieta_);
   tps_->Branch("iphi", &tp_iphi_);
   tps_->Branch("depth", &tp_depth_);
   tps_->Branch("version", &tp_version_);
   tps_->Branch("soi", &tp_soi_);
   tps_->Branch("et", &tp_et_);
   tps_->Branch("fg0", &tp_fg0_);
   tps_->Branch("fg1", &tp_fg1_);
 
   //   tps_->Branch("zerobiasTrigger", &zerobiasTrigger_);  
   // get vertices
   tps_->Branch("nvtx", &ev_nvtx_);


   ev_ = fs->make<TTree>("evs", "Event quantities");
   ev_->Branch("run", &run_);
   ev_->Branch("lumi", &lumi_);
   ev_->Branch("event", &event_);
   ev_->Branch("tp_v0_et", &ev_tp_v0_et_);
   ev_->Branch("tp_v1_et", &ev_tp_v1_et_);
   ev_->Branch("ntp_hb", &ev_ntp_hb_);
   ev_->Branch("ntp_he", &ev_ntp_he_);
   ev_->Branch("ntp_hf", &ev_ntp_hf_);
   ev_->Branch("ntp_hb_thr1", &ev_ntp_hb_thr1_);
   ev_->Branch("ntp_he_thr1", &ev_ntp_he_thr1_);
   ev_->Branch("ntp_hf_thr1", &ev_ntp_hf_thr1_);
   ev_->Branch("ntp_hb_thr5", &ev_ntp_hb_thr5_);
   ev_->Branch("ntp_he_thr5", &ev_ntp_he_thr5_);
   ev_->Branch("ntp_hf_thr5", &ev_ntp_hf_thr5_);


   //   ev_->Branch("zerobiasTrigger", &zerobiasTrigger_);
   ev_->Branch("nvtx", &ev_nvtx_);   

   match_ = fs->make<TTree>("ms", "TP matches");
   match_->Branch("run", &run_);
   match_->Branch("lumi", &lumi_);
   match_->Branch("event", &event_);
   match_->Branch("ieta", &m_ieta_);
   match_->Branch("iphi", &m_iphi_);
   match_->Branch("et1x1", &new_et_);
   match_->Branch("et2x3", &old_et_);
   match_->Branch("n1x1", &new_count_);
   match_->Branch("fg0_1x1", &new_fg0_);
   match_->Branch("fg1_1x1", &new_fg1_);
   match_->Branch("fg0_2x3", &old_fg0_);
   match_->Branch("fg1_2x3", &old_fg1_);
}

AnalyzeTP::~AnalyzeTP() {}

void
AnalyzeTP::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
   using namespace edm;

   run_ = event.id().run();
   lumi_ = event.id().luminosityBlock();
   event_ = event.id().event();
   Handle<HcalTrigPrimDigiCollection> digis;
   if (!event.getByLabel(digis_, digis)) {
      LogError("AnalyzeTP") <<
         "Can't find hcal trigger primitive digi collection with tag '" <<
         digis_ << "'" << std::endl;
      return;
   }

   ESHandle<CaloTPGTranscoder> decoder = setup.getHandle(tok_hcalCoder_);

   std::unordered_map<int, std::unordered_map<int, double>> old_ets;
   std::unordered_map<int, std::unordered_map<int, double>> new_ets;
   std::unordered_map<int, std::unordered_map<int, int>> new_counts;

   ev_tp_v0_et_ = 0.;
   ev_tp_v1_et_ = 0.;

   ev_ntp_hb_ = 0;
   ev_ntp_he_ = 0;
   ev_ntp_hf_ = 0;

   ev_ntp_hb_thr1_ = 0;
   ev_ntp_he_thr1_ = 0;
   ev_ntp_hf_thr1_ = 0;
   
   ev_ntp_hb_thr5_ = 0;
   ev_ntp_he_thr5_ = 0;
   ev_ntp_hf_thr5_ = 0;

   
   ESHandle<HcalTrigTowerGeometry> tpd_geo = setup.getHandle(tok_hcalTrigTowerGeometry_);
//   ESHandle<HcalTrigTowerGeometry> tpd_geo;
//   setup.get<CaloGeometryRecord>().get(tpd_geo);

   if (saturation_->Integral() == 0) {
      for (int i = 1; i <= 42; ++i) {
         HcalTrigTowerDetId id(i, 3, 1, i > 29 ? 1 : 0);
         saturation_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(255, 0)));
         delta_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(255, 0)) - decoder->hcaletValue(id, HcalTriggerPrimitiveSample(254, 0)));
         lsb_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(1, 0)));
         nlsb_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(2, 0)));
         nnlsb_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(3, 0)));
         nnnlsb_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(4, 0)));
         nnnnlsb_->SetBinContent(i, decoder->hcaletValue(id, HcalTriggerPrimitiveSample(5, 0)));
      }
   }

   std::map<HcalTrigTowerDetId, HcalTriggerPrimitiveDigi> ttids;
   for (const auto& digi: *digis) {
      if (digi.id().version() == 1)
         ttids[digi.id()] = digi;
   }
   

   // apply trigger -> Uncomment to apply
   /*
   edm::Handle<edm::TriggerResults> triggerBits;
   event.getByToken(triggerBits_,triggerBits);

   const edm::TriggerNames& trigNames = event.triggerNames(*triggerBits);

   size_t pathIndex = trigNames.triggerIndex("HLT_ZeroBias_v6");
   bool zerobiasTrigger = false;
   if ( pathIndex < triggerBits->size() && triggerBits->accept(pathIndex))
      zerobiasTrigger = true;
   zerobiasTrigger_ = zerobiasTrigger;
   */

   // get vertices
   if (doReco_) {
     ev_nvtx_ = 0;
     edm::Handle<reco::VertexCollection> vertices; 
     event.getByLabel(vtxToken_[0], vertices); 
     if (vertices.isValid()) {

       unsigned int nVtx_ = 0;
       for(reco::VertexCollection::const_iterator it=vertices->begin();
           it!=vertices->end() && nVtx_ < maxVtx_; 
           ++it) {
         if (!it->isFake()) { nVtx_++; }
       }
       ev_nvtx_ = (int)nVtx_;   
     }
   }

   for (const auto& digi: *digis) {
      HcalTrigTowerDetId id = digi.id();

      if (id.version() == 1 and abs(id.ieta()) >= 40 and id.iphi() % 4 == 1)
         continue;

      tp_ieta_ = id.ieta();
      tp_iphi_ = id.iphi();
      tp_depth_ = id.depth();
      tp_version_ = id.version();
      tp_soi_ = digi.SOI_compressedEt();
      tp_et_ = decoder->hcaletValue(id, digi.t0());
      tp_fg0_ = digi.t0().fineGrain(0);
      tp_fg1_ = digi.t0().fineGrain(1);

      if (tp_et_ < threshold_)
         continue;

      tps_->Fill();
      if (abs(tp_ieta_) <= 16) {
         ++ev_ntp_hb_;
         if (tp_et_ > 1.) 
            ++ev_ntp_hb_thr1_;
         if (tp_et_ > 5.) 
            ++ev_ntp_hb_thr5_;
      }
      else if (abs(tp_ieta_) <= 29) {
         ++ev_ntp_he_;
         if (tp_et_ > 1.) 
            ++ev_ntp_he_thr1_;
         if (tp_et_ > 5.) 
            ++ev_ntp_he_thr5_;
      }
      else {
         ++ev_ntp_hf_;
         if (tp_et_ > 1.)
            ++ev_ntp_hf_thr1_;
         if (tp_et_ > 5.)
            ++ev_ntp_hf_thr5_;
      }

	if (tp_version_ == 0 and abs(tp_ieta_) >= 29) {
	  ev_tp_v0_et_ += tp_et_;
	} else if (tp_version_ == 1) {
	  ev_tp_v1_et_ += tp_et_;
	}

	if (abs(tp_ieta_) >= 29 and tp_version_ == 0) {
	   std::set<HcalTrigTowerDetId> matches;
	   for (const auto& detid: tpd_geo->detIds(id)) {
               for (const auto& ttid: tpd_geo->towerIds(detid)) {
	       if (ttid.version() == 1)
		  matches.insert(ttid);
               }
	   }         
	

         m_ieta_ = tp_ieta_;
         m_iphi_ = tp_iphi_;
         new_et_ = 0;
         new_count_ = 0;
         old_et_ = tp_et_;
         old_fg0_ = tp_fg0_;
         old_fg1_ = tp_fg1_;
         new_fg0_ = 0;
         new_fg1_ = 0;
         for (const auto& m: matches) {
            if (m.version() == 1 and abs(m.ieta()) >= 40 and m.iphi() % 4 == 1)
               continue;

            new_et_ += decoder->hcaletValue(m, ttids[m].t0());
            ++new_count_;
            new_fg0_ = new_fg0_ || ttids[m].t0().fineGrain(0);
            new_fg1_ = new_fg1_ || ttids[m].t0().fineGrain(1);
         }
         match_->Fill();
      }
   }

   for (int i = -4; i <= 4; ++i) {
      if (i == 0)
         continue;
      for (int j = 0; j < 18; ++j) {
      }
   }

   ev_->Fill();
}

void
AnalyzeTP::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(AnalyzeTP);
