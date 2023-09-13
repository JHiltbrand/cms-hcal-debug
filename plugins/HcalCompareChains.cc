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

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalTrigTowerGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "TTree.h"

class HcalCompareChains : public edm::one::EDAnalyzer<> {
    public:
        explicit HcalCompareChains(const edm::ParameterSet&);
        ~HcalCompareChains();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&);

        // ----------member data ---------------------------
        edm::InputTag digis_pfa1p_;
        edm::InputTag digis_pfa2_;
        edm::InputTag rechits_;

        TTree *TPs_;

        unsigned int run_;
        unsigned int evt_;
        uint16_t ls_;
        int8_t nVtx_;

        float    tp_energy_pfa1p_;
        float    tp_energy_pfa2_;
        uint16_t tp_soi_pfa1p_;
        uint16_t tp_soi_pfa2_;
        int8_t   tp_ieta_;
        uint8_t  tp_iphi_;
        uint8_t  tp_exist_pfa2_;

        uint8_t rh_exist_mahi_;
        float rh_energy_mahi_;
        float rh_energy_aux_;
        float rh_energy_raw_;
        float rh_energy_mahi_cutoff_;
        float rh_energy_depth1_mahi_;
        float rh_energy_depth2_mahi_;
        float rh_energy_depth3_mahi_;
        float rh_energy_depth4_mahi_;
        float rh_energy_depth5_mahi_;
        float rh_energy_depth6_mahi_;
        float rh_energy_depth7_mahi_;

        float rh_chi2_depth1_mahi_;
        float rh_chi2_depth2_mahi_;
        float rh_chi2_depth3_mahi_;
        float rh_chi2_depth4_mahi_;
        float rh_chi2_depth5_mahi_;
        float rh_chi2_depth6_mahi_;
        float rh_chi2_depth7_mahi_;

        uint16_t rh_depth_profile_;

        float rh_chi2_worst_;
        uint8_t rh_chi2_worst_depth_;
        float rh_chi2_sum_;

        unsigned int maxVtx_;

        edm::ESGetToken<HcalTrigTowerGeometry, CaloGeometryRecord> tpd_geo_tok_;
        edm::ESGetToken<CaloGeometry, CaloGeometryRecord> gen_geo_tok_;
        edm::ESGetToken<CaloTPGTranscoder, CaloTPGRecord> decoder_tok_;
        edm::EDGetTokenT<reco::VertexCollection> vtx_tok_;
};

HcalCompareChains::HcalCompareChains(const edm::ParameterSet& config) :
    digis_pfa1p_(config.getParameter<edm::InputTag>("triggerPrimitives_pfa1p")),
    digis_pfa2_(config.getParameter<edm::InputTag>("triggerPrimitives_pfa2")),
    rechits_(config.getParameter<edm::InputTag>("recHits")),
    tpd_geo_tok_(esConsumes<HcalTrigTowerGeometry, CaloGeometryRecord>(edm::ESInputTag("", ""))),
    gen_geo_tok_(esConsumes<CaloGeometry, CaloGeometryRecord>(edm::ESInputTag("", ""))),
    decoder_tok_(esConsumes<CaloTPGTranscoder, CaloTPGRecord>(edm::ESInputTag("", ""))),
    vtx_tok_(consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices", "", "RECO")))
{
    consumes<HcalTrigPrimDigiCollection>(digis_pfa1p_);
    consumes<HcalTrigPrimDigiCollection>(digis_pfa2_);
    consumes<edm::SortedCollection<HBHERecHit>>(rechits_);

    maxVtx_   = config.getParameter<unsigned int>("maxVtx");

    edm::Service<TFileService> fs;

    TPs_ = fs->make<TTree>("TPs", "Matched TP from multiple algos and RHs");
    TPs_->Branch("RH_energy_MAHI",        &rh_energy_mahi_,       "RH_energy_MAHI/F");
    TPs_->Branch("RH_energy_raw",         &rh_energy_raw_,        "RH_energy_raw/F");
    TPs_->Branch("RH_energy_aux",         &rh_energy_aux_,        "RH_energy_aux/F");

    TPs_->Branch("RH_energy_MAHI_cutoff", &rh_energy_mahi_cutoff_, "RH_energy_MAHI_cutoff/F");
    TPs_->Branch("RH_exist_MAHI",         &rh_exist_mahi_,         "RH_exist_MAHI/b");
    TPs_->Branch("RH_chi2_worst",         &rh_chi2_worst_,         "RH_chi2_worst/F");
    TPs_->Branch("RH_chi2_worst_depth",   &rh_chi2_worst_depth_,   "RH_chi2_worst_depth/b");
    TPs_->Branch("RH_chi2_sum",           &rh_chi2_sum_,           "RH_chi2_sum/F");

    TPs_->Branch("RH_energy_depth1_MAHI", &rh_energy_depth1_mahi_, "RH_energy_depth1_MAHI/F");
    TPs_->Branch("RH_energy_depth2_MAHI", &rh_energy_depth2_mahi_, "RH_energy_depth2_MAHI/F");
    TPs_->Branch("RH_energy_depth3_MAHI", &rh_energy_depth3_mahi_, "RH_energy_depth3_MAHI/F");
    TPs_->Branch("RH_energy_depth4_MAHI", &rh_energy_depth4_mahi_, "RH_energy_depth4_MAHI/F");
    TPs_->Branch("RH_energy_depth5_MAHI", &rh_energy_depth5_mahi_, "RH_energy_depth5_MAHI/F");
    TPs_->Branch("RH_energy_depth6_MAHI", &rh_energy_depth6_mahi_, "RH_energy_depth6_MAHI/F");
    TPs_->Branch("RH_energy_depth7_MAHI", &rh_energy_depth7_mahi_, "RH_energy_depth7_MAHI/F");

    TPs_->Branch("RH_chi2_depth1_MAHI",   &rh_chi2_depth1_mahi_,   "RH_chi2_depth1_MAHI/F");
    TPs_->Branch("RH_chi2_depth2_MAHI",   &rh_chi2_depth2_mahi_,   "RH_chi2_depth2_MAHI/F");
    TPs_->Branch("RH_chi2_depth3_MAHI",   &rh_chi2_depth3_mahi_,   "RH_chi2_depth3_MAHI/F");
    TPs_->Branch("RH_chi2_depth4_MAHI",   &rh_chi2_depth4_mahi_,   "RH_chi2_depth4_MAHI/F");
    TPs_->Branch("RH_chi2_depth5_MAHI",   &rh_chi2_depth5_mahi_,   "RH_chi2_depth5_MAHI/F");
    TPs_->Branch("RH_chi2_depth6_MAHI",   &rh_chi2_depth6_mahi_,   "RH_chi2_depth6_MAHI/F");
    TPs_->Branch("RH_chi2_depth7_MAHI",   &rh_chi2_depth7_mahi_,   "RH_chi2_depth7_MAHI/F");

    TPs_->Branch("RH_depth_profile",      &rh_depth_profile_,      "RH_depth_profile/s");

    TPs_->Branch("TP_energy_PFA1p",       &tp_energy_pfa1p_,       "TP_energy_PFA1p/F");
    TPs_->Branch("TP_energy_PFA2",        &tp_energy_pfa2_,        "TP_energy_PFA2/F");
    TPs_->Branch("TP_exist_PFA2",         &tp_exist_pfa2_,         "TP_exist_PFA2/b");
    TPs_->Branch("TP_soi_PFA1p",          &tp_soi_pfa1p_,          "TP_soi_PFA1p/s");
    TPs_->Branch("TP_soi_PFA2",           &tp_soi_pfa2_,           "TP_soi_PFA2/s");

    TPs_->Branch("nVtx",                  &nVtx_,                  "nVtx/B");
    TPs_->Branch("run",                   &run_,                   "run/i");
    TPs_->Branch("evt",                   &evt_,                   "evt/i");
    TPs_->Branch("ls",                    &ls_,                    "ls/s");
    TPs_->Branch("ieta",                  &tp_ieta_,               "ieta/B");
    TPs_->Branch("iphi",                  &tp_iphi_,               "iphi/b");
}

HcalCompareChains::~HcalCompareChains() {}

void HcalCompareChains::beginLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup& setup)
{
}

void HcalCompareChains::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
    using namespace edm;

    const auto& tpd_geo  = setup.getHandle(tpd_geo_tok_);
    const auto& gen_geo  = setup.getHandle(gen_geo_tok_);
    const auto& vertices = event.getHandle(vtx_tok_);
    const auto& decoder  = setup.getHandle(decoder_tok_);

    run_ = event.id().run();
    evt_ = event.id().event();
    ls_  = event.id().luminosityBlock();

    nVtx_ = -1;
    if (vertices.isValid()) {
        nVtx_ = 0;
        for (reco::VertexCollection::const_iterator it = vertices->begin(); it != vertices->end() && nVtx_ < (int) maxVtx_; ++it)
        {
            if (!it->isFake())
            {
              ++nVtx_;
            }
        }
    }

    std::map<HcalTrigTowerDetId, std::vector<HBHERecHit>>  rhits;
    std::map<HcalTrigTowerDetId, std::vector<HcalTriggerPrimitiveDigi>> tpdigis_pfa1p;
    std::map<HcalTrigTowerDetId, std::vector<HcalTriggerPrimitiveDigi>> tpdigis_pfa2;

    Handle<HcalTrigPrimDigiCollection> digis_pfa1p;
    if (!event.getByLabel(digis_pfa1p_, digis_pfa1p))
    {
        LogError("HcalTrigPrimDigiCleaner") << "Can't find hcal trigger primitive digi collection with tag hcalDigis'" << digis_pfa1p_ << "'" << std::endl;
        return;
    }

    Handle<HcalTrigPrimDigiCollection> digis_pfa2;
    if (!event.getByLabel(digis_pfa2_, digis_pfa2))
    {
        LogError("HcalTrigPrimDigiCleaner") << "Can't find hcal trigger primitive digi collection with tag hcalDigis'" << digis_pfa2_ << "'" << std::endl;
        return;
    }

    edm::Handle< edm::SortedCollection<HBHERecHit> > hits;
    if (!event.getByLabel(rechits_, hits))
    {
        edm::LogError("HcalCompareLegacyChains") << "Can't find rec hit collection with tag '" << rechits_ << "'" << std::endl;
    }

    if (hits.isValid())
    {
        for (auto& hit: *(hits.product()))
        {
            HcalDetId id(hit.id());

            auto tower_ids = tpd_geo->towerIds(id);
            for (auto& tower_id: tower_ids)
            {
                tower_id = HcalTrigTowerDetId(tower_id.ieta(), tower_id.iphi(), 1);

                if (hit.chi2() > 0.0)
                {
                    rhits[tower_id].push_back(hit);
                }
            }
        }
    }

    for (const auto& digi_pfa1p : *digis_pfa1p)
    {
        const auto tower_id = HcalTrigTowerDetId(digi_pfa1p.id().ieta(), digi_pfa1p.id().iphi(), 1, digi_pfa1p.id().version());
        tpdigis_pfa1p[tower_id].push_back(digi_pfa1p);
    }

    for (const auto& digi_pfa2 : *digis_pfa2)
    {
        const auto tower_id = HcalTrigTowerDetId(digi_pfa2.id().ieta(), digi_pfa2.id().iphi(), 1, digi_pfa2.id().version());
        tpdigis_pfa2[tower_id].push_back(digi_pfa2);
    }

    for (const auto& tpdigi_pfa1p : tpdigis_pfa1p)
    {
        auto tt_id = tpdigi_pfa1p.first;
        tp_ieta_   = tt_id.ieta();
        tp_iphi_   = tt_id.iphi();

        if (tp_ieta_ < -28 or tp_ieta_ > 28)
            continue;

        tp_energy_pfa1p_ = 0.0;
        tp_soi_pfa1p_    = 0;
        for (const auto& digi : tpdigi_pfa1p.second)
        {
            tp_energy_pfa1p_ += decoder->hcaletValue(tt_id, digi.t0());
            tp_soi_pfa1p_    += digi.SOI_compressedEt();
        }

        tp_exist_pfa2_  = 0;
        tp_energy_pfa2_ = 0.0;
        tp_soi_pfa2_    = 0;

        const auto tpdigi_pfa2 = tpdigis_pfa2.find(tt_id);
        if (tpdigi_pfa2 != tpdigis_pfa2.end())
        { 
            tp_exist_pfa2_ = 1;
            for (const auto& digi : tpdigi_pfa2->second)
            {
                tp_energy_pfa2_ += decoder->hcaletValue(tt_id, digi.t0());
                tp_soi_pfa2_    += digi.SOI_compressedEt();
            }
        } 

        rh_energy_mahi_        = 0.0; 
        rh_energy_aux_         = 0.0; 
        rh_energy_raw_         = 0.0; 
        rh_energy_mahi_cutoff_ = 0.0; 
        rh_chi2_worst_         = 0.0; 
        rh_chi2_worst_depth_   = 0; 
        rh_chi2_sum_           = 0.0; 

        rh_energy_depth1_mahi_ = 0.0;
        rh_energy_depth2_mahi_ = 0.0;
        rh_energy_depth3_mahi_ = 0.0;
        rh_energy_depth4_mahi_ = 0.0;
        rh_energy_depth5_mahi_ = 0.0;
        rh_energy_depth6_mahi_ = 0.0;
        rh_energy_depth7_mahi_ = 0.0;

        rh_chi2_depth1_mahi_ = 0.0;
        rh_chi2_depth2_mahi_ = 0.0;
        rh_chi2_depth3_mahi_ = 0.0;
        rh_chi2_depth4_mahi_ = 0.0;
        rh_chi2_depth5_mahi_ = 0.0;
        rh_chi2_depth6_mahi_ = 0.0;
        rh_chi2_depth7_mahi_ = 0.0;

        rh_depth_profile_ = 0;

        auto rh = rhits.find(tt_id);
        if (rh != rhits.end())
        {
            std::vector<unsigned int> rh_depths = {0, 0, 0, 0, 0, 0, 0};

            for (const auto& hit: rh->second)
            {
                HcalDetId id(hit.id());

                if (abs(hit.id().ieta()) == 16 && hit.id().depth() == 4) continue;

                // Q: do i care about dead rec hits ?
                // A: no, i do not
                if (hit.energy() == 0.0) continue;

                rh_exist_mahi_ = 1;

                // Define minimum cutoffs in individual RH energy
                bool passPFHB = id.subdet() == 1 and hit.energy() > 0.3;
                bool passPFHE = id.subdet() == 2 and hit.energy() > 0.8;

                const auto *sub_geo = dynamic_cast<const HcalGeometry*>(gen_geo->getSubdetectorGeometry(id));
                auto cosheta = cosh(sub_geo->getPosition(id).eta());

                auto tower_ids = tpd_geo->towerIds(id);
                auto count = std::count_if(std::begin(tower_ids), std::end(tower_ids), [&](const auto& t) { return t.version() == tt_id.version(); });
                rh_energy_mahi_ += hit.energy() / cosheta / count;
                rh_energy_raw_ += hit.eraw() / cosheta / count;
                rh_energy_aux_ += hit.eaux() / cosheta / count;
                rh_chi2_sum_ += hit.chi2();

                if (passPFHB or passPFHE)
                    rh_energy_mahi_cutoff_ += hit.energy() / cosheta / count;

                if (hit.chi2() > rh_chi2_worst_)
                {
                    rh_chi2_worst_       = hit.chi2();
                    rh_chi2_worst_depth_ = hit.id().depth();
                }

                rh_depths.at(hit.id().depth()-1) = 1;

                switch (hit.id().depth())
                {
                    case 1:
                        rh_energy_depth1_mahi_ += hit.energy() / cosheta / count;
                        rh_chi2_depth1_mahi_ += hit.chi2(); 
                        break;
                    case 2:
                        rh_energy_depth2_mahi_ += hit.energy() / cosheta / count;
                        rh_chi2_depth2_mahi_ += hit.chi2(); 
                        break;
                    case 3:
                        rh_energy_depth3_mahi_ += hit.energy() / cosheta / count;
                        rh_chi2_depth3_mahi_ += hit.chi2(); 
                        break;
                    case 4:
                        rh_energy_depth4_mahi_ += hit.energy() / cosheta / count;
                        rh_chi2_depth4_mahi_ += hit.chi2(); 
                        break;
                    case 5:
                        rh_energy_depth5_mahi_ += hit.energy() / cosheta / count;
                        rh_chi2_depth5_mahi_ += hit.chi2(); 
                        break;
                    case 6:
                        rh_energy_depth6_mahi_ += hit.energy() / cosheta / count;
                        rh_chi2_depth6_mahi_ += hit.chi2(); 
                        break;
                    case 7:
                       rh_energy_depth7_mahi_ += hit.energy() / cosheta / count;
                       rh_chi2_depth7_mahi_ += hit.chi2(); 
                       break;
                }
            }

            auto vecLen = rh_depths.size();
            unsigned int firstDepth = 7;
            unsigned int lastDepth = 1;
            unsigned int ndepths = 0;
            for (unsigned int i = 0; i < vecLen; i++)
            {
                if (rh_depths.at(i) == 1) 
                {
                    if (i+1 < firstDepth)
                    {
                        firstDepth = i+1;
                        lastDepth = i+1;
                    }
                    if (i+1 > lastDepth)
                        lastDepth = i+1;
                    ndepths++;
                }
            }
            rh_depth_profile_ = firstDepth * 100 + ndepths * 10 + lastDepth;
            rhits.erase(rh);
        } else
        {
            rh_exist_mahi_ = 0;
        }

        TPs_->Fill();
    }
}

void HcalCompareChains::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HcalCompareChains);

