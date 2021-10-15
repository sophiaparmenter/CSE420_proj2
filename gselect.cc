#include "cpu/pred/gselect.hh"

GSelectBP::GselectBP(const LocalBPParams *params)
    : BPredUnit(params),
      PredictorSize(params->PredictorSize),
      PHTCtrBits(params->PHTCtrBits),
      localPredictorSets(PredictorSize / PHTCtrBits),
      localCtrs(localPredictorSets, SatCounter(PHTCtrBits)),
      indexMask(localPredictorSets - 1)
      {
      }
         
void      
GSelectBP::lookup(ThreadID tid, Addr branchAddr, void * &bpHistory)
{

//begin 2bit local implementation
    bool taken;
    
    //get the index from the branch address
    unsigned local_predictor_idx = getLocalIndex(branch_addr);
    
    //access the counter
    uint8_t counter_val = localCtrs[local_predictor_dx];
    
    //predict from counter's value, if MSB is 1
    taken = getPrediction(counter_val);


//begin bimodal implementation
    BPHistory *history = new BPHistory;
    history->globalHistoryReg	 = globalHistoryReg[tid];
    bpHistory = static_cast<void*>(history);
   //prediction functionality
   
   updateGlobalHistReg(tid, finalPrediction);
   
   //global history final prediction
    
}

unsigned
GSelectBP::getLocalIndex(Addr &branch_addr)
{
	return (branch_addr >> instShiftAmt) & indexMask;
	
}

bool
GSelectBP::getPrediction(uint8_t &count)
{
	return (count >> (PHTCtrBits -1));
}

void
GSelectBP::update(ThreadID tid, Addr branchAddr, bool taken, void *bpHistory, bool squashed, const StaticInstPtr & inst, Addr corrTarget)
{

//2 bit select implementation
	assert(bp_history == NULL);
	unsigned local_predictor_idx;
	
	//no state to store and we do not update on the wrong path
	if (squashed){
	
	//from bimode implementation
		globalHistoryReg[tid] = (history->globalHistoryReg << 1) | taken;
		return;
		}
		
	local_predictor_idx = getLocalIndex(branch_addr);
	
	if (taken){
		localCtrs[local_predictor_idx]++;
		
		//from bimode implementation
		delete history;
		}
	else {
		localCtrs[local_predictor_idx]--;
		}
		
//bimode implementation
	assert(bpHistory);
	BPHistory *history = static_cast<BPHistory*>(bpHistory);
		
	i
}

void
GSelectBP::uncondBranch(ThreadID tid, Addr pc, void * &bpHistory)
{
	BPHistory *history = newBPHistory;
	history->globalHistoryReg = globalHistoryReg[tid];
	bpHistory = static_cast<void*>(history);
	updateGlobalHistReg(tid, true);
}

void 
GSelectBP::updateGlobalHistReg(Thread tid, bool taken)
{
	globalHistoryReg[tid] = taken ? (globalHistoryReg[tid] << 1) | 1 :
					(globalHistoryReg[tid] << 1);
	globalHistoryReg[tid] &= historyRegisterMask;
	
}
void
GSelectBP::btbUpdate(ThreadID tid, Addr branchAddr, void * &bpHistory)
{

	globalHistoryReg[tid] &= (historyRegisterMask & ~1ULL);
}

void
GSelectBP::squash(ThreadID tid, void *bpHistory)
{

	BPHistory *history = static_cast<BPHistory*>(bpHistory);
	globalHistoryReg[tid] = history->globalHistoryReg;
	delete history;

}

GSelectBP*
GSelectBPParams::create()
{
    return new GSelectBP(this);
}
