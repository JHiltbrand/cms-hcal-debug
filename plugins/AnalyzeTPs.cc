// system include files
#include <string>
#include <unordered_map>
#include <unordered_set>

// CMSSW include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGRecord.h"

#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/Common/interface/TriggerResults.h"

// ROOT include files
#include "TTree.h"

class AnalyzeTPs : public edm::one::EDAnalyzer<> {
    public:
        explicit AnalyzeTPs(const edm::ParameterSet&);
        ~AnalyzeTPs();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void analyze(const edm::Event&, const edm::EventSetup&);

        // Applicable constants for Phase I, Run3
        static const unsigned int FGCOUNT    = 7;
        static const unsigned int TPSAMPLES  = 4;
        static const unsigned int MAXSAMPLES = 10;
        static const uint8_t      NIETARINGS = 16 /**HB**/ + 12 /**HE**/ + 12 /**HF**/;
        
        // For total ieta bins, need a dummy ieta = 0 and two (+,-) ieta = 29
        static const uint8_t      TOTALIETABINS = 2 * (NIETARINGS + 1) + 1;

        // For TP ET bins, LSB is 0.5 GeV, so double ET range, and add one more bin for 0
        static const uint16_t     NTPETBINS = 2 * 128 + 1;

        edm::EDGetTokenT<HcalTrigPrimDigiCollection>      tok_packedDigis_;
        edm::EDGetTokenT<HcalTrigPrimDigiCollection>      tok_reemulDigis_;
        edm::ESGetToken<CaloTPGTranscoder, CaloTPGRecord> tok_hcalCoder_;
        edm::EDGetTokenT<reco::VertexCollection>          tok_vtx_;
        edm::EDGetTokenT<edm::TriggerResults>             tok_trig_;

        TTree* tps_;

        unsigned int       run_;
        uint16_t           lumi_;
        unsigned long long event_;
        int8_t             nVtx_;
        uint8_t            zero_bias_;

        int8_t  tp_ieta_;
        uint8_t tp_iphi_;
        float   tp_et_packed_;
        float   tp_et_reemul_;
        uint8_t tp_found_reemul_;
        uint8_t tp_found_packed_;

        std::array<bool, FGCOUNT>        tp_fg_packed_;
        std::array<bool, FGCOUNT>        tp_fg_reemul_;
        std::array<uint16_t, MAXSAMPLES> tp_lin_adc_in_;
        std::array<uint16_t, TPSAMPLES>  tp_lin_adc_out_;
        std::array<uint8_t, TPSAMPLES>   tp_vetoed_;

        TTree* occ_;

        int8_t  occ_nVtx_;
        int8_t  occ_ieta_;
        float   occ_et_thresh_;
        uint8_t occ_packed_;
        uint8_t occ_reemul_;
};

AnalyzeTPs::AnalyzeTPs(const edm::ParameterSet& config) :
    tok_packedDigis_(consumes<HcalTrigPrimDigiCollection>(config.getParameter<edm::InputTag>("packedTriggerPrimitives"))),
    tok_reemulDigis_(consumes<HcalTrigPrimDigiCollection>(config.getParameter<edm::InputTag>("reemulTriggerPrimitives"))),
    tok_hcalCoder_(esConsumes<CaloTPGTranscoder, CaloTPGRecord>()),
    tok_vtx_(consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices", "", "RECO"))),
    tok_trig_(consumes<edm::TriggerResults>(edm::InputTag("TriggerResults", "", "HLT")))
{
    edm::Service<TFileService> fs;

    // Set up the TTree where each entry corresponds to a single TP
    tps_ = fs->make<TTree>("tps", "Trigger primitives");
    tps_->Branch("run",          &run_,             "run/i");
    tps_->Branch("lumi",         &lumi_,            "lumi/s");
    tps_->Branch("event",        &event_,           "event/l");
    tps_->Branch("nVtx",         &nVtx_,            "nVtx/B");
    tps_->Branch("zeroBias",     &zero_bias_,       "zeroBias/b");
    tps_->Branch("ieta",         &tp_ieta_,         "ieta/B");
    tps_->Branch("iphi",         &tp_iphi_,         "iphi/b");
    tps_->Branch("et_packed",    &tp_et_packed_,    "et_packed/f");
    tps_->Branch("et_reemul",    &tp_et_reemul_,    "et_reemul/f");
    tps_->Branch("found_reemul", &tp_found_reemul_, "found_reemul/b");
    tps_->Branch("found_packed", &tp_found_packed_, "found_packed/b");

    for (unsigned int i = 0; i < tp_fg_packed_.size(); ++i) {
        std::string bname = "fg" + std::to_string(i) + "_packed";
        tps_->Branch(bname.c_str(), &tp_fg_packed_[i], (bname + "/O").c_str());
    }
    for (unsigned int i = 0; i < tp_fg_reemul_.size(); ++i) {
        std::string bname = "fg" + std::to_string(i) + "_reemul";
        tps_->Branch(bname.c_str(), &tp_fg_reemul_[i], (bname + "/O").c_str());
    }

    for (unsigned int i = 0; i < tp_lin_adc_in_.size(); ++i) {
        std::string bname = "lin_adc_in" + std::to_string(i);
        tps_->Branch(bname.c_str(), &tp_lin_adc_in_[i], (bname + "/s").c_str());
    }
    for (unsigned int i = 0; i < tp_lin_adc_out_.size(); ++i) {
        std::string bname = "lin_adc_out" + std::to_string(i);
        tps_->Branch(bname.c_str(), &tp_lin_adc_out_[i], (bname + "/s").c_str());
    }
    for (unsigned int i = 0; i < tp_vetoed_.size(); ++i) {
        std::string bname = "tp_vetoed_ts" + std::to_string(i);
        tps_->Branch(bname.c_str(), &tp_vetoed_[i], (bname + "/b").c_str());
    }

    // Setup TTree for per-event TP occupancy where each entry is an event's TP occupancy
    // for a particular ieta for a given TP ET treshold
    occ_ = fs->make<TTree>("occ", "TP occupancy");
    occ_->Branch("nVtx",        &occ_nVtx_,      "nVtx/B");
    occ_->Branch("ieta",        &occ_ieta_,      "ieta/B");
    occ_->Branch("et_thresh",   &occ_et_thresh_, "et_thresh/f");
    occ_->Branch("occu_packed", &occ_packed_,    "occu_packed/b");
    occ_->Branch("occu_reemul", &occ_reemul_,    "occu_reemul/b");

}

AnalyzeTPs::~AnalyzeTPs() {}

namespace std {
    template<> struct hash<HcalTrigTowerDetId> {
        size_t operator()(const HcalTrigTowerDetId& id) const {
           return hash<int>()(id);
        }
    };
}

void AnalyzeTPs::analyze(const edm::Event& event, const edm::EventSetup& setup) {
    using namespace edm;

    ESHandle<CaloTPGTranscoder> decoder = setup.getHandle(tok_hcalCoder_);

    const auto& packedDigis = event.getHandle(tok_packedDigis_);
    if (!packedDigis.isValid()) {
        LogError("AnalyzeTPs") << "Can't find packed hcal trigger primitive digi collection---check input tag" << std::endl;
        return;
    }

    const auto& reemulDigis = event.getHandle(tok_reemulDigis_);
    if (!reemulDigis.isValid()) {
        LogError("AnalyzeTPs") << "Can't find reemulated hcal trigger primitive digi collection---check input tag" << std::endl;
        return;
    }

    // Try and get the vertices, don't complain or stop if can't
    const auto& vertices = event.getHandle(tok_vtx_);
    nVtx_     = -1;
    occ_nVtx_ = -1;
    if (vertices.isValid()) {
        nVtx_     = 0;
        occ_nVtx_ = 0;
        for (reco::VertexCollection::const_iterator it = vertices->begin(); it != vertices->end(); ++it)
        {
            if (!it->isFake())
            {
               ++nVtx_;
               ++occ_nVtx_;
            }
        }
    }

    run_   = event.id().run();
    lumi_  = event.id().luminosityBlock();
    event_ = event.id().event();

    // Determine if the current event passed any ZeroBias trigger
    zero_bias_ = 0;
    const auto& triggers = event.getHandle(tok_trig_);
    const auto& triggerNames = event.triggerNames(*triggers);
    std::string zeroBiasPath = "HLT_ZeroBias_v";
    for (unsigned int i = 0; i < triggerNames.size(); i++) {
        if (triggerNames.triggerName(i).find(zeroBiasPath) != std::string::npos) {
            if (triggers->accept(i)) {
                zero_bias_ = 1;
            }
            break;
        }
    }

    // Initialize map for keeping track of all TPs in an event
    std::unordered_set<HcalTrigTowerDetId> ids;
    typedef std::unordered_map<HcalTrigTowerDetId, HcalTriggerPrimitiveDigi> digi_map;

    digi_map packedMap;
    for (const auto& digi : *packedDigis) {
        ids.insert(digi.id());
        packedMap[digi.id()] = digi;
    }
    std::vector<std::vector<uint8_t> > occupancy_map_packed(TOTALIETABINS, std::vector<uint8_t>(NTPETBINS, 0));

    digi_map reemulMap;
    for (const auto& digi : *reemulDigis) {
        ids.insert(digi.id());
        reemulMap[digi.id()] = digi;
    }
    std::vector<std::vector<uint8_t> > occupancy_map_reemul(TOTALIETABINS, std::vector<uint8_t>(NTPETBINS, 0));

    // Begin loop over all TPs (IDs) (both packed and reemul) found in the event
    for (const auto& id : ids) {
        if (id.version() == 1 and abs(id.ieta()) >= 40 and id.iphi() % 4 == 1)
            continue;

        tp_ieta_    = id.ieta();
        tp_iphi_    = id.iphi();

        // Ieta index for filling out TP occupancy vector of vectors
        // index 0 -> ieta -41, and index 82 -> ieta 41
        uint8_t ietaIndex = tp_ieta_ + 41;

        digi_map::const_iterator digi;
        if ((digi = packedMap.find(id)) != packedMap.end()) {
            tp_found_packed_ = 1;

            tp_et_packed_ = decoder->hcaletValue(id, digi->second.t0());

            for (std::size_t i = 0; i < tp_fg_packed_.size(); ++i)
               tp_fg_packed_[i] = digi->second.t0().fineGrain(i);

            // TP ET index for filling out TP occupancy vector of vectors
            // index 0 -> ET = 0, index 256 -> ET = 128
            uint16_t etPackedIndex = tp_et_packed_ * 2.0;
            // TP ET here is used as a threshold, so a TP with particular ET
            // would occupy all ET bins up-to-and-including the specific ET
            for (auto i = 0; i <= etPackedIndex; i++)
                occupancy_map_packed[ietaIndex][i] += 1;
            
        // Not clear when (if ever) we have a packed TP but no reemul TP...
        } else {
            tp_found_packed_ = 0;
        }

        // Now looking for the ID in the reemul digi map
        if ((digi = reemulMap.find(id)) != reemulMap.end()) {
            tp_found_reemul_ = 1;

            tp_et_reemul_ = decoder->hcaletValue(id, digi->second.t0());

            for (std::size_t i = 0; i < tp_fg_reemul_.size(); ++i)
               tp_fg_reemul_[i] = digi->second.t0().fineGrain(i);

            // From custom (reemul-only) TP digi getting original linearized ADC frame
            const auto& input_lin_adc = digi->second.getInputLinearFrame();
            for (std::size_t i = 0; i < input_lin_adc.size(); ++i)
               tp_lin_adc_in_[i] = input_lin_adc[i];

            // From custom (reemul-only) TP digi getting original output ADC frame
            // post-OOTPU-subtraction and vetoing
            const auto& output_lin_adc = digi->second.getOutputLinearFrame();
            for (std::size_t i = 0; i < output_lin_adc.size(); ++i)
               tp_lin_adc_out_[i] = output_lin_adc[i];

            // From custom (reemul-only) TP digi getting veto bool for each TP sample
            const auto& vetoed_tps = digi->second.getVetoedTPs();
            for (std::size_t i = 0; i < vetoed_tps.size(); ++i)
               tp_vetoed_[i] = vetoed_tps[i];

            // TP ET index for filling out TP occupancy vector of vectors
            // index 0 -> ET = 0, index 256 -> ET = 128
            uint16_t etReemulIndex = tp_et_reemul_ * 2.0;
            // TP ET here is used as a threshold, so a TP with particular ET
            // would occupy all ET bins up-to-and-including the specific ET
            for (std::size_t i = 0; i <= etReemulIndex; i++)
                occupancy_map_reemul[ietaIndex][i] += 1;

        // Again, not clear when (if ever) we have a reemul TP but no packed TP...
        } else {
            tp_found_reemul_ = 0;
        }
        tps_->Fill();
    }

    // Having gone over all TPs in the event, loop over the occupancy "map"
    // and fill the separate TTree
    for (auto ietaIndex = 0; ietaIndex < TOTALIETABINS; ietaIndex++) {
        for (auto etIndex = 0; etIndex < NTPETBINS; etIndex++) {
            occ_ieta_      = ietaIndex - NIETARINGS;

            // Skip unphysical tower ietas
            if (occ_ieta_ == 0 or abs(occ_ieta_) == 29)
                continue;

            occ_et_thresh_ = float(etIndex) / 2.0;
            occ_packed_    = occupancy_map_packed[ietaIndex][etIndex];
            occ_reemul_    = occupancy_map_reemul[ietaIndex][etIndex];
            occ_->Fill();
        }
    }
} 

void
AnalyzeTPs::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(AnalyzeTPs);
