// system include files
#include <string>
#include <unordered_map>
#include <unordered_set>

// user include files
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

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
//
// class declaration
//

class AnalyzeTPs : public edm::one::EDAnalyzer<> {
    public:
        explicit AnalyzeTPs(const edm::ParameterSet&);
        ~AnalyzeTPs();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void analyze(const edm::Event&, const edm::EventSetup&);

        // Applicable Phase I, Run3
        static const unsigned int FGCOUNT    = 7;
        static const unsigned int TPSAMPLES  = 4;
        static const unsigned int MAXSAMPLES = 10;

        edm::InputTag packedDigis_;
        edm::InputTag reemulDigis_;

        bool swap_iphi_;

        edm::ESGetToken<CaloTPGTranscoder, CaloTPGRecord> tok_hcalCoder_;
        edm::EDGetTokenT<reco::VertexCollection>          tok_vtx_;
        edm::EDGetTokenT<edm::TriggerResults>             tok_trig_;

        TTree *tps_;

        unsigned int       run_;
        uint16_t           lumi_;
        unsigned long long event_;
        int8_t             nVtx_;
        uint8_t            zero_bias_;

        int8_t  tp_ieta_;
        uint8_t tp_iphi_;
        uint8_t tp_version_;
        float   tp_et_packed_;
        float   tp_et_reemul_;
        uint8_t tp_found_reemul_;
        uint8_t tp_found_packed_;

        std::array<bool, FGCOUNT>        tp_fg_packed_;
        std::array<bool, FGCOUNT>        tp_fg_reemul_;
        std::array<uint16_t, MAXSAMPLES> tp_lin_adc_in_;
        std::array<uint16_t, TPSAMPLES>  tp_lin_adc_out_;
        std::array<uint8_t, TPSAMPLES>   tp_vetoed_;
};

AnalyzeTPs::AnalyzeTPs(const edm::ParameterSet& config) :
    packedDigis_(config.getParameter<edm::InputTag>("packedTriggerPrimitives")),
    reemulDigis_(config.getParameter<edm::InputTag>("reemulTriggerPrimitives")),
    swap_iphi_(config.getParameter<bool>("swapIphi")),
    tok_hcalCoder_(esConsumes<CaloTPGTranscoder, CaloTPGRecord>()),
    tok_vtx_(consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices", "", "RECO"))),
    tok_trig_(consumes<edm::TriggerResults>(edm::InputTag("TriggerResults", "", "HLT")))
{
    edm::Service<TFileService> fs;

    consumes<HcalTrigPrimDigiCollection>(packedDigis_);
    consumes<HcalTrigPrimDigiCollection>(reemulDigis_);

    tps_ = fs->make<TTree>("tps", "Trigger primitives");
    tps_->Branch("run",          &run_,             "run/i");
    tps_->Branch("lumi",         &lumi_,            "lumi/s");
    tps_->Branch("event",        &event_,           "event/l");
    tps_->Branch("nVtx",         &nVtx_,            "nVtx/B");
    tps_->Branch("zeroBias",     &zero_bias_,       "zeroBias/b");
    tps_->Branch("ieta",         &tp_ieta_,         "ieta/B");
    tps_->Branch("iphi",         &tp_iphi_,         "iphi/b");
    tps_->Branch("version",      &tp_version_,      "version/b");
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

    run_ = event.id().run();
    lumi_ = event.id().luminosityBlock();
    event_ = event.id().event();

    Handle<HcalTrigPrimDigiCollection> packedDigis;
    if (!event.getByLabel(packedDigis_, packedDigis)) {
        LogError("AnalyzeTPs") <<
           "Can't find hcal trigger primitive digi collection with tag '" <<
           packedDigis_ << "'" << std::endl;
        return;
    }

    Handle<HcalTrigPrimDigiCollection> reemulDigis;
    if (!event.getByLabel(reemulDigis_, reemulDigis)) {
        LogError("AnalyzeTPs") <<
           "Can't find emulated hcal trigger primitive digi collection with tag '" <<
           reemulDigis_ << "'" << std::endl;
        return;
    }

    ESHandle<CaloTPGTranscoder> decoder = setup.getHandle(tok_hcalCoder_);

    const auto& vertices = event.getHandle(tok_vtx_);
    nVtx_ = -1;
    if (vertices.isValid()) {
        nVtx_ = 0;
        for (reco::VertexCollection::const_iterator it = vertices->begin(); it != vertices->end(); ++it)
        {
            if (!it->isFake())
            {
              ++nVtx_;
            }
        }
    }

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

    std::unordered_set<HcalTrigTowerDetId> ids;
    typedef std::unordered_map<HcalTrigTowerDetId, HcalTriggerPrimitiveDigi> digi_map;
    digi_map packedMap;
    digi_map reemulMap;

    for (const auto& digi: *packedDigis) {
        ids.insert(digi.id());
        packedMap[digi.id()] = digi;
    }

    for (const auto& digi: *reemulDigis) {
        ids.insert(digi.id());
        reemulMap[digi.id()] = digi;
    }

    for (const auto& id: ids) {
        if (id.version() == 1 and abs(id.ieta()) >= 40 and id.iphi() % 4 == 1)
            continue;

        tp_ieta_    = id.ieta();
        tp_iphi_    = id.iphi();
        tp_version_ = id.version();
        digi_map::const_iterator digi;
        if ((digi = packedMap.find(id)) != packedMap.end()) {
            tp_et_packed_ = decoder->hcaletValue(id, digi->second.t0());
            for (unsigned int i = 0; i < tp_fg_packed_.size(); ++i)
               tp_fg_packed_[i] = digi->second.t0().fineGrain(i);

            tp_found_packed_ = 1;
        } else {
            tp_et_packed_ = 0;
            for (unsigned int i = 0; i < tp_fg_packed_.size(); ++i)
               tp_fg_packed_[i] = 0;

            tp_found_packed_ = 0;
        }

        auto new_id(id);
        if (swap_iphi_ and id.version() == 1 and id.ieta() > 28 and id.ieta() < 40) {
            if (id.iphi() % 4 == 1)
               new_id = HcalTrigTowerDetId(id.ieta(), (id.iphi() + 70) % 72, id.depth(), id.version());
            else
               new_id = HcalTrigTowerDetId(id.ieta(), (id.iphi() + 2) % 72 , id.depth(), id.version());
        }

        if ((digi = reemulMap.find(new_id)) != reemulMap.end()) {
            tp_et_reemul_ = decoder->hcaletValue(id, digi->second.t0());
            for (unsigned int i = 0; i < tp_fg_reemul_.size(); ++i)
               tp_fg_reemul_[i] = digi->second.t0().fineGrain(i);

            const auto& input_lin_adc = digi->second.getInputLinearFrame();
            for (unsigned int i = 0; i < input_lin_adc.size(); ++i)
               tp_lin_adc_in_[i] = input_lin_adc[i];

            const auto& output_lin_adc = digi->second.getOutputLinearFrame();
            for (unsigned int i = 0; i < output_lin_adc.size(); ++i)
               tp_lin_adc_out_[i] = output_lin_adc[i];

            const auto& vetoed_tps = digi->second.getVetoedTPs();
            for (unsigned int i = 0; i < vetoed_tps.size(); ++i)
               tp_vetoed_[i] = vetoed_tps[i];

            tp_found_reemul_ = 1;
        } else {
            tp_et_reemul_ = 0;
            for (unsigned int i = 0; i < tp_fg_reemul_.size(); ++i)
               tp_fg_reemul_[i] = 0;

            tp_found_reemul_ = 0;
        }
        tps_->Fill();
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
