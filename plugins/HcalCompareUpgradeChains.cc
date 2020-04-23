// system include files
#include <vector>
#include <map>
#include <algorithm>

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
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"

#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HFRecHit.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalTrigTowerGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputer.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputerRcd.h"

#include "TH2D.h"
#include "TString.h"
#include "TTree.h"

class HcalCompareUpgradeChains : public edm::EDAnalyzer {
   public:
      explicit HcalCompareUpgradeChains(const edm::ParameterSet&);
      ~HcalCompareUpgradeChains();

   private:
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&) override;

      double get_cosh(const HcalDetId&);

      // ----------member data ---------------------------
      std::vector<edm::InputTag> frames_;
      edm::InputTag digis_;
      std::vector<edm::InputTag> rechits_;

      edm::ESHandle<CaloGeometry> gen_geo_; 
      edm::ESHandle<CaloTPGTranscoder> decoder_;
      edm::ESHandle<HcalTrigTowerGeometry> tpd_geo_h_;
      edm::ESHandle<HcalDbService> conditions_;

      TTree *events_;
      TTree *matches_;

      std::map<int, TH2D*> tprh_vs_et_;
      std::map<int, TH2D*> tprh_vs_et_tpgt0p0_;
      std::map<int, TH2D*> tprh_vs_et_tpgt0p5_;

      std::map<int, TH2D*> tprh_vs_et_peak_;
      std::map<int, TH2D*> tprh_vs_et_tpgt0p0_peak_;
      std::map<int, TH2D*> tprh_vs_et_tpgt0p5_peak_;

      std::map<int, TH2D*> tprh_vs_et_noPeak_;
      std::map<int, TH2D*> tprh_vs_et_tpgt0p0_noPeak_;
      std::map<int, TH2D*> tprh_vs_et_tpgt0p5_noPeak_;

      TH2D* tprh_vs_et_all_;
      TH2D* tprh_vs_et_all_tpgt0p0_;
      TH2D* tprh_vs_et_all_tpgt0p5_;

      TH2D* tprh_vs_ieta_low_;
      TH2D* tprh_vs_ieta_low_tpgt0p0_;
      TH2D* tprh_vs_ieta_low_tpgt0p5_;

      TH2D* tprh_vs_ieta_high_;
      TH2D* tprh_vs_ieta_high_tpgt0p0_;
      TH2D* tprh_vs_ieta_high_tpgt0p5_;

      TH2D* tprh_vs_et_all_peak_;
      TH2D* tprh_vs_et_all_tpgt0p0_peak_;
      TH2D* tprh_vs_et_all_tpgt0p5_peak_;

      TH2D* tprh_vs_ieta_low_peak_;
      TH2D* tprh_vs_ieta_low_tpgt0p0_peak_;
      TH2D* tprh_vs_ieta_low_tpgt0p5_peak_;

      TH2D* tprh_vs_ieta_high_peak_;
      TH2D* tprh_vs_ieta_high_tpgt0p0_peak_;
      TH2D* tprh_vs_ieta_high_tpgt0p5_peak_;

      TH2D* tprh_vs_et_all_noPeak_;
      TH2D* tprh_vs_et_all_tpgt0p0_noPeak_;
      TH2D* tprh_vs_et_all_tpgt0p5_noPeak_;

      TH2D* tprh_vs_ieta_low_noPeak_;
      TH2D* tprh_vs_ieta_low_tpgt0p0_noPeak_;
      TH2D* tprh_vs_ieta_low_tpgt0p5_noPeak_;

      TH2D* tprh_vs_ieta_high_noPeak_;
      TH2D* tprh_vs_ieta_high_tpgt0p0_noPeak_;
      TH2D* tprh_vs_ieta_high_tpgt0p5_noPeak_;

      int ev_tp_event_;
      std::vector<int> ev_tp_ieta_;
      std::vector<int> ev_tp_iphi_;
      std::vector<int> ev_tp_depth_;
      std::vector<double> ev_tp_et_;
      std::vector<int> ev_tp_peak_;
      std::vector<int> ev_tp_ts0_;
      std::vector<int> ev_tp_ts1_;
      std::vector<int> ev_tp_ts2_;
      std::vector<int> ev_tp_ts3_;
      std::vector<int> ev_tp_ts4_;
      std::vector<int> ev_tp_ts5_;
      std::vector<int> ev_tp_ts6_;
      std::vector<int> ev_tp_ts7_;

      double mt_rh_energy_lsb_;
      double mt_rh_energy_;
      double mt_tp_energy_;

      int mt_ispeak_;
      int mt_ieta_;
      int mt_iphi_;
      int mt_depth_;
      int mt_tp_soi_;
      int mt_event_;
      int mt_pu_;
      double mt_rTPRH_;
      double mt_r43_;
      double mt_r4Total_;
      int mt_ts0_;
      int mt_ts1_;
      int mt_ts2_;
      int mt_ts3_;
      int mt_ts4_;
      int mt_ts5_;
      int mt_ts6_;
      int mt_ts7_;

      bool swap_iphi_;

      int max_severity_;
      edm::InputTag puInfo_;
      const HcalChannelQuality* status_;
      const HcalSeverityLevelComputer* comp_;
};

HcalCompareUpgradeChains::HcalCompareUpgradeChains(const edm::ParameterSet& config) :
    edm::EDAnalyzer(),
    frames_(config.getParameter<std::vector<edm::InputTag>>("dataFrames")),
    digis_(config.getParameter<edm::InputTag>("triggerPrimitives")),
    rechits_(config.getParameter<std::vector<edm::InputTag>>("recHits")),
    swap_iphi_(config.getParameter<bool>("swapIphi")),
    max_severity_(config.getParameter<int>("maxSeverity")),
    puInfo_(edm::InputTag("addPileupInfo"))

{

    consumes<HcalTrigPrimDigiCollection>(digis_);
    consumes<QIE11DigiCollection>(frames_[0]);
    consumes<edm::SortedCollection<HBHERecHit>>(rechits_[0]);
    consumes<std::vector<PileupSummaryInfo> >(puInfo_);

    edm::Service<TFileService> fs;

    for (unsigned int aieta = 1; aieta < 29; aieta++) {
        tprh_vs_et_[aieta]         = fs->make<TH2D>(TString("TPRH_RHET_ieta"+std::to_string(aieta)), ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
        tprh_vs_et_tpgt0p0_[aieta] = fs->make<TH2D>(TString("TPRH_RHET_TPETgt0.0_ieta"+std::to_string(aieta)), ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
        tprh_vs_et_tpgt0p5_[aieta] = fs->make<TH2D>(TString("TPRH_RHET_TPETgt0.5_ieta"+std::to_string(aieta)), ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);

        tprh_vs_et_peak_[aieta]         = fs->make<TH2D>(TString("TPRH_RHET_ieta"+std::to_string(aieta))+"_peak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
        tprh_vs_et_tpgt0p0_peak_[aieta] = fs->make<TH2D>(TString("TPRH_RHET_TPETgt0.0_ieta"+std::to_string(aieta))+"_peak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
        tprh_vs_et_tpgt0p5_peak_[aieta] = fs->make<TH2D>(TString("TPRH_RHET_TPETgt0.5_ieta"+std::to_string(aieta))+"_peak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);

        tprh_vs_et_noPeak_[aieta]         = fs->make<TH2D>(TString("TPRH_RHET_ieta"+std::to_string(aieta))+"_noPeak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
        tprh_vs_et_tpgt0p0_noPeak_[aieta] = fs->make<TH2D>(TString("TPRH_RHET_TPETgt0.0_ieta"+std::to_string(aieta))+"_noPeak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
        tprh_vs_et_tpgt0p5_noPeak_[aieta] = fs->make<TH2D>(TString("TPRH_RHET_TPETgt0.5_ieta"+std::to_string(aieta))+"_noPeak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    }

    tprh_vs_et_all_         = fs->make<TH2D>("TPRH_RHET_ieta1to28", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    tprh_vs_et_all_tpgt0p0_ = fs->make<TH2D>("TPRH_RHET_TPETgt0.0_ieta1to28", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    tprh_vs_et_all_tpgt0p5_ = fs->make<TH2D>("TPRH_RHET_TPETgt0.5_ieta1to28", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);

    tprh_vs_ieta_low_          = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_low_tpgt0p0_  = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_TPETgt0.0", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_low_tpgt0p5_  = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_TPETgt0.5", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);

    tprh_vs_ieta_high_         = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_high_tpgt0p0_ = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_TPETgt0.0", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_high_tpgt0p5_ = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_TPETgt0.5", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);

    tprh_vs_et_all_peak_         = fs->make<TH2D>("TPRH_RHET_ieta1to28_peak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    tprh_vs_et_all_tpgt0p0_peak_ = fs->make<TH2D>("TPRH_RHET_TPETgt0.0_ieta1to28_peak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    tprh_vs_et_all_tpgt0p5_peak_ = fs->make<TH2D>("TPRH_RHET_TPETgt0.5_ieta1to28_peak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);

    tprh_vs_ieta_low_peak_          = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_peak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_low_tpgt0p0_peak_  = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_TPETgt0.0_peak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_low_tpgt0p5_peak_  = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_TPETgt0.5_peak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);

    tprh_vs_ieta_high_peak_         = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_peak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_high_tpgt0p0_peak_ = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_TPETgt0.0_peak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_high_tpgt0p5_peak_ = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_TPETgt0.5_peak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);

    tprh_vs_et_all_noPeak_         = fs->make<TH2D>("TPRH_RHET_ieta1to28_noPeak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    tprh_vs_et_all_tpgt0p0_noPeak_ = fs->make<TH2D>("TPRH_RHET_TPETgt0.0_ieta1to28_noPeak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);
    tprh_vs_et_all_tpgt0p5_noPeak_ = fs->make<TH2D>("TPRH_RHET_TPETgt0.5_ieta1to28_noPeak", ";E_{T,RH};E_{T,TP} / E_{T,RH}", 257, -0.25, 128.25, 720, -0.014, 19.986);

    tprh_vs_ieta_low_noPeak_          = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_noPeak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_low_tpgt0p0_noPeak_  = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_TPETgt0.0_noPeak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_low_tpgt0p5_noPeak_  = fs->make<TH2D>("TPRH_vs_ieta_RHET0.0to10.0_TPETgt0.5_noPeak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);

    tprh_vs_ieta_high_noPeak_         = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_noPeak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_high_tpgt0p0_noPeak_ = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_TPETgt0.0_noPeak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);
    tprh_vs_ieta_high_tpgt0p5_noPeak_ = fs->make<TH2D>("TPRH_vs_ieta_RHET10.0toInf_TPETgt0.5_noPeak", ";i#eta;E_{T,TP} / E_{T,RH}", 57, -28.5, 28.5, 720, -0.014, 19.986);

    events_ = fs->make<TTree>("events", "Event quantities");
    events_->Branch("ieta", &ev_tp_ieta_);
    events_->Branch("iphi", &ev_tp_iphi_);
    events_->Branch("depth", &ev_tp_depth_);
    events_->Branch("et", &ev_tp_et_);
    events_->Branch("ispeak", &ev_tp_peak_);
    events_->Branch("ts0", &ev_tp_ts0_);
    events_->Branch("ts1", &ev_tp_ts1_);
    events_->Branch("ts2", &ev_tp_ts2_);
    events_->Branch("ts3", &ev_tp_ts3_);
    events_->Branch("ts4", &ev_tp_ts4_);
    events_->Branch("ts5", &ev_tp_ts5_);
    events_->Branch("ts6", &ev_tp_ts6_);
    events_->Branch("ts7", &ev_tp_ts7_);
    events_->Branch("event", &ev_tp_event_);

    matches_ = fs->make<TTree>("matches", "Matched RH and TP");
    matches_->Branch("RH_energy", &mt_rh_energy_);
    matches_->Branch("RH_energy_LSB", &mt_rh_energy_lsb_);
    matches_->Branch("TP_energy", &mt_tp_energy_);
    matches_->Branch("ieta", &mt_ieta_);
    matches_->Branch("soi_peak", &mt_ispeak_);
    matches_->Branch("iphi", &mt_iphi_);
    matches_->Branch("depth", &mt_depth_);
    matches_->Branch("tp_soi", &mt_tp_soi_);
    matches_->Branch("event", &mt_event_);
    matches_->Branch("pu", &mt_pu_);
    matches_->Branch("rTPRH", &mt_rTPRH_);
    matches_->Branch("r43", &mt_r43_);
    matches_->Branch("r4Total", &mt_r4Total_);
    matches_->Branch("ts0", &mt_ts0_);
    matches_->Branch("ts1", &mt_ts1_);
    matches_->Branch("ts2", &mt_ts2_);
    matches_->Branch("ts3", &mt_ts3_);
    matches_->Branch("ts4", &mt_ts4_);
    matches_->Branch("ts5", &mt_ts5_);
    matches_->Branch("ts6", &mt_ts6_);
    matches_->Branch("ts7", &mt_ts7_);
}

HcalCompareUpgradeChains::~HcalCompareUpgradeChains() {}


double
HcalCompareUpgradeChains::get_cosh(const HcalDetId& id)
{

  const auto *sub_geo = dynamic_cast<const HcalGeometry*>(gen_geo_->getSubdetectorGeometry(id));
  auto eta = sub_geo->getPosition(id).eta();
  return cosh(eta);
}

void
HcalCompareUpgradeChains::beginLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup& setup)
{
  edm::ESHandle<HcalChannelQuality> status;
  setup.get<HcalChannelQualityRcd>().get("withTopo", status);
  status_ = status.product();
  edm::ESHandle<HcalSeverityLevelComputer> comp;
  setup.get<HcalSeverityLevelComputerRcd>().get(comp);
  comp_ = comp.product();
}


void
HcalCompareUpgradeChains::analyze(const edm::Event& event, const edm::EventSetup& setup)
{

    using namespace edm;

    setup.get<CaloGeometryRecord>().get(tpd_geo_h_);
    const HcalTrigTowerGeometry& tpd_geo = *tpd_geo_h_;

    mt_event_ = event.id().event(); ev_tp_event_ = mt_event_;
    
    try {
        Handle<std::vector<PileupSummaryInfo> > puInfo;
        event.getByLabel(puInfo_, puInfo);

        std::vector<PileupSummaryInfo>::const_iterator pui;
         
        mt_pu_ = puInfo->begin()->getTrueNumInteractions();

    } catch (...) {
        // If running on data we'll get here
        mt_pu_ = -1;
    }

    Handle<QIE11DigiCollection> frames;
    event.getByLabel(frames_[0], frames); 

    // ==============
    // Matching stuff
    // ==============

    std::map<HcalTrigTowerDetId, std::vector<HBHERecHit>> rhits;

    Handle<HcalTrigPrimDigiCollection> digis;
    if (!event.getByLabel(digis_, digis)) {
       LogError("HcalTrigPrimDigiCleaner") <<
          "Can't find hcal trigger primitive digi collection with tag '" <<
          digis_ << "'" << std::endl;
       return;
    }

    edm::Handle< edm::SortedCollection<HBHERecHit> > hits;
    if (!event.getByLabel(rechits_[0], hits)) {
       edm::LogError("HcalCompareUpgradeChains") <<
          "Can't find rec hit collection with tag '" << rechits_[0] << "'" << std::endl;
       /* return; */
    }

    setup.get<CaloGeometryRecord>().get(gen_geo_);

    auto isValid = [&](const auto& hit) {
        HcalDetId id(hit.id());
        auto s = status_->getValues(id);
        int level = comp_->getSeverityLevel(id, 0, s->getValue());
        return level <= max_severity_;
    };

    if (hits.isValid()) {
        for (auto& hit: *(hits.product())) {
            HcalDetId id(hit.id());
            if (not isValid(hit))  continue;

            auto tower_ids = tpd_geo.towerIds(id, id.depth());

            for (auto& tower_id: tower_ids) {
                // DEPTH: Set zero to tower_id.depth()
                tower_id = HcalTrigTowerDetId(tower_id.ieta(), tower_id.iphi(), 0);
                rhits[tower_id].push_back(hit);
            }
        }
    }

    setup.get<CaloTPGRecord>().get(decoder_);

    ev_tp_ieta_.clear();   ev_tp_ieta_.shrink_to_fit();
    ev_tp_iphi_.clear();   ev_tp_iphi_.shrink_to_fit();
    ev_tp_depth_.clear();  ev_tp_depth_.shrink_to_fit();
    ev_tp_et_.clear();     ev_tp_et_.shrink_to_fit();
    ev_tp_peak_.clear();   ev_tp_peak_.shrink_to_fit();
    ev_tp_ts0_.clear();    ev_tp_ts0_.shrink_to_fit();
    ev_tp_ts1_.clear();    ev_tp_ts1_.shrink_to_fit();
    ev_tp_ts2_.clear();    ev_tp_ts2_.shrink_to_fit();
    ev_tp_ts3_.clear();    ev_tp_ts3_.shrink_to_fit();
    ev_tp_ts4_.clear();    ev_tp_ts4_.shrink_to_fit();
    ev_tp_ts5_.clear();    ev_tp_ts5_.shrink_to_fit();
    ev_tp_ts6_.clear();    ev_tp_ts6_.shrink_to_fit();
    ev_tp_ts7_.clear();    ev_tp_ts7_.shrink_to_fit();

    // Main loop over trigger primitive collection
    // There should be a single TP per det id as we 
    // have already summed over depth upstream
    for (const auto& digi: *digis) {

        HcalTrigTowerDetId tpid = HcalTrigTowerDetId(digi.id().ieta(), digi.id().iphi(), digi.id().depth(), digi.id().version());
        auto aieta = tpid.ietaAbs();
        auto ieta  = tpid.ieta();

        // Due to digi ordering once we hit HF we can break out
        if (aieta >= 30) { break; }

        ev_tp_ieta_.push_back(tpid.ieta());
        ev_tp_iphi_.push_back(tpid.iphi());
        ev_tp_depth_.push_back(tpid.depth());

        std::vector<int> sampleData = digi.getSampleData();

        ev_tp_ts0_.push_back(sampleData[0]);
        ev_tp_ts1_.push_back(sampleData[1]);
        ev_tp_ts2_.push_back(sampleData[2]);
        ev_tp_ts3_.push_back(sampleData[3]);
        ev_tp_ts4_.push_back(sampleData[4]);
        ev_tp_ts5_.push_back(sampleData[5]);
        ev_tp_ts6_.push_back(sampleData[6]);
        ev_tp_ts7_.push_back(sampleData[7]);

        mt_ispeak_ = digi.SOI_isPeak();
        mt_ieta_ = tpid.ieta();

        mt_iphi_ = tpid.iphi();
        mt_depth_ = tpid.depth();
        mt_tp_energy_ = decoder_->hcaletValue(tpid, digi.SOI_compressedEt());
        mt_tp_soi_ = digi.SOI_compressedEt();
        ev_tp_et_.push_back(mt_tp_energy_);
        ev_tp_peak_.push_back(mt_ispeak_);
            
        mt_ts0_ = sampleData[0];
        mt_ts1_ = sampleData[1];
        mt_ts2_ = sampleData[2];
        mt_ts3_ = sampleData[3];
        mt_ts4_ = sampleData[4];
        mt_ts5_ = sampleData[5];
        mt_ts6_ = sampleData[6];
        mt_ts7_ = sampleData[7];
        
        mt_rh_energy_  = 0.;
        mt_rh_energy_lsb_ = 0.;

        // Do a little try catch nonsense to avoid dividing by 0
        if (sampleData[3] > 0) { mt_r43_ = float(sampleData[4]) / float(sampleData[3]); }
        else { mt_r43_ = -1.0; }

        if (sampleData[3] + sampleData[4] + sampleData[5] + sampleData[6] + sampleData[7] > 0) { mt_r4Total_ = float(sampleData[4]) / float(sampleData[3] + sampleData[4] + sampleData[5] + sampleData[6] + sampleData[7]); }
        else { mt_r4Total_ = -1.0; }

        auto rh = rhits.find(tpid);            
        if (rh != rhits.end()) {

            for (const auto& hit: rh->second) {
                HcalDetId hitid(hit.id());

                auto depth = hitid.depth();
                auto ieta = hitid.ieta();
                auto tower_ids = tpd_geo.towerIds(hitid);

                auto count = std::count_if(std::begin(tower_ids), std::end(tower_ids),
                  		     [&](const auto& t) { return t.version() == tpid.version(); });

                mt_rh_energy_ += hit.energy() / get_cosh(hitid) / count ;
            }

            mt_rh_energy_lsb_ = std::round(2.0 * mt_rh_energy_) / 2.0; 
            
            rhits.erase(rh);

        // With the else, there were no matches so fill RH info with -1
        } else {
            mt_rh_energy_ = -1.0;
            mt_rh_energy_lsb_ = -1.0;
            mt_rTPRH_ = -1.0;
        }

        if (mt_rh_energy_ > 0.0) { mt_rTPRH_ = mt_tp_energy_ / mt_rh_energy_; }
        else { mt_rTPRH_ = -1.0; }

        // Fill some histograms
        bool isPeak = mt_ispeak_ == 1 ? true : false;
        bool lowRHet  = mt_rh_energy_ > 0.0 && mt_rh_energy_ <= 10.0;
        bool highRHet = mt_rh_energy_ > 10.0;

        bool tpGT0p0 = mt_tp_energy_ > 0.0;
        bool tpGT0p5 = mt_tp_energy_ > 0.5;

        if (mt_tp_soi_ != 255) {
            tprh_vs_et_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_);
            tprh_vs_et_all_->Fill(mt_rh_energy_, mt_rTPRH_);
            if (tpGT0p0) {
                tprh_vs_et_tpgt0p0_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_); 
                tprh_vs_et_all_tpgt0p0_->Fill(mt_rh_energy_, mt_rTPRH_);
            }
            if (tpGT0p5) {
                tprh_vs_et_tpgt0p5_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_);
                tprh_vs_et_all_tpgt0p5_->Fill(mt_rh_energy_, mt_rTPRH_);
            }

            if (lowRHet) {

                tprh_vs_ieta_low_->Fill(ieta,  mt_rTPRH_);

                if (tpGT0p0) { tprh_vs_ieta_low_tpgt0p0_->Fill(ieta,  mt_rTPRH_); }
                if (tpGT0p5) { tprh_vs_ieta_low_tpgt0p5_->Fill(ieta,  mt_rTPRH_); }

            } else if (highRHet) {

                tprh_vs_ieta_high_->Fill(ieta,  mt_rTPRH_);

                if (tpGT0p0) { tprh_vs_ieta_high_tpgt0p0_->Fill(ieta,  mt_rTPRH_); }
                if (tpGT0p5) { tprh_vs_ieta_high_tpgt0p5_->Fill(ieta,  mt_rTPRH_); }
            }

            // Nominal case when we have a peak and peak finding does not zero TP
            if (isPeak) {
                tprh_vs_et_peak_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_);
                tprh_vs_et_all_peak_->Fill(mt_rh_energy_, mt_rTPRH_);
                if (tpGT0p0) {
                    tprh_vs_et_tpgt0p0_peak_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_); 
                    tprh_vs_et_all_tpgt0p0_peak_->Fill(mt_rh_energy_, mt_rTPRH_);
                }
                if (tpGT0p5) {
                    tprh_vs_et_tpgt0p5_peak_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_);
                    tprh_vs_et_all_tpgt0p5_peak_->Fill(mt_rh_energy_, mt_rTPRH_);
                }

                if (lowRHet) {

                    tprh_vs_ieta_low_peak_->Fill(ieta,  mt_rTPRH_);

                    if (tpGT0p0) { tprh_vs_ieta_low_tpgt0p0_peak_->Fill(ieta,  mt_rTPRH_); }
                    if (tpGT0p5) { tprh_vs_ieta_low_tpgt0p5_peak_->Fill(ieta,  mt_rTPRH_); }

                } else if (highRHet) {

                    tprh_vs_ieta_high_peak_->Fill(ieta,  mt_rTPRH_);

                    if (tpGT0p0) { tprh_vs_ieta_high_tpgt0p0_peak_->Fill(ieta,  mt_rTPRH_); }
                    if (tpGT0p5) { tprh_vs_ieta_high_tpgt0p5_peak_->Fill(ieta,  mt_rTPRH_); }
                }
            } else { // Now we preserve energy of non-peak TPs
                tprh_vs_et_noPeak_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_);
                tprh_vs_et_all_noPeak_->Fill(mt_rh_energy_, mt_rTPRH_);
                if (tpGT0p0) {
                    tprh_vs_et_tpgt0p0_noPeak_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_); 
                    tprh_vs_et_all_tpgt0p0_noPeak_->Fill(mt_rh_energy_, mt_rTPRH_);
                }
                if (tpGT0p5) {
                    tprh_vs_et_tpgt0p5_noPeak_[aieta]->Fill(mt_rh_energy_, mt_rTPRH_);
                    tprh_vs_et_all_tpgt0p5_noPeak_->Fill(mt_rh_energy_, mt_rTPRH_);
                }

                if (lowRHet) {

                    tprh_vs_ieta_low_noPeak_->Fill(ieta,  mt_rTPRH_);

                    if (tpGT0p0) { tprh_vs_ieta_low_tpgt0p0_noPeak_->Fill(ieta,  mt_rTPRH_); }
                    if (tpGT0p5) { tprh_vs_ieta_low_tpgt0p5_noPeak_->Fill(ieta,  mt_rTPRH_); }

                } else if (highRHet) {

                    tprh_vs_ieta_high_->Fill(ieta,  mt_rTPRH_);

                    if (tpGT0p0) { tprh_vs_ieta_high_tpgt0p0_noPeak_->Fill(ieta,  mt_rTPRH_); }
                    if (tpGT0p5) { tprh_vs_ieta_high_tpgt0p5_noPeak_->Fill(ieta,  mt_rTPRH_); }
                }
            }
        }
        matches_->Fill();

    }
    
    events_->Fill();
}

//define this as a plug-in
DEFINE_FWK_MODULE(HcalCompareUpgradeChains);
