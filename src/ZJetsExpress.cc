// -*- C++ -*-
//
// Package:    ZJetsExpress
// Class:      ZJetsExpress
// 
/**\class ZJetsExpress ZJetsExpress.cc ElectroWeakAnalysis/VPlusJets/src/ZJetsExpress.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  A. Marini, K. Kousouris,  K. Theofilatos
//         Created:  Mon Oct 31 07:52:10 CDT 2011
// $Id: ZJetsExpress.cc,v 1.8 2011/11/09 13:53:32 kkousour Exp $
//
//

// system include files
#include <memory>

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>


#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TH1I.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h" 
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonIsolation.h" 
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h" 
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"
#include "RecoCaloTools/Navigation/interface/CaloNavigator.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

//
// class declaration
//
using namespace edm;
using namespace std;
using namespace reco;

class ZJetsExpress : public edm::EDAnalyzer {
   public:
      explicit ZJetsExpress(const edm::ParameterSet&);
      ~ZJetsExpress();

   private:
      virtual void beginJob();
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob();
      // ---- method that builds the tree -------------------------------
      void buildTree();
      // ---- method that re-initializes the tree branches --------------
      void clearTree();
      // ---- structures for temporary object storage -------------------
      struct LEPTON {
        // ---- momentum 4-vector ---------------------------------------
        TLorentzVector p4; 
        // ---- standard isolation --------------------------------------
        float iso;
        // ---- modified isolation --------------------------------------
        float isoPF; 
        // ---- charge id (+/-1: electrons, +/-2: muons) ---------------- 
        int chid; 
        // ---- tight id ------------------------------------------------
        int id;
      };
      struct GENLEPTON {
        // ---- momentum 4-vector ---------------------------------------
        TLorentzVector p4;  
        // ---- pdgid ---------------------------------------------------
        int pdgId; 
      };
      struct JET {
        // ---- momentum 4-vector ---------------------------------------
        TLorentzVector p4; 
        // ---- tight id ------------------------------------------------
        int   id; 
        // ---- jet area (needed for pu subtraction) --------------------
        float area;
        // ---- track pt fraction associated with the PV ----------------
        float beta;
        // ---- track pt fraction associated with secondary vertices ----
        float betaStar;
        // ---- jet energy correction factor ----------------------------
        float jec; 
        // ---- jet energy uncertainty ----------------------------------
        float unc;
        // ---- charged hadron energy fraction --------------------------
        float chf;
        // ---- neutral hadron energy fraction --------------------------
        float nhf;
        // ---- photon energy fraction ---------------------------------- 
        float phf;
        // ---- muon energy fraction ------------------------------------ 
        float muf;
        // ---- electron energy fraction --------------------------------
        float elf;
      };
      // ---- sorting rules ---------------------------------------------
      static bool lepSortingRule(LEPTON x, LEPTON y)                {return x.p4.Pt() > y.p4.Pt();}
      static bool lepSortingRuleGEN(GENLEPTON x, GENLEPTON y)       {return x.p4.Pt() > y.p4.Pt();}
      static bool jetSortingRule(JET x, JET y)                      {return x.p4.Pt() > y.p4.Pt();}
      static bool p4SortingRule(TLorentzVector x, TLorentzVector y) {return x.Pt() > y.Pt();}
      // ---------- member data -----------------------------------------
      edm::Service<TFileService> fTFileService;	
      TTree *myTree_;
      // ---- histogram to record the number of events ------------------
      TH1I  *hRecoLeptons_,*hGenLeptons_;
      // ---- simulated in-time pile-up ---------------------------------
      TH1D  *mcPU_;
      // ---- flag to set the JEC uncertainty object --------------------
      bool mIsJECuncSet;
      // ---- jet energy corrector object -------------------------------
      const JetCorrector *mJEC;
      // ---- jet energy uncertainty object -----------------------------
      JetCorrectionUncertainty *mJECunc;
      // ---- configurable parameters -----------------------------------
      bool          mIsMC;
      int           mMinNjets;
      double        mMinJetPt,mMaxJetEta,mMinLepPt,mMaxLepEta,mJetLepIsoR;
      string        mJECservice,mPayloadName;
      edm::InputTag mJetsName,mSrcRho;
      // ---- tree variables --------------------------------------------
      // ---- event number ----------------------------------------------
      int eventNum_;
      // ---- run number ------------------------------------------------  
      int runNum_; 
      // ---- lumi section ----------------------------------------------
      int lumi_;
      // ---- flag to identify real data --------------------------------
      int isRealData_;
      // ---- dilepton mass ---------------------------------------------
      float llM_,llMGEN_;
      // ---- dilepton rapidity -----------------------------------------  
      float llY_,llYGEN_;
      // ---- dilepton pt -----------------------------------------------
      float llPt_,llPtGEN_;
      // ---- dilepton phi ----------------------------------------------
      float llPhi_,llPhiGEN_;
      // ---- dphi between the two leptons ------------------------------
      float llDPhi_,llDPhiGEN_; 
      // ---- dphi between jets and dilepton ----------------------------
      vector<float> *jetllDPhi_,*jetllDPhiGEN_;
      // ---- lepton kinematics -----------------------------------------
      vector<float> *lepPt_,*lepEta_,*lepPhi_,*lepE_,*lepPtGEN_,*lepEtaGEN_,*lepPhiGEN_,*lepEGEN_;
      // ---- lepton properties ----------------------------------------- 
      vector<int>   *lepChId_,*lepId_,*lepChIdGEN_;
      vector<float> *lepIso_,*lepIsoPF_;
      // ---- number of leptons -----------------------------------------
      int nLeptons_,nLeptonsGEN_;
      // ---- jet kinematics --------------------------------------------
      vector<float> *jetPt_,*jetEta_,*jetY_,*jetPhi_,*jetE_,*jetPtGEN_,*jetEtaGEN_,*jetYGEN_,*jetPhiGEN_,*jetEGEN_;
      // ---- jet composition fractions ---------------------------------
      vector<float> *jetCHF_,*jetPHF_,*jetNHF_,*jetMUF_,*jetELF_;
      // ---- other jet properties --------------------------------------
      vector<float> *jetBeta_,*jetBetaStar_,*jetArea_,*jetJEC_,*jetUNC_;
      // ---- tight jet id ----------------------------------------------
      vector<int>   *jetId_; 
      // ---- number of jets --------------------------------------------
      int nJets_,nJetsGEN_;
      // ---- flag to determine if the Z is one of the 2 leading objects-
      int isZlead_,isZleadGEN_;
      // ---- HT of the two leading objects -----------------------------
      float htLead_,htLeadGEN_;
      // ---- jet pt geometric mean -------------------------------------
      float jetPtGeMean_,jetPtGeMeanGEN_;
      // ---- jet pt arithmetic mean ------------------------------------
      float jetPtArMean_,jetPtArMeanGEN_;
      // ---- dphi between jets -----------------------------------------
      float j1j2DPhi_,j1j3DPhi_,j2j3DPhi_,j1j2DPhiGEN_,j1j3DPhiGEN_,j2j3DPhiGEN_;
      // ---- dR between jets -------------------------------------------
      float j1j2DR_,j1j3DR_,j2j3DR_,j1j2DRGEN_,j1j3DRGEN_,j2j3DRGEN_; 
      // ---- pf met ----------------------------------------------------
      float pfmet_;
      // ---- pf sumEt --------------------------------------------------
      float pfSumEt_;
      // ---- pf met phi ------------------------------------------------
      float pfmetPhi_;
      // ---- pt of the hadronic recoil ---------------------------------
      float pfhadPt_; 
      // ---- invariant mass of the Z and the leading jet --------------- 
      float mZj1_,mZj1GEN_;
      // ---- invariant mass of the two leading jets --------------------
      float mj1j2_,mj1j2GEN_;
      // ---- invariant mas of all leptons ------------------------------
      float mLep_,mLepGEN_;
      // ---- pf pt density ---------------------------------------------
      float rho_;
      // ---- reconstructed vertices' prperties -------------------------
      vector<float> *vtxZ_,*vtxNdof_;
      // ---- number of good reconstructed vertices ---------------------
      int   nVtx_;
      // ---- number of simulated pu interactions -----------------------
      int   pu_,puINT_,puOOT_;
      // ---- RECO, GEN accepted flags for MC ---------------------------
      int selRECO_,selGEN_;
};
//
// class omplemetation
//
// ---- constructor -----------------------------------------------------
ZJetsExpress::ZJetsExpress(const ParameterSet& iConfig)
{
  mIsMC        = iConfig.getParameter<bool>          ("isMC");
  mMinNjets    = iConfig.getParameter<int>           ("minNjets");   
  mJetLepIsoR  = iConfig.getParameter<double>        ("jetLepIsoRadius");
  mMinJetPt    = iConfig.getParameter<double>        ("minJetPt");
  mMaxJetEta   = iConfig.getParameter<double>        ("maxJetEta");
  mMinLepPt    = iConfig.getParameter<double>        ("minLepPt");
  mMaxLepEta   = iConfig.getParameter<double>        ("maxLepEta");
  mJetsName    = iConfig.getParameter<edm::InputTag> ("jets");
  mSrcRho      = iConfig.getParameter<edm::InputTag> ("srcRho");
  mJECservice  = iConfig.getParameter<std::string>   ("jecService");
  mPayloadName = iConfig.getParameter<std::string>   ("payload");
}
// ---- destructor ------------------------------------------------------
ZJetsExpress::~ZJetsExpress()
{
}
// ---- method called once each job just before starting event loop -----
void ZJetsExpress::beginJob()
{
  // ---- create the objects --------------------------------------------
  hRecoLeptons_ = fTFileService->make<TH1I>("RecoLeptons", "RecoLeptons",6,-1,5);
  if (mIsMC) {
    hGenLeptons_ = fTFileService->make<TH1I>("GenLeptons", "GenLeptons",6,-1,5);
  }
  mcPU_         = fTFileService->make<TH1D>("mcPU", "mcPU",40,0,40);
  myTree_       = fTFileService->make<TTree>("events", "events");
  // ---- build the tree ------------------------------------------------
  buildTree();
  // ---- set the jec uncertainty flag ----------------------------------
  mIsJECuncSet = false;
}
// ---- event loop ------------------------------------------------------
void ZJetsExpress::analyze(const Event& iEvent, const EventSetup& iSetup)
{
  // ---- event counter -------------------------------------------------
  hRecoLeptons_->Fill(-1);  
  // ---- initialize the tree branches ----------------------------------
  clearTree();
  isRealData_ = iEvent.isRealData() ? 1:0;
  // ----  MC truth block -----------------------------------------------
  vector<GENLEPTON>      myGenLeptons;
  vector<TLorentzVector> myGenJets;  
  if (mIsMC) {
    hGenLeptons_->Fill(-1);
    // ---- PU ----------------------------------------------------------
    Handle<vector<PileupSummaryInfo> > pileupInfo;
    iEvent.getByLabel("addPileupInfo", pileupInfo);
    vector<PileupSummaryInfo>::const_iterator PUI;
    puOOT_ = 0;
    puINT_ = 0;
    for(PUI = pileupInfo->begin(); PUI != pileupInfo->end(); ++PUI) {
      if (PUI->getBunchCrossing() == 0)
        puINT_ += PUI->getPU_NumInteractions();     
      else
        puOOT_ += PUI->getPU_NumInteractions();
    }// PUI loop
    pu_ = puINT_+puOOT_;
    mcPU_->Fill(pu_);
    Handle<GenJetCollection> genjets;
    iEvent.getByLabel("ak5GenJets",genjets);
    Handle<GenParticleCollection> gen;
    iEvent.getByLabel("genParticles", gen);
    GenParticleCollection::const_iterator i_gen;
    GenJetCollection::const_iterator i_genjet;
    // ---- loop over the gen particles ---------------------------------
    for(i_gen = gen->begin(); i_gen != gen->end(); i_gen++) {
      // ---- consider only final state particles -----------------------
      if (i_gen->status() == 1) {   
        // ---- consider only electron and muon flavors -----------------
        if (abs(i_gen->pdgId()) == 11 || abs(i_gen->pdgId()) == 13) {
           // ---- apply geometric and kinematic acceptance -------------
          if ((i_gen->pt() > mMinLepPt) && (fabs(i_gen->eta())) < mMaxLepEta) {
            GENLEPTON aGenLepton;
            TLorentzVector lepP4GEN(i_gen->p4().Px(),i_gen->p4().Py(),i_gen->p4().Pz(),i_gen->p4().E());
            aGenLepton.p4    = lepP4GEN; 
            aGenLepton.pdgId = i_gen->pdgId();
            myGenLeptons.push_back(aGenLepton);
          }
        }
      }
    }
    hGenLeptons_->Fill(int(myGenLeptons.size()));
    // ---- sort the genLeptons -----------------------------------------
    sort(myGenLeptons.begin(),myGenLeptons.end(),lepSortingRuleGEN);
    // ---- genjets -----------------------------------------------------
    for(i_genjet = genjets->begin(); i_genjet != genjets->end(); i_genjet++) {
      // ---- genlepton - genjet cross cleaning -------------------------
      bool isISO(true);
      for(unsigned l=0;l<myGenLeptons.size();l++) { 
        // ---- genjet vs 2 leading genlepton cleaning ------------------
        if (l >= 2) continue; 
        if (deltaR(i_genjet->eta(),i_genjet->phi(),myGenLeptons[l].p4.Eta(),myGenLeptons[l].p4.Phi()) < mJetLepIsoR) {
          isISO = false;
          continue;
        }
      }
      if (!isISO) continue;
      // ---- preselection on genjets -----------------------------------
      if ((i_genjet->pt() < mMinJetPt) || (fabs(i_genjet->eta()) > mMaxJetEta)) continue;
      TLorentzVector aGenJet(i_genjet->p4().Px(),i_genjet->p4().Py(),i_genjet->p4().Pz(),i_genjet->p4().E());
      myGenJets.push_back(aGenJet);  
    }// genjet loop
  }// if MC
  //---- Rho ------------------------------------------------------------
  Handle<double> rho;
  iEvent.getByLabel(mSrcRho,rho);
  //---- reco vertices block --------------------------------------------
  edm::Handle<VertexCollection> vertices_;
  iEvent.getByLabel("offlinePrimaryVertices", vertices_);
  const reco::Vertex *primVtx = &(*(vertices_.product()))[0];
  for(VertexCollection::const_iterator i_vtx = vertices_->begin(); i_vtx != vertices_->end(); ++i_vtx) {  
    if (!i_vtx->isFake() && (fabs(i_vtx->z()) < 24) && (i_vtx->ndof() >= 4)) {
      vtxZ_   ->push_back(i_vtx->z());
      vtxNdof_->push_back(i_vtx->ndof());
    }
  }
  //---- leptons block --------------------------------------------------
  Handle<View<GsfElectron> > electrons_;
  iEvent.getByLabel("gsfElectrons", electrons_);
  Handle<View<Muon> > muons_;
  iEvent.getByLabel("muons",muons_);
  vector<LEPTON> myLeptons;
  // ---- loop over muons -----------------------------------------------
  for(View<Muon>::const_iterator i_mu = muons_->begin(); i_mu != muons_->end(); ++i_mu) {
    if (!(i_mu->isGlobalMuon()))                                       continue;
    if (!(i_mu->isTrackerMuon()))                                      continue;
    if (!muon::isGoodMuon(*i_mu,muon::GlobalMuonPromptTight))          continue;
    if ((i_mu->pt() < mMinLepPt) || (fabs(i_mu->eta()) > mMaxLepEta))  continue;
    if (fabs(i_mu->globalTrack()->normalizedChi2()) > 10)              continue;
    if (fabs(i_mu->innerTrack()->dz(primVtx->position())) > 1.0)       continue;
    if (fabs(i_mu->innerTrack()->dxy(primVtx->position())) > 0.04)     continue;
    float muonIso = (i_mu->isolationR03().sumPt + i_mu->isolationR03().emEt + i_mu->isolationR03().hadEt)/i_mu->pt();
    if (muonIso > 0.15)                                                continue;
    if (i_mu->innerTrack()->hitPattern().numberOfValidHits() < 11)     continue;
    if (i_mu->innerTrack()->hitPattern().numberOfValidPixelHits() < 1) continue;
    if (i_mu->numberOfMatches() < 2)                                   continue;
    if (i_mu->innerTrack()->ptError()/i_mu->pt() > 0.1)                continue;
      
    LEPTON aLepton;
    TLorentzVector lepP4(i_mu->p4().Px(),i_mu->p4().Py(),i_mu->p4().Pz(),i_mu->p4().E());
    aLepton.p4   = lepP4;
    aLepton.chid = 2*i_mu->charge();
    aLepton.id   = 1;
    aLepton.iso  = muonIso;
    aLepton.isoPF = -999;
    myLeptons.push_back(aLepton);
  }// muon loop
  // ---- loop over electrons -------------------------------------------
  for(View<GsfElectron>::const_iterator i_el = electrons_->begin();i_el != electrons_->end(); ++i_el) {
    float elPt                           = i_el->p4().Pt();
    float elEta                          = i_el->p4().Eta();
    float trackIsoRel                    = i_el->dr03TkSumPt()/i_el->p4().Pt();
    float ecalIsoRel                     = i_el->dr03EcalRecHitSumEt()/i_el->p4().Pt();
    float hcalIsoRel                     = i_el->dr03HcalTowerSumEt()/i_el->p4().Pt();
    float sigmaIetaIeta                  = i_el->sigmaIetaIeta();
    float hadronicOverEm                 = i_el->hadronicOverEm();
    float deltaPhiSuperClusterTrackAtVtx = i_el->deltaPhiSuperClusterTrackAtVtx();
    float deltaEtaSuperClusterTrackAtVtx = i_el->deltaEtaSuperClusterTrackAtVtx();
    bool  isTight(false);
    float combinedIso03 = (i_el->dr03TkSumPt()+max(0.,i_el->dr03EcalRecHitSumEt()-1.)+i_el->dr03HcalTowerSumEt())/i_el->p4().Pt();
    if ((elPt < mMinLepPt) || (fabs(elEta) > mMaxLepEta)) continue;
    if (i_el->isEB()) {
      if (combinedIso03 > 0.15)                           continue;          
      if (trackIsoRel > 0.15)                             continue;
      if (ecalIsoRel > 2.00)                              continue; 
      if (hcalIsoRel > 0.12)                              continue;
      if (sigmaIetaIeta > 0.01)                           continue;
      if (deltaPhiSuperClusterTrackAtVtx > 0.8)           continue;
      if (deltaEtaSuperClusterTrackAtVtx > 0.007)         continue;
      if (hadronicOverEm > 0.15)                          continue;
      if (sigmaIetaIeta < 0.01) {
	if (deltaPhiSuperClusterTrackAtVtx < 0.06) 
	  if (deltaEtaSuperClusterTrackAtVtx < 0.004) 
	    if (hadronicOverEm < 0.04) 
              isTight = true;
      }
    }// if EB
    if (i_el->isEE()) {
      if (combinedIso03 > 0.1)                            continue;
      if (trackIsoRel > 0.08)                             continue;
      if (ecalIsoRel > 0.06)                              continue;
      if (hcalIsoRel > 0.05)                              continue;
      if (sigmaIetaIeta > 0.03)                           continue;
      if (deltaPhiSuperClusterTrackAtVtx > 0.7)           continue;
      if (deltaEtaSuperClusterTrackAtVtx > 0.01)          continue;
      if (hadronicOverEm > 0.15)                          continue;
      if (sigmaIetaIeta<0.03) {
	if (deltaPhiSuperClusterTrackAtVtx < 0.03)
	  if (deltaEtaSuperClusterTrackAtVtx < 0.007)
	    if (hadronicOverEm < 0.15) 
              isTight = true;
      } 
    }// if EE
    LEPTON aLepton;
    TLorentzVector lepP4(i_el->p4().Px(),i_el->p4().Py(),i_el->p4().Pz(),i_el->p4().E());
    aLepton.p4   = lepP4;
    aLepton.chid = i_el->charge();
    aLepton.id   = 0;
    if (isTight) {
      aLepton.id = 1;
    }
    aLepton.iso   = combinedIso03;
    aLepton.isoPF = -999;
    myLeptons.push_back(aLepton);
  } // electrons loop
  hRecoLeptons_->Fill(int(myLeptons.size()));
  // ---- sort the leptons according to their pt ------------------------
  sort(myLeptons.begin(),myLeptons.end(),lepSortingRule); 
  // ---- PF isolation for leptons --------------------------------------
  Handle<View<PFCandidate> > pfCandidates;
  iEvent.getByLabel("particleFlow", pfCandidates);
  for(unsigned il=0;il<myLeptons.size();il++) {
    float sumPt(0.0);
    for(unsigned ipf=0;ipf<pfCandidates->size();ipf++) {
      float dR = deltaR(myLeptons[il].p4.Eta(),myLeptons[il].p4.Phi(),(*pfCandidates)[ipf].eta(),(*pfCandidates)[ipf].phi());
      if (dR < 0.3) {
        sumPt += (*pfCandidates)[ipf].pt();
      }
    }
    float isoPF = (sumPt-myLeptons[il].p4.Pt()-(*rho)*0.2827434)/myLeptons[il].p4.Pt();
    myLeptons[il].isoPF = isoPF;
  }
  //---- jets block -----------------------------------------------------
  Handle<PFJetCollection> jets_;
  iEvent.getByLabel(mJetsName,jets_);
  vector<JET> myJets;
  //---- get the jet energy corrector -----------------------------------
  mJEC = JetCorrector::getJetCorrector(mJECservice,iSetup);
  //---- the JEC uncertainty only needs to be set-up once ---------------
  if (!mIsJECuncSet) {
    edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
    iSetup.get<JetCorrectionsRecord>().get(mPayloadName,JetCorParColl); 
    JetCorrectorParameters const& JetCorPar = (*JetCorParColl)["Uncertainty"];
    mJECunc = new JetCorrectionUncertainty(JetCorPar);
    mIsJECuncSet = true;
  }
  // ---- jets loop -----------------------------------------------------
  for(PFJetCollection::const_iterator i_jet = jets_->begin(); i_jet != jets_->end(); ++i_jet) {
    TLorentzVector jetP4(i_jet->px(),i_jet->py(),i_jet->pz(),i_jet->energy());
    bool jetIsDuplicate(false);
    //----- remove the leptons ------------------------------------------
    for(unsigned int i_lep = 0; i_lep < myLeptons.size(); i_lep++) {
      float DR = myLeptons[i_lep].p4.DeltaR(jetP4);
      if (DR < mJetLepIsoR) {
        jetIsDuplicate = true; 
      }
    }// lepton loop 
    if (jetIsDuplicate) continue; 
    // ---- get the jec and the uncertainty -----------------------------    
    int index = i_jet - jets_->begin();
    edm::RefToBase<reco::Jet> jetRef(edm::Ref<PFJetCollection>(jets_,index));
    double jec = mJEC->correction(*i_jet,jetRef,iEvent,iSetup);
    // ---- only keep jets within the kinematic acceptance --------------
    if ((jec * jetP4.Pt() < mMinJetPt) || (fabs(jetP4.Eta()) > mMaxJetEta)) continue;
    mJECunc->setJetEta(i_jet->eta());
    // ---- the unc is a function of the corrected pt -------------------
    mJECunc->setJetPt(jec * i_jet->pt());
    double unc = mJECunc->getUncertainty(true);
    // ---- keep only jets that pass the tight id -----------------------
    int   chm = i_jet->chargedHadronMultiplicity();
    int   npr = i_jet->chargedMultiplicity() + i_jet->neutralMultiplicity();
    float nhf = (i_jet->neutralHadronEnergy() + i_jet->HFHadronEnergy())/i_jet->energy();
    float phf = i_jet->photonEnergyFraction();
    float chf = i_jet->chargedHadronEnergyFraction();
    float elf = i_jet->electronEnergyFraction(); 
    bool id = (npr>1 && phf<0.99 && nhf<0.99 && ((fabs(i_jet->eta())<=2.4 && nhf<0.9 && phf<0.9 && elf<0.99 && chf>0 && chm>0) || fabs(i_jet->eta())>2.4));
    if (!id) continue;
    // ---- jet vertex association --------------------------------------
    // ---- get the vector of tracks ------------------------------------ 
    reco::TrackRefVector vTrks(i_jet->getTrackRefs());
    float sumTrkPt(0.0),sumTrkPtBeta(0.0),sumTrkPtBetaStar(0.0),beta(-1.0),betaStar(-1.0);
    // ---- loop over the tracks of the jet -----------------------------
    for(reco::TrackRefVector::const_iterator i_trk = vTrks.begin(); i_trk != vTrks.end(); i_trk++) {
      sumTrkPt += (*i_trk)->pt();
      // ---- loop over all vertices ------------------------------------
      for(unsigned i_vtx = 0;i_vtx < vertices_->size();i_vtx++) {
        // ---- loop over the tracks associated with the vertex ---------
        if ((*vertices_)[i_vtx].isFake() || (*vertices_)[i_vtx].ndof() < 4) continue; 
        for(reco::Vertex::trackRef_iterator i_vtxTrk = (*vertices_)[i_vtx].tracks_begin(); i_vtxTrk != (*vertices_)[i_vtx].tracks_end(); ++i_vtxTrk) {
          // ---- match the jet track to the track from the vertex ------
          reco::TrackRef trkRef(i_vtxTrk->castTo<reco::TrackRef>());
          // ---- check if the tracks match -----------------------------
          if (trkRef == (*i_trk)) {
            if (i_vtx == 0) {
              sumTrkPtBeta += (*i_trk)->pt();
            }
            else {
              sumTrkPtBetaStar += (*i_trk)->pt();
            }   
            continue;
          }
        }
      }// vertices loop 
    }// jet tracks loop
    if (sumTrkPt > 0) {
      beta     = sumTrkPtBeta/sumTrkPt;
      betaStar = sumTrkPtBetaStar/sumTrkPt;
    }
    JET aJet; 
    aJet.p4       = jec * jetP4;
    aJet.jec      = jec;
    aJet.unc      = unc;
    aJet.area     = i_jet->jetArea();
    aJet.chf      = chf;
    aJet.nhf      = nhf;
    aJet.phf      = phf;
    aJet.elf      = elf;
    aJet.muf      = i_jet->muonEnergyFraction();
    aJet.id       = 0;
    if (id) {
      aJet.id     = 1;
    }
    aJet.beta     = beta;
    aJet.betaStar = betaStar;
    myJets.push_back(aJet);  
  }// jet loop
  // ---- sort jets according to their corrected pt ---------------------
  sort(myJets.begin(),myJets.end(),jetSortingRule);    
  // ---- MET block -----------------------------------------------------
  Handle<View<PFMET> > pfmetCol_;
  iEvent.getByLabel("pfMet", pfmetCol_);
  float pfmetPx = (pfmetCol_->front()).px();
  float pfmetPy = (pfmetCol_->front()).py();
  // ---- counters ------------------------------------------------------
  nVtx_        = int(vtxZ_->size());
  nLeptons_    = int(myLeptons.size()); 
  nJets_       = int(myJets.size());
  nLeptonsGEN_ = int(myGenLeptons.size()); 
  nJetsGEN_    = int(myGenJets.size()); 
  // ---- keep only selected events -------------------------------------
  bool selectionRECO = ((nVtx_ > 0) && (nLeptons_ > 1) && (nJets_ >= mMinNjets));
  bool selection(selectionRECO);
  bool selectionGEN(false);
  selRECO_ = 0;
  if (mIsMC) {
    selectionGEN = ((nLeptonsGEN_ > 1) && (nJetsGEN_ >= mMinNjets)); 
    selection +=  selectionGEN;
    selGEN_ = 0;
  }
  if (selection) {
    eventNum_   = iEvent.id().event();
    runNum_     = iEvent.id().run();
    lumi_       = iEvent.luminosityBlock();
    isRealData_ = iEvent.isRealData() ? 1:0;
    rho_        = *rho; 
    pfmet_      = (pfmetCol_->front()).pt();
    pfmetPhi_   = (pfmetCol_->front()).phi();
    pfSumEt_    = (pfmetCol_->front()).sumEt();
    if (selectionRECO) {
      selRECO_ = 1;
      TLorentzVector llP4 = myLeptons[0].p4 + myLeptons[1].p4;
      // ---- hadronic recoil vector --------------------------------------
      TLorentzVector pfmetP4(pfmetPx,pfmetPy,0,sqrt(pfmetPx * pfmetPx + pfmetPy * pfmetPy));
      TLorentzVector pfhadP4 = -pfmetP4 - llP4;      
      llM_        = llP4.M();
      llPt_       = llP4.Pt();
      llPhi_      = llP4.Phi();
      llY_        = llP4.Rapidity();
      llDPhi_     = fabs(myLeptons[0].p4.DeltaPhi(myLeptons[1].p4));
      TLorentzVector lepP4(0,0,0,0); 
      for(unsigned l = 0; l < myLeptons.size(); l++) {
        lepP4 += myLeptons[l].p4;
        lepPt_     ->push_back(myLeptons[l].p4.Pt());
        lepEta_    ->push_back(myLeptons[l].p4.Eta());
        lepPhi_    ->push_back(myLeptons[l].p4.Phi());
        lepE_      ->push_back(myLeptons[l].p4.Energy());
        lepIso_    ->push_back(myLeptons[l].iso);
        lepIsoPF_  ->push_back(myLeptons[l].isoPF);
        lepId_     ->push_back(myLeptons[l].id);
        lepChId_   ->push_back(myLeptons[l].chid);
      }      
      pfhadPt_    = pfhadP4.Pt();
      mLep_       = lepP4.M(); 
      vector<TLorentzVector> allP4;
      allP4.push_back(llP4);
      double prod(1.0),sum(0.0);
      for(unsigned j = 0; j < myJets.size(); j++) {
        prod *= myJets[j].p4.Pt();
        sum  += myJets[j].p4.Pt();
        allP4.push_back(myJets[j].p4); 
        jetllDPhi_   ->push_back(fabs(llP4.DeltaPhi(myJets[j].p4)));
        jetPt_       ->push_back(myJets[j].p4.Pt()); 
        jetEta_      ->push_back(myJets[j].p4.Eta()); 
        jetPhi_      ->push_back(myJets[j].p4.Phi()); 
        jetE_        ->push_back(myJets[j].p4.Energy()); 
        jetY_        ->push_back(myJets[j].p4.Rapidity()); 
        jetArea_     ->push_back(myJets[j].area);
        jetBeta_     ->push_back(myJets[j].beta);
        jetBetaStar_ ->push_back(myJets[j].betaStar);
        jetJEC_      ->push_back(myJets[j].jec);
        jetUNC_      ->push_back(myJets[j].unc);
        jetCHF_      ->push_back(myJets[j].chf);
        jetPHF_      ->push_back(myJets[j].phf);
        jetNHF_      ->push_back(myJets[j].nhf);
        jetMUF_      ->push_back(myJets[j].muf);
        jetELF_      ->push_back(myJets[j].elf);
        jetId_       ->push_back(myJets[j].id);  
      }
      isZlead_ = 1;
      if (nJets_ > 0) {
        mZj1_        = (llP4+myJets[0].p4).M();
        jetPtGeMean_ = pow(prod,1./nJets_);
        jetPtArMean_ = sum/nJets_;
        if (nJets_ > 1) {
          mj1j2_    = (myJets[0].p4+myJets[1].p4).M();
          j1j2DPhi_ = fabs(myJets[0].p4.DeltaPhi(myJets[1].p4));
          j1j2DR_   = myJets[0].p4.DeltaR(myJets[1].p4);
          if (llPt_ < myJets[1].p4.Pt()) {
            isZlead_ = 0;
          }
          if (nJets_ > 2) {
            j1j3DPhi_ = fabs(myJets[0].p4.DeltaPhi(myJets[2].p4));
            j2j3DPhi_ = fabs(myJets[1].p4.DeltaPhi(myJets[2].p4));
            j1j3DR_   = myJets[0].p4.DeltaR(myJets[2].p4);
            j2j3DR_   = myJets[1].p4.DeltaR(myJets[2].p4);
          }  
        }
      }
      sort(allP4.begin(),allP4.end(),p4SortingRule);
      htLead_ = 0.0;
      for(int i=0;i<TMath::Min(int(allP4.size()),2);i++) {
        htLead_ += allP4[i].Pt();
      }
    }// if selection RECO 
    if (selectionGEN) {
      selGEN_ = 1;
      TLorentzVector llP4GEN = myGenLeptons[0].p4 + myGenLeptons[1].p4;  
      llMGEN_        = llP4GEN.M();
      llPtGEN_       = llP4GEN.Pt();
      llPhiGEN_      = llP4GEN.Phi();
      llYGEN_        = llP4GEN.Rapidity();
      llDPhiGEN_     = fabs(myGenLeptons[0].p4.DeltaPhi(myGenLeptons[1].p4));
      TLorentzVector lepP4GEN(0,0,0,0); 
      for(unsigned l = 0; l < myGenLeptons.size(); l++) {
        lepP4GEN += myGenLeptons[l].p4;
        lepPtGEN_     ->push_back(myGenLeptons[l].p4.Pt());
        lepEtaGEN_    ->push_back(myGenLeptons[l].p4.Eta());
        lepPhiGEN_    ->push_back(myGenLeptons[l].p4.Phi());
        lepEGEN_      ->push_back(myGenLeptons[l].p4.Energy());
        lepChIdGEN_   ->push_back(myGenLeptons[l].pdgId);
      }   
      mLepGEN_ = lepP4GEN.M(); 
      vector<TLorentzVector> allP4GEN;
      allP4GEN.push_back(llP4GEN);
      double prod(1.0),sum(0.0);
      for(unsigned j = 0; j < myGenJets.size(); j++) {
        prod *= myGenJets[j].Pt();
        sum  += myGenJets[j].Pt();
        allP4GEN.push_back(myGenJets[j]); 
        jetllDPhiGEN_   ->push_back(fabs(llP4GEN.DeltaPhi(myGenJets[j])));
        jetPtGEN_       ->push_back(myGenJets[j].Pt()); 
        jetEtaGEN_      ->push_back(myGenJets[j].Eta()); 
        jetPhiGEN_      ->push_back(myGenJets[j].Phi()); 
        jetEGEN_        ->push_back(myGenJets[j].Energy()); 
        jetYGEN_        ->push_back(myGenJets[j].Rapidity());  
      }
      isZleadGEN_ = 1;
      if (nJetsGEN_ > 0) {
        mZj1GEN_        = (llP4GEN + myGenJets[0]).M();
        jetPtGeMeanGEN_ = pow(prod,1./nJetsGEN_);
        jetPtArMeanGEN_ = sum/nJetsGEN_;
        if (nJetsGEN_ > 1) {
          mj1j2GEN_    = (myGenJets[0] + myGenJets[1]).M();
          j1j2DPhiGEN_ = fabs(myGenJets[0].DeltaPhi(myGenJets[1]));
          j1j2DRGEN_   = myGenJets[0].DeltaR(myGenJets[1]);
          if (llPtGEN_ < myGenJets[1].Pt()) {
            isZleadGEN_ = 0;
          }
          if (nJetsGEN_ > 2) {
            j1j3DPhiGEN_ = fabs(myGenJets[0].DeltaPhi(myGenJets[2]));
            j2j3DPhiGEN_ = fabs(myGenJets[1].DeltaPhi(myGenJets[2]));
            j1j3DRGEN_   = myGenJets[0].DeltaR(myGenJets[2]);
            j2j3DRGEN_   = myGenJets[1].DeltaR(myGenJets[2]);
          }  
        }
      }
      sort(allP4GEN.begin(),allP4GEN.end(),p4SortingRule);
      htLeadGEN_ = 0.0;
      for(int i=0;i<TMath::Min(int(allP4GEN.size()),2);i++) {
        htLeadGEN_ += allP4GEN[i].Pt();
      }
    }// if selection GEN 
    myTree_->Fill();
  }// if selectionRECO || selectionGEN
}
// ---- method called once each job just after ending the event loop  ---
void ZJetsExpress::endJob() 
{
}
// ---- method for tree building ----------------------------------------
void ZJetsExpress::buildTree()
{
  lepPt_             = new std::vector<float>();
  lepEta_            = new std::vector<float>();
  lepPhi_            = new std::vector<float>();
  lepE_              = new std::vector<float>(); 
  lepIso_            = new std::vector<float>();
  lepIsoPF_          = new std::vector<float>();
  lepChId_           = new std::vector<int>();
  lepId_             = new std::vector<int>();
  jetPt_             = new std::vector<float>(); 
  jetEta_            = new std::vector<float>();
  jetPhi_            = new std::vector<float>();
  jetE_              = new std::vector<float>();
  jetY_              = new std::vector<float>();
  jetArea_           = new std::vector<float>();
  jetBeta_           = new std::vector<float>();
  jetBetaStar_       = new std::vector<float>();
  jetJEC_            = new std::vector<float>();
  jetUNC_            = new std::vector<float>();
  jetllDPhi_         = new std::vector<float>();
  jetCHF_            = new std::vector<float>();
  jetPHF_            = new std::vector<float>();
  jetNHF_            = new std::vector<float>();
  jetMUF_            = new std::vector<float>();
  jetELF_            = new std::vector<float>();
  jetId_             = new std::vector<int>();
  vtxZ_              = new std::vector<float>();
  vtxNdof_           = new std::vector<float>();
  if (mIsMC) { 
    lepPtGEN_        = new std::vector<float>();
    lepEtaGEN_       = new std::vector<float>();
    lepPhiGEN_       = new std::vector<float>();
    lepEGEN_         = new std::vector<float>(); 
    lepChIdGEN_      = new std::vector<int>();
    jetPtGEN_        = new std::vector<float>(); 
    jetEtaGEN_       = new std::vector<float>();
    jetPhiGEN_       = new std::vector<float>();
    jetEGEN_         = new std::vector<float>();
    jetYGEN_         = new std::vector<float>();
    jetllDPhiGEN_    = new std::vector<float>();
  }
  // ---- global event variables ----------------------------------------
  myTree_->Branch("isRealData"       ,&isRealData_        ,"isRealData/I");
  myTree_->Branch("selRECO"          ,&selRECO_           ,"selRECO/I");
  myTree_->Branch("eventNum"         ,&eventNum_          ,"eventNum/I");
  myTree_->Branch("runNum"           ,&runNum_            ,"runNum/I");
  myTree_->Branch("lumi"             ,&lumi_              ,"lumi/I");
  myTree_->Branch("nVtx"             ,&nVtx_              ,"nVtx/I");
  myTree_->Branch("nLeptons"         ,&nLeptons_          ,"nLeptons/I");
  myTree_->Branch("nJets"            ,&nJets_             ,"nJets/I"); 
  myTree_->Branch("isZlead"          ,&isZlead_           ,"isZlead/I");
  myTree_->Branch("rho"              ,&rho_               ,"rho/F");
  myTree_->Branch("mZj1"             ,&mZj1_              ,"mZj1/F");
  myTree_->Branch("mj1j2"            ,&mj1j2_             ,"mj1j2/F");
  myTree_->Branch("mLep"             ,&mLep_              ,"mLep/F");
  myTree_->Branch("htLead"           ,&htLead_            ,"htLead/F");
  myTree_->Branch("j1j2DPhi"         ,&j1j2DPhi_          ,"j1j2DPhi/F");
  myTree_->Branch("j1j3DPhi"         ,&j1j3DPhi_          ,"j1j3DPhi/F");
  myTree_->Branch("j2j3DPhi"         ,&j2j3DPhi_          ,"j2j3DPhi/F");
  myTree_->Branch("j1j2DR"           ,&j1j2DR_            ,"j1j2DR/F");
  myTree_->Branch("j1j3DR"           ,&j1j3DR_            ,"j1j3DR/F");
  myTree_->Branch("j2j3DR"           ,&j2j3DR_            ,"j2j3DR/F");
  myTree_->Branch("jetPtGeMean"      ,&jetPtGeMean_       ,"jetPtGeMean/F");
  myTree_->Branch("jetPtArMean"      ,&jetPtArMean_       ,"jetPtArMean/F");
  // ---- met variables -------------------------------------------------
  myTree_->Branch("pfmet"            ,&pfmet_             ,"pfmet/F");
  myTree_->Branch("pfmetPhi"         ,&pfmetPhi_          ,"pfmetPhi/F");
  myTree_->Branch("pfhadPt"          ,&pfhadPt_           ,"pfhadPt_/F");
  myTree_->Branch("pfSumEt"          ,&pfSumEt_           ,"pfSumEt/F");  
  // ---- dilepton variables --------------------------------------------
  myTree_->Branch("llM"              ,&llM_               ,"llM/F");
  myTree_->Branch("llPt"             ,&llPt_              ,"llPt/F");
  myTree_->Branch("llPhi"            ,&llPhi_             ,"llPhi/F");
  myTree_->Branch("llDPhi"           ,&llDPhi_            ,"llDPhi/F");
  myTree_->Branch("llY"              ,&llY_               ,"llY/F");
  // ---- lepton variables ----------------------------------------------
  myTree_->Branch("lepPt"            ,"vector<float>"     ,&lepPt_);
  myTree_->Branch("lepEta"           ,"vector<float>"     ,&lepEta_);
  myTree_->Branch("lepPhi"           ,"vector<float>"     ,&lepPhi_);
  myTree_->Branch("lepE"             ,"vector<float>"     ,&lepE_);
  myTree_->Branch("lepIso"           ,"vector<float>"     ,&lepIso_);
  myTree_->Branch("lepIsoPF"         ,"vector<float>"     ,&lepIsoPF_);
  myTree_->Branch("lepChId"          ,"vector<int>"       ,&lepChId_);
  myTree_->Branch("lepId"            ,"vector<int>"       ,&lepId_);
  // ---- jet variables -------------------------------------------------
  myTree_->Branch("jetPt"            ,"vector<float>"     ,&jetPt_);
  myTree_->Branch("jetEta"           ,"vector<float>"     ,&jetEta_);
  myTree_->Branch("jetPhi"           ,"vector<float>"     ,&jetPhi_);
  myTree_->Branch("jetE"             ,"vector<float>"     ,&jetE_);
  myTree_->Branch("jetY"             ,"vector<float>"     ,&jetY_);
  myTree_->Branch("jetArea"          ,"vector<float>"     ,&jetArea_);
  myTree_->Branch("jetBeta"          ,"vector<float>"     ,&jetBeta_);
  myTree_->Branch("jetBetaStar"      ,"vector<float>"     ,&jetBetaStar_);
  myTree_->Branch("jetJEC"           ,"vector<float>"     ,&jetJEC_);
  myTree_->Branch("jetUNC"           ,"vector<float>"     ,&jetUNC_);
  myTree_->Branch("jetllDPhi"        ,"vector<float>"     ,&jetllDPhi_);
  myTree_->Branch("jetCHF"           ,"vector<float>"     ,&jetCHF_);
  myTree_->Branch("jetPHF"           ,"vector<float>"     ,&jetPHF_);
  myTree_->Branch("jetNHF"           ,"vector<float>"     ,&jetNHF_);
  myTree_->Branch("jetMUF"           ,"vector<float>"     ,&jetMUF_);
  myTree_->Branch("jetELF"           ,"vector<float>"     ,&jetELF_);
  myTree_->Branch("jetId"            ,"vector<int>"       ,&jetId_);
  // ---- vertex variables ----------------------------------------------
  myTree_->Branch("vtxZ"             ,"vector<float>"     ,&vtxZ_);
  myTree_->Branch("vtxNdof"          ,"vector<float>"     ,&vtxNdof_);
  // ---- simulated pu variables ----------------------------------------
  if (mIsMC) { 
    myTree_->Branch("selGEN"           ,&selGEN_            ,"selGEN/I");
    myTree_->Branch("pu"               ,&pu_                ,"pu/I");
    myTree_->Branch("puINT"            ,&puINT_             ,"puINT/I");
    myTree_->Branch("puOOT"            ,&puOOT_             ,"puOOT/I");
    myTree_->Branch("nLeptonsGEN"      ,&nLeptonsGEN_       ,"nLeptonsGEN/I");
    myTree_->Branch("nJetsGEN"         ,&nJetsGEN_          ,"nJetsGEN/I");
    myTree_->Branch("isZleadGEN"       ,&isZleadGEN_        ,"isZleadGEN/I");
    myTree_->Branch("mZj1GEN"          ,&mZj1GEN_           ,"mZj1GEN/F");
    myTree_->Branch("mj1j2GEN"         ,&mj1j2GEN_          ,"mj1j2GEN/F");
    myTree_->Branch("mLepGEN"          ,&mLepGEN_           ,"mLepGEN/F");
    myTree_->Branch("htLeadGEN"        ,&htLeadGEN_         ,"htLeadGEN/F");
    myTree_->Branch("j1j2DPhiGEN"      ,&j1j2DPhiGEN_       ,"j1j2DPhiGEN/F");
    myTree_->Branch("j1j3DPhiGEN"      ,&j1j3DPhiGEN_       ,"j1j3DPhiGEN/F");
    myTree_->Branch("j2j3DPhiGEN"      ,&j2j3DPhiGEN_       ,"j2j3DPhiGEN/F");
    myTree_->Branch("j1j2DRGEN"        ,&j1j2DRGEN_         ,"j1j2DRGEN/F");
    myTree_->Branch("j1j3DRGEN"        ,&j1j3DRGEN_         ,"j1j3DRGEN/F");
    myTree_->Branch("j2j3DRGEN"        ,&j2j3DRGEN_         ,"j2j3DRGEN/F");
    myTree_->Branch("jetPtGeMeanGEN"   ,&jetPtGeMeanGEN_    ,"jetPtGeMeanGEN/F");
    myTree_->Branch("jetPtArMeanGEN"   ,&jetPtArMeanGEN_    ,"jetPtArMeanGEN/F");
    myTree_->Branch("llMGEN"           ,&llMGEN_            ,"llMGEN/F");
    myTree_->Branch("llPtGEN"          ,&llPtGEN_           ,"llPtGEN/F");
    myTree_->Branch("llPhiGEN"         ,&llPhiGEN_          ,"llPhiGEN/F");
    myTree_->Branch("llDPhiGEN"        ,&llDPhiGEN_         ,"llDPhiGEN/F");
    myTree_->Branch("llYGEN"           ,&llYGEN_            ,"llYGEN/F");
    myTree_->Branch("lepPtGEN"         ,"vector<float>"     ,&lepPtGEN_);
    myTree_->Branch("lepEtaGEN"        ,"vector<float>"     ,&lepEtaGEN_);
    myTree_->Branch("lepPhiGEN"        ,"vector<float>"     ,&lepPhiGEN_);
    myTree_->Branch("lepEGEN"          ,"vector<float>"     ,&lepEGEN_);
    myTree_->Branch("lepChIdGEN"       ,"vector<int>"       ,&lepChIdGEN_);
    myTree_->Branch("jetPtGEN"         ,"vector<float>"     ,&jetPtGEN_);
    myTree_->Branch("jetEtaGEN"        ,"vector<float>"     ,&jetEtaGEN_);
    myTree_->Branch("jetPhiGEN"        ,"vector<float>"     ,&jetPhiGEN_);
    myTree_->Branch("jetEGEN"          ,"vector<float>"     ,&jetEGEN_);
    myTree_->Branch("jetllDPhiGEN"     ,"vector<float>"     ,&jetllDPhiGEN_);
  }
}
// ---- method for tree initialization ----------------------------------
void ZJetsExpress::clearTree()
{
  selRECO_           = -999;
  isRealData_        = -999;
  eventNum_          = -999;
  runNum_            = -999;
  lumi_              = -999;
  nVtx_              = -999;
  nLeptons_          = -999;
  nJets_             = -999;
  isZlead_           = -999;
  rho_               = -999;
  pfmet_             = -999;
  pfmetPhi_          = -999;
  pfhadPt_           = -999;
  pfSumEt_           = -999;
  mZj1_              = -999; 
  mj1j2_             = -999;
  mLep_              = -999;
  htLead_            = -999;
  jetPtGeMean_       = -999;
  jetPtArMean_       = -999; 
  j1j2DPhi_          = -999;
  j1j3DPhi_          = -999; 
  j2j3DPhi_          = -999;
  j1j2DR_            = -999;
  j1j3DR_            = -999; 
  j2j3DR_            = -999;
  llM_               = -999;
  llPt_              = -999; 
  llPhi_             = -999;
  llDPhi_            = -999;
  llY_               = -999;
  lepPt_             ->clear();
  lepEta_            ->clear();
  lepPhi_            ->clear();
  lepE_              ->clear();
  lepIso_            ->clear();
  lepIsoPF_          ->clear();
  lepChId_           ->clear();
  lepId_             ->clear();
  jetPt_             ->clear();
  jetEta_            ->clear();
  jetPhi_            ->clear();
  jetE_              ->clear();
  jetY_              ->clear();
  jetArea_           ->clear();
  jetBeta_           ->clear();
  jetBetaStar_       ->clear();
  jetJEC_            ->clear();
  jetUNC_            ->clear();
  jetllDPhi_         ->clear();
  jetCHF_            ->clear();
  jetPHF_            ->clear();
  jetNHF_            ->clear();
  jetMUF_            ->clear();
  jetELF_            ->clear();
  jetId_             ->clear();
  vtxZ_              ->clear();
  vtxNdof_           ->clear();
  if (mIsMC) {
    selGEN_            = -999;
    pu_                = -999;
    puINT_             = -999;
    puOOT_             = -999;
    isRealData_        = -999;
    nLeptonsGEN_       = -999;
    nJetsGEN_          = -999;
    isZleadGEN_        = -999;
    mZj1GEN_           = -999; 
    mj1j2GEN_          = -999;
    mLepGEN_           = -999;
    htLeadGEN_         = -999;
    jetPtGeMeanGEN_    = -999;
    jetPtArMeanGEN_    = -999; 
    j1j2DPhiGEN_       = -999;
    j1j3DPhiGEN_       = -999; 
    j2j3DPhiGEN_       = -999;
    j1j2DRGEN_         = -999;
    j1j3DRGEN_         = -999; 
    j2j3DRGEN_         = -999;
    llMGEN_            = -999;
    llPtGEN_           = -999; 
    llPhiGEN_          = -999;
    llDPhiGEN_         = -999;
    llYGEN_            = -999;
    lepPtGEN_          ->clear();
    lepEtaGEN_         ->clear();
    lepPhiGEN_         ->clear();
    lepEGEN_           ->clear();
    lepChIdGEN_        ->clear();
    jetPtGEN_          ->clear();
    jetEtaGEN_         ->clear();
    jetPhiGEN_         ->clear();
    jetEGEN_           ->clear();
    jetYGEN_           ->clear();
    jetllDPhiGEN_      ->clear();
  }
}
// ---- define this as a plug-in ----------------------------------------
DEFINE_FWK_MODULE(ZJetsExpress);