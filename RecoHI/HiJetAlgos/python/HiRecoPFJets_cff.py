import FWCore.ParameterSet.Config as cms

## Default Parameter Sets
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *
from RecoHI.HiJetAlgos.HiPFJetParameters_cff import *

#pseudo towers for noise suppression background subtraction
PFTowers = cms.EDProducer("ParticleTowerProducer",
                          src = cms.InputTag("particleFlow"),
                          useHF = cms.bool(False)
                          )

#dummy sequence to speed-up reconstruction in pp_on_AA era
pfNoPileUpJMEHI = cms.EDFilter('GenericPFCandidateSelector',
                                src = cms.InputTag('particleFlow'),
                                cut = cms.string("pt>9999")
                                )

ak5PFJets = cms.EDProducer(
    "FastjetJetProducer",
    HiPFJetParameters,
    AnomalousCellParameters,
    MultipleAlgoIteratorBlock,
    jetAlgorithm = cms.string("AntiKt"),
    rParam       = cms.double(0.5)
    )
ak5PFJets.src = cms.InputTag('particleFlow')

akPu5PFJets = ak5PFJets.clone(
    jetType = cms.string('BasicJet'),
    doPVCorrection = False,
    doPUOffsetCorr = True,
    subtractorName = cms.string("MultipleAlgoIterator"),    
    src = cms.InputTag('PFTowers'),
    doAreaFastjet = False
    )



akPu5PFJets.puPtMin = cms.double(25)
akPu1PFJets = akPu5PFJets.clone(rParam       = cms.double(0.1), puPtMin = 10)
akPu2PFJets = akPu5PFJets.clone(rParam       = cms.double(0.2), puPtMin = 10)
akPu3PFJets = akPu5PFJets.clone(rParam       = cms.double(0.3), puPtMin = 15)
akPu4PFJets = akPu5PFJets.clone(rParam       = cms.double(0.4), puPtMin = 20)
akPu6PFJets = akPu5PFJets.clone(rParam       = cms.double(0.6), puPtMin = 30)
akPu7PFJets = akPu5PFJets.clone(rParam       = cms.double(0.7), puPtMin = 35)

ak4PFJetsForFlow = akPu5PFJets.clone(
    Ghost_EtaMax = cms.double(5.0),
    Rho_EtaMax = cms.double(4.4),
    doRhoFastjet = cms.bool(False),
    jetPtMin = cms.double(15.0),
    nSigmaPU = cms.double(1.0),
    rParam = cms.double(0.4),
    radiusPU = cms.double(0.5),
    src = cms.InputTag("pfcandCleaner", "particleFlowCleaned"),
)

kt4PFJetsForRho = cms.EDProducer(
    "FastjetJetProducer",
    HiPFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("Kt"),
    rParam       = cms.double(0.4)
)

kt4PFJetsForRho.src = cms.InputTag('particleFlow')
kt4PFJetsForRho.doAreaFastjet = cms.bool(True)
kt4PFJetsForRho.jetPtMin      = cms.double(0.0)
kt4PFJetsForRho.GhostArea     = cms.double(0.005)

akCs4PFJets = cms.EDProducer(
    "CSJetProducer",
    HiPFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm  = cms.string("AntiKt"),
    rParam        = cms.double(0.4),
    etaMap = cms.InputTag('hiPuRhoProducer', 'mapEtaEdges'),
    rho = cms.InputTag('hiPuRhoProducer', 'mapToRho'),
    rhom = cms.InputTag('hiPuRhoProducer', 'mapToRhoM'),
    csRParam  = cms.double(-1.),
    csAlpha   = cms.double(2.),
    writeJetsWithConst = cms.bool(True),
    useModulatedRho = cms.bool(True),
    rhoFlowFitParams = cms.InputTag('hiFJRhoFlowModulationProducer', 'rhoFlowFitParams'),
    jetCollInstanceName = cms.string("pfParticlesCs"),
)
akCs4PFJets.src           = cms.InputTag('particleFlow')
akCs4PFJets.doAreaFastjet = cms.bool(True)
akCs4PFJets.jetPtMin      = cms.double(0.0)
akCs4PFJets.useExplicitGhosts = cms.bool(True)
akCs4PFJets.GhostArea     = cms.double(0.005)

akCs3PFJets = akCs4PFJets.clone(rParam       = cms.double(0.3))
