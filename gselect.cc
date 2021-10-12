#include "cpu/pred/gselect.hh"

GSelectBP::GselectBP(const LocalBPParams *params)
    : BPredUnit(params),
      PredictorSize(params->localPredictorSize),
      PHTCtrBits(params->localCtrBits),
      localPredictorSets(localPredictorSize / localCtrBits),
      localCtrs(localPredictorSets, SatCounter(localCtrBits)),
      indexMask(localPredictorSets - 1)
         
void      
GSelectBP::lookup(ThreadID tid, Addr branchAddr, void * &bpHistory)
{


    bool taken;
    unsigned local_predictor_idx = getLocalIndex(branchAddr);
    
}

void
GSelectBP::update(ThreadID tid, Addr branchAddr, bool taken, void *bpHistory,
                 bool squashed, const StaticInstPtr & inst, Addr corrTarget)
{

}

void
GSelectBP::uncondBranch(ThreadID tid, Addr pc, void * &bpHistory)
{

}

void
GSelectBP::btbUpdate(ThreadID tid, Addr branchAddr, void * &bpHistory)
{

}

void
GSelectBP::squash(ThreadID tid, void *bpHistory)
{

}
